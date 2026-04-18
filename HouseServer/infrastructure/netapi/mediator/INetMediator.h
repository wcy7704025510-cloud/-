#ifndef INETMEDIATOR_H
#define INETMEDIATOR_H
#include<functional>
class INet;

class INetMediator
{
public:
    INetMediator(std::function<void(int,char*,int)>); // 构造时注入上层大管家
    virtual ~INetMediator();

    //装载底层网络模型
    void SetNetEngine(INet* pNet);

    // 开启网络核心服务
    int Open(int port);
    // 关闭网络核心服务
    void Close();
    // 处理网络接收的数据 (底层向上报)
    void DealData(int sockfd, char* szbuf, int nlen);
    // 发送数据 (上层向下发)
    void SendData(int sockfd, char* szbuf, int nlen);
    // 启动epoll监听事件循环
    void EventLoop();

protected:
    INet* m_pNet;       // 向下沟通的桥 (底层引擎)
    std::function<void(int,char*,int)>readyData;       //向kernel上传数据
};

#endif // INETMEDIATOR_H
