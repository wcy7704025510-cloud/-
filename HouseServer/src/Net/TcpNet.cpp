#include "./Net/TcpNet.h"
#include <netinet/tcp.h>

TcpNet::TcpNet(INetMediator* pMediator)
{
    m_pMediator = pMediator;
    m_threadpool = nullptr;
    m_listenEv = nullptr;
    m_listenfd = -1;
    m_epoll_fd = -1;
}

TcpNet::~TcpNet()
{
    UninitNet();
}

bool TcpNet::InitNet(int port)
{
    InitThreadPool();

    int flags = 1;
    int ret = 0;
    m_listenEv = new myevent_s(this);

    //creat a tcp socket
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd == -1){
        perror("create socket error");
        return false;
    }

    //set REUSERADDR
    ret = setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flags, sizeof(flags));
    if (ret == -1){
        perror("setsockopt error");
        return false;
    }

    //监听套接字m_listenfd 采用 LT 非阻塞模式
    //set NONBLOCK
    setNonBlockFd(m_listenfd);

    struct sockaddr_in local_addr;
    bzero(&local_addr, sizeof(sockaddr_in));
    //set address
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    //bind addr
    ret = bind(m_listenfd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
    if(ret == -1) {
        perror("bind error");
        close(m_listenfd);
        return false;
    }

    if (listen(m_listenfd, 128) == -1){
        perror("listen error");
        close(m_listenfd);
        return false;
    }

    //create epoll
    m_epoll_fd = epoll_create(MAX_EVENTS);

    m_listenEv->eventset(m_listenfd, m_epoll_fd);
    //将监听套接字 添加到epoll中 , 模式LT 非阻塞
    m_listenEv->eventadd(EPOLLIN);

    return true;
}

void TcpNet::UninitNet()
{
    if (m_listenEv) {
        delete m_listenEv;
        m_listenEv = nullptr;
    }
    if (m_listenfd != -1) {
        close(m_listenfd);
        m_listenfd = -1;
    }
    if (m_epoll_fd != -1) {
        close(m_epoll_fd);
        m_epoll_fd = -1;
    }
    if (m_threadpool) {
        // Here we ideally destroy the thread pool
        // delete m_threadpool;
        m_threadpool = nullptr;
    }
}

bool TcpNet::InitThreadPool()
{
    m_threadpool = new thread_pool;

    //创建拥有10个线程的线程池 最大线程数200 环形队列最大值50000
    if((m_threadpool->Pool_create(200, 10, 50000)) == false)
    {
        perror("Create Thread_Pool Failed:");
        exit(-1);
    }

    return true;
}

void TcpNet::EventLoop()
{
    printf("EventLoop:server running\n");
    int i = 0;
    while (1) {
        /* 等待事件发生 */
        int nfd = epoll_wait(m_epoll_fd, events, MAX_EVENTS+1, 1000);
        if (nfd < 0) {
            printf("epoll_wait error, exit\n");
            continue;
        }
        for (i = 0; i < nfd; i++) {
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            int fd = ev->fd;
            if ((events[i].events & EPOLLIN)) {
                if(fd == m_listenfd)
                    accept_event();
                else
                    recv_event(ev);
            }
            if ((events[i].events & EPOLLOUT)) {
                epollout_event(ev);
            }
        }
    }
}

void TcpNet::accept_event()
{
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    int clientfd;
    if ((clientfd = accept(m_listenfd, (struct sockaddr *)&caddr, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR) {
            /* 暂时不做出错处理 */
        }
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }

    //设置接收缓冲区大小
    setRecvBufSize(clientfd);
    //设置发送缓冲区大小
    setRecvBufSize(clientfd);
    //设置 无延迟
    setNoDelay(clientfd);

    myevent_s * clientEv = new myevent_s(this);
    clientEv->eventset(clientfd, m_epoll_fd);
    // 使用EPOLLONESHOT epoll监听不会重复检测 避免多线程并发 使得同一个套接字接收是排队的
    clientEv->eventadd(EPOLLIN | EPOLLONESHOT);

    m_mapSockfdToEvent.insert(clientfd, clientEv);

    printf("new connect [%s:%d][time:%ld] \n",
           inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), time(NULL));
    return;
}

void TcpNet::recv_event(myevent_s *ev)
{
    //接收事件采用线程池 多线程处理, 由于使用EPOLLONESHOT , 避免同一套机字的线程并发问题
    m_threadpool->Producer_add(recv_task, (void*) ev);
}

void* TcpNet::recv_task(void* arg)
{
    myevent_s * ev = (myevent_s*)arg;
    //利用全局指针 方便操作
    TcpNet * pthis = ev->pNet;

    //接收和处理分离
    int nRelReadNum = 0;
    int nPackSize = 0;
    char *pSzBuf = NULL;
    do
    {
        nRelReadNum = read(ev->fd, &nPackSize, sizeof(nPackSize));
        if(nRelReadNum <= 0)
            break;

        pSzBuf = new char[nPackSize];
        int nOffSet = 0;
        nRelReadNum = 0;
        //接收包的数据
        while(nPackSize)
        {
            nRelReadNum = recv(ev->fd, pSzBuf+nOffSet, nPackSize, 0);
            if(nRelReadNum <= 0)
                break;

            nOffSet += nRelReadNum;
            nPackSize -= nRelReadNum;
        }
        //接收和处理分离 跑线程池里其他线程处理 , 避免处理影响接收
        DataBuffer * buffer = new DataBuffer(ev->pNet, ev->fd, pSzBuf, nOffSet);
        pthis->m_threadpool->Producer_add(Buffer_Deal, (void*) buffer);

        // 这次接收完 要重新注册事件  此时 EPOLL MODE -> EPOLLIN|EPOLLONESHOT 没有修改, 使用重复值
        ev->eventadd(ev->events);

        return 0;

    }while(0);

    ev->eventdel();
    close(ev->fd);
    //回收event结构
    pthis->m_mapSockfdToEvent.erase(ev->fd);
    delete ev;
    return NULL;
}

void * TcpNet::Buffer_Deal(void * arg)
{
    DataBuffer * buffer = (DataBuffer *)arg;
    if(!buffer) return NULL;

    //通过中介者处理数据
    if (buffer->pNet->m_pMediator) {
        buffer->pNet->m_pMediator->DealData(buffer->sockfd, buffer->buf, buffer->nlen);
    }

    if(buffer->buf != NULL)
    {
        delete [] buffer->buf;
        buffer->buf = NULL;
    }
    delete buffer;
    return 0;
}

void TcpNet::epollout_event(myevent_s *ev)
{
    // epoll LT模式 阻塞模式 发送阻塞 , 不用监听EPOLLOUT事件
}

int TcpNet::SendData(int fd, char *szbuf, int nlen)
{
    int nPackSize = nlen + 4;
    vector<char> vecbuf(nPackSize, 0);

    char* buf = &* vecbuf.begin();
    char* tmp = buf;
    *(int*)tmp = nlen;//按四个字节int写入
    tmp += sizeof(int);
    memcpy(tmp, szbuf, nlen);

    int res = send(fd, (const char *)buf, nPackSize, 0);

    return res;
}

void TcpNet::setNonBlockFd(int fd)
{
    int flags = 0;
    flags = fcntl(fd, F_GETFL, 0);
    int ret = fcntl(fd, F_SETFL, flags|O_NONBLOCK);
    if(ret == -1)
        perror("setNonBlockFd fail:");
}

void TcpNet::setRecvBufSize(int fd)
{
    int nRecvBuf = 256*1024;//设置为 256 K
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
}

void TcpNet::setSendBufSize(int fd)
{
    int nSendBuf=128*1024;//设置为 128 K
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
}

void TcpNet::setNoDelay(int fd)
{
    int value = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(int));
}
