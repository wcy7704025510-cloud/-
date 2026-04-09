#ifndef CROOMMANAGER_H
#define CROOMMANAGER_H

// Qt核心框架头文件
#include <QObject>              // Qt对象基类，支持信号槽机制
#include <QString>             // Qt字符串类
#include <map>                 // C++标准库map容器，存储键值对

// 前向声明 - 避免循环引用
class RoomDialog;              // 房间对话框(选择房间/成员列表)
class VedioConferenceDialog;   // 视频会议主界面
class Audio_Write;             // 音频写入/播放类

// CRoomManager - 房间业务管理器
// 职责：管理视频会议房间的创建、加入、离开，以及房间成员信息同步
// 协作关系：
//   VedioConferenceDialog(主界面) --SIG_createRoom/SIG_joinRoom--> Manager
//   RoomDialog(房间弹窗) --SIG_close--> Manager --SIG_RoomQuitted--> CMediaManager
//   服务器响应 --Ckernel分发--> Manager --更新UI--> VedioConferenceDialog/RoomDialog
class CRoomManager : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数
    // parent: 父对象指针
    explicit CRoomManager(QObject *parent = nullptr);

    // ============= 依赖注入接口 =============
    // 注入房间对话框指针，用于显示房间列表和成员
    void setRoomDialog(RoomDialog* roomDlg) { m_pRoomDialog = roomDlg; }
    // 注入视频会议主界面指针，用于更新房间状态
    void setVedioDialog(VedioConferenceDialog* vedioDlg) { m_pVedio = vedioDlg; }
    // 设置当前登录用户信息
    // id: 用户唯一ID (服务器分配)
    // name: 用户昵称
    void setUserId(int id, QString name) { m_id = id; m_name = name; }

    // ============= 状态查询接口 =============
    // 获取当前房间ID
    // 返回: 0表示未在房间中，非0为有效房间ID
    int getRoomId() const { return m_roomId; }

    // 获取用户ID到音频播放对象的映射
    // 返回: map容器，key=用户ID，value=该用户的Audio_Write播放对象
    // 用途: 根据收到的音频数据路由到对应用户的播放设备
    std::map<int, Audio_Write*>& getAudioWriteMap() { return m_mapIdToAudioWrite; }

signals:
    // 退出房间信号
    // 功能: 通知其他Manager(如CMediaManager)清理房间相关资源
    // 接收者: CMediaManager::slot_clearDevices()
    void SIG_RoomQuitted();

    // 网络数据发送请求
    // 参数: buf-待发送数据缓冲区, nLen-数据长度
    // 接收者: Ckernel::slot_SendClientData()
    void SIG_SendData(char* buf, int nLen);

public slots:
    // ============= UI触发槽函数 =============
    // 创建房间请求
    // 触发: VedioConferenceDialog点击"创建房间"按钮
    // 功能: 封装创建房间请求包，发送到服务器
    void slot_createRoom();

    // 加入房间请求
    // 触发: VedioConferenceDialog点击"加入房间"按钮
    // 功能: 从UI获取房间号，封装加入请求包
    void slot_joinRoom();

    // 退出房间请求
    // 触发: RoomDialog点击"退出"按钮或收到服务器踢人通知
    // 功能: 清理房间状态，停止音视频采集，通知服务器
    void slot_QuitRoom();

    // ============= 网络响应处理槽 =============
    // 创建房间响应
    // 解析服务器返回的房间号，更新UI显示
    void slot_dealCreateRoomRs(uint sock, char* buf, int nLen);

    // 加入房间响应
    // 成功: 显示房间界面，开始接收音视频数据
    // 失败: 弹窗提示错误原因
    void slot_dealJoinRoomRs(uint sock, char* buf, int nLen);

    // 用户信息通知(有人加入房间)
    // 参数: buf包含用户ID、昵称、头像等信息
    // 功能: 更新房间成员列表，创建该用户的音频播放对象
    void slot_dealUserInfoRq(uint sock, char* buf, int nLen);

    // 退出房间通知(有人离开)
    // 功能: 从成员列表移除，销毁对应的音频播放对象
    void slot_dealQuitRoomRq(uint sock, char* buf, int nLen);

private:
    // ============= UI对象指针 =============
    RoomDialog* m_pRoomDialog;              // 房间对话框
    VedioConferenceDialog* m_pVedio;       // 视频会议主界面

    // ============= 用户身份信息 =============
    int m_id;                              // 当前用户ID (服务器分配)
    int m_roomId;                          // 当前房间ID (0=未在房间)
    QString m_name;                        // 当前用户昵称

    // ============= 房间成员管理 =============
    // 用户ID -> 音频播放对象映射表
    // 功能: 存储房间内所有成员的音频播放对象
    // 生命周期: 成员加入时创建，离开时销毁
    std::map<int, Audio_Write*> m_mapIdToAudioWrite;
};

#endif // CROOMMANAGER_H
