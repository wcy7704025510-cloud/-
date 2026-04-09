#ifndef CMEDIAMANAGER_H
#define CMEDIAMANAGER_H

#include <QObject>              // Qt对象基类
#include <QImage>             // Qt图像类，用于视频帧处理
#include <QByteArray>         // Qt字节数组，用于音频数据封装
#include <QSharedPointer>     // Qt智能指针，自动管理内存
#include <QMap>              // Qt关联容器，存储用户ID到解码器的映射

class Audio_Read;             // 音频采集类
class VideoRead;               // 摄像头视频采集类
class ScreenRead;              // 屏幕采集类
class CRoomManager;            // 房间管理器
class RoomDialog;              // 房间对话框
class MediaSendWorker;        // 媒体发送工作线程

// 自定义H.264编解码器前向声明
class H264Encoder;            // H.264视频编码器
class H264Decoder;            // H.264视频解码器

// CMediaManager - 媒体业务管理器
// 职责：协调音视频采集、编码、发送、接收、解码、播放的全流程
// 核心功能：
//   1. 音视频设备管理(摄像头、麦克风、扬声器)
//   2. H.264视频编解码
//   3. 音视频数据路由(发送/接收)
// 数据流：
//   采集(摄像头) -> H.264编码 -> 网络发送 -> 服务器转发 -> 对方接收 -> H.264解码 -> 画面显示
class CMediaManager : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数/析构函数
    explicit CMediaManager(QObject *parent = nullptr);
    ~CMediaManager();

    // ============= 初始化接口 =============
    // 初始化音视频设备
    // 功能: 打开摄像头、初始化音频设备、创建编码器
    // 调用时机: 登录成功后由Ckernel调用
    void initDevices();

    // 依赖注入
    void setRoomManager(CRoomManager* rm) { m_pRoomManager = rm; }    // 房间管理器指针
    void setUserId(int id) { m_id = id; }                            // 设置当前用户ID
    void setRoomDialog(RoomDialog* roomDialog) { m_pRoomDialog = roomDialog; } // 房间对话框指针

signals:
    // 视频数据发送信号(内部使用)
    // 参数: buf-H.264编码后数据, nPackSize-数据包大小
    void SIG_sendVideo(char* buf, int nPackSize);

    // 音频数据发送信号
    // 参数: buf-Opus编码后音频数据, nLen-数据长度
    // 接收者: Ckernel::slot_SendAudioData()
    void SIG_SendAudioData(char* buf, int nLen);

    // 视频数据发送信号
    // 参数: buf-H.264编码后视频数据, nLen-数据长度
    // 接收者: Ckernel::slot_SendVideoData()
    void SIG_SendVideoData(char* buf, int nLen);

public slots:
    // ============= 网络数据接收槽 =============
    // 接收并处理他人音频数据
    // 参数: sock-socket标识, buf-音频数据包, nLen-数据长度
    // 功能: 根据用户ID路由到对应Audio_Write播放
    void slot_audioFrameRq(uint sock, char* buf, int nLen);

    // 接收并处理他人视频数据
    // 参数: sock-socket标识, buf-视频数据包, nLen-数据长度
    // 功能: 解码H.264，转换为QImage发送显示信号
    void slot_videoFrameRq(uint sock, char* buf, int nLen);

    // ============= 本地采集数据槽 =============
    // 采集到本地音频数据
    // 参数: ba-PCM原始音频数据
    // 功能: Opus编码后发送
    void slot_audioFrame(QByteArray ba);

    // 采集到摄像头视频帧
    // 参数: img-RGB格式图像
    // 功能: H.264编码后发送
    void slot_sendVedioFrame(QImage img);

    // 采集到屏幕画面
    // 参数: img-屏幕截图图像
    // 功能: H.264高质量编码后发送(桌面共享模式)
    void slot_sendScreenFrame(QImage img);

    // ============= 媒体控制槽 =============
    // 暂停/恢复音频采集播放
    void slot_AudioPause();     // 暂停本地音频采集
    void slot_AudioStart();    // 恢复本地音频采集

    // 暂停/恢复视频采集发送
    void slot_VideoPause();     // 暂停摄像头采集
    void slot_VideoStart();    // 恢复摄像头采集

    // 暂停/恢复屏幕共享
    void slot_ScreenPause();   // 暂停屏幕采集
    void slot_ScreenStart();   // 恢复屏幕采集

    // ============= 视频显示槽 =============
    // 刷新指定用户视频画面
    // 参数: id-用户ID, img-解码后QImage
    // 功能: 查找对应UserShow控件并更新显示
    void slot_refreshVideo(int id, QImage& img);

    // 发送视频数据(内部处理)
    void slot_sendVideo(char* buf, int nPackSize);

    // ============= 资源清理槽 =============
    // 清理所有媒体设备和资源
    // 调用时机: 退出房间或程序退出时
    void slot_clearDevices();

private:
    // ============= 媒体采集设备 =============
    Audio_Read* m_pAudio_Read;      // 麦克风采集对象，负责音频输入
    VideoRead* m_pVedioRead;       // 摄像头采集对象，负责视频输入
    ScreenRead* m_pSreenRead;       // 屏幕采集对象，负责桌面捕获
    QSharedPointer<MediaSendWorker> m_pMediaSendWorker; // 媒体发送工作线程

    // ============= 业务管理层指针 =============
    CRoomManager* m_pRoomManager;  // 房间管理器，用于获取成员信息
    RoomDialog* m_pRoomDialog;     // 房间对话框，用于更新UI

    // ============= 用户标识 =============
    int m_id;                       // 当前用户ID

    // ============= H.264编解码器 =============
    // 视频编码器: 处理本地摄像头数据
    // 参数: 320×240分辨率, 400Kbps码率, 15fps帧率
    H264Encoder* m_pH264Encoder;

    // 屏幕共享编码器: 处理屏幕采集数据
    // 参数: 960×720分辨率, 2Mbps码率(高质量), 更高帧率
    H264Encoder* m_pScreenEncoder;

    // 解码器池: 为每个远程用户维护独立解码器
    // Key: 用户ID (uint32_t)
    // Value: 该用户的H264Decoder实例
    // 原因: 多路视频流需要独立解码上下文，避免状态干扰
    QMap<int, H264Decoder*> m_mapDecoders;
};

#endif // CMEDIAMANAGER_H
