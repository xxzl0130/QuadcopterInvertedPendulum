// 基础头文件
#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdint.h>

// OpenCV头文件
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

// Boost头文件
#include <boost/thread.hpp>
#include <boost/bind.hpp>

// 自定义头文件
#include "serial/serial.h"
#include "../CommonDef.h"
#include "HostDef.h"
#include "msglink.hpp"
#include "Queue.h"
#include "DataFrame.h"
#include "MySerial.h"

using namespace std;
using namespace serial;
using namespace cv;
using boost::thread;

#define SlaveConnected 0

MySerial slave;// 从机通信串口
char buf[MAX_BUF_SIZE];// 缓冲区
VideoCapture cap;// 摄像头
bool backprojMode = false; //表示是否要进入反向投影模式，ture表示准备进入反向投影模式
bool selectObject = false;//代表是否在选要跟踪的初始目标，true表示正在用鼠标选择
int trackObject = 0; //代表跟踪目标数目
bool showHist = true;//是否显示直方图
Point origin;//用于保存鼠标选择第一次单击时点的位置
Rect selection, trackWindow;//用于保存鼠标选择的矩形框
int vmin = 10, vmax = 256, smin = 30;
MsgLink<DispMsg> linkd;

int main(int argc, char** argv)
{
	vector<thread> th;
	system("title 四旋翼倒立摆 主机端 V1.0");
	// 多线程同步初始化
#if SlaveConnected
	th.push_back(thread(initSerialArg, argc, argv));
#endif
	th.push_back(thread(initCameraArg, argc, argv));
	for (auto it = th.begin();it != th.end();++it)
	{
		it->join();
	}
	// 开启后台处理图像线程
	thread processTh(boost::bind(processImage, &linkd));
	processTh.detach();
	// 开始工作
	circleWork();
	return 0;
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud;
	if (argc == 1)
	{// 命令行未提供参数则由屏幕输入
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// 命令行参数只提供了串口，波特率默认
		port = string(argv[1]);
		baud = baudRate;
	}
	else if (argc >= 3)
	{// 命令行参数提供了串口和波特率
		port = string(argv[1]);
		sscanf_s(argv[2], "%u", &baud);
	}
	try
	{
		initSerial(port, baud);
	}
	catch (exception& err)
	{
		cerr << "Unhandled Exception: " << err.what() << endl;
		exit(1);
	}
}

void initSerial(string port, uint baud)
{
	string tmp;
	// 设置串口属性并开启
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// 检测开启状态
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	return;
	// 向从机发送握手信息
	slave.write(testComHost);
	// 检查回收的握手信息
	for (auto i = 0;i < 3;++i)
	{
		slave.readline(tmp);
		if (tmp.find(testComSlave) != string::npos)
		{
			cout << "串口初始化成功。" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;

serialErr:
	cerr << "无法打开串口:" << slave.getPort();
	system("pause");
	exit(1);
}

void initCameraArg(int argc, char** argv)
{
	if(argc <= 3)
	{// 命令行未提供摄像头参数，默认打开0
		initCamera(0);
	}
	else
	{// 命令行提供了摄像头参数
		int id;
		if(sscanf_s(argv[3], "%d", &id) == 1)
		{
			// 直接读到ID
			initCamera(id);
		}
		else
		{
			initCamera(string(argv[3]));
		}
	}
}

template<typename T>
void initCamera(T deviceID)
{
	try
	{
		// 打开摄像头并设置参数
		cap.open(deviceID);
		cap.set(CAP_PROP_FRAME_WIDTH, VIDEO_WIDTH);
		cap.set(CAP_PROP_FRAME_HEIGHT, VIDEO_HEIGHT);
		cap.set(CAP_PROP_FPS, VIDEO_FPS);
		if (cap.isOpened() == false)
		{
			cerr << "无法打开摄像头" << endl;
			system("pause");
			exit(1);
		}
		Mat tmp;
		cap >> tmp;
		cout << cap.get(CAP_PROP_FRAME_WIDTH) << ' ';
		cout << cap.get(CAP_PROP_FRAME_HEIGHT) << ' ';
		cout << cap.get(CAP_PROP_FPS) << endl;
		/*if ((!floatCmp(cap.get(CAP_PROP_FRAME_WIDTH),VIDEO_WIDTH)) ||
			(!floatCmp(cap.get(CAP_PROP_FRAME_HEIGHT),VIDEO_HEIGHT)) ||
			(!floatCmp(cap.get(CAP_PROP_FPS), VIDEO_FPS)))
		{
			cerr << "摄像头参数不匹配" << endl;
			system("pause");
			exit(1);
		}*/
		cout << "摄像头初始化成功。" << endl;
		return;
	}
	catch (exception& err)
	{
		cerr << "Unhandled Exception: " << err.what() << endl;
		system("pause");
		exit(1);
	}
}

void initWindow()
{
	namedWindow(histWindowName, 0);
	namedWindow(videoWindowName, WINDOW_AUTOSIZE);
	imshow(videoWindowName, Mat(VIDEO_WIDTH, VIDEO_HEIGHT, CV_8UC3, Scalar(0, 0, 0)));
	setMouseCallback(videoWindowName, onMouse, nullptr);//消息响应机制
	createTrackbar("Vmin", videoWindowName, &vmin, 256, nullptr);//createTrackbar函数的功能是在对应的窗口创建滑动条，滑动条Vmin,vmin表示滑动条的值，最大为256
	createTrackbar("Vmax", videoWindowName, &vmax, 256, nullptr);//最后一个参数为0代表没有调用滑动拖动的响应函数
	createTrackbar("Smin", videoWindowName, &smin, 256, nullptr);//vmin,vmax,smin初始值分别为10,256,30
	cout << "窗口初始化成功。" << endl;
}

void processImage(MsgLink<DispMsg>* ld)
{
	Mat image, hsv, mask, hue, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
	Rect trackWindow;
	RotatedRect trackBox;//定义一个旋转的矩阵类对象
	int hsize = 16;
	float hranges[] = { 0,180 };//hranges在后面的计算直方图函数中要用到
	const float* phranges = hranges;
	CircleQueue_Avg<Point2f> pointQue(4);
	CircleQueue_Avg<double> timeQue(15);
	Point2f pts[6];
	double time[2] = {0,0};
	int k = 0;
	char tmp[10];
	initWindow();
	while (true)
	{
		auto md = ld->receive();
		if (md != nullptr)
		{
			time[k ^= 1] = static_cast<double>(clock()) / CLOCKS_PER_SEC;
			timeQue.push(time[k] - time[k ^ 1]);
			md->image.copyTo(image);
			cvtColor(image, hsv, CV_BGR2HSV);//将rgb摄像头帧转化成hsv空间的
			if (trackObject)//trackObject初始化为0,或者按完键盘的'c'键后也为0，当鼠标单击松开后为-1
			{
				int _vmin = vmin, _vmax = vmax;

				//inRange函数的功能是检查输入数组每个元素大小是否在2个给定数值之间，可以有多通道,mask保存0通道的最小值，也就是h分量
				//这里利用了hsv的3个通道，比较h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)。如果3个通道都在对应的范围内，则
				//mask对应的那个点的值全为1(0xff)，否则为0(0x00).
				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0, 0 };
				hue.create(hsv.size(), hsv.depth());//hue初始化为与hsv大小深度一样的矩阵，色调的度量是用角度表示的，红绿蓝之间相差120度，反色相差180度
				mixChannels(&hsv, 1, &hue, 1, ch, 1);//将hsv第一个通道(也就是色调)的数复制到hue中，0索引数组

				if (trackObject < 0)//鼠标选择区域松开后，该函数内部又将其赋值1
				{
					//此处的构造函数roi用的是Mat hue的矩阵头，且roi的数据指针指向hue，即共用相同的数据，select为其感兴趣的区域
					Mat roi(hue, selection), maskroi(mask, selection);//mask保存的hsv的最小值

					//calcHist()函数第一个参数为输入矩阵序列，第2个参数表示输入的矩阵数目，第3个参数表示将被计算直方图维数通道的列表，第4个参数表示可选的掩码函数
					//第5个参数表示输出直方图，第6个参数表示直方图的维数，第7个参数为每一维直方图数组的大小，第8个参数为每一维直方图bin的边界
					calcHist(&roi, 1, nullptr, maskroi, hist, 1, &hsize, &phranges);//将roi的0通道计算直方图并通过mask放入hist中，hsize为每一维直方图的大小
					normalize(hist, hist, 0, 255, CV_MINMAX);//将hist矩阵进行数组范围归一化，都归一化到0~255

					trackWindow = selection;
					trackObject = 1;//只要鼠标选完区域松开后，且没有按键盘清0键'c'，则trackObject一直保持为1，因此该if函数只能执行一次，除非重新选择跟踪区域

					histimg = Scalar::all(0);//与按下'c'键是一样的，这里的all(0)表示的是标量全部清0
					int binW = histimg.cols / hsize;  //histing是一个200*300的矩阵，hsize应该是每一个bin的宽度，也就是histing矩阵能分出几个bin出来
					Mat buf(1, hsize, CV_8UC3);//定义一个缓冲单bin矩阵
					for (int i = 0; i < hsize; i++)//saturate_cast函数为从一个初始类型准确变换到另一个初始类型
						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);//Vec3b为3个char值的向量
					cvtColor(buf, buf, CV_HSV2BGR);//将hsv又转换成bgr

					for (int i = 0; i < hsize; i++)
					{
						int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);//at函数为返回一个指定数组元素的参考值
						rectangle(histimg, Point(i*binW, histimg.rows),    //在一幅输入图像上画一个简单抽的矩形，指定左上角和右下角，并定义颜色，大小，线型等
							Point((i + 1)*binW, histimg.rows - val),
							Scalar(buf.at<Vec3b>(i)), -1, 8);
					}
				}

				calcBackProject(&hue, 1, nullptr, hist, backproj, &phranges);//计算直方图的反向投影，计算hue图像0通道直方图hist的反向投影，并让入backproj中
				backproj &= mask;

				trackBox = CamShift(backproj, trackWindow,               //trackWindow为鼠标选择的区域，TermCriteria为确定迭代终止的准则
					TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));//CV_TERMCRIT_EPS是通过forest_accuracy,CV_TERMCRIT_ITER
				if (trackWindow.area() <= 1)                                                  //是通过max_num_of_trees_in_the_forest  
				{
					int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
					trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
						trackWindow.x + r, trackWindow.y + r) &
						Rect(0, 0, cols, rows);//Rect函数为矩阵的偏移和大小，即第一二个参数为矩阵的左上角点坐标，第三四个参数为矩阵的宽和高
				}

				if (backprojMode)
					cvtColor(backproj, image, CV_GRAY2BGR);//因此投影模式下显示的也是rgb图？
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);//跟踪的时候以椭圆为代表目标
				trackBox.points(pts);
				pointQue.push(trackBox.center);
				pts[4] = pointQue.avg();
				for (auto i = 0;i < 5;++i)
				{
					circle(image, pts[i], 1, Scalar(255, 0, 0), 3, 8, 0);
				}
				circle(image, pts[4], static_cast<int>(norm(pts[4] - ((pts[0] + pts[3]) / 2))), Scalar(0, 255, 0), 3, 8, 0);
			}
			if (selectObject && selection.width > 0 && selection.height > 0)
			{
				Mat roi(image, selection);
				bitwise_not(roi, roi);//bitwise_not为将每一个bit位取反
			}
			sprintf_s(tmp, sizeof(tmp), "FPS:%2d", static_cast<int>(1.0 / timeQue.avg() + 0.5));
			putText(image, tmp, Point(0, 20), 2, 1, CV_RGB(255, 255, 0));
			sprintf_s(tmp, sizeof(tmp), "%.0fms", timeQue.avg() * 1000);
			putText(image, tmp, Point(0, 40), 2, 1, CV_RGB(255, 255, 0));

			imshow(videoWindowName, image);
			imshow(histWindowName, histimg);
			sendInfo2Slave(pts[4].x - VIDEO_WIDTH / 2, pts[4].y - VIDEO_HEIGHT / 2, 0);
		}
		switch(waitKey(10))
		{
		case 27:	// ESC键结束图像处理
			goto outLoop;
		case 'b':	//反向投影模型交替
			backprojMode = !backprojMode;
			break;
		case 'c':	//清零跟踪目标对象
			trackObject = 0;
			histimg = Scalar::all(0);
			break;
		case 'h':	//显示直方图交替
			showHist = !showHist;
			if (!showHist)
			{
				destroyWindow(histWindowName);
				histimg = Scalar::all(0);
			}
			else
				namedWindow(histWindowName, 0);
			break;
		default:
			break;
		}
	}
outLoop:
	ld->close();
}

void sendInfo2Slave(float x, float y, float z)
{
	int16_t data[3];
	uint8_t frame[20];
	data[0] = static_cast<int16_t>(x * 10.0 + 0.5);
	data[1] = static_cast<int16_t>(y * 10.0 + 0.5);
	data[2] = static_cast<int16_t>(z * 10.0 + 0.5);
	auto size = makeDataFrame(data, frame, 3 * sizeof(int16_t));
#if SlaveConnected
	slave.write(frame,size);
#endif
	printf("(%.1f, %.1f, %.1f)\r", x, y, z);
}

void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//只有当鼠标左键按下去时才有效，然后通过if里面代码就可以确定所选择的矩形区域selection了
	{
		selection.x = MIN(x, origin.x);//矩形左上角顶点坐标
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);//矩形宽
		selection.height = std::abs(y - origin.y);//矩形高
		selection &= Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);//用于确保所选的矩形区域在图片范围内
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//鼠标刚按下去时初始化了一个矩形区域
		selectObject = true;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;
		break;
	}
}

void circleWork()
{
	while (true)
	{
		auto md = linkd.prepareMsg();
		cap >> md->image;
		linkd.send();
		if (linkd.isClosed())
			break;
	}
}
