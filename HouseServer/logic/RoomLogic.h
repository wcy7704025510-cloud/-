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

    void CreateRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    void JoinRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    void LeaveRoomRq(sock_fd clientfd, char*szbuf, int nlen);

private:
    CKernel* m_pKernel;
    CMysql* m_sql;
};

#endif // ROOMLOGIC_H