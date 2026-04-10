#ifndef DECODE_DATA_QUEUE_H
#define DECODE_DATA_QUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QByteArray>
#include <QObject>

#define DECODE_QUEUE_MAX_SIZE 10

class DecodeDataQueue : public QObject
{
    Q_OBJECT
public:
    explicit DecodeDataQueue(QObject *parent = nullptr);

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
