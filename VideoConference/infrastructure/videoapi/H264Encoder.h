#ifndef H264ENCODER_H
#define H264ENCODER_H

// Qt核心框架
#include <QObject>              // Qt对象基类
#include <QImage>             // Qt图像类
#include <QByteArray>         // Qt字节数组

// FFmpeg视频编解码库头文件
// extern "C"是C++关键字，强制FFmpeg(纯C库)按C方式编译，避免名字修饰问题
extern "C" {
    #include <libavcodec/avcodec.h>   // AVCodecContext、AVFrame、编码器API
    #include <libavutil/imgutils.h>   // 图像操作工具(分配、复制)
    #include <libavutil/opt.h>        // 选项设置API
    #include <libswscale/swscale.h>   // 色彩空间转换和缩放
}

// H264Encoder - H.264视频编码器封装类
// 职责：将QImage(RGB格式)编码为H.264码流
// 依赖：FFmpeg/libavcodec/libswscale
// 特点：
//   1. 支持动态分辨率 - 输入图像尺寸可以变化，自动进行缩放处理
//   2. 自动色彩转换 - 将RGB转换为YUV420P(FFmpeg标准格式)
//   3. 码率控制 - 可配置目标码率，默认400Kbps适合实时通话
class H264Encoder : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数/析构函数
    explicit H264Encoder(QObject *parent = nullptr);
    ~H264Encoder();

    // initEncoder - 初始化编码器
    // 参数:
    //   width: 编码目标宽度(如320、960)
    //   height: 编码目标高度(如240、720)
    //   fps: 目标帧率(如15、30)
    //   bitRate: 目标码率(默认400Kbps)
    //         - 摄像头模式: 400Kbps (低码率，省带宽)
    //         - 屏幕共享模式: 2Mbps (高码率，保清晰)
    // 返回: true=初始化成功，false=初始化失败
    bool initEncoder(int width, int height, int fps, int bitRate = 400000);

    // encode - 核心编码函数
    // 参数: image - 输入QImage(RGB格式，任意尺寸)
    // 返回: QByteArray - H.264编码后的数据包
    //       空QByteArray表示编码器还在缓冲(等待关键帧)
    // 特点:
    //   1. 支持任意尺寸输入，自动缩放到目标分辨率
    //   2. 首次调用会输出SPS/PPS参数集(关键帧)，后续调用输出P帧
    //   3. 需要周期性输入关键帧以防止错误累积
    QByteArray encode(const QImage &image);

    // destroy - 释放编码器资源
    // 功能: 销毁AVCodecContext、AVFrame、SwsContext
    void destroy();



private:
    // ensureSwsContext - 确保SwsContext与输入图像尺寸匹配
    // 参数: srcWidth/srcHeight - 输入图像尺寸
    // 返回: true=成功，false=失败
    // 功能: 如果SwsContext不存在或不匹配，则重新创建
    bool ensureSwsContext(int srcWidth, int srcHeight);

    // reinitSwsContext - 重新初始化SwsContext
    // 参数:
    //   srcWidth/srcHeight - 源图像尺寸(RGB)
    //   dstWidth/dstHeight - 目标图像尺寸(YUV420P)
    // 功能: 释放旧的SwsContext，创建新的用于缩放和色彩转换
    void reinitSwsContext(int srcWidth, int srcHeight, int dstWidth, int dstHeight);

private:
    // FFmpeg编码器相关成员
    AVCodecContext *m_pCodecCtx; // 编码器上下文
    // 存储编码器配置(分辨率、码率、帧率等)和编码状态
    // 生命周期: initEncoder创建，destroy释放

    AVFrame *m_pFrameYUV;        // YUV编码帧缓冲区
    // 存放转换后的YUV420P数据，供编码器使用
    // 生命周期: initEncoder创建，destroy释放

    SwsContext *m_pSwsCtx;       // 图像缩放和色彩转换器
    // 功能: RGB -> YUV420P + 任意尺寸 -> 目标尺寸
    // 生命周期: 按需创建/销毁

    // 编码器配置参数
    int m_width;                 // 编码目标宽度
    int m_height;                // 编码目标高度
    int m_fps;                   // 目标帧率
    int m_bitRate;              // 目标码率(bps)

    int m_frameIndex;            // 帧序号(用于计算PTS时间戳)
    // 每编码一帧+1，用于设置AVFrame.pts

    // SwsContext缓存
    // 记录当前SwsContext的源尺寸，避免频繁重建
    int m_currentSrcWidth;       // 当前SwsContext的源宽度
    int m_currentSrcHeight;      // 当前SwsContext的源高度
};

#endif // H264ENCODER_H
