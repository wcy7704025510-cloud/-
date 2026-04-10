
#include "audio_read.h"
#include "AudioDataQueue.h"
#include <QDebug>

Audio_Read::Audio_Read(QObject *parent)
    : QObject(parent)
    , m_device(0)
    , m_queue(nullptr)
    , m_isRunning(false)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed:" << SDL_GetError();
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

    m_device = SDL_OpenAudioDevice(NULL, 1, &desiredSpec, &obtainedSpec, 0);
    if (m_device == 0) {
        qDebug() << "Failed to open audio device:" << SDL_GetError();
    }
}

Audio_Read::~Audio_Read()
{
    if (m_device != 0) {
        SDL_PauseAudioDevice(m_device, 1);
        SDL_CloseAudioDevice(m_device);
        m_device = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Audio_Read::setQueue(AudioDataQueue* queue)
{
    m_queue = queue;
}

void Audio_Read::start()
{
    if (m_device == 0 || m_isRunning) {
        return;
    }

    SDL_PauseAudioDevice(m_device, 0);
    m_isRunning = true;
}

void Audio_Read::pause()
{
    if (m_device == 0 || !m_isRunning) {
        return;
    }

    SDL_PauseAudioDevice(m_device, 1);
    m_isRunning = false;
}

void Audio_Read::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Read* self = (Audio_Read*)userdata;

    if (!self || !self->m_queue) {
        return;
    }

    if (len < 1920) {
        return;
    }

    QByteArray buffer((const char*)stream, len);
    self->m_queue->push(buffer);
}
