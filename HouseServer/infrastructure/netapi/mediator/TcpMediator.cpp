#include "TcpMediator.h"
#include "TcpNet.h"
#include "CKernel.h"
#include "err_str.h"
#include <stdio.h>

using namespace std;

TcpMediator::TcpMediator(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_pNet = new TcpNet(this);
}

TcpMediator::~TcpMediator()
{
    if(m_pNet) {
        delete m_pNet;
        m_pNet = nullptr;
    }
}

int TcpMediator::Open(int port)
{
    //初始网络
    bool res = m_pNet->InitNet(port);
    if (!res) {
        err_str("net init fail:", -1);
        return FALSE;
    }

    return TRUE;
}

void TcpMediator::Close()
{
    m_pNet->UninitNet();
}

void TcpMediator::DealData(int sockfd, char *szbuf, int nlen)
{
    // 收到网络数据后，直接抛给核心类处理
    if (m_pKernel) {   
        m_pKernel->ReadyData(sockfd, szbuf, nlen);
    }
}

void TcpMediator::EventLoop()
{
    printf("event loop\n");
    if (m_pNet) {
        TcpNet* pTcpNet = dynamic_cast<TcpNet*>(m_pNet);
        if (pTcpNet) {
            pTcpNet->EventLoop();
        }
    }
}

void TcpMediator::SendData(int sockfd, char *szbuf, int nlen)
{
    if (m_pNet) {
        m_pNet->SendData(sockfd, szbuf, nlen);
    }
}
