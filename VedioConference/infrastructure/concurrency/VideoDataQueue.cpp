#include "VideoDataQueue.h"

VideoDataQueue::VideoDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

void VideoDataQueue::push(const QImage& image)
{
    m_mutex.lock();

    while (m_queue.size() >= VIDEO_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(image);
    m_condition.wakeAll();
    m_mutex.unlock();
}

QImage VideoDataQueue::pop()
{
    QMutexLocker locker(&m_mutex);

    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex);
    }

    if (!m_running) {
        return QImage();
    }

    return m_queue.dequeue();
}

void VideoDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

void VideoDataQueue::stop()
{
    m_mutex.lock();
    m_running = false;
    m_condition.wakeAll();
    m_mutex.unlock();
}
