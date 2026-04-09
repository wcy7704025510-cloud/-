#include "AccountLogic.h"
#include "CKernel.h"
#include"common/packdef.h"
#include <string.h>
#include <iostream>

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << __func__ << std::endl;  

//初始化
AccountLogic::AccountLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

AccountLogic::~AccountLogic() {}


//处理注册请求
void AccountLogic::RegisterRq(sock_fd clientfd, char* szbuf, int nlen)
{
    _DEF_COUT_FUNC_         //输出一下函数
    STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)szbuf;        //拆包
    STRU_REGISTER_RS rs;

    char sqlStr[1024] = {0};
    //输入电话号码
    sprintf(sqlStr, "select tel from t_user where tel ='%s';", rq->tel);

    std::list<std::string> resList;
    //查询数据库中电话号码是否存在（sql语句，查询的1个参数，返回的链表）
    if(!m_sql->SelectMysql(sqlStr, 1, resList)) return;

    //如果电话号码存在证明电话已经存在
    if(resList.size() > 0) {
        rs.result = tel_is_exist;
    } else {
        resList.clear();
        memset(sqlStr, 0, sizeof(sqlStr));
        //查询名字是否存在
        sprintf(sqlStr, "select name from t_user where name ='%s';", rq->name);   
        if(!m_sql->SelectMysql(sqlStr, 1, resList)) return;

        if(resList.size() > 0) {
            rs.result = name_is_exist;
        } else {
            //名字与电话号码均不存在
            //注册成功
            rs.result = register_success;
            resList.clear();
            memset(sqlStr, 0, sizeof(sqlStr));
            //插入语句
            sprintf(sqlStr, "insert into t_user(tel,password,name,icon,feeling) values('%s','%s','%s','%d','%s');",
                    rq->tel, rq->password, rq->name, 1, "比较懒，什么也没有~");
            m_sql->UpdataMysql(sqlStr);
        }
    }
    //将注册结果发回给客户端
    m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));
}

//登录请求
void AccountLogic::LoginRq(sock_fd clientfd, char* szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;

    char sqlStr[1024] = {0};
    std::list<std::string> resList;
    //查询个人信息（sql语句，查询的三个参数，返回的链表）
    sprintf(sqlStr, "select password,id,name from t_user where tel='%s';", rq->tel);
    if(!m_sql->SelectMysql(sqlStr, 3, resList)) return;
    //如果什么都没有则证明不存在
    if(resList.size() == 0) {
        rs.result = user_not_exist;
    } else {
        //如果查询到的密码和请求的密码不同则返回密码错误
        if(strcmp(resList.front().c_str(), rq->password) != 0) {
            rs.result = password_error;
        } else {
            //将密码弹出
            resList.pop_front();
            //得到客户端的用户id
            int id = atoi(resList.front().c_str());
            resList.pop_front();
            //创建新的用户信息（并将其赋值）
            UserInfo* pInfo = new UserInfo;
            pInfo->m_id = id;
            pInfo->m_roomid = 0;
            pInfo->m_sockfd = clientfd;
            strcpy(pInfo->m_userName, resList.front().c_str());

            if(m_pKernel->m_mapIdToUserInfo.IsExist(id)) {
                // TODO: 强制下线逻辑
            }
            //将用户的id到个人信息存储
            m_pKernel->m_mapIdToUserInfo.insert(pInfo->m_id, pInfo);

            rs.result = login_success;
            rs.userid = id;
            strcpy(rs.m_name, pInfo->m_userName);
        }
    }
    //将登录成功的信息返回给客户端
    m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));
}
