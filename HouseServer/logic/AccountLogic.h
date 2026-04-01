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

    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);

private:
    CKernel* m_pKernel;
    CMysql* m_sql;
};

#endif // ACCOUNTLOGIC_H