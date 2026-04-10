#ifndef H264DECODER_H
#define H264DECODER_H

// Qt核心框架
#include <QObject>              // Qt对象基类
#include <QImage>             // Qt图像类
#include <QByteArray>         // Qt字节数组

// FFmpeg头文件
extern "C" {
    #include <libavcodec/avcodec.h>   // AVCodecContext、AVFrame、解码器API
    #include <libavutil/imgutils.h>   // 图像操作工具
    #include <libswscale/swscale.h>   // 色彩空间转换和缩放
}

// H264Decoder - H.264视频解码器封装类
// 职责：将H.264码流解码为QImage(RGB格式)显示
// 依赖：FFmpeg/libavcodec/libswscale
// 特点：
//   1. 低延迟解码 - 开启AV_CODEC_FLAG_LOW_DELAY标志
//   2. 动态分辨率 - 自动适应不同尺寸的H.264流
//   3. 色彩转换 - YUV420P -> RGB24 -> QImage
class H264Decoder : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数/析构函数
    explicit H264Decoder(QObject *parent = nullptr);
    ~H264Decoder();

    // initDecoder - 初始化解码器
    // 返回: true=成功, false=失败
    bool initDecoder();

    // decode - 核心解码函数
    // 参数:
    //   h264Data - 输入H.264压缩数据包
    //   outImage - 输出QImage(引用参数，成功时填充)
    // 返回: true=成功解码出一帧, false=失败或无完整帧
    // 注意: 必须使用while循环取出所有解码帧
    bool decode(const QByteArray &h264Data, QImage &outImage);

    // destroy - 释放解码器资源
    void destroy();

private:
    // FFmpeg解码器相关成员
    AVCodecContext *m_pCodecCtx; // 解码器上下文

    AVFrame *m_pFrame;         // YUV解码帧缓冲区
    // 存放H.264解码后的YUV420P原始数据

    AVFrame *m_pFrameRGB;      // RGB帧缓冲区
    // 存放色彩转换后的RGB数据，供QImage使用

    SwsContext *m_pSwsCtx;      // 图像格式转换器
    // 功能: YUV420P -> RGB24

    uint8_t *m_buffer;         // RGB数据内存缓冲区
    // 分配独立内存用于存放转换后的RGB图像数据

    // 用于检测分辨率变化，决定是否重建SwsContext
    int m_currentW;             // 当前解码图像宽度
    int m_currentH;            // 当前解码图像高度

};

#endif // H264DECODER_H
