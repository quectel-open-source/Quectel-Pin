#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <string>
#include <string.h>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace HalconCpp;
using namespace cv;
// 运行参数结构体
struct RunParamsStruct_PaintDetect
{
	vector<HObject> vec_InputRegion;		//输入区域数组
	vector<HTuple> vec_Ret;		//输入结果数组，1是OK，非1是NG
	HTuple hv_Ret = 0;		//1是OK,非1是NG
	HTuple hv_LineWidth = 2;	//绘制区域的线宽
	HTuple hv_Word_X = 10;		//绘制文字的X坐标
	HTuple hv_Word_Y = 10;		//绘制文字的Y坐标
	HTuple hv_Word_Size = 10;	//文字的字号
};
class PaintDetect
{
public:
	PaintDetect();
	~PaintDetect();
	int PaintImage(HObject ho_Image, RunParamsStruct_PaintDetect &RunParams, cv::Mat &ho_DestImg);
	void DrawString(Mat& OutImageMat, HTuple isOK, double fontSize, int leftUpX, int leftUpY);
	cv::Mat HObject2Mat(HObject Hobj);
	HObject Mat2HObject(cv::Mat image);

};

