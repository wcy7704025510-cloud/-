#include "INetMediator.h"          // 网络中介者头文件
#include "INet.h"                // 底层网络接口头文件
#include <string.h>             // C字符串处理
#include<QDebug>               // Qt调试输出

// INetMediator构造函数
// 初始化成员变量
INetMediator::INetMediator()
    : m_pNet(nullptr)
    , m_port(0)
{
    // 清零IP地址缓冲区
    memset(m_szBufIP, 0, sizeof(m_szBufIP));
}

// INetMediator析构函数
// 释放底层网络对象
INetMediator::~INetMediator()
{
    if(m_pNet) {
        delete m_pNet;     // 释放底层网络对象
        m_pNet = nullptr;
    }
}

// SetNetEngine - 设置底层网络引擎(依赖注入)
// 参数: pNet - INet派生类实例
// 功能: 将具体网络实现注入中介者
// 设计: 采用依赖注入，支持运行时切换网络协议
void INetMediator::SetNetEngine(INet *pNet)
{
    m_pNet = pNet;
}

// setIpAndPort - 设置目标地址
void INetMediator::setIpAndPort(const char *szBufIP, unsigned short port)
{
    // 使用安全字符串复制函数
    strcpy_s(m_szBufIP, sizeof(m_szBufIP), szBufIP);
    m_port = port;
}

// OpenNet - 打开网络连接
bool INetMediator::OpenNet(const char *szBufIP, unsigned short port)
{
    // 保存连接参数
    setIpAndPort(szBufIP, port);

    // 调用底层网络初始化
    if(m_pNet) {
        return m_pNet->InitNet(szBufIP, port);
    }
    return false;
}

// CloseNet - 关闭网络连接
void INetMediator::CloseNet()
{
    if(m_pNet) {
        m_pNet->UnInitNet();  // 调用底层网络反初始化
    }
}

// SendData - 发送数据
// 透传给底层网络发送
bool INetMediator::SendData(unsigned int lSendIP, char *buf, int nlen)
{
    if(m_pNet) {
        // 输出调试信息(显示包类型，取前4字节作为协议类型)
        qDebug()<<"INetMediator::SendData"<<*(int*)buf;
        // 透传给底层网络
        return m_pNet->SendData(lSendIP, buf, nlen);
    }
    return false;
}

// DealData - 处理网络数据
// 将底层收到的数据向上发射信号
void INetMediator::DealData(unsigned int lSendIP, char *buf, int nlen)
{
    // 发射数据就绪信号，携带socket标识和数据
    Q_EMIT SIG_ReadyData(lSendIP, buf, nlen);
}

// disConnect - 主动断开连接
void INetMediator::disConnect()
{
    Q_EMIT SIG_disConnect();
}

// IsConnected - 查询连接状态
bool INetMediator::IsConnected()
{
    // 查询底层网络引擎的连接状态
    if (m_pNet) {
        return m_pNet->IsConnected();
    }
    return false;
}
