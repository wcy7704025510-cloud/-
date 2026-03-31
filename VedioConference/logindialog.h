#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_loginCommit(QString tel,QString password);
    void SIG_registerCommit(QString tel,QString password,QString name);
    void SIG_close();   //登录注册界面关闭信号
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    //登陆注册界面关闭事件处理
    void closeEvent(QCloseEvent* events);

private slots:
    void on_pb_commit_clicked();

    void on_pb_clean_register_clicked();

    void on_pb_commit_register_clicked();

    void on_pb_clean_clicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
