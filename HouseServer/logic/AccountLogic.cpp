#include "AccountLogic.h"
#include "CKernel.h"
#include "packdef.h"
#include <string.h>
#include <iostream>

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << __func__ << std::endl;  

AccountLogic::AccountLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

AccountLogic::~AccountLogic() {}

void AccountLogic::RegisterRq(sock_fd clientfd, char* szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;

    char sqlStr[1024] = {0};
    sprintf(sqlStr, "select tel from t_user where tel ='%s';", rq->tel);
    std::list<std::string> resList;
    if(!m_sql->SelectMysql(sqlStr, 1, resList)) return;

    if(resList.size() > 0) {
        rs.result = tel_is_exist;
    } else {
        resList.clear();
        memset(sqlStr, 0, sizeof(sqlStr));
        sprintf(sqlStr, "select name from t_user where name ='%s';", rq->name);   
        if(!m_sql->SelectMysql(sqlStr, 1, resList)) return;

        if(resList.size() > 0) {
            rs.result = name_is_exist;
        } else {
            rs.result = register_success;
            resList.clear();
            memset(sqlStr, 0, sizeof(sqlStr));
            sprintf(sqlStr, "insert into t_user(tel,password,name,icon,feeling) values('%s','%s','%s','%d','%s');",
                    rq->tel, rq->password, rq->name, 1, "比较懒，什么也没有~");
            m_sql->UpdataMysql(sqlStr);
        }
    }
    m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));
}

void AccountLogic::LoginRq(sock_fd clientfd, char* szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;

    char sqlStr[1024] = {0};
    std::list<std::string> resList;
    sprintf(sqlStr, "select password,id,name from t_user where tel='%s';", rq->tel);
    if(!m_sql->SelectMysql(sqlStr, 3, resList)) return;

    if(resList.size() == 0) {
        rs.result = user_not_exist;
    } else {
        if(strcmp(resList.front().c_str(), rq->password) != 0) {
            rs.result = password_error;
        } else {
            resList.pop_front();
            int id = atoi(resList.front().c_str());
            resList.pop_front();

            UserInfo* pInfo = new UserInfo;
            pInfo->m_id = id;
            pInfo->m_roomid = 0;
            pInfo->m_sockfd = clientfd;
            strcpy(pInfo->m_userName, resList.front().c_str());

            if(m_pKernel->m_mapIdToUserInfo.IsExist(id)) {
                // TODO: 强制下线逻辑
            }
            m_pKernel->m_mapIdToUserInfo.insert(pInfo->m_id, pInfo);

            rs.result = login_success;
            rs.userid = id;
            strcpy(rs.m_name, pInfo->m_userName);
        }
    }
    m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));
}