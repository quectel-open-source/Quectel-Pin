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


class TemplateDetect
{

public:
	TemplateDetect();
	~TemplateDetect();
	//����ģ��(OK����0��NG����1���㷨NG����-1)
	int FindTemplate(const HObject &ho_Image, const RunParamsStruct_Template &RunParams, ResultParamsStruct_Template &ResultParams);
	//����ģ��(OK����0��NG����1���㷨NG����-1)
	int CreateTemplate(const HObject &ho_Image, RunParamsStruct_Template &RunParams);

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
	RunParamsStruct_Template mRunParams;
	ResultParamsStruct_Template mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;

};
