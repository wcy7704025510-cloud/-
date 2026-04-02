#include "threadworker.h"

ThreadWorker:: ThreadWorker(QObject *parent) : QObject(parent)
{
    //创建线程
    m_pThread=new QThread;
    //线程切换
    this->moveToThread(m_pThread);
    //新线程启动
    m_pThread->start();
}

ThreadWorker::~ThreadWorker()
{
    if(m_pThread){
        // 发送退出信号给事件循环
        m_pThread->quit();
        // 阻塞等待线程退出（超时10ms或100ms均可，给个较安全的数值）
        m_pThread->wait(100);
        // 若线程还在运行，则强制终止
        if(m_pThread->isRunning()){
            m_pThread->terminate();
            m_pThread->wait(0);
        }
        delete m_pThread;
        m_pThread=nullptr;
    }
}

