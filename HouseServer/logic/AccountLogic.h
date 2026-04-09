#ifndef ACCOUNTLOGIC_H
#define ACCOUNTLOGIC_H

class CKernel;
class CMysql;
typedef int sock_fd;

class AccountLogic
{
public:
    AccountLogic(CKernel* pKernel);
    ~AccountLogic();

    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);        //处理注册请求
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);           //处理登录请求

private:
    CKernel* m_pKernel;                                             //指向中介者
    CMysql* m_sql;                                                  //指向mysql数据库
};

#endif // ACCOUNTLOGIC_H
