#pragma once
#include<winsock2.h>
#include<ws2tcpip.h>       // TCP/IP协议扩展头文件
#include<process.h>
#include<string>         // std::string字符串类
#include<set>          // std::set集合容器

// 前向声明
class INetMediator;     // 网络中介者类

// INet - 网络接口抽象基类
// 职责：定义网络通信的统一接口
// 设计模式：模板方法+工厂模式，子类实现具体TCP/UDP/KCP
// 使用方式：通过INetMediator持有INet指针，运行时多态
// 派生类：TcpClient(TCP), UdpNet(UDP), KcpNet(KCP)
class INet
{
public:
    // 构造函数
    INet(){}

    // 虚析构函数
    // 重要：父类指针指向子类时，delete能正确调用子类析构
    virtual ~INet(){}

    // InitNet - 初始化网络
    // 参数:
    //   szBufIP - 服务器IP地址
    //   port - 端口号
    // 返回: true=成功, false=失败
    // 功能: 创建socket、绑定/连接、启动接收线程
    virtual bool InitNet(const char* szBufIP , unsigned short port) = 0 ;

    // UnInitNet - 关闭网络
    // 功能: 关闭socket、停止线程、释放资源
    virtual void UnInitNet() = 0 ;

    // SendData - 发送数据
    // 参数:
    //   lSendIP - 目标标识(TCP未使用，UDP=目标地址)
    //   buf - 待发送数据缓冲区
    //   nlen - 数据长度
    // 返回: true=成功, false=失败
    // 功能: 将数据发送到网络
    virtual bool SendData( unsigned int lSendIP , char* buf , int nlen ) = 0;

    // GetIPString - IP转换工具函数
    // 参数: ip - long类型的IP地址
    // 返回: std::string 点分十进制IP地址字符串
    // 功能: 将数值IP转换为"192.168.1.1"格式
    static std::string GetIPString( long ip );

    // GetValidIPList - 获取本机有效IP列表
    // 返回: std::set<long> 本机所有IPv4地址集合
    // 功能: 枚举本机所有网络接口的IP地址
    static std::set<long> GetValidIPList();

    // IsConnected - 查询连接状态
    // 返回: true=已连接, false=未连接
    // 注意: 主要用于TCP连接状态查询
    virtual bool IsConnected(){ return true;}

protected:
    // RecvData - 接收数据(纯虚函数)
    // 功能: 在独立线程中接收网络数据
    // 实现: 子类实现具体的接收逻辑
    virtual void RecvData() = 0 ;

    // 中介者指针
    // 用途: 网络收到数据后，通过此指针回调INetMediator::DealData()
    // 赋值时机: INetMediator创建具体网络实现时注入
    INetMediator * m_pMediator;

    // Winsock库加载标志
    // 确保Winsock库只加载一次
    static bool m_isLoadlib;
};
