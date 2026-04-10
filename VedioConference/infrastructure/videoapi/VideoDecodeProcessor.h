
#ifndef VIDEO_DECODE_PROCESSOR_H
#define VIDEO_DECODE_PROCESSOR_H

#include <QObject>
#include <QImage>
#include <QByteArray>

class VideoDecodeDataQueue;
class H264Decoder;

class VideoDecodeProcessor : public QObject
{
    Q_OBJECT
public:
    explicit VideoDecodeProcessor(int userId, QObject *parent = nullptr);
    ~VideoDecodeProcessor();

    void setQueue(VideoDecodeDataQueue* queue);
    void setDecoder(H264Decoder* decoder);

signals:
    void SIG_decodedFrame(int userId, QImage img);

public slots:
    void slot_start();
    void slot_stop();

private:
    int m_userId;
    VideoDecodeDataQueue* m_queue;
    H264Decoder* m_decoder;
    bool m_running;
};

#endif
