#pragma once

#include "msglink.hpp"
#include <string>
// OpenCVͷ�ļ�
#include <opencv2/core/matx.hpp>
using std::string;

// ����������С(4KB)
#define MAX_BUF_SIZE	(4 * 1024)

// ��Ƶ���
#define VIDEO_WIDTH		640

// ��Ƶ�߶�
#define VIDEO_HEIGHT	480

// ��Ƶ֡��
#define VIDEO_FPS		30

// ��Ƶ�ӽ�
#define VIDEO_VIEW_ANGLE 90

// ��������
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

/*
�������ȽϺ�����
������һ�㲻��ֱ��==�Ƚϣ������жϲ�ֵ�ķ�ʽ��
����Ϊ������������Ϊ��ȵ���ֵ��Ĭ��1e-4����
*/
inline bool floatCmp(double a, double b, double eps = 1e-4)
{
	return abs(a - b) < eps;
}


/*
ͨ��main����������ʼ�����ڡ�
argv[1]������Ϊ�˿ڣ����������ɱ�׼�����ȡ
argv[2]������Ϊ�����ʣ���������ʹ��Ĭ�ϲ�����baudRate
��ȡ�˿ںͲ����ʺ����initSerial���н�һ����ʼ��
*/
void initSerialArg(int argc, char** argv);
/*
����portΪ�˿ڣ�����baudΪ�����ʣ�Ĭ��ΪbaurdRate��
ʹ��Serial���ṩ�ĳ�ʼ���������г�ʼ����
����ӻ����ͳ�ʼ����Ϣ��ȷ�����ӡ�
��ʼ��������ֹ��������
*/
void initSerial(string port, uint baud = gengeralBaudRate);
/*
ͨ��main����������ʼ�����ڡ�
argv[3]������Ϊ����ͷID����������Ĭ�ϴ�0��
��ȡID�����initCamera���н�һ����ʼ����
*/
void initCameraArg(int argc, char** argv);
/*
����Ϊ����ͷ�豸ID��
ʹ��VideoCapture���ṩ�ĳ�ʼ���������г�ʼ����
���������ͷ�����Ƿ����Ҫ��
��ʼ��������ֹ��������
*/
template<typename T>
void initCamera(T deviceID);
/*
��ʼ��������ʾ���ڲ���ӿؼ�
*/
void initWindow();
/*
����ͼ������
����Ϊ���߳���Ϣ������MsgLink��
�ú���Ϊ�����̣߳���̨��ʱ�����У��ڽ��յ�һ֡ͼ���
����ͼ��Ĵ������㣬��������Ϣ��
*/
void processImage(MsgLink<DispMsg>* ld);
/*
��ӻ�����λ����Ϣ��
*/
void sendInfo2Slave(float *data,size_t cnt);
// ����¼��ص�����
void onMouse(int event, int x, int y, int, void*);
/*
ѭ����������ȡͼ�񲢷��͸�processImage
*/
void circleWork();
// �ж�������ɫ�Ƿ�Ϊ��ͬ��ɫ
bool isSameColor(const cv::Vec3b &a, const cv::Vec3b &b);
// �����ɫ�����˲�
cv::Vec3b getColor(const cv::Mat& img, const cv::Point_<int>& point);