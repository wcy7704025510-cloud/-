#include "INetMediator.h"
#include "INet.h"
#include <string.h>
#include<QDebug>
INetMediator::INetMediator() : m_pNet(nullptr), m_port(0) {
    memset(m_szBufIP, 0, sizeof(m_szBufIP));
}

INetMediator::~INetMediator() {
    if(m_pNet) {
        delete m_pNet;
        m_pNet = nullptr;
    }
}

// 依赖注入：大老板把干活的引擎塞进来
void INetMediator::SetNetEngine(INet *pNet) {
    m_pNet = pNet;
}

void INetMediator::setIpAndPort(const char *szBufIP, unsigned short port) {
    strcpy_s(m_szBufIP, sizeof(m_szBufIP), szBufIP);
    m_port = port;
}

// 盲调底层的初始化
bool INetMediator::OpenNet(const char *szBufIP, unsigned short port) {
    setIpAndPort(szBufIP, port);
    if(m_pNet) {
        return m_pNet->InitNet(szBufIP, port);
    }
    return false;
}

void INetMediator::CloseNet() {
    if(m_pNet) {
        m_pNet->UnInitNet();
    }
}

// 向下发数据，透传给底层
bool INetMediator::SendData(unsigned int lSendIP, char *buf, int nlen) {
    if(m_pNet) {
        qDebug()<<"INetMediator::SendData"<<*(int*)buf;
        // TCP 断线重连逻辑也可以移到这里，或者由底层引擎自行处理
        return m_pNet->SendData(lSendIP, buf, nlen);
    }
    return false;
}

// 向上收数据，把底层收到的数据转成 Qt 信号发出
void INetMediator::DealData(unsigned int lSendIP, char *buf, int nlen) {
    Q_EMIT SIG_ReadyData(lSendIP, buf, nlen);
}

void INetMediator::disConnect() {
    Q_EMIT SIG_disConnect();
}

bool INetMediator::IsConnected() {
    // 根据底层 TCP 引擎的状态返回
    if (m_pNet) {
        return m_pNet->IsConnected();
    }
    return false;
}
