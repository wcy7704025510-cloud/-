#include "usershow.h"
#include "ui_usershow.h"

UserShow::UserShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserShow)
{
    ui->setupUi(this);
    //绑定定时器到时信号和对应的槽函数 &m_timer表示栈上的localtimer的内存空间的别名
    connect(&m_timer,SIGNAL(timeout()),
            this,SLOT(slot_checkTimer()));
    //初始化成员属性 最后时间
    m_lastTime=QTime::currentTime();
    //设置默认显示图片
    m_defaultImg.load(":/bq/000.png");
    //启动定时器
    m_timer.start(1000);
}

UserShow::~UserShow()
{
    delete ui;
}

//设置成员变量
void UserShow::slot_setInfo(int id, QString name)
{
    m_id=id;
    m_userName=name;

    QString text=QString("用户名:%1").arg(m_userName);
    ui->lb_name->setText(text);
}
#include<QDebug>
//重绘事件
void UserShow::paintEvent(QPaintEvent *event)
{
    //画黑背景
    QPainter painter(this);
    painter.setBrush(Qt::black);    //设置黑色画刷
    painter.drawRect(0,0,this->width(),this->height());
    //判断视频帧是否存在
    if(m_img.size().height()<=0){
        return;
    }
    //画视频帧
    //等比例缩放视频帧
    m_img=m_img.scaled(this->width(),this->height()-ui->lb_name->height(),Qt::KeepAspectRatio);
    //显示视频帧
    QPixmap pix=QPixmap::fromImage(m_img);
    int x=this->width()-pix.width();
    x=x/2;
    int y=this->height()-ui->lb_name->height()-pix.height();
    y=ui->lb_name->height()+y/2;
    painter.drawPixmap(QPoint(x,y),pix);
    painter.end();
}

//设置视频帧
void UserShow::slot_setImage(QImage &img)
{
    //设置当前视频帧
    m_img=img;
    //更新最后时间
    m_lastTime=QTime::currentTime();
    //控件更新，触发重绘时间
    this->update();
}

//检测鼠标点击事件
void UserShow::mousePressEvent(QMouseEvent *event)
{
    //接收事件
    event->accept();
    //发送信号
    Q_EMIT SIG_itemClicked(m_id,m_userName);
}

//处理定时器到时信号的槽函数
void UserShow::slot_checkTimer()
{
    //定时检测 界面是否超时
    if(m_lastTime.secsTo(QTime::currentTime())>5){
        //设置为默认画面
        slot_setImage(m_defaultImg);
    }
}
