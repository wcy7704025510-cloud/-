#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include <QObject>
#include <QDebug>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);
    virtual ~ThreadWorker();

    QThread* getThread() const { return m_pThread; }

signals:
    void SIG_workFinished();

protected:
    QThread *m_pThread;
};

#endif
