#ifndef H264ENCODER_H
#define H264ENCODER_H

#include <QObject>
#include <QImage>
#include <QByteArray>

// 必须使用 extern "C" 包裹 FFmpeg 头文件，否则 MinGW 会报找不到引用的错误
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

class H264Encoder : public QObject
{
    Q_OBJECT
public:
    explicit H264Encoder(QObject *parent = nullptr);
    ~H264Encoder();

    // 1. 初始化编码器 (传入你要压缩的视频宽高和帧率)
    bool initEncoder(int width, int height, int fps);

    // 2. 核心压缩函数：送入纯 RGB 图像，返回 H.264 极致压缩包
    QByteArray encode(const QImage &image);

    // 3. 释放资源
    void destroy();

private:
    AVCodecContext *m_pCodecCtx; // 编码器上下文
    AVFrame        *m_pFrameYUV; // 用于存放转换后的 YUV420P 数据
    SwsContext     *m_pSwsCtx;   // 图像色彩空间转换器 (RGB -> YUV)

    int m_width;
    int m_height;
    int m_fps;
    int m_frameIndex;            // 时间戳记录器
};

#endif // H264ENCODER_H
