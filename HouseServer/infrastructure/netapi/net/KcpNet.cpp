#include "KcpNet.h"
#include "INetMediator.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

// 获取Linux系统毫秒级时间戳，KCP协议需要时间戳驱动重传计时器
// 返回自1970年1月1日以来的总毫秒数
inline unsigned int GetTickCountLinux() {
    struct timeval tv;
    // 获取秒数+微秒数
    gettimeofday(&tv, NULL);
    // 转换为毫秒：秒*1000 + 微秒/1000
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// KcpWorker 构造函数
// 初始化IO线程所有成员变量
KcpWorker::KcpWorker(KcpNet* p)
    : epollfd(-1)        // epoll实例描述符，-1表示无效、未创建
    , sockfd(-1)         // UDP socket描述符，-1表示无效、未创建
    , vfdSeed(0)         // 虚拟fd自增种子，从0开始分配唯一ID
    , isStop(false)      // 线程停止标志，false=正常运行
    , threadHandle(nullptr) // 线程句柄，nullptr=未启动
    , parent(p)          // 指向所属KcpNet实例，用于跨模块调用
{
}

// 初始化网络核心对象
KcpNet::KcpNet(INetMediator* pMediator) : m_isStop(false), m_threadpool(nullptr)
{
    // 保存业务层中介者指针，用于数据上报
    m_pMediator = pMediator;
    // 默认启动2个IO线程
    m_ioThreadNum = 2;
    // 初始化全局路由表读写锁
    pthread_rwlock_init(&m_routeLock, NULL);
}

// 释放所有资源
KcpNet::~KcpNet()
{
    // 关闭网络
    UninitNet();
    // 销毁路由表读写锁
    pthread_rwlock_destroy(&m_routeLock);
}

// 初始化业务处理线程池
// 参数：最大100线程，最小10线程，队列50000
bool KcpNet::InitThreadPool()
{
    m_threadpool = new thread_pool;
    // 创建线程池：最大线程、最小线程、任务队列上限
    if (!m_threadpool->Pool_create(100, 10, 50000)) {
        return false;
    }
    return true;
}

// 设置fd为非阻塞模式
// epoll必须搭配非阻塞socket使用
void KcpNet::setNonBlockFd(int fd)
{
    // 获取原有flag
    int flags = fcntl(fd, F_GETFL, 0);
    // 添加非阻塞标志
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// KCP底层发送回调函数
// KCP内部需要发送数据时自动调用
int KcpNet::udpOutput(const char *buf, int len, ikcpcb *kcp, void *user)
{
    // user为创建KCP时传入的会话对象指针
    KcpWorker::KcpSession* session = (KcpWorker::KcpSession*)user;
    // 通过UDP发送到客户端地址
    sendto(session->worker->sockfd,     //服务端发送的socket（监听的socket）
           buf,                         //发送的数据
           len,                         //缓冲区的长度
           0,                           //默认发送方式
           (struct sockaddr*)&session->client_addr, //客户端的地址
           sizeof(session->client_addr) //客户端地址的大小
           );
    return 0;
}

// 初始化整个KCP网络服务
// 创建socket、绑定、epoll、启动IO线程
bool KcpNet::InitNet(int port)
{
    // 先初始化业务线程池（创建线程池）
    InitThreadPool();

    // 根据配置创建多个IO线程（每个线程分别使用epoll来进行监听）

    for (int i = 0; i < m_ioThreadNum; ++i) {
        KcpWorker* worker = new KcpWorker(this);
        // 每个IO线程虚拟fd起始值错开，避免冲突
        worker->vfdSeed = 10000 + (i * 10000);

        // 创建UDP socket（IPV4,UDP）
        worker->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        // 开启端口复用，多线程共享同一端口

        /*积累：
            SOL_SOCKET	通用 Socket 层	端口复用、缓冲区、广播
            IPPROTO_TCP	TCP 层	TCP_NODELAY 等
            IPPROTO_IP	IP 层	TTL、IP_OPTIONS 等
        */

        int opt = 1;        //表示开启端口复用
        setsockopt(worker->sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

        // 设置5M接收缓冲区，提高吞吐
        int bufSize = 5 * 1024 * 1024;
        setsockopt(worker->sockfd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));

        // 设置非阻塞
        setNonBlockFd(worker->sockfd);

        // 绑定地址端口
        /*
            htons(port)    // 端口：主机短整型 → 网络字节序
            htonl(ip)      // IP  ：主机长整型 → 网络字节序
            ntohs(port)    // 端口：网络字节序 → 主机短整型
            ntohl(ip)      // IP  ：网络字节序 → 主机长整型
         */
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;              //IPV4
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        bind(worker->sockfd, (struct sockaddr*)&addr, sizeof(addr));

        // 创建epoll实例
        worker->epollfd = epoll_create(256);    //256表示建议监听的socket数量（实际可以更多）
        struct epoll_event ev;
        // 监听可读事件+边沿触发
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = worker->sockfd;
        epoll_ctl(worker->epollfd, EPOLL_CTL_ADD, worker->sockfd, &ev);

        m_workers.push_back(worker);

        // 创建并分离IO线程
        worker->threadHandle = new std::thread(&KcpWorker::EventLoop, worker);
        worker->threadHandle->detach();
    }

    return true;
}

// 关闭网络，释放所有资源
// 停止线程、关闭fd、释放会话
void KcpNet::UninitNet()
{
    m_isStop = true;
    // 遍历所有IO线程
    for (auto worker : m_workers) {
        worker->isStop = true;
        close(worker->epollfd);     //关闭线程
        close(worker->sockfd);      //关闭套接字

        // 释放所有KCP会话
        for (auto it = worker->mapVfdToSession.begin(); it != worker->mapVfdToSession.end(); ++it) {
            ikcp_release(it->second->kcp);
            delete it->second;
        }
        delete worker->threadHandle;
        delete worker;
    }
    m_workers.clear();

    // 释放线程池
    if (m_threadpool) {
        delete m_threadpool;
        m_threadpool = nullptr;
    }
}

// 对外发送数据接口
// 根据虚拟fd找到对应会话发送
int KcpNet::SendData(int sockfd, char* szbuf, int nlen)
{
    // 加读锁，保护全局路由表
    pthread_rwlock_rdlock(&m_routeLock);
    if (m_globalVfdToWorker.count(sockfd) > 0) {
        KcpWorker* worker = m_globalVfdToWorker[sockfd];
        // 调用KCP发送数据
        ikcp_send(worker->mapVfdToSession[sockfd]->kcp, szbuf, nlen);
    }
    pthread_rwlock_unlock(&m_routeLock);
    return 0;
}

// 线程池业务处理函数
// 处理完整KCP数据包，上报给业务层
void* KcpNet::AppBufferDeal(void* arg)
{
    KcpAppBuffer* buffer = (KcpAppBuffer*)arg;
    if (!buffer) return NULL;

    // 通过桥接模式将业务数据交付给中介者
    if(buffer->pNet && buffer->pNet->m_pMediator) {
        buffer->pNet->m_pMediator->DealData(buffer->fd, buffer->buf, buffer->nlen);
    }
    // 释放内存
    delete[] buffer->buf;
    delete buffer;
    return NULL;
}

// IO线程主循环
// 处理UDP数据、管理KCP会话
void KcpWorker::EventLoop()
{
    struct epoll_event events[1024];
    //由于会发生分片，UDP所接受最大为1380，保险起见设置2k
    char recvBuf[2048];

    // 线程主循环
    while (!isStop) {
        // epoll_wait最多等10ms，如果有返回事件个数，没有则返回0
        //1024代表一次最多返回 1024 个就绪事件
        int nReady = epoll_wait(epollfd, events, 1024, 10);
        unsigned int currentTick = GetTickCountLinux();

        // 处理就绪事件
        if (nReady > 0) {
            for (int i = 0; i < nReady; i++) {
                //如果是UDP（监听读的socket）可读事件
                if (events[i].data.fd == sockfd && (events[i].events & EPOLLIN)) {
                    // 循环读空缓冲区
                    while (true) {
                        struct sockaddr_in client_addr;
                        socklen_t addrlen = sizeof(client_addr);
                        // 接收UDP数据（有数据返回包大小，无数据返回-1）
                        int nRecv = recvfrom(sockfd, recvBuf, sizeof(recvBuf), 0,
                                             (struct sockaddr*)&client_addr, &addrlen);
                        // 无数据可读时退出
                        if (nRecv < 0) break;

                        // 生成客户端唯一地址字符串 IP:PORT
                        char ipStr[INET_ADDRSTRLEN];        //固定16位长的ip
                        //将二进制 IP 转换为 点分字符串
                        inet_ntop(AF_INET, &client_addr.sin_addr, ipStr, INET_ADDRSTRLEN);
                        std::string addrStr = std::string(ipStr) + ":" +
                                              std::to_string(ntohs(client_addr.sin_port));

                        KcpSession* session = nullptr;
                        // 新客户端，创建KCP会话
                        if (mapAddrToVfd.count(addrStr) == 0) {
                            // 分配虚拟fd
                            int vfd = vfdSeed++;
                            //虚拟文件描述符、kcp控制块、客户端地址、这个kcpWorker
                            session = new KcpSession{vfd, nullptr, client_addr, this};
                            // 创建KCP控制块，传入会话指针作为用户数据
                            //不用担心0x11223344会重复，因为kcp是通过conv+ip+port来进行识别的
                            session->kcp = ikcp_create(0x11223344, session);

                            // KCP低延迟参数配置
                            /*
                            nodelay 设为 1 表示启用快速模式，
                            nterval 设为 10 表示内部定时器间隔 10 毫秒，每10ms处理一次发包/ACK/重传
                            resend 设为 2 表示收到2次重复ACK（丢包），立刻重传丢包，不用等超时重传
                            nc 设为 1 表示关闭拥塞控制
                            */
                            ikcp_nodelay(session->kcp, 1, 10, 2, 1);

                            //设置发送窗口和接受窗口均为512
                            ikcp_wndsize(session->kcp, 512, 512);
                            //设置 KCP 最大分片包大小
                            // 一次 UDP 包最大数据长度-IP 头 (20) - UDP 头 (8)预留1380最安全
                            ikcp_setmtu(session->kcp, 1380);


                            // 设置发送回调
                            session->kcp->output = KcpNet::udpOutput;

                            // 维护本地映射表
                            mapAddrToVfd[addrStr] = vfd;
                            mapVfdToSession[vfd] = session;

                            // 更新全局路由表（加写锁）
                            pthread_rwlock_wrlock(&parent->m_routeLock);
                            parent->m_globalVfdToWorker[vfd] = this;
                            pthread_rwlock_unlock(&parent->m_routeLock);
                        } else {
                            // 已存在会话，直接获取
                            session = mapVfdToSession[mapAddrToVfd[addrStr]];
                        }

                        // 将UDP数据输入KCP状态机
                        //KCP会解析其中的 KCP 协议头，更新接收序列号，处理确认和重传请求等。
                        ikcp_input(session->kcp, recvBuf, nRecv);
                    }
                }
            }
        }

        // 驱动所有KCP会话，收取完整数据包
        for (auto it = mapVfdToSession.begin(); it != mapVfdToSession.end(); ++it) {
            ikcpcb* kcp = it->second->kcp;
            // 驱动KCP计时器，如果没有待处理的事务，直接返回，几乎不消耗 CPU
            ikcp_update(kcp, currentTick);

            // 循环收取所有完整包
            while (true) {
                // 获取下一个完整包大小
                int len = ikcp_peeksize(kcp);
                if (len <= 0) break;

                char* packBuf = new char[len];
                // 读取完整业务包
                int recvLen = ikcp_recv(kcp, packBuf, len);
                if (recvLen > 0) {
                    // 如果取到了数据投入线程池处理
                    KcpAppBuffer* appBuf = new KcpAppBuffer{parent, it->first, packBuf, recvLen};
                    parent->m_threadpool->Producer_add(KcpNet::AppBufferDeal, appBuf);
                } else {
                    //如果没有取到数据，说明包还不完整，释放分配的缓冲区。
                    delete[] packBuf;
                }
            }
        }
    }
}
