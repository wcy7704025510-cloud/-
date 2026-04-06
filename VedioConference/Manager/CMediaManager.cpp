#include "CMediaManager.h"
#include "audio_read.h"
#include "videoread.h"
#include "screenread.h"
#include "audio_write.h"
#include "VideoWorker.h"
#include "CRoomManager.h"
#include "roomdialog.h"
#include "packdef.h"
#include <QTime>
#include <QBuffer>
#include <QDebug>

// 引入视频核武器库
#include "H264Encoder.h"
#include "H264Decoder.h"
#include "VideoCommon.h" // 获取 IMAGE_WIDTH, IMAGE_HEIGHT 和 FRAME_RATE

CMediaManager::CMediaManager(QObject *parent) : QObject(parent),
    m_pAudio_Read(nullptr), m_pVedioRead(nullptr), m_pSreenRead(nullptr),
    m_pRoomManager(nullptr), m_pRoomDialog(nullptr), m_id(0),
    m_pH264Encoder(nullptr) // 初始化编码器为空
{
}

CMediaManager::~CMediaManager()
{
    if(m_pAudio_Read){
        m_pAudio_Read->pause();
        delete m_pAudio_Read;
        m_pAudio_Read = nullptr;
    }

    // 释放专属编码器
    if(m_pH264Encoder) {
        delete m_pH264Encoder;
        m_pH264Encoder = nullptr;
    }

    // 清理可能遗留的解码器
    slot_clearDevices();
}

void CMediaManager::initDevices()
{
    // ==========================================
    // 1. 初始化音频采集设备 (Audio_Read)
    // ==========================================
    m_pAudio_Read = new Audio_Read;
    // 采集并被压缩后的极致小包，直接发给 Manager 进行网络封包
    connect(m_pAudio_Read, SIGNAL(SIG_audioFrame(QByteArray)), this, SLOT(slot_audioFrame(QByteArray)));

    // ==========================================
    // 2. 初始化视频采集设备 (VideoRead)
    // ==========================================
    m_pVedioRead = new VideoRead;
    connect(m_pVedioRead, SIGNAL(SIG_sendVedioFrame(QImage)), this, SLOT(slot_sendVedioFrame(QImage)));

    // ==========================================
    // 3. 初始化媒体发送工作线程 (MediaSendWorker)
    // ==========================================
    m_pMediaSendWorker = QSharedPointer<MediaSendWorker>(new MediaSendWorker(this));
    connect(this, SIGNAL(SIG_sendVideo(char*,int)), m_pMediaSendWorker.data(), SLOT(slot_dowork(char*,int)));

    // ==========================================
    // 4. 初始化屏幕共享设备 (ScreenRead)
    // ==========================================
    m_pSreenRead = new ScreenRead;
    connect(m_pSreenRead, SIGNAL(SIG_getScreenFrame(QImage)), this, SLOT(slot_sendVedioFrame(QImage)));

    // ==========================================
    // 5. 启动 H264 编码器
    // ==========================================
    m_pH264Encoder = new H264Encoder(this);
    m_pH264Encoder->initEncoder(IMAGE_WIDTH, IMAGE_HEIGHT, FRAME_RATE);
}

void CMediaManager::slot_setMoji(int type)
{
    if(m_pVedioRead) m_pVedioRead->slot_setMoji(type);
}

void CMediaManager::slot_clearDevices()
{
    qDebug()<<__func__;
    if(m_pAudio_Read) m_pAudio_Read->pause();
    if(m_pRoomDialog) m_pRoomDialog->slot_setAudioSet(false);

    if(m_pVedioRead) m_pVedioRead->slot_closeVideo();
    if(m_pRoomDialog) m_pRoomDialog->slot_setvideoSet(false);

    if(m_pSreenRead) m_pSreenRead->slot_closeVedio();
    if(m_pRoomDialog) m_pRoomDialog->slot_setScreenSet(false);

    // ==========================================
    // 彻底销毁清理所有其他人的专属解码器池！
    // ==========================================
    for(auto decoder : m_mapDecoders.values()){
        decoder->destroy();
        delete decoder;
    }
    m_mapDecoders.clear();
}

void CMediaManager::slot_AudioPause()
{
    qDebug()<<__func__;
    if(m_pAudio_Read) m_pAudio_Read->pause();
}

void CMediaManager::slot_AudioStart()
{
    qDebug()<<__func__;
    if(m_pAudio_Read) m_pAudio_Read->start();
}

void CMediaManager::slot_VideoPause()
{
    qDebug()<<__func__;
    if(m_pVedioRead) m_pVedioRead->slot_closeVideo();
}

void CMediaManager::slot_VideoStart()
{
    qDebug()<<__func__;
    if(m_pVedioRead) m_pVedioRead->slot_openVideo();
}

void CMediaManager::slot_ScreenPause()
{
    qDebug()<<__func__;
    if(m_pSreenRead) m_pSreenRead->slot_closeVedio();
}

void CMediaManager::slot_ScreenStart()
{
    qDebug()<<__func__;
    if(m_pSreenRead) m_pSreenRead->slot_openVedio();
}

void CMediaManager::slot_refreshVideo(int id, QImage& img)
{
    qDebug()<<__func__;
    if(m_pRoomDialog) m_pRoomDialog->slot_refreshUser(id,img);
}

void CMediaManager::slot_audioFrameRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;

    char* tmp = buf;
    tmp += sizeof(int);
    int userId = *(int*)tmp;
    tmp += sizeof(int);
    int roomId = *(int*)tmp;
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);

    int nbuflen = nLen - 6 * sizeof(int);
    QByteArray ba(tmp, nbuflen);

    if(m_pRoomManager && roomId == m_pRoomManager->getRoomId()){
        auto& audioMap = m_pRoomManager->getAudioWriteMap();
        if(audioMap.count(userId) > 0){
            Audio_Write* aw = audioMap[userId];
            aw->slot_net_rx(ba);
        }
    }
}

void CMediaManager::slot_videoFrameRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    char* tmp = buf;
    tmp += sizeof(int);
    int userId = *(int*)tmp;
    tmp += sizeof(int);
    int roomId = *(int*)tmp;
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);

    int dataLen = nLen - 6 * sizeof(int);
    QByteArray bt(tmp, dataLen);

    if(m_pRoomManager && roomId == m_pRoomManager->getRoomId()){
        // ==========================================
        // H.264 极限解压机制：为每一位好友分配专属解码器
        // ==========================================
        if (!m_mapDecoders.contains(userId)) {
            H264Decoder* decoder = new H264Decoder(this);
            decoder->initDecoder();
            m_mapDecoders[userId] = decoder;
        }

        QImage img;
        // 把微小的网络包丢给 FFmpeg 解码，解出完整画面立刻送给 OpenGL 显示
        if (m_mapDecoders[userId]->decode(bt, img)) {
            if(m_pRoomDialog) m_pRoomDialog->slot_refreshUser(userId, img);
        }
    }
}

void CMediaManager::slot_audioFrame(QByteArray ba)
{
    qDebug()<<__func__;
    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

    int nPackSize = 6 * sizeof(int) + ba.size();
    char* buf = new char[nPackSize];
    char* tail = buf;

    int type = _DEF_PACK_AUDIO_FRAME;
    int userId = m_id;
    int roomId = m_pRoomManager->getRoomId();
    QTime tm = QTime::currentTime();
    int min = tm.minute();
    int sec = tm.second();
    int msec = tm.msec();

    *(int*)tail = type; tail += sizeof(int);
    *(int*)tail = userId; tail += sizeof(int);
    *(int*)tail = roomId; tail += sizeof(int);
    *(int*)tail = min; tail += sizeof(int);
    *(int*)tail = sec; tail += sizeof(int);
    *(int*)tail = msec; tail += sizeof(int);
    memcpy(tail, ba.data(), ba.size());

    emit SIG_SendAudioData(buf, nPackSize);
    delete[] buf;
}

void CMediaManager::slot_sendVedioFrame(QImage img)
{
    qDebug()<<__func__;

    // 本地预览直接刷新
    slot_refreshVideo(m_id, img);

    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

    // ==========================================
    // H.264 极限压缩机制
    // ==========================================
    QByteArray ba = m_pH264Encoder->encode(img);

    // 如果编码器还在缓冲帧，或者没有产生数据包，就不发网络请求，节省带宽！
    if(ba.isEmpty()) return;

    // ---- 原始的封包逻辑保持不变 ----
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

void CMediaManager::slot_sendVideo(char *buf, int nPackSize)
{
    qDebug()<<__func__;
    char *tmp = buf;
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);
    int min = *(int*)tmp; tmp += sizeof(int);
    int sec = *(int*)tmp; tmp += sizeof(int);
    int msec = *(int*)tmp;


    emit SIG_SendVideoData(buf, nPackSize);
}
