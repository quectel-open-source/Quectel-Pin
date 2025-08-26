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


class QRCodeDetect
{
public:
	QRCodeDetect();
	~QRCodeDetect();
	//ʶ���ά��(OK����0��NG����1���㷨NG����-1)
	int FindCode2dFunc(const HObject &ho_Image, const RunParamsStruct_Code2d &runParams, ResultParamsStruct_Code2d &resultParams);
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
	RunParamsStruct_Code2d mRunParams;
	ResultParamsStruct_Code2d mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;
};

