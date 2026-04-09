#ifndef VIDEOWORKER_H
#define VIDEOWORKER_H

// 项目内部头文件
#include "threadworker.h"     // 线程工作基类

// 前向声明
class CMediaManager;           // 媒体管理器

// MediaSendWorker - 视频发送工作线程类
// 职责：在独立线程中处理视频数据发送
// 继承自ThreadWorker，获取线程分离的基础功能
// 设计目的：将视频发送操作从主线程分离，避免阻塞UI
// 工作流程：CMediaManager发射SIG_sendVideo信号 -> MediaSendWorker槽函数处理 -> 调用m_manager->slot_sendVideo
class MediaSendWorker : public ThreadWorker
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数
    // 参数: manager - CMediaManager指针，用于回调视频发送
    explicit MediaSendWorker(CMediaManager* manager);

public slots:
    // 工作槽函数
    // 参数: buf - H.264编码后的视频数据包
    //       len - 数据包长度
    // 功能: 在独立线程中调用CMediaManager的视频发送接口
    // 注意: 线程ID与主线程不同，避免阻塞UI
    void slot_dowork(char* buf, int len);

private:
    // 媒体管理器指针
    // 用途: 调用CMediaManager的视频发送功能
    CMediaManager* m_manager;
};

#endif // VIDEOWORKER_H
