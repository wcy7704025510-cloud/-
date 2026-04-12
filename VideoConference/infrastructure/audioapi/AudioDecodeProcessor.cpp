#include "AudioDecodeProcessor.h"
#include "../concurrency/AudioDecodeDataQueue.h"
#include "../concurrency/PCMPlayQueue.h"
#include "audio_common.h"
#include <QDebug>

AudioDecodeProcessor::AudioDecodeProcessor(QObject *parent)
    : QObject(parent)
    , m_inputQueue(nullptr)
    , m_outputQueue(nullptr)
    , m_decoder(nullptr)
    , m_running(false)
{
    int err;
    // 初始化 48000Hz 的 Opus 解码器
    m_decoder = opus_decoder_create(48000, 1, &err);
    if (err != OPUS_OK) {
        qDebug() << "AudioDecodeProcessor: Opus decoder create failed" << opus_strerror(err);
        emit SIG_decodeError(QString("Opus decoder create failed: %1").arg(opus_strerror(err)));
    }
}

AudioDecodeProcessor::~AudioDecodeProcessor()
{
    if (m_decoder) {
        opus_decoder_destroy(m_decoder);
        m_decoder = nullptr;
    }
}

void AudioDecodeProcessor::setInputQueue(AudioDecodeDataQueue* queue) {
    m_inputQueue = queue;
}
void AudioDecodeProcessor::setOutputQueue(PCMPlayQueue* queue) {
    m_outputQueue = queue;
}

void AudioDecodeProcessor::slot_start()
{
    m_running = true;
    processLoop();
}

void AudioDecodeProcessor::slot_stop()
{
    m_running = false;
    if (m_inputQueue) {
        m_inputQueue->stop();
    }
}

// 消费者核心工作流
void AudioDecodeProcessor::processLoop()
{
    while (m_running) {
        if (!m_inputQueue || !m_outputQueue || !m_decoder) break;

        // 1. 无差别吸水：不区分用户 ID，只要网络层收到了任何人的音频压缩包，全取出来
        QByteArray opusData = m_inputQueue->pop();

        if (!m_running) break;
        if (opusData.isEmpty()) continue;

        opus_int16 pcmBuffer[4096];

        // 2. 将轻量压缩包还原为庞大的 PCM 裸流 (1920字节)
        int frameSize = opus_decode(
            m_decoder,
            (const unsigned char*)opusData.constData(),
            opusData.size(),
            pcmBuffer,
            sizeof(pcmBuffer) / sizeof(opus_int16),
            0
        );

        if (frameSize <= 0) {
            qDebug() << "AudioDecodeProcessor: decode failed" << opus_strerror(frameSize);
            continue;
        }

        // 3. 将纯净水注入扬声器专用的 PCMPlayQueue 水库，供声卡随时拉取
        QByteArray pcmData((const char*)pcmBuffer, frameSize * 2);
        m_outputQueue->push(pcmData);
    }
}
