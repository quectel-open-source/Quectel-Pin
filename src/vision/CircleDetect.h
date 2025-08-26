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
//*******************************************************************************************************************************
//ץ�߲�����ȫ
	//* 1.Roi������'num_measures'
	//set_metrology_object_param(MetrologyHandle, 'all', 'num_measures', Num)
	//* 2.��С�÷֣�'min_score'
	//set_metrology_object_param(MetrologyHandle, 'all', 'min_score', 0.3)
	//* 3.ֱ�����������Ĭ��ֵ�� 1 ֵ���б� 1��2��3��4
	//set_metrology_object_param(MetrologyHandle, 'all', 'num_instances', 4)
	//* 4.�����޳���Ĭ��ֵ�� 3.5  ֵ���б� 0��1.0��2.0�� 3.5��5.0
	//set_metrology_object_param(MetrologyHandle, 'all', 'distance_threshold', 3)
	//* 5.��������ֵ�б� 'all'��'negative'�� 'positive'��'uniform'
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_transition', 'negative')
	//* 6.RANSAC�㷨������������������ӣ�Ĭ��ֵ�� 42  ֵ���б� 0��1��42
	//set_metrology_object_param(MetrologyHandle, 'all', 'rand_seed', 42)
	//* 7.ָ���Բ����������֤��Ĭ��ֵ�� �� false��  ֵ�б� 'true'��'false'
	//set_metrology_object_param(MetrologyHandle, 'all', 'instances_outside_measure_regions', 'true')
	//* 8.��������: Ĭ��ֵ��-1  ֵ���б� 10��100��1000
	//set_metrology_object_param(MetrologyHandle, 'all', 'max_num_iterations',1000)
	//* 9.��ֵ���ͣ�nearest_neighbor'�Ҷ�ֵ�Ǵ�������صĻҶ�ֵ��ã�'bilinear'��ʹ��˫���Բ�ֵ ��'bicubic'����ʹ��˫���β�ֵ��
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_interpolation', 'nearest_neighbor')
	//* 10.��Եѡȡ��'all'��'first'��'last'
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_select', 'first')
//��ע������Բ�Ρ���Բ�λ�������͵Ķ��󣬲��������ڵĲ��������ǴӼ���������ڲ����ⲿ��
	  //����ֱ�����͵Ĳ������󣬲��������ڵĲ�����������ֱ��roi��㵽�յ㷽�򿴣����������Ǵ����ҡ�
//********************************************************************************************************************************


class CircleDetect
{
public:
	CircleDetect();
	~CircleDetect();
	//Բ���(OK����0��NG����1���㷨NG����-1)
	int FindCircle(const HObject &ho_Image, const RunParamsStruct_Circle &RunParams, ResultParamsStruct_Circle &resultParams);

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
	RunParamsStruct_Circle mRunParams;
	ResultParamsStruct_Circle mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;

};
