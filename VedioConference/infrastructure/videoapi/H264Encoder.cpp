#include "H264Encoder.h"
#include <QDebug>

H264Encoder::H264Encoder(QObject *parent) : QObject(parent),
    m_pCodecCtx(nullptr), m_pFrameYUV(nullptr), m_pSwsCtx(nullptr),
    m_width(0), m_height(0), m_fps(15), m_frameIndex(0)
{
}

H264Encoder::~H264Encoder()
{
    destroy();
}

bool H264Encoder::initEncoder(int width, int height, int fps)
{
    m_width = width;
    m_height = height;
    m_fps = fps;

    // 1. 寻找 H.264 编码器 (libx264)
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "FFmpeg Error: 找不到 H264 编码器！";
        return false;
    }

    // 2. 申请编码器上下文内存
    m_pCodecCtx = avcodec_alloc_context3(codec);
    if (!m_pCodecCtx) return false;

    // 3. 配置硬核编码参数
    m_pCodecCtx->width = width;
    m_pCodecCtx->height = height;
    m_pCodecCtx->time_base = {1, fps};
    m_pCodecCtx->framerate = {fps, 1};
    // 目标码率 400Kbps，足够保证 320x240 极其清晰流畅
    m_pCodecCtx->bit_rate = 400000;
    // GOP大小等于帧率（每秒1个I帧，如果网络丢包，最多卡1秒就会瞬间恢复）
    m_pCodecCtx->gop_size = fps;
    // ⚠️ 绝密参数：彻底关闭 B 帧。B 帧需要向后预测，是造成直播画面延迟的元凶！
    m_pCodecCtx->max_b_frames = 0;
    // 设置色彩空间：H.264 的国际标准是 YUV420P
    m_pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    // 4. ⚠️ 实时音视频领域的灵魂参数 (通过字典传入底层)
    // preset: ultrafast -> 榨干 CPU，以最快的速度编码
    av_opt_set(m_pCodecCtx->priv_data, "preset", "ultrafast", 0);
    // tune: zerolatency -> 零延迟模式，编码器不再缓存数据，来一帧压一帧，压完立刻吐出！
    av_opt_set(m_pCodecCtx->priv_data, "tune", "zerolatency", 0);

    // 5. 打开编码器
    if (avcodec_open2(m_pCodecCtx, codec, nullptr) < 0) {
        qDebug() << "FFmpeg Error: 无法打开 H264 编码器！";
        return false;
    }

    // 6. 申请 YUV420P 帧和图像内存缓存
    m_pFrameYUV = av_frame_alloc();
    m_pFrameYUV->format = m_pCodecCtx->pix_fmt;
    m_pFrameYUV->width = m_pCodecCtx->width;
    m_pFrameYUV->height = m_pCodecCtx->height;
    av_image_alloc(m_pFrameYUV->data, m_pFrameYUV->linesize, m_width, m_height, AV_PIX_FMT_YUV420P, 32);

    // 7. 初始化 SwsContext：这是 FFmpeg 自带的高性能像素格式转换器
    m_pSwsCtx = sws_getContext(m_width, m_height, AV_PIX_FMT_RGB24,
                               m_width, m_height, AV_PIX_FMT_YUV420P,
                               SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    m_frameIndex = 0;
    return true;
}

QByteArray H264Encoder::encode(const QImage &image)
{
    QByteArray encodedData;
    if (!m_pCodecCtx || !m_pFrameYUV || !m_pSwsCtx) return encodedData;

    // 1. 像素转换：把 Qt 的 QImage(RGB24) 瞬间转换为 YUV420P
    const uint8_t *inData[1] = { image.bits() };
    int inLinesize[1] = { image.bytesPerLine() };
    sws_scale(m_pSwsCtx, inData, inLinesize, 0, m_height, m_pFrameYUV->data, m_pFrameYUV->linesize);

    // 2. 赋予时间戳 (PTS)
    m_pFrameYUV->pts = m_frameIndex++;

    // 3. 准备数据包接收压缩后的成果
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // 4. 执行编码：把 YUV 喂进编码器
    int ret = avcodec_send_frame(m_pCodecCtx, m_pFrameYUV);
    if (ret >= 0) {
        // 循环收取编码器吐出来的 H.264 裸流包
        while (avcodec_receive_packet(m_pCodecCtx, &pkt) == 0) {
            // 将压缩后的数据收集进 Qt 的字节数组里
            encodedData.append((char*)pkt.data, pkt.size);
            av_packet_unref(&pkt); // 用完释放
        }
    }

    // 你可以在这里打印 encodedData.size()，感受一下它把一帧画面压缩了多少倍！
    return encodedData;
}

void H264Encoder::destroy()
{
    if (m_pSwsCtx) { sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr; }
    if (m_pFrameYUV) {
        av_freep(&m_pFrameYUV->data[0]);
        av_frame_free(&m_pFrameYUV);
        m_pFrameYUV = nullptr;
    }
    if (m_pCodecCtx) {
        avcodec_close(m_pCodecCtx);
        avcodec_free_context(&m_pCodecCtx);
        m_pCodecCtx = nullptr;
    }
}
