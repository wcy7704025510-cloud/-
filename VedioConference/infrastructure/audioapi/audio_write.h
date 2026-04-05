#ifndef AUDIO_WRITE_H
#define AUDIO_WRITE_H

#include <QObject>
#include "audio_common.h"

#include<mutex>
class Audio_Write : public QObject
{
    Q_OBJECT
public:
    explicit Audio_Write(QObject *parent = nullptr);
    ~Audio_Write();

public slots:
    void slot_net_rx(QByteArray ba);
    void start();
    void pause();

private:
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID dev;
    bool m_isOpen;

    std::list<QByteArray> m_audioQueue;
    std::mutex m_mutex;

    // 按照手册：定义解码器
    OpusDecoder* decoder;
};

#endif // AUDIO_WRITE_H
