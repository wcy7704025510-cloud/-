
#include"./videoread.h"
VideoRead::VideoRead(QObject *parent) : QObject(parent)
{
    //创建定时器对象
    m_timer=new QTimer;
    //绑定定时信号和获取视频帧的槽函数
//    connect(m_timer,SIGNAL(timeout()),
//            this,SLOT(slot_getVideoFrame()));
    //创建线程工作类对象
    m_pVideoWorker=QSharedPointer<VideoWorker>(new VideoWorker);
    //设置线程工作类对象属性
    m_pVideoWorker->slot_setinfo(this);
    //绑定定时信号和获取视频帧的槽函数
    connect(m_timer,SIGNAL(timeout()),
            m_pVideoWorker.data(),SLOT(slot_dowork()));
    //初始化人脸识别 需要先将对应的.xml文件放到.exe同级目录下
    MyFaceDetact::FaceDetectInit();

    //初始化特效类型
    m_moji=0;
    //加载特效：兔耳朵
    m_tuer.load(":/images/tuer.png");
    //加载特效：帽子
    m_hat.load(":/images/hat.png");
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


    //todo 加滤镜 加人脸识别 加萌拍等功能     opencv图像识别
    //获取摄像头图片后，识别出人脸的位置，返回位置对应的矩形框
    //人脸识别失败，使用上一次的人脸矩形
    std::vector<Rect> faces;
    MyFaceDetact::detectAndDisplay(frame,faces);

    //将 opencv 采集的BGR的图片类型转化为RGB24(24位)的类型
    cvtColor(frame,frame,CV_BGR2RGB);

    //定义QImage对象, 用于发送数据以及图片显示
    QImage image ((unsigned const
    char*)frame.data,frame.cols,frame.rows,QImage::Format_RGB888);
    //将道具绘制到图片上
    QImage tmpimg;
    switch(m_moji){
    case moji_tuer:
        tmpimg=m_tuer;
        break;
    case moji_hat:
        tmpimg=m_hat;
        break;
    }
    //人脸识别成功，修改类属性
    if(faces.size()>0){
        m_vecFace=faces;
    }
    //绘制
    if(tmpimg==m_tuer || tmpimg==m_hat){
        QPainter paint(&image);
        //遍历所有人脸矩形，画道具
        for(int p=0;p<m_vecFace.size();p++){
            Rect rct=m_vecFace[p];
            int x=rct.x+rct.width*0.5-tmpimg.width()*0.5;
            int y=rct.y-tmpimg.height();
            QPoint point(x,y);
            paint.drawImage(point,tmpimg);
        }
    }

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

//设置萌拍特效类型
void VideoRead::slot_setMoji(int newMoji)
{
    m_moji = newMoji;
}
