#ifndef MYFACEDETACT_H
#define MYFACEDETACT_H

#include <QObject>
#include <QDebug>
#include "./VedioCommon.h"
#include <opencv2\imgproc\types_c.h>

class MyFaceDetact : public QObject
{
    Q_OBJECT
public:
    explicit MyFaceDetact(QObject *parent = nullptr);

signals:

public slots:
    //人脸识别初始化
    static void FaceDetectInit();
    //获取摄像头图片后 识别出人脸的位置 返回对应位置的矩形框
    static void detectAndDisplay(Mat &frame,std::vector<Rect> &faces);
};

#endif // MYFACEDETACT_H
