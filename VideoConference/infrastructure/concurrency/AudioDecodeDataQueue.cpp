#include "AudioDecodeDataQueue.h"

AudioDecodeDataQueue::AudioDecodeDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

// 生产者端：网络层主线程收到 UDP 包后瞬间推入
void AudioDecodeDataQueue::push(const QByteArray& opusPacket)
{
    QMutexLocker locker(&m_mutex); // RAII 风格加锁

    // 防堆积机制：网络极度卡顿时引发瞬间的包爆炸，丢弃最老的音频包
    if (m_queue.size() >= AUDIO_DECODE_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(opusPacket);
    m_condition.wakeAll(); // 唤醒全局唯一的音频解码打工人
}

// 消费者端：全局唯一的音频解码子线程调用
QByteArray AudioDecodeDataQueue::pop()
{
    QMutexLocker locker(&m_mutex);

    // 没网速/没说话时，解码线程安安静静睡觉，0 性能消耗
    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex);
    }

    if (!m_running) {
        return QByteArray();
    }

    return m_queue.dequeue();
}


void AudioDecodeDataQueue::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    m_condition.wakeAll();
}
