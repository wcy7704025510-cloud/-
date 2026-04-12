#pragma once

// 项目内部头文件
#include"INet.h"              // 网络接口抽象基类

// TcpClient - TCP客户端网络实现类
// 职责：实现TCP协议的控制信令传输(登录、注册、房间操作等)
// 继承自INet抽象基类，实现具体TCP通信
// 特点：
//   1. 面向连接、可靠传输 - 适合控制信令
//   2. 单客户端连接 - 控制通道共用一个TCP连接
//   3. 独立接收线程 - 非阻塞接收网络数据
// 数据包格式：[包长度(4B)][包类型(4B)][包内容...]
class TcpClient : public INet
{
public:
    // 构造函数
    // 参数: pMediator - 中介者指针，用于回调接收到的数据
    TcpClient(INetMediator* pMediator);

    // 析构函数
    // 注意：使用虚析构确保正确清理
    ~TcpClient(void);

    // InitNet - 初始化TCP网络连接
    // 参数: szBufIP - 服务器IP, port - 服务器端口
    // 流程: 加载Winsock -> 创建TCP socket -> 连接服务器 -> 创建接收线程
    bool InitNet(const char* szBufIP, unsigned short port);

    // UnInitNet - 关闭TCP网络连接
    // 流程: 停止线程 -> 关闭socket -> 卸载Winsock
    void UnInitNet();

    // SendData - 发送数据
    // 参数: lSendIP - socket标识(未使用), buf - 数据缓冲区, nlen - 数据长度
    // 功能: 在数据前添加4字节长度头，封装后发送
    bool SendData(unsigned int lSendIP, char* buf, int nlen);

    // IsConnected - 查询连接状态
    // 返回: true=已连接, false=未连接
    bool IsConnected();

protected:
    // RecvThread - 接收线程入口(静态函数)
    // 参数: lpvoid - TcpClient对象指针
    // 返回: 线程退出码
    // 注意: 使用_stdcall调用约定
    static unsigned int __stdcall RecvThread(void* lpvoid);

    // RecvData - 接收数据(在独立线程中运行)
    // 流程: 循环接收 -> 解析长度头 -> 接收完整包 -> 回调上层
    virtual void RecvData();

private:
    SOCKET m_sock;              // TCP socket描述符
    HANDLE m_hThreadHandle;     // 接收线程句柄
    bool m_isStop;              // 线程停止标志
    bool m_isConnected;         // 连接状态标志
};
