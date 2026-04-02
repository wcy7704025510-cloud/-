#include "AudioWorker.h"
#include "CMediaManager.h"
#include <QDebug>

AudioWorker::AudioWorker(CMediaManager *manager) : QObject(nullptr), m_manager(manager)
{
    //创建线程
    m_pThread = new QThread;
    //线程切换
    this->moveToThread(m_pThread);
    //新线程启动
    m_pThread->start();
}

AudioWorker::~AudioWorker()
{
    if(m_pThread){
        m_pThread->quit();
        m_pThread->wait(100);
        if(m_pThread->isRunning()){
            m_pThread->terminate();
            m_pThread->wait(0);
        }
        delete m_pThread;
        m_pThread=nullptr;
    }
}

void AudioWorker::slot_dowork(char* buf,int len)
{
    qDebug()<<"send audio threadId:"<<QThread::currentThreadId();
}
