#include "BarCodeDetect.h"
BarCodeDetect::BarCodeDetect()
{
	////通用类指针对象实例化
	//m_Common = new Common();
}

BarCodeDetect::~BarCodeDetect()
{
	////释放通用类指针资源
	//delete m_Common;
}

//识别条码
int BarCodeDetect::FindBarCodeFunc(const HObject &ho_Image, const RunParamsStruct_BarCode &runParams, ResultParamsStruct_BarCode &resultParams)
{
	try
	{
		//清空结果
		resultParams.hv_Row.Clear();
		resultParams.hv_Column.Clear();
		resultParams.hv_CodeType_Result.Clear();
		resultParams.hv_CodeContent.Clear();
		resultParams.hv_CodeLength_Result.Clear();
		resultParams.hv_QualityGrade.Clear();
		resultParams.hv_RetNum = 0;
		GenEmptyObj(&resultParams.ho_ContourBarCode);
		resultParams.ho_ContourBarCode.Clear();
		GenEmptyObj(&resultParams.ho_RegionBarCode);
		resultParams.ho_RegionBarCode.Clear();

		//mutex1.lock();
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}
		//等级类别,每个值代表一个类别
		HTuple hv_Overall_Quality = 0;	//扫描反射率剖面等级,每条扫描线的扫描反射率轮廓等级的算术平均值
		HTuple hv_GRADE_DECODE = 1;	    //当符号的反射率分布可以根据符号体系的参考解码算法进行解码时，设置为4，否则设置为0
		HTuple hv_GRADE_CONTRAST = 2;	//反射率分布中最小值和最大值之间的范围。强烈的对比会带来好的评分。
		HTuple hv_GRADE_MINIMAL_REFLECTANCE = 3;	//如果扫描反射率分布中的最低反射率值低于或等于最大反射率值的0.5，则设置为4。否则，将分配值0。
		HTuple hv_GRADE_MINIMAL_EDGE_CONTRAST = 4;	//任何两个相邻元素之间的对比，无论是条到空间还是空间到条。最小边缘对比度对反射率分布中测量的边缘对比度值的最小值进行分级。
		HTuple hv_GRADE_MODULATION = 5;		//指示条形码元素的振幅有多强。大振幅使元素到条或空间的分配更加确定，从而产生高调制等级。
		HTuple hv_GRADE_DEFECTS = 6;		//反映元素和安静区内灰度值分布的不规则性。
		HTuple hv_GRADE_DECODABILITY = 7;		//反映元素宽度与为相应符号定义的标称宽度的偏差。
		HTuple hv_GRADE_ADDITIONAL_REQS = 8;	//关于条形码符号特定要求的等级。它主要涉及所需的安静区，但有时也可能与宽/窄比、字符间间隙、防护模式等有关。


		HTuple hv_BarCodeHandle; //条码识别句柄
		HObject mho_Image;
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//搜索区域为空，或者被clear(搜索区域为空，默认全图搜索)
			mho_Image = ho_Image;
		}
		else
		{
			HObject RegionAffine;
			GenEmptyObj(&RegionAffine);
			//是否进行仿射变换，区域跟随
			if (runParams.isFollow)
			{
				if (runParams.hv_HomMat2D == NULL)
				{
					mResultParams = resultParams;
					return 1;
				}
				//利用传入的矩阵进行仿射变换
				AffineTransRegion(runParams.ho_SearchRegion, &RegionAffine, runParams.hv_HomMat2D, "nearest_neighbor");
			}
			else
			{
				RegionAffine = runParams.ho_SearchRegion;
			}
			ReduceDomain(ho_Image, RegionAffine, &mho_Image);
		}

		//创建条形码模型句柄。
		CreateBarCodeModel(HTuple(), HTuple(), &hv_BarCodeHandle);
		if (runParams.hv_Tolerance.TupleEqual(HTuple("low")) == 1)
		{
			//设置要查找的条码的最小宽度参数，也就是黑色和白色矩形 条沿着扫描方向的最小宽度
			SetBarCodeParam(hv_BarCodeHandle, "element_size_min", 1.5);
			//设置要查找条码的解码过程的中间结果是否保存，1是，0，否。
			SetBarCodeParam(hv_BarCodeHandle, "persistence", 1);
			//设置最少使用两条扫描线进行计算，结果相同才认为结果正确
			SetBarCodeParam(hv_BarCodeHandle, "min_identical_scanlines", 2);
		}
		////设置要查找的条码的最小宽度参数，也就是黑色和白色矩形 条沿着扫描方向的最小宽度
		//SetBarCodeParam(hv_BarCodeHandle, "element_size_min", 1.5);
		////设置要查找条码的解码过程的中间结果是否保存，1是，0，否。
		//SetBarCodeParam(hv_BarCodeHandle, "persistence", 1);
		////设置最少使用两条扫描线进行计算，结果相同才认为结果正确
		//SetBarCodeParam(hv_BarCodeHandle, "min_identical_scanlines", 2);
		//设置扫码个数(设置为0，则扫描所有条码)
		SetBarCodeParam(hv_BarCodeHandle, "stop_after_result_num", runParams.hv_CodeNum.TupleInt());
		//分别检查扫描线的开始或停止模式时，强制执行宽容（“高”）或严格（“低”）搜索条件。如果在处理的图像中成像清晰的符号，
		//则更宽容的标准通常会增加条形码的检测机会。另一方面，它可能导致在噪声图像或包含其他符号的图像中进行错误检测。容忍度较低的标准提高了对错误检测的鲁棒性，
		//但可能会降低总体检测率。目前，这个参数仅对Code 128和GS1 - 128有区别。
		SetBarCodeParam(hv_BarCodeHandle, "start_stop_tolerance", runParams.hv_Tolerance);
		//设置扫码超时时间
		SetBarCodeParam(hv_BarCodeHandle, "timeout", runParams.hv_TimeOut.TupleInt());

		//在图片中开始查找条码，字符结果保存在DecodedDataStrings，条码区域保存在SymbolRegions
		//'auto'自动搜索条码类型
		if (runParams.hv_CodeType_Run.TupleLength() <= 0)
		{
			mResultParams = resultParams;
			return 1;
		}
		HObject ho_CodeRegionsTemp;
		HTuple hv_CodeIds, hv_CodeContentTemp, hv_CodeTypeTemp;
		FindBarCode(mho_Image, &ho_CodeRegionsTemp, hv_BarCodeHandle, runParams.hv_CodeType_Run, &hv_CodeContentTemp);
		HTuple CodeRegionCount;
		CountObj(ho_CodeRegionsTemp, &CodeRegionCount);
		int RegionsCount = CodeRegionCount.I();
		if (RegionsCount > 0)
		{
			GenEmptyObj(&resultParams.ho_ContourBarCode);
			GenEmptyObj(&resultParams.ho_RegionBarCode);
			hv_CodeIds = HTuple::TupleGenSequence(0, (hv_CodeContentTemp.TupleLength()) - 1, 1);
			int RetNumTemp = 0;
			HObject ho_CodeRegions, ho_CodeScanLines;
			for (int i = 0; i < RegionsCount; i++)
			{
				//条码类型
				GetBarCodeResult(hv_BarCodeHandle, i, "decoded_types", &hv_CodeTypeTemp);

				//计算条码等级
				HTuple hv_QualityLabels, hv_Quality, hv_QualityValues;
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_labels", &hv_QualityLabels);
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_values", &hv_QualityValues);
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_float_grades", &hv_Quality);
				hv_Quality = 4;
				
				//过滤错误条码
				//计算条码字符长度
				HTuple CodeLengthTemp;//计算条码长度临时变量
				TupleStrlen(hv_CodeContentTemp[i], &CodeLengthTemp);
				if (CodeLengthTemp.TupleInt().I() < runParams.hv_CodeLength_Run.TupleInt())
				{
					//条码长度不满足，退出当前循环
					continue;
				}
				if (runParams.hv_ContainChar != "" && runParams.hv_ContainChar != "null")
				{
					//查找字符串是否包含特定字符
					HTuple hv_Position;
					TupleStrchr(hv_CodeContentTemp[i], runParams.hv_ContainChar, &hv_Position);
					if (hv_Position.TupleInt().I() == -1)
					{
						//不包含，退出当前循环
						continue;
					}
				}
				if (runParams.hv_NotContainChar != "" && runParams.hv_NotContainChar != "null")
				{
					//查找字符串是否不包含特定字符
					HTuple hv_Position;
					TupleStrchr(hv_CodeContentTemp[i], runParams.hv_NotContainChar, &hv_Position);
					if (hv_Position.TupleInt().I() != -1)
					{
						//包含了不应该包含的字符，退出当前循环
						continue;
					}
				}
				if (runParams.hv_HeadChar != "" && runParams.hv_HeadChar != "null")
				{
					//查找字符串是否特定字符开头
					//截取字符串的第一个字符
					HTuple hv_Substring;
					TupleStrFirstN(hv_CodeContentTemp[i], 0, &hv_Substring);
					HTuple hv_Equal;
					TupleEqual(hv_Substring, runParams.hv_HeadChar, &hv_Equal);
					if (hv_Equal.TupleInt().I() == 0)
					{
						//没有以特定字符开头，退出当前循环
						continue;
					}
				}
				if (runParams.hv_EndChar != "" && runParams.hv_EndChar != "null")
				{
					//查找字符串是否特定字符结尾
					//截取字符串的最后一个字符
					HTuple hv_Substring;
					TupleStrLastN(hv_CodeContentTemp[i], CodeLengthTemp.TupleInt().I() - 1, &hv_Substring);
					HTuple hv_Equal;
					TupleEqual(hv_Substring, runParams.hv_EndChar, &hv_Equal);
					if (hv_Equal.TupleInt().I() == 0)
					{
						//没有以特定字符结尾，退出当前循环
						continue;
					}
				}


				//获取条码计算的中间结果：条码区域
				ho_CodeRegions.Clear();
				GetBarCodeObject(&ho_CodeRegions, hv_BarCodeHandle, HTuple(hv_CodeIds[i]),
					"candidate_regions");
				HObject RegionTrans;
				ShapeTrans(ho_CodeRegions, &RegionTrans, "rectangle2");
				ConcatObj(resultParams.ho_RegionBarCode, RegionTrans, &resultParams.ho_RegionBarCode);
				//获取条码计算的中间结果：扫描线
				ho_CodeScanLines.Clear();
				GetBarCodeObject(&ho_CodeScanLines, hv_BarCodeHandle, HTuple(hv_CodeIds[i]),
					"scanlines_valid");
				ConcatObj(resultParams.ho_ContourBarCode, ho_CodeScanLines, &resultParams.ho_ContourBarCode);

				HTuple hv_AreaTemp, hv_RowTemp, hv_ColumnTemp;
				AreaCenter(ho_CodeRegions, &hv_AreaTemp, &hv_RowTemp, &hv_ColumnTemp);

				resultParams.hv_Row.Append(hv_RowTemp);
				resultParams.hv_Column.Append(hv_ColumnTemp);
				resultParams.hv_CodeLength_Result.Append(CodeLengthTemp.TupleInt());
				resultParams.hv_CodeType_Result.Append(hv_CodeTypeTemp);
				resultParams.hv_CodeContent.Append(hv_CodeContentTemp[i]);
				resultParams.hv_QualityGrade.Append(hv_Quality);
				RetNumTemp += 1;
			}
			ClearBarCodeModel(hv_BarCodeHandle);
			resultParams.hv_RetNum = RetNumTemp;//找到的条码个数
			//mutex1.unlock();
			mResultParams = resultParams;

			return 0;
		}
		else
		{
			ClearBarCodeModel(hv_BarCodeHandle);
			mResultParams = resultParams;
			return 1;
		}
	}
	catch (...)
	{
		//清空结果
		resultParams.hv_Row.Clear();
		resultParams.hv_Column.Clear();
		resultParams.hv_CodeType_Result.Clear();
		resultParams.hv_CodeContent.Clear();
		resultParams.hv_CodeLength_Result.Clear();
		resultParams.hv_QualityGrade.Clear();
		resultParams.hv_RetNum = 0;
		GenEmptyObj(&resultParams.ho_ContourBarCode);
		resultParams.ho_ContourBarCode.Clear();
		GenEmptyObj(&resultParams.ho_RegionBarCode);
		resultParams.ho_RegionBarCode.Clear();

		mResultParams = resultParams;
		return 1;
	}
}

//写日志函数
int BarCodeDetect::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}

//获取系统时间
vector<string> BarCodeDetect::get_current_time()
{
	/*能精确到毫秒*/
	/*
	#include <windows.h>
	#include <iostream>
	*/
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms

	char tmpbuff[16];
	/*年*/
	sprintf(tmpbuff, "%d", st.wYear);
	std::string year = tmpbuff;
	/*月*/
	sprintf(tmpbuff, "%d", st.wMonth);
	std::string month = tmpbuff;
	/*日*/
	sprintf(tmpbuff, "%d", st.wDay);
	std::string day = tmpbuff;
	/*时*/
	sprintf(tmpbuff, "%d", st.wHour);
	std::string hour = tmpbuff;
	/*分*/
	sprintf(tmpbuff, "%d", st.wMinute);
	std::string minute = tmpbuff;
	/*秒*/
	sprintf(tmpbuff, "%d", st.wSecond);
	std::string second = tmpbuff;
	/*毫秒*/
	sprintf(tmpbuff, "%d", st.wMilliseconds);
	std::string millisecond = tmpbuff;

	std::vector<string>current_time = {};

	current_time.push_back(year + month + day);
	current_time.push_back(hour + minute + second);
	current_time.push_back(year + month + day + hour + minute + second);
	current_time.push_back(year + "_" + month + "_" + day + "_" + hour + "_" + minute + "_" + second + "_" + millisecond);

	return current_time;
}

int BarCodeDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML读取参数
	ErrorCode_Xml errorCode = dataIOC.ReadParams_BarCode(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
std::vector<std::string> BarCodeDetect::getModubleResultTitleList()
{
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("条码类型");
	modubleResultTitleList.push_back("条码内容");
	modubleResultTitleList.push_back("条码长度");
	modubleResultTitleList.push_back("X坐标");
	modubleResultTitleList.push_back("Y坐标");
	modubleResultTitleList.push_back("质量等级");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> BarCodeDetect::getModubleResultList()
{
	std::vector<std::map<int, std::string>> results;
	int count = mResultParams.hv_RetNum.I();
	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = mResultParams.hv_CodeType_Result[i].S().Text();
			result[index++] = mResultParams.hv_CodeContent[i].S().Text();
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_CodeLength_Result[i]).TupleReal().D(), 0);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Column[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_Row[i]).TupleReal().D(), 2);
			result[index++] = formatDobleValue(HTuple(mResultParams.hv_QualityGrade[i]).TupleReal().D(), 2);

			results.push_back(result);
		}
	}
	return results;
}
std::string BarCodeDetect::formatDobleValue(double val, int fixed)
{
	double val2 = val;
	if (val2 < 0) {
		val2 = -val2;
	}
	int intNum = 0;
	int temp = val2;
	while (temp) {
		temp /= 10;
		intNum++;
	}
	int intNum2 = 0;
	if (intNum == 0 && val2 > 0) {
		while ((int)val2 <= 0) {
			val2 *= 10;
			intNum2++;
		}
		if (intNum2 > 0)
			intNum2--;
	}

	std::ostringstream oss;
	oss << std::setprecision(fixed + intNum - intNum2) << val;
	return oss.str();
}

