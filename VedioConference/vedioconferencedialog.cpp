#include "vedioconferencedialog.h"
#include "ui_vedioconferencedialog.h"

VedioConferenceDialog::VedioConferenceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VedioConferenceDialog)
{
    ui->setupUi(this);
    //设置主界面标题
    setWindowTitle("视频会议v1.0");
}

VedioConferenceDialog::~VedioConferenceDialog()
{
    delete ui;
}

//设置主界面用户信息
void VedioConferenceDialog::setInfo(QString name, int icon)
{
    //设置名字 头像
    this->ui->lb_name->setText(name);
}

//窗口关闭时间
void VedioConferenceDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;
    if(QMessageBox::question(this,"提示","是否退出?")
            ==QMessageBox::Yes){    //确认退出
        Q_EMIT SIG_close();
        event->accept();            //执行此次退出操作
    }else{      //取消退出
        event->ignore();            //忽略此次退出操作
    }
}

//创建会议
void VedioConferenceDialog::on_tb_create_clicked()
{
    //发送信号通知核心类处理
    Q_EMIT SIG_createRoom();
}

//加入会议
void VedioConferenceDialog::on_tb_join_clicked()
{
    //发送信号通知核心类处理
    Q_EMIT SIG_joinRoom();
}

