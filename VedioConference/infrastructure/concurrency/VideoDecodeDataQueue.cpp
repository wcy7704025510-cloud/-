
#include "VideoDecodeDataQueue.h"

VideoDecodeDataQueue::VideoDecodeDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

void VideoDecodeDataQueue::push(const QByteArray& h264Packet)
{
    QMutexLocker locker(&m_mutex);

    if (m_queue.size() >= VIDEO_DECODE_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(h264Packet);
    m_condition.wakeAll();
}

QByteArray VideoDecodeDataQueue::pop()
{
    QMutexLocker locker(&m_mutex);

    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex);
    }

    if (!m_running) {
        return QByteArray();
    }

    return m_queue.dequeue();
}

void VideoDecodeDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

void VideoDecodeDataQueue::stop()
{
    m_mutex.lock();
    m_running = false;
    m_condition.wakeAll();
    m_mutex.unlock();
}
