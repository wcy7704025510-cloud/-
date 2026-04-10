#include "ThreadWorker.h"

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{
    m_pThread = new QThread;
    this->moveToThread(m_pThread);
    m_pThread->start();
}

ThreadWorker::~ThreadWorker()
{
    if(m_pThread){
        m_pThread->quit();
        m_pThread->wait(100);
        if(m_pThread->isRunning()){
            m_pThread->terminate();
            m_pThread->wait(0);
        }
        delete m_pThread;
        m_pThread = nullptr;
    }
}
