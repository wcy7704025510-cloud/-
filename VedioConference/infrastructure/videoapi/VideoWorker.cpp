#include "VideoWorker.h"           // 视频工作线程头文件
#include "CMediaManager.h"         // 媒体管理器头文件
#include <QDebug>                 // Qt调试输出

// MediaSendWorker构造函数
// 参数: manager - CMediaManager指针
MediaSendWorker::MediaSendWorker(CMediaManager* manager)
    : ThreadWorker()           // 调用基类构造函数
    , m_manager(manager)
{
}

// slot_dowork - 视频发送工作槽函数
// 参数: buf - H.264编码后的视频数据包
//       len - 数据包长度
// 功能: 在独立线程中处理视频数据发送
void MediaSendWorker::slot_dowork(char* buf,int len)
{
    // 检查媒体管理器是否有效
    if(m_manager){
        // 调用CMediaManager的视频发送接口
        // 注意：虽然调用在同一线程，但信号槽已切换到MediaSendWorker所在线程
        m_manager->slot_sendVideo(buf,len);
    }
}
