#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <io.h>
#include <direct.h>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include <thread>
#include <mutex>
#include "datas.h"
#include "dllAPI.h"


using namespace std;
using namespace HalconCpp;


class _declspec(dllexport) PinDetect
{
public:
	PinDetect();
	PinDetect(std::string _nodeName, int _processModbuleID, int _processID);
	~PinDetect();

	//void setGetConfigPath(std::function<std::string()> getConfigPath);
	void setGetConfigPath(std::string getConfigPath);

	portNames portNames;
	std::map<std::string, portRegisterInfo> inputCtrlInfos;
	std::map<std::string, portRegisterInfo> outputCtrlInfos;
	std::map<std::string, portRegisterInfo> inputIconicInfos;
	std::map<std::string, portRegisterInfo> outputIconicInfos;

	groupLists groupLists;
	std::string groupName = groupLists.visualProjectsLayoutV;
	std::string modelCaption = "Pin针";
	modubleType modubleType = modubleType::Halcon;

	void setPortInfos();
	void setPortInfoInputCtrl();
	void setPortInfoOutputCtrl();
	void setPortInfoInputIconic();
	void setPortInfoOutputIconic();

	void run(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList);
	void ReadParams(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList);

	ResultParamsStruct_Pin resultParams;

	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();

	//Pin针检测(OK返回0，NG返回1，算法NG返回-1)
	int PinDetectFunc(const HObject &ho_Image, const RunParamsStruct_Pin &runParams, ResultParamsStruct_Pin &resultParams);
	//Pin找Blob函数
	int PinBlob(const HObject &ho_Image, const RunParamsStruct_Pin &runParams, HObject &ho_Region_Pin, HTuple &hv_Row_Pin, HTuple &hv_Column_Pin,
		HTuple &hv_Area_Blob, HTuple &hv_Row_Blob, HTuple &hv_Column_Blob, HTuple &hv_RecLen1_Blob, HTuple &hv_RecLen2_Blob, HTuple &hv_Circularity_Blob, HTuple &hv_Rectangularity_Blob
		, HTuple &hv_Width_Blob, HTuple &hv_Height_Blob);
	//抓边函数
	int FindLine(const HObject &ho_Image, const RunParamsStruct_Pin &runParams,
		HTuple &hv_Row1_Ret, HTuple &hv_Column1_Ret, HTuple &hv_Row2_Ret, HTuple &hv_Column2_Ret, HObject &ho_ContourLine_Ret);
	//抓圆函数
	int FindCircle(const HObject &ho_Image, const RunParamsStruct_Pin &runParams,
		HTuple &hv_Row_Ret, HTuple &hv_Column_Ret, HTuple &hv_Radius_Ret, HObject &ho_ContourCircle_Ret);
	//创建模板(int Type == 0 是位置修正)
	int CreateTemplate(const HObject &ho_Image, RunParamsStruct_Pin &RunParams, int Type);
	//查找模板(int Type == 0 是位置修正)
	int FindTemplate(const HObject &ho_Image, int Type, const RunParamsStruct_Pin &RunParams, HObject &ho_XLD_Template, HObject &ho_Region_Pin, HTuple &hv_Row_Pin, HTuple &hv_Column_Pin,
		HTuple &hv_Angle_Pin, HTuple &hv_Score_Pin);
private:
	//写日志函数
	int WriteTxt(string content);
	//写入csv
	int WriteCsv(const RunParamsStruct_Pin &runParams, const ResultParamsStruct_Pin &resultParams);
	//获取系统时间
	vector<string> get_current_time();

	std::vector<std::string> modubleResultTitleList;
private:
	int processID;
	int processModbuleID;
	std::string nodeName;

	QLog* qLog = nullptr;
	RunParamsStruct_Pin runParams;

	DllDatas* dllDatas1 = nullptr;
};

