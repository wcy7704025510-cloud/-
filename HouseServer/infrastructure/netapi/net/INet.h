#ifndef INET_H
#define INET_H
#include<functional>
class INetMediator;

class INet
{
public:
    INet() {}
    virtual ~INet() {}

    // 初始化网络
    virtual bool InitNet(int port) = 0;
    // 关闭网络
    virtual void UninitNet() = 0;
    // 发送数据
    virtual int SendData(int sockfd, char* szbuf, int nlen) = 0;
    // 把事件循环提上来，让桥接器可以多态盲调
    virtual void EventLoop() = 0;
public:
    std::function<void(int,char*,int)>DealData;      //向桥接模式上层传输数据
};

#endif // INET_H
