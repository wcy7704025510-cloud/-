#ifndef USERSHOW_H
#define USERSHOW_H


#include <QWidget>           // Qt窗口控件基类
#include <QImage>         // Qt图像类(视频帧)
#include <QTimer>         // Qt定时器类(超时检测)
#include <QTime>          // Qt时间类(时间差计算)
#include <QMouseEvent>    // Qt鼠标事件类(点击检测)


#include "./myopenglwidget.h" // OpenGL视频渲染控件
namespace Ui {
class UserShow;
}

// UserShow - 用户视频显示控件类
// 职责：显示单个用户的视频画面(缩略图)
// 设计：作为房间成员列表中的每个小窗口
// 特性：
//   1. OpenGL GPU渲染，提高显示性能
//   2. 点击切换大预览窗口
//   3. 超时检测，画面卡死保护
class UserShow : public QWidget
{
    Q_OBJECT                    // Qt元对象系统宏
    friend class RoomDialog;    // RoomDialog需要访问m_id成员
signals:
    // 用户头像点击信号
    // 参数: fd - 用户ID, name - 用户昵称
    // 触发: 鼠标点击此控件
    // 接收者: RoomDialog::slot_setBigImgId()
    // 功能: 点击后将此用户设为大预览窗口显示
    void SIG_itemClicked(int fd, QString name);

public:
    // 构造函数/析构函数
    explicit UserShow(QWidget *parent = nullptr);
    ~UserShow();

public slots:
    // 设置用户信息
    // 参数: id - 用户ID, name - 用户昵称
    // 功能: 保存信息并更新界面显示
    void slot_setInfo(int id, QString name);

    // 设置视频画面
    // 参数: img - 解码后的视频帧图像
    // 功能: 将图像传递给OpenGL控件渲染显示
    void slot_setImage(QImage& img);

    // 鼠标点击事件处理
    // 功能: 检测用户点击，发射切换预览信号
    void mousePressEvent(QMouseEvent *event);

    // 定时器超时槽函数
    // 功能: 检测视频流是否中断(超过5秒无数据)
    void slot_checkTimer();

private:
    // Qt Designer生成的UI界面指针
    Ui::UserShow *ui;

    // 用户标识
    int m_id;                  // 用户唯一ID(服务器分配)
    QString m_userName;        // 用户昵称

    // 视频超时检测
    // 定时器: 每秒触发一次检测
    // 最后活跃时间: 记录收到最后一帧的时间
    // 超时阈值: 5秒
    // 超时动作: 显示默认占位图
    QTimer m_timer;             // 检测定时器(1秒间隔)
    QTime m_lastTime;          // 最后收到视频帧的时间

    // 默认显示图片
    // 用途: 无视频流或超时时显示的占位图
    QImage  m_defaultImg;

    // OpenGL视频渲染控件
    // 用途: 使用GPU渲染视频帧，性能比QLabel更高
    MyOpenGLWidget* m_glVideo;
};

#endif // USERSHOW_H
