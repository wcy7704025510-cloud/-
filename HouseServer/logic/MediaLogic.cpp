#include "MediaLogic.h"
#include "CKernel.h"
#include "common/packdef.h"
#include <iostream>
#include <list>

using namespace std;

#define _DEF_COUT_FUNC_    std::cout << "clientfd:"<< clientfd << "  " << __func__ << std::endl;

MediaLogic::MediaLogic(CKernel* pKernel)
{
    m_pKernel = pKernel;
    m_sql = pKernel->m_sql;
}

MediaLogic::~MediaLogic()
{

}


//接收音频数据，并转发给房间内其他用户
void MediaLogic::AudioFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_;

    // 解析数据包头：Type(4) + UserID(4) + RoomID(4) + 音频数据
    char* tmp = szbuf;
    tmp += sizeof(int);                // 跳过消息类型Type(4字节)
    int userId = *(int*)tmp;           // 提取发送者的用户ID
    tmp += sizeof(int);                // 指针后移
    int roomId = *(int*)tmp;           // 提取房间ID

    //检查房间是否存在，不存在直接返回
    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId))
        return;

    //获取房间内所有用户ID列表
    list<int> lstUser;
    m_pKernel->m_mapIdToRoomId.find(roomId, lstUser);

    //遍历房间所有成员，转发音频（排除自己）
    int dstUserId;
    for(auto ite = lstUser.begin(); ite != lstUser.end(); ite++)
    {
        dstUserId = *ite;

        // 不发给自己
        if(userId == dstUserId)
            continue;

        // 查找目标用户信息
        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(dstUserId, userInfo))
        {
            // 通过 音频通道fd 转发原始数据
            m_pKernel->SendData(userInfo->m_audiofd, szbuf, nlen);
        }
    }
}


// 接收视频数据，并转发给房间内其他用户
void MediaLogic::VideoFrameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_;

    // 解析数据包头：Type(4) + UserID(4) + RoomID(4) + 音频数据
    char* tmp = szbuf;
    tmp += sizeof(int);                // 跳过消息类型Type(4字节)
    int userId = *(int*)tmp;           // 提取发送者的用户ID
    tmp += sizeof(int);                // 指针后移
    int roomId = *(int*)tmp;           // 提取房间ID

    // 房间不存在直接返回
    if(!m_pKernel->m_mapIdToRoomId.IsExist(roomId))
        return;

    // 获取房间用户列表
    list<int> lstUser;
    m_pKernel->m_mapIdToRoomId.find(roomId, lstUser);

    // 遍历转发视频数据
    int dstUserId;
    for(auto ite = lstUser.begin(); ite != lstUser.end(); ite++)
    {
        dstUserId = *ite;
        // 不发给自己
        if(userId == dstUserId)
            continue;

        UserInfo* userInfo;
        if(m_pKernel->m_mapIdToUserInfo.find(dstUserId, userInfo))
        {
            // 通过 视频通道fd 转发数据
            m_pKernel->SendData(userInfo->m_videofd, szbuf, nlen);
        }
    }
}


// 功能：音频通道注册，将KCP音频fd 与 用户ID 绑定

void MediaLogic::AudioRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_;

    //拆包
    STRU_AUDIO_REGISTER *rq = (STRU_AUDIO_REGISTER*)szbuf;
    int userId = rq->userid;

    // 用户存在，绑定音频fd
    if(m_pKernel->m_mapIdToUserInfo.IsExist(userId))
    {
        UserInfo* userInfo;
        m_pKernel->m_mapIdToUserInfo.find(userId, userInfo);
        userInfo->m_audiofd = clientfd;  // 绑定音频通道fd
        cout << "用户" << userId << " 音频通道注册成功" << endl;
    }
}

// 功能：视频通道注册，将KCP视频fd 与 用户ID 绑定
void MediaLogic::VideoRegister(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_;

    // 拆包
    STRU_VIDEO_REGISTER *rq = (STRU_VIDEO_REGISTER*)szbuf;
    int userId = rq->userid;

    // 用户存在，绑定视频fd
    if(m_pKernel->m_mapIdToUserInfo.IsExist(userId))
    {
        UserInfo* userInfo;
        m_pKernel->m_mapIdToUserInfo.find(userId, userInfo);
        userInfo->m_videofd = clientfd;  // 绑定视频通道fd
        cout << "用户" << userId << " 视频通道注册成功" << endl;
    }
}
