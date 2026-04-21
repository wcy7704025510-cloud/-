#include "CRoomManager.h"          // 房间管理器头文件
#include "roomdialog.h"           // 房间对话框头文件
#include "videoconferencedialog.h" // 视频会议主界面头文件
#include "packdef.h"              // 协议包定义
#include "usershow.h"             // 用户视频显示控件头文件
#include "audio_write.h"         // 音频播放头文件
#include <QMessageBox>            // Qt消息对话框
#include <QInputDialog>           // Qt输入对话框
#include <QRegExp>               // Qt正则表达式(用于验证房间号格式)
#include <QDebug>                 // Qt调试输出

// CRoomManager构造函数
// 初始化所有成员变量为初始状态
// m_pRoomDialog=nullptr: 房间对话框指针，setter注入后有效
// m_pVedio=nullptr: 视频会议主界面指针，setter注入后有效
// m_id=0: 用户ID，setter注入后有效
// m_roomId=0: 房间ID，0表示当前不在任何房间
CRoomManager::CRoomManager(QObject *parent)
    : QObject(parent)
    , m_pRoomDialog(nullptr)
    , m_pVedio(nullptr)
    , m_id(0)
    , m_roomId(0)
{
}

// slot_createRoom - 创建房间请求
// 触发: VedioConferenceDialog点击"创建房间"按钮
// 流程: 检查当前状态 -> 封装请求包 -> 发送到服务器
void CRoomManager::slot_createRoom()
{
    qDebug()<<__func__;           // 调试日志

    // 检查是否已在房间中(多人会议通常一次只能在一个房间)
    if(m_roomId != 0){
        if(m_pVedio) QMessageBox::about(m_pVedio,"提示","在房间内，无法创建新房间");
        return;
    }

    // 封装创建房间请求包
    STRU_CREATEROOM_RQ rq;
    rq.m_userId = m_id;           // 携带创建者用户ID

    // 发送到服务器
    emit SIG_SendData((char*)&rq,sizeof(rq));
}

// slot_joinRoom - 加入房间请求
// 触发: VedioConferenceDialog点击"加入房间"按钮
// 流程: 弹出输入框 -> 验证房间号 -> 封装请求包 -> 发送服务器
void CRoomManager::slot_joinRoom()
{
    qDebug()<<__func__;           // 调试日志

    // 检查是否已在房间
    if(m_roomId != 0){
        if(m_pVedio) QMessageBox::about(m_pVedio,"提示","在房间内，无法加入新房间");
        return;
    }
    if (!m_pVedio) return;       // 防御性检查

    // 弹出输入对话框获取房间号
    // 参数: 父窗口, 标题, 提示文本
    QString strRoomId = QInputDialog::getText(m_pVedio,"加入房间","请输入房间号");

    // 正则验证: 1-8位纯数字
    // ^[0-9]{1,8}$: 开头到结尾必须是数字，长度1-8位
    QRegExp exp("^[0-9]{1,8}$");
    if(!exp.exactMatch(strRoomId)){
        QMessageBox::about(m_pVedio,"提示","房间号不合法，房间号为8位以下纯数字");
        return;
    }

    // 封装加入房间请求包
    STRU_JOINROOM_RQ rq;
    rq.m_RoomId = strRoomId.toInt();  // 字符串转整型
    rq.m_UserId = m_id;               // 携带加入者用户ID

    // 发送到服务器
    emit SIG_SendData((char*)&rq,sizeof(rq));
}

// slot_QuitRoom - 退出房间
// 触发: 点击退出按钮或收到服务器踢人通知
// 流程: 发送离开通知 -> 清理本地资源 -> 切换界面 -> 通知其他模块
void CRoomManager::slot_QuitRoom()
{
    qDebug()<<__func__;           // 调试日志

    // 1. 发送离开房间的数据包给服务器
    // 通知服务器和其他成员当前用户离开
    STRU_LEAVEROOM_RQ rq;
    rq.RoomId = m_roomId;                    // 当前房间ID
    rq.m_nUserId = m_id;                      // 离开的用户ID
    strcpy(rq.UserName, m_name.toLocal8Bit().data()); // 用户昵称(GBK编码)
    emit SIG_SendData((char*)&rq,sizeof(rq));

    // 2. 清理自己的音频播放对象
    // 遍历map容器，销毁所有远程用户的Audio_Write对象
    for(auto ite = m_mapIdToAudioWrite.begin(); ite != m_mapIdToAudioWrite.end();){
        Audio_Write* aw = ite->second;       // 获取音频播放对象指针
        ite = m_mapIdToAudioWrite.erase(ite); // 先删除迭代器指向的元素，返回下一个迭代器
        delete aw;                            // 释放内存
    }

    m_roomId = 0;                             // 重置房间ID，表示已离开

    // 3. 界面切换逻辑
    if(m_pRoomDialog) {
        m_pRoomDialog->slot_clearUserShow(); // 清理房间内所有小窗口(显示其他用户视频的控件)
        m_pRoomDialog->hide();              // 隐藏房间界面
    }

    if(m_pVedio) {
        m_pVedio->showNormal();             // 重新显示主控制大厅界面
    }

    // 4. 通知其他模块(如MediaManager)清理外设资源
    // 音视频设备可能需要释放或停止采集
    emit SIG_RoomQuitted();
}

// slot_dealCreateRoomRs - 处理创建房间响应
// 解析: 服务器返回的房间号和创建结果
void CRoomManager::slot_dealCreateRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;           // 调试日志

    // 解析创建房间响应包
    STRU_CREATEROOM_RS* rs=(STRU_CREATEROOM_RS*)buf;
    if (!m_pRoomDialog) return;  // 防御性检查

    // 显示房间号在界面上
    m_pRoomDialog->slot_setInfo(QString::number(rs->m_RoomId));

    // 创建自己的用户视频显示控件(创建者自己也要显示在列表中)
    UserShow* user = new UserShow;
    // 连接信号: 点击用户头像 -> 设置为主画面显示
    connect(user, SIGNAL(SIG_itemClicked(int,QString)), m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));
    user->slot_setInfo(m_id, m_name); // 初始化用户信息

    // 添加到房间成员列表
    m_pRoomDialog->slot_addUserInfo(user);

    // 更新当前房间ID
    m_roomId = rs->m_RoomId;

    // 显示房间界面
    m_pRoomDialog->showNormal();

    // 初始化音视频设置状态(默认关闭，待用户开启)
    m_pRoomDialog->slot_setAudioSet(false);  // 音频默认关闭
    m_pRoomDialog->slot_setvideoSet(false);  // 视频默认关闭

    // 通知媒体管理器启动音视频引擎
    emit SIG_RoomJoined();
}

// slot_dealJoinRoomRs - 处理加入房间响应
// 解析: 服务器返回的加入结果和房间信息
void CRoomManager::slot_dealJoinRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;           // 调试日志

    STRU_JOINROOM_RS* rs=(STRU_JOINROOM_RS*)buf;
    if (!m_pRoomDialog || !m_pVedio) return; // 防御性检查

    // 根据结果码处理
    if(rs->m_lResult == room_isnot_exist){
        // 房间不存在: 弹出错误提示
        QString res = QString("加入房间失败，房间%1不存在").arg(rs->m_roomId);
        QMessageBox::about(m_pVedio,"提示",res);
        return;
    } else if(rs->m_lResult == join_success){
        // 加入成功: 显示房间信息，切换到房间界面
        m_pRoomDialog->slot_setInfo(QString::number(rs->m_roomId));
        m_roomId = rs->m_roomId;
        m_pRoomDialog->showNormal();

        // 通知媒体管理器启动音视频引擎
        emit SIG_RoomJoined();
    }

    // 初始化音视频设置状态
    m_pRoomDialog->slot_setAudioSet(false);  // 音频默认关闭
    m_pRoomDialog->slot_setvideoSet(false);  // 视频默认关闭
}

// slot_dealUserInfoRq - 处理用户加入通知(有人加入房间)
// 服务器主动推送给房间内所有成员
void CRoomManager::slot_dealUserInfoRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;           // 调试日志

    // 解析新加入用户信息包
    STRU_ROOM_MEMBER_RQ* rq=(STRU_ROOM_MEMBER_RQ*)buf;
    if (!m_pRoomDialog) return;   // 防御性检查

    QString name=rq->m_userName;
    name.toLocal8Bit().data();    // 转换为本地编码(GBK)

    // 创建新用户的视频显示控件
    UserShow* user = new UserShow;
    // 连接点击信号
    connect(user, SIGNAL(SIG_itemClicked(int,QString)), m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));
    user->slot_setInfo(rq->m_userId, name); // 设置用户ID和昵称

    // 添加到房间成员列表显示
    m_pRoomDialog->slot_addUserInfo(user);

    // 为新用户创建音频播放对象(用于播放该用户的音频)
    if(m_mapIdToAudioWrite.count(rq->m_userId) == 0){
        Audio_Write* aw = new Audio_Write;  // 创建音频播放对象
        m_mapIdToAudioWrite[rq->m_userId] = aw; // 存入map: 用户ID -> 播放对象
        aw->start();                         // 启动播放线程
    }
}

// slot_dealQuitRoomRq - 处理用户离开通知(有人退出房间)
void CRoomManager::slot_dealQuitRoomRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;

    STRU_LEAVEROOM_RQ* rq=(STRU_LEAVEROOM_RQ*)buf;
    if (!m_pRoomDialog) return;

    if(rq->RoomId == m_roomId){
        m_pRoomDialog->slot_removeUserShow(rq->m_nUserId);
    }

    if(m_mapIdToAudioWrite.count(rq->m_nUserId) > 0){
        Audio_Write* pAw = m_mapIdToAudioWrite[rq->m_nUserId];
        m_mapIdToAudioWrite.erase(rq->m_nUserId);
        delete pAw;
    }
    emit SIG_UserLeft(rq->m_nUserId);
}
