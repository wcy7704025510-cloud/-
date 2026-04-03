#include "ckernel.h"
#include "videoconferencedialog.h"
#include "logindialog.h"
#include "roomdialog.h"
#include "INetMediator.h"
#include "TcpClient.h"
#include "CLoginManager.h"
#include "CRoomManager.h"
#include "CMediaManager.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>

// [核心设计] 利用宏定义实现 O(1) 时间复杂度的协议路由映射
// 通过协议号减去基准值，直接作为数组下标访问处理函数
#define NetPackMap(a) m_netPackMap[ a -_DEF_PACK_BASE]

Ckernel::Ckernel(QObject *parent) : QObject(parent),
    m_pVedio(nullptr), m_pLogin(nullptr), m_pRoomDialog(nullptr),
    m_pClient(nullptr), m_pLoginManager(nullptr), m_pRoomManager(nullptr), m_pMediaManager(nullptr)
{
    qDebug()<<__func__;
    //   QThread::currentThreadId()：不常用，但在排查多线程死锁或 UI 跨线程崩溃时非常有用
    qDebug()<<"main threadID:"<< QThread::currentThreadId();

    m_pAVClient[0] = nullptr;
    m_pAVClient[1] = nullptr;

    // 初始化本地配置（如服务器 IP）
    initConfig();

    // ==========================================
    // Layer 1: 创建 UI 对象 (遵循中介者模式，UI 只管显示，不碰逻辑)
    // ==========================================
    m_pLogin = new LoginDialog;
    connect(m_pLogin, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    m_pVedio = new VedioConferenceDialog;
    connect(m_pVedio, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    m_pRoomDialog = new RoomDialog;
    connect(m_pRoomDialog, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    // ==========================================
    // Layer 5: 创建网络层 (物理连接层)
    // ==========================================
    // 1. 信令通道装配 (负责登录、加入房间等控制指令)
    m_pClient = new INetMediator();
    m_pClient->SetNetEngine(new TcpClient(m_pClient));
    m_pClient->OpenNet(m_serverIp.toStdString().c_str(), _DEF_PORT);
    // 绑定底层原始数据上送信号
    connect(m_pClient, SIGNAL(SIG_ReadyData(uint,char*,int)), this, SLOT(slot_dealData(uint,char*,int)));

    // 2. 音视频双通道装配 (独立 TCP 连接，防止大流量视频帧导致控制指令延迟)
    for(int p=0; p<2; p++){
        m_pAVClient[p] = new INetMediator();
        m_pAVClient[p]->SetNetEngine(new TcpClient(m_pAVClient[p]));
        m_pAVClient[p]->OpenNet(m_serverIp.toStdString().c_str(), _DEF_PORT);
        connect(m_pAVClient[p], SIGNAL(SIG_ReadyData(uint,char*,int)), this, SLOT(slot_dealData(uint,char*,int)));
    }

    // ==========================================
    // Layer 3 & 4: 创建业务管理器 (Business Managers)
    // ==========================================

    // 登录模块：处理账号、密码、注册等
    m_pLoginManager = new CLoginManager(this);
    m_pLoginManager->setLoginDialog(m_pLogin);
    connect(m_pLogin, SIGNAL(SIG_loginCommit(QString,QString)), m_pLoginManager, SLOT(slot_loginCommit(QString,QString)));
    connect(m_pLogin, SIGNAL(SIG_registerCommit(QString,QString,QString)), m_pLoginManager, SLOT(slot_registerCommit(QString,QString,QString)));
    connect(m_pLoginManager, SIGNAL(SIG_LoginSuccess(int,QString)), this, SLOT(slot_LoginSuccess(int,QString)));
    connect(m_pLoginManager, SIGNAL(SIG_SendData(char*,int)), this, SLOT(slot_SendClientData(char*,int)));

    // 房间模块：处理成员进出、房间创建等
    m_pRoomManager = new CRoomManager(this);
    m_pRoomManager->setRoomDialog(m_pRoomDialog);
    m_pRoomManager->setVedioDialog(m_pVedio);
    connect(m_pVedio, SIGNAL(SIG_joinRoom()), m_pRoomManager, SLOT(slot_joinRoom()));
    connect(m_pVedio, SIGNAL(SIG_createRoom()), m_pRoomManager, SLOT(slot_createRoom()));
    connect(m_pRoomDialog, SIGNAL(SIG_close()), m_pRoomManager, SLOT(slot_QuitRoom()));
    connect(m_pRoomManager, SIGNAL(SIG_SendData(char*,int)), this, SLOT(slot_SendClientData(char*,int)));

    // 媒体模块：处理设备初始化、音视频采集绘制、滤镜(萌拍)等
    m_pMediaManager = new CMediaManager(this);
    m_pMediaManager->setRoomManager(m_pRoomManager);
    m_pMediaManager->setRoomDialog(m_pRoomDialog);
    m_pMediaManager->initDevices(); // [关键] 初始化声卡、摄像头硬件

    connect(m_pMediaManager, SIGNAL(SIG_SendAudioData(char*,int)), this, SLOT(slot_SendAudioData(char*,int)));
    connect(m_pMediaManager, SIGNAL(SIG_SendVideoData(char*,int)), this, SLOT(slot_SendVideoData(char*,int)));

    // 媒体控制信号连接
    connect(m_pRoomDialog, SIGNAL(SIG_AudioPause()), m_pMediaManager, SLOT(slot_AudioPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_AudioStart()), m_pMediaManager, SLOT(slot_AudioStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_VideoPause()), m_pMediaManager, SLOT(slot_VideoPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_VideoStart()), m_pMediaManager, SLOT(slot_VideoStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_ScreenPause()), m_pMediaManager, SLOT(slot_ScreenPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_ScreenStart()), m_pMediaManager, SLOT(slot_ScreenStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_setMoji(int)), m_pMediaManager, SLOT(slot_setMoji(int)));

    connect(m_pRoomManager, SIGNAL(SIG_RoomQuitted()), m_pMediaManager, SLOT(slot_clearDevices()));

    // [核心步骤] Manager 初始化完毕后，挂载路由表
    setNetPackMap();

    // 程序入口：显示登录界面
    m_pLogin->show();
}

Ckernel::~Ckernel()
{
}

/**
 * [配置读取逻辑]
 * 负责从 ini 文件读取 IP，实现“不用重新编译即可连接不同服务器”
 */
void Ckernel::initConfig()
{
     m_serverIp=_DEF_SERVERIP;
     //   applicationDirPath()：获取程序运行时的绝对路径，确保配置文件位置正确
     QString path = QApplication::applicationDirPath()+"/config.ini";
     QFileInfo info(path);
     //   QSettings：Qt 专门处理配置文件的类，支持 IniFormat，逻辑简单高效
     QSettings settings(path,QSettings::IniFormat,NULL);

     if( info.exists()){
         settings.beginGroup("Net");
         QVariant ip=settings.value("ip");
         QString strIP=ip.toString();
         settings.endGroup();
         if(!strIP.isEmpty()){
             m_serverIp=strIP;
         }
     }else{
         // 首次运行，自动创建配置文件
         settings.beginGroup("Net");
         settings.setValue("ip",m_serverIp);
         settings.endGroup();
     }
     qDebug()<<"ServerIp:"<<m_serverIp;
}

/**
 * [路由表挂载]
 * 这种设计避免了传统的几千行 switch-case，通过下标直接定位业务函数
 */
void Ckernel::setNetPackMap()
{
    qDebug()<<__func__;

    for(int i = 0; i < _DEF_PACK_COUNT; i++) {
        m_netPackMap[i] = nullptr;
    }

    // 利用 Lambda 表达式实现 O(1) 路由。当网络包到达，直接执行对应的闭包函数
    NetPackMap(_DEF_PACK_REGISTER_RS) = [this](uint sock, char* buf, int nLen) {
        m_pLoginManager->slot_dealRegisterRs(sock, buf, nLen);
    };
    NetPackMap(_DEF_PACK_LOGIN_RS) = [this](uint sock, char* buf, int nLen) {
        m_pLoginManager->slot_dealLoginRs(sock, buf, nLen);
    };

    NetPackMap(_DEF_JOINROOM_RS) = [this](uint sock, char* buf, int nLen) {
        m_pRoomManager->slot_dealJoinRoomRs(sock, buf, nLen);
    };
    NetPackMap(_DEF_CREATEROOM_RS) = [this](uint sock, char* buf, int nLen) {
        m_pRoomManager->slot_dealCreateRoomRs(sock, buf, nLen);
    };
    NetPackMap(_DEF_PACK_ROOM_MEMBER) = [this](uint sock, char* buf, int nLen) {
        m_pRoomManager->slot_dealUserInfoRq(sock, buf, nLen);
    };
    NetPackMap(_DEF_PACK_LEAVEROOM_RQ) = [this](uint sock, char* buf, int nLen) {
        m_pRoomManager->slot_dealQuitRoomRq(sock, buf, nLen);
    };

    // 媒体包路由：音频帧、视频帧
    NetPackMap(_DEF_PACK_AUDIO_FRAME) = [this](uint sock, char* buf, int nLen) {
        m_pMediaManager->slot_audioFrameRq(sock, buf, nLen);
    };
    NetPackMap(_DEF_PACK_VEDIO_FRAME) = [this](uint sock, char* buf, int nLen) {
        m_pMediaManager->slot_videoFrameRq(sock, buf, nLen);
    };
}

/**
 * [网络数据分发槽]
 * 所有底层收到的包，统一经过此门，根据包头 type 分流
 */
void Ckernel::slot_dealData(uint sock, char *buf, int nLen)
{
    // 提取包头前 4 字节的协议类型
    int type=*(int*)buf;
    if(type>=_DEF_PACK_BASE && type<_DEF_PACK_BASE+_DEF_PACK_COUNT){
        // 查找路由表并调用业务逻辑
        NetHandler handler = NetPackMap(type);
        if(handler){
            handler(sock,buf,nLen);
        } else {
            qDebug() << "Warning: 未处理的协议类型:" << type;
        }
    } else {
        qDebug() << "Error: 协议号越界:" << type;
    }
    // [重要] 释放由网络底层申请的缓冲区，防止内存溢出
    delete[] buf;
}

// 三路发送函数：分别对应控制流、音频流、视频流
void Ckernel::slot_SendClientData(char *buf, int nLen)
{
    if(m_pClient) m_pClient->SendData(0, buf, nLen);
}

void Ckernel::slot_SendAudioData(char *buf, int nLen)
{
    if(m_pAVClient[audio_client]) m_pAVClient[audio_client]->SendData(0, buf, nLen);
}

void Ckernel::slot_SendVideoData(char *buf, int nLen)
{
    qDebug()<<"slot_SendVideoData"<<*(int*)buf;
    if(m_pAVClient[video_client]) m_pAVClient[video_client]->SendData(0, buf, nLen);
    delete[] buf;
}

/**
 * [关键业务点：登录成功]
 */
void Ckernel::slot_LoginSuccess(int userId, QString name)
{
    //   toLocal8Bit()：解决 Windows 环境下 GBK 与 Unicode 之间的乱码问题
    m_pVedio->setInfo(name.toLocal8Bit().data());
    m_pVedio->showNormal();

    // 业务下沉：同步用户身份给各个管理器
    m_pRoomManager->setUserId(userId, name.toLocal8Bit().data());
    m_pMediaManager->setUserId(userId);

    // [逻辑深点] 注册 AV 连接：向服务器告知当前这几个 Socket 属于哪位 UserId
    STRU_VIDEO_REGISTER rq_video;
    rq_video.userid = userId;
    STRU_AUDIO_REGISTER rq_audio;
    rq_audio.userid = userId;

    m_pAVClient[audio_client]->SendData(0,(char*)&rq_audio,sizeof(rq_audio));
    m_pAVClient[video_client]->SendData(0,(char*)&rq_video,sizeof(rq_video));
}

/**
 * [销毁逻辑]
 * 负责回收网络连接、销毁堆内存 UI 对象，确保程序正常退出
 */
void Ckernel::slot_destroy()
{
    qDebug()<<__func__;

    if(m_pClient){
        m_pClient->CloseNet();
        delete m_pClient;
        m_pClient=nullptr;
    }
    for(int i=0; i<2; i++){
        if(m_pAVClient[i]){
            m_pAVClient[i]->CloseNet();
            delete m_pAVClient[i];
            m_pAVClient[i] = nullptr;
        }
    }
    if(m_pVedio)      { m_pVedio->hide(); delete m_pVedio; m_pVedio=nullptr; }
    if(m_pLogin)      { m_pLogin->hide(); delete m_pLogin; m_pLogin=nullptr; }
    if(m_pRoomDialog) { m_pRoomDialog->hide(); delete m_pRoomDialog; m_pRoomDialog=nullptr; }

    // 强制终止当前进程并向 OS 返回状态码 0
    exit(0);
}
