#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>            // Qt对话框基类
#include <QImage>           // Qt图像类(视频帧显示)
#include <QVBoxLayout>      // Qt垂直布局管理器
#include <QCloseEvent>       // Qt关闭事件
#include <QMessageBox>       // Qt消息对话框(退出确认)
#include <map>             // C++关联容器(用户ID映射)

#include "./usershow.h"     // 用户视频显示控件

namespace Ui {
class RoomDialog;
}

// RoomDialog - 会议房间界面类
// 职责：会议房间内界面，管理房间成员视频显示、控制音视频设备
// 界面设计：
//   左侧/中间: 大预览窗口(显示选中用户的视频)
//   右侧/下方: 成员列表(所有房间成员的缩略视频)
//   底部: 音频/视频/桌面共享控制按钮
// 协作：
//   UI控制 -> 信号 -> CMediaManager(音视频控制)
//   服务器用户变动 -> CRoomManager -> 更新成员列表
class RoomDialog : public QDialog
{
    Q_OBJECT                    // Qt元对象系统宏

signals:
    // 房间关闭信号
    // 触发: 用户确认退出房间
    // 接收者: Ckernel -> CRoomManager::slot_QuitRoom()
    void SIG_close();

    // 音频控制信号
    void SIG_AudioPause();   // 暂停音频采集
    void SIG_AudioStart();   // 开始音频采集

    // 视频控制信号
    void SIG_VideoStart();   // 开启摄像头采集
    void SIG_VideoPause();   // 暂停摄像头采集

    // 屏幕共享控制信号
    void SIG_ScreenStart();  // 开启屏幕共享
    void SIG_ScreenPause();  // 暂停屏幕共享

public:
    // 构造函数/析构函数
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();

    // ============= 房间信息管理槽 =============

    // 设置房间号显示
    // 参数: roomid - 房间号字符串
    // 功能: 更新窗口标题和界面显示
    void slot_setInfo(QString roomid);

    // 添加房间成员
    // 参数: user - 新成员的用户视频显示控件指针
    // 功能: 将新成员添加到成员列表显示
    void slot_addUserInfo(UserShow* user);

    // 删除房间成员
    // 参数1: user - 待删除的用户控件指针
    // 参数2: id - 待删除的用户ID
    // 功能: 从成员列表移除并销毁对应控件
    void slot_removeUserShow(UserShow* user);
    void slot_removeUserShow(int id);

    // 清空所有房间成员
    // 功能: 退出房间时调用，清理所有成员控件
    void slot_clearUserShow();

    // ============= 设备控制状态槽 =============

    // 设置音频复选框状态
    void slot_setAudioSet(bool check);

    // 设置视频复选框状态
    void slot_setvideoSet(bool check);

    // 设置屏幕共享复选框状态
    void slot_setScreenSet(bool check);

    // ============= 视频画面更新槽 =============

    // 刷新指定用户的视频画面
    // 参数: id - 用户ID, img - 解码后的视频帧
    // 功能: 更新该用户的大预览窗口和小窗口显示
    void slot_refreshUser(int id, QImage& img);

    // 设置大预览窗口的用户信息
    // 参数: id - 用户ID, name - 用户昵称
    // 功能: 点击用户小窗口时调用，设置预览窗口信息
    void slot_setBigImgId(int id, QString name);

private slots:
    // 关闭按钮点击
    void on_pb_close_clicked();

    // 退出按钮点击
    void on_pb_exit_clicked();

    // 音频复选框切换
    // 功能: 根据勾选状态开启/暂停音频采集
    void on_cb_audio_clicked();

    // 视频复选框切换
    // 功能: 开启视频时自动关闭桌面共享(互斥)
    void on_cb_vedio_clicked();

    // 桌面共享复选框切换
    // 功能: 开启桌面共享时自动关闭视频(互斥)
    void on_cb_desk_clicked();

private:
    // 窗口关闭事件
    // 功能: 弹出确认对话框，用户确认后发送退出信号
    void closeEvent(QCloseEvent* event);

private:
    // Qt Designer生成的UI界面指针
    Ui::RoomDialog *ui;

    // 主垂直布局管理器
    // 用途: 动态管理成员列表中的UserShow控件
    QVBoxLayout* m_mainLayout;

    // 用户ID到显示控件的映射表
    // Key: 用户ID(服务器分配)
    // Value: 该用户的UserShow视频显示控件
    // 用途: 根据用户ID快速查找对应的显示控件
    std::map<int, UserShow*> m_mapIdToUserShow;
};

#endif // ROOMDIALOG_H
