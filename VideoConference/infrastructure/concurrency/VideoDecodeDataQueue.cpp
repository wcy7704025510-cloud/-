#include "VideoDecodeDataQueue.h"

VideoDecodeDataQueue::VideoDecodeDataQueue(QObject *parent)
    : QObject(parent)
    , m_running(true)
{
}

// 生产者端：网络层收到特定用户的 H.264 视频流后推入
void VideoDecodeDataQueue::push(const QByteArray& h264Packet)
{
    QMutexLocker locker(&m_mutex);

    // 防花屏与低延迟妥协：H264 丢包会导致局部花屏（直到下一个I帧）
    // 但为了不引发灾难性的内存暴涨和十几秒的延迟，超限时仍需丢弃
    if (m_queue.size() >= VIDEO_DECODE_QUEUE_MAX_SIZE) {
        m_queue.dequeue();
    }

    m_queue.enqueue(h264Packet);
    m_condition.wakeAll(); // 唤醒该用户专属的视频解码打工人
}

// 消费者端：该用户专属的视频解码子线程调用
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
