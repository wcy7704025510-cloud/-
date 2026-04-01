#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include <QObject>
#include <QDebug>

// 一个安全的基于 QObject 和 moveToThread 的工作基类
// 将自身移动到 m_pThread 中运行，并在析构时安全地停止和清理线程
class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);
    virtual ~ThreadWorker();

    // 可以提供一个外部直接获取当前线程的接口，但不建议直接操作 m_pThread
    QThread* getThread() const { return m_pThread; }

signals:
    // 如果子类在子线程需要抛出处理完成等信号，可以统一定义在这里
    void SIG_workFinished();

protected:
    QThread *m_pThread;
};

// 基础的测试用 worker 类（可做参考模板）
class worker:public ThreadWorker{
    Q_OBJECT
public:
    ~worker() override;
public slots:
    void slot_dowork();
};

#endif // THREADWORKER_H
