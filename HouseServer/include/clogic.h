#ifndef CLOGIC_H
#define CLOGIC_H

#include"TCPKernel.h"

class CLogic
{
public:
    CLogic( TcpKernel* pkernel )
    {
        m_pKernel = pkernel;
        m_sql = pkernel->m_sql;
        m_tcp = pkernel->m_tcp;
    }
public:
    //设置协议映射
    void setNetPackMap();
    /************** 发送数据*********************/
    void SendData( sock_fd clientfd, char*szbuf, int nlen )
    {
        m_pKernel->SendData( clientfd ,szbuf , nlen );
    }
    /************** 网络处理 *********************/
    //注册
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);
    //创建会议
    void CreateRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //加入会议
    void JoinRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //离开会议
    void LeaveRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理音频帧
    void AudioFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理视频帧
    void VideoFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    //音频注册
    void AudioRegister(sock_fd clientfd, char*szbuf, int nlen);
    //视频注册
    void VideoRegister(sock_fd clientfd, char*szbuf, int nlen);
    /*******************************************/

private:
    TcpKernel* m_pKernel;
    CMysql * m_sql;
    Block_Epoll_Net * m_tcp;
};

#endif // CLOGIC_H
