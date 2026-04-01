#ifndef INETMEDIATOR_H
#define INETMEDIATOR_H

class INet;

class INetMediator
{
public:
    INetMediator() {}
    virtual ~INetMediator() {}

    // 开启网络核心服务
    virtual int Open(int port) = 0;
    // 关闭网络核心服务
    virtual void Close() = 0;
    // 处理网络接收的数据
    virtual void DealData(int sockfd, char* szbuf, int nlen) = 0;
    // 发送数据
    virtual void SendData(int sockfd, char* szbuf, int nlen) = 0;

    //打开事件循环
    virtual void EventLoop()=0;
protected:
    // 网络层对象
    INet* m_pNet;
};

#endif // INETMEDIATOR_H
