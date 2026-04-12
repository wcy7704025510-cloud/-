
#ifndef AUDIO_WRITE_H
#define AUDIO_WRITE_H

#include <QObject>
#include <QMutex>
#include <mutex>

#include <SDL.h>

class PCMPlayQueue;

class Audio_Write : public QObject
{
    Q_OBJECT
public:
    explicit Audio_Write(QObject *parent = nullptr);
    ~Audio_Write();

public slots:
    void start();
    void pause();
    void setPCMQueue(PCMPlayQueue* queue);

private:
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID dev;
    bool m_isOpen;
    PCMPlayQueue* m_pcmQueue;
};

#endif
