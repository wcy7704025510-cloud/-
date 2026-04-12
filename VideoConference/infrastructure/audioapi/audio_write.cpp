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
    wantedSpec.samples = 960; // 同样锁定 20ms 一次的回调频率
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
        SDL_PauseAudioDevice(dev, 1); // 1表示强制暂停声卡
        SDL_CloseAudioDevice(dev);    // 释放声卡硬件句柄
        dev = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO); // 卸载 SDL 音频子系统
}
/*
声卡播放类自己是不生产数据的，它只负责播。
这个函数就是让外部的大管家（CMediaManager）把装满纯净 PCM 声音的“水库”的指针递给它。
接上这根管子后，底层的 audioCallback 才知道去哪里抽水。
*/
void Audio_Write::setPCMQueue(PCMPlayQueue* queue) {
    m_pcmQueue = queue;
}
void Audio_Write::start()
{
    if (dev == 0 || m_isOpen) return; // 防御：设备没开或者已经在播了，直接退
    SDL_PauseAudioDevice(dev, 0);     // 0 表示解除暂停（开始播放）
    m_isOpen = true;
}
void Audio_Write::pause()
{
    if (dev == 0 || !m_isOpen) return; // 防御：设备没开或者已经暂停了，直接退
    SDL_PauseAudioDevice(dev, 1);      // 1 表示暂停
    m_isOpen = false;
}


// 这是由物理声卡时钟强制触发的，一旦耗时过长或死锁，系统直接崩溃或爆音。
void Audio_Write::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Write* audio = static_cast<Audio_Write*>(userdata);
    // 进来的第一步，不问三七二十一，先铺一层全0的静音数据。
    // 如果后续网络卡顿没拿到数据，声卡播放的也是平滑的静音，而绝不是刺耳的内存底噪（爆音）。
    memset(stream, 0, len);

    if (!audio || !audio->m_pcmQueue) {
        return;
    }

    // 绝对禁止在这里使用 wait() 阻塞！
    // 调用定制的 tryPop()，有水就立刻打水，没水立刻返回（依赖刚才的静音兜底）
    QByteArray pcmData = audio->m_pcmQueue->tryPop();

    if (!pcmData.isEmpty() && pcmData.size() <= len) {
        // 使用极其高效的底层 memcpy 完成数据交接，毫秒级内完成回调！
        memcpy(stream, pcmData.constData(), pcmData.size());
    }
}
