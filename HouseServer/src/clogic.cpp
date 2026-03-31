#include "clogic.h"

//设置函数影射数组
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
    NetPackMap(_DEF_CREATEROOM_RQ)       = &CLogic::CreateRoomRq;
    NetPackMap(_DEF_JOINROOM_RQ)         = &CLogic::JoinRoomRq;
    NetPackMap(_DEF_PACK_LEAVEROOM_RQ)   = &CLogic::LeaveRoomRq;
    NetPackMap(_DEF_PACK_AUDIO_FRAME)    = &CLogic::AudioFrameRq;
    NetPackMap(_DEF_PACK_VEDIO_FRAME)    = &CLogic::VideoFrameRq;
    NetPackMap(_DEF_PACK_AUDIO_REGISTER) = &CLogic::AudioRegister;
    NetPackMap(_DEF_PACK_VIDEO_REGISTER) = &CLogic::VideoRegister;
}

#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd << __func__ << endl;

//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    //cout << "clientfd:"<< clientfd << __func__ << endl;
    _DEF_COUT_FUNC_
   //拆包
   //获取tel password name
    STRU_REGISTER_RQ* rq=(STRU_REGISTER_RQ*)szbuf;
   STRU_REGISTER_RS rs;
   //查表 t_user 根据 tel查password
    char sqlStr[1024]={0};  //初始化sql查询字符串
    sprintf(sqlStr,"select tel from t_user where tel ='%s';",rq->tel);
    list<string> resList;
    if(!(m_sql->SelectMysql(sqlStr,1,resList))){
        printf("SelectMysql error: %s\n",sqlStr);
        return;
    }
   //有 user存在
    if(resList.size()>0){
        rs.result=tel_is_exist;
    }else{
        //没有 name存在
        resList.clear();
        memset(sqlStr,0,sizeof(sqlStr));
        sprintf(sqlStr,"select name from t_user where name ='%s';",rq->name);
        if(!(m_sql->SelectMysql(sqlStr,1,resList))){
            printf("SelectMysql error: %s\n",sqlStr);
            return;
        }
        if(resList.size()>0){
            rs.result=name_is_exist;
        }else{
            //name 不存在
            rs.result=register_success;
            //没有 写表 tel pass name 头像和签名的默认值 返回注册成功
            resList.clear();
            memset(sqlStr,0,sizeof(sqlStr));
            sprintf(sqlStr,"insert into t_user(tel,password,name,icon,feeling) values('%s','%s','%s','%d','%s');",
                    rq->tel,rq->password,rq->name,1,"比较懒，什么也没有写");
            if(!(m_sql->UpdataMysql(sqlStr))){
                printf("UpdataMysql error: %s\n",sqlStr);
                return;
            }
        }

    }
    //返回注册结果
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
//    cout << "clientfd:"<< clientfd << __func__ << endl;
    _DEF_COUT_FUNC_
    //拆包 手机号 密码
    STRU_LOGIN_RQ* rq=(STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;
    //根据tel查 pass 和 id
    char sqlStr[1024]={0};
    list<string> resList;
    sprintf(sqlStr,"select password,id,name from t_user where tel='%s';",rq->tel);
    if(!(m_sql->SelectMysql(sqlStr,3,resList))){
        printf("SelectMysql error,sqlStr=%s\n",sqlStr);
        return;
    }
    //查不到 用户不存在
    if(resList.size()==0){
        rs.result=user_not_exist;
    }else{
        //密码错误
        if(strcmp(resList.front().c_str(),rq->password)!=0){
            rs.result=password_error;
        }else{
            //密码正确,存储id->sock映射(通信)
            resList.pop_front();
            int id=atoi(resList.front().c_str());
            resList.pop_front();
            //存储登录成功的客户端socket(UserInfo结构体)
            UserInfo* pInfo=new UserInfo;
            pInfo->m_id=id;
            pInfo->m_roomid=0;
            pInfo->m_sockfd=clientfd;
            strcpy(pInfo->m_userName,resList.front().c_str());
            resList.pop_front();
            //判断id是否在线，在线，强制下线，不在线，添加
            if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
                //强制下线

            }
            m_pKernel->m_mapIdToUserInfo.insert(pInfo->m_id,pInfo);

            //登录回复结果为登录成功
            rs.result=login_success;
            rs.userid=id;
            strcpy(rs.m_name,pInfo->m_userName);
        }
    }
    //返回登录结果
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//创建房间
void CLogic::CreateRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1、拆包
    STRU_CREATEROOM_RQ* rq=(STRU_CREATEROOM_RQ*)szbuf;
    //2、随机数 得到房间号 看有没有房间号，可能循环随机 map roomid->list
    //房间号：1-8 位房间号
    int roomid=0;
    do{
        roomid=rand()%99999999+1;
    }while(m_pKernel->m_mapIdToRoomId.IsExist(roomid));
    list<int> lst;
    lst.push_back(rq->m_userId);
    m_pKernel->m_mapIdToRoomId.insert( roomid , lst );

    //3、回复
    STRU_CREATEROOM_RS rs;
    rs.m_result=create_success;
    rs.m_RoomId=roomid;
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//加入房间
void CLogic::JoinRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_JOINROOM_RQ* rq=(STRU_JOINROOM_RQ*)szbuf;
    STRU_JOINROOM_RS rs;
    rs.m_roomId=rq->m_RoomId;
    //查看房间是否存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->m_RoomId)){
        //不存在 返回失败
        rs.m_lResult=room_isnot_exist;
        SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
    //存在，返回成功
    rs.m_lResult=join_success;
    SendData(clientfd,(char*)&rs,sizeof(rs));
    //设置房间成员请求结构体
    if(!m_pKernel->m_mapIdToUserInfo.IsExist(rq->m_UserId)){
        return;
    }
    UserInfo* joiner;
    if(!m_pKernel->m_mapIdToUserInfo.find(rq->m_UserId,joiner)){
        return;
    }
    STRU_ROOM_MEMBER_RQ joinerrq;
    joinerrq.m_userId=rq->m_UserId;
    strcpy(joinerrq.m_userName,joiner->m_userName);
    //根据房间号拿到房间成员列表
    list<int> ls;
    if(!m_pKernel->m_mapIdToRoomId.find(rq->m_RoomId,ls)){
        return;
    }
    //返回自身信息，用于更新房间成员
    SendData(clientfd,(char*)&joinerrq,sizeof(joinerrq));
    //遍历列表 --交换信息
    int Memid=0;
    UserInfo* memInfo;
    STRU_ROOM_MEMBER_RQ memrq;
    for(auto ite=ls.begin();ite!=ls.end();ite++){
        //获取处理用户的用户信息
        Memid=*ite;
        if(!m_pKernel->m_mapIdToUserInfo.IsExist(Memid)){
            continue;
        }
        if(!m_pKernel->m_mapIdToUserInfo.find(Memid,memInfo)){
            continue;
        }
        //以当前用户信息为蓝本，构建房间成员请求结构体
        memrq.m_userId=memInfo->m_id;
        strcpy(memrq.m_userName,memInfo->m_userName);
        //把加入人的信息发给房间内的每一个成员
        SendData(memInfo->m_sockfd,(char*)&joinerrq,sizeof(joinerrq));
        //房间内成员每个人信息发给加入人
        SendData(clientfd,(char*)&memrq,sizeof(memrq));
    }
    //加入人 添加到房间列表
    ls.push_back(rq->m_UserId);
    m_pKernel->m_mapIdToRoomId.insert(rq->m_RoomId,ls);
}

//处理离开房间请求
void CLogic::LeaveRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)szbuf;
    //检查房间是否存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->RoomId)){
        return;
    }
    //房间存在，获得房间列表
    list<int> ls;
    if(!m_pKernel->m_mapIdToRoomId.find(rq->RoomId,ls)){
        return;
    }
    //遍历房间列表，向所有的在线用户转发离开房间请求
    for(auto ite=ls.begin();ite!=ls.end();){
        int userId=*ite;
        //是不是自己，如果是自己，从房间列表中去除
        if(userId == rq->m_nUserId){
            ite=ls.erase(ite);
        }else{
            //判断用户是否在线
            if(m_pKernel->m_mapIdToUserInfo.IsExist(userId)){
                UserInfo* info;
                if(!m_pKernel->m_mapIdToUserInfo.find(userId,info)){
                    continue;
                }
                SendData(info->m_sockfd,szbuf,nlen);
            }
            ++ite;
        }
        //列表是否节点数为0，是则将该房间对应的map项去除
        if(ls.size()==0){
            m_pKernel->m_mapIdToRoomId.erase(rq->RoomId);
        }else{
            //更新房间成员列表
            m_pKernel->m_mapIdToRoomId.insert(rq->RoomId,ls);
        }
    }
}


/*音视频数据帧
成员描述
int type;
int userId;
int roomId;
int min;
int sec;
int msec;
QByteArray audioFrame;
*/
//处理音频帧请求
void CLogic::AudioFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包，获取用户ID和房间ID
    char* tmp=szbuf;
    //跳过type
    tmp+=sizeof(int);
    //获取用户ID
    int userId=*(int*)tmp;
    //跳过用户ID
    tmp+=sizeof(int);
    //获取房间ID
    int roomId=*(int*)tmp;
    //查看房间是否存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)){
        return;
    }
    //获取房间成员列表
    list<int> lst;
    if(!m_pKernel->m_mapIdToRoomId.find(roomId,lst)){
        return;
    }
    //遍历，用户在线 转发音频帧
    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        //当前处理用户为自己，不转发，continue
        if(userId==ID){
            continue;
        }
        //判断当前处理的用户是否在线
        if(!m_pKernel->m_mapIdToUserInfo.IsExist(ID)){
            continue;
        }
        //取出用户信息结构体，获取当前处理用户的套接字
        UserInfo* userInfo;
        if(!m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            continue;
        }
        //转发音频帧至当前处理的用户
        SendData(userInfo->m_audiofd,szbuf,nlen);
    }
}

/*视频数据帧
 * 成员描述
 * int type;
 * int userId;
 * int roomId;
 * int min;
 * int sec;
 * int msec;
 * QByteArray videoFrame;
*/
//处理视频帧
void CLogic::VideoFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包，获取用户ID和房间ID
    char* tmp=szbuf;
    //跳过type
    tmp+=sizeof(int);
    //获取用户ID
    int userId=*(int*)tmp;
    //跳过用户ID
    tmp+=sizeof(int);
    //获取房间ID
    int roomId=*(int*)tmp;
    //查看房间是否存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)){
        return;
    }
    //获取房间成员列表
    list<int> lst;
    if(!m_pKernel->m_mapIdToRoomId.find(roomId,lst)){
        return;
    }
    //遍历，用户在线 转发视频帧
    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        //当前处理用户为自己，不转发，continue
        if(userId==ID){
            continue;
        }
        //判断当前处理的用户是否在线
        if(!m_pKernel->m_mapIdToUserInfo.IsExist(ID)){
            continue;
        }
        //取出用户信息结构体，获取当前处理用户的套接字
        UserInfo* userInfo;
        if(!m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            continue;
        }
        //转发视频帧至当前处理的用户
        SendData(userInfo->m_videofd,szbuf,nlen);
    }
}

//音频注册
void CLogic::AudioRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_AUDIO_REGISTER *rq=(STRU_AUDIO_REGISTER*)szbuf;
    int id=rq->userid;
    //根据 userid 找到对应的用户信息结构体 更新fd
    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_audiofd=clientfd;
    }
}

//视频注册
void CLogic::VideoRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_VIDEO_REGISTER *rq=(STRU_VIDEO_REGISTER*)szbuf;
    int id=rq->userid;
    //根据 userid 找到对应的用户信息结构体 更新fd
    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_videofd=clientfd;
    }
}
