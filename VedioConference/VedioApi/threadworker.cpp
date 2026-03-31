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
        m_pThread->quit();
        m_pThread->wait(10);
        if(m_pThread->isRunning()){
            m_pThread->terminate();
            m_pThread->wait(0);
        }
        delete m_pThread;
        m_pThread=nullptr;
    }
}


worker::~worker()
{
     qDebug()<<"~worker";
}

void worker::slot_dowork()
{
    qDebug()<<"worker:"<<QThread::currentThreadId();
}
