#ifndef CLOGINMANAGER_H
#define CLOGINMANAGER_H

#include <QObject>
#include <QString>
#include <QMessageBox>
#include "packdef.h"

// 前向声明，解耦
class LoginDialog;

class CLoginManager : public QObject
{
    Q_OBJECT
public:
    explicit CLoginManager(QObject *parent = nullptr);
    ~CLoginManager();

    // 初始化时注入依赖
    void setLoginDialog(LoginDialog* dialog) { m_pLogin = dialog; }

signals:
    // 登录成功后抛出信号给 CKernel，通知记录 ID 和 Name 等
    void SIG_LoginSuccess(int userId, QString name);
    // 通知 CKernel 发送网络数据
    void SIG_SendData(char* buf, int nLen);

public slots:
    // 处理用户界面的登录/注册提交
    void slot_loginCommit(QString tel, QString password);
    void slot_registerCommit(QString tel, QString password, QString name);

    // 处理网络层分发过来的登录/注册回复
    void slot_dealLoginRs(uint sock, char* buf, int nLen);
    void slot_dealRegisterRs(uint sock, char* buf, int nLen);

private:
    std::string GetMD5(QString value);

private:
    LoginDialog* m_pLogin;       // 持有UI的指针（只调用显示/隐藏/弹窗）
};

#endif // CLOGINMANAGER_H
