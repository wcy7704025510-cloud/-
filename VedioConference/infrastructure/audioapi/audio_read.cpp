#include "audio_read.h"           // 音频采集类头文件

// Audio_Read构造函数
// 功能: 初始化SDL音频子系统、打开麦克风设备、创建Opus编码器
Audio_Read::Audio_Read(QObject *parent)
    : QObject(parent)
    , dev(0)                      // SDL音频设备ID，0表示未打开
    , m_recordState(_Stop)        // 初始状态为停止
    , m_isOpen(false)             // 设备未打开
    , encoder(nullptr)             // 编码器指针
{
    // 初始化SDL音频子系统
    // SDL_INIT_AUDIO: 只初始化音频模块，不初始化视频等
    // 返回0表示成功，非0表示失败
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed in Read:" << SDL_GetError();
        return;
    }

    // 定义期望的音频规格(Desired Specification)
    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;   // 实际获得的音频规格(可能与期望不同)

    // 清零desiredSpec结构体
    SDL_memset(&desiredSpec, 0, sizeof(desiredSpec));

    // 采样率: 48000Hz (48kHz)
    // 48kHz是高质量音频采样率，也是Opus编码器的标准采样率
    desiredSpec.freq = 48000;

    // 音频格式: AUDIO_S16LSB
    // 有符号16位小端序(Signed 16-bit Little-Endian)
    // 每个采样点占用2字节
    desiredSpec.format = AUDIO_S16LSB;

    // 声道数: 1 (单声道)
    // 单声道足以进行语音通话，节省带宽
    desiredSpec.channels = 1;

    // 采样缓冲区大小: 960
    // 960个采样点 × 2字节 = 1920字节 ≈ 20ms音频数据
    // (960 / 48000 = 0.02秒 = 20毫秒)
    // 20ms是Opus编码的标准帧长度
    desiredSpec.samples = 960;

    // 音频回调函数: 当SDL准备好音频数据时调用
    // 这是SDL音频API的核心机制
    desiredSpec.callback = audioCallback;

    // 用户数据: 将本类指针传入回调函数
    // 回调函数通过此指针访问类的成员
    desiredSpec.userdata = this;

    // 打开音频设备
    // 参数1: NULL表示使用默认音频设备
    // 参数2: 1表示录音模式(1=录音，0=播放)
    // 参数3: 期望的音频规格
    // 参数4: 实际获得的音频规格(输出)
    // 参数5: 0表示不改变音频规格(允许SDL调整)
    dev = SDL_OpenAudioDevice(NULL, 1, &desiredSpec, &obtainedSpec, 0);
    if (dev == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
    }

    // 初始化Opus编码器
    // opus_encoder_create参数:
    //   48000: 采样率(Hz)
    //   1: 声道数(单声道)
    //   OPUS_APPLICATION_VOIP: 编码模式(适合语音通话，还有AUDIO/LOWDELAY)
    //   &err: 错误码输出参数
    int err;
    encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        qDebug() << "Opus encoder create failed:" << opus_strerror(err);
    }
}

// Audio_Read析构函数
// 功能: 清理SDL音频设备、销毁Opus编码器
Audio_Read::~Audio_Read()
{
    // 关闭音频设备
    if (dev != 0) {
        // 暂停音频设备
        // 参数1: 设备ID
        // 参数2: 1表示暂停，0表示恢复
        SDL_PauseAudioDevice(dev, 1);

        // 关闭音频设备，释放系统资源
        SDL_CloseAudioDevice(dev);
        dev = 0;
    }

    // 退出SDL音频子系统
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    // 销毁Opus编码器，释放内存
    if (encoder) {
        opus_encoder_destroy(encoder);
        encoder = nullptr;
    }
}

// start - 开始音频采集
// 功能: 恢复SDL音频设备，开始采集麦克风音频
void Audio_Read::start()
{
    // 防御性检查: 设备未打开或已在采集则直接返回
    if (dev == 0 || m_isOpen) return;

    // 恢复音频设备
    // 参数2=0表示开始播放/录音
    SDL_PauseAudioDevice(dev, 0);

    // 更新状态
    m_recordState = _Record;
    m_isOpen = true;
}

// pause - 暂停音频采集
// 功能: 暂停SDL音频设备，停止采集但不关闭设备
void Audio_Read::pause()
{
    // 防御性检查
    if (dev == 0 || !m_isOpen) return;

    // 暂停音频设备
    // 参数2=1表示暂停
    SDL_PauseAudioDevice(dev, 1);

    // 更新状态
    m_recordState = _Pause;
    m_isOpen = false;
}

// audioCallback - SDL音频回调函数(静态成员函数)
// 参数:
//   userdata: 用户数据指针(Audio_Read对象)
//   stream: SDL填充音频数据的缓冲区(输出参数)
//   len: 缓冲区长度(SDL期望填充的字节数)
// 机制: SDL内部音频线程定期调用此函数获取数据
void Audio_Read::audioCallback(void *userdata, Uint8 *stream, int len)
{
    // 从userdata获取Audio_Read对象指针
    Audio_Read * audio = (Audio_Read *)userdata;

    // 防御性检查: 数据长度小于20ms音频数据则忽略
    // 960采样点 × 2字节 = 1920字节
    if (len < 1920) return;




    // Opus编码
    // opus_encode参数:
    //   encoder: Opus编码器实例
    //   stream: 输入PCM数据(signed 16-bit, little-endian)
    //   960: 采样点数量(必须与初始化时samples一致)
    //   opusData: 编码输出缓冲区
    //   sizeof(opusData): 输出缓冲区大小
    // 返回值: 编码后数据长度，负数表示错误
    unsigned char opusData[4096];
    int nbBytes = opus_encode(audio->encoder, (const opus_int16*)(stream), 960, opusData, sizeof(opusData));
    // 检查编码是否成功
    if (nbBytes < 0) {
        qDebug() << "Opus encode error:" << opus_strerror(nbBytes);
        return;
    }

    // 编码成功，发送信号
    // 将Opus数据封装为QByteArray发送
    QByteArray sendBuffer((char*)opusData, nbBytes);

    // 发射音频帧信号，供上层(CMediaManager)处理
    emit audio->SIG_audioFrame(sendBuffer);
}
