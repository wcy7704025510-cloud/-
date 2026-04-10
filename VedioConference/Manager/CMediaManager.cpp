
#include "CMediaManager.h"
#include "audio_read.h"
#include "videoread.h"
#include "screenread.h"
#include "audio_write.h"
#include "AudioProcessor.h"
#include "AudioDecodeProcessor.h"
#include "VideoProcessor.h"
#include "ScreenProcessor.h"
#include "H264Encoder.h"
#include "H264Decoder.h"
#include "VideoCommon.h"
#include "../infrastructure/concurrency/AudioDataQueue.h"
#include "../infrastructure/concurrency/VideoDataQueue.h"
#include "../infrastructure/concurrency/VideoDecodeDataQueue.h"
#include "../infrastructure/concurrency/AudioDecodeDataQueue.h"
#include "../infrastructure/concurrency/PCMPlayQueue.h"
#include "VideoDecodeProcessor.h"
#include "CRoomManager.h"
#include "roomdialog.h"
#include "packdef.h"
#include "INetMediator.h"
#include <QMetaObject>
#include <QTime>
#include <QDebug>
#include <QThread>

CMediaManager::CMediaManager(QObject *parent)
    : QObject(parent)
    , m_audioDecodeQueue(nullptr)
    , m_pcmPlayQueue(nullptr)
    , m_audioDecodeProcessor(nullptr)
    , m_audioDecodeThread(nullptr)
    , m_audioQueue(nullptr)
    , m_audioRead(nullptr)
    , m_audioProcessor(nullptr)
    , m_audioThread(nullptr)
    , m_videoQueue(nullptr)
    , m_videoRead(nullptr)
    , m_videoProcessor(nullptr)
    , m_videoThread(nullptr)
    , m_screenQueue(nullptr)
    , m_screenRead(nullptr)
    , m_screenProcessor(nullptr)
    , m_screenThread(nullptr)
    , m_videoEncoder(nullptr)
    , m_screenEncoder(nullptr)
    , m_audioClient(nullptr)
    , m_videoClient(nullptr)
    , m_pRoomManager(nullptr)
    , m_pRoomDialog(nullptr)
    , m_id(0)
{
}

CMediaManager::~CMediaManager()
{
    slot_clearDevices();
}

void CMediaManager::slot_startMediaEngines()
{
    qDebug() << "开始装配并启动音视频流水线...";

    if (m_audioThread != nullptr || m_videoThread != nullptr || m_screenThread != nullptr) {
        qDebug() << "检测到已有线程，先清理再重建...";
        slot_clearDevices();
    }

    setupAudioReceivePipeline();
    setupAudioCapturePipeline();
    setupVideoCapturePipeline();
    setupScreenCapturePipeline();

    qDebug() << "音视频流水线装配完毕，引擎点火！";
}

void CMediaManager::initDevices()
{
    setupAudioReceivePipeline();
    setupAudioCapturePipeline();
    setupVideoCapturePipeline();
    setupScreenCapturePipeline();
}

void CMediaManager::setupAudioReceivePipeline()
{
    m_audioDecodeQueue = new AudioDecodeDataQueue(this);
    m_pcmPlayQueue = new PCMPlayQueue(this);
    m_audioDecodeProcessor = new AudioDecodeProcessor(nullptr);
    m_audioDecodeThread = new QThread(this);

    m_audioDecodeProcessor->setInputQueue(m_audioDecodeQueue);
    m_audioDecodeProcessor->setOutputQueue(m_pcmPlayQueue);

    m_audioDecodeProcessor->moveToThread(m_audioDecodeThread);

    m_audioDecodeThread->start();

    QTimer::singleShot(0, m_audioDecodeProcessor, SLOT(slot_start()));
}

void CMediaManager::setupAudioCapturePipeline()
{
    m_audioQueue = new AudioDataQueue(this);
    m_audioRead = new Audio_Read(this);

    m_audioProcessor = new AudioProcessor(nullptr);
    m_audioThread = new QThread(this);

    m_audioRead->setQueue(m_audioQueue);
    m_audioProcessor->setQueue(m_audioQueue);

    m_audioProcessor->setSendCallback(
        [this](char* rawData, int rawLen) {
            this->audioSendCallback(rawData, rawLen);
        }
    );

    m_audioProcessor->moveToThread(m_audioThread);

    m_audioThread->start();

    QTimer::singleShot(0, m_audioProcessor, SLOT(slot_start()));
}

void CMediaManager::setupVideoCapturePipeline()
{
    m_videoQueue = new VideoDataQueue(this);
    m_videoRead = new VideoRead(this);
    m_videoEncoder = new H264Encoder(nullptr);

    m_videoProcessor = new VideoProcessor(nullptr);
    m_videoThread = new QThread(this);

    m_videoRead->setQueue(m_videoQueue);
    m_videoProcessor->setQueue(m_videoQueue);
    m_videoProcessor->setEncoder(m_videoEncoder);

    connect(m_videoRead, SIGNAL(SIG_sendVedioFrame(QImage)),
            this, SLOT(slot_localVideoPreview(QImage)));

    m_videoEncoder->initEncoder(IMAGE_WIDTH, IMAGE_HEIGHT, FRAME_RATE);

    m_videoProcessor->setSendCallback(
        [this](char* rawData, int rawLen) {
            this->videoSendCallback(rawData, rawLen);
        }
    );

    m_videoProcessor->moveToThread(m_videoThread);

    m_videoThread->start();

    QTimer::singleShot(0, m_videoProcessor, SLOT(slot_start()));
}

void CMediaManager::setupScreenCapturePipeline()
{
    m_screenQueue = new VideoDataQueue(this);
    m_screenRead = new ScreenRead(this);
    m_screenEncoder = new H264Encoder(nullptr);

    m_screenProcessor = new ScreenProcessor(nullptr);
    m_screenThread = new QThread(this);

    m_screenRead->setQueue(m_screenQueue);
    m_screenProcessor->setQueue(m_screenQueue);
    m_screenProcessor->setEncoder(m_screenEncoder);

    connect(m_screenRead, SIGNAL(SIG_getScreenFrame(QImage)),
            this, SLOT(slot_localVideoPreview(QImage)));

    m_screenEncoder->initEncoder(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FRAME_RATE, SCREEN_BIT_RATE);

    m_screenProcessor->setSendCallback(
        [this](char* rawData, int rawLen) {
            this->screenSendCallback(rawData, rawLen);
        }
    );

    m_screenProcessor->moveToThread(m_screenThread);

    m_screenThread->start();

    QTimer::singleShot(0, m_screenProcessor, SLOT(slot_start()));
}

void CMediaManager::audioSendCallback(char* rawData, int rawLen)
{
    if (!m_pRoomManager || m_pRoomManager->getRoomId() == 0) {
        return;
    }

    int totalLen = 6 * sizeof(int) + rawLen;
    char* packet = new char[totalLen];
    char* p = packet;

    *(int*)p = _DEF_PACK_AUDIO_FRAME; p += sizeof(int);
    *(int*)p = m_id; p += sizeof(int);
    *(int*)p = m_pRoomManager->getRoomId(); p += sizeof(int);

    QTime tm = QTime::currentTime();
    *(int*)p = tm.minute(); p += sizeof(int);
    *(int*)p = tm.second(); p += sizeof(int);
    *(int*)p = tm.msec(); p += sizeof(int);

    memcpy(p, rawData, rawLen);

    if (m_audioClient) {
        m_audioClient->SendData(0, packet, totalLen);
    }

    delete[] packet;
}

void CMediaManager::videoSendCallback(char* rawData, int rawLen)
{
    if (!m_pRoomManager || m_pRoomManager->getRoomId() == 0) {
        return;
    }

    int totalLen = 6 * sizeof(int) + rawLen;
    char* packet = new char[totalLen];
    char* p = packet;

    *(int*)p = _DEF_PACK_VEDIO_FRAME; p += sizeof(int);
    *(int*)p = m_id; p += sizeof(int);
    *(int*)p = m_pRoomManager->getRoomId(); p += sizeof(int);

    QTime tm = QTime::currentTime();
    *(int*)p = tm.minute(); p += sizeof(int);
    *(int*)p = tm.second(); p += sizeof(int);
    *(int*)p = tm.msec(); p += sizeof(int);

    memcpy(p, rawData, rawLen);

    if (m_videoClient) {
        m_videoClient->SendData(0, packet, totalLen);
    }

    delete[] packet;
}

void CMediaManager::screenSendCallback(char* rawData, int rawLen)
{
    if (!m_pRoomManager || m_pRoomManager->getRoomId() == 0) {
        return;
    }

    int totalLen = 6 * sizeof(int) + rawLen;
    char* packet = new char[totalLen];
    char* p = packet;

    *(int*)p = _DEF_PACK_VEDIO_FRAME; p += sizeof(int);
    *(int*)p = m_id; p += sizeof(int);
    *(int*)p = m_pRoomManager->getRoomId(); p += sizeof(int);

    QTime tm = QTime::currentTime();
    *(int*)p = tm.minute(); p += sizeof(int);
    *(int*)p = tm.second(); p += sizeof(int);
    *(int*)p = tm.msec(); p += sizeof(int);

    memcpy(p, rawData, rawLen);

    if (m_videoClient) {
        m_videoClient->SendData(0, packet, totalLen);
    }

    delete[] packet;
}

void CMediaManager::slot_AudioPause()
{
    qDebug() << __func__;
    if (m_audioRead) {
        m_audioRead->pause();
    }
}

void CMediaManager::slot_AudioStart()
{
    qDebug() << __func__;
    if (m_audioRead) {
        m_audioRead->start();
    }
}

void CMediaManager::slot_VideoPause()
{
    qDebug() << __func__;
    if (m_videoRead) {
        m_videoRead->slot_closeVideo();
    }
}

void CMediaManager::slot_VideoStart()
{
    qDebug() << __func__;
    if (m_videoRead) {
        m_videoRead->slot_openVideo();
    }
}

void CMediaManager::slot_ScreenPause()
{
    qDebug() << __func__;
    if (m_screenRead) {
        m_screenRead->slot_closeVedio();
    }
}

void CMediaManager::slot_ScreenStart()
{
    qDebug() << __func__;
    if (m_screenRead) {
        m_screenRead->slot_openVedio();
    }
}

void CMediaManager::slot_refreshVideo(int id, QImage& img)
{
    if (m_pRoomDialog) {
        m_pRoomDialog->slot_refreshUser(id, img);
    }
}

void CMediaManager::slot_localVideoPreview(QImage img)
{
    slot_refreshVideo(m_id, img);
}

void CMediaManager::slot_clearDevices()
{
    qDebug() << "开始清理音视频设备资源...";

    if (m_audioRead)  m_audioRead->pause();
    if (m_videoRead)  m_videoRead->slot_closeVideo();
    if (m_screenRead) m_screenRead->slot_closeVedio();

    if (m_audioProcessor)  m_audioProcessor->slot_stop();
    if (m_audioDecodeProcessor) m_audioDecodeProcessor->slot_stop();
    if (m_videoProcessor) m_videoProcessor->slot_stop();
    if (m_screenProcessor) m_screenProcessor->slot_stop();

    if (m_audioQueue)  m_audioQueue->stop();
    if (m_audioDecodeQueue) m_audioDecodeQueue->stop();
    if (m_videoQueue) m_videoQueue->stop();
    if (m_screenQueue) m_screenQueue->stop();

    if (m_audioThread) { m_audioThread->quit(); m_audioThread->wait(); }
    if (m_audioDecodeThread) { m_audioDecodeThread->quit(); m_audioDecodeThread->wait(); }
    if (m_videoThread) { m_videoThread->quit(); m_videoThread->wait(); }
    if (m_screenThread) { m_screenThread->quit(); m_screenThread->wait(); }

    if (m_audioProcessor)  { delete m_audioProcessor; m_audioProcessor = nullptr; }
    if (m_audioDecodeProcessor) { delete m_audioDecodeProcessor; m_audioDecodeProcessor = nullptr; }
    if (m_videoProcessor)  { delete m_videoProcessor; m_videoProcessor = nullptr; }
    if (m_screenProcessor) { delete m_screenProcessor; m_screenProcessor = nullptr; }

    if (m_audioThread)  { delete m_audioThread; m_audioThread = nullptr; }
    if (m_audioDecodeThread) { delete m_audioDecodeThread; m_audioDecodeThread = nullptr; }
    if (m_videoThread)  { delete m_videoThread; m_videoThread = nullptr; }
    if (m_screenThread) { delete m_screenThread; m_screenThread = nullptr; }

    if (m_audioRead)  { delete m_audioRead; m_audioRead = nullptr; }
    if (m_videoRead)  { delete m_videoRead; m_videoRead = nullptr; }
    if (m_screenRead) { delete m_screenRead; m_screenRead = nullptr; }

    if (m_videoEncoder)  { delete m_videoEncoder; m_videoEncoder = nullptr; }
    if (m_screenEncoder) { delete m_screenEncoder; m_screenEncoder = nullptr; }

    if (m_audioQueue)  { delete m_audioQueue; m_audioQueue = nullptr; }
    if (m_audioDecodeQueue) { delete m_audioDecodeQueue; m_audioDecodeQueue = nullptr; }
    if (m_pcmPlayQueue) { delete m_pcmPlayQueue; m_pcmPlayQueue = nullptr; }
    if (m_videoQueue)  { delete m_videoQueue; m_videoQueue = nullptr; }
    if (m_screenQueue) { delete m_screenQueue; m_screenQueue = nullptr; }

    for (auto& pair : m_mapVideoPipelines) {
        destroyVideoPipeline(pair.second);
    }
    m_mapVideoPipelines.clear();

    qDeleteAll(m_mapAudioWrite);
    m_mapAudioWrite.clear();

    qDebug() << "音视频设备资源清理完毕，安全退出！";
}

void CMediaManager::destroyVideoPipeline(VideoDecodePipeline* pl)
{
    if (!pl) return;

    pl->processor->slot_stop();
    pl->queue->stop();
    pl->thread->quit();
    pl->thread->wait();

    delete pl->processor;
    delete pl->thread;
    delete pl->decoder;
    delete pl->queue;
    delete pl;
}

void CMediaManager::slot_audioFrameRq(uint sock, char* buf, int nLen)
{
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

    if (m_pRoomManager && roomId == m_pRoomManager->getRoomId()) {
        if (!m_mapAudioWrite.contains(userId)) {
            Audio_Write* aw = new Audio_Write();
            aw->setPCMQueue(m_pcmPlayQueue);
            aw->start();
            m_mapAudioWrite[userId] = aw;
        }
        m_audioDecodeQueue->push(ba);
    }
}

void CMediaManager::slot_videoFrameRq(uint sock, char* buf, int nLen)
{
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

    if (m_pRoomManager && roomId == m_pRoomManager->getRoomId()) {
        if (m_mapVideoPipelines.find(userId) == m_mapVideoPipelines.end()) {
            VideoDecodePipeline* pl = new VideoDecodePipeline();
            pl->userId = userId;
            pl->queue = new VideoDecodeDataQueue();
            pl->decoder = new H264Decoder();
            pl->decoder->initDecoder();
            pl->processor = new VideoDecodeProcessor(userId, nullptr);
            pl->thread = new QThread();

            pl->processor->setQueue(pl->queue);
            pl->processor->setDecoder(pl->decoder);

            connect(pl->processor, &VideoDecodeProcessor::SIG_decodedFrame,
                    this, &CMediaManager::slot_dispatchDecodedVideo);

            pl->processor->moveToThread(pl->thread);
            pl->thread->start();
            QMetaObject::invokeMethod(pl->processor, "slot_start", Qt::QueuedConnection);

            m_mapVideoPipelines[userId] = pl;
        }

        m_mapVideoPipelines[userId]->queue->push(bt);
    }
}

void CMediaManager::slot_dispatchDecodedVideo(int userId, QImage img)
{
    if (m_pRoomDialog) {
        m_pRoomDialog->slot_refreshUser(userId, img);
    }
}

void CMediaManager::slot_userLeftRoom(int userId)
{
    auto it = m_mapVideoPipelines.find(userId);
    if (it != m_mapVideoPipelines.end()) {
        VideoDecodePipeline* pl = it->second;
        destroyVideoPipeline(pl);
        m_mapVideoPipelines.erase(it);
    }

    auto itAudio = m_mapAudioWrite.find(userId);
    if (itAudio != m_mapAudioWrite.end()) {
        delete itAudio.value();
        m_mapAudioWrite.erase(itAudio);
    }
}
