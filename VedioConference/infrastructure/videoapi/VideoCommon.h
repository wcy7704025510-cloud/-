#ifndef VIDEOCOMMON_H
#define VIDEOCOMMON_H

#include <QPainter>
#include <QPoint>

// OpenCV计算机视觉库头文件
// 用途：摄像头采集、图像处理、格式转换
#include "opencv2/highgui/highgui.hpp"  // OpenCV高层GUI接口
#include "opencv2/imgproc/imgproc.hpp"  // 图像处理(缩放、滤波、色彩转换)
#include "opencv2/core/core.hpp"        // OpenCV核心数据结构
#include "opencv2/imgproc/types_c.h"    // C语言接口类型定义

// ============= 摄像头视频配置 =============
// 摄像头使用较低分辨率，节省网络带宽
// 适合实时视频通话场景

// 摄像头视频宽度(像素)
#define IMAGE_WIDTH (320)

// 摄像头视频高度(像素)
// 4:3比例，320×240是经典的视频通话分辨率
#define IMAGE_HEIGHT (240)

// 摄像头视频帧率(FPS)
// 每秒传输10帧，平衡流畅度和带宽占用
#define FRAME_RATE (10)

// ============= 屏幕共享配置 =============
// 屏幕共享使用高分辨率高质量配置
// 因为屏幕内容需要清晰展示文字和细节

// 屏幕共享宽度(像素)
#define SCREEN_WIDTH (960)

// 屏幕共享高度(像素)
// 960×720比摄像头清晰3倍(面积9倍像素)
#define SCREEN_HEIGHT (720)

// 屏幕共享帧率(FPS)
// 15FPS更流畅，适合观看屏幕操作
#define SCREEN_FRAME_RATE (15)

// 屏幕共享码率(bps)
// 2Mbps高码率，确保文字和细节清晰
// 摄像头只用400Kbps，屏幕共享需要5倍码率
#define SCREEN_BIT_RATE (2000000)

#endif // VEDIOCOMMON_H
