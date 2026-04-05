#include "audio_read.h"

Audio_Read::Audio_Read(QObject *parent) : QObject(parent), dev(0), m_recordState(_Stop), m_isOpen(false), encoder(nullptr)
{
    // SDL 初始化
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed in Read:" << SDL_GetError();
        return;
    }

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;
    SDL_memset(&desiredSpec, 0, sizeof(desiredSpec));
    desiredSpec.freq = 48000;
    desiredSpec.format = AUDIO_S16LSB;
    desiredSpec.channels = 1;
    desiredSpec.samples = 960;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;

    dev = SDL_OpenAudioDevice(NULL, 1, &desiredSpec, &obtainedSpec, 0);
    if (dev == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
    }

    // ==========================================
    // 按照手册：初始化 Opus 编码器
    // ==========================================
    int err;
    encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        qDebug() << "Opus encoder create failed:" << opus_strerror(err);
    }
}

Audio_Read::~Audio_Read()
{
    if (dev != 0) {
        SDL_PauseAudioDevice(dev, 1);
        SDL_CloseAudioDevice(dev);
        dev = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    // ==========================================
    // 按照手册：回收 销毁编码器
    // ==========================================
    if (encoder) {
        opus_encoder_destroy(encoder);
        encoder = nullptr;
    }
}

void Audio_Read::start()
{
    if (dev == 0 || m_isOpen) return;
    SDL_PauseAudioDevice(dev, 0);
    m_recordState = _Record;
    m_isOpen = true;
}

void Audio_Read::pause()
{
    if (dev == 0 || !m_isOpen) return;
    SDL_PauseAudioDevice(dev, 1);
    m_recordState = _Pause;
    m_isOpen = false;
}

void Audio_Read::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Read * audio = (Audio_Read *)userdata;
    if (len < 1920) return;

    // ==========================================
    // 按照手册：进行 Opus 编码
    // ==========================================
    unsigned char opusData[4096];
    int nbBytes = opus_encode(audio->encoder, (const opus_int16*)(stream), 960, opusData, sizeof(opusData));

    if (nbBytes < 0) {
        qDebug() << "Opus encode error:" << opus_strerror(nbBytes);
        return;
    }

    // 只有编码成功，才将压缩后的数据发往上层
    QByteArray sendBuffer((char*)opusData, nbBytes);
    emit audio->SIG_audioFrame(sendBuffer);
}
