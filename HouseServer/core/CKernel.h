#ifndef CKERNEL_H
#define CKERNEL_H

#include "INetMediator.h"
#include "Mysql.h"
#include "TcpNet.h"
#include <list>
#include <map>
#include <functional>
#include <thread>

class AccountLogic;     //用户登录业务处理类
class RoomLogic;        //房间业务处理类
class MediaLogic;       //视频流媒体处理类
typedef int sock_fd;

class CKernel
{
private:
    CKernel();
    ~CKernel();
    void initRand(); // 用于生成随机数的内部方法

public:
    // 单例模式
    static CKernel* GetInstance();
    // 开启核心服务
    int StartServer(int port);
    // 关闭核心服务
    void CloseServer();
    // 发送数据 (提供给逻辑层调用)
    void SendData(int sockfd, char* szbuf, int nlen);
    // 处理数据 (提供给中介者调用具体的处理函数)
    void ReadyData(int sockfd, char* szbuf, int nlen);
public:
    // 数据库指针
    CMysql* m_sql;
    // 逻辑处理层指针
    AccountLogic* m_accountLogic;
    RoomLogic* m_roomLogic;
    MediaLogic* m_mediaLogic;

    //网络层指针
    INetMediator* m_pSignalingNet; // 信令专属网络层 (处理高可靠、低频指令)
    INetMediator* m_pMediaNet;     // 音视频专属网络层 (处理高吞吐、密集型媒体流)

    // 协议映射表
    std::map<int, std::function<void(sock_fd, char*, int)>> m_NetPackMap;

    //初始化协议
    void setNetPackMap();

    // 共享数据区：STL map 不是线程安全的，使用自定义的 MyMap 加锁避免冲突
    MyMap<int, UserInfo*> m_mapIdToUserInfo;     // 存储用户ID -> 用户信息
    MyMap<int, std::list<int>> m_mapIdToRoomId;  // 存储房间ID -> 房间内的成员列表
};

#endif // CKERNEL_H
