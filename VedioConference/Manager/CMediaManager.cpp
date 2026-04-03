#include "CMediaManager.h"
#include "audio_read.h"
#include "videoread.h"
#include "screenread.h"
#include "audio_write.h"
#include "VideoWorker.h" // Includes MediaSendWorker now
#include "CRoomManager.h"
#include "roomdialog.h"
#include "packdef.h"
#include <QTime>
#include <QBuffer>
#include <QDebug>

CMediaManager::CMediaManager(QObject *parent) : QObject(parent),
    m_pAudio_Read(nullptr), m_pVedioRead(nullptr), m_pSreenRead(nullptr),
    m_pRoomManager(nullptr), m_pRoomDialog(nullptr), m_id(0)
{
}

CMediaManager::~CMediaManager()
{
    if(m_pAudio_Read){
        m_pAudio_Read->pause();
        delete m_pAudio_Read;
        m_pAudio_Read = nullptr;
    }
}

void CMediaManager::initDevices()
{
    m_pAudio_Read = new Audio_Read;
    connect(m_pAudio_Read, SIGNAL(SIG_audioFrame(QByteArray)), this, SLOT(slot_audioFrame(QByteArray)));

    m_pVedioRead = new VideoRead;
    connect(m_pVedioRead, SIGNAL(SIG_sendVedioFrame(QImage)), this, SLOT(slot_sendVedioFrame(QImage)));

    m_pMediaSendWorker = QSharedPointer<MediaSendWorker>(new MediaSendWorker(this));
    connect(this, SIGNAL(SIG_sendVideo(char*,int)), m_pMediaSendWorker.data(), SLOT(slot_dowork(char*,int)));

    m_pSreenRead = new ScreenRead;
    connect(m_pSreenRead, SIGNAL(SIG_getScreenFrame(QImage)), this, SLOT(slot_sendVedioFrame(QImage)));
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
    QImage img;
    img.loadFromData(bt);
    
    if(m_pRoomManager && roomId == m_pRoomManager->getRoomId()){
        if(m_pRoomDialog) m_pRoomDialog->slot_refreshUser(userId, img);
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
    slot_refreshVideo(m_id, img);
    
    QByteArray ba;
    QBuffer qbuf(&ba);
    qbuf.open(QIODevice::WriteOnly);
    img.save(&qbuf, "JPEG", 50);
    qbuf.close();
    
    if(!m_pRoomManager || m_pRoomManager->getRoomId() == 0) return;

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
    
    QTime ctm = QTime::currentTime();
    QTime tm(ctm.hour(), min, sec, msec);
    
    if(tm.msecsTo(ctm) > 300){
        qDebug()<<"Time out , send VedioFrame fail";
        delete[] buf;
        return;
    }
    emit SIG_SendVideoData(buf, nPackSize);

}
