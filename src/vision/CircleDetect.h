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
//抓边参数大全
	//* 1.Roi数量：'num_measures'
	//set_metrology_object_param(MetrologyHandle, 'all', 'num_measures', Num)
	//* 2.最小得分：'min_score'
	//set_metrology_object_param(MetrologyHandle, 'all', 'min_score', 0.3)
	//* 3.直线拟合数量：默认值： 1 值的列表： 1，2，3，4
	//set_metrology_object_param(MetrologyHandle, 'all', 'num_instances', 4)
	//* 4.距离剔除：默认值： 3.5  值的列表： 0，1.0，2.0， 3.5，5.0
	//set_metrology_object_param(MetrologyHandle, 'all', 'distance_threshold', 3)
	//* 5.明暗方向：值列表： 'all'，'negative'， 'positive'，'uniform'
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_transition', 'negative')
	//* 6.RANSAC算法的随机数生成器的种子：默认值： 42  值的列表： 0，1，42
	//set_metrology_object_param(MetrologyHandle, 'all', 'rand_seed', 42)
	//* 7.指定对测量结果的验证：默认值： “ false”  值列表： 'true'，'false'
	//set_metrology_object_param(MetrologyHandle, 'all', 'instances_outside_measure_regions', 'true')
	//* 8.迭代次数: 默认值：-1  值的列表： 10，100，1000
	//set_metrology_object_param(MetrologyHandle, 'all', 'max_num_iterations',1000)
	//* 9.插值类型：nearest_neighbor'灰度值是从最近像素的灰度值获得；'bilinear'，使用双线性插值 ；'bicubic'，则使用双三次插值。
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_interpolation', 'nearest_neighbor')
	//* 10.边缘选取：'all'，'first'，'last'
	//set_metrology_object_param(MetrologyHandle, 'all', 'measure_select', 'first')
//备注：对于圆形、椭圆形或矩形类型的对象，测量区域内的测量方向是从计量对象的内部到外部。
	  //对于直线类型的测量对象，测量区域内的测量方向是以直线roi起点到终点方向看，测量方向是从左到右。
//********************************************************************************************************************************


class CircleDetect
{
public:
	CircleDetect();
	~CircleDetect();
	//圆检测(OK返回0，NG返回1，算法NG返回-1)
	int FindCircle(const HObject &ho_Image, const RunParamsStruct_Circle &RunParams, ResultParamsStruct_Circle &resultParams);

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
	RunParamsStruct_Circle mRunParams;
	ResultParamsStruct_Circle mResultParams;
private:
	DataIO dataIOC;
	std::vector<std::string> modubleResultTitleList;

};
