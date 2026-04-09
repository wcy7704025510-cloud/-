#pragma once

// Qt核心框架
#include <QObject>              // Qt对象基类

// ============= 网络配置常量 =============
// 默认TCP端口：控制信令使用(登录、注册、房间操作等)
#define _DEF_TCP_PORT  (8000)

// 默认UDP端口：音视频数据传输使用
// UDP优势：无连接、低延迟、适合实时音视频
#define _DEF_UDP_PORT  (1234)

// 默认服务器IP地址
// 开发环境使用本地回环地址
#define _DEF_SERVER_IP ("192.168.1.168")

// 前向声明
class INet; // 底层网络接口类

// INetMediator
// 职责：作为网络层与业务层之间的桥梁
// 工作机制：
//   业务层 -> INetMediator -> INet(底层) -> 网络
//   网络 -> INet(底层) -> INetMediator -> 业务层
// 优势：业务层不关心具体使用TCP/UDP/KCP，只需调用INetMediator
class INetMediator : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
signals:
    // 网络数据就绪信号
    // 参数:
    //   lSendIP - 发送端标识(TCP=socket描述符，UDP=0)
    //   buf - 接收到的数据缓冲区
    //   nlen - 数据长度
    // 接收者: Ckernel::slot_dealData()
    void SIG_ReadyData( unsigned int lSendIP , char* buf , int nlen );

    // 连接断开信号
    // 触发: 网络连接异常断开
    // 接收者: 业务层用于处理断开逻辑
    void SIG_disConnect();

public:
    // 构造函数/析构函数
    INetMediator();
    virtual ~INetMediator();

    // SetNetEngine - 设置底层网络引擎(依赖注入)
    // 参数: pNet - INet派生类实例(TCP/UDP/KCP)
    // 功能: 将具体网络实现注入中介者
    // 设计: 支持切换不同网络协议(TcpClient/UdpNet/KcpNet等)
    void SetNetEngine(INet* pNet);

    // OpenNet - 打开网络连接
    // 参数:
    //   szBufIP - 服务器IP地址(默认"0.0.0.0"表示本地)
    //   port - 端口号
    // 返回: true=成功, false=失败
    // 功能: 初始化底层网络，开始监听/连接
    virtual bool OpenNet(const char* szBufIP = "0.0.0.0", unsigned short port = _DEF_TCP_PORT);

    // CloseNet - 关闭网络连接
    // 功能: 断开连接，释放资源
    virtual void CloseNet();

    // SendData - 发送数据
    // 参数:
    //   lSendIP - 目标标识(TCP未使用，UDP=目标地址)
    //   buf - 待发送数据缓冲区
    //   nlen - 数据长度
    // 返回: true=成功, false=失败
    // 功能: 将数据透传给底层网络发送
    virtual bool SendData(unsigned int lSendIP , char* buf , int nlen);

    // DealData - 处理网络数据
    // 参数: 同SIG_ReadyData
    // 功能: 将底层接收的数据转换为Qt信号向上发射
    // 触发: 底层网络收到数据后调用
    virtual void DealData(unsigned int lSendIP , char* buf , int nlen);

    // setIpAndPort - 设置目标地址
    // 参数: szBufIP-IP地址, port-端口号
    // 功能: 保存连接目标，供OpenNet使用
    void setIpAndPort(const char* szBufIP, unsigned short port);

    // IsConnected - 查询连接状态
    // 返回: true=已连接, false=未连接
    virtual bool IsConnected();

    // disConnect - 主动断开连接
    // 功能: 发送断开信号，通知业务层处理
    virtual void disConnect();

protected:
    // 底层网络接口指针
    // 使用抽象基类指针，支持多态切换不同网络实现
    // 赋值时机: SetNetEngine()调用时
    INet* m_pNet;

    // 服务器IP地址字符串(IPv4最大15字符+结束符=16，实际18字节保险)
    char  m_szBufIP[18];

    // 服务器端口号
    unsigned short m_port;
};
