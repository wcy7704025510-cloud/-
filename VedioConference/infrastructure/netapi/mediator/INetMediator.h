#pragma once

#include <QObject>

#define _DEF_TCP_PORT  (8000)
#define _DEF_UDP_PORT  (1234)
#define _DEF_SERVER_IP ("192.168.1.168")

class INet; // 前向声明底层的网络接口

class INetMediator : public QObject
{
    Q_OBJECT
signals:
    void SIG_ReadyData( unsigned int lSendIP , char* buf , int nlen );
    void SIG_disConnect();

public:
    INetMediator();
    virtual ~INetMediator();

    //配置底层的协议
    void SetNetEngine(INet* pNet);

    // 下面的函数变为具体的实现，负责向下透传数据
    virtual bool OpenNet(const char* szBufIP = "0.0.0.0", unsigned short port = _DEF_TCP_PORT);
    virtual void CloseNet();
    virtual bool SendData(unsigned int lSendIP , char* buf , int nlen);
    virtual void DealData(unsigned int lSendIP , char* buf , int nlen);

    void setIpAndPort(const char* szBufIP, unsigned short port);
    virtual bool IsConnected();
    virtual void disConnect();

protected:
    INet* m_pNet; // 指向纯 C++ 的底层网络对象
    char  m_szBufIP[18];
    unsigned short m_port;
};
