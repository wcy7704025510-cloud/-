#ifndef VEDIOREAD_H
#define VEDIOREAD_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QMessageBox>
#include <QSharedPointer>

#include "VideoCommon.h"

#include <opencv2/opencv.hpp>

class VideoDataQueue;

class VideoRead : public QObject
{
    Q_OBJECT
public:
    explicit VideoRead(QObject *parent = nullptr);
    ~VideoRead();

    void setQueue(VideoDataQueue* queue);

signals:
    void SIG_sendVedioFrame(QImage img);

public slots:
    void slot_getVideoFrame();
    void slot_openVideo();
    void slot_closeVideo();

private:
    QTimer* m_timer;
    cv::VideoCapture cap;
    VideoDataQueue* m_queue;
};

#endif
