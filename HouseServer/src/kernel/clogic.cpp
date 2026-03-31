#include "./kernel/clogic.h"
#include <string.h>

// 网络包映射宏：根据协议号找到对应的处理函数
#define NetPackMap(a) m_NetPackMap[a - _DEF_PACK_BASE]

// 打印日志宏：输出客户端fd和当前函数名
#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd << __func__ << endl;

// 初始化网络协议包与处理函数的映射关系
void CLogic::setNetPackMap()
{
    // 清空函数映射数组
    bzero(m_NetPackMap, sizeof(m_NetPackMap));

    // 绑定每个协议包对应的处理函数
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;     // 注册请求
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;        // 登录请求
    NetPackMap(_DEF_CREATEROOM_RQ)       = &CLogic::CreateRoomRq;   // 创建房间请求
    NetPackMap(_DEF_JOINROOM_RQ)         = &CLogic::JoinRoomRq;     // 加入房间请求
    NetPackMap(_DEF_PACK_LEAVEROOM_RQ)   = &CLogic::LeaveRoomRq;    // 离开房间请求
    NetPackMap(_DEF_PACK_AUDIO_FRAME)    = &CLogic::AudioFrameRq;    // 音频数据帧
    NetPackMap(_DEF_PACK_VEDIO_FRAME)    = &CLogic::VideoFrameRq;    // 视频数据帧
    NetPackMap(_DEF_PACK_AUDIO_REGISTER) = &CLogic::AudioRegister;  // 音频通道注册
    NetPackMap(_DEF_PACK_VIDEO_REGISTER) = &CLogic::VideoRegister;  // 视频通道注册
}

// 处理客户端数据，根据协议类型跳转到对应函数
void CLogic::DealData(sock_fd clientfd, char *szbuf, int nlen)
{
    // 取出协议头类型
    PackType type = *(PackType*)szbuf;

    // 判断协议是否合法
    if ((type >= _DEF_PACK_BASE) && (type < _DEF_PACK_BASE + _DEF_PACK_COUNT))
    {
        // 找到对应处理函数
        PFUN pf = NetPackMap(type);
        if (pf)
        {
            // 调用处理函数
            (this->*pf)(clientfd, szbuf, nlen);
        }
    }
}

//======================== 业务处理函数 ========================

// 注册请求处理
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_
    // 拆包：获取手机号、密码、昵称
    STRU_REGISTER_RQ* rq=(STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;

    // 1. 查询手机号是否已被注册
    char sqlStr[1024]={0};
    sprintf(sqlStr,"select tel from t_user where tel ='%s';",rq->tel);
    list<string> resList;
    if(!m_sql->SelectMysql(sqlStr,1,resList)) return;

    if(resList.size()>0){
        rs.result=tel_is_exist; // 手机号已存在
    }else{
        // 2. 查询昵称是否已存在
        resList.clear();
        memset(sqlStr,0,sizeof(sqlStr));
        sprintf(sqlStr,"select name from t_user where name ='%s';",rq->name);
        if(!m_sql->SelectMysql(sqlStr,1,resList)) return;

        if(resList.size()>0){
            rs.result=name_is_exist; // 昵称已存在
        }else{
            // 3. 注册成功，插入用户数据（默认头像、签名）
            rs.result=register_success;
            resList.clear();
            memset(sqlStr,0,sizeof(sqlStr));
            sprintf(sqlStr,"insert into t_user(tel,password,name,icon,feeling) values('%s','%s','%s','%d','%s');",
                    rq->tel,rq->password,rq->name,1,"比较懒，什么也没有?");
            m_sql->UpdataMysql(sqlStr);
        }
    }
    // 回复注册结果给客户端
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

// 登录请求处理
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_
    // 拆包：获取手机号、密码
    STRU_LOGIN_RQ* rq=(STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;

    // 查询用户信息
    char sqlStr[1024]={0};
    list<string> resList;
    sprintf(sqlStr,"select password,id,name from t_user where tel='%s';",rq->tel);
    if(!m_sql->SelectMysql(sqlStr,3,resList)) return;

    if(resList.size()==0){
        rs.result=user_not_exist; // 用户不存在
    }else{
        // 校验密码
        if(strcmp(resList.front().c_str(),rq->password)!=0){
            rs.result=password_error; // 密码错误
        }else{
            // 密码正确，获取用户ID和昵称
            resList.pop_front();
            int id=atoi(resList.front().c_str());
            resList.pop_front();

            // 保存用户在线信息
            UserInfo* pInfo=new UserInfo;
            pInfo->m_id=id;
            pInfo->m_roomid=0;
            pInfo->m_sockfd=clientfd;
            strcpy(pInfo->m_userName,resList.front().c_str());

            // 如果已在线，踢下线
            if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
                // 强制下线逻辑
            }
            // 加入在线用户映射
            m_pKernel->m_mapIdToUserInfo.insert(pInfo->m_id,pInfo);

            // 回复登录成功
            rs.result=login_success;
            rs.userid=id;
            strcpy(rs.m_name,pInfo->m_userName);
        }
    }
    // 回复登录结果
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

// 创建房间
void CLogic::CreateRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_CREATEROOM_RQ* rq=(STRU_CREATEROOM_RQ*)szbuf;

    // 随机生成一个未被使用的房间号
    int roomid=0;
    do{
        roomid=rand()%99999999+1;
    }while(m_pKernel->m_mapIdToRoomId.IsExist(roomid));

    // 创建房间，把创建者加入房间
    list<int> lst;
    lst.push_back(rq->m_userId);
    m_pKernel->m_mapIdToRoomId.insert( roomid , lst );

    // 回复创建成功
    STRU_CREATEROOM_RS rs;
    rs.m_result=create_success;
    rs.m_RoomId=roomid;
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

// 加入房间
void CLogic::JoinRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_JOINROOM_RQ* rq=(STRU_JOINROOM_RQ*)szbuf;
    STRU_JOINROOM_RS rs;
    rs.m_roomId=rq->m_RoomId;

    // 房间不存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->m_RoomId)){
        rs.m_lResult=room_isnot_exist;
        SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }

    // 加入成功
    rs.m_lResult=join_success;
    SendData(clientfd,(char*)&rs,sizeof(rs));

    // 获取加入者信息
    UserInfo* joiner;
    m_pKernel->m_mapIdToUserInfo.find(rq->m_UserId,joiner);
    STRU_ROOM_MEMBER_RQ joinerrq;
    joinerrq.m_userId=rq->m_UserId;
    strcpy(joinerrq.m_userName,joiner->m_userName);

    // 获取房间成员列表
    list<int> ls;
    m_pKernel->m_mapIdToRoomId.find(rq->m_RoomId,ls);

    // 把加入者信息发给客户端自己
    SendData(clientfd,(char*)&joinerrq,sizeof(joinerrq));

    // 房间内成员 ←→ 新成员 互相发送信息
    int Memid=0;
    UserInfo* memInfo;
    STRU_ROOM_MEMBER_RQ memrq;
    for(auto ite=ls.begin();ite!=ls.end();ite++){
        Memid=*ite;
        m_pKernel->m_mapIdToUserInfo.find(Memid,memInfo);

        // 把新成员发给房间内所有人
        SendData(memInfo->m_sockfd,(char*)&joinerrq,sizeof(joinerrq));
        // 把房间成员发给新成员
        SendData(clientfd,(char*)&memrq,sizeof(memrq));
    }

    //把新成员加入房间
    ls.push_back(rq->m_UserId);
    m_pKernel->m_mapIdToRoomId.insert(rq->m_RoomId,ls);
}

//离开房间
void CLogic::LeaveRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)szbuf;

    // 房间不存在直接返回
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->RoomId)) return;

    // 获取房间成员
    list<int> ls;
    m_pKernel->m_mapIdToRoomId.find(rq->RoomId,ls);

    // 遍历房间成员
    for(auto ite=ls.begin();ite!=ls.end();){
        int userId=*ite;
        // 找到自己，从房间移除
        if(userId == rq->m_nUserId){
            ite=ls.erase(ite);
        }else{
            // 转发离开通知给房间内其他人
            UserInfo* info;
            if(m_pKernel->m_mapIdToUserInfo.find(userId,info)){
                SendData(info->m_sockfd,szbuf,nlen);
            }
            ++ite;
        }
    }

    // 房间没人了就删除房间
    if(ls.size()==0){
        m_pKernel->m_mapIdToRoomId.erase(rq->RoomId);
    }else{
        m_pKernel->m_mapIdToRoomId.insert(rq->RoomId,ls);
    }
}

// 处理音频数据帧，转发给房间内其他人
void CLogic::AudioFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    char* tmp=szbuf;
    tmp+=sizeof(int);        // 跳过协议类型
    int userId=*(int*)tmp;   // 获取用户ID
    tmp+=sizeof(int);
    int roomId=*(int*)tmp;   // 获取房间ID

    // 房间不存在直接返回
    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)) return;

    // 获取房间成员
    list<int> lst;
    m_pKernel->m_mapIdToRoomId.find(roomId,lst);

    // 转发音频给房间内其他人（不发给自己）
    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        if(userId==ID) continue;

        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            // 发送到对方音频专用socket
            SendData(userInfo->m_audiofd,szbuf,nlen);
        }
    }
}

// 处理视频数据帧，转发给房间内其他人
void CLogic::VideoFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    char* tmp=szbuf;
    tmp+=sizeof(int);
    int userId=*(int*)tmp;
    tmp+=sizeof(int);
    int roomId=*(int*)tmp;

    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)) return;

    list<int> lst;
    m_pKernel->m_mapIdToRoomId.find(roomId,lst);

    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        if(userId==ID) continue;

        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            // 发送到对方视频专用socket
            SendData(userInfo->m_videofd,szbuf,nlen);
        }
    }
}

// 音频通道注册：把当前socket绑定为用户的音频通道
void CLogic::AudioRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_AUDIO_REGISTER *rq=(STRU_AUDIO_REGISTER*)szbuf;
    int id=rq->userid;

    // 更新用户的音频socket
    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_audiofd=clientfd;
    }
}

// 视频通道注册：把当前socket绑定为用户的视频通道
void CLogic::VideoRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_VIDEO_REGISTER *rq=(STRU_VIDEO_REGISTER*)szbuf;
    int id=rq->userid;

    // 更新用户的视频socket
    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_videofd=clientfd;
    }
}
