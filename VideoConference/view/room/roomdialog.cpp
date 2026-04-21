#include "roomdialog.h"
#include "ui_roomdialog.h"
#include "usershow.h"
#include <QDebug>
#include <QMessageBox>

RoomDialog::RoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{
    ui->setupUi(this);

    //动态创建一个垂直布局管理器，用于存放成员列表
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setContentsMargins(0,0,0,0); // 设置布局外边距为0
    m_mainLayout->setSpacing(5);               // 设置控件间的间距

    /*设置一个竖直布局的画布，可以向这个里面添加控件 */
    ui->wdg_list->setLayout(m_mainLayout);
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

/*显示房间号 */
void RoomDialog::slot_setInfo(QString roomid)
{
    QString title = QString("房间号:%1").arg(roomid);
    setWindowTitle(title);
    ui->lb_title->setText(title);
}

/*添加一个房间成员 */
void RoomDialog::slot_addUserInfo(UserShow *user)
{
    /*将用户信息控件添加到画布中 */
    m_mainLayout->addWidget(user);

    /*记录映射关系: pair<用户ID, 对应小窗口界面指针> */
    m_mapIdToUserShow[user->m_id] = user;

    connect(user, &UserShow::SIG_itemClicked, this, &RoomDialog::slot_setBigImgId);
}

/* 通过用户ID删除指定的用户小界面 */
void RoomDialog::slot_removeUserShow(int id)
{
    // 1. 使用 find 查找，比 count 更高效（可以直接拿到迭代器）
    auto ite = m_mapIdToUserShow.find(id);

    if(ite != m_mapIdToUserShow.end()){
        UserShow* user = ite->second;

        // 2. 先从 map 中移除该用户的指针记录，防止后续逻辑通过 ID 找到已销毁的对象
        m_mapIdToUserShow.erase(ite);

        // 3. 调用具体的 UI 销毁槽函数
        this->slot_removeUserShow(user);
    }
}

/* 彻底删除并销毁一个指定的房间用户小界面 */
void RoomDialog::slot_removeUserShow(UserShow *user)
{
    if(!user) return;

    // 1. 隐藏对应用户小界面，防止在布局刷新前出现视觉残留
    user->hide();

    /* 2. 从布局管理器中移除对应控件 */
    if(m_mainLayout) {
        m_mainLayout->removeWidget(user);
    }

    /* 3.
       不要只移除不删除。使用 deleteLater() 让 Qt 在当前事件循环结束后
       安全地释放 UserShow 及其内部 OpenGL 控件和定时器的内存资源。
    */
    user->deleteLater();
}

/*清空房间所有成员 */
void RoomDialog::slot_clearUserShow()
{
    /*遍历 std::map，安全释放里面所有的 UserShow 指针*/
    for (auto& pair : m_mapIdToUserShow) {
        if (pair.second != nullptr) {
            delete pair.second;
            pair.second = nullptr;
        }
    }

    /*绝对不能漏掉的最后一步：彻底清空字典！*/
    m_mapIdToUserShow.clear();
    // 重置大窗口状态，防止“鬼影”出现
    ui->wdg_userShow->slot_setInfo(0,"");// 清除用户ID和名字
    QImage blackImg(320, 240, QImage::Format_RGB888);
    blackImg.fill(Qt::black); // 或者使用你的默认占位图
    ui->wdg_userShow->slot_setImage(blackImg);
}

/*设置音频勾选状态 */
void RoomDialog::slot_setAudioSet(bool check)
{
    ui->cb_audio->setChecked(check);
}

/*设置视频勾选状态 */
void RoomDialog::slot_setvideoSet(bool check)
{
    ui->cb_vedio->setChecked(check);
}

/*设置屏幕共享勾选框状态 */
void RoomDialog::slot_setScreenSet(bool check)
{
    ui->cb_desk->setChecked(check);
}

/*刷新指定用户的小窗口图像（视频帧回调） */
void RoomDialog::slot_refreshUser(int id, QImage& img)
{
    /*预览大图的 ID 和要刷新的图片 ID 一致，则刷新预览区域 */
    if(ui->wdg_userShow->m_id == id){
        ui->wdg_userShow->slot_setImage(img);
    }

    // 同时也刷新成员列表中对应的小窗口
    if(m_mapIdToUserShow.count(id) > 0){
        UserShow* user = m_mapIdToUserShow[id];
        user->slot_setImage(img);
    }
}

/*设置大图预览区域的信息 */
void RoomDialog::slot_setBigImgId(int id, QString name)
{
    ui->wdg_userShow->slot_setInfo(id, name);
}

/*房间界面退出事件处理 */
void RoomDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this, "退出提示", "是否退出会议?") ==
            QMessageBox::Yes){
        /*发送信号通知核心处理类执行退出逻辑 */
        Q_EMIT SIG_close();
        // 接收（接受）关闭事件，窗口关闭
        event->accept();
        return;
    }
    // 忽略关闭事件，窗口保持开启
    event->ignore();
}

/* 右上角关闭按钮点击 */
void RoomDialog::on_pb_close_clicked()
{
    this->close();  // 调用此函数会触发 closeEvent()
}

/* 右下角退出按钮点击 */
void RoomDialog::on_pb_exit_clicked()
{
    this->close();
}

/* 开启或关闭 音频 */
void RoomDialog::on_cb_audio_clicked()
{
    if(ui->cb_audio->isChecked()){  // 打开音频
        Q_EMIT SIG_AudioStart();
    }else{                          // 关闭音频
        Q_EMIT SIG_AudioPause();
    }
}

/*开启或关闭 视频 */
void RoomDialog::on_cb_vedio_clicked()
{
    if(ui->cb_vedio->isChecked()){  // 打开视频
        ui->cb_desk->setChecked(false); // 视频与桌面共享通常互斥
        Q_EMIT SIG_VideoStart();
        Q_EMIT SIG_ScreenPause();
    }else{                          // 关闭视频
        Q_EMIT SIG_VideoPause();
    }
}

/* 开启或关闭 桌面共享 */
void RoomDialog::on_cb_desk_clicked()
{
    if(ui->cb_desk->isChecked()){  // 开启桌面共享
        ui->cb_vedio->setChecked(false); // 关闭视频
        Q_EMIT SIG_ScreenStart();
        Q_EMIT SIG_VideoPause();
    }else{                          // 关闭桌面共享
        Q_EMIT SIG_ScreenPause();
    }
}
