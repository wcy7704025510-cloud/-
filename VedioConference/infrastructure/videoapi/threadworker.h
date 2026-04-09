#ifndef THREADWORKER_H
#define THREADWORKER_H

// Qt线程和对象框架
#include <QThread>   // Qt线程类
#include <QObject>   // Qt对象基类
#include <QDebug>    // Qt调试输出

// ThreadWorker - 线程工作基类
// 职责：提供将QObject移动到独立线程执行的通用模式
// 设计目的：封装线程创建、移动、销毁的通用逻辑
// 使用方式：派生类继承，添加具体槽函数，moveToThread自动处理
// 特性：
//   1. 构造函数创建独立线程
//   2. 析构函数安全退出线程
//   3. 提供线程对象获取接口
class ThreadWorker : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数
    // 功能: 创建独立工作线程，但此时Worker对象仍在主线程
    explicit ThreadWorker(QObject *parent = nullptr);

    // 析构函数
    // 功能: 安全停止线程并释放资源
    virtual ~ThreadWorker();

    // 获取工作线程指针
    // 返回: 工作线程QThread指针
    // 用途: 外部可获取线程对象(但不建议直接操作)
    QThread* getThread() const { return m_pThread; }

signals:
    // 工作完成信号
    // 用途: 子类可在子线程中发射此信号通知工作完成
    void SIG_workFinished();

protected:
    // 工作线程指针
    // 用途: 管理独立线程的生命周期
    QThread *m_pThread;
};

#endif // THREADWORKER_H
