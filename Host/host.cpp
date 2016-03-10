// ����ͷ�ļ�
#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdint.h>

// OpenCVͷ�ļ�
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

// Boostͷ�ļ�
#include <boost/thread.hpp>
#include <boost/bind.hpp>

// �Զ���ͷ�ļ�
#include "serial/serial.h"
#include "../Slave/CommonDef.h"
#include "HostDef.h"
#include "msglink.hpp"
#include "Queue.h"

using namespace std;
using namespace serial;
using namespace cv;
using boost::thread;

#define SlaveConnected 0

Serial slave;// �ӻ�ͨ�Ŵ���
VideoCapture cap;// ����ͷ
bool backprojMode = false; //��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��ture��ʾ׼�����뷴��ͶӰģʽ
bool selectObject = false;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
int trackObject = 0; //�������Ŀ����Ŀ
bool showHist = true;//�Ƿ���ʾֱ��ͼ
Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection;//���ڱ������ѡ��ľ��ο�
int vmin = 10, vmax = 256, smin = 30;
MsgLink<DispMsg> linkd;

int main(int argc, char** argv)
{
	vector<thread> th;
	system("title ���������� ������ V1.0");
	// ���߳�ͬ����ʼ��
#if SlaveConnected
	th.push_back(thread(initSerialArg, argc, argv));
#endif
	th.push_back(thread(initCameraArg, argc, argv));
	for (auto it = th.begin();it != th.end();++it)
	{
		it->join();
	}
	// ������̨����ͼ���߳�
	thread processTh(boost::bind(processImage, &linkd));
	processTh.detach();
	// ��ʼ����
	circleWork();
	return 0;
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud = gengeralBaudRate;
	if (argc == 1)
	{// ������δ�ṩ����������Ļ����
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// �����в���ֻ�ṩ�˴��ڣ�������Ĭ��
		port = string(argv[1]);
		baud = gengeralBaudRate;
	}
	else if (argc >= 3)
	{// �����в����ṩ�˴��ںͲ�����
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
	// ���ô������Բ�����
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// ��⿪��״̬
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	cout << "���ڳ�ʼ���ɹ���" << endl;
	return;
	/*
	// ��ӻ�����������Ϣ
	slave.write(testComHost);
	// �����յ�������Ϣ
	for (auto i = 0;i < 3;++i)
	{
		slave.readline(tmp);
		if (tmp.find(testComSlave) != string::npos)
		{
			cout << "���ڳ�ʼ���ɹ���" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;
	*/
serialErr:
	cerr << "�޷��򿪴���:" << slave.getPort();
	system("pause");
	exit(1);
}

void initCameraArg(int argc, char** argv)
{
	if(argc <= 3)
	{// ������δ�ṩ����ͷ������Ĭ�ϴ�0
		initCamera(0);
	}
	else
	{// �������ṩ������ͷ����
		int id;
		if(sscanf_s(argv[3], "%d", &id) == 1)
		{
			// ֱ�Ӷ���ID
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
		// ������ͷ�����ò���
		cap.open(deviceID);
		cap.set(CAP_PROP_FRAME_WIDTH, VIDEO_WIDTH);
		cap.set(CAP_PROP_FRAME_HEIGHT, VIDEO_HEIGHT);
		cap.set(CAP_PROP_FPS, VIDEO_FPS);
		if (cap.isOpened() == false)
		{
			cerr << "�޷�������ͷ" << endl;
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
			cerr << "����ͷ������ƥ��" << endl;
			system("pause");
			exit(1);
		}*/
		cout << "����ͷ��ʼ���ɹ���" << endl;
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
	setMouseCallback(videoWindowName, onMouse, nullptr);//��Ϣ��Ӧ����
	createTrackbar("Vmin", videoWindowName, &vmin, 256, nullptr);//createTrackbar�����Ĺ������ڶ�Ӧ�Ĵ��ڴ�����������������Vmin,vmin��ʾ��������ֵ�����Ϊ256
	createTrackbar("Vmax", videoWindowName, &vmax, 256, nullptr);//���һ������Ϊ0����û�е��û����϶�����Ӧ����
	createTrackbar("Smin", videoWindowName, &smin, 256, nullptr);//vmin,vmax,smin��ʼֵ�ֱ�Ϊ10,256,30
	cout << "���ڳ�ʼ���ɹ���" << endl;
}

void processImage(MsgLink<DispMsg>* ld)
{
	Mat image,imgBak, hsv, mask, hue, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
	Rect trackWindow;
	RotatedRect trackBox;//����һ����ת�ľ��������
	int hsize = 16;
	float hranges[] = { 0,180 };//hranges�ں���ļ���ֱ��ͼ������Ҫ�õ�
	const float* phranges = hranges;
	CircleQueue_Avg<Point2f> pointQue(4);
	CircleQueue_Avg<double> timeQue(15);
	Point2f pts[6],centerPoint;
	// ������ɫ
	Vec3b centerColor;
	// ����
	bool out = false;
	double time[2] = {0,0};
	int k = 0;
	char tmp[10];
	// ��Ҫ���͵�����
	float data2Send[10];

	initWindow();
	while (true)
	{
		auto md = ld->receive();
		if (md != nullptr)
		{
			time[k ^= 1] = static_cast<double>(clock()) / CLOCKS_PER_SEC;
			timeQue.push(time[k] - time[k ^ 1]);
			auto last = pointQue.back();
			md->image.copyTo(image);
			image.copyTo(imgBak);
			cvtColor(image, hsv, CV_BGR2HSV);//��rgb����ͷ֡ת����hsv�ռ��
			if (trackObject)//trackObject��ʼ��Ϊ0,���߰�����̵�'c'����ҲΪ0������굥���ɿ���Ϊ-1
			{
				int _vmin = vmin, _vmax = vmax;

				//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h����
				//����������hsv��3��ͨ�����Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)�����3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
				//mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).
				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0, 0 };
				hue.create(hsv.size(), hsv.depth());//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ�ģ�������֮�����120�ȣ���ɫ���180��
				mixChannels(&hsv, 1, &hue, 1, ch, 1);//��hsv��һ��ͨ��(Ҳ����ɫ��)�������Ƶ�hue�У�0��������

				if (trackObject < 0)//���ѡ�������ɿ��󣬸ú����ڲ��ֽ��丳ֵ1
				{
					//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ����roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
					Mat roi(hue, selection), maskroi(mask, selection);//mask�����hsv����Сֵ
					
					//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
					//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
					calcHist(&roi, 1, nullptr, maskroi, hist, 1, &hsize, &phranges);//��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
					normalize(hist, hist, 0, 255, CV_MINMAX);//��hist����������鷶Χ��һ��������һ����0~255

					trackWindow = selection;
					trackObject = 1;//ֻҪ���ѡ�������ɿ�����û�а�������0��'c'����trackObjectһֱ����Ϊ1����˸�if����ֻ��ִ��һ�Σ���������ѡ���������

					histimg = Scalar::all(0);//�밴��'c'����һ���ģ������all(0)��ʾ���Ǳ���ȫ����0
					int binW = histimg.cols / hsize;  //histing��һ��200*300�ľ���hsizeӦ����ÿһ��bin�Ŀ�ȣ�Ҳ����histing�����ֳܷ�����bin����
					Mat buf(1, hsize, CV_8UC3);//����һ�����嵥bin����
					for (int i = 0; i < hsize; i++)//saturate_cast����Ϊ��һ����ʼ����׼ȷ�任����һ����ʼ����
						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);//Vec3bΪ3��charֵ������
					cvtColor(buf, buf, CV_HSV2BGR);//��hsv��ת����bgr

					for (int i = 0; i < hsize; i++)
					{
						int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);//at����Ϊ����һ��ָ������Ԫ�صĲο�ֵ
						rectangle(histimg, Point(i*binW, histimg.rows),    //��һ������ͼ���ϻ�һ���򵥳�ľ��Σ�ָ�����ϽǺ����½ǣ���������ɫ����С�����͵�
							Point((i + 1)*binW, histimg.rows - val),
							Scalar(buf.at<Vec3b>(i)), -1, 8);
					}

					// ����ɫ����
					centerColor = Vec3b(0, 0, 0);
				}

				calcBackProject(&hue, 1, nullptr, hist, backproj, &phranges);//����ֱ��ͼ�ķ���ͶӰ������hueͼ��0ͨ��ֱ��ͼhist�ķ���ͶӰ��������backproj��
				backproj &= mask;

				trackBox = CamShift(backproj, trackWindow,               //trackWindowΪ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��
					TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));//CV_TERMCRIT_EPS��ͨ��forest_accuracy,CV_TERMCRIT_ITER
				if (trackWindow.area() <= 1)                                                  //��ͨ��max_num_of_trees_in_the_forest  
				{
					int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
					trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
						trackWindow.x + r, trackWindow.y + r) &
						Rect(0, 0, cols, rows);//Rect����Ϊ�����ƫ�ƺʹ�С������һ��������Ϊ��������Ͻǵ����꣬�����ĸ�����Ϊ����Ŀ�͸�
				}

				if (backprojMode)
					cvtColor(backproj, image, CV_GRAY2BGR);//���ͶӰģʽ����ʾ��Ҳ��rgbͼ��
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);//���ٵ�ʱ������ԲΪ����Ŀ��
				trackBox.points(pts);
				pointQue.push(trackBox.center);
				pts[4] = pointQue.avg();
				for (auto i = 0;i < 5;++i)
				{
					circle(image, pts[i], 1, Scalar(255, 0, 0), 3, 8, 0);
				}
				circle(image, pts[4], static_cast<int>(norm(pts[4] - ((pts[0] + pts[3]) / 2))), Scalar(0, 255, 0), 3, 8, 0);
				if (centerColor == Vec3b(0, 0, 0))
				{
					// ��ѡ��ѡ�к��һ�μ��ʱ����������ɫ
					//centerColor = imgBak.at<Vec3b>(Point_<int>(pts[4]));
					centerColor = getColor(imgBak, Point_<int>(pts[4]));
				}
				if (!isSameColor(centerColor, getColor(imgBak, Point_<int>(pts[4]))) || out)
				{
					// ����ͬһ��ɫ��˵���ѳ���
					// ��һ�������ֵ
					centerPoint = Point2f(VIDEO_WIDTH * 1.1, VIDEO_HEIGHT * 1.1);
					sprintf_s(tmp, sizeof(tmp), "OUT");
					putText(image, tmp, Point(0, 60), 2, 1, CV_RGB(255, 255, 0));
				}
				else
				{
					centerPoint = Point2f(pts[4].x - VIDEO_WIDTH / 2, VIDEO_HEIGHT / 2 - pts[4].y);
				}
				data2Send[0] = centerPoint.x;
				data2Send[1] = centerPoint.y;
				system("cls");
				sendInfo2Slave(data2Send,2);
				cout << endl << (pts[4] - last) << endl << centerColor << endl << imgBak.at<Vec3b>(Point_<int>(pts[4])) << endl;
			}
			if (selectObject && selection.width > 0 && selection.height > 0)
			{
				Mat roi(image, selection);
				bitwise_not(roi, roi);//bitwise_notΪ��ÿһ��bitλȡ��
			}
			sprintf_s(tmp, sizeof(tmp), "%2d FPS", static_cast<int>(1.0 / timeQue.avg() + 0.5));
			putText(image, tmp, Point(0, 20), 2, 1, CV_RGB(255, 255, 0));
			sprintf_s(tmp, sizeof(tmp), "%.0f ms", timeQue.avg() * 1000);
			putText(image, tmp, Point(0, 40), 2, 1, CV_RGB(255, 255, 0));

			imshow(videoWindowName, image);
			imshow(histWindowName, histimg);
		}
		switch(waitKey(10))
		{
		case 27:	// ESC������ͼ����
			goto outLoop;
		case 'b':	//����ͶӰģ�ͽ���
			backprojMode = !backprojMode;
			break;
		case 'c':	//�������Ŀ�����
			trackObject = 0;
			histimg = Scalar::all(0);
			break;
		case 'h':	//��ʾֱ��ͼ����
			showHist = !showHist;
			if (!showHist)
			{
				destroyWindow(histWindowName);
				histimg = Scalar::all(0);
			}
			else
				namedWindow(histWindowName, 0);
			break;
		case 'o':
			// ��Ϊ����Ϊ�����ָ�
			out = !out;
		default:
			break;
		}
	}
outLoop:
	ld->close();
}

void sendInfo2Slave(float *data, size_t cnt)
{
	auto *dataTmp1 = new uint16_t[cnt];
	auto *frame = new uint8_t[cnt * 2 + 10];
	// ��һ������Ϊx���� ת��Ϊ�Ƕ� �Ŵ�100��
	dataTmp1[0] = static_cast<uint16_t>(data[0] / (VIDEO_HEIGHT / 2) * (VIDEO_VIEW_ANGLE / 2)) * 100.0 + 0.5;
	// �ڶ�������Ϊy���� ת��Ϊ�Ƕ� �Ŵ�100��
	dataTmp1[1] = static_cast<uint16_t>(data[1] / (VIDEO_HEIGHT / 2) * (VIDEO_VIEW_ANGLE / 2)) * 100.0 + 0.5;
	// ������ת��Ϊint16_t �Ŵ�100��
	for (auto i = 2u; i < cnt;++i)
	{
		dataTmp1[i] = static_cast<uint16_t>(data[i] * 100.0 + 0.5);
	}
#if SlaveConnected
	auto size = makeDataFrame(data, frame, 4 * sizeof(int16_t));
	slave.write(frame,size);
#endif
	printf("(%.1f, %.1f)", data[0], data[1]);
	delete dataTmp1;
	delete frame;
}

void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//ֻ�е�����������ȥʱ����Ч��Ȼ��ͨ��if�������Ϳ���ȷ����ѡ��ľ�������selection��
	{
		selection.x = MIN(x, origin.x);//�������ϽǶ�������
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);//���ο�
		selection.height = std::abs(y - origin.y);//���θ�
		selection &= Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);//����ȷ����ѡ�ľ���������ͼƬ��Χ��
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//���հ���ȥʱ��ʼ����һ����������
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

bool isSameColor(const cv::Vec3b &a, const cv::Vec3b &b)
{
	auto diff = Vec3b(abs(char(a[0] - b[0])),abs(char(a[1] - b[1])),abs(char(a[2] - b[2])));
	cout << norm(diff) << endl;
	return norm(diff) < 64.0;
}

cv::Vec3b getColor(const cv::Mat& img, const cv::Point_<int>& point)
{
	Vec3b colorAvg(0, 0, 0), tmp;
	Vec3i color(0, 0, 0);
	bool flag;
	int cnt = 0;
	static const int pos[8][2] = { {1,0},{0,1},{-1,0},{0,-1},{1,1},{1,-1},{-1,-1},{-1,1} };
	for (auto i = 0; i < 8;++i)
	{
		flag = true;
		try
		{
			tmp = img.at<Vec3b>(point.x + pos[i][0], point.y + pos[i][1]);
		}
		catch(exception &)
		{
			// ����Ļ��ǲ�������ɫ�˲���
			flag = false;
			tmp = Vec3b(0, 0, 0);
		}
		if(flag)
		{
			++cnt;
			for (auto j = 0; j < 3;++j)
			{
				color[j] += tmp[j];
			}
		}
	}
	if(cnt != 0)
	{
		for (auto i = 0; i < 3;++i)
		{
			colorAvg[i] = color[i] / cnt;
		}
	}
	else
	{
		colorAvg = Vec3b(0, 0, 0);
	}
	return colorAvg;
}
