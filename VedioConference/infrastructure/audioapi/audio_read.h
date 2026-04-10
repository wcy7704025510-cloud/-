
#ifndef AUDIO_READ_H
#define AUDIO_READ_H

#include <QObject>
#include "audio_common.h"

class AudioDataQueue;

class Audio_Read : public QObject
{
    Q_OBJECT
public:
    explicit Audio_Read(QObject *parent = nullptr);
    ~Audio_Read();

    void setQueue(AudioDataQueue* queue);
    void start();
    void pause();

private:
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID m_device;
    AudioDataQueue* m_queue;
    bool m_isRunning;
};

#endif
