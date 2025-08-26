#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <thread>
#include <string>
#include <mutex>
#include <io.h>
#include <direct.h>
#include "XmlClass2.h"
#include "dataDetect.h"


using namespace std;
using namespace HalconCpp;
class DataIO
{
public:
	DataIO();
	~DataIO();
	//保存一维码检测参数(runParams 一维码参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID)
	int WriteParams_BarCode(const string ConfigPath, const string XmlPath, const RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取一维码检测参数(runParams 一维码参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID, value 输出读取到的值)
	ErrorCode_Xml ReadParams_BarCode(const string ConfigPath, const string XmlPath, RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存二维码检测参数
	int WriteParams_Code2d(const string ConfigPath, const string XmlPath, const RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取二维码检测参数
	ErrorCode_Xml ReadParams_Code2d(const string ConfigPath, const string XmlPath, RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存模板匹配参数(OK返回0，NG返回1)
	int WriteParams_Template(const string ConfigPath, const string XmlPath, const RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//加载模板匹配参数(OK返回0，NG返回1)
	ErrorCode_Xml ReadParams_Template(const string ConfigPath, const string XmlPath, RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	
	//保存模板匹配参数(OK返回0，NG返回1)
	int WriteParams_ImgDifference(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//加载模板匹配参数(OK返回0，NG返回1)
	ErrorCode_Xml ReadParams_ImgDifference(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	
	//保存直线检测参数
	int WriteParams_Line(const string ConfigPath, const string XmlPath, const RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取直线检测参数
	ErrorCode_Xml ReadParams_Line(const string ConfigPath, const string XmlPath, RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存圆检测参数
	int WriteParams_Circle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取圆检测参数
	ErrorCode_Xml ReadParams_Circle(const string ConfigPath, const string XmlPath, RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存矩形检测参数
	int WriteParams_Rectangle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取矩形检测参数
	ErrorCode_Xml ReadParams_Rectangle(const string ConfigPath, const string XmlPath, RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存椭圆检测参数
	int WriteParams_Ellipse(const string ConfigPath, const string XmlPath, const RunParamsStruct_Ellipse &runParams, const int& processId, const const string& nodeName, const int& processModbuleID);
	//读取椭圆检测参数
	ErrorCode_Xml ReadParams_Ellipse(const string ConfigPath, const string XmlPath, RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存二值化参数
	int WriteParams_Binarization(const string ConfigPath, const string XmlPath, const RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取二值化参数
	ErrorCode_Xml ReadParams_Binarization(const string ConfigPath, const string XmlPath, RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存形态学参数
	int WriteParams_Morphology(const string ConfigPath, const string XmlPath, const RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取形态学参数
	ErrorCode_Xml ReadParams_Morphology(const string ConfigPath, const string XmlPath, RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存Blob参数
	int WriteParams_Blob(const string ConfigPath, const string XmlPath, const RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取Blob参数
	ErrorCode_Xml ReadParams_Blob(const string ConfigPath, const string XmlPath, RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存图像增强参数
	int WriteParams_ImgProcessOne(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取图像增强参数
	ErrorCode_Xml ReadParams_ImgProcessOne(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存两图处理参数
	int WriteParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const const string& nodeName, const int& processModbuleID);
	//读取两图处理参数
	ErrorCode_Xml ReadParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存图像转换参数
	int WriteParams_ImgConvert(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取图像转换参数
	ErrorCode_Xml ReadParams_ImgConvert(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存图像旋转参数
	int WriteParams_ImgRotate(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取图像旋转参数
	ErrorCode_Xml ReadParams_ImgRotate(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存图片分类参数
	int WriteParams_ImgClassify(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取图片分类参数
	ErrorCode_Xml ReadParams_ImgClassify(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存极坐标变换参数
	int WriteParams_PolarTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取极坐标变换参数
	ErrorCode_Xml ReadParams_PolarTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存标定板标定参数
	int WriteParams_CalibBoard(const string ConfigPath, const string XmlPath, const ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取标定板标定参数
	ErrorCode_Xml ReadParams_CalibBoard(const string ConfigPath, const string XmlPath, ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存光度立体参数
	int WriteParams_PhotoStereo(const string ConfigPath, const string XmlPath, const RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取光度立体参数
	ErrorCode_Xml ReadParams_PhotoStereo(const string ConfigPath, const string XmlPath, RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存傅里叶变换参数
	int WriteParams_FourierTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取傅里叶变换参数
	ErrorCode_Xml ReadParams_FourierTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存仿射变换参数
	int WriteParams_AffineMatrix(const string ConfigPath, const string XmlPath, const RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取仿射变换参数
	ErrorCode_Xml ReadParams_AffineMatrix(const string ConfigPath, const string XmlPath, RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存裁图参数
	int WriteParams_CutImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取裁图参数
	ErrorCode_Xml ReadParams_CutImg(const string ConfigPath, const string XmlPath, RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存创建Roi参数
	int WriteParams_CreateRoi(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取创建Roi参数
	ErrorCode_Xml ReadParams_CreateRoi(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////保存颜色检测参数
	//int WriteParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////读取颜色检测参数
	//ErrorCode_Xml ReadParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存颜色检测参数
	int WriteParams_ColorDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取颜色检测参数
	ErrorCode_Xml ReadParams_ColorDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存拟合参数
	int WriteParams_FitDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取拟合参数
	ErrorCode_Xml ReadParams_FitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存交点参数
	int WriteParams_Intersection(const string ConfigPath, const string XmlPath, const RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取交点参数
	ErrorCode_Xml ReadParams_Intersection(const string ConfigPath, const string XmlPath, RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//保存测量参数
	int WriteParams_Measurement(const string ConfigPath, const string XmlPath, const RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取测量参数
	ErrorCode_Xml ReadParams_Measurement(const string ConfigPath, const string XmlPath, RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存存图参数
	int WriteParams_SaveImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取存图参数
	ErrorCode_Xml ReadParams_SaveImg(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存OKNG判断的参数
	int WriteParams_Judgement(const string ConfigPath, const string XmlPath, const RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取OKNG判断的参数
	ErrorCode_Xml ReadParams_Judgement(const string ConfigPath, const string XmlPath, RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存显示窗口参数
	int WriteParams_ImgShow(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取显示窗口参数
	ErrorCode_Xml ReadParams_ImgShow(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存区域变换参数
	int WriteParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取区域变换参数
	ErrorCode_Xml ReadParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存创建轮廓参数
	int WriteParams_CreateContourDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取创建轮廓参数
	ErrorCode_Xml ReadParams_CreateContourDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////保存机器人定位引导参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID)
	//int WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////读取机器人定位参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID, value 输出读取到的值)
	//ErrorCode_Xml ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存联合标定标定参数
	int WriteParams_JointCalibration(const string ConfigPath, const string XmlPath, const RunParamsStruct_JointCalibrationDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取联合标定标定参数
	ErrorCode_Xml ReadParams_JointCalibration(const string ConfigPath, const string XmlPath, RunParamsStruct_JointCalibrationDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////保存Pin针检测参数
	//int WriteParams_PinDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////读取Pin针检测参数
	//ErrorCode_Xml ReadParams_PinDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存Pcb检测参数
	int WriteParams_PcbDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取Pcb检测参数
	ErrorCode_Xml ReadParams_PcbDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID);


	//保存MachineLearning检测参数
	int WriteParams_MachineLearning(const string ConfigPath, const string XmlPath, const RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取MachineLearning检测参数
	ErrorCode_Xml ReadParams_MachineLearning(const string ConfigPath, const string XmlPath, RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存字符检测参数(OK返回0，NG返回1)
	int WriteParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//加载字符检测参数(OK返回0，NG返回1)
	ErrorCode_Xml ReadParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存图像拆分参数(OK返回0，NG返回1)
	int WriteParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//加载图像拆分参数(OK返回0，NG返回1)
	ErrorCode_Xml ReadParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	
	//保存灰度计算参数(OK返回0，NG返回1)
	int WriteParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//加载灰度计算参数(OK返回0，NG返回1)
	ErrorCode_Xml ReadParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存ColorSort检测参数
	int WriteParams_ColorSortDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取ColorSort检测参数
	ErrorCode_Xml ReadParams_ColorSortDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//保存ColorExtract检测参数
	int WriteParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//读取ColorExtract检测参数
	ErrorCode_Xml ReadParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID);



	//读取流程使用的模块字段
	ErrorCode_Xml ReadXML_Process_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID);
	//保存全局变量的字段名称(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
	int WriteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//保存全局变量的字段值和全局变量名字(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
	int WriteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//读取全局变量的字段名称
	ErrorCode_Xml ReadXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//读取全局变量的字段值和全局变量名字
	ErrorCode_Xml ReadXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//删除全局变量的字段名称
	int DeleteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath);
	//删除全局变量的字段值和全局变量名称
	int DeleteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath);


private:

	//写日志函数
	int WriteTxt(string content);
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	//按照指定字符分割字符串到vector
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);
	//路径设置
	void PathSet(const string ConfigPath, const string XmlPath);
	string FolderPath();
private:
	//mutex mutex1;//参数设置的线程同步变量

};

