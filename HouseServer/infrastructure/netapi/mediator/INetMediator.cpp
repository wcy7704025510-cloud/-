#include "INetMediator.h"
#include "INet.h"
#include "CKernel.h"

INetMediator::INetMediator(CKernel* pKernel) : m_pNet(nullptr), m_pKernel(pKernel)
{
}

INetMediator::~INetMediator()
{
    if(m_pNet) {
        delete m_pNet;
        m_pNet = nullptr;
    }
}

// ✅ 依赖注入点
void INetMediator::SetNetEngine(INet* pNet)
{
    m_pNet = pNet;
}

int INetMediator::Open(int port)
{
    if(m_pNet) return m_pNet->InitNet(port);
    return false;
}

void INetMediator::Close()
{
    if(m_pNet) m_pNet->UninitNet();
}

// 底层线程池收到数据后，调用这里，这里再直接抛给大管家查路由表
void INetMediator::DealData(int sockfd, char *szbuf, int nlen)
{
    if (m_pKernel) {
        m_pKernel->ReadyData(sockfd, szbuf, nlen);
    }
}

void INetMediator::SendData(int sockfd, char *szbuf, int nlen)
{
    if (m_pNet) {
        m_pNet->SendData(sockfd, szbuf, nlen);
    }
}

void INetMediator::EventLoop()
{
    // 多态盲调！不用管底层是 TCP 还是 KCP，跑就完事了！
    if (m_pNet) {
        m_pNet->EventLoop();
    }
}
