#include "UdpNet.h"
#include "INetMediator.h"
#include <QDebug>

UdpNet::UdpNet( INetMediator * pMediator ):m_sock( INVALID_SOCKET ),m_isStop(false), m_kcp(nullptr)
{
    m_pMediator = pMediator;
    m_hThreadHandle = NULL;
}

UdpNet::~UdpNet()  //使用时, 父类指针指向子类, 使用虚析构
{
    UdpNet::UnInitNet();
}

// KCP 帮你把大包切碎后，会调用这里，把碎片通过你原来的 UDP 端口发出去
int UdpNet::udpOutput(const char *buf, int len, ikcpcb *kcp, void *user)
{
    UdpNet* pThis = (UdpNet*)user;
    if (pThis && pThis->m_sock != INVALID_SOCKET) {
        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(pThis->m_port);
        addr.sin_addr.S_un.S_addr = pThis->m_serverIp;
        sendto(pThis->m_sock, buf, len, 0, (sockaddr*)&addr, sizeof(addr));
    }
    return 0;
}

// 初始化网络
bool UdpNet::InitNet(const char* szBufIP , unsigned short port)
{
    if( !m_isLoadlib )
    {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        int err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) return false;
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            WSACleanup();
            return false;
        }
        m_isLoadlib = true;
    }

    // 1. 创建 UDP 套接字
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET) return false;

    //2. 将 UDP 套接字设置为【非阻塞模式】(极度重要，防止 recvfrom 堵死线程)
    /*
    如果不设置非阻塞：recvfrom 会阻塞等待，没有数据就一直卡死在那里，线程动不了，KCP 也无法工作。
        Linux 设置非阻塞用 fcntl，
        Windows 用 ioctlsocket。
        setsockopt 是管缓冲区、超时、心跳的
    */

    //设置非阻塞
    u_long mode = 1;
    ioctlsocket(m_sock, FIONBIO, &mode);

    m_serverIp = inet_addr(szBufIP);
    m_port = port;

    //3. 调用 ikcp.c 的函数初始化 KCP (0x11223344 是房间暗号，必须和服务器一致)
    m_kcp = ikcp_create(0x11223344, this);
    m_kcp->output = udpOutput; // 绑定发送函数

    ikcp_nodelay(m_kcp, 1, 10, 2, 1); // 开启极速模式
    ikcp_wndsize(m_kcp, 512, 512);    // 放大窗口
    ikcp_setmtu(m_kcp, 1380);

    // 4. 开启线程
    m_isStop = false;
    m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    return true;
}

unsigned int __stdcall UdpNet::RecvThread( void * lpvoid)
{
    UdpNet* pthis = (UdpNet*) lpvoid;
    pthis->RecvData();
    return 0;
}

//关闭网络
void UdpNet::UnInitNet()
{
    m_isStop = true ;
    if( m_hThreadHandle )
    {
        if( WaitForSingleObject(m_hThreadHandle , 100 ) == WAIT_TIMEOUT )
            TerminateThread( m_hThreadHandle , -1 );
        CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
    //释放 KCP 内存
    if(m_kcp){
        ikcp_release(m_kcp);
        m_kcp = nullptr;
    }
    if( m_sock && m_sock != INVALID_SOCKET ) closesocket(m_sock);
}

//发送：不再直接 sendto，而是交给 ikcp_send 去切片处理
bool UdpNet::SendData( unsigned int lSendIP , char* buf , int nlen )
{
    if (m_kcp) {
        qDebug()<<nlen;
        ikcp_send(m_kcp, buf, nlen);
        return true;
    }
    return false;
}

//接收：处理 KCP 的拼接逻辑
void UdpNet::RecvData()
{
    char recvBuf[4096];
    SOCKADDR_IN addrFrom;
    int addrLen = sizeof(addrFrom);

    while (!m_isStop) {
        // 1. 驱动 KCP 心跳 (必须要有)
        ikcp_update(m_kcp, GetTickCount());

        // 2. 疯狂读取底层的所有 UDP 碎片包
        while (true) {
            int nRes = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &addrLen);
            if (nRes > 0) {
                // 把收到的碎片喂给 KCP
                ikcp_input(m_kcp, recvBuf, nRes);
            } else {
                break; // 读完了跳出
            }
        }

        // 3. 向 KCP 索要拼装完整的业务包
        while (true) {
            int len = ikcp_peeksize(m_kcp);
            if (len <= 0) break; // 还没拼完

            char* packBuf = new char[len];
            int recvLen = ikcp_recv(m_kcp, packBuf, len);
            if (recvLen > 0) {
                // 拼装出了完美的包，扔给中介者解析！
                m_pMediator->DealData(m_serverIp, packBuf, recvLen);
            } else {
                delete[] packBuf;
            }
        }

        // 4. 休息 10 毫秒
        Sleep(10);
    }
}
