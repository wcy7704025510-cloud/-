#include "usershow.h"
#include "ui_usershow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include<QString>
UserShow::UserShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserShow)
{
    ui->setupUi(this);

    //绑定定时器信号：当定时器时间到，执行超时检测槽函数
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(slot_checkTimer()));

    /*初始化成员属性：记录最后一次收到视频帧的时间 */
    m_lastTime = QTime::currentTime();

    // 设置默认显示图片（如：未开启摄像头时的占位图）
    m_defaultImg.load(":/bq/000.png");

    /*启动定时器：每 1000ms (1秒) 触发一次检测 */
    m_timer.start(1000);
}

UserShow::~UserShow()
{
    delete ui;
}

/*设置成员变量：绑定用户ID和昵称 */
void UserShow::slot_setInfo(int id, QString name)
{
    m_id = id;
    m_userName = name;
    /*界面显示用户名 */
    QString text = QString("用户名:%1").arg(m_userName);
    ui->lb_name->setText(text);
}

/*
 * 重绘事件：Qt 界面渲染的核心
 * 只要调用 update()，系统就会异步调用此函数进行画面更新
 */
void UserShow::paintEvent(QPaintEvent *event)
{
    // 准备画布
    QPainter painter(this);

    // 1. 画黑色背景
    painter.setBrush(Qt::black);    // 设置黑色画刷
    painter.drawRect(0, 0, this->width(), this->height());

    /*2. 判断视频帧是否存在（校验高度） */
    if(m_img.size().height() <= 0){
        return;
    }

    /* 3. 绘制视频帧 */
    // 等比例缩放视频帧：Qt::KeepAspectRatio 保证画面不拉伸变形
    // 计算缩放后的图像，高度需减去下方名字标签占用的高度
    m_img = m_img.scaled(this->width(), this->height() - ui->lb_name->height(), Qt::KeepAspectRatio);

    /*4. 计算居中位置并显示视频帧 */
    QPixmap pix = QPixmap::fromImage(m_img);

    // 计算 X 方向居中偏移
    int x = (this->width() - pix.width()) / 2;

    // 计算 Y 方向居中偏移（在名字标签下方区域居中）
    int y_offset = (this->height() - ui->lb_name->height() - pix.height()) / 2;
    int y = ui->lb_name->height() + y_offset;

    painter.drawPixmap(QPoint(x, y), pix);
    painter.end(); // 结束绘制
}

/* 设置（接收）新的视频帧 */
void UserShow::slot_setImage(QImage &img)
{
    // 更新当前显示的视频帧
    m_img = img;

    /* 更新最后活跃时间（用于防掉线/超时检测） */
    m_lastTime = QTime::currentTime();

    /* 触发重绘：通知 Qt 尽快调用 paintEvent */
    this->update();
}

/* 监测鼠标点击事件：用于切换主画面预览 */
void UserShow::mousePressEvent(QMouseEvent *event)
{
    // 接收事件，防止事件继续向父控件传递
    event->accept();

    /* 发送自定义信号：通知房间类该用户项被点击了 */
    Q_EMIT SIG_itemClicked(m_id, m_userName);
}

/* 处理定时器到时信号的槽函数 */
void UserShow::slot_checkTimer()
{
    /* 定时检测：如果超过 5 秒没有收到新视频帧 */
    if(m_lastTime.secsTo(QTime::currentTime()) > 5){
        /*设置为默认画面（防止画面卡死在最后一帧） */
        slot_setImage(m_defaultImg);
    }
}
