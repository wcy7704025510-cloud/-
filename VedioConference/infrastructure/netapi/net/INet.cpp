#include"INet.h"               // 网络接口头文件

// Winsock库加载标志初始化
// static成员变量类外定义
bool INet::m_isLoadlib = false;

// GetIPString - IP数值转换为字符串
// 参数: ip - long类型的IP地址(网络字节序)
// 返回: std::string 点分十进制格式IP地址
// 原理: 利用SOCKADDR_IN结构体的临时转换
std::string INet::GetIPString( long ip )
{
    // 创建sockaddr_in结构体
    SOCKADDR_IN sockaddr;
    // 将long型IP赋值给结构体的地址字段
    sockaddr.sin_addr.S_un.S_addr = ip;
    // inet_ntoa: 网络字节序IP转换为"xxx.xxx.xxx.xxx"字符串
    return inet_ntoa(sockaddr.sin_addr);
}

// GetValidIPList - 获取本机有效IP地址列表
// 返回: std::set<long> 本机所有IPv4地址集合
// 流程: 获取主机名 -> 根据主机名获取IP列表
std::set<long> INet::GetValidIPList()
{
    char bufip[100];  // 存储主机名

    // 1. 获取本机主机名
    // gethostname: 获取本地主机的标准主机名
    // 参数: bufip-存储主机名缓冲区, 100-缓冲区大小
    // 返回: 0成功, SOCKET_ERROR失败
    if( SOCKET_ERROR != gethostname( bufip , 100) )
    {
        // 2. 根据主机名获取IP地址列表
        struct hostent *remoteHost;  // 主机信息结构体指针

        struct in_addr addr;         // IP地址结构体

        std::set<long> setIP;       // 存储有效IP集合

        // gethostbyname: 根据主机名获取主机信息
        // 包括所有IP地址列表
        remoteHost = gethostbyname( bufip );

        // 遍历所有IP地址(一个主机可能有多个网卡)
        int i = 0 ;
        while (remoteHost->h_addr_list[i] != 0) {
            // 获取第i个IP地址
            addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
            // 转换为long型并加入集合
            setIP.insert( addr.S_un.S_addr );
        }
        return setIP;   // 返回IP集合
    }else
        return std::set<long>();  // 失败返回空集合
}
