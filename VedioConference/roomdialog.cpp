#include "roomdialog.h"
#include "ui_roomdialog.h"

RoomDialog::RoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{
    ui->setupUi(this);
    m_mainLayout=new QVBoxLayout;
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(5);

    //设置一个竖直布局的画布，可以向这个里面添加控件
    ui->wdg_list->setLayout(m_mainLayout);
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

//显示房间号
void RoomDialog::slot_setInfo(QString roomid)
{
    QString title=QString("房间号:%1").arg(roomid);
    setWindowTitle(title);
    ui->lb_title->setText(title);
}

//添加一个房间成员
void RoomDialog::slot_addUserInfo(UserShow *user)
{
    //将用户信息控件添加到画布上
    m_mainLayout->addWidget(user);
    //记录pair<用户ID,对应小窗口界面>
    m_mapIdToUserShow[user->m_id]=user;
}

//删除一个指定的房间用户小界面
void RoomDialog::slot_removeUserShow(UserShow *user)
{
    //隐藏对应用户小界面
   user->hide();
    //从画布中移除对应控件
   m_mainLayout->removeWidget(user);
   //用户小窗口控件在画布对象上，由画布负责回收，不需要手动回收用户小窗口的额外空间
}

//通过用户ID删除指定的用户小界面
void RoomDialog::slot_removeUserShow(int id)
{
    if(m_mapIdToUserShow.count(id)>0){
        UserShow* user=m_mapIdToUserShow[id];
        this->slot_removeUserShow(user);
    }

}

//清空房间
void RoomDialog::slot_clearUserShow()
{
    for(auto ite=m_mapIdToUserShow.begin();ite!=m_mapIdToUserShow.end();ite++){
        slot_removeUserShow(ite->second);
    }
}

//设置音频状态
void RoomDialog::slot_setAudioSet(bool check)
{
    ui->cb_audio->setChecked(check);
}

//设置视频状态
void RoomDialog::slot_setvideoSet(bool check)
{
    ui->cb_vedio->setChecked(check);
}

//设置屏幕共享勾选框
void RoomDialog::slot_setScreenSet(bool check)
{
    ui->cb_desk->setChecked(check);
}

//刷新指定用户的小窗口控件
void RoomDialog::slot_refreshUser(int id,QImage& img)
{
    //预览图的id 和要刷新的图片id 一致，刷新预览图
    if(ui->wdg_userShow->m_id==id){
        ui->wdg_userShow->slot_setImage(img);
    }
    if(m_mapIdToUserShow.count(id)>0){
        UserShow* user=m_mapIdToUserShow[id];
        user->slot_setImage(img);
    }
}

//设置预览图
void RoomDialog::slot_setBigImgId(int id,QString name)
{
    ui->wdg_userShow->slot_setInfo(id,name);
}

//房间界面退出事件
void RoomDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"退出提示","是否退出会议?")==
            QMessageBox::Yes){
        //发送信息通知核心类处理
        Q_EMIT SIG_close();
        //接收关闭事件
        event->accept();
        return;
    }
    //忽略关闭事件
    event->ignore();
}

//右上角退出
void RoomDialog::on_pb_close_clicked()
{
    this->close();  //页面关闭，执行closeEvent()函数处理
}

//右下角退出
void RoomDialog::on_pb_exit_clicked()
{
    this->close();  //页面关闭，执行closeEvent()函数处理
}

//开启或关闭 音频
void RoomDialog::on_cb_audio_clicked()
{
    if(ui->cb_audio->isChecked()){  //打开音频
        ui->cb_audio->setChecked(true);
        Q_EMIT SIG_AudioStart();
    }else{                          //关闭音频
        ui->cb_audio->setChecked(false);
        Q_EMIT SIG_AudioPause();
    }
}

//开启或关闭 视频
void RoomDialog::on_cb_vedio_clicked()
{
    if(ui->cb_vedio->isChecked()){  //打开视频
        ui->cb_vedio->setChecked(true);
        ui->cb_desk->setChecked(false);
        Q_EMIT SIG_VideoStart();
        Q_EMIT SIG_ScreenPause();
    }else{                          //关闭视频
        ui->cb_vedio->setChecked(false);
        Q_EMIT SIG_VideoPause();
    }
}

//开启或关闭 桌面共享
void RoomDialog::on_cb_desk_clicked()
{
    if(ui->cb_desk->isChecked()){  //开启桌面共享
        ui->cb_desk->setChecked(true);
        ui->cb_vedio->setChecked(false);
        Q_EMIT SIG_ScreenStart();
        Q_EMIT SIG_VideoPause();
    }else{                          //关闭桌面共享
        ui->cb_desk->setChecked(false);
        Q_EMIT SIG_ScreenPause();
    }
}

//萌拍处理
void RoomDialog::on_cb_moji_currentIndexChanged(int index)
{
    Q_EMIT SIG_setMoji(index);
}

