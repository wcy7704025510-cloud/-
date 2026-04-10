
#include "audio_write.h"
#include "../concurrency/PCMPlayQueue.h"
#include <QDebug>

Audio_Write::Audio_Write(QObject *parent)
    : QObject(parent)
    , dev(0)
    , m_isOpen(false)
    , m_pcmQueue(nullptr)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed in Write:" << SDL_GetError();
        return;
    }

    SDL_AudioSpec wantedSpec;
    SDL_zero(wantedSpec);

    wantedSpec.freq = 48000;
    wantedSpec.format = AUDIO_S16LSB;
    wantedSpec.channels = 1;
    wantedSpec.samples = 960;
    wantedSpec.callback = audioCallback;
    wantedSpec.userdata = this;

    dev = SDL_OpenAudioDevice(NULL, 0, &wantedSpec, nullptr, 0);
    if (dev == 0) {
        qDebug() << "Failed to open audio device:" << SDL_GetError();
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
}

void Audio_Write::setPCMQueue(PCMPlayQueue* queue)
{
    m_pcmQueue = queue;
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

void Audio_Write::slot_net_rx(QByteArray ba)
{
    Q_UNUSED(ba);
}

void Audio_Write::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Write* audio = static_cast<Audio_Write*>(userdata);

    memset(stream, 0, len);

    if (!audio || !audio->m_pcmQueue) {
        return;
    }

    QByteArray pcmData = audio->m_pcmQueue->tryPop();

    if (!pcmData.isEmpty() && pcmData.size() <= len) {
        memcpy(stream, pcmData.constData(), pcmData.size());
    }
}
