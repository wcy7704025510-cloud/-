#ifndef USERSHOW_H
#define USERSHOW_H

#include <QWidget>
#include <QPaintEvent>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QTime>
#include <QMouseEvent>

namespace Ui {
class UserShow;
}

class UserShow : public QWidget
{
    Q_OBJECT

public:
    explicit UserShow(QWidget *parent = nullptr);
    ~UserShow();
signals:
    void SIG_itemClicked(int fd,QString name);

public slots:
    //设置用户小界面信息
    void slot_setInfo(int id,QString name);
    //重绘事件
    void paintEvent(QPaintEvent* event);
    //设置视频帧
    void slot_setImage(QImage& img);
    //检测鼠标点击事件
    void mousePressEvent(QMouseEvent *event);
    //处理定时器到时信号的槽函数
    void slot_checkTimer();
private:
    Ui::UserShow *ui;

    int m_id;
    QString m_userName;
    QImage m_img;
    friend class RoomDialog;

    QImage  m_defaultImg;
    QTimer m_timer;         //每隔1s检查一次上次传输图片过来的时候是否差5s
    QTime m_lastTime;       //每传过来图片记录那次的时间
};

#endif // USERSHOW_H
