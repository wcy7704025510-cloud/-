#ifndef VEDIOREAD_H
#define VEDIOREAD_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

#include "VedioCommon.h"
#include "threadworker.h"
#include "myfacedetact.h"

class VideoWorker;

class VideoRead : public QObject
{
    Q_OBJECT
public:
    explicit VideoRead(QObject *parent = nullptr);
    ~VideoRead();


signals:
    void SIG_sendVedioFrame(QImage img);

public slots:   //共有方法
    //获取视频帧
    void slot_getVideoFrame();
    //开启视频
    void slot_openVideo();
    //结束视频
    void slot_closeVideo();
    //设置萌拍特效
    void slot_setMoji(int newMoji);
private:        //私有属性
    QTimer* m_timer;                                //定时器类型
    cv::VideoCapture cap;                           //opencv获取图片对象
    QSharedPointer<VideoWorker> m_pVideoWorker;     //线程工作类智能指针
    std::vector<Rect> m_vecFace;                    //识别出的人脸框

    //特效类型
    enum moji_type{moji_tuer=1,moji_hat};
    int m_moji;                                     //用于存储当前的萌拍效果
    QImage m_tuer;                                  //存储特效：兔耳朵
    QImage m_hat;                                   //存储特效：帽子
};

class VideoWorker:public ThreadWorker{
    Q_OBJECT
public slots:
    void slot_setinfo(VideoRead *p){
        m_pVedioRead=p;
    }
    //定时器到时，执行
    void slot_dowork() {
        m_pVedioRead->slot_getVideoFrame();
    }
private:
    VideoRead *m_pVedioRead;
};

#endif // VEDIOREAD_H
