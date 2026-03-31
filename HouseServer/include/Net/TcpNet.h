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
#include<iostream>
#include<vector>
#include<list>
#include<map>

#include "./Net/Thread_pool.h"
#include "./Net/INet.h"
#include "./Mediator/INetMediator.h"

#define MAX_EVENTS 4096
using namespace std;

class TcpNet;

//数据缓存
struct DataBuffer
{
    DataBuffer(TcpNet* _pNet, int _sock, char* _buf, int _nlen)
        :pNet(_pNet), sockfd(_sock), buf(_buf), nlen(_nlen){}

    TcpNet* pNet;
    int sockfd;
    char* buf;
    int nlen;
};

template<class K, class V>
struct MyMap
{
public:
    MyMap(){
        pthread_mutex_init(&m_lock, NULL);
    }

    bool find(K k, V& v)
    {
        pthread_mutex_lock(&m_lock);
        if(m_map.count(k) == 0){
            pthread_mutex_unlock(&m_lock);
            return false;
        }
        v = m_map[k];
        pthread_mutex_unlock(&m_lock);
        return true;
    }
    void insert(K k, V v)
    {
        pthread_mutex_lock(&m_lock);
        m_map[k] = v;
        pthread_mutex_unlock(&m_lock);
    }
    void erase(K k)
    {
        pthread_mutex_lock(&m_lock);
        m_map.erase(k);
        pthread_mutex_unlock(&m_lock);
    }
    bool IsExist(K k)
    {
        bool flag = false;
        pthread_mutex_lock(&m_lock);
        if(m_map.count(k) > 0)
            flag = true;
        pthread_mutex_unlock(&m_lock);
        return flag;
    }
private:
    pthread_mutex_t m_lock;
    map<K, V> m_map;
};

//事件结构
struct myevent_s {
    int fd; //cfd listenfd
    int epoll_fd; //epoll_create 句柄
    int events; //EPOLLIN EPLLOUT
    int status;/* status:1表示在监听事件中，0表示不在 */
    TcpNet* pNet;

    myevent_s(TcpNet* _pNet)
    {
        this->pNet = _pNet;
    }
    void eventset(int fd, int efd/*epoll_create返回的句柄*/)
    {
        this->fd = fd;
        this->events = 0;
        this->status = 0;
        epoll_fd = efd;
    }
    //上监听树
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
    //下监听树
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

class TcpNet : public INet
{
public:
    TcpNet(INetMediator* pMediator);
    virtual ~TcpNet();

    //初始化网络
    virtual bool InitNet(int port) override;
    //关闭网络
    virtual void UninitNet() override;
    //发送数据
    virtual int SendData(int fd, char* szbuf, int nlen) override;

    //epoll事件循环
    void EventLoop();

    //socket 设置
    static void setNonBlockFd(int fd);
    static void setRecvBufSize(int fd);
    static void setSendBufSize(int fd);
    static void setNoDelay(int fd);

private:
    //初始线程池
    bool InitThreadPool();
    /*接收数据和处理在两个线程完成 避免相互影响*/
    //线程函数 处理数据包
    static void *Buffer_Deal(void *arg);
    //线程函数 接收数据
    static void *recv_task(void *arg);

    //epoll事件处理
    void accept_event();
    void recv_event(myevent_s *ev);
    void epollout_event(myevent_s *ev);

    //监听套接字对应的是事件
    myevent_s * m_listenEv;
    //监听套接字
    int m_listenfd;
    // epoll_create 句柄
    int m_epoll_fd;

    //每一个套接字 对应一个事件结构
    MyMap<int, myevent_s*> m_mapSockfdToEvent;
    /* 事件循环使用 */
    struct epoll_event events[MAX_EVENTS+1];
    //线程池相关
    thread_pool *m_threadpool;
};

#endif // TCPNET_H
