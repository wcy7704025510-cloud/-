#include "VideoProcessor.h"
#include "VideoDataQueue.h"
#include "H264Encoder.h"
#include <QDebug>

VideoProcessor::VideoProcessor(QObject *parent)
    : QObject(parent)
    , m_queue(nullptr)
    , m_encoder(nullptr)
    , m_running(false)
{
}

VideoProcessor::~VideoProcessor()
{
}

void VideoProcessor::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}

void VideoProcessor::setEncoder(H264Encoder* encoder)
{
    m_encoder = encoder;
}

void VideoProcessor::setSendCallback(std::function<void(char*, int)> callback)
{
    m_sendCallback = callback;
}

void VideoProcessor::slot_start()
{
    m_running = true;
    processLoop();
}

void VideoProcessor::slot_stop()
{
    m_running = false;
    if (m_queue) {
        m_queue->stop();
    }
}

void VideoProcessor::processLoop()
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

        QByteArray encodedData = m_encoder->encode(image);

        if (encodedData.isEmpty()) {
            continue;
        }

        if (m_sendCallback) {
            m_sendCallback(encodedData.data(), encodedData.size());
        }
    }
}
