#ifndef CMEDIAMANAGER_H
#define CMEDIAMANAGER_H

#include <QObject>
#include <QImage>
#include <QByteArray>
#include <QSharedPointer>

class Audio_Read;
class VideoRead;
class ScreenRead;
class CRoomManager;
class RoomDialog;
class MediaSendWorker;

class CMediaManager : public QObject
{
    Q_OBJECT
public:
    explicit CMediaManager(QObject *parent = nullptr);
    ~CMediaManager();

    void initDevices();
    void setRoomManager(CRoomManager* rm) { m_pRoomManager = rm; }
    void setUserId(int id) { m_id = id; }
    void setRoomDialog(RoomDialog* roomDialog) { m_pRoomDialog = roomDialog; }

signals:
    void SIG_sendVideo(char* buf, int nPackSize);
    
    // 通知 CKernel 发送音频数据
    void SIG_SendAudioData(char* buf, int nLen);
    // 通知 CKernel 发送视频数据
    void SIG_SendVideoData(char* buf, int nLen);

public slots:
    void slot_audioFrameRq(uint sock, char* buf, int nLen);
    void slot_videoFrameRq(uint sock, char* buf, int nLen);
    
    void slot_audioFrame(QByteArray ba);
    void slot_sendVedioFrame(QImage img);
    
    // UI 控制设备的槽函数
    void slot_AudioPause();
    void slot_AudioStart();
    void slot_VideoPause();
    void slot_VideoStart();
    void slot_ScreenPause();
    void slot_ScreenStart();
    
    void slot_refreshVideo(int id, QImage& img);
    
    // 多线程处理槽
    void slot_sendVideo(char* buf, int nPackSize);
    
    // 当离开房间时清理和重置设备
    void slot_clearDevices();
    void slot_setMoji(int type);

private:
    Audio_Read* m_pAudio_Read;
    VideoRead* m_pVedioRead;
    ScreenRead* m_pSreenRead;
    QSharedPointer<MediaSendWorker> m_pMediaSendWorker;

    CRoomManager* m_pRoomManager;
    RoomDialog* m_pRoomDialog;

    int m_id;
};

#endif // CMEDIAMANAGER_H
