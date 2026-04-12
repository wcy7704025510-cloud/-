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
    m_encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        qDebug() << "AudioProcessor: Opus encoder create failed" << opus_strerror(err);
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
            m_encoder,
            (const opus_int16*)rawData.constData(),
            rawData.size() / 2, // 传入采样点数 (字节数/2)
            opusData,
            sizeof(opusData)
        );

        if (opusLen <= 0) continue;

        // 3. 将极其轻量级的 Opus 字节流通过回调扔给网络层发送
        if (m_sendCallback) {
            m_sendCallback((char*)opusData, opusLen);
        }
    }
}
