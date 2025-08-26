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
	//ʶ������(OK����0��NG����1���㷨NG����-1)
	int FindBarCodeFunc(const HObject &ho_Image, const RunParamsStruct_BarCode &runParams, ResultParamsStruct_BarCode &resultParams);
	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();
private:
	//д��־����
	int WriteTxt(string content);

	//��ȡϵͳʱ��
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

