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
#include "DataIO.h"


using namespace std;
using namespace HalconCpp;


class BarCodeDetect
{
public:
	BarCodeDetect();
	~BarCodeDetect();
	//识别条码(OK返回0，NG返回1，算法NG返回-1)
	int FindBarCodeFunc(const HObject &ho_Image, const RunParamsStruct_BarCode &runParams, ResultParamsStruct_BarCode &resultParams);
	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();
private:
	//写日志函数
	int WriteTxt(string content);

	//获取系统时间
	vector<string> get_current_time();
	std::string formatDobleValue(double val, int fixed);
public:
	string configPath;
	string XMLPath;
	RunParamsStruct_BarCode mRunParams;
	ResultParamsStruct_BarCode mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;
};

