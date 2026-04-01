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
    //获取桌面分辨率 得到矩形桌面
    //QRect deskRect  =  QApplication::desktop()->screenGeometry();
    //获取桌面图片
    QPixmap map = src->grabWindow( QApplication::desktop()->winId()/* ,0,0 , deskRect.width() ,
    deskRect.height()*/);
    QImage image = map.toImage();
    //设置图片大小及缩放比例
    //image = image.scaled( IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatio);
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
