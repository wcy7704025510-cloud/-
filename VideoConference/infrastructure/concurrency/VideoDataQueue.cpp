#include "VideoDataQueue.h"

VideoDataQueue::VideoDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

// 生产者端：摄像头/屏幕采集定时器调用
void VideoDataQueue::push(const QImage& image)
{
    m_mutex.lock();

    // 主动丢帧保实时
    // 视频极其消耗内存，如果堆积会导致 OOM（内存溢出）和严重的画面延迟
    // 超过阈值无情丢弃旧画面，保证编码线程拿到的永远是最新的一帧
    while (m_queue.size() >= VIDEO_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(image);
    m_condition.wakeAll(); // 唤醒视频编码线程
    m_mutex.unlock();
}

// 消费者端：视频编码子线程调用 (阻塞型)
QImage VideoDataQueue::pop()
{
    QMutexLocker locker(&m_mutex);

    // 没数据时挂起，不占用主线程和 CPU 资源
    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex);
    }

    // 收到下班通知，返回空图片以打破处理循环
    if (!m_running) {
        return QImage();
    }

    return m_queue.dequeue();
}

void VideoDataQueue::wakeAll()
{
    m_condition.wakeAll();
}

// 大管家退房清理时调用
void VideoDataQueue::stop()
{
    m_mutex.lock();
    m_running = false;
    m_condition.wakeAll(); // 唤醒编码线程，防止其死锁在 wait() 中
    m_mutex.unlock();
}
