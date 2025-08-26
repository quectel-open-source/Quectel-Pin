#include "QRCodeDetect.h"
QRCodeDetect::QRCodeDetect()
{
	////通用类指针对象实例化
	//m_Common = new Common();
}

QRCodeDetect::~QRCodeDetect()
{
	////释放通用类指针资源
	//delete m_Common;
}

//识别二维码
int QRCodeDetect::FindCode2dFunc(const HObject &ho_Image, const RunParamsStruct_Code2d &runParams, ResultParamsStruct_Code2d &resultParams)
{
	//mutex1.lock();
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
		GenEmptyObj(&resultParams.ho_ContourCode2d);
		GenEmptyObj(&resultParams.ho_RegionCode2d);

		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}

		CodeTypeStruct_Code2d Code2dTypeStruct;
		HObject mho_Image;
		//等级类别,每个值代表一个类别
		//二维码打印质量用一个包含十二个元素的元组来描述：
		//（整体质量、对比度、调制、固定图案损坏、解码、轴向不均匀性、网格不均匀性，未使用的纠错、反射裕度、打印增长、对比度均匀性、孔径）。
		HTuple hv_GRADE_Overall_Quality = 0;	//扫描反射率剖面等级,每条扫描线的扫描反射率轮廓等级的算术平均值
		HTuple hv_GRADE_Contrast = 1;	//反射率分布中最小值和最大值之间的范围。强烈的对比会带来好的评分。
		HTuple hv_GRADE_Modulation = 2;		//指示条形码元素的振幅有多强。大振幅使元素到条或空间的分配更加确定，从而产生高调制等级。
		HTuple hv_GRADE_Fixed_Pattern_Damage = 3;
		HTuple hv_GRADE_Decode = 4;	    //当符号的反射率分布可以根据符号体系的参考解码算法进行解码时，设置为4，否则设置为0
		HTuple hv_GRADE_Axial_Nonuniformity = 5;
		HTuple hv_GRADE_Grid_Nonuniformity = 6;
		HTuple hv_GRADE_Unused_Error_Correction = 7;
		HTuple hv_GRADE_Reflectance_Margin = 8;
		HTuple hv_GRADE_Print_Growth = 9;
		HTuple hv_GRADE_Contrast_Uniformity = 10;
		HTuple hv_GRADE_Aperture = 11;

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

		//需要识别的二维码类型数量
		int Code2dTypeNum = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		for (int j = 0; j < Code2dTypeNum; j++)
		{
			HTuple hv_DataCodeHandle;
			HTuple hv_ResultHandles;
			//创建二维码模型
			CreateDataCode2dModel(runParams.hv_CodeType_Run[j], HTuple(), HTuple(), &hv_DataCodeHandle);
			//设置参数，以便存储中间结果
			SetDataCode2dParam(hv_DataCodeHandle, "persistence", 1);
			//利用增强型读码模型(“standard_rerecognition”、“enhanced_egnition”、“maximum_recognity”将所有模型参数重置为标准、增强或最大三种基本默认设置之一)
			//注意：如果此参数与其他参数列表一起设置，则此参数必须位于第一个位置
			SetDataCode2dParam(hv_DataCodeHandle, "default_parameters", "enhanced_recognition");
			//读码超时时间ms
			SetDataCode2dParam(hv_DataCodeHandle, "timeout", runParams.hv_TimeOut.TupleInt());
			//******************以下参数不是所有类型的码都可以设置的，暂时屏蔽**********************************************************************************
			if (runParams.hv_CodeType_Run[j] == Code2dTypeStruct.DM || runParams.hv_CodeType_Run[j] == Code2dTypeStruct.Aztec)
			{
				if (runParams.hv_CodeType_Run[j] == Code2dTypeStruct.DM)
				{
					//在图案被遮挡或破损是，识别能力('low', 'high', 'any'),high会导致时间增长，识别率增加
					SetDataCode2dParam(hv_DataCodeHandle, "finder_pattern_tolerance", "any");
					//存在局部反光情况下，扫码算法('low'速度更快，鲁棒性不强, 'high'速度慢，鲁棒性强, 'any')
					SetDataCode2dParam(hv_DataCodeHandle, "contrast_tolerance", "any");
					//控制用于符号检测的候选区域的选择。 将此参数设置为'extensive'会增加生成的候选区域的数量，从而增加检测到代码的可能性。
					//但是，候选者的数量越多，运行时的时间就越长。 如果'candidate_selection'被设置为'default'，则会使用更少的候选区域。 在大多数情况下，这个较小的候选集合就足够了。
					SetDataCode2dParam(hv_DataCodeHandle, "candidate_selection", "extensive");
				}
				else
				{
					//在图案被遮挡或破损是，识别能力('low', 'high', 'any'),high会导致时间增长，识别率增加
					SetDataCode2dParam(hv_DataCodeHandle, "finder_pattern_tolerance", runParams.hv_Tolerance);
				}
			}
			//二维码背景之间极性，light_on_dark，dark_on_light，any
			SetDataCode2dParam(hv_DataCodeHandle, "polarity", "any");

			//二维码图像中模块的最小大小,最大大小(以像素为单位)
			//SetDataCode2dParam(hv_DataCodeHandle, (HTuple("module_size_min").Append("module_size_max")),(HTuple(6).Append(20)));
			//二维码图像中模块之间的间隙('no', 'yes', 'any')
			//SetDataCode2dParam(hv_DataCodeHandle, "module_gap", "no");
			//描述符号是否被镜像或可能被镜像('no', 'yes', 'any')
			SetDataCode2dParam(hv_DataCodeHandle, "mirrored", "any");

			//具有非常小的模块大小的数据码解码的鲁棒性。将参数“small_moduless_robustness”设置为“high”会增加解码模块大小非常小的数据代码的可能性。
			//此外，在这种情况下，还应相应地调整最小模块大小，因此“module_size_min”和“module_width_min”（PDF417）应分别设置为预期的最小模块大小和宽度。
			//将“small_modules_robustness”设置为“high”可以显著增加find_data_code_2d的内部内存使用量。因此，在默认情况下，
			//“small_modules_robustness”应设置为“low”。
			SetDataCode2dParam(hv_DataCodeHandle, "small_modules_robustness", "low");

			//描述模块的大小是否可能变化(在特定范围内)。 根据这个参数，不同的算法被用来计算模块的中心位置。 如果它被设置为'fixed'，则会使用一个等距网格。
			//允许可变模块大小('variable')，网格只与查找器模式的交替一侧对齐。
			//有了“any”，这两种方法都被一个接一个地测试。 请注意，如果'finder_pattern_tolerance'设置为'high'， 'module_grid'的值会被忽略。 在这种情况下，假设有一个等距网格。
			//SetDataCode2dParam(hv_DataCodeHandle, "module_grid", "any");
			//**********************************************************************************************************************************************************
			//读取二维码,设置读取数量
			HObject ho_CodeXLDsTemp;
			HTuple hv_CodeContentTemp;
			FindDataCode2d(mho_Image, &ho_CodeXLDsTemp, hv_DataCodeHandle, "stop_after_result_num", runParams.hv_CodeNum.TupleInt(), &hv_ResultHandles, &hv_CodeContentTemp);
			HTuple CodeXldCount;
			CountObj(ho_CodeXLDsTemp, &CodeXldCount);
			int CodeCount = CodeXldCount.I();
			if (CodeCount > 0)
			{
				int RetNumTemp = 0;
				HObject ho_CodeRegion, ho_CodeXLD;
				for (int i = 0; i < CodeCount; i++)
				{
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

					//计算条码等级
					HTuple hv_QualityLabels, hv_Quality, hv_QualityValues;
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415_labels", &hv_QualityLabels);
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415_values", &hv_QualityValues);
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415", &hv_Quality);
					hv_Quality = 4;
					
					//获取条码计算的中间结果：条码区域
					ho_CodeXLD.Clear();
					SelectObj(ho_CodeXLDsTemp, &ho_CodeXLD, i + 1);
					ConcatObj(resultParams.ho_ContourCode2d, ho_CodeXLD, &resultParams.ho_ContourCode2d);
					ho_CodeRegion.Clear();
					GenRegionContourXld(ho_CodeXLD, &ho_CodeRegion, "filled");
					ConcatObj(resultParams.ho_RegionCode2d, ho_CodeRegion, &resultParams.ho_RegionCode2d);
					HTuple hv_AreaTemp, hv_RowTemp, hv_ColumnTemp;
					AreaCenter(ho_CodeRegion, &hv_AreaTemp, &hv_RowTemp, &hv_ColumnTemp);

					resultParams.hv_Row.Append(hv_RowTemp);
					resultParams.hv_Column.Append(hv_ColumnTemp);
					resultParams.hv_CodeLength_Result.Append(CodeLengthTemp);
					resultParams.hv_CodeType_Result.Append(runParams.hv_CodeType_Run[j]);
					resultParams.hv_CodeContent.Append(hv_CodeContentTemp[i]);
					resultParams.hv_QualityGrade.Append(hv_Quality);
					RetNumTemp += 1;
				}
				ClearDataCode2dModel(hv_DataCodeHandle);
				hv_DataCodeHandle.Clear();
				resultParams.hv_RetNum += RetNumTemp;//找到的条码个数
			}
			else
			{
				ClearDataCode2dModel(hv_DataCodeHandle);
				hv_DataCodeHandle.Clear();
			}
		}

		if (resultParams.hv_RetNum > 0)
		{
			//mutex1.unlock();
			mResultParams = resultParams;
			return 0;
		}
		else
		{
			//mutex1.unlock();
			GenEmptyObj(&resultParams.ho_ContourCode2d);
			resultParams.ho_ContourCode2d.Clear();
			GenEmptyObj(&resultParams.ho_RegionCode2d);
			resultParams.ho_RegionCode2d.Clear();
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
		GenEmptyObj(&resultParams.ho_ContourCode2d);
		resultParams.ho_ContourCode2d.Clear();
		GenEmptyObj(&resultParams.ho_RegionCode2d);
		resultParams.ho_RegionCode2d.Clear();
		//mutex1.unlock();
		mResultParams = resultParams;
		return -1;
	}
}
//写日志函数
int QRCodeDetect::WriteTxt(string content)
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
vector<string> QRCodeDetect::get_current_time()
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

int QRCodeDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML读取参数
	ErrorCode_Xml errorCode = dataIOC.ReadParams_Code2d(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
std::vector<std::string> QRCodeDetect::getModubleResultTitleList()
{
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("二维码类型");
	modubleResultTitleList.push_back("二维码内容");
	modubleResultTitleList.push_back("二维码长度");
	modubleResultTitleList.push_back("X坐标");
	modubleResultTitleList.push_back("Y坐标");
	modubleResultTitleList.push_back("质量等级");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> QRCodeDetect::getModubleResultList()
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
std::string QRCodeDetect::formatDobleValue(double val, int fixed)
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

