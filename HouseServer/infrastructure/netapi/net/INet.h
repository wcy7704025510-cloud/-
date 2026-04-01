#ifndef INET_H
#define INET_H

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

protected:
    // 接收数据的回调函数交由中介者处理
    INetMediator* m_pMediator;
};

#endif // INET_H
