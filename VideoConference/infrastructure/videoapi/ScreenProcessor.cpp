#include "ScreenProcessor.h"
#include "VideoDataQueue.h"
#include "H264Encoder.h"
#include <QDebug>

ScreenProcessor::ScreenProcessor(QObject *parent)
    : QObject(parent)
    , m_queue(nullptr)
    , m_encoder(nullptr)
    , m_running(false)
{
}

ScreenProcessor::~ScreenProcessor()
{
}

void ScreenProcessor::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}

void ScreenProcessor::setEncoder(H264Encoder* encoder)
{
    m_encoder = encoder;
}

void ScreenProcessor::setSendCallback(std::function<void(char*, int)> callback)
{
    m_sendCallback = callback;
}

void ScreenProcessor::slot_start()
{
    m_running = true;
    processLoop();
}

void ScreenProcessor::slot_stop()
{
    m_running = false;
    if (m_queue) {
        m_queue->stop();
    }
}

void ScreenProcessor::processLoop()
{
    while (m_running) {
        if (!m_queue) {
            break;
        }

        QImage image = m_queue->pop();

        if (!m_running) {
            break;
        }

        if (image.isNull()) {
            continue;
        }

        if (!m_encoder) {
            continue;
        }

        // 强制转换为 RGB888，保证后续编码器拿到的是标准格式
        if (image.format() != QImage::Format_RGB888) {
            image = image.convertToFormat(QImage::Format_RGB888);
        }
        QByteArray encodedData = m_encoder->encode(image);

        if (encodedData.isEmpty()) {
            continue;
        }

        if (m_sendCallback) {
            m_sendCallback(encodedData.data(), encodedData.size());
        }
    }
}
