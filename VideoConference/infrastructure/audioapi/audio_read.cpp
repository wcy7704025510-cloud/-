#include "audio_read.h"
#include "AudioDataQueue.h"
#include <QDebug>

Audio_Read::Audio_Read(QObject *parent)
    : QObject(parent)
    , m_device(0)
    , m_queue(nullptr)
    , m_isRunning(false)
{
    // 初始化 SDL 音频子系统
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed:" << SDL_GetError();
        return;
    }

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;

    // 硬件级音频规格配置
    SDL_memset(&desiredSpec, 0, sizeof(desiredSpec));
    desiredSpec.freq = 48000;      // 录音室级超高采样率
    desiredSpec.format = AUDIO_S16LSB; // 16位有符号整数 (小端)
    desiredSpec.channels = 1;      // 语音通信通常使用单声道即可
    desiredSpec.samples = 960;     // 核心：48000Hz下，960个采样点正好是 20ms！决定了底层硬件中断的触发频率
    desiredSpec.callback = audioCallback; // 绑定硬件时钟中断回调
    desiredSpec.userdata = this;

    m_device=SDL_OpenAudioDevice(
        NULL,        // 1. 设备名：用默认声卡（填NULL）
        1,           // 2. 0=播放  1=录制（采集、录音）
        &desiredSpec,// 3. 你想要的音频参数（采样率、通道、格式）
        &obtainedSpec,//4. 系统实际给你的参数（返回值）
        0            // 5. 标志：0=默认
    );
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

void Audio_Read::setQueue(AudioDataQueue* queue) {
    m_queue = queue;
}

void Audio_Read::start()
{
    if (m_device == 0 || m_isRunning) return;
    SDL_PauseAudioDevice(m_device, 0); // 开启硬件采集
    m_isRunning = true;
}

void Audio_Read::pause()
{
    if (m_device == 0 || !m_isRunning) return;
    SDL_PauseAudioDevice(m_device, 1);
    m_isRunning = false;
}

// 生产者：硬件时钟中断回调(每20ms被系统底层强制唤醒一次)
void Audio_Read::audioCallback(void *userdata, Uint8 *stream, int len)
{
    Audio_Read* self = (Audio_Read*)userdata;

    if (!self || !self->m_queue) return;

    // 数据校验 20ms 的 48000Hz 16-bit 单声道数据，其字节数绝对是 1920 字节 (48000 * 2 * 1 * 0.02)
    // 防止底层由于抖动传入异常大小的数据导致崩溃
    if (len < 1920) return;


    // 仅仅是将流强转为 QByteArray，然后立刻推入并发水库。
    // 整个回调过程绝不涉及任何业务逻辑和耗时运算，保证物理声卡永不被阻塞！
    QByteArray buffer((const char*)stream, len);
    self->m_queue->push(buffer);
}
