#ifndef NETBUFFER_H
#define NETBUFFER_H

#include <stddef.h>

// 前向声明，避免循环依赖
class INet;

//用于在线程池的生产者-消费者模式中传递数据
struct NetBuffer
{

    //构造函数进行初始化
    NetBuffer(INet* _pNet, int _fd, char* _buf, int _nlen)
        : pNet(_pNet)
        , fd(_fd)
        , buf(_buf)
        , nlen(_nlen)
    {}

    INet* pNet;   //网络类基类指针，用于回调
    int fd;       //文件描述符（TCP socket 或 KCP 的fd）
    char* buf;    //数据缓冲区
    int nlen;     //数据长度
};

//TCP专用数据缓冲结构
struct TcpDataBuffer : public NetBuffer
{
    TcpDataBuffer(INet* pNet, int sockfd, char* buf, int nlen)
        : NetBuffer(pNet, sockfd, buf, nlen)
    {}
};

//KCP专用数据缓冲结构
struct KcpAppBuffer : public NetBuffer
{
    KcpAppBuffer(INet* pNet, int vfd, char* buf, int nlen)
        : NetBuffer(pNet, vfd, buf, nlen)
    {}
};

#endif // NETBUFFER_H
