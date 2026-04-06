#ifndef H264DECODER_H
#define H264DECODER_H

#include <QObject>
#include <QImage>
#include <QByteArray>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

class H264Decoder : public QObject
{
    Q_OBJECT
public:
    explicit H264Decoder(QObject *parent = nullptr);
    ~H264Decoder();

    // 1. 初始化解码器
    bool initDecoder();

    // 2. 核心解码函数：传入 H.264 压缩包，如果成功解出一帧，返回 true 并输出 QImage
    bool decode(const QByteArray &h264Data, QImage &outImage);

    // 3. 释放资源
    void destroy();

private:
    AVCodecContext *m_pCodecCtx;
    AVFrame        *m_pFrame;     // 存放解码出的 YUV 原始数据
    AVFrame        *m_pFrameRGB;  // 存放转换后的 RGB 数据
    SwsContext     *m_pSwsCtx;    // 图像格式转换器
    uint8_t        *m_buffer;     // 转换后的画面内存缓存

    int m_currentW;
    int m_currentH;
};

#endif // H264DECODER_H
