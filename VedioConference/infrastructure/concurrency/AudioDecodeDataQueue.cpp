
#include "AudioDecodeDataQueue.h"

AudioDecodeDataQueue::AudioDecodeDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

void AudioDecodeDataQueue::push(const QByteArray& opusPacket)
{
    QMutexLocker locker(&m_mutex);

    if (m_queue.size() >= AUDIO_DECODE_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(opusPacket);
    m_condition.wakeAll();
}

QByteArray AudioDecodeDataQueue::pop()
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

void AudioDecodeDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

void AudioDecodeDataQueue::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    m_condition.wakeAll();
}
