#include "AudioDataQueue.h"

AudioDataQueue::AudioDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

void AudioDataQueue::push(const QByteArray& data)
{
    m_mutex.lock();

    while (m_queue.size() >= AUDIO_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(data);
    m_condition.wakeAll();
    m_mutex.unlock();
}

QByteArray AudioDataQueue::pop()
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

void AudioDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

void AudioDataQueue::stop()
{
    m_mutex.lock();
    m_running = false;
    m_condition.wakeAll();
    m_mutex.unlock();
}
