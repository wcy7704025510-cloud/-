#include "videoread.h"
#include "VideoDataQueue.h"

using namespace cv;

VideoRead::VideoRead(QObject *parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_queue(nullptr)
{
}

VideoRead::~VideoRead()
{
    // 析构时必须清理定时器，释放摄像头硬件占用
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

// 注入并发水库 (依赖注入，与下层管线解耦)
void VideoRead::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}

//生产者 由定时器高频触发 (例如15fps)
void VideoRead::slot_getVideoFrame()
{
    Mat frame;
    if (!cap.read(frame)) {
        return; // 摄像头未准备好时直接返回，防崩溃
    }

    // 色彩空间转换：OpenCV 默认是 BGR，Qt 渲染需要 RGB
    cvtColor(frame, frame, CV_BGR2RGB);

    // 将底层内存封装为 QImage (共享内存，避免深拷贝开销)
    QImage image((unsigned const char*)frame.data,
                 frame.cols, frame.rows,
                 QImage::Format_RGB888);

    // 缩放到统一分辨率，降低后续 H.264 编码的矩阵运算压力
    image = image.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatio);

    //将轻量化处理后的图像瞬间推入水库，绝不阻塞当前定时器线程！
    if (m_queue) {
        m_queue->push(image);
    }

    // 发射给主线程进行本地画面的零延迟预览
    Q_EMIT SIG_sendVedioFrame(image);
}

void VideoRead::slot_openVideo()
{
    if (m_timer) return; // 防御性编程：防止重复打开导致定时器泄漏

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &VideoRead::slot_getVideoFrame);

    //减去 10ms 是为了抵消代码执行的开销，确保真实帧率贴近目标帧率（线程切换）
    m_timer->start(1000/FRAME_RATE-10);
    cap.open(0); // 打开默认摄像头
    if (!cap.isOpened()) {
        QMessageBox::information(nullptr, "提示", "视频没有打开");
        return;
    }
}

void VideoRead::slot_closeVideo()
{
    // 安全关闭：停定时器、释放 OpenCV 摄像头句柄
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    if (cap.isOpened()) {
        cap.release();
    }
}
