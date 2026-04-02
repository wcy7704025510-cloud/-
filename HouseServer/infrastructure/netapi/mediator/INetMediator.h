#ifndef INETMEDIATOR_H
#define INETMEDIATOR_H

class INet;
class CKernel; // 前置声明核心大管家

class INetMediator
{
public:
    INetMediator(CKernel* pKernel); // 构造时注入上层大管家
    virtual ~INetMediator();

    // 【核心灵魂：动态装配底层网络引擎】
    void SetNetEngine(INet* pNet);

    // 开启网络核心服务
    int Open(int port);
    // 关闭网络核心服务
    void Close();
    // 处理网络接收的数据 (底层向上报)
    void DealData(int sockfd, char* szbuf, int nlen);
    // 发送数据 (上层向下发)
    void SendData(int sockfd, char* szbuf, int nlen);
    // 启动事件循环
    void EventLoop();

protected:
    INet* m_pNet;       // 向下沟通的桥 (底层引擎)
    CKernel* m_pKernel; // 向上汇报的桥 (核心逻辑)
};

#endif // INETMEDIATOR_H
