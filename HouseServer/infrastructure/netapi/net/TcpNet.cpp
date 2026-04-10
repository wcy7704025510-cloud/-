#include "TcpNet.h"
#include <netinet/tcp.h>
using namespace std;

// 初始化所有成员变量
TcpNet::TcpNet(std::function<void(int,char*,int)>a)
{
    DealData=a;         // 桥接模式的桥梁：接收数据后上报给上层业务
    m_threadpool = nullptr;          // 业务线程池，处理数据解析
    m_listenEv = nullptr;            // 监听socket对应的epoll事件对象
    m_listenfd = -1;                 // TCP监听套接字，-1表示未创建
    m_epoll_fd = -1;                 // epoll实例文件描述符，-1表示未创建
}


// 自动释放所有网络资源
TcpNet::~TcpNet()

{
    UninitNet();
}

// 初始化TCP网络服务，port表示：要监听的端口
bool TcpNet::InitNet(int port)
{
    // 1. 初始化业务线程池
    InitThreadPool();

    int flags = 1;      //表示启用端口复用
    int ret = 0;
    m_listenEv = new myevent_s(this);  // 创建监听事件对象

    // 2. 创建TCP套接字（AF_INET：IPv4，SOCK_STREAM：TCP流式协议，0：自动选择TCP协议）
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd == -1){
        perror("create socket error");
        return false;
    }

    // 3. 设置地址复用 SO_REUSEADDR：允许快速重启服务器，解决TIME_WAIT端口占用问题
    ret = setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flags, sizeof(flags));
    if (ret == -1){
        perror("setsockopt error");
        return false;
    }

    // 4. 设置监听套接字为非阻塞模式epoll
    setNonBlockFd(m_listenfd);

    // 5. 绑定IP和端口
    struct sockaddr_in local_addr;
    bzero(&local_addr, sizeof(sockaddr_in));        // 清空结构体
    local_addr.sin_family = AF_INET;                // IPv4
    local_addr.sin_port = htons(port);              // 端口（主机字节序转网络字节序）
    local_addr.sin_addr.s_addr = INADDR_ANY;        // 监听本机所有网卡

    // 执行绑定
    ret = bind(m_listenfd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
    if(ret == -1) {
        perror("bind error");
        close(m_listenfd);
        return false;
    }

    // 6. 开始监听
    // 128：内核等待连接队列的最大长度
    if (listen(m_listenfd, 128) == -1){
        perror("listen error");
        close(m_listenfd);
        return false;
    }

    // 7. 创建epoll实例4096只是建议
    m_epoll_fd = epoll_create(MAX_EVENTS);

    // 8. 把监听socket加入epoll
    m_listenEv->eventset(m_listenfd, m_epoll_fd);
    // 监听可读事件（有新连接到来），LT水平触发模式
    m_listenEv->eventadd(EPOLLIN);

    return true;
}

// 关闭socket、epoll、释放资源
void TcpNet::UninitNet()
{
    if (m_listenEv) {
        delete m_listenEv;
        m_listenEv = nullptr;
    }
    if (m_listenfd != -1) {
        close(m_listenfd);       // 关闭监听socket
        m_listenfd = -1;
    }
    if (m_epoll_fd != -1) {
        close(m_epoll_fd);       // 关闭epoll
        m_epoll_fd = -1;
    }
    if (m_threadpool) {
        //Todo:销毁线程池
        m_threadpool = nullptr;
    }
}

// 初始化线程池
// 最大200线程，最小10线程，队列50000
bool TcpNet::InitThreadPool()
{
    m_threadpool = new thread_pool;

    // 创建线程池：最大线程、最小线程、队列最大容量
    if((m_threadpool->Pool_create(200, 10, 20000)) == false)
    {
        perror("Create Thread_Pool Failed:");
        exit(-1);
    }

    return true;
}


// 负责接收所有连接与数据
void TcpNet::EventLoop()
{
    printf("EventLoop:server running\n");
    int i = 0;
    while (1) {
        // 等待事件
        // 监听的epoll，返回的事件，socket超时1000ms，防止永久阻塞
        int nfd = epoll_wait(m_epoll_fd, events, MAX_EVENTS+1, 1000);
        if (nfd < 0) {
            printf("epoll_wait error, exit\n");
            continue;
        }

        // 遍历所有就绪事件
        for (i = 0; i < nfd; i++) {
            // 从epoll事件中取出自定义event对象(在eventadd中添加的)
            struct myevent_s *ev = (struct myevent_s*)events[i].data.ptr;
            int fd = ev->fd;

            // 可读事件
            if ((events[i].events & EPOLLIN)) {
                if(fd == m_listenfd)
                    accept_event();        // 监听fd：新连接
                else
                    recv_event(ev);        // 客户端fd：有数据可读
            }

            // 可写事件Todo:将LT改为ET的时候需要实现epollout
            if ((events[i].events & EPOLLOUT)) {
                epollout_event(ev);
            }
        }
    }
}


// 接受新客户端连接
void TcpNet::accept_event()
{
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    int clientfd;

    // 接受连接，返回客户端专属通信socket
    if ((clientfd = accept(m_listenfd, (struct sockaddr *)&caddr, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR) {
            /*
                EAGAIN：非阻塞模式下，暂时不可读 / 不可写，正常情况。
                EINTR：被系统信号打断，重试即可，不是错误。

            */
            // 非关键错误，不处理
        }
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }

    // 设置客户端socket参数
    setRecvBufSize(clientfd);       // 设置接收缓冲区256K
    setSendBufSize(clientfd);       // 设置发送缓冲区256K
    setNoDelay(clientfd);           // 关闭Nagle算法，降低延迟

    // 创建客户端事件对象，加入epoll
    myevent_s * clientEv = new myevent_s(this);
    clientEv->eventset(clientfd, m_epoll_fd);

    // EPOLLONESHOT：同一个socket只会被一个线程处理，避免并发混乱
    clientEv->eventadd(EPOLLIN | EPOLLONESHOT);

    // 保存fd -> event映射
    m_mapSockfdToEvent.insert(clientfd, clientEv);

    printf("new connect [%s:%d][time:%ld] \n",
           inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), time(NULL));
    return;
}

// 客户端数据可读事件
// 交给线程池异步处理
void TcpNet::recv_event(myevent_s *ev)
{
    // 接收任务抛入线程池，不阻塞主线程
    m_threadpool->Producer_add(recv_task, (void*) ev);
}


// 线程池：真正接收TCP数据
void* TcpNet::recv_task(void* arg)
{
    myevent_s * ev = (myevent_s*)arg;
    TcpNet * pthis = ev->pNet;

    int nRelReadNum = 0;
    int nPackSize = 0;
    char *pSzBuf = NULL;

    do
    {
        // 1. 先读4字节：数据包总长度
        nRelReadNum = read(ev->fd, &nPackSize, sizeof(nPackSize));
        if(nRelReadNum <= 0)
            break;  // 连接断开/出错

        // 2. 根据长度申请缓冲区
        pSzBuf = new char[nPackSize];
        int nOffSet = 0;
        nRelReadNum = 0;

        // 3. 循环收完整个包
        while(nPackSize)
        {
            nRelReadNum = recv(ev->fd, pSzBuf+nOffSet, nPackSize, 0);
            if(nRelReadNum <= 0)
                break;

            nOffSet += nRelReadNum;
            nPackSize -= nRelReadNum;
        }

        // 4. 抛给业务线程池处理
        TcpDataBuffer * buffer = new TcpDataBuffer(ev->pNet, ev->fd, pSzBuf, nOffSet);
        pthis->m_threadpool->Producer_add(Buffer_Deal, (void*) buffer);

        // 5. 重新注册EPOLL事件（因为用了EPOLLONESHOT）
        ev->eventadd(ev->events);

        return 0;

    }while(0);

    // 出错/断开：清理资源
    ev->eventdel();
    close(ev->fd);
    pthis->m_mapSockfdToEvent.erase(ev->fd);
    delete ev;
    ev=nullptr;
    return NULL;
}



// 把完整数据包交给上层业务逻辑（因为要在线程池中加入buffer_deal处理函数所以必须静态）
void * TcpNet::Buffer_Deal(void * arg)
{
    TcpDataBuffer * buffer = (TcpDataBuffer *)arg;
    if(!buffer) return NULL;

    // 通过桥接模式把数据抛给业务层
    buffer->pNet->DealData(buffer->fd, buffer->buf, buffer->nlen);
    // 释放内存
    if(buffer->buf != NULL)
    {
        delete [] buffer->buf;
        buffer->buf = NULL;
    }
    delete buffer;

    return 0;
}


// 可写事件（本项目未使用在改为ET的时候需要更改）
// LT模式无需监听EPOLLOUT
void TcpNet::epollout_event(myevent_s *ev)
{
    // epoll LT模式 无需监听可写事件
}


// 发送数据（带粘包处理）
// 格式：4字节长度 + 数据体
int TcpNet::SendData(int fd, char *szbuf, int nlen)
{
    // 总长度 = 数据长度 + 4字节包头
    int nPackSize = nlen + 4;
    vector<char> vecbuf(nPackSize, 0);

    char* buf = vecbuf.data();
    char* tmp = buf;

    // 写入4字节长度（解决TCP粘包）
    *(int*)tmp = nlen;
    tmp += sizeof(int);

    // 拷贝真实数据
    memcpy(tmp, szbuf, nlen);

    // 发送
    int res = send(fd, (const char *)buf, nPackSize, 0);

    return res;
}


// 设置socket为非阻塞模式
void TcpNet::setNonBlockFd(int fd)
{
    int flags = 0;
    flags = fcntl(fd, F_GETFL, 0);                  // 获取原有flag
    int ret = fcntl(fd, F_SETFL, flags|O_NONBLOCK); // 添加非阻塞
    if(ret == -1)
        perror("setNonBlockFd fail:");
}


// 设置接收缓冲区 256KB
void TcpNet::setRecvBufSize(int fd)
{
    int nRecvBuf = 256*1024;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
}


// 设置发送缓冲区 128KB
void TcpNet::setSendBufSize(int fd)
{
    int nSendBuf=128*1024;
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
}

// 关闭Nagle算法，降低延迟
void TcpNet::setNoDelay(int fd)
{
    int value = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(int));
}
