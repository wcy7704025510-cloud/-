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
