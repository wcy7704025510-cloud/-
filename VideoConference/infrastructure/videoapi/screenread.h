#ifndef SCREENREAD_H
#define SCREENREAD_H

#include <QObject>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QBuffer>
#include <QScreen>
#include <QImage>
#include <QSharedPointer>

#include "VideoCommon.h"

class VideoDataQueue;

class ScreenRead : public QObject
{
    Q_OBJECT
public:
    explicit ScreenRead(QObject *parent = nullptr);
    ~ScreenRead();

    void setQueue(VideoDataQueue* queue);

signals:
    void SIG_getScreenFrame(QImage img);

public slots:
    void slot_getScreenFrame();
    void slot_openVedio();
    void slot_closeVedio();

private:
    QTimer* m_pTimer;
    VideoDataQueue* m_queue;
};

#endif
