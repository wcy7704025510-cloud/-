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

void AudioProcessor::setQueue(AudioDataQueue* queue)
{
    m_queue = queue;
}

void AudioProcessor::setSendCallback(std::function<void(char*, int)> callback)
{
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
        m_queue->stop();
    }
}

void AudioProcessor::processLoop()
{
    while (m_running) {
        if (!m_queue) {
            break;
        }

        QByteArray rawData = m_queue->pop();

        if (!m_running) {
            break;
        }

        if (rawData.isEmpty()) {
            continue;
        }

        unsigned char opusData[4096];
        int opusLen = opus_encode(
            m_encoder,
            (const opus_int16*)rawData.constData(),
            rawData.size() / 2,
            opusData,
            sizeof(opusData)
        );

        if (opusLen <= 0) {
            continue;
        }

        if (m_sendCallback) {
            m_sendCallback((char*)opusData, opusLen);
        }
    }
}
