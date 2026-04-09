#include "RoomLogic.h"
#include "CKernel.h"
#include"common/packdef.h"
#include <string.h>
#include <iostream>

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << __func__ << std::endl;

//初始化
RoomLogic::RoomLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

RoomLogic::~RoomLogic() {}

//创建房间请求
void RoomLogic::CreateRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_CREATEROOM_RQ* rq=(STRU_CREATEROOM_RQ*)szbuf;
    //获取八位随机数作为房间号
    int roomid=0;
    do{
        roomid = rand() % 90000000 + 10000000;
    }while(m_pKernel->m_mapIdToRoomId.IsExist(roomid));

    //以房间号为键，存储用户id的链表为值存入中介者的map中
    std::list<int> lst;
    lst.push_back(rq->m_userId);
    m_pKernel->m_mapIdToRoomId.insert(roomid, lst);
    //返回创建房间成功
    STRU_CREATEROOM_RS rs;
    rs.m_result=create_success;
    rs.m_RoomId=roomid;
    m_pKernel->SendData(clientfd,(char*)&rs,sizeof(rs));
}
void RoomLogic::JoinRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_

    // 1. 拆包
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

    // find 判断返回值，防止空指针（不会发生不存在就创建，因为MyMap写的是count）
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
        //获取存在的用户的个人信息
        if(!m_pKernel->m_mapIdToUserInfo.find(Memid, memInfo) || memInfo == nullptr){
            continue;
        }

        // 给 memrq 赋值
        memrq.m_userId = memInfo->m_id;
        strcpy(memrq.m_userName, memInfo->m_userName);

        // 1. 把 新加入的人 发给 房间内所有老成员
        m_pKernel->SendData(memInfo->m_sockfd, (char*)&joinerrq, sizeof(joinerrq));

        // 2. 把【每个老成员】发给【新加入的人】
        m_pKernel->SendData(clientfd, (char*)&memrq, sizeof(memrq));
    }

    //把新用户加入房间列表
    ls.push_back(rq->m_UserId);
    m_pKernel->m_mapIdToRoomId.insert(rq->m_RoomId, ls);
}

//离开房间请求
void RoomLogic::LeaveRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //拆包
    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)szbuf;
    //查看房间是否存在
    if(!m_pKernel->m_mapIdToRoomId.IsExist(rq->RoomId)) return;
    //获取房间成员链表
    std::list<int> ls;
    m_pKernel->m_mapIdToRoomId.find(rq->RoomId, ls);
    //循环遍历所有的成员
    for(auto ite=ls.begin();ite!=ls.end();){
        int userId=*ite;
        //如果用户id是要退出房间的id就清除
        if(userId == rq->m_nUserId){
            ite=ls.erase(ite);
        }else{
            //如果不是退出房间用户id，那么给每个人发送用户信息，由用户退出
            UserInfo* info;
            if(m_pKernel->m_mapIdToUserInfo.find(userId,info)){
                m_pKernel->SendData(info->m_sockfd, szbuf, nlen);
            }
            ++ite;
        }
    }
    //如果房间人数为0那么清除房间
    if(ls.size()==0){
        m_pKernel->m_mapIdToRoomId.erase(rq->RoomId);
    }else{
    //如果房间人数不为0那么就更新map
        m_pKernel->m_mapIdToRoomId.insert(rq->RoomId, ls);
    }
}
