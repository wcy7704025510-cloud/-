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
    // FFmpeg使用libx264作为H.264软件编码器
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "FFmpeg Error: 找不到 H264 编码器！";
        return false;
    }

    // 2. 申请编码器上下文内存
    // AVCodecContext: 编码器核心结构体，存储所有配置信息
    m_pCodecCtx = avcodec_alloc_context3(codec);
    if (!m_pCodecCtx) return false;

    // 3. 配置编码器参数
    // 分辨率设置
    m_pCodecCtx->width = width;        // 编码输出宽度
    m_pCodecCtx->height = height;       // 编码输出高度

    // 时间基准设置
    // time_base: 帧时间戳单位，分数形式 {1, fps}
    // 如 fps=15, time_base={1, 15} 表示每1/15秒为一单位
    m_pCodecCtx->time_base = {1, fps};
    m_pCodecCtx->framerate = {fps, 1}; // 帧率 {fps, 1}

    // 码率设置(比特率)
    // bps = bits per second，每秒传输的比特数
    // 400Kbps = 400,000 bps
    m_pCodecCtx->bit_rate = bitRate;

    // GOP(Group of Pictures)设置
    // gop_size = fps 表示每秒1个I帧(关键帧)
    // I帧间隔越小，抗丢包能力越强，但码率略高
    m_pCodecCtx->gop_size = fps;

    // B帧设置
    // max_b_frames = 0 完全关闭B帧
    // B帧需要向后预测，会引入额外延迟，实时通话必须关闭
    m_pCodecCtx->max_b_frames = 0;

    // 像素格式设置
    // AV_PIX_FMT_YUV420P: H.264标准格式
    // YUV420P采用4:2:0采样，1像素占1.5字节，比RGB节省50%空间
    m_pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    // 4. 设置libx264私有参数(通过字典传入)
    // preset: 编码速度预设
    // ultrafast: 最快编码速度，CPU开销最小，牺牲压缩率
    // 其他选项: ultrafast -> superfast -> veryfast -> faster -> fast -> medium -> slow
    av_opt_set(m_pCodecCtx->priv_data, "preset", "ultrafast", 0);

    // tune: 调优参数，针对特定场景优化
    // zerolatency: 零延迟模式，编码器不缓存数据
    // 来一帧压一帧，立刻输出，适合实时通话
    av_opt_set(m_pCodecCtx->priv_data, "tune", "zerolatency", 0);

    // 5. 打开编码器
    // 验证所有参数，准备编码器内部资源
    if (avcodec_open2(m_pCodecCtx, codec, nullptr) < 0) {
        qDebug() << "FFmpeg Error: 无法打开 H264 编码器！";
        return false;
    }

    // 6. 申请YUV帧缓冲区
    // AVFrame: 存储一帧图像的数据结构
    m_pFrameYUV = av_frame_alloc();
    m_pFrameYUV->format = m_pCodecCtx->pix_fmt;       // 像素格式
    m_pFrameYUV->width = m_pCodecCtx->width;          // 宽度
    m_pFrameYUV->height = m_pCodecCtx->height;        // 高度

    // 分配图像内存
    // linesize: 每行字节数(可能对齐到32/64字节)
    // align: 32字节对齐
    av_image_alloc(m_pFrameYUV->data, m_pFrameYUV->linesize, m_width, m_height, AV_PIX_FMT_YUV420P, 32);

    qDebug() << QString("H264Encoder: 初始化完成 %1x%2 @ %3 fps, %4 bps")
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
