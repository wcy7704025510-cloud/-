#include "CLoginManager.h"
#include "md5.h"
#include "logindialog.h"
#include <QDebug>

#define MD5_KEY (1234)

CLoginManager::CLoginManager(QObject *parent) : QObject(parent), m_pLogin(nullptr)
{
}

CLoginManager::~CLoginManager()
{
}

std::string CLoginManager::GetMD5(QString value)
{
    QString str=QString("%1_%2").arg(value).arg(MD5_KEY);
    std::string strSrc=str.toStdString();
    MD5 md5(strSrc);
    return md5.toString();
}

void CLoginManager::slot_loginCommit(QString tel, QString password)
{
    qDebug()<<__func__;

    std::string strTel=tel.toStdString();
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    
    std::string strPassMD5 = GetMD5(password);
    strcpy(rq.password,strPassMD5.c_str());

    emit SIG_SendData((char*)&rq, sizeof(rq));
}

void CLoginManager::slot_registerCommit(QString tel, QString password, QString name)
{
    qDebug()<<__func__;

    std::string strTel=tel.toStdString();
    STRU_REGISTER_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    
    std::string strPassMD5 = GetMD5(password);
    strcpy(rq.password,strPassMD5.c_str());

    strcpy(rq.name,name.toStdString().c_str());

    emit SIG_SendData((char*)&rq, sizeof(rq));
}

void CLoginManager::slot_dealLoginRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    STRU_LOGIN_RS* rs=(STRU_LOGIN_RS*)buf;
    
    switch(rs->result){
    case user_not_exist:
        if(m_pLogin) QMessageBox::about(m_pLogin,"提示","登录失败，当前手机号未被注册");
        break;
    case password_error:
        if(m_pLogin) QMessageBox::about(m_pLogin,"提示","注册失败，密码错误");
        break;
    case login_success:
        if(m_pLogin) m_pLogin->hide();
        emit SIG_LoginSuccess(rs->userid, QString::fromLocal8Bit(rs->m_name));
        break;
    default:
        break;
    }
}

void CLoginManager::slot_dealRegisterRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    STRU_REGISTER_RS* rs=(STRU_REGISTER_RS*)buf;
    
    switch(rs->result){
    case tel_is_exist:
        if(m_pLogin) QMessageBox::about(m_pLogin,"提示","注册失败，当前手机号已被注册");
        break;
    case name_is_exist:
        if(m_pLogin) QMessageBox::about(m_pLogin,"提示","注册失败，当前昵称已被注册");
        break;
    case register_success:
        if(m_pLogin) QMessageBox::about(m_pLogin,"提示","注册成功");
        break;
    default:
        break;
    }
}
