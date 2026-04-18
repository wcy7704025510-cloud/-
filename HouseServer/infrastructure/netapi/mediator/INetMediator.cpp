#include "INetMediator.h"
#include "INet.h"
#include "CKernel.h"

INetMediator::INetMediator(std::function<void(int,char*,int)>cb) : m_pNet(nullptr)
{
   readyData=cb;
}

INetMediator::~INetMediator()
{
    if(m_pNet) {
        delete m_pNet;
        m_pNet = nullptr;
    }
}

//装载底层网络模型
void INetMediator::SetNetEngine(INet* pNet)
{
    m_pNet = pNet;
}
// 开启网络核心服务
int INetMediator::Open(int port)
{
    if(m_pNet) return m_pNet->InitNet(port);
    return false;
}
// 关闭网络核心服务
void INetMediator::Close()
{
    if(m_pNet) m_pNet->UninitNet();
}

// 底层线程池收到数据后，调用这里，这里再直接抛给中介者查路由表
void INetMediator::DealData(int sockfd, char *szbuf, int nlen)
{
    readyData(sockfd,szbuf,nlen);
}
//调用底层的协议进行发送消息
void INetMediator::SendData(int sockfd, char *szbuf, int nlen)
{
    if (m_pNet) {
        m_pNet->SendData(sockfd, szbuf, nlen);
    }
}
//不管底层是TCP还是KCP，直接调用底层事件监听循环
void INetMediator::EventLoop()
{
    if (m_pNet) {
        m_pNet->EventLoop();
    }
}
