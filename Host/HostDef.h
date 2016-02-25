#pragma once

#include "msglink.hpp"

// ����������С(4KB)
#define MAX_BUF_SIZE	(4 * 1024)

// ��Ƶ���
#define VIDEO_WIDTH		640

// ��Ƶ�߶�
#define VIDEO_HEIGHT	480

// ��Ƶ֡��
#define VIDEO_FPS		30

const char *histWindowName = "ֱ��ͼ", *videoWindowName = "��Ƶ";

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
