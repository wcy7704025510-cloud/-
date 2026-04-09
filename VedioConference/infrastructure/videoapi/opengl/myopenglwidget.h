#pragma once

#include <QOpenGLWidget>    // Qt OpenGL窗口控件类
#include <QOpenGLFunctions> // Qt OpenGL函数接口
#include <QImage>          // Qt图像类

// MyOpenGLWidget - OpenGL视频渲染控件
// 职责：使用GPU加速渲染视频帧
// 继承自QOpenGLWidget + QOpenGLFunctions
// 特点：
//   1. GPU硬件加速 - 比CPU渲染性能更高
//   2. 纹理贴图 - 使用OpenGL纹理存储和显示图像
//   3. 自动缩放 - 图像自适应窗口大小
class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT                    // Qt元对象系统宏
public:
    // 构造函数
    // 参数: parent - 父窗口指针
    MyOpenGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}

    // 设置视频画面
    // 参数: img - QImage格式的视频帧图像
    // 功能: 更新待显示图像，下次paintGL时渲染
    void slot_setImage(QImage img);

protected:
    // OpenGL初始化
    // 触发时机: OpenGL上下文首次创建时
    // 功能: 初始化OpenGL状态、创建纹理对象
    void initializeGL() override;

    // OpenGL绘制
    // 触发时机: 需要重绘时(调用update()或图像变化)
    // 功能: 将纹理绑定到OpenGL管线，绘制到窗口
    void paintGL() override;

    // OpenGL窗口大小变化
    // 触发时机: 窗口大小改变时
    // 功能: 更新OpenGL视口，调整渲染区域
    void resizeGL(int w, int h) override;

    // 计算图像缩放后的大小
    // 参数: size - 目标窗口大小
    // 返回: 保持宽高比的缩放后尺寸
    // 功能: 图像适应窗口时保持原始宽高比
    QSize getImageScaledSize(QSize size);

private:
    // 待显示的图像数据
    // 存储最近一次slot_setImage设置的图像
    QImage m_image;

    // OpenGL纹理对象ID
    // 用于存储上传到GPU的图像数据
    // 每帧更新时需要重新绑定纹理
    GLuint m_texture;
};
