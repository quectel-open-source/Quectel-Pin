#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <thread>
#include <mutex>
//#include "BinarizationDetect.h"
//#include "MorphologyDetect.h"
#include "DataIO.h"


using namespace std;
using namespace HalconCpp;



class BlobDetect
{
public:
	BlobDetect();
	~BlobDetect();
	//Blob函数
	//传入参数有三个，图片，二值化运行参数结构体，形态学运行参数结构体
	//传出参数一个，Blob结果参数结构体
	int BlobFunc(const HObject &ho_Image, const RunParamsStruct_Blob &runParams_Blob, ResultParamsStruct_Blob &resultParams);
	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();
private:
	//获取系统时间
	vector<string> get_current_time();
	std::string formatDobleValue(double val, int fixed);
public:
	string configPath;
	string XMLPath;
	RunParamsStruct_Blob mRunParams;
	ResultParamsStruct_Blob mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;
};

