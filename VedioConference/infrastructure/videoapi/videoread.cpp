
#include"./videoread.h"
using namespace cv;
VideoRead::VideoRead(QObject *parent) : QObject(parent)
{
    //创建定时器对象
    m_timer=new QTimer;
    //创建线程工作类对象
    m_pVideoWorker=QSharedPointer<VideoWorker>(new VideoWorker);
    //设置线程工作类对象属性
    m_pVideoWorker->slot_setinfo(this);
    //绑定定时信号和获取视频帧的槽函数
    connect(m_timer,SIGNAL(timeout()),
            m_pVideoWorker.data(),SLOT(slot_dowork()));
}

VideoRead::~VideoRead()
{
    //回收定时器对象
    if(m_timer){
        m_timer->stop();
        delete m_timer;
        m_timer=nullptr;
    }
}

//获取视频帧
void VideoRead::slot_getVideoFrame()
{
    Mat frame;              //Mat对象存储图像
    if( !cap.read(frame) )  //从摄像头对象读取数据
    {
        return;
    }

    //将 opencv 采集的BGR的图片类型转化为RGB24(24位)的类型
    cvtColor(frame,frame,CV_BGR2RGB);

    //定义QImage对象, 用于发送数据以及图片显示
    QImage image ((unsigned const
    char*)frame.data,frame.cols,frame.rows,QImage::Format_RGB888);

    //转化为大小更小的图片
    image = image.scaled( IMAGE_WIDTH,IMAGE_HEIGHT, Qt::KeepAspectRatio );
    //发送图片
    Q_EMIT SIG_sendVedioFrame( image );
}

//开启视频
void VideoRead::slot_openVideo()
{
    //开启定时器
    m_timer->start(1000/FRAME_RATE-10);
    //打开摄像头
    cap.open(0);    //开启默认摄像头
    if(!cap.isOpened()){
        QMessageBox::information(NULL,tr("提示"),tr("视频没有打开"));
        return;
    }
}

//关闭视频
void VideoRead::slot_closeVideo()
{
    //关闭定时器
    m_timer->stop();
    //关闭摄像头
    if(cap.isOpened()){
        cap.release();
    }
}
