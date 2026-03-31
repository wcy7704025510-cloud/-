#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);
    ~ThreadWorker();
signals:

protected:
    QThread *m_pThread;
};

class worker:public ThreadWorker{
    Q_OBJECT
public:
    ~worker();
public slots:
    void slot_dowork();
};

#endif // THREADWORKER_H
