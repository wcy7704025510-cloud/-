#include "screenread.h"

ScreenRead::ScreenRead(QObject *parent) : QObject(parent)
{
    //创建定时器对象
    m_pTimer=new QTimer;
    //绑定定时器到时信号和对应的槽函数
//    connect(m_pTimer,SIGNAL(timeout()),
//            this,SLOT(slot_getScreenFrame()));
    //创建线程工作类对象
    m_pScreenWorker=QSharedPointer<ScreenWorker>(new ScreenWorker);
    //设置线程工作类对象属性
    m_pScreenWorker->slot_setinfo(this);
    //绑定定时器到时信号和对应的槽函数
    connect(m_pTimer,SIGNAL(timeout()),
            m_pScreenWorker.data(),SLOT(slot_dowork()));
}

ScreenRead::~ScreenRead()
{
    if(m_pTimer){
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer=nullptr;
    }
}

void ScreenRead::slot_getScreenFrame()
{
    //获取桌面对象
    QScreen *src = QApplication::primaryScreen();
    //获取桌面图片
    QPixmap map = src->grabWindow( QApplication::desktop()->winId() );
    QImage image = map.toImage();

    // 转换为RGB888格式，确保与H264Encoder期望的AV_PIX_FMT_RGB24一致
    // Windows桌面截图可能是ARGB32或其他格式，必须明确转换为RGB888
    if (image.format() != QImage::Format_RGB888) {
        image = image.convertToFormat(QImage::Format_RGB888);
    }

    // 移除Qt的scaled调用，交给H264Encoder使用FFmpeg进行缩放
    // 这样做的好处：
    // 1. FFmpeg的缩放算法质量更高
    // 2. 支持任意分辨率的桌面
    // 3. 与视频处理保持一致

    Q_EMIT  SIG_getScreenFrame(image);
}

void ScreenRead::slot_openVedio()
{
    m_pTimer->start(1000/FRAME_RATE-10);
}

void ScreenRead::slot_closeVedio()
{
    m_pTimer->stop();
}
