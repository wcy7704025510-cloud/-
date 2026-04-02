#include "videoconferencedialog.h"
#include "ui_videoconferencedialog.h"
#include <QMessageBox>
#include <QDebug>

VedioConferenceDialog::VedioConferenceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VedioConferenceDialog)
{
    ui->setupUi(this);

    /*设置窗口标题 */
    setWindowTitle("视频会议 v1.0");
}

VedioConferenceDialog::~VedioConferenceDialog()
{
    delete ui;
}

/*
 * 设置用户信息
 * @param name 用户昵称
 * @param icon 用户头像索引
 */
void VedioConferenceDialog::setInfo(QString name, int icon)
{
    /*设置昵称标签内容*/
    this->ui->lb_name->setText(name);
}

/*
 * 窗口关闭事件重写
 * 当点击窗口右上角的 'X' 按钮时触发
 */
void VedioConferenceDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;

    /*弹出询问对话框：确认退出吗？ */
    if(QMessageBox::question(this, "退出", "确定要退出会议吗？")
            == QMessageBox::Yes){

        //如果用户点“是”，向核心逻辑层发送关闭信号
        Q_EMIT SIG_close();

        /*accept() 告诉 Qt 处理并同意该事件，窗口将真正关闭 */
        event->accept();
    } else {

        // 如果用户点“否”
        /* ignore() 忽略该事件，窗口将保持原样，不会关闭 */
        event->ignore();
    }
}

// “创建会议”按钮点击槽函数
void VedioConferenceDialog::on_tb_create_clicked()
{
    //发送创建房间信号，由 CKernel 等核心逻辑类接收处理
    Q_EMIT SIG_createRoom();
}

//“加入会议”按钮点击槽函数
void VedioConferenceDialog::on_tb_join_clicked()
{
    //发送加入房间信号
    Q_EMIT SIG_joinRoom();
}
