#pragma once

#include "msglink.hpp"

// 缓冲区最大大小(4KB)
#define MAX_BUF_SIZE	(4 * 1024)

// 视频宽度
#define VIDEO_WIDTH		640

// 视频高度
#define VIDEO_HEIGHT	480

// 视频帧率
#define VIDEO_FPS		30

const char *histWindowName = "直方图", *videoWindowName = "视频";

class DispMsg : public MsgData
{
public:
	cv::Mat image;
};

union int16_8
{
	unsigned __int16 i16;
	unsigned __int8 i8[2];
};

inline bool floatCmp(double a, double b, double eps = 1e-4)
{
	return abs(a - b) < eps;
}
