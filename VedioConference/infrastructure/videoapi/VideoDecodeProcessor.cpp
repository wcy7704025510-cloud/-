
#include "VideoDecodeProcessor.h"
#include "../concurrency/VideoDecodeDataQueue.h"
#include "H264Decoder.h"
#include <QDebug>

VideoDecodeProcessor::VideoDecodeProcessor(int userId, QObject *parent)
    : QObject(parent)
    , m_userId(userId)
    , m_queue(nullptr)
    , m_decoder(nullptr)
    , m_running(false)
{
}

VideoDecodeProcessor::~VideoDecodeProcessor()
{
}

void VideoDecodeProcessor::setQueue(VideoDecodeDataQueue* queue)
{
    m_queue = queue;
}

void VideoDecodeProcessor::setDecoder(H264Decoder* decoder)
{
    m_decoder = decoder;
}

void VideoDecodeProcessor::slot_start()
{
    m_running = true;

    while (m_running) {
        if (!m_queue) {
            break;
        }

        QByteArray packet = m_queue->pop();

        if (!m_running) {
            break;
        }

        if (packet.isEmpty()) {
            continue;
        }

        if (!m_decoder) {
            continue;
        }

        QImage outImg;
        if (m_decoder->decode(packet, outImg)) {
            emit SIG_decodedFrame(m_userId, outImg);
        }
    }
}

void VideoDecodeProcessor::slot_stop()
{
    m_running = false;
    if (m_queue) {
        m_queue->wakeAll();
    }
}
