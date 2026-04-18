#include "H264Encoder.h"           // H.264编码器头文件
#include <QDebug>                 // Qt调试输出

// H264Encoder构造函数
// 初始化所有成员变量为初始状态
H264Encoder::H264Encoder(QObject *parent) : QObject(parent),
    m_pCodecCtx(nullptr), m_pFrameYUV(nullptr), m_pSwsCtx(nullptr),
    m_width(0), m_height(0), m_fps(15), m_bitRate(400000), m_frameIndex(0),
    m_currentSrcWidth(0), m_currentSrcHeight(0)
{
}

// H264Encoder析构函数
// 自动调用destroy释放资源
H264Encoder::~H264Encoder()
{
    destroy();
}




// initEncoder - 初始化H.264编码器
// 参数: width-目标宽度, height-目标高度, fps-帧率, bitRate-码率
// 返回: true=成功, false=失败
bool H264Encoder::initEncoder(int width, int height, int fps, int bitRate)
{
    // 保存配置参数
    m_width = width;
    m_height = height;
    m_fps = fps;
    m_bitRate = bitRate;

    // 1. 查找H.264编码器
    // AV_CODEC_ID_H264: H.264标准编码器ID
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "FFmpeg Error: 找不到 H264 编码器！";
        return false;
    }

    // 2. 申请编码器上下文内存
    m_pCodecCtx = avcodec_alloc_context3(codec);
    if (!m_pCodecCtx) return false;

    // 3. 配置基础参数
    m_pCodecCtx->width = width;        // 编码输出宽度
    m_pCodecCtx->height = height;      // 编码输出高度
    m_pCodecCtx->time_base = {1, fps}; // 时间基准
    m_pCodecCtx->framerate = {fps, 1}; // 帧率

    // 智能区分摄像头与屏幕流
    // 通过传入的分辨率宽度自动判断当前是摄像头还是屏幕共享
    bool isScreen = (width >= 800);

    if (isScreen) {
            // 屏幕共享专属编码
            // 把 crf 设置为24（数字越小，画质越好，能消灭“小部分马赛克”）
            av_opt_set(m_pCodecCtx->priv_data, "crf", "24", 0);

            // 限制峰值码率。如果bitRate 传入 2M，这里最多允许飙升到 3M，绝不能再高，否则网络必堵车！
            m_pCodecCtx->rc_max_rate = bitRate ;
            m_pCodecCtx->rc_buffer_size = bitRate;

            av_opt_set(m_pCodecCtx->priv_data, "preset", "ultrafast", 0);

            // 2. 开启零延迟模型，放弃画质，死保 RTC 的极低延迟
            av_opt_set(m_pCodecCtx->priv_data, "tune", "zerolatency", 0);

            qDebug() << "H264Encoder: 屏幕共享模式 (CRF 24 + ZeroLatency)";
        }else {
        // 摄像头专属编码
        // 1. 严格使用 ABR (平均码率) 压制，节省基础网络带宽
        m_pCodecCtx->bit_rate = bitRate;

        // 2. 开启零延迟模型，放弃画质，死保 RTC 的极低延迟
        av_opt_set(m_pCodecCtx->priv_data, "preset", "ultrafast", 0);
        av_opt_set(m_pCodecCtx->priv_data, "tune", "zerolatency", 0);

        qDebug() << "H264Encoder: 检测到小分辨率，自动切换为【摄像头 ABR 极低延迟模式】";
    }

    // 4. 设置通用标准参数 (GOP 和 像素格式)
    m_pCodecCtx->gop_size = fps;
    m_pCodecCtx->max_b_frames = 0;       // 实时流严禁使用 B 帧
    m_pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;


    // 5. 打开编码器
    if (avcodec_open2(m_pCodecCtx, codec, nullptr) < 0) {
        qDebug() << "FFmpeg Error: 无法打开 H264 编码器！";
        return false;
    }

    // 6. 申请YUV帧缓冲区
    m_pFrameYUV = av_frame_alloc();
    m_pFrameYUV->format = m_pCodecCtx->pix_fmt;       // 像素格式
    m_pFrameYUV->width = m_pCodecCtx->width;          // 宽度
    m_pFrameYUV->height = m_pCodecCtx->height;        // 高度

    // 分配图像内存 (32字节对齐)
    av_image_alloc(m_pFrameYUV->data, m_pFrameYUV->linesize, m_width, m_height, AV_PIX_FMT_YUV420P, 32);

    qDebug() << QString("H264Encoder: 初始化完成 %1x%2 @ %3 fps, 基础码率 %4 bps")
                .arg(width).arg(height).arg(fps).arg(bitRate);

    m_frameIndex = 0;  // 重置帧序号
    return true;
}

// reinitSwsContext - 重新初始化图像转换上下文
// 功能: 释放旧的SwsContext，创建新的支持缩放+色彩转换
void H264Encoder::reinitSwsContext(int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
    // 释放旧的SwsContext
    if (m_pSwsCtx) {
        sws_freeContext(m_pSwsCtx);
        m_pSwsCtx = nullptr;
    }

    // 创建新的SwsContext
    // sws_getContext参数:
    //   srcWidth/srcHeight: 源图像尺寸
    //   AV_PIX_FMT_RGB24: 源格式(RGB24)
    //   dstWidth/dstHeight: 目标图像尺寸
    //   AV_PIX_FMT_YUV420P: 目标格式(YUV420P)
    //   SWS_FAST_BILINEAR: 缩放算法(快速双线性插值)
    m_pSwsCtx = sws_getContext(srcWidth, srcHeight, AV_PIX_FMT_RGB24,
                               dstWidth, dstHeight, AV_PIX_FMT_YUV420P,
                               SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    if (!m_pSwsCtx) {
        qDebug() << "FFmpeg Error: 无法创建 SwsContext！";
        return;
    }

    // 缓存当前源尺寸，避免重复创建
    m_currentSrcWidth = srcWidth;
    m_currentSrcHeight = srcHeight;

    qDebug() << QString("H264Encoder: SwsContext重新初始化 %1x%2 -> %3x%4")
                .arg(srcWidth).arg(srcHeight).arg(dstWidth).arg(dstHeight);
}

// ensureSwsContext - 确保SwsContext与输入图像匹配
bool H264Encoder::ensureSwsContext(int srcWidth, int srcHeight)
{
    // 检查是否需要重新创建
    // 条件: SwsContext为空 或 源尺寸发生变化
    if (!m_pSwsCtx ||
        m_currentSrcWidth != srcWidth ||
        m_currentSrcHeight != srcHeight) {
        reinitSwsContext(srcWidth, srcHeight, m_width, m_height);
        return m_pSwsCtx != nullptr;
    }
    return true;
}

// encode - 核心编码函数
// 参数: image - 输入QImage(RGB格式)
// 返回: QByteArray - H.264编码数据，空表示还在缓冲
QByteArray H264Encoder::encode(const QImage &image)
{
    QByteArray encodedData;
    if (!m_pCodecCtx || !m_pFrameYUV) return encodedData;

    // 获取输入图像尺寸
    int srcWidth = image.width();
    int srcHeight = image.height();

    // 1. 确保SwsContext与输入尺寸匹配
    if (!ensureSwsContext(srcWidth, srcHeight)) {
        qDebug() << "H264Encoder: SwsContext初始化失败";
        return encodedData;
    }

    // 2. 像素格式转换 + 缩放
    // RGB24 -> YUV420P + 缩放到目标分辨率
    // inData: 源图像数据指针数组
    // inLinesize: 源图像每行字节数
    const uint8_t *inData[1] = { image.bits() };
    int inLinesize[1] = { image.bytesPerLine() };

    // sws_scale执行实际转换
    // 参数: 上下文, 源数据, 源行大小, 源高度, 目标数据, 目标行大小
    sws_scale(m_pSwsCtx, inData, inLinesize, 0, srcHeight,
              m_pFrameYUV->data, m_pFrameYUV->linesize);

    // 3. 设置时间戳(PTS)
    // pts: presentation time stamp，显示时间戳
    // 编码器根据PTS计算帧间关系
    m_pFrameYUV->pts = m_frameIndex++;

    // 4. 初始化数据包
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // 5. 执行H.264编码(发送-接收模式)
    // avcodec_send_frame: 将YUV帧发送给编码器
    int ret = avcodec_send_frame(m_pCodecCtx, m_pFrameYUV);
    if (ret >= 0) {
        // avcodec_receive_packet: 从编码器获取H.264数据包
        // 循环收取，直到编码器输出完毕
        while (avcodec_receive_packet(m_pCodecCtx, &pkt) == 0) {
            // 追加到输出数组
            encodedData.append((char*)pkt.data, pkt.size);
            // 释放数据包内部引用(避免内存泄漏)
            av_packet_unref(&pkt);
        }
    }

    return encodedData;
}

// destroy - 释放编码器资源
void H264Encoder::destroy()
{
    // 释放SwsContext
    if (m_pSwsCtx) { sws_freeContext(m_pSwsCtx); m_pSwsCtx = nullptr; }

    // 释放YUV帧
    if (m_pFrameYUV) {
        // 先释放图像数据内存
        av_freep(&m_pFrameYUV->data[0]);
        // 再释放帧结构体
        av_frame_free(&m_pFrameYUV);
        m_pFrameYUV = nullptr;
    }

    // 释放编码器上下文
    if (m_pCodecCtx) {
        avcodec_close(m_pCodecCtx);      // 关闭编码器
        avcodec_free_context(&m_pCodecCtx); // 释放上下文内存
        m_pCodecCtx = nullptr;
    }
}
