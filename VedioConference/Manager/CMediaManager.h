#ifndef CMEDIAMANAGER_H
#define CMEDIAMANAGER_H

#include <QObject>
#include <QImage>
#include <QByteArray>
#include <QSharedPointer>
#include <QMap> // 引入 QMap 管理多用户解码器

class Audio_Read;
class VideoRead;
class ScreenRead;
class CRoomManager;
class RoomDialog;
class MediaSendWorker;

// 前向声明我们写的 H.264 编解码器
class H264Encoder;
class H264Decoder;

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
    void SIG_SendAudioData(char* buf, int nLen);
    void SIG_SendVideoData(char* buf, int nLen);

public slots:
    void slot_audioFrameRq(uint sock, char* buf, int nLen);
    void slot_videoFrameRq(uint sock, char* buf, int nLen);

    void slot_audioFrame(QByteArray ba);
    void slot_sendVedioFrame(QImage img);

    void slot_AudioPause();
    void slot_AudioStart();
    void slot_VideoPause();
    void slot_VideoStart();
    void slot_ScreenPause();
    void slot_ScreenStart();

    void slot_refreshVideo(int id, QImage& img);
    void slot_sendVideo(char* buf, int nPackSize);

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

    // ==========================================
    // 🌟 核心新增：音视频核武器挂载
    // ==========================================
    H264Encoder* m_pH264Encoder;              // 我方专属：单路发送编码器
    QMap<int, H264Decoder*> m_mapDecoders;    // 敌方专属：多路接收解码器池 (用户ID -> 专属解码器)
};

#endif // CMEDIAMANAGER_H
