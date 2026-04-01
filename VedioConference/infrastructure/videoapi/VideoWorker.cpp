#include "VideoWorker.h"
#include "CMediaManager.h"
#include <QDebug>

MediaSendWorker::MediaSendWorker(CMediaManager* manager) : ThreadWorker(), m_manager(manager)
{
}

void MediaSendWorker::slot_dowork(char* buf,int len){
    qDebug()<<"send video threadId:"<<QThread::currentThreadId();
    if(m_manager){
        m_manager->slot_sendVideo(buf,len);
    }
}