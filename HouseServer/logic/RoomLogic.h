#ifndef ROOMLOGIC_H
#define ROOMLOGIC_H

class CKernel;
class CMysql;
typedef int sock_fd;

class RoomLogic
{
public:
    RoomLogic(CKernel* pKernel);
    ~RoomLogic();
    //创建房间请求包
    void CreateRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //加入房间请求包
    void JoinRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //离开房间请求包
    void LeaveRoomRq(sock_fd clientfd, char*szbuf, int nlen);

private:
    CKernel* m_pKernel;
    CMysql* m_sql;
};

#endif // ROOMLOGIC_H
