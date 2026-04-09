#include "CLoginManager.h"          // 登录管理器头文件
#include "md5.h"                   // MD5加密算法实现
#include "logindialog.h"           // 登录界面头文件
#include <QDebug>                  // Qt调试输出宏

// MD5加密盐值
// 作用：对用户密码添加额外字符串再加密，增强安全性防止彩虹表攻击
// 原理：密码明文 + 盐值 -> MD5哈希，即使相同密码也不同哈希值
#define MD5_KEY (1234)

// CLoginManager构造函数
// 初始化成员变量指针为nullptr，防止野指针
CLoginManager::CLoginManager(QObject *parent) : QObject(parent), m_pLogin(nullptr)
{
}

// GetMD5 - MD5加密计算
// 参数: value - 待加密字符串(用户输入的密码)
// 返回: 32位MD5十六进制哈希值
// 实现: 将密码与盐值拼接后进行MD5哈希，防止密码泄露
std::string CLoginManager::GetMD5(QString value)
{
    // 拼接：密码_盐值 (如 "123456_1234")
    QString str = QString("%1_%2").arg(value).arg(MD5_KEY);
    // QString转std::string (UTF-8编码)
    std::string strSrc = str.toStdString();
    // 计算MD5哈希
    MD5 md5(strSrc);
    // 返回32位十六进制字符串
    return md5.toString();
}

// slot_loginCommit - 处理登录提交
// 参数: tel - 用户手机号/账号
//       password - 用户密码(明文)
// 功能: 密码MD5加密，封装登录请求包，发送到服务器
void CLoginManager::slot_loginCommit(QString tel, QString password)
{
    qDebug() << __func__;         // 调试输出函数名

    // 将QString转换为std::string用于协议结构体
    std::string strTel = tel.toStdString();

    // 封装登录请求包
    STRU_LOGIN_RQ rq;
    // 复制手机号到协议结构体(固定长度字符数组)
    strcpy(rq.tel, strTel.c_str());

    // 密码MD5加密
    std::string strPassMD5 = GetMD5(password);
    // 复制加密后密码到协议结构体
    strcpy(rq.password, strPassMD5.c_str());

    // 发送登录请求到服务器
    emit SIG_SendData((char*)&rq, sizeof(rq));
}

// slot_registerCommit - 处理注册提交
// 参数: tel - 注册手机号
//       password - 注册密码
//       name - 用户昵称
// 功能: 验证输入，密码MD5加密，封装注册请求包
void CLoginManager::slot_registerCommit(QString tel, QString password, QString name)
{
    qDebug() << __func__;         // 调试输出函数名

    std::string strTel = tel.toStdString();

    // 封装注册请求包
    STRU_REGISTER_RQ rq;
    strcpy(rq.tel, strTel.c_str());

    // 密码MD5加密
    std::string strPassMD5 = GetMD5(password);
    strcpy(rq.password, strPassMD5.c_str());

    // 复制昵称到协议结构体
    strcpy(rq.name, name.toStdString().c_str());

    // 发送注册请求到服务器
    emit SIG_SendData((char*)&rq, sizeof(rq));
}

// slot_dealLoginRs - 处理登录响应
// 参数: sock - socket标识符(未使用)
//       buf - 服务器响应数据包
//       nLen - 数据长度
// 功能: 解析登录结果，失败弹窗提示，成功则隐藏登录页并通知上层
void CLoginManager::slot_dealLoginRs(uint sock, char *buf, int nLen)
{
    qDebug() << __func__;         // 调试输出函数名

    // 解析登录响应包
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;

    // 根据结果码处理不同情况
    switch(rs->result){
    case user_not_exist:          // 用户不存在
        // 弹出提示对话框
        if(m_pLogin) QMessageBox::about(m_pLogin, "提示", "登录失败，当前手机号未被注册");
        break;
    case password_error:           // 密码错误
        if(m_pLogin) QMessageBox::about(m_pLogin, "提示", "登录失败，密码错误");
        break;
    case login_success:           // 登录成功
        // 隐藏登录界面
        if(m_pLogin) m_pLogin->hide();
        // 发送登录成功信号，携带用户ID和昵称
        emit SIG_LoginSuccess(rs->userid, QString::fromLocal8Bit(rs->m_name));
        break;
    default:                       // 未知结果码
        break;
    }
}

// slot_dealRegisterRs - 处理注册响应
// 参数: sock - socket标识符
//       buf - 服务器响应数据包
//       nLen - 数据长度
// 功能: 解析注册结果，失败弹窗提示，成功弹窗提示并自动切换到登录页
void CLoginManager::slot_dealRegisterRs(uint sock, char *buf, int nLen)
{
    qDebug() << __func__;         // 调试输出函数名

    // 解析注册响应包
    STRU_REGISTER_RS* rs = (STRU_REGISTER_RS*)buf;

    switch(rs->result){
    case tel_is_exist:            // 手机号已注册
        if(m_pLogin) QMessageBox::about(m_pLogin, "提示", "注册失败，当前手机号已被注册");
        break;
    case name_is_exist:           // 昵称已存在
        if(m_pLogin) QMessageBox::about(m_pLogin, "提示", "注册失败，当前昵称已被注册");
        break;
    case register_success:        // 注册成功
        if(m_pLogin) QMessageBox::about(m_pLogin, "提示", "注册成功");
        break;
    default:
        break;
    }
}
