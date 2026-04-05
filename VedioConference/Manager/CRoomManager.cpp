#include "CRoomManager.h"
#include "roomdialog.h"
#include "videoconferencedialog.h"
#include "packdef.h"
#include "usershow.h"
#include "audio_write.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QRegExp>
#include <QDebug>

CRoomManager::CRoomManager(QObject *parent) : QObject(parent), m_pRoomDialog(nullptr), m_pVedio(nullptr), m_id(0), m_roomId(0)
{
}

CRoomManager::~CRoomManager()
{
}

void CRoomManager::slot_createRoom()
{
    qDebug()<<__func__;
    if(m_roomId != 0){
        if(m_pVedio) QMessageBox::about(m_pVedio,"提示","在房间内，无法创建新房间");
        return;
    }

    STRU_CREATEROOM_RQ rq;
    rq.m_userId = m_id;
    emit SIG_SendData((char*)&rq,sizeof(rq));
}

void CRoomManager::slot_joinRoom()
{
    qDebug()<<__func__;
    if(m_roomId != 0){
        if(m_pVedio) QMessageBox::about(m_pVedio,"提示","在房间内，无法加入新房间");
        return;
    }
    if (!m_pVedio) return;

    QString strRoomId = QInputDialog::getText(m_pVedio,"加入房间","请输入房间号");
    QRegExp exp("^[0-9]{1,8}$");
    if(!exp.exactMatch(strRoomId)){
        QMessageBox::about(m_pVedio,"提示","房间号不合法，房间号为8位以下纯数字");
        return;
    }
    STRU_JOINROOM_RQ rq;
    rq.m_RoomId = strRoomId.toInt();
    rq.m_UserId = m_id;
    emit SIG_SendData((char*)&rq,sizeof(rq));
}

void CRoomManager::slot_QuitRoom()
{
    qDebug()<<__func__;

    // 1. 发送离开房间的数据包给服务器
    STRU_LEAVEROOM_RQ rq;
    rq.RoomId = m_roomId;
    rq.m_nUserId = m_id;
    strcpy(rq.UserName, m_name.toLocal8Bit().data());
    emit SIG_SendData((char*)&rq,sizeof(rq));

    // 2. 清理自己的音频播放对象
    for(auto ite = m_mapIdToAudioWrite.begin(); ite != m_mapIdToAudioWrite.end();){
        Audio_Write* aw = ite->second;
        ite = m_mapIdToAudioWrite.erase(ite);
        delete aw;
    }

    m_roomId = 0;

    // 3. 界面切换逻辑
    if(m_pRoomDialog) {
        m_pRoomDialog->slot_clearUserShow(); // 清理房间内所有小窗口
        m_pRoomDialog->hide();               // 隐藏房间界面
    }

    if(m_pVedio) {
        m_pVedio->showNormal();              // 重新显示主控制大厅界面
    }

    // 4. 通知其他模块（如 MediaManager）清理外设资源
    emit SIG_RoomQuitted();
}
void CRoomManager::slot_dealCreateRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    STRU_CREATEROOM_RS* rs=(STRU_CREATEROOM_RS*)buf;
    if (!m_pRoomDialog) return;

    m_pRoomDialog->slot_setInfo(QString::number(rs->m_RoomId));

    UserShow* user = new UserShow;
    connect(user, SIGNAL(SIG_itemClicked(int,QString)), m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));
    user->slot_setInfo(m_id, m_name);


    m_pRoomDialog->slot_addUserInfo(user);

    m_roomId = rs->m_RoomId;
    m_pRoomDialog->showNormal();
    m_pRoomDialog->slot_setAudioSet(false);
    m_pRoomDialog->slot_setvideoSet(false);
}

void CRoomManager::slot_dealJoinRoomRs(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;
    STRU_JOINROOM_RS* rs=(STRU_JOINROOM_RS*)buf;
    if (!m_pRoomDialog || !m_pVedio) return;

    if(rs->m_lResult == room_isnot_exist){
        QString res = QString("加入房间失败，房间%1不存在").arg(rs->m_roomId);
        QMessageBox::about(m_pVedio,"提示",res);
        return;
    } else if(rs->m_lResult == join_success){
        m_pRoomDialog->slot_setInfo(QString::number(rs->m_roomId));
        m_roomId = rs->m_roomId;
        m_pRoomDialog->showNormal();
    }
    m_pRoomDialog->slot_setAudioSet(false);
    m_pRoomDialog->slot_setvideoSet(false);
}

void CRoomManager::slot_dealUserInfoRq(uint sock, char *buf, int nLen)
{
    qDebug()<<__func__;

    STRU_ROOM_MEMBER_RQ* rq=(STRU_ROOM_MEMBER_RQ*)buf;
    if (!m_pRoomDialog) return;

    QString name=rq->m_userName;
    name.toLocal8Bit().data();
    UserShow* user = new UserShow;
    connect(user, SIGNAL(SIG_itemClicked(int,QString)), m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));
    user->slot_setInfo(rq->m_userId, name);
    m_pRoomDialog->slot_addUserInfo(user);

    if(m_mapIdToAudioWrite.count(rq->m_userId) == 0){
        Audio_Write* aw = new Audio_Write;
        m_mapIdToAudioWrite[rq->m_userId] = aw;
        aw->start();
    }
}

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
}
