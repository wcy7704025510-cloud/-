#ifndef VIDEOWORKER_H
#define VIDEOWORKER_H

#include "threadworker.h"

class CMediaManager;

// 专门用于 CMediaManager 发送视频的网络工作类
class MediaSendWorker : public ThreadWorker {
    Q_OBJECT
public:
    explicit MediaSendWorker(CMediaManager* manager);
public slots:
    // 收到信号 发送视频帧 回收额外空间
    void slot_dowork(char* buf, int len);
private:
    CMediaManager* m_manager;
};

#endif // VIDEOWORKER_H