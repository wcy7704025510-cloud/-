#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

// Qt对话框框架
#include <QDialog>            // Qt对话框基类
#include <QDebug>           // Qt调试输出
#include <QRegExp>          // Qt正则表达式(手机号验证)
#include <QMessageBox>      // Qt消息对话框
#include <QCloseEvent>      // Qt关闭事件

// UI命名空间(由Qt Designer生成的界面类)
namespace Ui {
class LoginDialog;
}

// LoginDialog - 登录注册界面类
// 职责：用户登录/注册的UI交互
// 协作：UI输入 -> 信号 -> CLoginManager -> 网络 -> 服务器
// 界面设计：TabWidget切换登录/注册两个页面
class LoginDialog : public QDialog
{
    Q_OBJECT                    // Qt元对象系统宏
signals:
    // 登录提交信号
    // 参数: tel-手机号/账号, password-密码(MD5加密后传输)
    // 接收者: Ckernel -> CLoginManager::slot_loginCommit()
    void SIG_loginCommit(QString tel, QString password);

    // 注册提交信号
    // 参数: tel-手机号, password-密码, name-昵称
    // 接收者: Ckernel -> CLoginManager::slot_registerCommit()
    void SIG_registerCommit(QString tel, QString password, QString name);

    // 界面关闭信号
    // 触发: 点击关闭按钮
    // 接收者: Ckernel::slot_destroy()
    // 特殊处理: 隐藏而非真正关闭，保持后台运行
    void SIG_close();

public:
    // 构造函数
    // 参数: parent-父窗口指针
    explicit LoginDialog(QWidget *parent = nullptr);

    // 析构函数
    ~LoginDialog();

    // closeEvent - 窗口关闭事件重写
    // 参数: events-关闭事件对象
    // 功能: 点击关闭按钮时，隐藏窗口而非真正关闭
    void closeEvent(QCloseEvent* events);

private slots:
    // 登录按钮点击槽
    // 功能: 获取UI输入，校验数据，发射登录信号
    void on_pb_commit_clicked();

    // 清空注册信息按钮槽
    void on_pb_clean_register_clicked();

    // 提交注册信息按钮槽
    // 功能: 获取UI输入，校验数据，发射注册信号
    void on_pb_commit_register_clicked();

    // 清空登录信息按钮槽
    void on_pb_clean_clicked();

private:
    // Qt Designer生成的UI界面指针
    // 由uic工具从logindialog.ui自动生成
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
