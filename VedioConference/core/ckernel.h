#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <functional>

#include "packdef.h"

// 前向声明
class VedioConferenceDialog;
class LoginDialog;
class INetMediator;
class RoomDialog;
class CLoginManager;
class CRoomManager;
class CMediaManager;

class Ckernel : public QObject
{
    Q_OBJECT
public:
    explicit Ckernel(QObject *parent = nullptr);
    ~Ckernel();

     //单例模式
    static Ckernel* GetInstance(){
        static Ckernel kernel;
        return &kernel;
    }

public slots:
    //设置网络协议映射表
    void setNetPackMap();
    //初始化配置
    void initConfig();
    //销毁主界面类槽函数
    void slot_destroy();

    //网络信息处理入口
    void slot_dealData(uint sock,char* buf,int nLen);

    // 监听LoginManager登录成功的信号
    void slot_LoginSuccess(int userId, QString name);

    //转发各 Manager 的网络发送请求
    void slot_SendClientData(char* buf, int nLen);
    void slot_SendAudioData(char* buf, int nLen);
    void slot_SendVideoData(char* buf, int nLen);

private:
    // UI 对象
    VedioConferenceDialog* m_pVedio;
    LoginDialog* m_pLogin;
    RoomDialog* m_pRoomDialog;

    // 网络对象
    INetMediator* m_pClient;        //用来发送信令
    enum client_type{audio_client = 0,video_client};
    INetMediator* m_pAVClient[2];   //用来发送kcp音频和视频

    // 业务 Manager
    CLoginManager* m_pLoginManager;     //登录业务
    CRoomManager* m_pRoomManager;       //房间管理业务
    CMediaManager* m_pMediaManager;     //音视频业务

    //相当于typedef std::function<void(uint, char*, int)> NetHandler
    using NetHandler = std::function<void(uint, char*, int)>;
    //存储std::function<void(uint, char*, int)>类型的映射函数
    NetHandler              m_netPackMap[_DEF_PACK_COUNT];

    //服务端的ip
    QString                 m_serverIp;
};

#endif // Ckernel_H
