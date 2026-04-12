#include "AudioDataQueue.h"

AudioDataQueue::AudioDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true) // 默认水库处于营业状态
{
}

// 生产者端：麦克风采集线程调用
void AudioDataQueue::push(const QByteArray& data)
{
    m_mutex.lock();

    // 当编码速度跟不上采集速度，队列达到上限时，主动丢弃最老的音频块
    // 宁可丢失部分声音，也绝不让底层产生累积延迟
    while (m_queue.size() >= AUDIO_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(data); // 存入最新数据
    m_condition.wakeAll(); // 唤醒所有正在睡眠等数据的消费者（编码线程）
    m_mutex.unlock();
}

// 消费者端：音频编码子线程调用 (阻塞型)
QByteArray AudioDataQueue::pop()
{
    QMutexLocker locker(&m_mutex); // 离开作用域自动解锁，防止异常导致死锁

    // 如果水库是空的，且还在正常营业，消费者线程进入 0 CPU 消耗的挂起休眠状态
    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex);
    }

    // 如果是被 stop() 强制叫醒的（公司倒闭了），直接返回空数据，让子线程跳出死循环
    if (!m_running) {
        return QByteArray();
    }

    return m_queue.dequeue(); // 取出最老的一块数据去编码
}

void AudioDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

// 防死锁安全退出机制
void AudioDataQueue::stop()
{
    m_mutex.lock();
    m_running = false;     // 拉下电闸，标记为停止营业
    m_condition.wakeAll(); // 瞬间惊醒所有还在 pop() 里死等睡眠的线程，让它们安全下班
    m_mutex.unlock();
}
