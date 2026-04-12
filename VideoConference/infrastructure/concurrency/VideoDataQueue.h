#ifndef VIDEO_DATA_QUEUE_H
#define VIDEO_DATA_QUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QImage>
#include <QObject>

#define VIDEO_QUEUE_MAX_SIZE 3

class VideoDataQueue : public QObject
{
    Q_OBJECT
public:
    explicit VideoDataQueue(QObject *parent = nullptr);

    void push(const QImage& image);
    QImage pop();
    void wakeAll();
    void stop();
    bool isRunning() const { return m_running; }

private:
    QQueue<QImage> m_queue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_running;
};

#endif
