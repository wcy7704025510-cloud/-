 #ifndef VEDIOCOMMON_H
#define VEDIOCOMMON_H

#include <QPainter>
#include <QPoint>

#include "highgui/highgui.hpp"
#include "imgproc/imgproc.hpp"
#include "core/core.hpp"
#include "opencv2\imgproc\types_c.h"

using namespace cv;

//定义图片的尺寸
#define IMAGE_WIDTH (320)
#define IMAGE_HEIGHT (240)

//帧率
#define FRAME_RATE (10)

#endif // VEDIOCOMMON_H
