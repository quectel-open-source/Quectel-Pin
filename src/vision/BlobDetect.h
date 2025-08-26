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
	//Blob����
	//���������������ͼƬ����ֵ�����в����ṹ�壬��̬ѧ���в����ṹ��
	//��������һ����Blob��������ṹ��
	int BlobFunc(const HObject &ho_Image, const RunParamsStruct_Blob &runParams_Blob, ResultParamsStruct_Blob &resultParams);
	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();
private:
	//��ȡϵͳʱ��
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

