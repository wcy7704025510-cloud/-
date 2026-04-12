#include "VideoDecodeProcessor.h"
#include "../concurrency/VideoDecodeDataQueue.h"
#include "H264Decoder.h"
#include <QDebug>

// 构造时绑定 userId，该线程只处理该用户的视频流！杜绝 H.264 状态机花屏！
VideoDecodeProcessor::VideoDecodeProcessor(int userId, QObject *parent)
    : QObject(parent)
    , m_userId(userId)
    , m_queue(nullptr)
    , m_decoder(nullptr)
    , m_running(false)
{
}

VideoDecodeProcessor::~VideoDecodeProcessor()
{
}

void VideoDecodeProcessor::setQueue(VideoDecodeDataQueue* queue) {
    m_queue = queue;
}
void VideoDecodeProcessor::setDecoder(H264Decoder* decoder) {
    m_decoder = decoder;
}

void VideoDecodeProcessor::slot_start()
{
    m_running = true;

    // 消费者核心工作流：后台独立解压
    while (m_running) {
        if (!m_queue) break;

        // 1. 取出特定远端用户的 H.264 压缩包 (没包时自动挂起)
        QByteArray packet = m_queue->pop();

        // 被 stop() 唤醒时立刻安全退出
        if (!m_running) break;
        if (packet.isEmpty()) continue;
        if (!m_decoder) continue;

        QImage outImg;
        // 2. 调用 FFmpeg 解码引擎进行解压和色彩转换 (YUV420P -> RGB888)
        if (m_decoder->decode(packet, outImg)) {
            // 跨线程投递渲染
            // 解压好的图像绝对不能在子线程直接画 UI！
            // 必须通过 Qt 信号弹射给主线程，主线程会自动将其放入事件队列安全渲染
            emit SIG_decodedFrame(m_userId, outImg);
        }
    }
}

void VideoDecodeProcessor::slot_stop()
{
    m_running = false;
    if (m_queue) {
        m_queue->stop(); // 打破 wait() 僵局，实现完美的内存级安全销毁
    }
}
