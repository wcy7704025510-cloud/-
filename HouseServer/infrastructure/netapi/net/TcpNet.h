#ifndef TCPNET_H
#define TCPNET_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>

#include "Thread_pool.h"
#include "INet.h"
#include "INetMediator.h"
#include "../common/MyMap.h"
#include "../common/NetBuffer.h"

#define MAX_EVENTS 4096
using namespace std;

class TcpNet;

// epoll事件结构体：管理socket描述符与监听事件
struct myevent_s
{
    int fd; 		//listenfd监听事件/客户端的socket
    int epoll_fd; 	//epoll_create 句柄
    int events; 	//EPOLLIN EPLLOUT
    int status;		//status:1表示在监听事件中，0表示不在
    TcpNet* pNet;   // 所属TcpNet对象指针


    myevent_s(TcpNet* _pNet)
    {
        this->pNet = _pNet;
    }

    // 初始化事件信息
    void eventset(int fd, int efd/*epoll_create返回的句柄*/)
    {
        this->fd = fd;
        this->events = 0;
        this->status = 0;
        epoll_fd = efd;
    }
    // 添加/修改epoll监听事件
    void eventadd(int events)
    {
        struct epoll_event epv = {0, {0}};
        int op;
        epv.data.ptr = this;
        epv.events = this->events = events;
        if (this->status == 1) {
            op = EPOLL_CTL_MOD;
        }
        else {
            op = EPOLL_CTL_ADD;
            this->status = 1;
        }
        int ret = epoll_ctl(epoll_fd, op, this->fd, &epv);
        if (ret < 0)
            printf("event add failed [fd=%d], events[%d]\n", this->fd, events);

        return;
    }
    // 从epoll中移除事件
    void eventdel()
    {
        struct epoll_event epv = {0, {0}};
        if (this->status != 1)
            return;
        epv.data.ptr = this;
        this->status = 0;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->fd, &epv);
        return;
    }
};

// TCP网络核心类
// 继承INet接口，基于epoll实现高并发TCP服务器
class TcpNet : public INet
{
public:
    TcpNet(INetMediator* pMediator);  // 构造函数
    virtual ~TcpNet();                // 析构函数

    // 初始化网络：创建socket、绑定端口、启动epoll、初始化线程池
    virtual bool InitNet(int port) override;

    // 关闭网络：释放资源、关闭套接字、销毁线程池
    virtual void UninitNet() override;

    // 发送数据：通过指定客户端fd发送数据
    virtual int SendData(int fd, char* szbuf, int nlen) override;

    // epoll事件循环：监听并处理所有网络IO事件
    void EventLoop();

    // 设置文件描述符为非阻塞模式
    static void setNonBlockFd(int fd);
    // 设置socket接收缓冲区大小
    static void setRecvBufSize(int fd);
    // 设置socket发送缓冲区大小
    static void setSendBufSize(int fd);
    // 禁用Nagle算法，降低传输延迟
    static void setNoDelay(int fd);

private:
    // 初始化业务处理线程池
    bool InitThreadPool();

    // 线程池业务处理函数：处理完整数据包
    static void *Buffer_Deal(void *arg);
    // 线程池接收任务函数：读取客户端数据
    static void *recv_task(void *arg);

    // 处理客户端连接请求
    void accept_event();
    // 处理读事件：接收客户端数据
    void recv_event(myevent_s *ev);
    // 处理写事件：发送数据
    void epollout_event(myevent_s *ev);

private:
    myevent_s * m_listenEv;           // 监听socket事件对象
    int m_listenfd;                   // 监听套接字
    int m_epoll_fd;                   // epoll实例描述符

    MyMap<int, myevent_s*> m_mapSockfdToEvent;  // socket fd -> 事件映射表

    struct epoll_event events[MAX_EVENTS+1];    // epoll就绪事件集合

    thread_pool *m_threadpool;         // 业务逻辑处理线程池
};

#endif // TCPNET_H
