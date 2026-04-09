#include "audio_write.h"          // 音频播放类头文件

// Audio_Write构造函数
// 功能: 初始化SDL音频子系统、打开扬声器设备、创建Opus解码器
Audio_Write::Audio_Write(QObject *parent)
    : QObject(parent)
    , dev(0)                      // SDL音频设备ID，0表示未打开
    , m_isOpen(false)             // 设备未打开
    , decoder(nullptr)              // 解码器指针
{
    // 初始化SDL音频子系统(与Audio_Read相同)
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        qDebug() << "SDL Audio Init Failed in Write:" << SDL_GetError();
        return;
    }

    // 定义期望的音频规格(与Audio_Read保持一致)
    SDL_AudioSpec wantedSpec;
    SDL_AudioSpec obtainedSpec;   // 实际获得的音频规格

    // 清零wanteSpec结构体
    SDL_zero(wantedSpec);

    // 采样率: 48000Hz (必须与编码端一致)
    wantedSpec.freq = 48000;

    // 音频格式: AUDIO_S16LSB (有符号16位小端序)
    wantedSpec.format = AUDIO_S16LSB;

    // 声道数: 1 (单声道)
    wantedSpec.channels = 1;

    // 采样缓冲区大小: 960 (20ms音频数据)
    wantedSpec.samples = 960;

    // 音频回调函数
    wantedSpec.callback = audioCallback;
    wantedSpec.userdata = this;

    // 打开音频播放设备
    // 参数2: 0表示播放模式(1=录音，0=播放)
    dev = SDL_OpenAudioDevice(NULL, 0, &wantedSpec, &obtainedSpec, 0);
    if (dev == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
    }

    // 初始化Opus解码器
    // opus_decoder_create参数:
    //   48000: 采样率(Hz)，必须与编码器一致
    //   1: 声道数，必须与编码器一致
    //   &err: 错误码输出参数
    // 返回: 解码器实例指针
    int err;
    decoder = opus_decoder_create(48000, 1, &err);
    if (err != OPUS_OK) {
        qDebug() << "Opus decoder create failed:" << opus_strerror(err);
    }
}

// Audio_Write析构函数
// 功能: 清理SDL音频设备、销毁Opus解码器
Audio_Write::~Audio_Write()
{
    // 关闭音频设备
    if (dev != 0) {
        SDL_PauseAudioDevice(dev, 1);   // 暂停设备
        SDL_CloseAudioDevice(dev);       // 关闭设备
        dev = 0;
    }

    // 退出SDL音频子系统
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    // 销毁Opus解码器
    if (decoder) {
        opus_decoder_destroy(decoder);
        decoder = nullptr;
    }
}

// start - 开始音频播放
void Audio_Write::start()
{
    if (dev == 0 || m_isOpen) return;
    SDL_PauseAudioDevice(dev, 0);  // 参数0表示恢复播放
    m_isOpen = true;
}

// pause - 暂停音频播放
void Audio_Write::pause()
{
    if (dev == 0 || !m_isOpen) return;
    SDL_PauseAudioDevice(dev, 1);  // 参数1表示暂停
    m_isOpen = false;
}

// slot_net_rx - 网络音频数据接收槽
// 参数: recvBuffer - 接收到的Opus编码音频数据包
// 功能: 将数据包加入播放队列，等待SDL回调取出播放
void Audio_Write::slot_net_rx(QByteArray recvBuffer)
{
    // 检查是否在播放状态
    if( !m_isOpen ) return;

    // 加锁保护队列(防止与audioCallback线程冲突)
    std::lock_guard<std::mutex> lck( m_mutex );

    // 将数据包加入队列尾部
    // 队列保证FIFO(先进先出)播放顺序
    m_audioQueue.push_back( recvBuffer );
}

//SDL内部音频线程定期调用此函数获取音频数据播放
void Audio_Write::audioCallback(void *userdata, Uint8 *stream, int len)
{
    // 获取Audio_Write对象指针
    Audio_Write * audio = (Audio_Write *)userdata;

    // 清零输出缓冲区
    // 重要：如果没有音频数据，SDL会播放静音
    memset( stream , 0 , len );

    // 加锁保护队列
    std::lock_guard<std::mutex> lck( audio->m_mutex );

    // 检查队列是否为空
    if( !audio->m_audioQueue.empty() )
    {
        // 取出队列头部的数据包
        QByteArray recvBuffer = audio->m_audioQueue.front();
        audio->m_audioQueue.pop_front();  // 取出后删除

        // Opus解码
        // opus_decode参数:
        //   decoder: Opus解码器实例
        //   recvBuffer.data(): 输入Opus数据
        //   recvBuffer.size(): 输入数据长度
        //   decodedData: 输出PCM数据缓冲区
        //   sizeof(decodedData)/sizeof(decodedData[0]): 最大采样点数
        //   0: 标志位(0=FEC解码关闭)
        // 返回: 实际解码的采样点数，负数表示错误
        opus_int16 decodedData[4096];
        int frameSizeDecoded = opus_decode(audio->decoder, (const unsigned char*)recvBuffer.data(),
                                           recvBuffer.size(), decodedData, sizeof(decodedData)/sizeof(decodedData[0]), 0);

        // 检查解码是否成功
        if (frameSizeDecoded < 0) {
            qDebug() << "Opus decode error:" << opus_strerror(frameSizeDecoded);
            return;
        }

        // SDL混音播放
        // SDL_MixAudioFormat: 将音频数据混合到输出缓冲区
        // 参数:
        //   stream: SDL输出缓冲区
        //   (uint8_t*)decodedData: 源音频数据(解码后的PCM)
        //   AUDIO_S16LSB: 音频格式(必须匹配)
        //   frameSizeDecoded * sizeof(opus_int16): 数据长度(采样点数×每采样字节数)
        //   SDL_MIX_MAXVOLUME: 播放音量(最大音量0-128)
        // 注意: frameSizeDecoded是采样点数，需要转换为字节数
        SDL_MixAudioFormat( stream , (uint8_t*)decodedData , AUDIO_S16LSB , frameSizeDecoded * sizeof(opus_int16) , SDL_MIX_MAXVOLUME );
    }
}
