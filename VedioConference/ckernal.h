#ifndef CKERNAL_H
#define CKERNAL_H

#include <QDebug>
#include <QTime>
#include <QImage>
#include <QBuffer>
#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QInputDialog>

#include "QRegExp"
#include "./vedioconferencedialog.h"
#include "./logindialog.h"
#include "./TcpClientMediator.h"
#include "./packdef.h"
#include "./logindialog.h"
#include "./md5/md5.h"
#include "./roomdialog.h"
#include "./AudioApi/audio_read.h"
#include "./AudioApi/audio_write.h"
#include "./VedioApi/vedioread.h"
#include "./VedioApi/screenread.h"
#include "./threadworker.h"

//协议定义表使用的类型
class Ckernal;
typedef void (Ckernal::*PFUN)(uint sock,char* buf,int nLen);

//发送视频帧线程类声明
class SendVedioWorker;

class Ckernal : public QObject
{
    Q_OBJECT
public:
    explicit Ckernal(QObject *parent = nullptr);

     //单例模式
    static Ckernal* GetInstance(){
        static Ckernal kernal;
        return &kernal;
    }
signals:
    //发送视频帧的信号
    void SIG_sendVideo(char*,int);
public slots:
    //设置网络协议映射表
    void setNetPackMap();
    //初始化配置
    void initConfig();
    //销毁主界面类槽函数
    void slot_destroy();
    //网络信息处理
    void slot_dealData(uint sock,char* buf,int nLen);
    //处理登录回复
    void slot_dealLoginRs(uint sock,char* buf,int nLen);
    //处理注册回复
    void slot_dealRegisterRs(uint sock,char* buf,int nLen);
    //处理创建房间回复
    void slot_dealCreateRoomRs(uint sock,char* buf,int nLen);
    //处理加入房间回复
    void slot_dealJoinRoomRs(uint sock,char* buf,int nLen);
    //处理用户信息请求
    void slot_dealUserInfoRq(uint sock,char* buf,int nLen);
    //处理用户退出房间请求
    void slot_dealQuitRoomRq(uint sock,char* buf,int nLen);
    //处理用户登录请求
    void slot_loginCommit(QString tel,QString password);
    //处理用户注册请求
    void slot_registerCommit(QString tel,QString password,QString name);
    //处理接收到的音频数据
    void slot_audioFrameRq(uint sock,char* buf,int nLen);
    //处理接收到的视频数据
    void slot_videoFrameRq(uint sock,char* buf,int nLen);
    //发送音频帧
    void slot_audioFrame(QByteArray ba);
    //发送视频帧
    void slot_sendVedioFrame(QImage img);
    //处理用户创建会议信号的槽函数
    void slot_createRoom();
    //处理用户加入会议信号的槽函数
    void slot_joinRoom();
    //处理用户退出会议信号的槽函数
    void slot_QuitRoom();
    //处理关闭音频信号的槽函数
    void slot_AudioPause();
    //处理打开音频信号的槽函数
    void slot_AudioStart();
    //处理关闭视频信号的槽函数
    void slot_VideoPause();
    //处理打开视频信号的槽函数
    void slot_VideoStart();
    //处理关闭桌面共享信号的槽函数
    void slot_ScreenPause();
    //处理打开桌面共享信号的槽函数
    void slot_ScreenStart();
    //刷新显示的视频帧
    void slot_refreshVideo(int id,QImage& img);
    //多线程处理接收到的视频帧信号对应的槽函数
    void slot_sendVideo(char*,int);

private:    //私有属性，公有方法
    VedioConferenceDialog*  m_pVedio;                           //主界面类
    INetMediator*           m_pClient;                          //客户端网络类
    LoginDialog*            m_pLogin;                           //登录注册类
    PFUN                    m_netPackMap[_DEF_PACK_COUNT];      //网络协议映射数组
    QString                 m_serverIp;                         //存储服务端ip地址
    RoomDialog*             m_pRoomDialog;                      //存储用户房间信息

    //用户属性
    int                     m_id;                               //存储用户ID
    int                     m_roomId;                           //存储房间ID
    char                    m_name[_MAX_SIZE];                  //存储用户姓名

    //音频：一个采集，多个播放 每个房间成员 1:1 map映射
    Audio_Read*             m_pAudio_Read;                      //获取音频类对象
    std::map<int,Audio_Write*>m_mapIdToAudioWrite;              //播放音频类map

    //视频：一个采集
    VideoRead *m_pVedioRead;

    //桌面共享类对象
    ScreenRead *m_pSreenRead;

    //发送视频帧的线程对象
    QSharedPointer<SendVedioWorker> m_pSendVedioWorker;

    //音频/视频发送网络类对象
    enum client_type{audio_client = 0,video_client};
    INetMediator *m_pAVClient[2];
};

class SendVedioWorker : public ThreadWorker{
    Q_OBJECT
public slots:
    //收到信号 发送视频帧 回收额外空间
    void slot_dowork(char* buf,int len){
        qDebug()<<"send video threadId:"<<QThread::currentThreadId();
        Ckernal::GetInstance()->slot_sendVideo(buf,len);
    }
};

#endif // CKERNAL_H
