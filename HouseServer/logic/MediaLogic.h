#ifndef MEDIALOGIC_H
#define MEDIALOGIC_H

class CKernel;
class CMysql;
typedef int sock_fd;

class MediaLogic
{
public:
    MediaLogic(CKernel* pKernel);
    ~MediaLogic();

    void AudioFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    void VideoFrameRq(sock_fd clientfd, char*szbuf, int nlen);
    void AudioRegister(sock_fd clientfd, char*szbuf, int nlen);
    void VideoRegister(sock_fd clientfd, char*szbuf, int nlen);

private:
    CKernel* m_pKernel;
    CMysql* m_sql;
};

#endif // MEDIALOGIC_H