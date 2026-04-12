#ifndef SCREEN_PROCESSOR_H
#define SCREEN_PROCESSOR_H

#include <QObject>
#include <QImage>
#include <functional>

class VideoDataQueue;
class H264Encoder;

class ScreenProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ScreenProcessor(QObject *parent = nullptr);
    ~ScreenProcessor();

    void setQueue(VideoDataQueue* queue);
    void setEncoder(H264Encoder* encoder);
    void setSendCallback(std::function<void(char*, int)> callback);

public slots:
    void slot_start();
    void slot_stop();

private:
    void processLoop();

private:
    VideoDataQueue* m_queue;
    H264Encoder* m_encoder;
    std::function<void(char*, int)> m_sendCallback;
    bool m_running;
};

#endif
