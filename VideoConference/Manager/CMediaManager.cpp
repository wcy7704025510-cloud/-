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

// 构造函数：初始化所有底层组件指针为空，防止野指针
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
    , m_videoCaptureThread(nullptr)
{
}

CMediaManager::~CMediaManager()
{
    // 析构时必须触发绝对安全的清理流程
    slot_clearDevices();
}

// 业务层（加入/创建房间成功后）调用此函数，组装并启动所有音视频流水线
void CMediaManager::slot_startMediaEngines()
{
    qDebug() << "开始装配并启动音视频流水线...";

    // 防御性编程：防止用户因为网络抖动多次点击导致线程重复创建、内存泄漏
    if (m_audioThread != nullptr || m_videoThread != nullptr || m_screenThread != nullptr) {
        qDebug() << "检测到已有线程，先清理再重建...";
        slot_clearDevices();
    }

    // 按顺序装配四大管线
    setupAudioReceivePipeline();  // 音频下行（接收解压播放）
    setupAudioCapturePipeline();  // 音频上行（采集压缩发送）
    setupVideoCapturePipeline();  // 视频上行（采集压缩发送）
    setupScreenCapturePipeline(); // 屏幕上行（截屏压缩发送）

    qDebug() << "音视频流水线装配完毕，引擎点火！";
}

void CMediaManager::initDevices()
{
    setupAudioReceivePipeline();
    setupAudioCapturePipeline();
    setupVideoCapturePipeline();
    setupScreenCapturePipeline();
}

//音频大一统接收流水线
void CMediaManager::setupAudioReceivePipeline()
{
    // 1. 实例化组件：网络水库、声卡水库、解码打工人、独立线程
    m_audioDecodeQueue = new AudioDecodeDataQueue(this);
    m_pcmPlayQueue = new PCMPlayQueue(this);
    m_audioDecodeProcessor = new AudioDecodeProcessor(nullptr);
    m_audioDecodeThread = new QThread(this);

    // 2. 依赖注入：告诉解码打工人去哪里拿 Opus（输入），解好的 PCM 放哪（输出）
    m_audioDecodeProcessor->setInputQueue(m_audioDecodeQueue);
    m_audioDecodeProcessor->setOutputQueue(m_pcmPlayQueue);

    // 3. 灵魂出窍：把打工人流放到后台子线程
    m_audioDecodeProcessor->moveToThread(m_audioDecodeThread);

    // 4. 点火：启动操作系统线程，并跨线程触发打工人的死循环
    m_audioDecodeThread->start();
    QTimer::singleShot(0, m_audioDecodeProcessor, SLOT(slot_start()));
}

// 音频采集上行流水线
void CMediaManager::setupAudioCapturePipeline()
{
    m_audioQueue = new AudioDataQueue(this);
    m_audioRead = new Audio_Read(this);

    m_audioProcessor = new AudioProcessor(nullptr);
    m_audioThread = new QThread(this);

    m_audioRead->setQueue(m_audioQueue);
    m_audioProcessor->setQueue(m_audioQueue);

    //  注入发送动作。当打工人压缩好数据后，通过这个 lambda 回调本类的封包发送函数
    m_audioProcessor->setSendCallback(
        [this](char* rawData, int rawLen) {
            this->audioSendCallback(rawData, rawLen);
        }
    );

    m_audioProcessor->moveToThread(m_audioThread);
    m_audioThread->start();
    QTimer::singleShot(0, m_audioProcessor, SLOT(slot_start()));
}

// 视频采集上行流水线
void CMediaManager::setupVideoCapturePipeline()
{
    m_videoQueue = new VideoDataQueue(this);
    m_videoRead = new VideoRead();

    m_videoCaptureThread = new QThread(this);




    m_videoEncoder = new H264Encoder(nullptr);

    m_videoProcessor = new VideoProcessor(nullptr);
    m_videoThread = new QThread(this);
    //将采集器搬迁到子线程！
    m_videoRead->moveToThread(m_videoCaptureThread);

    //跨线程绑定打开/关闭信号
    connect(this, &CMediaManager::SIG_Ctrl_OpenVideo, m_videoRead, &VideoRead::slot_openVideo);
    connect(this, &CMediaManager::SIG_Ctrl_CloseVideo, m_videoRead, &VideoRead::slot_closeVideo);

    m_videoRead->setQueue(m_videoQueue);
    m_videoRead->moveToThread(m_videoCaptureThread);

    m_videoProcessor->setQueue(m_videoQueue);
    m_videoProcessor->setEncoder(m_videoEncoder);

    // 本地零延迟预览：摄像头一抓到画面，直接发射给 UI 画出来
    connect(m_videoRead, SIGNAL(SIG_sendVedioFrame(QImage)),
            this, SLOT(slot_localVideoPreview(QImage)));

    // 初始化硬件压缩算法
    m_videoEncoder->initEncoder(IMAGE_WIDTH, IMAGE_HEIGHT, FRAME_RATE);

    m_videoProcessor->setSendCallback(
        [this](char* rawData, int rawLen) {
            this->videoSendCallback(rawData, rawLen);
        }
    );

    m_videoProcessor->moveToThread(m_videoThread);
    m_videoCaptureThread->start(); // 启动采集线程
    m_videoThread->start();        // 启动压缩线程
    QTimer::singleShot(0, m_videoProcessor, SLOT(slot_start()));
}

// 桌面采集上行流水线 (逻辑同视频采集)
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

// 音频封包与路由
void CMediaManager::audioSendCallback(char* rawData, int rawLen)
{
    if (!m_pRoomManager || m_pRoomManager->getRoomId() == 0) {
        return; // 不在房间时，不发送任何数据
    }

    // 封装自定义的应用层报文头 (24字节 = 6 * 4字节)
    int totalLen = 6 * sizeof(int) + rawLen;
    char* packet = new char[totalLen];
    char* p = packet;

    // 1-3. 写入包类型、用户ID、房间号
    *(int*)p = _DEF_PACK_AUDIO_FRAME; p += sizeof(int);
    *(int*)p = m_id; p += sizeof(int);
    *(int*)p = m_pRoomManager->getRoomId(); p += sizeof(int);

    // 4-6. 写入 PTS (显示时间戳)，用于未来的音视频同步与防抖动
    QTime tm = QTime::currentTime();
    *(int*)p = tm.minute(); p += sizeof(int);
    *(int*)p = tm.second(); p += sizeof(int);
    *(int*)p = tm.msec(); p += sizeof(int);

    // 拷贝纯音频数据
    memcpy(p, rawData, rawLen);

    // 透传给 UDP 网络层发送
    if (m_audioClient) {
        m_audioClient->SendData(0, packet, totalLen);
    }

    delete[] packet;
}

// 视频封包与路由
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

// 桌面封包与路由
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

// 外设硬件启停控制控制开关
// 改为发射信号，让子线程去执行硬件调用
void CMediaManager::slot_AudioPause() { qDebug() << __func__; if (m_audioRead) m_audioRead->pause(); }
void CMediaManager::slot_AudioStart() { qDebug() << __func__; if (m_audioRead) m_audioRead->start(); }
void CMediaManager::slot_VideoPause() { qDebug() << __func__; Q_EMIT SIG_Ctrl_CloseVideo(); }
void CMediaManager::slot_VideoStart() { qDebug() << __func__; Q_EMIT SIG_Ctrl_OpenVideo(); }
void CMediaManager::slot_ScreenPause(){ qDebug() << __func__; if (m_screenRead) m_screenRead->slot_closeVedio(); }
void CMediaManager::slot_ScreenStart(){ qDebug() << __func__; if (m_screenRead) m_screenRead->slot_openVedio(); }
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

// 严格按照层级顺序销毁多线程系统，防止互斥锁和条件变量引发的程序死锁！
void CMediaManager::slot_clearDevices()
{
    qDebug() << "开始清理音视频设备资源...";

    // 第一步：切断源头。停止所有物理外设的硬件时钟，不再产生新数据
    if (m_audioRead)  m_audioRead->pause();
    // 使用阻塞队列安全关闭子线程的定时器
    if (m_videoRead) {
        QMetaObject::invokeMethod(m_videoRead, "slot_closeVideo", Qt::BlockingQueuedConnection);
    }
    if (m_screenRead) m_screenRead->slot_closeVedio();

    // 第二步：降下标志旗。通知所有打工人线程把 m_running 改为 false
    if (m_audioProcessor)  m_audioProcessor->slot_stop();
    if (m_audioDecodeProcessor) m_audioDecodeProcessor->slot_stop();
    if (m_videoProcessor) m_videoProcessor->slot_stop();
    if (m_screenProcessor) m_screenProcessor->slot_stop();

    // 第三步：强制破冰。敲响所有水库的警钟（wakeAll），把正在休眠死等数据的线程全部惊醒！
    // 因为第二步已经设了 false，醒来的线程会立刻跳出死循环！
    if (m_audioQueue)  m_audioQueue->stop();
    if (m_audioDecodeQueue) m_audioDecodeQueue->stop();
    if (m_videoQueue) m_videoQueue->stop();
    if (m_screenQueue) m_screenQueue->stop();

    // 第四步：线程入土。通知操作系统结束线程，并挂起主线程耐心等待（wait），直到子线程完全死透。
    if (m_audioThread) { m_audioThread->quit(); m_audioThread->wait(); }
    if (m_audioDecodeThread) { m_audioDecodeThread->quit(); m_audioDecodeThread->wait(); }
    if (m_videoThread) { m_videoThread->quit(); m_videoThread->wait(); }
    if (m_videoCaptureThread) { m_videoCaptureThread->quit(); m_videoCaptureThread->wait(); }
    if (m_screenThread) { m_screenThread->quit(); m_screenThread->wait(); }

    // 第五步：清扫战场。按顺序安全的 delete 所有堆区内存。
    if (m_audioProcessor)  { delete m_audioProcessor; m_audioProcessor = nullptr; }
    if (m_audioDecodeProcessor) { delete m_audioDecodeProcessor; m_audioDecodeProcessor = nullptr; }
    if (m_videoProcessor)  { delete m_videoProcessor; m_videoProcessor = nullptr; }
    if (m_screenProcessor) { delete m_screenProcessor; m_screenProcessor = nullptr; }

    if (m_audioThread)  { delete m_audioThread; m_audioThread = nullptr; }
    if (m_audioDecodeThread) { delete m_audioDecodeThread; m_audioDecodeThread = nullptr; }
    if (m_videoThread)  { delete m_videoThread; m_videoThread = nullptr; }
    if (m_screenThread) { delete m_screenThread; m_screenThread = nullptr; }
    if (m_videoCaptureThread) { delete m_videoCaptureThread; m_videoCaptureThread = nullptr; }

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

    // 清理所有为远端用户动态分配的沙盒解码流水线
    for (auto& pair : m_mapVideoPipelines) {
        destroyVideoPipeline(pair.second);
    }
    m_mapVideoPipelines.clear();

    // 清理底层的独立音频播放硬件 (Audio_Write)
    qDeleteAll(m_mapAudioWrite);
    m_mapAudioWrite.clear();

    qDebug() << "音视频设备资源清理完毕，安全退出！";
}

// 辅助函数：安全销毁一个动态分配的视频解码沙盒流水线
void CMediaManager::destroyVideoPipeline(VideoDecodePipeline* pl)
{
    if (!pl) return;

    // 同样严格遵循：降标志 -> 破阻塞 -> 退线程 -> 清内存的黄金顺序
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
    // 1. 从报文中解析出 24 字节头信息
    char* tmp = buf;
    tmp += sizeof(int); // 跳过 type
    int userId = *(int*)tmp;
    tmp += sizeof(int);
    int roomId = *(int*)tmp;
    tmp += sizeof(int);
    tmp += sizeof(int); // 略过 min
    tmp += sizeof(int); // 略过 sec
    tmp += sizeof(int); // 略过 msec

    int nbuflen = nLen - 6 * sizeof(int);
    QByteArray ba(tmp, nbuflen); // 提取纯 Opus 压缩包

    if (m_pRoomManager && roomId == m_pRoomManager->getRoomId()) {
        // 如果是没见过的新用户，马上为他分配专属的物理声卡输出口
        if (!m_mapAudioWrite.contains(userId)) {
            Audio_Write* aw = new Audio_Write();
            aw->setPCMQueue(m_pcmPlayQueue); // 对接同一个公共水库
            aw->start();                     // 启动硬件回调
            m_mapAudioWrite[userId] = aw;
        }
        // 将压缩包扔进全局大一统漏斗，交由后台解码线程慢慢解
        m_audioDecodeQueue->push(ba);
    }
}

// 数据入口：视频下行动态沙盒路由 (顶级架构设计)
void CMediaManager::slot_videoFrameRq(uint sock, char* buf, int nLen)
{
    // 1. 解析包头
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
        // 2. 沙盒动态分配机制
        // H.264 极其依赖上下文（P帧依赖前一张图像）。绝不能把张三和李四的视频流混着解，必花屏！
        if (m_mapVideoPipelines.find(userId) == m_mapVideoPipelines.end()) {

            //立刻在内存里为他开辟一条专属的解码管线！
            VideoDecodePipeline* pl = new VideoDecodePipeline();
            pl->userId = userId;
            pl->queue = new VideoDecodeDataQueue();             // 专属水库
            pl->decoder = new H264Decoder();                    // 专属解码器
            pl->decoder->initDecoder();
            pl->processor = new VideoDecodeProcessor(userId, nullptr); // 专属打工人
            pl->thread = new QThread();                         // 专属线程

            pl->processor->setQueue(pl->queue);
            pl->processor->setDecoder(pl->decoder);

            // 连接解码完成信号，通知 UI 刷新这名特定用户的画面
            connect(pl->processor, &VideoDecodeProcessor::SIG_decodedFrame,
                    this, &CMediaManager::slot_dispatchDecodedVideo);

            // 点火
            pl->processor->moveToThread(pl->thread);
            pl->thread->start();
            QMetaObject::invokeMethod(pl->processor, "slot_start", Qt::QueuedConnection);

            m_mapVideoPipelines[userId] = pl;
        }

        // 3. 将收到的 H.264 字节流精准推入该用户专属的水库
        m_mapVideoPipelines[userId]->queue->push(bt);
    }
}

void CMediaManager::slot_dispatchDecodedVideo(int userId, QImage img)
{
    // 解码线程在此处将画面安全移交回主线程 UI
    if (m_pRoomDialog) {
        m_pRoomDialog->slot_refreshUser(userId, img);
    }
}

// 资源精准回收
void CMediaManager::slot_userLeftRoom(int userId)
{
    // 当某人退出房间时，精准拆除他专属的视频解码流水线
    auto it = m_mapVideoPipelines.find(userId);
    if (it != m_mapVideoPipelines.end()) {
        VideoDecodePipeline* pl = it->second;
        destroyVideoPipeline(pl);
        m_mapVideoPipelines.erase(it);
    }

    // 同样，销毁他专属的音频硬件资源
    auto itAudio = m_mapAudioWrite.find(userId);
    if (itAudio != m_mapAudioWrite.end()) {
        delete itAudio.value();
        m_mapAudioWrite.erase(itAudio);
    }
}
