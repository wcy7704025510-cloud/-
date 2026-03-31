#include "./kernel/CKernel.h"
#include "./Mediator/TcpMediator.h"
#include "./kernel/clogic.h"
#include "packdef.h"
#include <stdio.h>
#include <sys/time.h>

CKernel::CKernel()
{
    m_sql = new CMysql;
    m_pMediator = new TcpMediator(this); // 中介者需要认识 Kernel
    m_logic = new CLogic(this);          // 逻辑层认识 Kernel

    if(m_pMediator) {
        m_pMediator->EventLoop();
    }
}

CKernel::~CKernel()
{
    if(m_logic) {
        delete m_logic;
        m_logic = nullptr;
    }
    if(m_pMediator) {
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_sql) {
        delete m_sql;
        m_sql = nullptr;
    }
}

CKernel* CKernel::GetInstance()
{
    static CKernel kernel;
    return &kernel;
}

int CKernel::StartServer(int port)
{
    initRand();

    // 1. 初始化数据库
    if (!m_sql->ConnectMysql(_DEF_DB_IP, _DEF_DB_USER, _DEF_DB_PWD, _DEF_DB_NAME))
    {
        printf("Connect Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }

    // 2. 初始化逻辑层协议映射
    m_logic->setNetPackMap();

    // 3. 开启网络中介者
    if (!m_pMediator->Open(port)) {
        printf("Net Open Failed...\n");
        return FALSE;
    }

    return TRUE;
}

void CKernel::CloseServer()
{
    if (m_sql) {
        m_sql->DisConnect();
    }
    if (m_pMediator) {
        m_pMediator->Close();
    }
}

void CKernel::SendData(int sockfd, char *szbuf, int nlen)
{
    if (m_pMediator) {
        m_pMediator->SendData(sockfd, szbuf, nlen);
    }
}

void CKernel::DealData(int sockfd, char *szbuf, int nlen)
{
    // 将数据抛给逻辑层处理
    m_logic->DealData(sockfd, szbuf, nlen);
}

void CKernel::initRand()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_sec + time.tv_usec);
}
