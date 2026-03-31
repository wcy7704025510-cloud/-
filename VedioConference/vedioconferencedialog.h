#ifndef VEDIOCONFERENCEDIALOG_H
#define VEDIOCONFERENCEDIALOG_H

#include <QDialog>
#include<QMessageBox>
#include<QCloseEvent>
#include<QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class VedioConferenceDialog; }
QT_END_NAMESPACE

class VedioConferenceDialog : public QDialog
{
    Q_OBJECT

signals:
    //主界面关闭发送给核心类的处理信号
    void SIG_close();
    //加入房间信号
    void SIG_joinRoom();
    //创建房间信号
    void SIG_createRoom();

public:
    VedioConferenceDialog(QWidget *parent = nullptr);
    ~VedioConferenceDialog();
    //设置主界面用户信息
    void setInfo(QString name,int icon=1);
    //主界面退出事件
    void closeEvent(QCloseEvent* event);
private slots:
    //创建会议按键按下
    void on_tb_create_clicked();
    //加入会议按键按下
    void on_tb_join_clicked();

private:
    Ui::VedioConferenceDialog *ui;
};
#endif // VEDIOCONFERENCEDIALOG_H
