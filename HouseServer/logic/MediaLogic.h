#ifndef MEDIALOGIC_H
#define MEDIALOGIC_H

class CKernel;
class CMysql;
typedef int sock_fd;  // 套接字文件描述符（KCP虚拟fd）


class MediaLogic
{
public:
    MediaLogic(CKernel* pKernel);
    ~MediaLogic();

    // 接收并转发音频数据帧
    void AudioFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    // 接收并转发视频数据帧
    void VideoFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    // 注册音频通道FD（绑定用户ID）
    void AudioRegister(sock_fd clientfd, char*szbuf, int nlen);
    // 注册视频通道FD（绑定用户ID）
    void VideoRegister(sock_fd clientfd, char*szbuf, int nlen);

private:
    CKernel* m_pKernel;  // 核心层指针（管理用户、房间、所有map）
    CMysql*  m_sql;      // 数据库句柄
};

#endif // MEDIALOGIC_H
