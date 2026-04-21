#include "PCMPlayQueue.h"

PCMPlayQueue::PCMPlayQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

// 生产者端：音频解码打工人解出纯净 PCM 后瞬间推入
void PCMPlayQueue::push(const QByteArray& pcmData)
{
    QMutexLocker locker(&m_mutex);

    // 蓄水池上限控制：缓存太多会导致严重的听觉延迟（音画不同步）
    if (m_queue.size() >= PCM_PLAY_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(pcmData);
    m_condition.wakeAll();
}

// 标准阻塞出队（一般不用，除非有其他软件线程需要拿 PCM 录音）
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

// 非阻塞极限拉取
// 消费者端：SDL 的 audioCallback (硬件时钟中断线程) 专属调用！
QByteArray PCMPlayQueue::tryPop()
{
    QMutexLocker locker(&m_mutex);

    // 绝对禁止使用 wait()！
    // 物理声卡每 20ms 无情索要数据，如果没水，立刻返回空盆子（播静音）
    // 决不能让声卡硬件去睡觉，否则引发死锁和极度刺耳的系统爆音！
    if (m_queue.isEmpty()) {
        return QByteArray();
    }

    return m_queue.dequeue();
}



void PCMPlayQueue::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    m_condition.wakeAll(); // 唤醒潜在的 pop() 调用者
}
