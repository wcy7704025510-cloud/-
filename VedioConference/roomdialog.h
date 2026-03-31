#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
#include <QImage>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>

#include "./usershow.h"

namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT
signals:
    //房间关闭信号
    void SIG_close();
    //关闭音频信号
    void SIG_AudioPause();
    //打开音频信号
    void SIG_AudioStart();
    //打开视频信号
    void SIG_VideoStart();
    //关闭视频信号
    void SIG_VideoPause();
    //打开桌面共享信号
    void SIG_ScreenStart();
    //关闭桌面共享信号
    void SIG_ScreenPause();
    //设置萌拍表情类型的信号
    void SIG_setMoji(int moji);
public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();
public slots:
    //显示房间号
    void slot_setInfo(QString roomid);
    //添加房间成员
    void slot_addUserInfo(UserShow* user);
    //删除房间成员1，参数：用户小界面控件
    void slot_removeUserShow(UserShow* user);
    //删除房间成员2，参数：对应的用户id
    void slot_removeUserShow(int id);
    //清空房间成员
    void slot_clearUserShow();
    //设置音频勾选框
    void slot_setAudioSet(bool check);
    //设置视频勾选框
    void slot_setvideoSet(bool check);
    //设置屏幕共享勾选框
    void slot_setScreenSet(bool check);
    //刷新指定用户的小窗口控件
    void slot_refreshUser(int id,QImage& img);
    //设置预览图
    void slot_setBigImgId(int id,QString name);
private slots:
    //会议界面退出事件
    void closeEvent(QCloseEvent* event);
    void on_pb_close_clicked();

    void on_pb_exit_clicked();

    void on_cb_audio_clicked();

    void on_cb_vedio_clicked();

    void on_cb_desk_clicked();

    void on_cb_moji_currentIndexChanged(int index);

private:
    Ui::RoomDialog *ui;
    QVBoxLayout* m_mainLayout;
    std::map<int,UserShow*> m_mapIdToUserShow;                  //用户ID-->对应的会议界面小窗口
};

#endif // ROOMDIALOG_H
