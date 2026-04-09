#ifndef CLOGINMANAGER_H
#define CLOGINMANAGER_H

#include <QObject>              // Qt对象基类，支持信号槽机制
#include <QString>              // Qt字符串类，Unicode编码，支持多语言
#include <QMessageBox>          // Qt消息对话框，用于显示错误/成功提示

#include "packdef.h"           // 协议包定义，包含数据包结构体和类型枚举

class LoginDialog;             // 登录界面类

// CLoginManager - 登录注册业务管理器
// 职责：处理用户登录、注册的业务逻辑，包括密码MD5加密、协议打包、网络请求发送
// 协作关系：
//   UI层(LoginDialog)  --SIG_loginCommit/SIG_registerCommit-->  Manager层
//   Manager层 --SIG_SendData--> Ckernel --网络发送--> 服务器
//   服务器响应 --Ckernel分发--> Manager层 --SIG_LoginSuccess--> Ckernel
class CLoginManager : public QObject
{
    Q_OBJECT                    // Qt元对象系统宏，启用信号槽功能
public:

    explicit CLoginManager(QObject *parent = nullptr);

    // 依赖注入接口
    // 参数: dialog - 登录界面指针，用于显示登录结果
    // 功能: 建立Manager与UI的关联，便于后续访问界面控件
    void setLoginDialog(LoginDialog* dialog) { m_pLogin = dialog; }

signals:
    // 登录成功信号
    // 参数: userId - 服务器分配的用户唯一标识符
    //       name   - 用户昵称，用于界面显示
    // 接收者: Ckernel::slot_LoginSuccess()
    // 功能: 通知上层登录成功，触发界面切换和用户信息保存
    void SIG_LoginSuccess(int userId, QString name);

    // 网络数据发送请求信号
    // 参数: buf - 待发送的数据缓冲区
    //       nLen - 数据长度(字节)
    // 接收者: Ckernel::slot_SendClientData()
    // 功能: 将业务数据转发给网络层发送到服务器
    void SIG_SendData(char* buf, int nLen);

public slots:
    // 处理用户登录提交
    // 参数: tel - 用户手机号/账号
    //       password - 用户密码(明文输入)
    // 功能: 对密码进行MD5加密，封装登录请求包，发送网络请求
    // 触发: LoginDialog点击登录按钮
    void slot_loginCommit(QString tel, QString password);

    // 处理用户注册提交
    // 参数: tel - 注册手机号/账号
    //       password - 注册密码
    //       name - 用户昵称
    // 功能: 验证输入合法性，MD5加密密码，封装注册请求包
    // 触发: LoginDialog点击注册按钮
    void slot_registerCommit(QString tel, QString password, QString name);

    // 处理服务器登录响应
    // 参数: sock - 网络socket标识(当前未使用)
    //       buf - 响应数据包缓冲区
    //       nLen - 数据长度
    // 功能: 解析登录结果，登录成功则发送SIG_LoginSuccess，失败则弹窗提示
    void slot_dealLoginRs(uint sock, char* buf, int nLen);

    // 处理服务器注册响应
    // 参数: sock - 网络socket标识
    //       buf - 响应数据包缓冲区
    //       nLen - 数据长度
    // 功能: 解析注册结果，成功弹窗提示并切换到登录页，失败显示错误信息
    void slot_dealRegisterRs(uint sock, char* buf, int nLen);

private:
    // MD5加密计算
    // 参数: value - 待加密的字符串(密码明文)
    // 返回: 32位MD5十六进制哈希字符串
    // 功能: 对用户密码进行MD5不可逆加密，密码传输和存储安全
    std::string GetMD5(QString value);

private:
    // 登录界面指针
    // 用途: 显示登录/注册结果(成功/失败提示对话框)
    LoginDialog* m_pLogin;
};

#endif // CLOGINMANAGER_H
