#include "CKernel.h"
#include "AccountLogic.h"
#include "RoomLogic.h"
#include "MediaLogic.h"
#include"common/packdef.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "KcpNet.h"

CKernel::CKernel()
{
    m_sql = new CMysql;

    //1：装配信令专属网络通道 (默认走 TCP)

    m_pSignalingNet = new INetMediator(this);
    m_pSignalingNet->SetNetEngine(new TcpNet(m_pSignalingNet));

    //2：装配音视频专属网络通道（KCP）
    m_pMediaNet = new INetMediator(this);
    m_pMediaNet->SetNetEngine(new KcpNet(m_pMediaNet));

    //3：挂载逻辑层处理中心
    m_accountLogic = new AccountLogic(this);
    m_roomLogic = new RoomLogic(this);
    m_mediaLogic = new MediaLogic(this);
}

CKernel::~CKernel()
{
    if(m_accountLogic) { delete m_accountLogic; m_accountLogic = nullptr; }
    if(m_roomLogic) { delete m_roomLogic; m_roomLogic = nullptr; }
    if(m_mediaLogic) { delete m_mediaLogic; m_mediaLogic = nullptr; }

    // 分别释放双通道内存
    if(m_pSignalingNet) { delete m_pSignalingNet; m_pSignalingNet = nullptr; }
    if(m_pMediaNet) { delete m_pMediaNet; m_pMediaNet = nullptr; }

    if(m_sql) { delete m_sql; m_sql = nullptr; }
}
// 单例模式
CKernel* CKernel::GetInstance()
{
    //通过懒汉式单例模式来实现中介者的唯一性
    static CKernel kernel;
    return &kernel;
}

// 用于生成随机数的内部方法
void CKernel::initRand()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_sec * 1000 + time.tv_usec / 1000);
}

// 开启核心服务(初始化随机数、设置映射函数、连接数据库、打开双端口)
int CKernel::StartServer(int port)
{
    //初始化随机数
    initRand();
    //设置映射函数
    setNetPackMap();
    //连接数据库
    if (m_sql) {
        if(!m_sql->ConnectMysql(_DEF_DB_IP, _DEF_DB_USER, _DEF_DB_PWD, _DEF_DB_NAME)) {
            printf("数据库连接失败!\n");
        } else {
            printf("数据库连接成功!\n");
        }
    }
    // 开启信令服务端口：负责登录、注册、好友管理等可靠信令交互
    if(m_pSignalingNet) {
        m_pSignalingNet->Open(port);     // 信令监听，例如 8000
    }

    // 开启媒体服务端口：负责音视频流、屏幕共享等实时数据传输
    if(m_pMediaNet) {
        m_pMediaNet->Open(port + 1);     // 音视频监听，例如 8001
    }

    // 启动独立线程处理媒体流epoll事件
    // 避免信令与媒体IO阻塞，提升高并发场景下系统稳定性
    std::thread mediaThread([this]() {
        if(m_pMediaNet) {
            m_pMediaNet->EventLoop();
        }
    });
    // 分离线程，让其在后台独立处理海量视频流
    mediaThread.detach();
    return 0;
}


//关闭整体服务
void CKernel::CloseServer()
{
    if(m_pSignalingNet) { m_pSignalingNet->Close(); }
    if(m_pMediaNet) { m_pMediaNet->Close(); }
}

//业务逻辑层调用中介者发送信息
void CKernel::SendData(int sockfd, char* szbuf, int nlen)
{
    // 提取包头前 4 个字节，获取协议类型
    int nType = *(int*)szbuf;

    // 根据业务协议类型，进行物理通道的精准分流
    if (nType == _DEF_PACK_AUDIO_FRAME || nType == _DEF_PACK_VEDIO_FRAME) {

        // 纯媒体流数据，绝对走KCP极速通道
        if(m_pMediaNet) {
            m_pMediaNet->SendData(sockfd, szbuf, nlen);
        }
    } else {
        // 其他所有控制信令（注册、登录、建房、人员变动等）走TCP高可靠通道
        if(m_pSignalingNet) {
            m_pSignalingNet->SendData(sockfd, szbuf, nlen);
        }
    }
}

// 处理数据(提供给中介者调用具体的处理函数)
void CKernel::ReadyData(int sockfd, char* szbuf, int nlen)
{
    //获取包头
    int nType = *(int*)szbuf;
    //判断包类型是否正确范围内
    if(nType >= _DEF_PACK_BASE && nType < _DEF_PACK_BASE + _DEF_PACK_COUNT)
    {
        auto ite = m_NetPackMap.find(nType);
        if(ite != m_NetPackMap.end())
        {
            ite->second(sockfd, szbuf, nlen);
        }
    }
}

//绑定函数映射
void CKernel::setNetPackMap()
{
    m_NetPackMap[_DEF_PACK_REGISTER_RQ]    = std::bind(&AccountLogic::RegisterRq, m_accountLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_PACK_LOGIN_RQ]       = std::bind(&AccountLogic::LoginRq, m_accountLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_CREATEROOM_RQ]       = std::bind(&RoomLogic::CreateRoomRq, m_roomLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_JOINROOM_RQ]         = std::bind(&RoomLogic::JoinRoomRq, m_roomLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_PACK_LEAVEROOM_RQ]   = std::bind(&RoomLogic::LeaveRoomRq, m_roomLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    // 媒体处理层挂载
    m_NetPackMap[_DEF_PACK_AUDIO_FRAME]    = std::bind(&MediaLogic::AudioFrameRq, m_mediaLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_PACK_VEDIO_FRAME]    = std::bind(&MediaLogic::VideoFrameRq, m_mediaLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    // 客户端音视频通道建立后，绑定的路由
    m_NetPackMap[_DEF_PACK_AUDIO_REGISTER] = std::bind(&MediaLogic::AudioRegister, m_mediaLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_NetPackMap[_DEF_PACK_VIDEO_REGISTER] = std::bind(&MediaLogic::VideoRegister, m_mediaLogic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}
