#include "VideoProcessor.h"
#include "VideoDataQueue.h"
#include "H264Encoder.h"
#include <QDebug>

VideoProcessor::VideoProcessor(QObject *parent)
    : QObject(parent)
    , m_queue(nullptr)
    , m_encoder(nullptr)
    , m_running(false)
{
}

VideoProcessor::~VideoProcessor()
{
}

// 注入并发水库与纯算法编码引擎，实现控制逻辑与算法计算的完全解耦
void VideoProcessor::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}
void VideoProcessor::setEncoder(H264Encoder* encoder) {
    m_encoder = encoder;
}
void VideoProcessor::setSendCallback(std::function<void(char*, int)> callback) {
    m_sendCallback = callback;
}

//跨线程调用此函数启动后台死循环
void VideoProcessor::slot_start()
{
    m_running = true;
    processLoop();
}

// 防死锁然后退出
void VideoProcessor::slot_stop()
{
    m_running = false; // 先拉下电闸
    if (m_queue) {
        m_queue->stop(); // 关键！唤醒底层死等 pop() 的线程，打破阻塞
    }
}

// 消费者工作：在独立子线程中疯狂运转
void VideoProcessor::processLoop()
{
    while (m_running) {
        if (!m_queue) break;

        // 1. 取水：队列为空时会进入 wait() 挂起状态，0 CPU 消耗
        QImage image = m_queue->pop();

        // 2. 状态二次确认：如果是被 stop() 强行叫醒的，立刻跳出循环，线程安全结束！
        if (!m_running) break;
        if (image.isNull()) continue; // 防御脏数据
        if (!m_encoder) continue;

        // 3. 重体力劳动：执行 H.264 矩阵压缩算法 (由于配置了 zerolatency，这里是 0 延迟立出数据)
        QByteArray encodedData = m_encoder->encode(image);

        if (encodedData.isEmpty()) continue;

        // 4. 物流交接：通过 std::function 回调，将极其轻巧的 H.264 字节流扔给网络层发送
        if (m_sendCallback) {
            m_sendCallback(encodedData.data(), encodedData.size());
        }
    }
}
