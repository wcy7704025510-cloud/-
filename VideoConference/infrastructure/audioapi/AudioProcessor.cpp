#include "AudioProcessor.h"
#include "AudioDataQueue.h"
#include <QDebug>

AudioProcessor::AudioProcessor(QObject *parent)
    : QObject(parent)
    , m_queue(nullptr)
    , m_running(false)
    , m_encoder(nullptr)
{
    int err;
    // 不使用普通的 OPUS_APPLICATION_AUDIO，而是明确指定 OPUS_APPLICATION_VOIP
    // 这会让底层算法开启语音频段的过滤与优化，即使在极低带宽下也能保证人声的高度清晰
    /*
        启用 SILK 引擎优先： Opus 内部有两个引擎（用于语音的 SILK 和用于音乐的 CELT）。
        VOIP 模式会强制偏向使用 SILK 引擎，这是专门针对人类声带发声物理模型设计的算法。
        高频截断:它会自动丢弃那些人类语音中不需要的超高频信号，
        把宝贵的比特率（Bitrate）全部分配给中低频的人声频段。
    */


    m_encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        qDebug() << "AudioProcessor: Opus encoder create failed" << opus_strerror(err);
    }
    // 在创建 encoder 之后，手动开启 DTX 模式
    /*
     在静音期间，Opus 不再每 20ms 输出一次数据，
     而是可能每隔 400ms 才输出一个极小的包，以维持对方听觉上的“环境底噪（舒适噪音）”，
     这会极其夸张地节省带宽。
     */
    int ret = opus_encoder_ctl(m_encoder, OPUS_SET_DTX(1));
    if (ret != OPUS_OK) {
        qDebug() << "开启 DTX 失败";
    }
}

AudioProcessor::~AudioProcessor()
{
    if (m_encoder) {
        opus_encoder_destroy(m_encoder);
        m_encoder = nullptr;
    }
}

void AudioProcessor::setQueue(AudioDataQueue* queue) {
    m_queue = queue;
}
void AudioProcessor::setSendCallback(std::function<void(char*, int)> callback) {
    m_sendCallback = callback;
}

void AudioProcessor::slot_start()
{
    m_running = true;
    processLoop();
}

void AudioProcessor::slot_stop()
{
    m_running = false;
    if (m_queue) {
        m_queue->stop(); // 唤醒底层 wait，打破死锁，安全退出
    }
}

//消费者核心工作流：异步重体力压缩
void AudioProcessor::processLoop()
{
    while (m_running) {
        if (!m_queue) break;

        // 1. 阻塞等待麦克风的 1920 字节纯净 PCM (平时 0 CPU 消耗)
        QByteArray rawData = m_queue->pop();

        if (!m_running) break;
        if (rawData.isEmpty()) continue;

        unsigned char opusData[4096];

        // 2.音频极致压缩
        // 将 1920 字节的 PCM 数据深度压缩。Opus 通常能将其压缩到原来的 1/10左右
        // 极大降低了网络传输的带宽压力
        int opusLen = opus_encode(
            m_encoder,                              //Opus 编码器句柄
            (const opus_int16*)rawData.constData(), //PCM 原始数据（必须16位）
            rawData.size() / 2,                     //采样点数量（大小/一个点的字节数2）
            opusData,                               //编码后输出缓冲区
            sizeof(opusData)                        //输出缓冲区最大大小
        );

        if (opusLen <= 0) continue;

        // 3. 将极其轻量级的 Opus 字节流通过回调扔给网络层发送
        if (m_sendCallback) {
            m_sendCallback((char*)opusData, opusLen);
        }
    }
}
