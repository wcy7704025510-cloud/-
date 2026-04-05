#include "audio_write.h"

Audio_Write::Audio_Write(QObject *parent) : QObject(parent), dev(0), m_isOpen(false), decoder(nullptr)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed in Write:" << SDL_GetError();
        return;
    }

    SDL_AudioSpec wantedSpec;
    SDL_AudioSpec obtainedSpec;
    SDL_zero(wantedSpec);
    wantedSpec.freq = 48000;
    wantedSpec.format = AUDIO_S16LSB;
    wantedSpec.channels = 1;
    wantedSpec.samples = 960;
    wantedSpec.callback = audioCallback;
    wantedSpec.userdata = this;

    dev = SDL_OpenAudioDevice(NULL, 0, &wantedSpec, &obtainedSpec, 0);
    if (dev == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
    }

    // ==========================================
    // 按照手册：初始化 Opus 解码器
    // ==========================================
    int err;
    decoder = opus_decoder_create(48000, 1, &err);
    if (err != OPUS_OK) {
        qDebug() << "Opus decoder create failed:" << opus_strerror(err);
    }
}

Audio_Write::~Audio_Write()
{
    if (dev != 0) {
        SDL_PauseAudioDevice(dev, 1);
        SDL_CloseAudioDevice(dev);
        dev = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    // ==========================================
    // 按照手册：回收 销毁解码器
    // ==========================================
    if (decoder) {
        opus_decoder_destroy(decoder);
        decoder = nullptr;
    }
}

void Audio_Write::start()
{
    if (dev == 0 || m_isOpen) return;
    SDL_PauseAudioDevice(dev, 0);
    m_isOpen = true;
}

void Audio_Write::pause()
{
    if (dev == 0 || !m_isOpen) return;
    SDL_PauseAudioDevice(dev, 1);
    m_isOpen = false;
}

void Audio_Write::slot_net_rx(QByteArray recvBuffer)
{
    if( !m_isOpen ) return;
    std::lock_guard<std::mutex> lck( m_mutex );
    m_audioQueue.push_back( recvBuffer );
}

void Audio_Write::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Write * audio = (Audio_Write *)userdata;
    memset( stream , 0 , len );

    std::lock_guard<std::mutex> lck( audio->m_mutex );
    if( !audio->m_audioQueue.empty() )
    {
        QByteArray recvBuffer = audio->m_audioQueue.front();
        audio->m_audioQueue.pop_front();

        // ==========================================
        // 按照手册：进行 Opus 解码
        // ==========================================
        opus_int16 decodedData[4096];
        int frameSizeDecoded = opus_decode(audio->decoder, (const unsigned char*)recvBuffer.data(),
                                           recvBuffer.size(), decodedData, sizeof(decodedData)/sizeof(decodedData[0]), 0);

        if (frameSizeDecoded < 0) {
            qDebug() << "Opus decode error:" << opus_strerror(frameSizeDecoded);
            return;
        }

        // 解码成功后，将解压后的 decodedData 进行混音播放
        // 注意：计算长度时，采样点数(frameSizeDecoded) 需要乘以字节数(sizeof(opus_int16))
        SDL_MixAudioFormat( stream , (uint8_t*)decodedData , AUDIO_S16LSB , frameSizeDecoded * sizeof(opus_int16) , SDL_MIX_MAXVOLUME );
    }
}
