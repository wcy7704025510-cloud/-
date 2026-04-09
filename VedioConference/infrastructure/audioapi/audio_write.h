#ifndef AUDIO_WRITE_H
#define AUDIO_WRITE_H

#include <QObject>

#include "audio_common.h"     // 音频公共定义
#include<mutex>              // 互斥锁，用于线程安全

//接收网络传来的Opus音频数据，解码后通过扬声器播放
//ps:每个远程用户对应一个Audio_Write实例，实现多人同时播放
class Audio_Write : public QObject
{
    Q_OBJECT
public:
    // 构造函数/析构函数
    explicit Audio_Write(QObject *parent = nullptr);
    ~Audio_Write();

public slots:
    // 功能: 将接收到的数据包加入播放队列(ba - Opus编码的音频数据包)
    void slot_net_rx(QByteArray ba);

    // 开始音频播放
    void start();

    // 暂停音频播放
    void pause();

private:
    // SDL音频回调函数(静态成员函数)
    // 参数:
    //   userdata: 用户自定义数据(本类指针)
    //   stream: SDL2期望填充音频数据的缓冲区(输出参数)
    //   len: 缓冲区长度(SDL2期望填充的字节数)
    // 机制: SDL2定期调用此函数获取音频数据进行播放
    static void audioCallback(void *userdata, Uint8 *stream, int len);

private:
    SDL_AudioDeviceID dev;      // SDL2音频设备ID，标识打开的播放设备
    bool m_isOpen;             // 设备是否已打开标志

    // 音频播放队列
    // 类型: std::list<QByteArray> - 链表存储音频数据包
    // 原因: 链表适合频繁的插入/删除操作，保证线程安全
    // 用途: 存储待播放的Opus数据包，先入先出
    std::list<QByteArray> m_audioQueue;

    // 队列互斥锁
    // 原因: 网络接收线程和SDL音频回调线程并发访问队列
    // 作用: 保护m_audioQueue的读写操作
    std::mutex m_mutex;

    // Opus音频解码器
    // 用于将Opus压缩格式解码为PCM原始音频数据
    // 与Audio_Read的encoder相对，一个编码一个解码
    OpusDecoder* decoder;
};

#endif // AUDIO_WRITE_H
