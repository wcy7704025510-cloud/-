
#include "PCMPlayQueue.h"

PCMPlayQueue::PCMPlayQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

void PCMPlayQueue::push(const QByteArray& pcmData)
{
    QMutexLocker locker(&m_mutex);

    if (m_queue.size() >= PCM_PLAY_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(pcmData);
    m_condition.wakeAll();
}

QByteArray PCMPlayQueue::pop()
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

QByteArray PCMPlayQueue::tryPop()
{
    QMutexLocker locker(&m_mutex);

    if (m_queue.isEmpty()) {
        return QByteArray();
    }

    return m_queue.dequeue();
}

void PCMPlayQueue::wakeAll()
{
    m_condition.wakeAll();
}

void PCMPlayQueue::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    m_condition.wakeAll();
}
