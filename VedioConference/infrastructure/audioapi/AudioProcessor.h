#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <functional>

#include "audio_common.h"

class AudioDataQueue;

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent = nullptr);
    ~AudioProcessor();

    void setQueue(AudioDataQueue* queue);
    void setSendCallback(std::function<void(char*, int)> callback);

public slots:
    void slot_start();
    void slot_stop();

private:
    void processLoop();

private:
    AudioDataQueue* m_queue;
    std::function<void(char*, int)> m_sendCallback;
    bool m_running;
    OpusEncoder* m_encoder;
};

#endif
