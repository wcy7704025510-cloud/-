#include "RoomLogic.h"
#include "CKernel.h"
#include "packdef.h"
#include <string.h>
#include <iostream>

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << __func__ << std::endl;

RoomLogic::RoomLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

RoomLogic::~RoomLogic() {}

void RoomLogic::CreateRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_CREATEROOM_RQ* rq=(STRU_CREATEROOM_RQ*)szbuf;

    int roomid=0;
    do{
        roomid=rand()%99999999+1;
    }while(m_pKernel->m_mapIdToRoomId.IsExist(roomid));

    std::list<int> lst;
    lst.push_back(rq->m_userId);
    m_pKernel->m_mapIdToRoomId.insert(roomid, lst);

    STRU_CREATEROOM_RS rs;
    rs.m_result=create_success;
    rs.m_RoomId=roomid;
    m_pKernel->SendData(clientfd,(char*)&rs,sizeof(rs));
}
void RoomLogic::JoinRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_

    // 1. 拆包（安全强转）
    STRU_JOINROOM_RQ* rq = (STRU_JOINROOM_RQ*)szbuf;
    STRU_JOINROOM_RS rs;
    rs.m_roomId = rq->m_RoomId;

    // 2. 判断房间是否存在（不存在直接返回）
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->m_RoomId)){
        rs.m_lResult = room_isnot_exist;
        m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));
        return;
    }

    // 3. 房间存在 → 返回加入成功
    rs.m_lResult = join_success;
    m_pKernel->SendData(clientfd, (char*)&rs, sizeof(rs));

    //判断用户是否存在
    if(!m_pKernel->m_mapIdToUserInfo.IsExist(rq->m_UserId)){
        return;
    }

    // find 判断返回值，防止空指针
    UserInfo* joiner = nullptr;
    if(!m_pKernel->m_mapIdToUserInfo.find(rq->m_UserId, joiner) || joiner == nullptr){
        return;
    }

    // 组装加入者信息包
    STRU_ROOM_MEMBER_RQ joinerrq;
    joinerrq.m_userId = rq->m_UserId;
    strcpy(joinerrq.m_userName, joiner->m_userName);

    // 获取房间成员列表，判断是否成功
    std::list<int> ls;
    if(!m_pKernel->m_mapIdToRoomId.find(rq->m_RoomId, ls)){
        return;
    }

    // 把自己的信息发给自己（用于客户端更新成员列表）
    m_pKernel->SendData(clientfd, (char*)&joinerrq, sizeof(joinerrq));

    //遍历房间成员，安全遍历 + 双向发送
    int Memid = 0;
    UserInfo* memInfo = nullptr;
    STRU_ROOM_MEMBER_RQ memrq;

    for(auto ite = ls.begin(); ite != ls.end(); ite++){
        Memid = *ite;

        // 跳过不存在的用户
        if(!m_pKernel->m_mapIdToUserInfo.IsExist(Memid)){
            continue;
        }
        if(!m_pKernel->m_mapIdToUserInfo.find(Memid, memInfo) || memInfo == nullptr){
            continue;
        }

        // 给 memrq 赋值
        memrq.m_userId = memInfo->m_id;
        strcpy(memrq.m_userName, memInfo->m_userName);

        // 1. 把【新加入的人】发给【房间内所有老成员】
        m_pKernel->SendData(memInfo->m_sockfd, (char*)&joinerrq, sizeof(joinerrq));

        // 2. 把【每个老成员】发给【新加入的人】
        m_pKernel->SendData(clientfd, (char*)&memrq, sizeof(memrq));
    }

    //把新用户加入房间列表
    ls.push_back(rq->m_UserId);
    m_pKernel->m_mapIdToRoomId.insert(rq->m_RoomId, ls);
}

void RoomLogic::LeaveRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)szbuf;

    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->RoomId)) return;

    std::list<int> ls;
    m_pKernel->m_mapIdToRoomId.find(rq->RoomId, ls);

    for(auto ite=ls.begin();ite!=ls.end();){
        int userId=*ite;
        if(userId == rq->m_nUserId){
            ite=ls.erase(ite);
        }else{
            UserInfo* info;
            if(m_pKernel->m_mapIdToUserInfo.find(userId,info)){
                m_pKernel->SendData(info->m_sockfd, szbuf, nlen);
            }
            ++ite;
        }
    }

    if(ls.size()==0){
        m_pKernel->m_mapIdToRoomId.erase(rq->RoomId);
    }else{
        m_pKernel->m_mapIdToRoomId.insert(rq->RoomId, ls);
    }
}
