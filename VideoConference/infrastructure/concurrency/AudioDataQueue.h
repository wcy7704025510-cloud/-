#ifndef AUDIO_DATA_QUEUE_H
#define AUDIO_DATA_QUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QByteArray>
#include <QObject>

#define AUDIO_QUEUE_MAX_SIZE 10

class AudioDataQueue : public QObject
{
    Q_OBJECT
public:
    explicit AudioDataQueue(QObject *parent = nullptr);

    void push(const QByteArray& data);
    QByteArray pop();
    void stop();
    bool isRunning() const { return m_running; }

private:
    QQueue<QByteArray> m_queue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    //它让消费者线程能够区分"队列暂时无数据（正常等待）"和"队列永久停止（安全退出）"，从而实现 零死锁的线程终止
    bool m_running;
};

#endif
