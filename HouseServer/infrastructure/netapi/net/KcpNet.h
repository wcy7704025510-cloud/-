#ifndef KCPNET_H
#define KCPNET_H

#include "INet.h"
#include "Thread_pool.h"
#include "ikcp.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <pthread.h>

#include "../common/MyMap.h"
#include "../common/NetBuffer.h"

class KcpNet;

// KCP IO工作线程结构体
struct KcpWorker
{
    int epollfd;                              // 当前worker的epoll实例
    int sockfd;                               // UDP socket文件描述符
    int vfdSeed;                              // 虚拟fd生成种子
    bool isStop;                              // 线程停止标志
    std::thread* threadHandle;                // IO线程句柄
    KcpNet* parent;                           // 所属KcpNet对象

    // KCP会话管理结构体
    struct KcpSession
    {
        int vfd;                              // 虚拟文件描述符
        ikcpcb* kcp;                          // KCP控制块
        struct sockaddr_in client_addr;       // 客户端地址
        KcpWorker* worker;                    // 所属IO线程

        KcpSession(int _vfd, ikcpcb* _kcp, struct sockaddr_in _addr, KcpWorker* _worker)
            : vfd(_vfd), kcp(_kcp), client_addr(_addr), worker(_worker) {}
    };

    std::map<int, KcpSession*> mapVfdToSession;   // 虚拟fd到会话的映射
    std::map<std::string, int> mapAddrToVfd;      // 客户端地址到虚拟fd的映射

    // 构造函数
    KcpWorker(KcpNet* p);

    // IO线程事件循环
    void EventLoop();
};

// KCP网络模块类
// 继承INet接口，基于UDP+KCP实现低延迟可靠传输
class KcpNet : public INet
{
public:
    KcpNet(INetMediator* pMediator);           // 构造函数
    virtual ~KcpNet();                         // 析构函数

    // 初始化网络：创建UDP套接字、启动IO线程、初始化线程池
    virtual bool InitNet(int port) override;

    // 关闭网络：释放资源、停止线程、清理会话
    virtual void UninitNet() override;

    // 发送数据：通过虚拟fd找到对应会话，经由KCP发送
    virtual int SendData(int sockfd, char* szbuf, int nlen) override;

    // 事件循环：KCP使用多线程独立循环，接口层空实现
    virtual void EventLoop() override {}

    // KCP底层发送回调：将KCP数据包通过UDP发出
    static int udpOutput(const char *buf, int len, ikcpcb *kcp, void *user);

private:
    bool InitThreadPool();                     // 初始化业务线程池
    void setNonBlockFd(int fd);                // 设置文件描述符为非阻塞

public:
    static void* AppBufferDeal(void* arg);     // 线程池业务处理函数

public:
    bool m_isStop;                             // 服务停止标志
    thread_pool* m_threadpool;                 // 业务处理线程池
    int m_ioThreadNum;                         // IO线程数量
    std::vector<KcpWorker*> m_workers;         // 保存所有 worker 指针，用于服务器关闭时遍历清理

    pthread_rwlock_t m_routeLock;              // 虚拟fd路由表读写锁（读写锁性能优于互斥锁）
    std::map<int, KcpWorker*> m_globalVfdToWorker; //通过虚拟 fd 找到对应的 worker（发送数据）

};

#endif // KCPNET_H
