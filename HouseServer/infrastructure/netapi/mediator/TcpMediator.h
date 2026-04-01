#ifndef TCPMEDIATOR_H
#define TCPMEDIATOR_H

#include "INetMediator.h"

class CKernel; // 前置声明核心类
class TcpNet; // 前置声明

class TcpMediator : public INetMediator
{
public:
    TcpMediator(CKernel* pKernel);
    virtual ~TcpMediator();

    // 开启核心服务
    virtual int Open(int port) override;
    // 关闭核心服务
    virtual void Close() override;
    // 处理网络接收
    virtual void DealData(int sockfd, char* szbuf, int nlen) override;
    // 发送数据
    virtual void SendData(int sockfd, char* szbuf, int nlen) override;

    // 事件循环 (供主线程阻塞调用)
    virtual void EventLoop() override;

private:
    CKernel* m_pKernel; // 指向核心类的指针
};

#endif // TCPMEDIATOR_H
