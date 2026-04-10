
#ifndef VIDEO_DECODE_DATA_QUEUE_H
#define VIDEO_DECODE_DATA_QUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QByteArray>
#include <QObject>

#define VIDEO_DECODE_QUEUE_MAX_SIZE 3

class VideoDecodeDataQueue : public QObject
{
    Q_OBJECT
public:
    explicit VideoDecodeDataQueue(QObject *parent = nullptr);

    void push(const QByteArray& h264Packet);
    QByteArray pop();
    void wakeAll();
    void stop();
    bool isRunning() const { return m_running; }

private:
    QQueue<QByteArray> m_queue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_running;
};

#endif
