#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#endif
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include <thread>
#include <mutex>
#include "XmlClass2.h"
#include "PaintDetect.h"


using namespace std;
using namespace HalconCpp;
//模板匹配类型结构体
struct ModelTypeStruct_Template_RobotCalib
{
	string Aniso = "aniso";
	string Ncc = "ncc";
};
//九点标定类型结构体
struct RunParamsStruct_RobotCalib
{
	//******************机器人标定参数*************************************************
	HTuple hv_NPointCamera_X;	  //九点标定像素坐标X
	HTuple hv_NPointCamera_Y;    //九点标定像素坐标Y
	HTuple hv_NPointRobot_X;	 //九点标定机器坐标X
	HTuple hv_NPointRobot_Y;	 //九点标定机器坐标Y
	HTuple hv_HomMat2D = NULL;	 //九点标定矩阵
	HTuple hv_CenterCamera_X;	 //旋转中心标定像素坐标X
	HTuple hv_CenterCamera_Y;    //旋转中心标定像素坐标Y
	HTuple hv_CenterRobot_X;	//旋转中心标定机器坐标X
	HTuple hv_CenterRobot_Y;    //旋转中心标定机器坐标Y

	HTuple hv_Center_X;			//机器坐标旋转中心X
	HTuple hv_Center_Y;			//机器坐标旋转中心Y
	HTuple hv_Center_Radius;	//机器坐标旋转半径
	HObject ho_CenterCircle;	//旋转中心的圆


	//是否计算偏移量还是机器人绝对坐标
	bool hv_IsAbsCoordinate = false;
	//九宫格相邻两点之间距离(单位mm)
	HTuple hv_DistancePP_Robot = 10.0;
	//旋转中心标定时，每次旋转的角度
	HTuple hv_AngleRotate_Robot = 10;
	//旋转中心标定时，机器人绝对坐标
	HTuple hv_CenterPosition_RobotX = 0.000;
	HTuple hv_CenterPosition_RobotY = 0.000;
	HTuple hv_CenterPosition_RobotA = 0.000;
	//建模位机器人绝对坐标
	HTuple hv_ModelPosition_RobotX = 0.000;
	HTuple hv_ModelPosition_RobotY = 0.000;
	HTuple hv_ModelPosition_RobotA = 0.000;
	//建模位物料上面Mark点坐标
	HTuple hv_ModelX = 0.000;	//模板位置X
	HTuple hv_ModelY = 0.000;	//模板位置Y
	HTuple hv_ModelA = 0.000;	//模板位置A
	//待纠偏物料上面Mark点当前坐标
	HTuple hv_NowX = 0.000;		//当前位置X
	HTuple hv_NowY = 0.000;		//当前位置Y
	HTuple hv_NowA = 0.000;		//当前位置A
	bool hv_IsCamFirst = true;  //先拍后抓或者先抓后拍
	//补偿角度是否取反(当图像中角度正方向与机器人的旋转角度正反向相反时，需要把补偿角度取反),true是取反，false是不取反
	bool hv_IsReverseAngle = false;
	//是否启用旋转中心 true是启用, false不启用
	bool hv_IsUseCenterCalib = true;

	//*********************模板匹配参数**************************************************************
	int hv_IsUsSearchRoi = 0;//0 不启用搜索区域，1 启用搜索区域
	string hv_MatchMethod = "aniso";      //模板类型
	int hv_NumLevels = -1;              //金字塔级别
	double hv_AngleStart = -30;                //起始角度
	double hv_AngleExtent = 30;                //角度范围

	double hv_ScaleRMin = 0.9;                 //最小行缩放(ScaleModel模式的时候hv_ScaleRMin_Train代表X和Y方向最小缩放系数)
	double hv_ScaleRMax = 1.1;                 //最大行缩放(ScaleModel模式的时候hv_ScaleRMax_Train代表X和Y方向最大缩放系数)

	double hv_ScaleCMin = 0.9;                 //最小列缩放
	double hv_ScaleCMax = 1.1;				   //最大列缩放

	string hv_Metric = "use_polarity";         //极性/度量
	int hv_Contrast = -1;               //对比度
	int hv_MinContrast = -1;            //最小对比度

	double hv_Angle_Original = 0;			 //创建模板时，原始模板角度
	double hv_CenterRow_Original = 0;		//创建模板时，原始模板Y
	double hv_CenterCol_Original = 0;		//创建模板时，原始模板X

	double hv_MinScore = 0.5;               //最低分数
	int hv_NumMatches = 1;               //匹配个数，0则自动选择，100则最多匹配100个
	double hv_MaxOverlap = 0.5;             //要找到模型实例的最大重叠
	double hv_Greediness = 0.5;             //贪婪系数
	string hv_SubPixel = "least_squares";   //亚像素
	int hv_TimeOut = 1000;			//超时时间(单位ms)
	//训练区域，区域内所有点的集合
	vector<long long> VecRows_TrainRoi;
	vector<long long> VecColumns_TrainRoi;
	//搜索区域，矩形的两个角点
	cv::Point P1_SearchRoi = cv::Point(10, 200);
	cv::Point P2_SearchRoi = cv::Point(10, 200);

	HTuple hv_ModelID;      //模板句柄

};
//计算偏移量或者绝对坐标结果结构体
struct ResultParamsStruct_RobotCalib
{
	//*************************************机器人标定参数***********************************
	//偏移量结果参数(偏移量或者机器人绝对坐标)
    double hv_X_Robot = -99999;	//偏移量X
    double hv_Y_Robot = -99999;	//偏移量Y
    double hv_A_Robot = -99999;	//偏移量A
	//*************************************模板匹配参数*****************************************
	cv::Mat DestImg;
	//建模时的坐标和角度
	double hv_Angle_Original = 0;			 //创建模板时，原始模板角度
	double hv_CenterRow_Original = 0;		//创建模板时，原始模板Y
	double hv_CenterCol_Original = 0;		//创建模板时，原始模板X
	//运行时找到的模板结果
	vector<double> hv_Row;                    //中心点（X）
	vector<double> hv_Column;                 //中心点（Y）
	vector<double> hv_Angle;                  //角度
	vector<double> hv_Score;                  //匹配分数
	int hv_RetNum = 0;             //找到的模板个数(默认0，找到模板后会大于0)

	//仿射变换矩阵(用于抓边跟随)
	HTuple hv_ROIHomMat2D;
};

class RobotCalibDetect
{
public:
	RobotCalibDetect();
	~RobotCalibDetect();
	//机器人定位
	int RobotLocate(const cv::Mat &InputImage, const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams);
	//计算偏移量和机器人绝对坐标函数
	int CalculateFunc(const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams);
	//九点标定函数
	int NPointCalibrate(RunParamsStruct_RobotCalib &RunParams);
	//旋转中心标定
	int CenterCalibrate(RunParamsStruct_RobotCalib &RunParams);


	//查找模板(OK返回0，NG返回1，算法NG返回-1)
	int FindTemplate(const cv::Mat &InputImage, const RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams);
	//创建模板(OK返回0，NG返回1，算法NG返回-1)
	int CreateTemplate(const cv::Mat &InputImage, RunParamsStruct_RobotCalib &RunParams);

	//保存机器人定位引导参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID)
	int WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取机器人定位参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID, value 输出读取到的值)
	ErrorCode_Xml ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	int getRefreshConfig(int processID, int modubleID, std::string modubleName);

private:
	//路径设置(返回值 0 OK, 非0 NG)
	int PathSet(const string ConfigPath, const string XmlPath);
	//按照指定字符分割字符串到vector
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	std::string formatDobleValue(double val, int fixed);
	PaintDetect *PaintDetect1;
public:
	string configPath;
	string XMLPath;
	RunParamsStruct_RobotCalib mRunParams;
	ResultParamsStruct_RobotCalib mResultParams;

private:
	//计算偏移量
	int CalcuOffset(const RunParamsStruct_RobotCalib &RunParams, double &OffsetX, double &OffsetY, double &OffsetA);
	//坐标点转换
	int TransPoint(const RunParamsStruct_RobotCalib &RunParams, double &Output_X, double &Output_Y);
	/// <summary>
	/// 先拍后抓偏移量算法
	/// </summary>
	/// <param name="Model_X">模板X</param>
	/// <param name="Model_Y">模板Y</param>
	/// <param name="Model_A">模板A</param>
	/// <param name="Now_X">当前X</param>
	/// <param name="Now_Y">当前Y</param>
	/// <param name="Now_A">当前A</param>
	/// <param name="Center_X">旋转中心X</param>
	/// <param name="Center_Y">旋转中心Y</param>
	/// <param name="offset_x">偏移量X</param>
	/// <param name="offset_y">偏移量Y</param>
	/// <param name="offset_a">偏移量A</param>
	void GetOffset_CamFirst2(bool isReverseAngle, double Model_X, double Model_Y, double Model_A, double Now_X, double Now_Y,
		double Now_A, double Center_X, double Center_Y, double &offset_x, double &offset_y, double &offset_a);
	/// <summary>
	/// 先抓后拍偏移量算法
	/// </summary>
	/// <param name="Model_X">模板X</param>
	/// <param name="Model_Y">模板Y</param>
	/// <param name="Model_A">模板A</param>
	/// <param name="Now_X">当前X</param>
	/// <param name="Now_Y">当前Y</param>
	/// <param name="Now_A">当前A</param>
	/// <param name="Center_X">旋转中心X</param>
	/// <param name="Center_Y">旋转中心Y</param>
	/// <param name="offset_x">偏移量X</param>
	/// <param name="offset_y">偏移量Y</param>
	/// <param name="offset_a">偏移量A</param>
	void GetOffset_GrabFirst2(bool isReverseAngle, double Model_X, double Model_Y, double Model_A, double Now_X, double Now_Y,
		double Now_A, double Center_X, double Center_Y, double &offset_x, double &offset_y, double &offset_a);

};

