#include "H264Decoder.h"           // H.264解码器头文件
#include <QDebug>                 // Qt调试输出

// H264Decoder构造函数
H264Decoder::H264Decoder(QObject *parent) : QObject(parent),
    m_pCodecCtx(nullptr), m_pFrame(nullptr), m_pFrameRGB(nullptr),
    m_pSwsCtx(nullptr), m_buffer(nullptr), m_currentW(0), m_currentH(0)
{
}

// H264Decoder析构函数
H264Decoder::~H264Decoder()
{
    destroy();
}

// initDecoder - 初始化H.264解码器
// 返回: true=成功, false=失败
bool H264Decoder::initDecoder()
{
    // 1. 查找H.264解码器
    // FFmpeg内置H.264软件解码器(不依赖外部库)
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "FFmpeg Error: 找不到 H264 解码器！";
        return false;
    }

    // 2. 申请解码器上下文内存
    m_pCodecCtx = avcodec_alloc_context3(codec);
    if (!m_pCodecCtx) return false;

    // 3. 开启低延迟解码模式
    // AV_CODEC_FLAG_LOW_DELAY: 解码器不等完整GOP(关键帧间隔)
    // 直接解码，减少延迟，非常适合实时音视频会议
    m_pCodecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;

    // 4. 打开解码器
    if (avcodec_open2(m_pCodecCtx, codec, nullptr) < 0) {
        qDebug() << "FFmpeg Error: 无法打开 H264 解码器！";
        return false;
    }

    // 5. 申请帧内存
    // m_pFrame: 存储解码后的YUV数据
    // m_pFrameRGB: 存储转换后的RGB数据
    m_pFrame = av_frame_alloc();
    m_pFrameRGB = av_frame_alloc();

    return true;
}

// decode - 核心解码函数
// 参数: h264Data-输入H.264数据, outImage-输出QImage
// 返回: true=成功, false=失败
bool H264Decoder::decode(const QByteArray &h264Data, QImage &outImage)
{
    // 防御性检查
    if (h264Data.isEmpty() || !m_pCodecCtx) return false;

    // 初始化数据包
    AVPacket pkt;
    av_init_packet(&pkt);
    // 将QByteArray数据指针赋值给AVPacket
    pkt.data = (uint8_t*)h264Data.data();
    pkt.size = h264Data.size();

    bool gotPicture = false;  // 标记是否成功解码

    // 1. 将H.264数据包发送给解码器
    int ret = avcodec_send_packet(m_pCodecCtx, &pkt);
    if (ret == 0) {
        // 2. 循环取出解码后的YUV帧
        // 重要: 必须使用while循环，解码器可能一次输出多帧
        while (avcodec_receive_frame(m_pCodecCtx, m_pFrame) == 0) {
            // 获取解码图像尺寸
            int width = m_pFrame->width;
            int height = m_pFrame->height;

            // 检查分辨率是否变化
            if (m_pSwsCtx && (m_currentW != width || m_currentH != height)) {
                // 分辨率变化，释放旧的SwsContext和缓冲区
                sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr;
                av_freep(&m_buffer);
            }

            // 如果SwsContext不存在，则创建
            if (!m_pSwsCtx) {
                m_currentW = width;
                m_currentH = height;

                // 创建SwsContext: YUV420P -> RGB24
                // 参数: 源格式(来自解码器), 目标格式(RGB24)
                m_pSwsCtx = sws_getContext(width, height, m_pCodecCtx->pix_fmt,
                                           width, height, AV_PIX_FMT_RGB24,
                                           SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

                // 计算RGB缓冲区大小并分配内存
                int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
                m_buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

                // 填充AVFrame的RGB数据数组
                av_image_fill_arrays(m_pFrameRGB->data, m_pFrameRGB->linesize,
                                     m_buffer, AV_PIX_FMT_RGB24, width, height, 1);
            }

            // 执行色彩转换: YUV420P -> RGB24
            sws_scale(m_pSwsCtx, (const uint8_t* const*)m_pFrame->data, m_pFrame->linesize,
                      0, height, m_pFrameRGB->data, m_pFrameRGB->linesize);

            // 构建QImage
            // 注意: 直接使用m_pFrameRGB->data[0]可能导致内存问题
            // 因此使用copy()创建独立副本
            outImage = QImage(m_pFrameRGB->data[0], width, height,
                             m_pFrameRGB->linesize[0], QImage::Format_RGB888).copy();
            gotPicture = true;
        }
    }
    return gotPicture;
}

// destroy - 释放解码器资源
void H264Decoder::destroy()
{
    // 释放RGB缓冲区
    if (m_buffer) { av_free(m_buffer); m_buffer = nullptr; }

    // 释放RGB帧
    if (m_pFrameRGB) { av_frame_free(&m_pFrameRGB); }

    // 释放YUV帧
    if (m_pFrame) { av_frame_free(&m_pFrame); }

    // 释放SwsContext
    if (m_pSwsCtx) { sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr; }

    // 释放解码器上下文
    if (m_pCodecCtx) {
        avcodec_free_context(&m_pCodecCtx);
        m_pCodecCtx = nullptr;
    }
}
