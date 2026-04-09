#include "myopenglwidget.h"
#include <QDebug>

// 初始化 OpenGL
void MyOpenGLWidget::initializeGL()
{
    // 初始化 OpenGL 函数
    initializeOpenGLFunctions();
    // 设置背景颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // 启用 2D 纹理
    glEnable(GL_TEXTURE_2D);
    // 生成纹理对象并绑定
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// 绘制 OpenGL 场景
void MyOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_image.isNull())
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);

        //解除内存对齐限制
        // 因为 RGB888 是 3 字节，如果宽度不是 4 的倍数，OpenGL 默认的 4 字节对齐会直接导致花屏
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //告诉显卡，这是GL_RGB (3字节)，不要用 GL_RGBA (4字节) 去读！
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image.width(), m_image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_image.bits());

        QSize imgSize = getImageScaledSize(m_image.size());
        QSize wndSize = this->size();

        float left = -imgSize.width();
        float right = imgSize.width();
        float up = imgSize.height();
        float bottom = -imgSize.height();

        left = left * 1.0 / wndSize.width();
        right = right * 1.0 / wndSize.width();
        up = up * 1.0 / wndSize.height();
        bottom = bottom * 1.0 / wndSize.height();

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2d(left, bottom);
        glTexCoord2f(1, 1); glVertex2d(right, bottom);
        glTexCoord2f(1, 0); glVertex2d(right, up);
        glTexCoord2f(0, 0); glVertex2d(left, up);
        glEnd();
    }
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

// 设置图像，并触发重新绘制
void MyOpenGLWidget::slot_setImage(QImage img)
{
    // 无论外面传进来的是 PNG(ARGB32) 还是网络 JPEG(RGB32)
    // 均强行转换为显卡认识的 RGB888 (24位纯RGB) 格式！
    m_image = img.convertToFormat(QImage::Format_RGB888);

    this->update();
}

QSize MyOpenGLWidget::getImageScaledSize(QSize size)
{
    return size.scaled(this->size().width(), this->size().height(), Qt::KeepAspectRatio);
}
