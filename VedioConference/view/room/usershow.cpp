#include "usershow.h"
#include "ui_usershow.h"
#include <QDebug>
#include <QString>
#include"./myopenglwidget.h"
UserShow::UserShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserShow)
{
    ui->setupUi(this);



    // 动态嵌入 OpenGL 渲染器
    m_glVideo = new MyOpenGLWidget(this);

    // 将 OpenGL 控件添加到 ui 的垂直布局中
    ui->verticalLayout->addWidget(m_glVideo);

    // 设置拉伸策略：第0项(lb_name)不拉伸，第1项(m_glVideo)占满剩余空间
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 1);

    // 绑定定时器信号：当定时器时间到，执行超时检测槽函数
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slot_checkTimer()));

    /*初始化成员属性：记录最后一次收到视频帧的时间 */
    m_lastTime = QTime::currentTime();

    // 设置默认显示图片（如：未开启摄像头时的占位图）
    m_defaultImg.load(":/bq/000.png");

    //初始化时先把默认占位图喂给显卡，避免一开始出现纯黑框
    if(!m_defaultImg.isNull()){
        m_glVideo->slot_setImage(m_defaultImg);
    }

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

/* 接收新的视频帧，并喂给显卡 */
void UserShow::slot_setImage(QImage &img)
{
    /* 更新最后活跃时间（用于防掉线/超时检测） */
    m_lastTime = QTime::currentTime();

    //直接把 QImage 喂给你的 OpenGL 控件进行 GPU 渲染
    if (m_glVideo) {
        m_glVideo->slot_setImage(img);
    }
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
