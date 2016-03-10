#pragma once

#include "msglink.hpp"
#include <string>
// OpenCV头文件
#include <opencv2/core/matx.hpp>
using std::string;

// 缓冲区最大大小(4KB)
#define MAX_BUF_SIZE	(4 * 1024)

// 视频宽度
#define VIDEO_WIDTH		640

// 视频高度
#define VIDEO_HEIGHT	480

// 视频帧率
#define VIDEO_FPS		30

// 视频视角
#define VIDEO_VIEW_ANGLE 90

// 窗口名称
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

/*
浮点数比较函数。
浮点数一般不能直接==比较，采用判断差值的方式。
参数为两浮点数和认为相等的阈值（默认1e-4）。
*/
inline bool floatCmp(double a, double b, double eps = 1e-4)
{
	return abs(a - b) < eps;
}


/*
通过main函数参数初始化串口。
argv[1]存在则为端口，不存在则由标准输入获取
argv[2]存在则为波特率，不存在则使用默认波特率baudRate
获取端口和波特率后调用initSerial进行进一步初始化
*/
void initSerialArg(int argc, char** argv);
/*
参数port为端口，参数baud为波特率，默认为baurdRate。
使用Serial类提供的初始化方法进行初始化，
并向从机发送初始化信息以确认连接。
初始化错误将终止整个程序。
*/
void initSerial(string port, uint baud = gengeralBaudRate);
/*
通过main函数参数初始化串口。
argv[3]存在则为摄像头ID，不存在则默认打开0。
获取ID后调用initCamera进行进一步初始化。
*/
void initCameraArg(int argc, char** argv);
/*
参数为摄像头设备ID。
使用VideoCapture类提供的初始化方法进行初始化，
并检查摄像头参数是否符合要求。
初始化错误将终止整个程序。
*/
template<typename T>
void initCamera(T deviceID);
/*
初始化各个显示窗口并添加控件
*/
void initWindow();
/*
处理图像函数。
参数为多线程消息传递类MsgLink。
该函数为独立线程，后台长时间运行，在接收到一帧图像后
进行图像的处理运算，并反馈信息。
*/
void processImage(MsgLink<DispMsg>* ld);
/*
向从机发送位置信息。
*/
void sendInfo2Slave(float *data,size_t cnt);
// 鼠标事件回调函数
void onMouse(int event, int x, int y, int, void*);
/*
循环工作，读取图像并发送给processImage
*/
void circleWork();
// 判断两个颜色是否为相同颜色
bool isSameColor(const cv::Vec3b &a, const cv::Vec3b &b);
// 获得颜色，并滤波
cv::Vec3b getColor(const cv::Mat& img, const cv::Point_<int>& point);