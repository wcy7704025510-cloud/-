#include "CMediaManager.h"          // 媒体管理器头文件
#include "audio_read.h"            // 音频采集头文件
#include "videoread.h"            // 摄像头视频采集头文件
#include "screenread.h"           // 屏幕共享采集头文件
#include "audio_write.h"          // 音频播放头文件
#include "VideoWorker.h"          // 视频工作线程头文件
#include "CRoomManager.h"         // 房间管理器头文件
#include "roomdialog.h"           // 房间对话框头文件
#include "packdef.h"             // 协议包定义
#include <QTime>                 // Qt时间类，用于音视频同步时间戳
#include <QDebug>               // Qt调试输出

// H.264视频编解码库头文件
#include "H264Encoder.h"         // H.264编码器(将YUV/RGB编码为H.264)
#include "H264Decoder.h"        // H.264解码器(将H.264解码为YUV/RGB)
#include "VideoCommon.h"        // 视频公共配置(分辨率、帧率、码率等参数)

// CMediaManager构造函数
// 初始化所有成员变量指针为nullptr，防止野指针
// m_pAudio_Read=nullptr: 音频采集对象，initDevices时创建
// m_pVedioRead=nullptr: 摄像头采集对象，initDevices时创建
// m_pSreenRead=nullptr: 屏幕采集对象，initDevices时创建
// m_pH264Encoder=nullptr: 视频编码器，initDevices时创建
// m_pScreenEncoder=nullptr: 屏幕共享编码器，initDevices时创建
CMediaManager::CMediaManager(QObject *parent) : QObject(parent),
    m_pAudio_Read(nullptr), m_pVedioRead(nullptr), m_pSreenRead(nullptr),
    m_pRoomManager(nullptr), m_pRoomDialog(nullptr), m_id(0),
    m_pH264Encoder(nullptr), m_pScreenEncoder(nullptr)
{
}

// CMediaManager析构函数
// 释放所有媒体设备和资源，防止内存泄漏
CMediaManager::~CMediaManager()
{
    // 停止并释放音频采集对象
    if(m_pAudio_Read){
        m_pAudio_Read->pause();   // 先暂停采集，防止析构时还在写入
        delete m_pAudio_Read;
        m_pAudio_Read = nullptr;
    }

    // 释放视频编码器(摄像头)
    if(m_pH264Encoder) {
        delete m_pH264Encoder;
        m_pH264Encoder = nullptr;
    }

    // 释放屏幕共享编码器
    if(m_pScreenEncoder) {
        delete m_pScreenEncoder;
        m_pScreenEncoder = nullptr;
    }

    // 清理可能遗留的解码器池(其他用户的解码器)
    slot_clearDevices();
}

// initDevices - 初始化所有音视频设备
// 调用时机: 用户加入房间成功后，由Ckernel调用
// 流程: 创建采集对象 -> 连接信号槽 -> 初始化编码器
void CMediaManager::initDevices()
{
    // 1. 初始化音频采集设备
    // Audio_Read: 麦克风采集，负责PCM音频数据采集和Opus编码
    m_pAudio_Read = new Audio_Read;
    // 连接信号: 采集到音频帧 -> 发送处理
    // 原始PCM数据经过Opus压缩后发出，减少网络带宽占用
    connect(m_pAudio_Read, SIGNAL(SIG_audioFrame(QByteArray)), this, SLOT(slot_audioFrame(QByteArray)));

    // 2. 初始化视频采集设备
    // VideoRead: 摄像头采集，负责从摄像头获取视频帧
    m_pVedioRead = new VideoRead;
    // 连接信号: 采集到视频帧 -> H.264编码发送
    connect(m_pVedioRead, SIGNAL(SIG_sendVedioFrame(QImage)), this, SLOT(slot_sendVedioFrame(QImage)));

    // 3. 初始化媒体发送工作线程
    // MediaSendWorker: 独立工作线程，用于视频数据发送
    // 避免主线程阻塞，提高UI响应性
    m_pMediaSendWorker = QSharedPointer<MediaSendWorker>(new MediaSendWorker(this));
    // 连接信号: 发送视频数据 -> 工作线程处理
    connect(this, SIGNAL(SIG_sendVideo(char*,int)), m_pMediaSendWorker.data(), SLOT(slot_dowork(char*,int)));

    // 4. 初始化屏幕共享设备
    // ScreenRead: 屏幕采集，负责桌面屏幕捕获
    m_pSreenRead = new ScreenRead;
    // 连接信号: 采集到屏幕帧 -> 屏幕专用编码发送(高质量)
    connect(m_pSreenRead, SIGNAL(SIG_getScreenFrame(QImage)), this, SLOT(slot_sendScreenFrame(QImage)));

    // 5. 初始化视频H.264编码器(摄像头)
    // 参数: IMAGE_WIDTH×IMAGE_HEIGHT分辨率, FRAME_RATE帧率
    // 低分辨率(320×240)节省带宽，适合实时通话
    m_pH264Encoder = new H264Encoder(this);
    m_pH264Encoder->initEncoder(IMAGE_WIDTH, IMAGE_HEIGHT, FRAME_RATE);

    // 6. 初始化屏幕共享H.264编码器
    // 参数: SCREEN_WIDTH×SCREEN_HEIGHT分辨率, 更高帧率和码率
    // 屏幕共享需要更高质量，SCREEN_BIT_RATE=2Mbps
    m_pScreenEncoder = new H264Encoder(this);
    m_pScreenEncoder->initEncoder(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FRAME_RATE, SCREEN_BIT_RATE);
}

// slot_clearDevices - 清理所有媒体设备
// 调用时机: 退出房间或程序退出时
// 功能: 停止采集、销毁编码器、清理解码器池
void CMediaManager::slot_clearDevices()
{
    qDebug()<<__func__;

    // 停止音频采集，更新UI状态
    if(m_pAudio_Read) m_pAudio_Read->pause();
    if(m_pRoomDialog) m_pRoomDialog->slot_setAudioSet(false);

    // 停止视频采集，关闭摄像头
    if(m_pVedioRead) m_pVedioRead->slot_closeVideo();
    if(m_pRoomDialog) m_pRoomDialog->slot_setvideoSet(false);

    // 停止屏幕采集
    if(m_pSreenRead) m_pSreenRead->slot_closeVedio();
    if(m_pRoomDialog) m_pRoomDialog->slot_setScreenSet(false);

    // 销毁所有远程用户的解码器
    // 每个解码器对应一个远程用户的视频流
    for(auto decoder : m_mapDecoders.values()){
        decoder->destroy();       // 销毁FFmpeg解码器上下文
        delete decoder;           // 释放对象内存
    }
    m_mapDecoders.clear();        // 清空map容器
}

// ============= 媒体控制槽函数 =============
// slot_AudioPause - 暂停音频采集
void CMediaManager::slot_AudioPause()
{
    qDebug()<<__func__;
    if(m_pAudio_Read) m_pAudio_Read->pause();
}

// slot_AudioStart - 恢复音频采集
void CMediaManager::slot_AudioStart()
{
    qDebug()<<__func__;
    if(m_pAudio_Read) m_pAudio_Read->start();
}

// slot_VideoPause - 暂停视频采集
void CMediaManager::slot_VideoPause()
{
    qDebug()<<__func__;
    if(m_pVedioRead) m_pVedioRead->slot_closeVideo();
}

// slot_VideoStart - 恢复视频采集
void CMediaManager::slot_VideoStart()
{
    qDebug()<<__func__;
    if(m_pVedioRead) m_pVedioRead->slot_openVideo();
}

// slot_ScreenPause - 暂停屏幕共享
void CMediaManager::slot_ScreenPause()
{
    qDebug()<<__func__;
    if(m_pSreenRead) m_pSreenRead->slot_closeVedio();
}

// slot_ScreenStart - 恢复屏幕共享
void CMediaManager::slot_ScreenStart()
{
    qDebug()<<__func__;
    if(m_pSreenRead) m_pSreenRead->slot_openVedio();
}

// slot_refreshVideo - 刷新用户视频画面
// 参数: id-用户ID, img-解码后的QImage
// 功能: 查找对应UserShow控件并更新显示
void CMediaManager::slot_refreshVideo(int id, QImage& img)
{
    qDebug()<<__func__;
    if(m_pRoomDialog) m_pRoomDialog->slot_refreshUser(id,img);
}

// ============= 网络数据接收处理 =============
// slot_audioFrameRq - 处理接收到的他人音频数据
// 参数: sock-socket标识, buf-音频数据包, nLen-数据长度
// 数据包格式: [type(4B)][userId(4B)][roomId(4B)][min(4B)][sec(4B)][msec(4B)][audioData...]
void CMediaManager::slot_audioFrameRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;

    char* tmp = buf;
    tmp += sizeof(int);                  // 跳过type(包类型)
    int userId = *(int*)tmp;             // 提取发送者用户ID
    tmp += sizeof(int);
    int roomId = *(int*)tmp;            // 提取房间ID
    tmp += sizeof(int);
    tmp += sizeof(int);                 // 跳过min
    tmp += sizeof(int);                 // 跳过sec
    tmp += sizeof(int);                 // 跳过msec

    // 计算音频数据长度和提取数据
    int nbuflen = nLen - 6 * sizeof(int);  // 总长度减去5个头部字段
    QByteArray ba(tmp, nbuflen);           // 构建QByteArray供SDL播放

    // 判断是否为本房间用户的数据
    if(m_pRoomManager && roomId == m_pRoomManager->getRoomId()){
        // 从房间管理器获取用户ID到Audio_Write的映射
        auto& audioMap = m_pRoomManager->getAudioWriteMap();
        if(audioMap.count(userId) > 0){
            Audio_Write* aw = audioMap[userId]; // 获取该用户的音频播放对象
            aw->slot_net_rx(ba);                // 发送网络音频数据到播放队列
        }
    }
}

// slot_videoFrameRq - 处理接收到的他人视频数据
// 参数: sock-socket标识, buf-视频数据包(H.264), nLen-数据长度
// 数据包格式: [type(4B)][userId(4B)][roomId(4B)][min(4B)][sec(4B)][msec(4B)][h264Data...]
void CMediaManager::slot_videoFrameRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;

    char* tmp = buf;
    tmp += sizeof(int);                  // 跳过type
    int userId = *(int*)tmp;             // 提取发送者用户ID
    tmp += sizeof(int);
    int roomId = *(int*)tmp;            // 提取房间ID
    tmp += sizeof(int);
    tmp += sizeof(int);                 // 跳过min
    tmp += sizeof(int);                 // 跳过sec
    tmp += sizeof(int);                 // 跳过msec

    // 计算视频数据长度和提取数据
    int dataLen = nLen - 6 * sizeof(int);
    QByteArray bt(tmp, dataLen);        // H.264裸流数据

    // 判断是否为本房间用户的数据
    if(m_pRoomManager && roomId == m_pRoomManager->getRoomId()){
        // 为每个远程用户创建独立的H.264解码器
        // 原因: 多路视频流需要独立解码上下文，避免状态干扰
        if (!m_mapDecoders.contains(userId)) {
            H264Decoder* decoder = new H264Decoder(this);
            decoder->initDecoder();    // 初始化FFmpeg解码器上下文
            m_mapDecoders[userId] = decoder; // 存入解码器池
        }

        QImage img;                    // 解码后的图像
        // H.264解码: 将H.264码流解码为QImage
        if (m_mapDecoders[userId]->decode(bt, img)) {
            // 解码成功，发送给UI显示
            if(m_pRoomDialog) m_pRoomDialog->slot_refreshUser(userId, img);
        }
    }
}

// ============= 本地采集数据处理 =============
// slot_audioFrame - 处理本地采集的音频数据
// 参数: ba-Opus编码后的音频数据
// 功能: 封装协议包，发送到网络
void CMediaManager::slot_audioFrame(QByteArray ba)
{
    qDebug()<<__func__;

    // 检查是否在房间中(不在房间不发送数据)
    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

    // 构建数据包: 5个int头部(20字节) + 音频数据
    int nPackSize = 6 * sizeof(int) + ba.size(); // 6个int: type+userId+roomId+min+sec+msec
    char* buf = new char[nPackSize];
    char* tail = buf;

    // 填充包头信息
    int type = _DEF_PACK_AUDIO_FRAME;   // 包类型: 音频帧
    int userId = m_id;                  // 当前用户ID
    int roomId = m_pRoomManager->getRoomId(); // 当前房间ID
    QTime tm = QTime::currentTime();   // 获取当前时间用于音视频同步
    int min = tm.minute();
    int sec = tm.second();
    int msec = tm.msec();

    // 写入各字段
    *(int*)tail = type; tail += sizeof(int);
    *(int*)tail = userId; tail += sizeof(int);
    *(int*)tail = roomId; tail += sizeof(int);
    *(int*)tail = min; tail += sizeof(int);
    *(int*)tail = sec; tail += sizeof(int);
    *(int*)tail = msec; tail += sizeof(int);
    // 复制音频数据
    memcpy(tail, ba.data(), ba.size());

    // 发送到音频网络通道
    emit SIG_SendAudioData(buf, nPackSize);
    delete[] buf;                        // 释放临时缓冲区
}

// slot_sendVedioFrame - 处理本地摄像头视频帧
// 参数: img-RGB格式视频帧
// 功能: 编码 -> 封包 -> 发送
void CMediaManager::slot_sendVedioFrame(QImage img)
{
    qDebug()<<__func__;

    // 本地预览: 将自己的视频帧直接显示在自己的窗口中
    slot_refreshVideo(m_id, img);

    // 检查是否在房间中
    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

    // H.264编码: 将RGB图像编码为H.264码流
    // 低分辨率(320×240)、低码率(400Kbps)节省带宽
    QByteArray ba = m_pH264Encoder->encode(img);

    // 编码器内部有缓冲，可能还没有输出(等待关键帧)
    // 空数据不发送，避免无效网络请求
    if(ba.isEmpty()) return;

    // 封包并发送
    int nPackSize = 6 * sizeof(int) + ba.size();
    char *buf = new char[nPackSize];
    char* tmp = buf;

    *(int*)tmp = _DEF_PACK_VEDIO_FRAME; tmp += sizeof(int);
    *(int*)tmp = m_id; tmp += sizeof(int);
    *(int*)tmp = m_pRoomManager->getRoomId(); tmp += sizeof(int);

    QTime tm = QTime::currentTime();
    *(int*)tmp = tm.minute(); tmp += sizeof(int);
    *(int*)tmp = tm.second(); tmp += sizeof(int);
    *(int*)tmp = tm.msec(); tmp += sizeof(int);

    memcpy(tmp, ba.data(), ba.size());

    // 发送信号触发MediaSendWorker发送
    Q_EMIT SIG_sendVideo(buf, nPackSize);
}

// slot_sendScreenFrame - 处理本地屏幕共享视频帧
// 参数: img-屏幕截图
// 功能: 高质量编码 -> 封包 -> 发送
void CMediaManager::slot_sendScreenFrame(QImage img)
{
    qDebug()<<__func__;

    // 本地预览
    slot_refreshVideo(m_id, img);

    // 检查是否在房间中
    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

    // 屏幕共享使用独立的高质量编码器
    // 高分辨率(960×720)、高码率(2Mbps)保证清晰度
    QByteArray ba = m_pScreenEncoder->encode(img);

    if(ba.isEmpty()) return;

    // 封包并发送
    int nPackSize = 6 * sizeof(int) + ba.size();
    char *buf = new char[nPackSize];
    char* tmp = buf;

    *(int*)tmp = _DEF_PACK_VEDIO_FRAME; tmp += sizeof(int);
    *(int*)tmp = m_id; tmp += sizeof(int);
    *(int*)tmp = m_pRoomManager->getRoomId(); tmp += sizeof(int);

    QTime tm = QTime::currentTime();
    *(int*)tmp = tm.minute(); tmp += sizeof(int);
    *(int*)tmp = tm.second(); tmp += sizeof(int);
    *(int*)tmp = tm.msec(); tmp += sizeof(int);

    memcpy(tmp, ba.data(), ba.size());

    Q_EMIT SIG_sendVideo(buf, nPackSize);
}

// slot_sendVideo - 工作线程处理视频发送
// 参数: buf-视频数据包, nPackSize-数据长度
// 功能: 提取时间戳，发送到视频网络通道
void CMediaManager::slot_sendVideo(char *buf, int nPackSize)
{
    qDebug()<<__func__;

    char *tmp = buf;
    tmp += sizeof(int);                 // 跳过type
    tmp += sizeof(int);                 // 跳过userId
    tmp += sizeof(int);                 // 跳过roomId
    // 提取时间戳用于音视频同步
    int min = *(int*)tmp; tmp += sizeof(int);
    int sec = *(int*)tmp; tmp += sizeof(int);
    int msec = *(int*)tmp;

    // 发送到视频网络通道(独立于控制信令)
    emit SIG_SendVideoData(buf, nPackSize);
}
