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
    STRU_JOINROOM_RQ* rq=(STRU_JOINROOM_RQ*)szbuf;
    STRU_JOINROOM_RS rs;
    rs.m_roomId=rq->m_RoomId;

    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->m_RoomId)){
        rs.m_lResult=room_isnot_exist;
        m_pKernel->SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }

    rs.m_lResult=join_success;
    m_pKernel->SendData(clientfd,(char*)&rs,sizeof(rs));

    UserInfo* joiner;
    m_pKernel->m_mapIdToUserInfo.find(rq->m_UserId, joiner);
    STRU_ROOM_MEMBER_RQ joinerrq;
    joinerrq.m_userId=rq->m_UserId;
    strcpy(joinerrq.m_userName, joiner->m_userName);

    std::list<int> ls;
    m_pKernel->m_mapIdToRoomId.find(rq->m_RoomId, ls);

    m_pKernel->SendData(clientfd,(char*)&joinerrq,sizeof(joinerrq));

    int Memid=0;
    UserInfo* memInfo;
    STRU_ROOM_MEMBER_RQ memrq;
    for(auto ite=ls.begin();ite!=ls.end();ite++){
        Memid=*ite;
        m_pKernel->m_mapIdToUserInfo.find(Memid,memInfo);

        m_pKernel->SendData(memInfo->m_sockfd,(char*)&joinerrq,sizeof(joinerrq));
        m_pKernel->SendData(clientfd,(char*)&memrq,sizeof(memrq));
    }

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