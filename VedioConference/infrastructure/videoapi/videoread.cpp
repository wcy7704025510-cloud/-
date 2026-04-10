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
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

void VideoRead::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}

void VideoRead::slot_getVideoFrame()
{
    Mat frame;
    if (!cap.read(frame)) {
        return;
    }

    cvtColor(frame, frame, CV_BGR2RGB);

    QImage image((unsigned const char*)frame.data,
                 frame.cols, frame.rows,
                 QImage::Format_RGB888);

    image = image.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatio);

    if (m_queue) {
        m_queue->push(image);
    }

    Q_EMIT SIG_sendVedioFrame(image);
}

void VideoRead::slot_openVideo()
{
    if (m_timer) return; // 防止重复创建

    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &VideoRead::slot_getVideoFrame);

    m_timer->start(1000/FRAME_RATE-10);
    cap.open(0);
    if (!cap.isOpened()) {
        QMessageBox::information(nullptr, "提示", "视频没有打开");
        return;
    }
}

void VideoRead::slot_closeVideo()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    if (cap.isOpened()) {
        cap.release();
    }
}
