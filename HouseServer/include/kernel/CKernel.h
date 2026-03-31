#ifndef CKERNEL_H
#define CKERNEL_H

#include "./Mediator/INetMediator.h"
#include "Mysql.h"
#include "./Mediator/TcpMediator.h"
#include "./Net/TcpNet.h"
#include <list>
#include <map>

// 前置声明
class CLogic;
struct UserInfo; // 假设在 packdef.h 中定义
typedef int sock_fd;

class CKernel
{
public:
    // 单例模式
    static CKernel* GetInstance();

    // 开启核心服务
    int StartServer(int port);
    // 关闭核心服务
    void CloseServer();

    // 发送数据 (提供给逻辑层调用)
    void SendData(int sockfd, char* szbuf, int nlen);
    // 处理数据 (提供给中介者调用)
    void DealData(int sockfd, char* szbuf, int nlen);

public:
    // 数据库指针
    CMysql* m_sql;
    // 逻辑处理层指针
    CLogic* m_logic;
    // 网络中介者指针
    INetMediator* m_pMediator;

    // 共享数据区：STL map 不是线程安全的，需要加锁避免添加移除元素可能导致的问题
    MyMap<int, UserInfo*> m_mapIdToUserInfo;     //存储用户ID->用户信息结构体
    MyMap<int, std::list<int>> m_mapIdToRoomId;  //存储房间id->房间内用户ID

private:
    CKernel();
    ~CKernel();

    // 初始化随机数
    void initRand();
};

#endif // CKERNEL_H
