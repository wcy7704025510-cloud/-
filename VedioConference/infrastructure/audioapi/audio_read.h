#ifndef AUDIO_READ_H
#define AUDIO_READ_H

#include <QObject>
#include "audio_common.h"


class Audio_Read : public QObject
{
    Q_OBJECT
public:
    explicit Audio_Read(QObject *parent = nullptr);
    ~Audio_Read();

    enum audio_state { _Stop, _Record, _Pause };

signals:
    // 现在发出的不再是庞大的 PCM，而是压缩后的微小 Opus 数据包
    void SIG_audioFrame(QByteArray ba);

public slots:
    void start();
    void pause();

private:
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID dev;
    int m_recordState;
    bool m_isOpen;

    // 按照手册：定义编码器
    OpusEncoder* encoder;
};

#endif // AUDIO_READ_H
