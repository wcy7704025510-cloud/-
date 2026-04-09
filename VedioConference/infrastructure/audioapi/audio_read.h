#ifndef AUDIO_READ_H
#define AUDIO_READ_H

#include <QObject>

#include "audio_common.h"

//音频采集与编码：从麦克风使用SDL采集PCM音频数据，使用Opus编码器压缩后发送
//麦克风 -> SDL2采集 -> PCM原始数据 -> Opus编码 -> 网络发送
class Audio_Read : public QObject
{
    Q_OBJECT
public:
    // 构造函数/析构函数
    explicit Audio_Read(QObject *parent = nullptr);
    ~Audio_Read();

    // 音频采集状态
    // _Stop: 停止状态
    // _Record: 正在录制/采集
    // _Pause: 暂停状态
    enum audio_state { _Stop, _Record, _Pause };

signals:
    //将采集到的信号传递给音视频处理类
    void SIG_audioFrame(QByteArray ba);

public slots:
    // 开始音频采集
    void start();

    // 暂停音频采集
    // 功能: 暂停SDL2音频设备，但不关闭，保留设备句柄
    void pause();

private:
    // SDL2音频回调函数(静态成员函数)
    // 参数:
    //   userdata: 用户自定义数据(本类指针)
    //   stream: SDL2填充音频数据的缓冲区指针(输出参数)
    //   len: 缓冲区长度(SDL2期望填充的字节数)
    // 机制: SDL2音频线程定期调用此函数获取音频数据
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID dev;      // SDL2音频设备ID，标识打开的音频设备
    int m_recordState;          // 当前采集状态(枚举: _Stop/_Record/_Pause)
    bool m_isOpen;             // 设备是否已打开标志

    // Opus音频编码器
    // 用于将PCM原始音频数据压缩为Opus格式
    OpusEncoder* encoder;
};

#endif // AUDIO_READ_H
