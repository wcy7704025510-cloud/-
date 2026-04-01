#include "ckernel.h"
#include "videoconferencedialog.h"
#include "logindialog.h"
#include "roomdialog.h"
#include "TcpClientMediator.h"

#include "CLoginManager.h"
#include "CRoomManager.h"
#include "CMediaManager.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>

#define NetPackMap(a) m_netPackMap[ a -_DEF_PACK_BASE]

Ckernal::Ckernal(QObject *parent) : QObject(parent),
    m_pVedio(nullptr), m_pLogin(nullptr), m_pRoomDialog(nullptr),
    m_pClient(nullptr), m_pLoginManager(nullptr), m_pRoomManager(nullptr), m_pMediaManager(nullptr)
{
    qDebug()<<__func__;
    qDebug()<<"main threadID:"<< QThread::currentThreadId();

    m_pAVClient[0] = nullptr;
    m_pAVClient[1] = nullptr;

    initConfig();

    // ==========================================
    // Layer 1: 创建 UI 对象 (但不做业务逻辑)
    // ==========================================
    m_pLogin = new LoginDialog;
    connect(m_pLogin, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    m_pVedio = new VedioConferenceDialog;
    connect(m_pVedio, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    m_pRoomDialog = new RoomDialog;
    connect(m_pRoomDialog, SIGNAL(SIG_close()), this, SLOT(slot_destroy()));

    // ==========================================
    // Layer 5: 创建 网络对象 (负责数据搬运)
    // ==========================================
    m_pClient = new TcpClientMediator;
    m_pClient->OpenNet(m_serverIp.toStdString().c_str(), _DEF_PORT);
    connect(m_pClient, SIGNAL(SIG_ReadyData(uint,char*,int)), this, SLOT(slot_dealData(uint,char*,int)));

    for(int p=0; p<2; p++){
        m_pAVClient[p] = new TcpClientMediator;
        m_pAVClient[p]->OpenNet(m_serverIp.toStdString().c_str(), _DEF_PORT);
        connect(m_pAVClient[p], SIGNAL(SIG_ReadyData(uint,char*,int)), this, SLOT(slot_dealData(uint,char*,int)));
    }

    // ==========================================
    // Layer 3 & 4: 创建 Business Managers (依赖注入)
    // ==========================================
    m_pLoginManager = new CLoginManager(this);
    m_pLoginManager->setLoginDialog(m_pLogin);
    connect(m_pLogin, SIGNAL(SIG_loginCommit(QString,QString)), m_pLoginManager, SLOT(slot_loginCommit(QString,QString)));
    connect(m_pLogin, SIGNAL(SIG_registerCommit(QString,QString,QString)), m_pLoginManager, SLOT(slot_registerCommit(QString,QString,QString)));
    connect(m_pLoginManager, SIGNAL(SIG_LoginSuccess(int,QString)), this, SLOT(slot_LoginSuccess(int,QString)));
    connect(m_pLoginManager, SIGNAL(SIG_SendData(char*,int)), this, SLOT(slot_SendClientData(char*,int)));

    m_pRoomManager = new CRoomManager(this);
    m_pRoomManager->setRoomDialog(m_pRoomDialog);
    m_pRoomManager->setVedioDialog(m_pVedio);
    connect(m_pVedio, SIGNAL(SIG_joinRoom()), m_pRoomManager, SLOT(slot_joinRoom()));
    connect(m_pVedio, SIGNAL(SIG_createRoom()), m_pRoomManager, SLOT(slot_createRoom()));
    connect(m_pRoomDialog, SIGNAL(SIG_close()), m_pRoomManager, SLOT(slot_QuitRoom()));
    connect(m_pRoomManager, SIGNAL(SIG_SendData(char*,int)), this, SLOT(slot_SendClientData(char*,int)));

    m_pMediaManager = new CMediaManager(this);
    m_pMediaManager->setRoomManager(m_pRoomManager);
    m_pMediaManager->setRoomDialog(m_pRoomDialog);
    m_pMediaManager->initDevices();

    connect(m_pMediaManager, SIGNAL(SIG_SendAudioData(char*,int)), this, SLOT(slot_SendAudioData(char*,int)));
    connect(m_pMediaManager, SIGNAL(SIG_SendVideoData(char*,int)), this, SLOT(slot_SendVideoData(char*,int)));

    connect(m_pRoomDialog, SIGNAL(SIG_AudioPause()), m_pMediaManager, SLOT(slot_AudioPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_AudioStart()), m_pMediaManager, SLOT(slot_AudioStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_VideoPause()), m_pMediaManager, SLOT(slot_VideoPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_VideoStart()), m_pMediaManager, SLOT(slot_VideoStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_ScreenPause()), m_pMediaManager, SLOT(slot_ScreenPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_ScreenStart()), m_pMediaManager, SLOT(slot_ScreenStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_setMoji(int)), m_pMediaManager, SLOT(slot_setMoji(int)));

    connect(m_pRoomManager, SIGNAL(SIG_RoomQuitted()), m_pMediaManager, SLOT(slot_clearDevices()));

    // ⚠️ 注意：Manager 初始化完毕后，再挂载路由表，确保 Lambda 里调用的指针不为空
    setNetPackMap();

    // 显示登录界面
    m_pLogin->show();
}

Ckernal::~Ckernal()
{
}

void Ckernal::initConfig()
{
     m_serverIp=_DEF_SERVERIP;
     QString path = QApplication::applicationDirPath()+"/config.ini";
     QFileInfo info(path);
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
         settings.beginGroup("Net");
         settings.setValue("ip",m_serverIp);
         settings.endGroup();
     }
     qDebug()<<"ServerIp:"<<m_serverIp;
}

void Ckernal::setNetPackMap()
{
    qDebug()<<__func__;

    for(int i = 0; i < _DEF_PACK_COUNT; i++) {
        m_netPackMap[i] = nullptr;
    }

    // 【核心修改】利用 Lambda 表达式实现 O(1) 路由，告别 if/else
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

    NetPackMap(_DEF_PACK_AUDIO_FRAME) = [this](uint sock, char* buf, int nLen) {
        m_pMediaManager->slot_audioFrameRq(sock, buf, nLen);
    };
    NetPackMap(_DEF_PACK_VEDIO_FRAME) = [this](uint sock, char* buf, int nLen) {
        m_pMediaManager->slot_videoFrameRq(sock, buf, nLen);
    };
}

void Ckernal::slot_dealData(uint sock, char *buf, int nLen)
{
    int type=*(int*)buf;
    if(type>=_DEF_PACK_BASE && type<_DEF_PACK_BASE+_DEF_PACK_COUNT){
        // 【核心修改】提取 std::function 直接调用
        NetHandler handler = NetPackMap(type);
        if(handler){
            handler(sock,buf,nLen);
        } else {
            qDebug() << "Warning: 未注册或未知的网络协议类型:" << type;
        }
    } else {
        qDebug() << "Error: 越界数据包类型:" << type;
    }
    delete[] buf;
}

void Ckernal::slot_SendClientData(char *buf, int nLen)
{
    if(m_pClient) m_pClient->SendData(0, buf, nLen);
}

void Ckernal::slot_SendAudioData(char *buf, int nLen)
{
    if(m_pAVClient[audio_client]) m_pAVClient[audio_client]->SendData(0, buf, nLen);
}

void Ckernal::slot_SendVideoData(char *buf, int nLen)
{
    if(m_pAVClient[video_client]) m_pAVClient[video_client]->SendData(0, buf, nLen);
}

void Ckernal::slot_LoginSuccess(int userId, QString name)
{
    m_pVedio->setInfo(name.toLocal8Bit().data());
    m_pVedio->showNormal();

    // 记录 ID 给下游
    m_pRoomManager->setUserId(userId, name);
    m_pMediaManager->setUserId(userId);

    // 注册 视频和音频的fd
    STRU_VIDEO_REGISTER rq_video;
    rq_video.userid = userId;
    STRU_AUDIO_REGISTER rq_audio;
    rq_audio.userid = userId;

    m_pAVClient[audio_client]->SendData(0,(char*)&rq_audio,sizeof(rq_audio));
    m_pAVClient[video_client]->SendData(0,(char*)&rq_video,sizeof(rq_video));
}

void Ckernal::slot_destroy()
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
    if(m_pVedio){
        m_pVedio->hide();
        delete m_pVedio;
        m_pVedio=nullptr;
    }
    if(m_pLogin){
        m_pLogin->hide();
        delete m_pLogin;
        m_pLogin=nullptr;
    }
    if(m_pRoomDialog){
        m_pRoomDialog->hide();
        delete m_pRoomDialog;
        m_pRoomDialog=nullptr;
    }

    exit(0);
}
