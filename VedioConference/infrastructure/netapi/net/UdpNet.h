#pragma once

// 项目内部头文件
#include "INet.h"              // 网络接口抽象基类
#include "ikcp.h"             // KCP可靠UDP协议库(第三方)

// UdpNet - UDP客户端网络实现类(KCP封装)
// 职责：实现KCP可靠UDP协议的音视频数据传输
// 继承自INet抽象基类
// 特点：
//   1. UDP传输 + KCP可靠保证 - 兼顾低延迟和可靠性
//   2. 非阻塞模式 - 防止recvfrom阻塞线程
//   3. 自动分包/组包 - KCP处理大包切分和重组
//   4. 高速模式 - nodelay配置降低延迟
// 使用场景：音视频数据传输(音频、视频)
class UdpNet:public INet
{
public:
    // 构造函数/析构函数
    UdpNet(INetMediator* pMediator);
    ~UdpNet();

    // InitNet - 初始化UDP网络连接
    // 参数: szBufIP - 服务器IP, port - 服务器端口
    // 流程: 加载Winsock -> 创建UDP socket -> 初始化KCP状态机
    bool InitNet(const char* szBufIP, unsigned short port);

    // UnInitNet - 关闭UDP网络连接
    void UnInitNet();

    // SendData - 发送数据
    // 参数: lSendIP - 目标标识(未使用), buf - 数据缓冲区, nlen - 数据长度
    // 功能: 将数据交给KCP，由KCP自动分包发送
    bool SendData(unsigned int lSendIP, char* buf, int nlen);

protected:
    // RecvThread - 接收线程入口(静态函数)
    static unsigned int __stdcall RecvThread(void* lpvoid);

    // RecvData - 接收数据(KCP处理)
    // 流程: 驱动KCP心跳 -> 接收UDP碎片 -> 喂给KCP -> 从KCP取出完整包
    virtual void RecvData();

    // udpOutput - KCP底层UDP发送回调
    // 参数: buf-待发送数据, len-数据长度, kcp-KCP实例, user-用户数据
    // 功能: KCP切分后调用此函数，实际通过UDP发送到服务器
    static int udpOutput(const char *buf, int len, ikcpcb *kcp, void *user);

private:
    SOCKET m_sock;              // UDP socket描述符
    HANDLE m_hThreadHandle;     // 接收线程句柄
    bool m_isStop;              // 线程停止标志
    int m_port;                 // 服务器端口
    ikcpcb* m_kcp;              // KCP状态机控制块
    unsigned int m_serverIp;    // 服务器IP地址缓存
};
