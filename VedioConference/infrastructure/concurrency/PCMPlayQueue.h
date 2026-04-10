
#ifndef PCM_PLAY_QUEUE_H
#define PCM_PLAY_QUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QByteArray>
#include <QObject>

#define PCM_PLAY_QUEUE_MAX_SIZE 50

class PCMPlayQueue : public QObject
{
    Q_OBJECT
public:
    explicit PCMPlayQueue(QObject *parent = nullptr);

    void push(const QByteArray& pcmData);
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
