#include "H264Decoder.h"
#include <QDebug>

H264Decoder::H264Decoder(QObject *parent) : QObject(parent),
    m_pCodecCtx(nullptr), m_pFrame(nullptr), m_pFrameRGB(nullptr),
    m_pSwsCtx(nullptr), m_buffer(nullptr), m_currentW(0), m_currentH(0)
{
}

H264Decoder::~H264Decoder()
{
    destroy();
}

bool H264Decoder::initDecoder()
{
    // 1. 查找 H.264 解码器
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "FFmpeg Error: 找不到 H264 解码器！";
        return false;
    }

    // 2. 申请解码器上下文
    m_pCodecCtx = avcodec_alloc_context3(codec);
    if (!m_pCodecCtx) return false;

    // 🌟 开启低延迟解码模式 (极其适合音视频会议，不等待完整 GOP 直接强解)
    m_pCodecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;

    // 3. 打开解码器
    if (avcodec_open2(m_pCodecCtx, codec, nullptr) < 0) {
        qDebug() << "FFmpeg Error: 无法打开 H264 解码器！";
        return false;
    }

    // 4. 申请帧内存
    m_pFrame = av_frame_alloc();
    m_pFrameRGB = av_frame_alloc();

    return true;
}
bool H264Decoder::decode(const QByteArray &h264Data, QImage &outImage)
{
    if (h264Data.isEmpty() || !m_pCodecCtx) return false;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = (uint8_t*)h264Data.data();
    pkt.size = h264Data.size();

    bool gotPicture = false;

    // 1. 将压缩包送入解码器
    int ret = avcodec_send_packet(m_pCodecCtx, &pkt);
    if (ret == 0) {
        // 2. 🌟 修复：必须使用 while 循环，把解码器里解好的帧全部掏空！
        while (avcodec_receive_frame(m_pCodecCtx, m_pFrame) == 0) {
            int width = m_pFrame->width;
            int height = m_pFrame->height;

            if (m_pSwsCtx && (m_currentW != width || m_currentH != height)) {
                sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr;
                av_freep(&m_buffer);
            }

            if (!m_pSwsCtx) {
                m_currentW = width;
                m_currentH = height;
                m_pSwsCtx = sws_getContext(width, height, m_pCodecCtx->pix_fmt,
                                           width, height, AV_PIX_FMT_RGB24,
                                           SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

                int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
                m_buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
                av_image_fill_arrays(m_pFrameRGB->data, m_pFrameRGB->linesize, m_buffer, AV_PIX_FMT_RGB24, width, height, 1);
            }

            // 执行像素格式转换
            sws_scale(m_pSwsCtx, (const uint8_t* const*)m_pFrame->data, m_pFrame->linesize,
                      0, height, m_pFrameRGB->data, m_pFrameRGB->linesize);

            outImage = QImage(m_pFrameRGB->data[0], width, height, m_pFrameRGB->linesize[0], QImage::Format_RGB888).copy();
            gotPicture = true;
        }
    }
    return gotPicture;
}
void H264Decoder::destroy()
{
    if (m_buffer) { av_free(m_buffer); m_buffer = nullptr; }
    if (m_pFrameRGB) { av_frame_free(&m_pFrameRGB); }
    if (m_pFrame) { av_frame_free(&m_pFrame); }
    if (m_pSwsCtx) { sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr; }
    if (m_pCodecCtx) { avcodec_free_context(&m_pCodecCtx); m_pCodecCtx = nullptr; }
}
