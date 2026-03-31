#include "ckernal.h"

//宏定义MD5密文键值
#define MD5_KEY (1234)
//宏定义简化协议类型转换
#define NetPackMap(a) m_netPackMap[ a -_DEF_PACK_BASE]

//创建MD5密文
static std::string GetMD5(QString value){
    QString str=QString("%1_%2").arg(value).arg(MD5_KEY);
    std::string strSrc=str.toStdString();
    MD5 md5(strSrc);
    return md5.toString();
}

//设置函数协议映射表
void Ckernal::setNetPackMap()
{
    qDebug()<<__func__;
    memset(this->m_netPackMap,0,sizeof(this->m_netPackMap));

    NetPackMap(_DEF_PACK_REGISTER_RS)       =   &Ckernal::slot_dealRegisterRs;
    NetPackMap(_DEF_PACK_LOGIN_RS)          =   &Ckernal::slot_dealLoginRs;
    NetPackMap(_DEF_JOINROOM_RS)            =   &Ckernal::slot_dealJoinRoomRs;
    NetPackMap(_DEF_CREATEROOM_RS)          =   &Ckernal::slot_dealCreateRoomRs;
    NetPackMap(_DEF_PACK_ROOM_MEMBER)       =   &Ckernal::slot_dealUserInfoRq;
    NetPackMap(_DEF_PACK_LEAVEROOM_RQ)      =   &Ckernal::slot_dealQuitRoomRq;
    NetPackMap(_DEF_PACK_AUDIO_FRAME)       =   &Ckernal::slot_audioFrameRq;
    NetPackMap(_DEF_PACK_VEDIO_FRAME)       =   &Ckernal::slot_videoFrameRq;
}

//初始化配置文件
void Ckernal::initConfig()
{
     m_serverIp=_DEF_SERVERIP;
     //路径设置：.exe 同级目录下(applicationDirPath config.ini)的config.ini文件
     QString path = QApplication::applicationDirPath()+"/config.ini";
     //判断配置文件是否存在
     QFileInfo info(path);
     QSettings settings(path,QSettings::IniFormat,NULL);    //有打开，没有创建
     if( info.exists()){
         //加载配置文件，ip设置为配置文件中的ip
         //打开配置文件
         //移动到Net组
         settings.beginGroup("Net");
         //读取ip->addr->赋值
         QVariant ip=settings.value("ip");
         QString strIP=ip.toString();
         //结束
         settings.endGroup();
         if(!strIP.isEmpty()){
             m_serverIp=strIP;
         }
     }else{
        //没有配置文件，写入默认的ip
         settings.beginGroup("Net");
         settings.setValue("ip",m_serverIp);
         settings.endGroup();
     }
     //输出当前使用的IP地址
     qDebug()<<"ServerIp:"<<m_serverIp;
}

Ckernal::Ckernal(QObject *parent) : QObject(parent),
    m_id(0),m_roomId(0)
{
    qDebug()<<__func__;
    qDebug()<<"main threadID:"<< QThread::currentThreadId();
    //初始化核心类属性
    memset(m_name,0,sizeof(m_name));

    //加载配置文件
    this->initConfig();

    //设置函数协议映射表
    this->setNetPackMap();

    //添加注册登录界面类
    m_pLogin=new LoginDialog;
    //绑定发送用户登录信息的信号和处理用户登录信息的槽函数
    connect(m_pLogin,SIGNAL(SIG_loginCommit(QString,QString)),
            this,SLOT(slot_loginCommit(QString,QString)));
    connect(m_pLogin,SIGNAL(SIG_registerCommit(QString,QString,QString)),
            this,SLOT(slot_registerCommit(QString,QString,QString)));
    //绑定关闭登录注册界面发送的信号和槽函数
    connect(m_pLogin,SIGNAL(SIG_close()),
            this,SLOT(slot_destroy()));
    m_pLogin->show();

    //添加主界面类对象
    m_pVedio=new VedioConferenceDialog;
    //绑定主界面退出发出信号和自身回收主界面对象的槽函数
    connect(m_pVedio,SIGNAL(SIG_close()),
            this,SLOT(slot_destroy()));
    //绑定主界面点击加入会议按钮信号和对应处理槽函数
    connect(m_pVedio,SIGNAL(SIG_joinRoom()),
            this,SLOT(slot_joinRoom()));
    //绑定主界面点击创建会议按钮信号和对应处理槽函数
    connect(m_pVedio,SIGNAL(SIG_createRoom()),
            this,SLOT(slot_createRoom()));

    //添加音频类接收类对象
    m_pAudio_Read=new Audio_Read;
    //绑定音频接收信号和音频处理槽函数
    connect(m_pAudio_Read,SIGNAL(SIG_audioFrame(QByteArray)),
            this,SLOT(slot_audioFrame(QByteArray)));

    //添加视频类采集类对象
    m_pVedioRead=new VideoRead;
    //绑定发送视频帧的信号和对应的槽函数
    connect(m_pVedioRead,SIGNAL(SIG_sendVedioFrame(QImage)),
            this,SLOT(slot_sendVedioFrame(QImage)));

    //创建发送视频帧的线程对象
    m_pSendVedioWorker=QSharedPointer<SendVedioWorker>(new SendVedioWorker);
    //绑定多线程处理视频帧信号和对应的槽函数
    connect(this,SIGNAL(SIG_sendVideo(char*,int)),
            m_pSendVedioWorker.data(),SLOT(slot_dowork(char*,int)));

    //添加桌面共享类对象
    m_pSreenRead=new ScreenRead;
    //绑定发送桌面帧的信号和对应的槽函数
    connect(m_pSreenRead,SIGNAL(SIG_getScreenFrame(QImage)),
            this,SLOT(slot_sendVedioFrame(QImage)));

    //添加房间类对象
    m_pRoomDialog=new RoomDialog;
    //绑定会议界面关闭发送的信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_close()),
            this,SLOT(slot_QuitRoom()));
    //绑定会议界面关闭音频信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_AudioPause()),
            this,SLOT(slot_AudioPause()));
    //绑定会议界面打开音频信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_AudioStart()),
            this,SLOT(slot_AudioStart()));
    //绑定会议界面关闭视频信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_VideoPause()),
            this,SLOT(slot_VideoPause()));
    //绑定会议界面打开视频信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_VideoStart()),
            this,SLOT(slot_VideoStart()));
    //绑定会议界面关闭桌面共享信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_ScreenPause()),
            this,SLOT(slot_ScreenPause()));
    //绑定会议界面打开桌面共享信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_ScreenStart()),
            this,SLOT(slot_ScreenStart()));
    //绑定会议界面修改萌拍特效类型的信号和对应的槽函数
    connect(m_pRoomDialog,SIGNAL(SIG_setMoji(int)),
            m_pVedioRead,SLOT(slot_setMoji(int)));

    //添加网络类
    m_pClient=new TcpClientMediator;
    //打开网络
    m_pClient->OpenNet(m_serverIp.toStdString().c_str(),_DEF_PORT);
    //连接客户端接受数据信号和处理接受数据的槽函数
    connect(m_pClient,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealData(uint,char*,int)));

    //创建音频/视频发送网络类对象
    for(int p=0;p<2;p++){
        //创建网络类对象
        m_pAVClient[p]=new TcpClientMediator;
        //打开网络
        m_pAVClient[p]->OpenNet(m_serverIp.toStdString().c_str(),_DEF_PORT);
        //连接客户端接受数据信号和处理接受数据的槽函数
        connect(m_pAVClient[p],SIGNAL(SIG_ReadyData(uint,char*,int)),
                this,SLOT(slot_dealData(uint,char*,int)));
    }
}



//系统资源回收函数
void Ckernal::slot_destroy()
{
    qDebug()<<__func__;
    //回收网络类资源
    if(m_pClient){
        m_pClient->CloseNet();
        delete m_pClient;
        m_pClient=nullptr;
    }
    //回收主界面资源
    if(m_pVedio){
        m_pVedio->hide();
        delete m_pVedio;
        m_pVedio=nullptr;
    }
    //回收登录注册界面资源
    if(m_pLogin){
        m_pLogin->hide();
        delete m_pLogin;
        m_pLogin=nullptr;
    }
    //回收接收音频类对象资源
    if(m_pAudio_Read){
        m_pAudio_Read->pause();
        delete m_pAudio_Read;
        m_pAudio_Read=nullptr;
    }
    //回收房间资源
    if(m_pRoomDialog){
        m_pRoomDialog->hide();
        delete m_pRoomDialog;
        m_pRoomDialog=nullptr;
    }

    //系统退出
    exit(0);
}

//客户端处理接收到的数据
void Ckernal::slot_dealData(uint sock, char *buf, int nLen)
{
    int type=*(int*)buf;
    if(type>=_DEF_PACK_BASE && type<_DEF_PACK_BASE+_DEF_PACK_COUNT){
        //取得协议头，根据协议映射关系，找到函数指针
        PFUN pf=NetPackMap(type);
        if(pf){
            (this->*pf)(sock,buf,nLen);
        }
    }
    delete[] buf;
}

//处理登录回复
void Ckernal::slot_dealLoginRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_LOGIN_RS* rs=(STRU_LOGIN_RS*)buf;
    //根据返回结果，得到不同信息
    switch(rs->result){
    case user_not_exist:
        QMessageBox::about(m_pLogin,"提示","登录失败，当前手机号未被注册");
        break;
    case password_error:
        QMessageBox::about(m_pLogin,"提示","注册失败，密码错误");
        break;
    case login_success:{
        //QString strName=QString("用户[%1]登录成功").arg(rs->m_name);
        //QMessageBox::about(m_pLogin,"提示",strName);
        //记录用户id，用户名称
        m_id=rs->userid;
        strcpy(m_name,rs->m_name);
        //初始化主界面用户信息
        m_pVedio->setInfo(m_name);
        //ui跳转
        m_pLogin->hide();
        m_pVedio->showNormal();

        //注册 视频和音频的fd
        STRU_VIDEO_REGISTER rq_video;
        rq_video.userid=m_id;
        STRU_AUDIO_REGISTER rq_audio;
        rq_audio.userid=m_id;

        //发送视频和音频的fd
        m_pAVClient[audio_client]->SendData(0,(char*)&rq_audio,sizeof(rq_audio));
        m_pAVClient[video_client]->SendData(0,(char*)&rq_video,sizeof(rq_video));
        }
        break;
    default:
        break;
    }
}

//处理注册回复
void Ckernal::slot_dealRegisterRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_REGISTER_RS* rs=(STRU_REGISTER_RS*)buf;
    //根据不同的结果，弹出不同的提示窗口
    switch(rs->result){
    //电话号已存在
    case tel_is_exist:
        QMessageBox::about(m_pLogin,"提示","注册失败，当前手机号已被注册");
        break;
    //昵称存在
    case name_is_exist:
        QMessageBox::about(m_pLogin,"提示","注册失败，当前昵称已被注册");
        break;
    //注册成功
    case register_success:
        QMessageBox::about(m_pLogin,"提示","注册成功");
        break;
    default:
        break;
    }
}

//处理创建房间回复
void Ckernal::slot_dealCreateRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
     STRU_CREATEROOM_RS* rs=(STRU_CREATEROOM_RS*)buf;
    //获取房间号 显示到界面 跳转
    this->m_pRoomDialog->slot_setInfo(QString::number(rs->m_RoomId));
    //将自己的信息放在房间中
     //创建用户信息控件
    UserShow* user=new UserShow;
    //绑定点击用户信息控件信号和对应的槽函数
    connect(user,SIGNAL(SIG_itemClicked(int,QString)),
            m_pRoomDialog,SLOT(slot_setBigImgId(int,QString)));
    //初始化用户信息控件
    user->slot_setInfo(m_id,m_name);
    //将用户信息控件添加到房间界面上
    m_pRoomDialog->slot_addUserInfo(user);
    //核心类存储用户的房间号
    m_roomId=rs->m_RoomId;
    //显示会议界面
    m_pRoomDialog->showNormal();
    //音频设置 初始化
    m_pRoomDialog->slot_setAudioSet(false);
    //视频设置 初始化
    m_pRoomDialog->slot_setvideoSet(false);
}

//处理加入房间回复
void Ckernal::slot_dealJoinRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_JOINROOM_RS* rs=(STRU_JOINROOM_RS*)buf;
    //2、结果处理
    //失败：提示
    if(rs->m_lResult==room_isnot_exist){
        QString res=QString("加入房间失败，房间%1不存在").arg(rs->m_roomId);
        QMessageBox::about(m_pVedio,"提示",res);
        return;
    }else if(rs->m_lResult==join_success){
        //成功：跳转
        //获取房间号 显示到界面 跳转
        this->m_pRoomDialog->slot_setInfo(QString::number(rs->m_roomId));
        //将自己的信息放在房间中 todo
        m_roomId=rs->m_roomId;
        //显示会议界面
        m_pRoomDialog->showNormal();
    }
    //音频设置 初始化
    m_pRoomDialog->slot_setAudioSet(false);
    //视频设置 初始化
    m_pRoomDialog->slot_setvideoSet(false);
}

//处理用户信息请求
void Ckernal::slot_dealUserInfoRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_ROOM_MEMBER_RQ* rq=(STRU_ROOM_MEMBER_RQ*)buf;
    //2、创建用户对应的控件
    UserShow* user=new UserShow;
    //绑定点击用户信息控件信号和对应的槽函数
    connect(user,SIGNAL(SIG_itemClicked(int,QString)),
            m_pRoomDialog,SLOT(slot_setBigImgId(int,QString)));
    //初始化用户信息控件
    user->slot_setInfo(rq->m_userId,QString::fromStdString(rq->m_userName));
    //将用户信息控件添加到房间界面上
    m_pRoomDialog->slot_addUserInfo(user);
    //音频的内容
    //创建加入用户对应的音频播放类对象
    Audio_Write* aw=nullptr;
    //判断该用户的音频播放类对象是否存在
    if(m_mapIdToAudioWrite.count(rq->m_userId)==0){
        aw=new Audio_Write;
        m_mapIdToAudioWrite[rq->m_userId]=aw;
    }
    //视频的内容

}

//处理用户退出房间请求
void Ckernal::slot_dealQuitRoomRq(uint sock, char *buf, int nLen)
{
    //1、拆包
    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)buf;

    //2、将对应用户的小界面控件从自己的房间UI中移除
    if(rq->RoomId==m_roomId){
        this->m_pRoomDialog->slot_removeUserShow(rq->m_nUserId);
    }
    //3、去除退出房间用户的音频播放类对象
    if(m_mapIdToAudioWrite.count(rq->m_nUserId)>0){
        Audio_Write* pAw=m_mapIdToAudioWrite[rq->m_nUserId];
        m_mapIdToAudioWrite.erase(rq->m_nUserId);
        delete pAw;
    }
}

//处理用户登录请求
void Ckernal::slot_loginCommit(QString tel, QString password)
{
    qDebug()<<__func__;
    //1、登录信息封包
    std::string strTel=tel.toStdString();
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    //md5密文
    std::string strPassMD5 = GetMD5(password);
    strcpy(rq.password,strPassMD5.c_str());

    //2、发送登录信息
    this->m_pClient->SendData(0,(char*)&rq,sizeof(rq));
}
//处理用户注册请求
void Ckernal::slot_registerCommit(QString tel, QString password, QString name)
{
    qDebug()<<__func__;
    //1、注册信息封包
    std::string strTel=tel.toStdString();
    STRU_REGISTER_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    //md5密文
    std::string strPassMD5 = GetMD5(password);
    strcpy(rq.password,strPassMD5.c_str());

    //兼容中文utf8 QString-->std::string-->char*
    strcpy(rq.name,name.toStdString().c_str());

    //2、发送注册信息
    this->m_pClient->SendData(0,(char*)&rq,sizeof(rq));
}

//处理接收到的音频帧数据
void Ckernal::slot_audioFrameRq(uint sock,char* buf,int nLen)
{
    qDebug()<<__func__;
    //拆包
    //反序列化：提取用户ID，房间号
    char* tmp=buf;
    //跳过类型
    tmp+=sizeof(int);
    //获取用户ID
    int userId=*(int*)tmp;
    tmp+=sizeof(int);
    //获取房间ID
    int roomId=*(int*)tmp;
    tmp+=sizeof(int);
    //跳过分：秒：毫秒
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    //获取音频帧数据
    int nbuflen=nLen-6*sizeof(int);
    QByteArray ba(tmp,nbuflen);
    //判断房间ID是否是自己所在房间的ID
    if(roomId==m_roomId){
        //判断用户ID对应的用户是否在房间中
        if(m_mapIdToAudioWrite.count(userId)>0){
            Audio_Write* aw=m_mapIdToAudioWrite[userId];
            //播放音频
            aw->slot_net_rx(ba);
        }
    }
}

//处理接收到的视频数据
void Ckernal::slot_videoFrameRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    //反序列化：提取用户ID，房间号
    char* tmp=buf;
    //跳过类型
    tmp+=sizeof(int);
    //获取用户ID
    int userId=*(int*)tmp;
    tmp+=sizeof(int);
    //获取房间ID
    int roomId=*(int*)tmp;
    tmp+=sizeof(int);
    //跳过分：秒：毫秒
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    //获取视频帧数据
    int dataLen=nLen-6*sizeof (int);
    QByteArray bt(tmp,dataLen);
    QImage img;
    img.loadFromData(bt);
    //判断房间ID是否是自己所在房间的ID
    if(roomId==m_roomId){
        //更新对应用户的图像
        m_pRoomDialog->slot_refreshUser(userId,img);
    }
}


/*音视频数据帧
成员描述
int type;
int userId;
int roomId;
int min;
int sec;
int msec;
QByteArray audioFrame;
*/
//发送音频帧
void Ckernal::slot_audioFrame(QByteArray ba)
{
    qDebug()<<__func__;
    //计算数据包长度
    int nPackSize=6*sizeof(int)+ba.size();
    //申请空间
    char* buf=new char[nPackSize];
    char* tail=buf;
    //获取原始数据
    int type=_DEF_PACK_AUDIO_FRAME;
    int userId=m_id;
    int roomId=m_roomId;
    QTime tm=QTime::currentTime();
    int min=tm.minute();
    int sec=tm.second();
    int msec=tm.msec();
    //序列化操作
    *(int*)tail=type;
    tail+=sizeof(int);
    *(int*)tail=userId;
    tail+=sizeof(int);
    *(int*)tail=roomId;
    tail+=sizeof(int);
    *(int*)tail=min;
    tail+=sizeof(int);
    *(int*)tail=sec;
    tail+=sizeof(int);
    *(int*)tail=msec;
    tail+=sizeof(int);
    memcpy(tail,ba.data(),ba.size());
    //发送音频帧数据包
    m_pAVClient[audio_client]->SendData(0,buf,nPackSize);
    //回收申请的空间
    delete[] buf;
    buf=nullptr;
}

/*视频数据帧
 * 成员描述
 * int type;
 * int userId;
 * int roomId;
 * int min;
 * int sec;
 * int msec;
 * QByteArray videoFrame;
*/
//发送视频帧
void Ckernal::slot_sendVedioFrame(QImage img)
{
    qDebug()<<__func__;
    //显示视频帧
    slot_refreshVideo(m_id,img);
    //压缩图片从RGB24格式压缩到JPEG格式, 发送出去
    QByteArray ba;
    QBuffer qbuf(&ba); // QBuffer 与 QByteArray 字节数组联立联系
    qbuf.open(QIODevice::WriteOnly);
    img.save( &qbuf , "JPEG" , 50 );  //将图片的数据写入 ba
    qbuf.close();
    //使用ba对象, 可以获取图片对应的缓冲区
    //可以使用ba.data() , ba.size()将缓冲区发送出去

    //写视频帧数据包 发送
    int nPackSize=6*sizeof(int)+ba.size();
    char *buf=new char[nPackSize];
    char* tmp=buf;
    *(int*)tmp=_DEF_PACK_VEDIO_FRAME;
    tmp+=sizeof(int);
    *(int*)tmp=m_id;
    tmp+=sizeof(int);
    *(int*)tmp=m_roomId;
    tmp+=sizeof(int);
    //用于延迟过久，舍弃一些帧的参考时间
    QTime tm=QTime::currentTime();
    *(int*)tmp=tm.minute();
    tmp+=sizeof(int);
    *(int*)tmp=tm.second();
    tmp+=sizeof(int);
    *(int*)tmp=tm.msec();
    tmp+=sizeof(int);
    //写入视频帧
    memcpy(tmp,ba.data(),ba.size());

    //发送视频帧数据包
    /*优化:send() 阻塞函数 数据量大 滑动窗口变小 send()阻塞
    影响用户界面响应 出现卡顿问题

    解决方式：发送信号，在另一个线程中执行具体的发送和回收操作*/
//    m_pClient->SendData(0,buf,nPackSize);

//    //回收申请的额外空间
//    delete[] buf;
//    buf=nullptr;
    Q_EMIT SIG_sendVideo(buf,nPackSize);
}

//处理创建会议信号的槽函数
void Ckernal::slot_createRoom()
{
    qDebug()<<__func__;
    //判断是否在房间内
    if(m_roomId != 0){
        QMessageBox::about(m_pVedio,"提示","在房间内，无法创建新房间");
        return;
    }
    //封包，发送信息
    STRU_CREATEROOM_RQ rq;
    rq.m_userId=m_id;
    m_pClient->SendData(0,(char*)&rq,sizeof(rq));
}

//处理加入会议信号的槽函数
void Ckernal::slot_joinRoom()
{
    qDebug()<<__func__;
    //判断是否在房间内
    if(m_roomId != 0){
        QMessageBox::about(m_pVedio,"提示","在房间内，无法加入新房间");
        return;
    }
    //弹出窗口，获取房间号
    QString strRoomId=QInputDialog::getText(m_pVedio,"加入房间","请输入房间号");
    //合法性检测：房间号是8位以下的纯数字
    QRegExp exp("^[0-9]\{1,8\}$");
    if(!exp.exactMatch(strRoomId)){
        QMessageBox::about(m_pVedio,"提示","房间号不合法，房间号为8位以下纯数字");
    }
    //封包，发送信息
    STRU_JOINROOM_RQ rq;
    rq.m_RoomId=strRoomId.toInt();
    rq.m_UserId=m_id;
    m_pClient->SendData(0,(char*)&rq,sizeof(rq));
}

//处理用户退出房间信号的槽函数
void Ckernal::slot_QuitRoom()
{
    qDebug()<<__func__;
    //发送退出包
    STRU_LEAVEROOM_RQ rq;
    rq.RoomId=m_roomId;
    rq.m_nUserId=m_id;
   strcpy(rq.UserName,m_name);
    m_pClient->SendData(0,(char*)&rq,sizeof(rq));
    //关闭 音频
    m_pAudio_Read->pause();
    //音频设置复位
    m_pRoomDialog->slot_setAudioSet(false);
    //回收所有人的音频播放类对象
    for(auto ite=m_mapIdToAudioWrite.begin();ite!=m_mapIdToAudioWrite.end();){
        Audio_Write* aw=ite->second;
        ite=m_mapIdToAudioWrite.erase(ite);
        delete aw;
    }

    //关闭 视频
    m_pVedioRead->slot_closeVideo();
    //视频设置复位
    m_pRoomDialog->slot_setvideoSet(false);

    //关闭 屏幕共享
    m_pSreenRead->slot_closeVedio();
    //屏幕共享设置复位
    m_pRoomDialog->slot_setScreenSet(false);

    //修改核心类存储的用户信息
    m_roomId=0;
    //回收资源
    m_pRoomDialog->slot_clearUserShow();
}

//处理关闭音频信号的槽函数
void Ckernal::slot_AudioPause()
{
    qDebug()<<__func__;
    m_pAudio_Read->pause();
}

//处理打开音频信号的槽函数
void Ckernal::slot_AudioStart()
{
    qDebug()<<__func__;
    m_pAudio_Read->start();
}

//处理关闭视频信号的槽函数
void Ckernal::slot_VideoPause()
{
    qDebug()<<__func__;
    m_pVedioRead->slot_closeVideo();
}

//处理打开视频信号的槽函数
void Ckernal::slot_VideoStart()
{
    qDebug()<<__func__;
    m_pVedioRead->slot_openVideo();
}

//处理关闭桌面共享信号的槽函数
void Ckernal::slot_ScreenPause()
{
    qDebug()<<__func__;
    m_pSreenRead->slot_closeVedio();
}

//处理打开桌面共享信号的槽函数
void Ckernal::slot_ScreenStart()
{
    qDebug()<<__func__;
    m_pSreenRead->slot_openVedio();
}

//刷新显示的视频帧
void Ckernal::slot_refreshVideo(int id,QImage& img)
{
    qDebug()<<__func__;
    m_pRoomDialog->slot_refreshUser(id,img);
}

//多线程处理接收到的视频帧信号对应的槽函数
void Ckernal::slot_sendVideo(char *buf, int nPackSize)
{
    qDebug()<<__func__;
    //视频帧发送检测
    //获取视频帧发送时间
    char *tmp=buf;
    //跳转至记录发送时间的位置
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    tmp+=sizeof(int);
    //提取发送时间
    int min=*(int*)tmp;
    tmp+=sizeof(int);
    int sec=*(int*)tmp;
    tmp+=sizeof(int);
    int msec=*(int*)tmp;
    //获取当前时间
    QTime ctm=QTime::currentTime();
    //发送时间
    QTime tm(ctm.hour(),min,sec,msec);
    //发送数据包延迟超过 300 ms 丢弃
    if(tm.msecsTo(ctm)>300){
        qDebug()<<"Time out , send VedioFrame fail";
        delete[] buf;
        buf=nullptr;
        return;
    }
    //发送视频帧
    m_pAVClient[video_client]->SendData(0,buf,nPackSize);

    //回收申请的额外空间
    delete[] buf;
    buf=nullptr;
}
