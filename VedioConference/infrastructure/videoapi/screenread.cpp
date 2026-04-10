#include "screenread.h"
#include "VideoDataQueue.h"

ScreenRead::ScreenRead(QObject *parent)
    : QObject(parent)
    , m_pTimer(nullptr)
    , m_queue(nullptr)
{
}

ScreenRead::~ScreenRead()
{
    if (m_pTimer) {
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

void ScreenRead::setQueue(VideoDataQueue* queue)
{
    m_queue = queue;
}

void ScreenRead::slot_getScreenFrame()
{
    QScreen* src = QApplication::primaryScreen();
    QPixmap map = src->grabWindow(QApplication::desktop()->winId());
    QImage image = map.toImage();

    if (image.format() != QImage::Format_RGB888) {
        image = image.convertToFormat(QImage::Format_RGB888);
    }

    if (m_queue) {
        m_queue->push(image);
    }

    Q_EMIT SIG_getScreenFrame(image);
}

void ScreenRead::slot_openVedio()
{
    if (m_pTimer) return; // 防止重复创建

    m_pTimer = new QTimer(this);

    connect(m_pTimer, &QTimer::timeout, this, &ScreenRead::slot_getScreenFrame);

    m_pTimer->start(1000/SCREEN_FRAME_RATE-10);
}

void ScreenRead::slot_closeVedio()
{
    if (m_pTimer) {
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}
