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

// 屏幕抓取：生产者
void ScreenRead::slot_getScreenFrame()
{
    // 获取主屏幕句柄
    QScreen* src = QApplication::primaryScreen();
    // 抓取整个桌面的图像 (耗时较小)
    QPixmap map = src->grabWindow(QApplication::desktop()->winId());
    QImage image = map.toImage();


    // 瞬间推入专属的屏幕共享并发水库
    if (m_queue) {
        m_queue->push(image);
    }

    // 发射本地预览信号
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
