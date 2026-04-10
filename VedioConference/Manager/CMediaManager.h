
#ifndef CMEDIAMANAGER_H
#define CMEDIAMANAGER_H

#include <QObject>
#include <QImage>
#include <QByteArray>
#include <QMap>
#include <QTimer>
#include <map>

class AudioDataQueue;
class AudioDecodeDataQueue;
class PCMPlayQueue;
class AudioDecodeProcessor;
class Audio_Read;
class AudioProcessor;
class VideoDataQueue;
class VideoRead;
class VideoProcessor;
class ScreenRead;
class ScreenProcessor;
class H264Encoder;
class H264Decoder;
class CRoomManager;
class RoomDialog;
class Audio_Write;
class INetMediator;
class VideoDecodeDataQueue;
class VideoDecodeProcessor;

struct VideoDecodePipeline {
    int userId;
    VideoDecodeDataQueue* queue;
    VideoDecodeProcessor* processor;
    H264Decoder* decoder;
    QThread* thread;
};

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
    void setAudioClient(INetMediator* client) { m_audioClient = client; }
    void setVideoClient(INetMediator* client) { m_videoClient = client; }

public slots:
    void slot_startMediaEngines();
    void slot_AudioPause();
    void slot_AudioStart();
    void slot_VideoPause();
    void slot_VideoStart();
    void slot_ScreenPause();
    void slot_ScreenStart();
    void slot_refreshVideo(int id, QImage& img);
    void slot_clearDevices();

    void slot_audioFrameRq(uint sock, char* buf, int nLen);
    void slot_videoFrameRq(uint sock, char* buf, int nLen);
    void slot_dispatchDecodedVideo(int userId, QImage img);
    void slot_userLeftRoom(int userId);

    void slot_localVideoPreview(QImage img);

private:
    void setupAudioReceivePipeline();
    void setupAudioCapturePipeline();
    void setupVideoCapturePipeline();
    void setupScreenCapturePipeline();

    void audioSendCallback(char* rawData, int rawLen);
    void videoSendCallback(char* rawData, int rawLen);
    void screenSendCallback(char* rawData, int rawLen);

    void destroyVideoPipeline(VideoDecodePipeline* pl);

private:
    AudioDecodeDataQueue* m_audioDecodeQueue;
    PCMPlayQueue* m_pcmPlayQueue;
    AudioDecodeProcessor* m_audioDecodeProcessor;
    QThread* m_audioDecodeThread;

    AudioDataQueue* m_audioQueue;
    Audio_Read* m_audioRead;
    AudioProcessor* m_audioProcessor;
    QThread* m_audioThread;

    VideoDataQueue* m_videoQueue;
    VideoRead* m_videoRead;
    VideoProcessor* m_videoProcessor;
    QThread* m_videoThread;

    VideoDataQueue* m_screenQueue;
    ScreenRead* m_screenRead;
    ScreenProcessor* m_screenProcessor;
    QThread* m_screenThread;

    H264Encoder* m_videoEncoder;
    H264Encoder* m_screenEncoder;

    INetMediator* m_audioClient;
    INetMediator* m_videoClient;

    std::map<int, VideoDecodePipeline*> m_mapVideoPipelines;
    QMap<int, Audio_Write*> m_mapAudioWrite;

    CRoomManager* m_pRoomManager;
    RoomDialog* m_pRoomDialog;
    int m_id;
};

#endif
