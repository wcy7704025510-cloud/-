#ifndef CROOMMANAGER_H
#define CROOMMANAGER_H

#include <QObject>
#include <QString>
#include <map>

// 前向声明
class RoomDialog;
class VedioConferenceDialog;
class Audio_Write;

class CRoomManager : public QObject
{
    Q_OBJECT
public:
    explicit CRoomManager(QObject *parent = nullptr);
    ~CRoomManager();

    // 依赖注入
    void setRoomDialog(RoomDialog* roomDlg) { m_pRoomDialog = roomDlg; }
    void setVedioDialog(VedioConferenceDialog* vedioDlg) { m_pVedio = vedioDlg; }
    void setUserId(int id, QString name) { m_id = id; m_name = name; }
    
    int getRoomId() const { return m_roomId; }
    std::map<int, Audio_Write*>& getAudioWriteMap() { return m_mapIdToAudioWrite; }

signals:
    // 通知外部(例如MediaManager)进行清理
    void SIG_RoomQuitted();
    // 通知 CKernel 发送网络数据
    void SIG_SendData(char* buf, int nLen);

public slots:
    // 处理UI信号
    void slot_createRoom();
    void slot_joinRoom();
    void slot_QuitRoom();

    // 处理网络返回
    void slot_dealCreateRoomRs(uint sock, char* buf, int nLen);
    void slot_dealJoinRoomRs(uint sock, char* buf, int nLen);
    void slot_dealUserInfoRq(uint sock, char* buf, int nLen);
    void slot_dealQuitRoomRq(uint sock, char* buf, int nLen);

private:
    RoomDialog* m_pRoomDialog;
    VedioConferenceDialog* m_pVedio;

    int m_id;
    int m_roomId;
    QString m_name;

    // 此处仅维护成员音频对象的生命周期和映射，具体的数据接收交给MediaManager
    std::map<int, Audio_Write*> m_mapIdToAudioWrite;
};

#endif // CROOMMANAGER_H