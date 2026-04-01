#include "MediaLogic.h"
#include "CKernel.h"
#include "packdef.h"
#include <iostream>

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << __func__ << std::endl;

MediaLogic::MediaLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

MediaLogic::~MediaLogic() {}

void MediaLogic::AudioFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    char* tmp=szbuf;
    tmp+=sizeof(int);
    int userId=*(int*)tmp;
    tmp+=sizeof(int);
    int roomId=*(int*)tmp;

    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)) return;

    std::list<int> lst;
    m_pKernel->m_mapIdToRoomId.find(roomId,lst);

    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        if(userId==ID) continue;

        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            m_pKernel->SendData(userInfo->m_audiofd,szbuf,nlen);
        }
    }
}

void MediaLogic::VideoFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    char* tmp=szbuf;
    tmp+=sizeof(int);
    int userId=*(int*)tmp;
    tmp+=sizeof(int);
    int roomId=*(int*)tmp;

    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId)) return;

    std::list<int> lst;
    m_pKernel->m_mapIdToRoomId.find(roomId,lst);

    int ID;
    for(auto ite=lst.begin();ite!=lst.end();ite++){
        ID=*ite;
        if(userId==ID) continue;

        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(ID,userInfo)){
            m_pKernel->SendData(userInfo->m_videofd,szbuf,nlen);
        }
    }
}

void MediaLogic::AudioRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_AUDIO_REGISTER *rq=(STRU_AUDIO_REGISTER*)szbuf;
    int id=rq->userid;

    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_audiofd=clientfd;
    }
}

void MediaLogic::VideoRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_VIDEO_REGISTER *rq=(STRU_VIDEO_REGISTER*)szbuf;
    int id=rq->userid;

    if(m_pKernel->m_mapIdToUserInfo.IsExist(id)){
        UserInfo* info;
        m_pKernel->m_mapIdToUserInfo.find(id,info);
        info->m_videofd=clientfd;
    }
}