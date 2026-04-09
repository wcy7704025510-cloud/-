#ifndef VIDEOCONFERENCEDIALOG_H
#define VIDEOCONFERENCEDIALOG_H

#include <QDialog>            // Qt对话框基类
#include<QMessageBox>         // Qt消息对话框(退出确认)
#include<QCloseEvent>        // Qt关闭事件
#include<QDebug>            // Qt调试输出

QT_BEGIN_NAMESPACE
namespace Ui { class VedioConferenceDialog; }
QT_END_NAMESPACE

// VedioConferenceDialog - 视频会议主界面类
// 职责：会议大厅主界面，提供创建/加入房间入口
// 界面设计：登录成功后显示的房间列表/控制大厅
// 协作：界面按钮 -> 信号 -> CRoomManager
class VedioConferenceDialog : public QDialog
{
    Q_OBJECT                    // Qt元对象系统宏

signals:
    // 主界面关闭信号
    // 触发: 用户确认退出后
    // 接收者: Ckernel::slot_destroy()
    // 功能: 通知核心类清理资源并退出程序
    void SIG_close();

    // 加入房间信号
    // 触发: 点击"加入会议"按钮
    // 接收者: Ckernel -> CRoomManager::slot_joinRoom()
    void SIG_joinRoom();

    // 创建房间信号
    // 触发: 点击"创建会议"按钮
    // 接收者: Ckernel -> CRoomManager::slot_createRoom()
    void SIG_createRoom();

public:
    // 构造函数/析构函数
    VedioConferenceDialog(QWidget *parent = nullptr);
    ~VedioConferenceDialog();

    // setInfo - 设置用户信息
    // 参数: name-用户昵称, icon-头像索引(默认1)
    // 功能: 在界面上显示登录用户的信息
    void setInfo(QString name, int icon=1);

    // closeEvent - 窗口关闭事件重写
    // 功能: 点击关闭按钮时弹出确认对话框
    void closeEvent(QCloseEvent* event);

private slots:
    // 创建会议按钮槽
    void on_tb_create_clicked();

    // 加入会议按钮槽
    void on_tb_join_clicked();

private:
    // Qt Designer生成的UI界面指针
    Ui::VedioConferenceDialog *ui;
};
#endif // VEDIOCONFERENCEDIALOG_H
