#ifndef SCREENREAD_H
#define SCREENREAD_H

#include <QObject>
#include<QTimer>
#include<QApplication>
#include<QDesktopWidget>
#include<QBuffer>
#include<QScreen>
#include<QImage>

#include "./VedioCommon.h"
#include "./threadworker.h"

class ScreenWorker;

class ScreenRead : public QObject
{
    Q_OBJECT
public:
    explicit ScreenRead(QObject *parent = nullptr);
    ~ScreenRead();
signals:
    void SIG_getScreenFrame(QImage img);
public slots:
    void slot_getScreenFrame();
    void slot_openVedio();
    void slot_closeVedio();
private:
    QTimer *m_pTimer;
    //线程工作类智能指针
    QSharedPointer<ScreenWorker> m_pScreenWorker;
};

class ScreenWorker:public ThreadWorker{
    Q_OBJECT
public slots:
    void slot_setinfo(ScreenRead *p){
        m_pVedioRead=p;
    }
    //定时器到时，执行
    void slot_dowork() {
        m_pVedioRead->slot_getScreenFrame();
    }
private:
    ScreenRead *m_pVedioRead;
};

#endif // SCREENREAD_H
