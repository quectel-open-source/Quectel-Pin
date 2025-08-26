#include "DataIO.h"
DataIO::DataIO()
{
}
DataIO::~DataIO()
{
}
//路径设置
void DataIO::PathSet(const string ConfigPath, const string XmlPath)
{
	XmlClass2 xmlC;//xml存取变量

	// 判定目录或文件是否存在的标识符
	int mode = 0;
	if (_access(ConfigPath.c_str(), mode))
	{
		//system("mkdir head");
		_mkdir(ConfigPath.c_str());
	}

	//创建XML(存在就不会创建)
	int Ret = xmlC.CreateXML(XmlPath);
	if (Ret != 0)
	{
		WriteTxt("打开参数配置xml文件失败");
		//mutex1.unlock();
		return;
	}
}
//保存一维码检测参数
int DataIO::WriteParams_BarCode(const string ConfigPath, const string XmlPath, const RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//一维码参数保存
		//写入数组(数组按照逗号分割存储)
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", "auto");
			WriteTxt("一维码类型为空，默认保存为auto");
		}

		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", runParams.hv_CodeType_Run.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_CodeType_Run[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CodeType_Run[i];
				}
				else
				{
					typeTemp += runParams.hv_CodeType_Run[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", typeTemp.S().Text());
		}
		//保存其余参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", to_string(runParams.hv_CodeNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", runParams.hv_Tolerance.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", runParams.hv_HeadChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", runParams.hv_EndChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", runParams.hv_ContainChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", runParams.hv_NotContainChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", to_string(runParams.hv_CodeLength_Run.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(runParams.hv_TimeOut.TupleInt().I()));
		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}

		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("条码搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("一维码检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，一维码检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取一维码检测参数
ErrorCode_Xml DataIO::ReadParams_BarCode(const string ConfigPath, const string XmlPath, RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//一维码检测参数读取
		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_CodeType_Run失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CodeType_Run.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_CodeType_Run.Append(HTuple(typeArray[i].c_str()));
		}
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("一维码类型为空，读取一维码检测参数hv_CodeType_Run失败");
			return nullKeyName_Xml;
		}


		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_CodeNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_Tolerance失败");
			return errorCode;
		}
		runParams.hv_Tolerance = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_HeadChar失败");
			return errorCode;
		}
		runParams.hv_HeadChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_EndChar失败");
			return errorCode;
		}
		runParams.hv_EndChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_ContainChar失败");
			return errorCode;
		}
		runParams.hv_ContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_NotContainChar失败");
			return errorCode;
		}
		runParams.hv_NotContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_CodeLength_Run失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeLength_Run = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取一维码检测参数hv_TimeOut失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}


		WriteTxt("一维码检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，一维码检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存二维码检测参数
int DataIO::WriteParams_Code2d(const string ConfigPath, const string XmlPath, const RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//二维码参数保存
		//写入数组(数组按照逗号分割存储)
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", "QR Code");
			WriteTxt("二维码类型为空，默认保存QR Code");
		}

		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", runParams.hv_CodeType_Run.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_CodeType_Run[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CodeType_Run[i];
				}
				else
				{
					typeTemp += runParams.hv_CodeType_Run[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", typeTemp.S().Text());
		}
		//保存其余参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", to_string(runParams.hv_CodeNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", runParams.hv_Tolerance.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", runParams.hv_HeadChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", runParams.hv_EndChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", runParams.hv_ContainChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", runParams.hv_NotContainChar.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", to_string(runParams.hv_CodeLength_Run.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(runParams.hv_TimeOut.TupleInt().I()));
		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}
		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("二维码搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("二维码检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，二维码检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取二维码检测参数
ErrorCode_Xml DataIO::ReadParams_Code2d(const string ConfigPath, const string XmlPath, RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//二维码检测参数读取
		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_CodeType_Run失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CodeType_Run.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_CodeType_Run.Append(HTuple(typeArray[i].c_str()));
		}
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("二维码类型为空，读取二维码检测参数hv_CodeType_Run失败");
			return nullKeyName_Xml;
		}

		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_CodeNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_Tolerance失败");
			return errorCode;
		}
		runParams.hv_Tolerance = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_HeadChar失败");
			return errorCode;
		}
		runParams.hv_HeadChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_EndChar失败");
			return errorCode;
		}
		runParams.hv_EndChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_ContainChar失败");
			return errorCode;
		}
		runParams.hv_ContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_NotContainChar失败");
			return errorCode;
		}
		runParams.hv_NotContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_CodeLength_Run失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeLength_Run = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取二维码检测参数hv_TimeOut失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		WriteTxt("二维码检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃,二维码检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存模板检测参数(OK返回0，NG返回1)
int DataIO::WriteParams_Template(const string ConfigPath, const string XmlPath, const RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//训练参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", RunParams.hv_MatchMethod.S().Text());
		//判断tuple类型数据是不是数字
		HTuple hv_isNumber;
		TupleIsNumber(RunParams.hv_NumLevels, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", to_string(RunParams.hv_NumLevels.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", RunParams.hv_NumLevels.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(RunParams.hv_AngleStart.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(RunParams.hv_AngleExtent.TupleReal().D()));
		TupleIsNumber(RunParams.hv_AngleStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", to_string(RunParams.hv_AngleStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", RunParams.hv_AngleStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(RunParams.hv_ScaleRMin.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(RunParams.hv_ScaleRMax.TupleReal().D()));
		TupleIsNumber(RunParams.hv_ScaleRStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", to_string(RunParams.hv_ScaleRStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", RunParams.hv_ScaleRStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(RunParams.hv_ScaleCMin.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(RunParams.hv_ScaleCMax.TupleReal().D()));
		TupleIsNumber(RunParams.hv_ScaleCStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", to_string(RunParams.hv_ScaleCStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", RunParams.hv_ScaleCStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Optimization", RunParams.hv_Optimization.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", RunParams.hv_Metric.S().Text());
		TupleIsNumber(RunParams.hv_Contrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", to_string(RunParams.hv_Contrast.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", RunParams.hv_Contrast.S().Text());
		}
		TupleIsNumber(RunParams.hv_MinContrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", to_string(RunParams.hv_MinContrast.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", RunParams.hv_MinContrast.S().Text());
		}

		//保存模板原始坐标和角度
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", to_string(RunParams.hv_Angle_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", to_string(RunParams.hv_CenterRow_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", to_string(RunParams.hv_CenterCol_Original.TupleReal().D()));
		//保存运行参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(RunParams.hv_MinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(RunParams.hv_NumMatches.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(RunParams.hv_MaxOverlap.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", RunParams.hv_SubPixel.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(RunParams.hv_Greediness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(RunParams.hv_TimeOut.TupleInt().I()));


		//保存训练区域
		if (RunParams.ho_TrainRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(RunParams.ho_TrainRegion, PathTrainRegion);
		}
		//保存训练小图
		if (RunParams.ho_CropModelImg.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("训练图片为空，保存失败");
		}
		else
		{
			HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CropModelImg.bmp";
			WriteImage(RunParams.ho_CropModelImg, "bmp", 0, PathTrainImg);
		}
		//保存运行区域
		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("模板搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRegion, PathRunRegion);
		}

		//保存训练好的模板ID模型
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			WriteNccModel(RunParams.hv_ModelID, modelIDPath);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			WriteShapeModel(RunParams.hv_ModelID, modelIDPath);
		}
		WriteTxt("模板检测参数保存成功!");
		//mutex1.unlock();
		return 0;

	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，模板检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//加载模板检测参数(OK返回0，NG返回1)
ErrorCode_Xml DataIO::ReadParams_Template(const string ConfigPath, const string XmlPath, RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//读取训练参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MatchMethod失败");
			return errorCode;
		}
		RunParams.hv_MatchMethod = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumLevels失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_NumLevels = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_NumLevels = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleStart失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleExtent失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_AngleStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_AngleStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_ScaleRStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_ScaleRStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_ScaleCStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_ScaleCStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Optimization", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Optimization失败");
			return errorCode;
		}
		RunParams.hv_Optimization = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Metric失败");
			return errorCode;
		}
		RunParams.hv_Metric = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Contrast失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_Contrast = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_Contrast = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MinContrast失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_MinContrast = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_MinContrast = tempIntValue;
		}

		//读取模板原始坐标和角度
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Angle_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Angle_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_CenterRow_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterRow_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_CenterCol_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterCol_Original = tempDoubleValue;

		//读取运行参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumMatches失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MaxOverlap失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_SubPixel失败");
			return errorCode;
		}
		RunParams.hv_SubPixel = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Greediness失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_TimeOut失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_TimeOut = tempIntValue;

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_SearchRegion);
			ReadRegion(&RunParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_SearchRegion);
			RunParams.ho_SearchRegion.Clear();
		}

		//训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_TrainRegion);
			ReadRegion(&RunParams.ho_TrainRegion, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_TrainRegion);
			RunParams.ho_TrainRegion.Clear();
		}
		//读取训练小图
		HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CropModelImg.bmp";
		FileExists(PathTrainImg, &isExist);
		if (isExist > 0)
		{
			ReadImage(&RunParams.ho_CropModelImg, PathTrainImg);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_CropModelImg);
			RunParams.ho_CropModelImg.Clear();
		}

		//读取本地模板模型
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			ReadNccModel(modelIDPath, &RunParams.hv_ModelID);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			string str = modelIDPath.S().Text();
			ReadShapeModel(modelIDPath, &RunParams.hv_ModelID);
		}

		WriteTxt("模板检测参数加载成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，模板检测参数加载失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存差分算法参数(OK返回0，NG返回1)
int DataIO::WriteParams_ImgDifference(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//训练参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", RunParams.hv_MatchMethod.S().Text());
		//判断tuple类型数据是不是数字
		HTuple hv_isNumber;
		TupleIsNumber(RunParams.hv_NumLevels, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", to_string(RunParams.hv_NumLevels.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", RunParams.hv_NumLevels.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(RunParams.hv_AngleStart.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(RunParams.hv_AngleExtent.TupleReal().D()));
		TupleIsNumber(RunParams.hv_AngleStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", to_string(RunParams.hv_AngleStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", RunParams.hv_AngleStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(RunParams.hv_ScaleRMin.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(RunParams.hv_ScaleRMax.TupleReal().D()));
		TupleIsNumber(RunParams.hv_ScaleRStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", to_string(RunParams.hv_ScaleRStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", RunParams.hv_ScaleRStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(RunParams.hv_ScaleCMin.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(RunParams.hv_ScaleCMax.TupleReal().D()));
		TupleIsNumber(RunParams.hv_ScaleCStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", to_string(RunParams.hv_ScaleCStep.TupleReal().D()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", RunParams.hv_ScaleCStep.S().Text());
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Optimization", RunParams.hv_Optimization.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", RunParams.hv_Metric.S().Text());
		TupleIsNumber(RunParams.hv_Contrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", to_string(RunParams.hv_Contrast.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", RunParams.hv_Contrast.S().Text());
		}
		TupleIsNumber(RunParams.hv_MinContrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", to_string(RunParams.hv_MinContrast.TupleInt().I()));
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", RunParams.hv_MinContrast.S().Text());
		}

		//保存模板原始坐标和角度
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", to_string(RunParams.hv_Angle_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", to_string(RunParams.hv_CenterRow_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", to_string(RunParams.hv_CenterCol_Original.TupleReal().D()));
		//保存运行参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(RunParams.hv_MinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(RunParams.hv_NumMatches.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(RunParams.hv_MaxOverlap.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", RunParams.hv_SubPixel.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(RunParams.hv_Greediness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(RunParams.hv_TimeOut.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", to_string(RunParams.hv_IsUsSearchRoi.TupleInt().I()));
		//保存训练原图
		if (RunParams.ho_ModelImg.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("训练图片为空，保存失败");
			return 1;
		}
		else
		{
			HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelImg.bmp";
			WriteImage(RunParams.ho_ModelImg, "bmp", 0, PathTrainImg);
		}
		//保存训练区域
		if (RunParams.ho_TrainRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(RunParams.ho_TrainRegion, PathTrainRegion);
		}
		//保存运行区域
		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("模板搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRegion, PathRunRegion);
		}

		//保存训练好的模板ID模型
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			WriteNccModel(RunParams.hv_ModelID, modelIDPath);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			WriteShapeModel(RunParams.hv_ModelID, modelIDPath);
		}
		
		//保存差分参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseGauss", to_string(RunParams.hv_IsUseGauss.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(RunParams.hv_Sigma.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Mult", to_string(RunParams.hv_Mult.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Add", to_string(RunParams.hv_Add.TupleInt().I()));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMin", to_string(RunParams.hv_FilterGrayMin.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMax", to_string(RunParams.hv_FilterGrayMax.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMin", to_string(RunParams.hv_FilterAreaMin.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMax", to_string(RunParams.hv_FilterAreaMax.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMin ", to_string(RunParams.hv_FilterWidthMin.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMax ", to_string(RunParams.hv_FilterWidthMax.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMin", to_string(RunParams.hv_FilterHeightMin.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMax", to_string(RunParams.hv_FilterHeightMax.TupleInt().I()));

		//mutex1.unlock();
		return 0;

	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，模板检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//加载差分算法参数(OK返回0，NG返回1)
ErrorCode_Xml DataIO::ReadParams_ImgDifference(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//读取训练参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MatchMethod失败");
			return errorCode;
		}
		RunParams.hv_MatchMethod = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumLevels失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_NumLevels = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_NumLevels = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleStart失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleExtent失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_AngleStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_AngleStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_ScaleRStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_ScaleRStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCStep失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_ScaleCStep = "auto";
		}
		else
		{
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			RunParams.hv_ScaleCStep = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Optimization", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Optimization失败");
			return errorCode;
		}
		RunParams.hv_Optimization = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Metric失败");
			return errorCode;
		}
		RunParams.hv_Metric = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Contrast失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_Contrast = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_Contrast = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MinContrast失败");
			return errorCode;
		}
		if (valueXml == "auto")
		{
			RunParams.hv_MinContrast = "auto";
		}
		else
		{
			xmlC.StrToInt(valueXml, tempIntValue);
			RunParams.hv_MinContrast = tempIntValue;
		}

		//读取模板原始坐标和角度
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Angle_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Angle_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_CenterRow_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterRow_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_CenterCol_Original失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterCol_Original = tempDoubleValue;

		//读取运行参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumMatches失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MaxOverlap失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_SubPixel失败");
			return errorCode;
		}
		RunParams.hv_SubPixel = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Greediness失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_TimeOut失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_IsUsSearchRoi失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUsSearchRoi = tempIntValue;

		//读取训练原图
		HTuple isExist;
		HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelImg.bmp";
		FileExists(PathTrainImg, &isExist);
		if (isExist > 0)
		{
			ReadImage(&RunParams.ho_ModelImg, PathTrainImg);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_ModelImg);
			RunParams.ho_ModelImg.Clear();
			return nullNode_Xml;
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_SearchRegion);
			ReadRegion(&RunParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_SearchRegion);
			RunParams.ho_SearchRegion.Clear();
		}

		//训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_TrainRegion);
			ReadRegion(&RunParams.ho_TrainRegion, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_TrainRegion);
			RunParams.ho_TrainRegion.Clear();
		}

		//读取本地模板模型
		if (RunParams.hv_MatchMethod.S() == "auto_ncc" || RunParams.hv_MatchMethod.S() == "ncc")
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.ncm";
			ReadNccModel(modelIDPath, &RunParams.hv_ModelID);
		}
		else
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelId.shm";
			ReadShapeModel(modelIDPath, &RunParams.hv_ModelID);
		}

		//读取差分参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseGauss", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_IsUseGauss失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseGauss = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Sigma失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Sigma = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Mult", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Mult失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Mult = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Add", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Add失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Add = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterGrayMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterGrayMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterGrayMax失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterGrayMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterAreaMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterAreaMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterAreaMax失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterAreaMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterWidthMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterWidthMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterWidthMax失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterWidthMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterHeightMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterHeightMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterHeightMax失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterHeightMax = tempIntValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，模板检测参数加载失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存直线检测参数
int DataIO::WriteParams_Line(const string ConfigPath, const string XmlPath, const RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//找边参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1", to_string(runParams.hv_Row1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1", to_string(runParams.hv_Column1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2", to_string(runParams.hv_Row2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2", to_string(runParams.hv_Column2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", to_string(runParams.hv_InstancesNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", runParams.hv_MeasureInterpolation.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.TupleReal().D()));

		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}

		WriteTxt("直线检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，直线检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取直线检测参数
ErrorCode_Xml DataIO::ReadParams_Line(const string ConfigPath, const string XmlPath, RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//找边参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_Row1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_Column1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_Row2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_Column2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureLength1失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureLength2失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureTransition失败");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureSelect失败");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_InstancesNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureMinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_DistanceThreshold失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MeasureInterpolation失败");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数hv_MmPixel失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}

		WriteTxt("直线检测参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，直线检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存圆检测参数
int DataIO::WriteParams_Circle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//找圆参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row", to_string(runParams.hv_Row.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column", to_string(runParams.hv_Column.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius", to_string(runParams.hv_Radius.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", to_string(runParams.hv_InstancesNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", runParams.hv_MeasureInterpolation.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DectType", to_string(runParams.hv_DectType.TupleInt().I()));

		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}

		WriteTxt("圆检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，圆检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取圆检测参数
ErrorCode_Xml DataIO::ReadParams_Circle(const string ConfigPath, const string XmlPath, RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//找圆参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_Row失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_Column失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_Radius失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureLength1失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureLength2失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureTransition失败");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureSelect失败");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_InstancesNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureMinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_DistanceThreshold失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MeasureInterpolation失败");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_MmPixel失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数hv_DectType失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_DectType = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取圆检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}

		WriteTxt("圆检测参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，圆检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存矩形检测参数
int DataIO::WriteParams_Rectangle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//找矩形参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row", to_string(runParams.hv_Row.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column", to_string(runParams.hv_Column.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", to_string(runParams.hv_Angle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Length1", to_string(runParams.hv_Length1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Length2", to_string(runParams.hv_Length2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", to_string(runParams.hv_InstancesNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", runParams.hv_MeasureInterpolation.S().Text());

		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}

		WriteTxt("矩形检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，矩形检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取矩形检测参数
ErrorCode_Xml DataIO::ReadParams_Rectangle(const string ConfigPath, const string XmlPath, RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//找矩形参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_Row失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_Column失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_Angle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_Length1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_Length2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureLength1失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureLength2失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureTransition失败");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureSelect失败");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_InstancesNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureMinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_DistanceThreshold失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数hv_MeasureInterpolation失败");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取矩形检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}


		WriteTxt("矩形检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，矩形检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存椭圆检测参数
int DataIO::WriteParams_Ellipse(const string ConfigPath, const string XmlPath, const RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//找椭圆参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row", to_string(runParams.hv_Row.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column", to_string(runParams.hv_Column.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", to_string(runParams.hv_Angle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1", to_string(runParams.hv_Radius1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2", to_string(runParams.hv_Radius2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", to_string(runParams.hv_InstancesNum.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", runParams.hv_MeasureInterpolation.S().Text());

		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}

		WriteTxt("椭圆检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，椭圆检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取椭圆检测参数
ErrorCode_Xml DataIO::ReadParams_Ellipse(const string ConfigPath, const string XmlPath, RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//找椭圆参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_Row失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_Column失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_Angle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_Radius1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_Radius2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureLength1失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureLength2失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureTransition失败");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureSelect失败");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_InstancesNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureMinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_DistanceThreshold失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数hv_MeasureInterpolation失败");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取椭圆检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		WriteTxt("椭圆检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，椭圆检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存二值化参数
int DataIO::WriteParams_Binarization(const string ConfigPath, const string XmlPath, const RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}
		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("二值化搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}
		WriteTxt("二值化参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，二值化参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取二值化参数
ErrorCode_Xml DataIO::ReadParams_Binarization(const string ConfigPath, const string XmlPath, RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//检测参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_ThresholdType失败");
			return errorCode;
		}
		runParams.hv_ThresholdType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_LowThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_LowThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_HighThreshold失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_HighThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_Sigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_CoreWidth失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreWidth = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_CoreHeight失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreHeight = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_DynThresholdContrast失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_DynThresholdContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Blob二值化检测参数hv_DynThresholdPolarity失败");
			return errorCode;
		}
		runParams.hv_DynThresholdPolarity = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		WriteTxt("二值化参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，二值化参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存形态学参数
int DataIO::WriteParams_Morphology(const string ConfigPath, const string XmlPath, const RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//写入数组(数组按照逗号分割存储)
		//*******************************区域操作****************************************************************
		//写入区域操作类型(膨胀，腐蚀等等)
		int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_RegionOperator_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", "null");
			WriteTxt("Blob区域操作类型为空,保存字符串null");
		}
		else
		{
			//写入区域操作类型(膨胀，腐蚀等等)
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", runParams.hv_RegionOperator_Type.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_RegionOperator_Type[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_RegionOperator_Type[i];
					}
					else
					{
						typeTemp += runParams.hv_RegionOperator_Type[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", typeTemp.S().Text());
			}

			//写入滤波核的宽
			typeCount = runParams.hv_Width_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", to_string(runParams.hv_Width_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Width_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Width_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_Width_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", typeTemp.S().Text());
				}
			}


			//写入滤波核的高
			typeCount = runParams.hv_Height_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", to_string(runParams.hv_Height_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Height_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Height_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_Height_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", typeTemp.S().Text());
				}
			}


			//写入圆形结构元素的半径
			typeCount = runParams.hv_CircleRadius_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", to_string(runParams.hv_CircleRadius_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_CircleRadius_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_CircleRadius_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_CircleRadius_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", typeTemp.S().Text());
				}
			}



			//写入孔洞填充最小面积
			typeCount = runParams.hv_MinArea_FillUpShape_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", to_string(runParams.hv_MinArea_FillUpShape_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MinArea_FillUpShape_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MinArea_FillUpShape_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_MinArea_FillUpShape_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", typeTemp.S().Text());
				}
			}




			//写入孔洞填充最大面积
			typeCount = runParams.hv_MaxArea_FillUpShape_Operator.TupleLength().TupleReal().D();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", to_string(runParams.hv_MaxArea_FillUpShape_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MaxArea_FillUpShape_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MaxArea_FillUpShape_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_MaxArea_FillUpShape_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", typeTemp.S().Text());
				}
			}


			if (typeNum > 0)
			{
				WriteTxt("Blob区域操作参数保存成功");
			}
		}

		//****************************区域筛选****************************************************************
		typeNum = 0;
		//写入区域筛选类型
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_FeaturesFilter_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
			WriteTxt("Blob区域筛选类型为空,保存字符串null");
		}
		else
		{
			//写入区域筛选类型(面积，圆度等等)
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", runParams.hv_FeaturesFilter_Type.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_FeaturesFilter_Type[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i];
					}
					else
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", typeTemp.S().Text());
			}

			//写入筛选参数最小值
			typeCount = runParams.hv_MinValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", to_string(runParams.hv_MinValue_Filter.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MinValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MinValue_Filter[i];
						}
						else
						{
							typeTemp += runParams.hv_MinValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", typeTemp.S().Text());
				}
			}


			//写入筛选参数最大值
			typeCount = runParams.hv_MaxValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", to_string(runParams.hv_MaxValue_Filter.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MaxValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MaxValue_Filter[i];
						}
						else
						{
							typeTemp += runParams.hv_MaxValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", typeTemp.S().Text());
				}
			}


			//写入与或
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", runParams.hv_And_Or_Filter.S().Text());

			if (typeNum > 0)
			{
				WriteTxt("Blob形态学区域筛选参数保存成功!");
			}
		}

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，Blob形态学参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取形态学参数
ErrorCode_Xml DataIO::ReadParams_Morphology(const string ConfigPath, const string XmlPath, RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//形态学参数读取
		//数组需要清空
		runParams.hv_RegionOperator_Type.Clear();
		runParams.hv_Height_Operator.Clear();
		runParams.hv_Width_Operator.Clear();
		runParams.hv_CircleRadius_Operator.Clear();
		runParams.hv_MinArea_FillUpShape_Operator.Clear();
		runParams.hv_MaxArea_FillUpShape_Operator.Clear();
		runParams.hv_FeaturesFilter_Type.Clear();
		runParams.hv_MinValue_Filter.Clear();
		runParams.hv_MaxValue_Filter.Clear();

		//读取区域操作类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			WriteTxt("XML读取形态学检测参数hv_RegionOperator_Type失败");
			runParams.hv_RegionOperator_Type.Clear();
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			runParams.hv_RegionOperator_Type.Clear();
			TypeStruct_RegionOperator_MorphologyDetect codeTypeStruct;
			for (int i = 0; i < typeArray.size(); i++)
			{
				if (typeArray[i] == "矩形膨胀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Rectangle);
				}
				else if (typeArray[i] == "圆形膨胀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Circle);
				}
				else if (typeArray[i] == "矩形腐蚀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Rectangle);
				}
				else if (typeArray[i] == "圆形腐蚀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Circle);
				}
				else if (typeArray[i] == "矩形开运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Rectangle);
				}
				else if (typeArray[i] == "圆形开运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Circle);
				}
				else if (typeArray[i] == "矩形闭运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Rectangle);
				}
				else if (typeArray[i] == "圆形闭运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Circle);
				}
				else if (typeArray[i] == "底帽运算矩形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Rectangle);
				}
				else if (typeArray[i] == "底帽运算圆形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Circle);
				}
				else if (typeArray[i] == "顶帽运算矩形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Rectangle);
				}
				else if (typeArray[i] == "顶帽运算圆形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Circle);
				}
				else if (typeArray[i] == "孔洞全部填充")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Fillup);
				}
				else if (typeArray[i] == "孔洞面积填充")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_FillUp_Shape);
				}
				else if (typeArray[i] == "计算连通域")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Connection);
				}
				else if (typeArray[i] == "所有区域合并")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union1);
				}
				else if (typeArray[i] == "两个区域合并")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union2);
				}
				else if (typeArray[i] == "补集")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Complement);
				}
				else if (typeArray[i] == "差集")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Difference);
				}
				else if (typeArray[i] == "交集")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Intersection);
				}
				else if (typeArray[i] == "和集")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_ConcatObj);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("Blob区域操作类型为空，参数读取失败");
			}
			else
			{
				int isContainRectangle = 0;
				int isContainCircle = 0;
				int isContainFillUpShape = 0;
				for (int i = 0; i < typeCount; i++)
				{
					if (runParams.hv_RegionOperator_Type[i] == "矩形膨胀" || runParams.hv_RegionOperator_Type[i] == "矩形腐蚀" ||
						runParams.hv_RegionOperator_Type[i] == "矩形开运算" || runParams.hv_RegionOperator_Type[i] == "矩形闭运算" ||
						runParams.hv_RegionOperator_Type[i] == "底帽运算矩形结构体" || runParams.hv_RegionOperator_Type[i] == "顶帽运算矩形结构体")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "圆形膨胀" || runParams.hv_RegionOperator_Type[i] == "圆形腐蚀" ||
						runParams.hv_RegionOperator_Type[i] == "圆形开运算" || runParams.hv_RegionOperator_Type[i] == "圆形闭运算" ||
						runParams.hv_RegionOperator_Type[i] == "底帽运算圆形结构体" || runParams.hv_RegionOperator_Type[i] == "顶帽运算圆形结构体")
					{
						isContainCircle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "孔洞面积填充")
					{
						isContainFillUpShape += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//读取滤波核宽度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_Width_Operator失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Width_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Width_Operator.Append(tempDoubleValue);
						}
					}

					//读取滤波核高度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_Height_Operator失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Height_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Height_Operator.Append(tempDoubleValue);
						}
					}

				}
				if (isContainCircle > 0)
				{
					//读取圆形结构元素的半径
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_CircleRadius_Operator失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_CircleRadius_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_CircleRadius_Operator.Append(tempDoubleValue);
						}
					}

				}
				if (isContainFillUpShape > 0)
				{
					//读取填充最小面积
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_MinArea_FillUpShape_Operator失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MinArea_FillUpShape_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MinArea_FillUpShape_Operator.Append(tempDoubleValue);
						}
					}

					//读取填充最大面积
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_MaxArea_FillUpShape_Operator失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MaxArea_FillUpShape_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MaxArea_FillUpShape_Operator.Append(tempDoubleValue);
						}
					}
					if (typeNum > 0)
					{
						WriteTxt("Blob区域操作类型参数读取成功");
					}
				}
			}
		}
		else
		{
			runParams.hv_RegionOperator_Type.Clear();
		}

		//读取区域筛选类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			WriteTxt("XML读取形态学检测参数hv_FeaturesFilter_Type失败");
			runParams.hv_FeaturesFilter_Type.Clear();
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			runParams.hv_FeaturesFilter_Type.Clear();
			TypeStruct_FeaturesFilter_MorphologyDetect codeTypeStruct;
			for (int i = 0; i < typeArray.size(); i++)
			{
				if (typeArray[i] == "面积")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Area_Filter);
				}
				if (typeArray[i] == "X")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Column_Filter);
				}
				if (typeArray[i] == "Y")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Row_Filter);
				}
				if (typeArray[i] == "角度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Angle_Filter);
				}
				if (typeArray[i] == "圆度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Circularity_Filter);
				}
				if (typeArray[i] == "紧密度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Compactness_Filter);
				}
				if (typeArray[i] == "凸度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Convexity_Filter);
				}
				if (typeArray[i] == "矩形度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rectangularity_Filter);
				}
				if (typeArray[i] == "高度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Height_Filter);
				}
				if (typeArray[i] == "宽度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Width_Filter);
				}
				if (typeArray[i] == "最小外接圆半径")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_OuterRadius_Filter);
				}
				if (typeArray[i] == "最大内接圆半径")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_InnerRadius_Filter);
				}
				if (typeArray[i] == "最小外接矩半长")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len1_Filter);
				}
				if (typeArray[i] == "最小外接矩半宽")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len2_Filter);
				}
				if (typeArray[i] == "最小外接矩角度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Angle_Filter);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("Blob区域筛选类型为空，参数读取失败");
			}
			else
			{
				//读取筛选参数最小值
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML读取形态学检测参数hv_MinValue_Filter失败");
					return errorCode;
				}
				if (valueXml != "null")
				{
					vector<string> typeArray;
					stringToken(valueXml, ",", typeArray);
					runParams.hv_MinValue_Filter.Clear();
					for (int i = 0; i < typeArray.size(); i++)
					{
						xmlC.StrToDouble(typeArray[i], tempDoubleValue);
						runParams.hv_MinValue_Filter.Append(tempDoubleValue);
					}
				}

				//读取筛选参数最大值
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML读取形态学检测参数hv_MaxValue_Filter失败");
					return errorCode;
				}
				if (valueXml != "null")
				{
					vector<string> typeArray;
					stringToken(valueXml, ",", typeArray);
					runParams.hv_MaxValue_Filter.Clear();
					for (int i = 0; i < typeArray.size(); i++)
					{
						xmlC.StrToDouble(typeArray[i], tempDoubleValue);
						runParams.hv_MaxValue_Filter.Append(tempDoubleValue);
					}
				}

				//读取与或
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML读取形态学检测参数hv_And_Or_Filter失败");
					return errorCode;
				}
				runParams.hv_And_Or_Filter = HTuple(valueXml.c_str());

				if (typeNum > 0)
				{
					WriteTxt("Blob区域筛选类型参数读取成功");
				}
			}
		}
		else
		{
			runParams.hv_FeaturesFilter_Type.Clear();
		}
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，Blob形态学参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存Blob参数
int DataIO::WriteParams_Blob(const string ConfigPath, const string XmlPath, const RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SetBlobCount", to_string(runParams.hv_SetBlobCount.TupleInt().I()));
		//二值化参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", to_string(runParams.hv_IsUsSearchRoi.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		int typeNum = 0;
		//写入数组(数组按照逗号分割存储)
		//*******************************区域操作****************************************************************
		//写入区域操作类型(膨胀，腐蚀等等)
		int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_RegionOperator_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", "null");
		}
		else
		{
			//写入区域操作类型(膨胀，腐蚀等等)
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", runParams.hv_RegionOperator_Type.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_RegionOperator_Type[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_RegionOperator_Type[i];
					}
					else
					{
						typeTemp += runParams.hv_RegionOperator_Type[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", typeTemp.S().Text());
			}

			//写入滤波核的宽
			typeCount = runParams.hv_Width_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", to_string(runParams.hv_Width_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Width_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Width_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_Width_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", typeTemp.S().Text());
				}
			}


			//写入滤波核的高
			typeCount = runParams.hv_Height_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", to_string(runParams.hv_Height_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Height_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Height_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_Height_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", typeTemp.S().Text());
				}
			}


			//写入圆形结构元素的半径
			typeCount = runParams.hv_CircleRadius_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", to_string(runParams.hv_CircleRadius_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_CircleRadius_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_CircleRadius_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_CircleRadius_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", typeTemp.S().Text());
				}
			}



			//写入孔洞填充最小面积
			typeCount = runParams.hv_MinArea_FillUpShape_Operator.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", to_string(runParams.hv_MinArea_FillUpShape_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MinArea_FillUpShape_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MinArea_FillUpShape_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_MinArea_FillUpShape_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", typeTemp.S().Text());
				}
			}




			//写入孔洞填充最大面积
			typeCount = runParams.hv_MaxArea_FillUpShape_Operator.TupleLength().TupleReal().D();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", to_string(runParams.hv_MaxArea_FillUpShape_Operator.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MaxArea_FillUpShape_Operator[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MaxArea_FillUpShape_Operator[i];
						}
						else
						{
							typeTemp += runParams.hv_MaxArea_FillUpShape_Operator[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", typeTemp.S().Text());
				}
			}
		}

		//****************************区域筛选****************************************************************
		typeNum = 0;
		//写入区域筛选类型
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_FeaturesFilter_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
		}
		else
		{
			//写入区域筛选类型(面积，圆度等等)
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", runParams.hv_FeaturesFilter_Type.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_FeaturesFilter_Type[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i];
					}
					else
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", typeTemp.S().Text());
			}

			//写入筛选参数最小值
			typeCount = runParams.hv_MinValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", to_string(runParams.hv_MinValue_Filter.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MinValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MinValue_Filter[i];
						}
						else
						{
							typeTemp += runParams.hv_MinValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", typeTemp.S().Text());
				}
			}


			//写入筛选参数最大值
			typeCount = runParams.hv_MaxValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", to_string(runParams.hv_MaxValue_Filter.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_MaxValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_MaxValue_Filter[i];
						}
						else
						{
							typeTemp += runParams.hv_MaxValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", typeTemp.S().Text());
				}
			}


			//写入与或
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", runParams.hv_And_Or_Filter.S().Text());
		}

		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", to_string(runParams.hv_IsUsSearchRoi.TupleInt().I()));
		//保存运行区域
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P1x_SearchRoi", to_string(runParams.P1_SearchRoi.x));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P1y_SearchRoi", to_string(runParams.P1_SearchRoi.y));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P2x_SearchRoi", to_string(runParams.P2_SearchRoi.x));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "P2y_SearchRoi", to_string(runParams.P2_SearchRoi.y));
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		return -1;
	}
}
//读取Blob参数
ErrorCode_Xml DataIO::ReadParams_Blob(const string ConfigPath, const string XmlPath, RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SetBlobCount", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_SetBlobCount = tempIntValue;

		//二值化参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		runParams.hv_ThresholdType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_LowThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_HighThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreWidth = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreHeight = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_DynThresholdContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		runParams.hv_DynThresholdPolarity = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsUsSearchRoi = tempIntValue;

		//读取运行区域
		int p1x, p1y, p2x, p2y;
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P1x_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p1x = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P1y_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p1y = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P2x_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p2x = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "P2y_SearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		p2y = tempIntValue;

		runParams.P1_SearchRoi.x = p1x;
		runParams.P1_SearchRoi.y = p1y;
		runParams.P2_SearchRoi.x = p2x;
		runParams.P2_SearchRoi.y = p2y;

		//形态学参数读取
		runParams.hv_RegionOperator_Type.Clear();
		runParams.hv_Height_Operator.Clear();
		runParams.hv_Width_Operator.Clear();
		runParams.hv_CircleRadius_Operator.Clear();
		runParams.hv_MinArea_FillUpShape_Operator.Clear();
		runParams.hv_MaxArea_FillUpShape_Operator.Clear();
		runParams.hv_FeaturesFilter_Type.Clear();
		runParams.hv_MinValue_Filter.Clear();
		runParams.hv_MaxValue_Filter.Clear();

		//读取区域操作类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			runParams.hv_RegionOperator_Type.Clear();
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			runParams.hv_RegionOperator_Type.Clear();
			TypeStruct_RegionOperator_Blob codeTypeStruct;
			for (int i = 0; i < typeArray.size(); i++)
			{
				if (typeArray[i] == "矩形膨胀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Rectangle);
				}
				else if (typeArray[i] == "圆形膨胀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Circle);
				}
				else if (typeArray[i] == "矩形腐蚀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Rectangle);
				}
				else if (typeArray[i] == "圆形腐蚀")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Circle);
				}
				else if (typeArray[i] == "矩形开运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Rectangle);
				}
				else if (typeArray[i] == "圆形开运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Circle);
				}
				else if (typeArray[i] == "矩形闭运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Rectangle);
				}
				else if (typeArray[i] == "圆形闭运算")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Circle);
				}
				else if (typeArray[i] == "底帽运算矩形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Rectangle);
				}
				else if (typeArray[i] == "底帽运算圆形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Circle);
				}
				else if (typeArray[i] == "顶帽运算矩形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Rectangle);
				}
				else if (typeArray[i] == "顶帽运算圆形结构体")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Circle);
				}
				else if (typeArray[i] == "孔洞全部填充")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Fillup);
				}
				else if (typeArray[i] == "孔洞面积填充")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_FillUp_Shape);
				}
				else if (typeArray[i] == "计算连通域")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Connection);
				}
				else if (typeArray[i] == "所有区域合并")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union1);
				}
				else if (typeArray[i] == "补集")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Complement);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount > 0)
			{
				int isContainRectangle = 0;
				int isContainCircle = 0;
				int isContainFillUpShape = 0;
				for (int i = 0; i < typeCount; i++)
				{
					if (runParams.hv_RegionOperator_Type[i] == "矩形膨胀" || runParams.hv_RegionOperator_Type[i] == "矩形腐蚀" ||
						runParams.hv_RegionOperator_Type[i] == "矩形开运算" || runParams.hv_RegionOperator_Type[i] == "矩形闭运算" ||
						runParams.hv_RegionOperator_Type[i] == "底帽运算矩形结构体" || runParams.hv_RegionOperator_Type[i] == "顶帽运算矩形结构体")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "圆形膨胀" || runParams.hv_RegionOperator_Type[i] == "圆形腐蚀" ||
						runParams.hv_RegionOperator_Type[i] == "圆形开运算" || runParams.hv_RegionOperator_Type[i] == "圆形闭运算" ||
						runParams.hv_RegionOperator_Type[i] == "底帽运算圆形结构体" || runParams.hv_RegionOperator_Type[i] == "顶帽运算圆形结构体")
					{
						isContainCircle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "孔洞面积填充")
					{
						isContainFillUpShape += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//读取滤波核宽度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Width_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Width_Operator.Append(tempDoubleValue);
						}
					}

					//读取滤波核高度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Height_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Height_Operator.Append(tempDoubleValue);
						}
					}

				}
				if (isContainCircle > 0)
				{
					//读取圆形结构元素的半径
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_CircleRadius_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_CircleRadius_Operator.Append(tempDoubleValue);
						}
					}

				}
				if (isContainFillUpShape > 0)
				{
					//读取填充最小面积
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MinArea_FillUpShape_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MinArea_FillUpShape_Operator.Append(tempDoubleValue);
						}
					}

					//读取填充最大面积
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MaxArea_FillUpShape_Operator.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MaxArea_FillUpShape_Operator.Append(tempDoubleValue);
						}
					}
				}
			}
		}
		else
		{
			runParams.hv_RegionOperator_Type.Clear();
		}

		//读取区域筛选类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			runParams.hv_FeaturesFilter_Type.Clear();
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			runParams.hv_FeaturesFilter_Type.Clear();
			TypeStruct_FeaturesFilter_Blob codeTypeStruct;
			for (int i = 0; i < typeArray.size(); i++)
			{
				if (typeArray[i] == "面积")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Area_Filter);
				}
				if (typeArray[i] == "X")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Column_Filter);
				}
				if (typeArray[i] == "Y")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Row_Filter);
				}
				if (typeArray[i] == "角度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Angle_Filter);
				}
				if (typeArray[i] == "圆度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Circularity_Filter);
				}
				if (typeArray[i] == "紧密度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Compactness_Filter);
				}
				if (typeArray[i] == "凸度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Convexity_Filter);
				}
				if (typeArray[i] == "矩形度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rectangularity_Filter);
				}
				if (typeArray[i] == "高度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Height_Filter);
				}
				if (typeArray[i] == "宽度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Width_Filter);
				}
				if (typeArray[i] == "最小外接圆半径")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_OuterRadius_Filter);
				}
				if (typeArray[i] == "最小外接矩半长")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len1_Filter);
				}
				if (typeArray[i] == "最小外接矩半宽")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len2_Filter);
				}
				if (typeArray[i] == "最小外接矩角度")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Angle_Filter);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount > 0)
			{
				//读取筛选参数最小值
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					return errorCode;
				}
				if (valueXml != "null")
				{
					vector<string> typeArray;
					stringToken(valueXml, ",", typeArray);
					runParams.hv_MinValue_Filter.Clear();
					for (int i = 0; i < typeArray.size(); i++)
					{
						xmlC.StrToDouble(typeArray[i], tempDoubleValue);
						runParams.hv_MinValue_Filter.Append(tempDoubleValue);
					}
				}

				//读取筛选参数最大值
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					return errorCode;
				}
				if (valueXml != "null")
				{
					vector<string> typeArray;
					stringToken(valueXml, ",", typeArray);
					runParams.hv_MaxValue_Filter.Clear();
					for (int i = 0; i < typeArray.size(); i++)
					{
						xmlC.StrToDouble(typeArray[i], tempDoubleValue);
						runParams.hv_MaxValue_Filter.Append(tempDoubleValue);
					}
				}

				//读取与或
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					return errorCode;
				}
				runParams.hv_And_Or_Filter = HTuple(valueXml.c_str());
			}
		}
		else
		{
			runParams.hv_FeaturesFilter_Type.Clear();
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存图像增强参数
int DataIO::WriteParams_ImgProcessOne(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//写入数组(数组按照逗号分割存储)

		//写入图像增强类型
		int typeCount = runParams.hv_ProcessType.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_ProcessType == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", "null");
			WriteTxt("图像增强类型为空,保存字符串null");
		}
		else
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", runParams.hv_ProcessType.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_ProcessType[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_ProcessType[i];
					}
					else
					{
						typeTemp += runParams.hv_ProcessType[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", typeTemp.S().Text());
			}

			//写入滤波核的宽
			typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.TupleInt().I()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_CoreWidth[i];
						}
						else
						{
							typeTemp += runParams.hv_CoreWidth[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
				}
			}


			//写入滤波核的高
			typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.TupleInt().I()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_CoreHeight[i];
						}
						else
						{
							typeTemp += runParams.hv_CoreHeight[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
				}
			}


			//写入Sigma
			typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Sigma[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Sigma[i];
						}
						else
						{
							typeTemp += runParams.hv_Sigma[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
				}
			}


			//写入双边滤波灰度阈值
			typeCount = runParams.hv_BilateralFilterThreshold.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralFilterThreshold", to_string(runParams.hv_BilateralFilterThreshold.TupleInt().I()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_BilateralFilterThreshold[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_BilateralFilterThreshold[i];
						}
						else
						{
							typeTemp += runParams.hv_BilateralFilterThreshold[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralFilterThreshold", typeTemp.S().Text());
				}
			}


			//写入灰度拉伸乘法因子
			typeCount = runParams.hv_Scale_Image_MultValue.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_MultValue", to_string(runParams.hv_Scale_Image_MultValue.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Scale_Image_MultValue[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Scale_Image_MultValue[i];
						}
						else
						{
							typeTemp += runParams.hv_Scale_Image_MultValue[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_MultValue", typeTemp.S().Text());
				}
			}


			//写入灰度拉伸加法因子
			typeCount = runParams.hv_Scale_Image_AddValue.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_AddValue", to_string(runParams.hv_Scale_Image_AddValue.TupleReal().D()));
				}
				else
				{
					HTuple typeTemp = runParams.hv_Scale_Image_AddValue[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						if (i == typeCount - 1)
						{
							typeTemp += runParams.hv_Scale_Image_AddValue[i];
						}
						else
						{
							typeTemp += runParams.hv_Scale_Image_AddValue[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_AddValue", typeTemp.S().Text());
				}
			}

		}



		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("图像增强区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		if (typeNum > 0)
		{
			WriteTxt("图像增强检测参数保存成功!");
		}

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，图像增强参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取图像增强参数
ErrorCode_Xml DataIO::ReadParams_ImgProcessOne(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		//参数读取
		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//图像增强参数读取
		//读取数组(数组按照逗号分割存储)

		int typeNum = 0;
		//读取图像增强类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像增强参数hv_ProcessType失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			runParams.hv_ProcessType.Clear();
			TypeStruct_ImgProcessOneDetect codeTypeStruct;
			for (int i = 0; i < typeArray.size(); i++)
			{
				if (typeArray[i] == "高斯滤波")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Gauss_Filter);
				}
				else if (typeArray[i] == "双边滤波")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Bilateral_Filter);
				}
				else if (typeArray[i] == "均值滤波")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Mean_Image);
				}
				else if (typeArray[i] == "中值滤波")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Median_Rect);
				}
				else if (typeArray[i] == "冲击波滤波")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Shock_Filter);
				}
				else if (typeArray[i] == "直方图均衡")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Equ_Histo_Image);
				}
				else if (typeArray[i] == "对比度增强")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Emphasize);
				}
				else if (typeArray[i] == "灰度拉伸")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Scale_Image);
				}
				else if (typeArray[i] == "灰度拉伸至0-255")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Scale_ImageMax);
				}
				else if (typeArray[i] == "灰度反转")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Invert_Image);
				}
				else if (typeArray[i] == "对数变换")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Log_Image);
				}
				else if (typeArray[i] == "指数变换")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Exp_Image);
				}
				else if (typeArray[i] == "灰度腐蚀")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayErosion_Rect);
				}
				else if (typeArray[i] == "灰度膨胀")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayDilation_Rect);
				}
				else if (typeArray[i] == "灰度开运算")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayOpening);
				}
				else if (typeArray[i] == "灰度闭运算")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayClosing);
				}
			}
			int typeCount = runParams.hv_ProcessType.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("图像增强类型为空，参数读取失败");
			}
			else
			{
				int isContainRectangle = 0;
				int isContainSigma = 0;
				int isContainBilateral = 0;
				int isContainScaleImg = 0;
				for (int i = 0; i < typeCount; i++)
				{
					if (runParams.hv_ProcessType[i] == "均值滤波" || runParams.hv_ProcessType[i] == "中值滤波" ||
						runParams.hv_ProcessType[i] == "对比度增强" || runParams.hv_ProcessType[i] == "灰度腐蚀" ||
						runParams.hv_ProcessType[i] == "灰度膨胀" || runParams.hv_ProcessType[i] == "灰度开运算" ||
						runParams.hv_ProcessType[i] == "灰度闭运算")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_ProcessType[i] == "高斯滤波" || runParams.hv_ProcessType[i] == "冲击波滤波")
					{
						isContainSigma += 1;

					}
					else if (runParams.hv_ProcessType[i] == "双边滤波")
					{
						isContainSigma += 1;
						isContainBilateral += 1;
					}
					else if (runParams.hv_ProcessType[i] == "灰度拉伸")
					{
						isContainScaleImg += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//读取滤波核宽度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_CoreWidth失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_CoreWidth.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToInt(typeArray[i], tempIntValue);
							runParams.hv_CoreWidth.Append(tempIntValue);
						}
					}

					//读取滤波核高度
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_CoreHeight失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_CoreHeight.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToInt(typeArray[i], tempIntValue);
							runParams.hv_CoreHeight.Append(tempIntValue);
						}
					}
				}

				if (isContainSigma > 0)
				{
					//读取Sigma
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_Sigma失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Sigma.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Sigma.Append(tempDoubleValue);
						}
					}
				}

				if (isContainBilateral > 0)
				{
					//读取双边滤波灰度阈值
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralFilterThreshold", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_BilateralFilterThreshold失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_BilateralFilterThreshold.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToInt(typeArray[i], tempIntValue);
							runParams.hv_BilateralFilterThreshold.Append(tempIntValue);
						}
					}
				}

				if (isContainScaleImg > 0)
				{
					//读取灰度拉伸乘法因子
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_MultValue", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_Scale_Image_MultValue失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Scale_Image_MultValue.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Scale_Image_MultValue.Append(tempDoubleValue);
						}
					}

					//读取灰度拉伸加法因子
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_AddValue", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取图像增强参数hv_Scale_Image_AddValue失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_Scale_Image_AddValue.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_Scale_Image_AddValue.Append(tempDoubleValue);
						}
					}
				}
			}

		}
		else
		{
			runParams.hv_ProcessType.Clear();
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		if (typeNum > 0)
		{
			WriteTxt("图像增强参数读取成功!");
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图像增强参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存两图处理参数
int DataIO::WriteParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//找两图处理参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", runParams.hv_ProcessType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MultValue", to_string(runParams.hv_MultValue.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AddValue", to_string(runParams.hv_AddValue.TupleReal().D()));


		WriteTxt("两图处理参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，两图处理参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取两图处理参数
ErrorCode_Xml DataIO::ReadParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取两图处理参数hv_ProcessType失败");
			return errorCode;
		}
		runParams.hv_ProcessType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MultValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取两图处理参数hv_MultValue失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MultValue = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AddValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取两图处理参数hv_AddValue失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_AddValue = tempDoubleValue;




		WriteTxt("两图处理参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，两图处理参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存图像转换参数
int DataIO::WriteParams_ImgConvert(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgConvertDetectType", runParams.hv_ImgConvertDetectType.S().Text());

		WriteTxt("图像转换参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，图像转换参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取图像转换参数
ErrorCode_Xml DataIO::ReadParams_ImgConvert(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;


		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgConvertDetectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像转换参数hv_ProcessType失败");
			return errorCode;
		}
		runParams.hv_ImgConvertDetectType = HTuple(valueXml.c_str());

		WriteTxt("图像转换参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图像转换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存图像旋转参数
int DataIO::WriteParams_ImgRotate(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", to_string(runParams.hv_Angle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type", runParams.hv_Type.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MirrorDirection", runParams.hv_MirrorDirection.S().Text());

		WriteTxt("图像旋转参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，图像旋转参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取图像旋转参数
ErrorCode_Xml DataIO::ReadParams_ImgRotate(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像旋转参数hv_Angle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像旋转参数hv_Type失败");
			return errorCode;
		}
		runParams.hv_Type = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MirrorDirection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像旋转参数hv_MirrorDirection失败");
			return errorCode;
		}
		runParams.hv_MirrorDirection = HTuple(valueXml.c_str());

		WriteTxt("图像旋转参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图像旋转参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存图片分类参数
int DataIO::WriteParams_ImgClassify(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//写入数组(数组按照逗号分割存储)
		//*******************************区域操作****************************************************************
		int typeCount = runParams.hv_ClassNames.size();
		typeNum = typeCount;
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", "null");
			WriteTxt("图片分类操作类型为空,保存字符串null");
		}
		else
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", runParams.hv_ClassNames[0].S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_ClassNames[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_ClassNames[i];
					}
					else
					{
						typeTemp += runParams.hv_ClassNames[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_ImgFilePaths.size();
		typeNum = typeCount;
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePaths", "null");
			WriteTxt("图片分类操作类型为空,保存字符串null");
		}
		else
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePaths", runParams.hv_ImgFilePaths[0].S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_ImgFilePaths[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_ImgFilePaths[i];
					}
					else
					{
						typeTemp += runParams.hv_ImgFilePaths[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePaths", typeTemp.S().Text());
			}
		}

		//保存分类算法
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassMethod", runParams.hv_ClassMethod.S().Text());

		//保存训练好的模型
		if (runParams.hv_ClassMethod == "MLP")
		{
			HTuple ClassifyIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
			WriteClassMlp(runParams.hv_ClassifyHandle, ClassifyIDPath);
		}
		else
		{
			HTuple ClassifyIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gsc";
			WriteClassSvm(runParams.hv_ClassifyHandle, ClassifyIDPath);
		}


		WriteTxt("图片分类参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("图片分类保存代码崩溃，参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取图片分类参数
ErrorCode_Xml DataIO::ReadParams_ImgClassify(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图片分类参数hv_ClassNames失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ClassNames.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ClassNames.push_back(HTuple(typeArray[i].c_str()));
		}
		int typeCount = runParams.hv_ClassNames.size();
		if (typeCount == 0)
		{
			WriteTxt("图片分类参数为空，读取图片分类参数失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePaths", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图片分类参数hv_ImgFilePaths失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ImgFilePaths.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ImgFilePaths.push_back(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ImgFilePaths.size();
		if (typeCount == 0)
		{
			WriteTxt("图片分类参数为空，读取图片分类参数失败");
			return nullKeyName_Xml;
		}

		//读取分类算法
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取图像分类参数hv_ClassMethod失败");
			return errorCode;
		}
		runParams.hv_ClassMethod = HTuple(valueXml.c_str());

		//读取模型
		if (runParams.hv_ClassMethod == "MLP")
		{
			HTuple ClassifyIdPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
			ReadClassMlp(ClassifyIdPath, &runParams.hv_ClassifyHandle);
		}
		else
		{
			HTuple ClassifyIdPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gsc";
			ReadClassSvm(ClassifyIdPath, &runParams.hv_ClassifyHandle);
		}


		WriteTxt("图片分类参数加载成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图片分类参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存极坐标变换参数
int DataIO::WriteParams_PolarTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InnerRadius", to_string(runParams.hv_InnerRadius.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OuterRadius", to_string(runParams.hv_OuterRadius.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row", to_string(runParams.hv_Row.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column", to_string(runParams.hv_Column.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalWidth", to_string(runParams.hv_OriginalWidth.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalHeight", to_string(runParams.hv_OriginalHeight.TupleInt().I()));


		WriteTxt("极坐标变换参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，极坐标变换参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取极坐标变换参数
ErrorCode_Xml DataIO::ReadParams_PolarTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InnerRadius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_InnerRadius失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_InnerRadius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OuterRadius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_OuterRadius失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_OuterRadius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_Row失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_Column失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_OriginalWidth失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_OriginalWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取极坐标变换参数hv_OriginalHeight失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_OriginalHeight = tempIntValue;



		WriteTxt("极坐标变换参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，极坐标变换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存标定板标定参数
int DataIO::WriteParams_CalibBoard(const string ConfigPath, const string XmlPath, const ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Error", to_string(resultParams.hv_Error.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", to_string(resultParams.hv_Resolution.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceCC", to_string(resultParams.hv_DistanceCC.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MapImgType", resultParams.hv_MapImgType.S().Text());



		//保存相机内参和外参
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		WriteCamPar(resultParams.hv_CamParam, hv_CamParam_Path);
		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		WritePose(resultParams.hv_CamPose, hv_CamPose_Path);


		WriteTxt("标定板标定参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，标定板标定参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取标定板标定参数
ErrorCode_Xml DataIO::ReadParams_CalibBoard(const string ConfigPath, const string XmlPath, ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Error", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_Error失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_Error = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_Resolution失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_Resolution = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceCC", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_DistanceCC失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_DistanceCC = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MapImgType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_MapImgType失败");
			return errorCode;
		}
		resultParams.hv_MapImgType = HTuple(valueXml.c_str());


		//读取相机内参和外参
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		HTuple isExist;
		FileExists(hv_CamParam_Path, &isExist);
		if (isExist > 0)
		{
			ReadCamPar(hv_CamParam_Path, &resultParams.hv_CamParam);
		}
		else
		{
			resultParams.hv_CamParam.Clear();
		}

		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		HTuple isExist1;
		FileExists(hv_CamPose_Path, &isExist1);
		if (isExist1 > 0)
		{
			ReadPose(hv_CamPose_Path, &resultParams.hv_CamPose);
		}
		else
		{
			resultParams.hv_CamPose.Clear();
		}


		WriteTxt("标定板标定参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，标定板标定参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存联合标定标定参数
int DataIO::WriteParams_JointCalibration(const string ConfigPath, const string XmlPath, const RunParamsStruct_JointCalibrationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Error", to_string(runParams.hv_Error.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", to_string(runParams.hv_Resolution.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabThickness", to_string(runParams.hv_CaltabThickness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabDescrPath", runParams.hv_CaltabDescrPath.S().Text());
		//保存需要被标定的相机参数
		int typeCount = runParams.hv_StartCamPar.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", "null");
			WriteTxt("相机参数为空，默认保存为null");
		}

		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", runParams.hv_StartCamPar.S().Text());
		}
		else
		{
			char buffer[100];
			double dValue = runParams.hv_StartCamPar[0].D();
			sprintf(buffer, "%.15f", dValue);
			string sValue = string(buffer);
			HTuple typeTemp = HTuple(sValue.c_str()) + ",";
			for (int i = 1; i < typeCount; i++)
			{
				char buffer[100];
				double dValue = runParams.hv_StartCamPar[i].D();
				sprintf(buffer, "%.15f", dValue);
				string sValue = string(buffer);
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(sValue.c_str());
				}
				else
				{
					typeTemp += HTuple(sValue.c_str()) + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", typeTemp.S().Text());
		}

		//保存标定后的相机内参和外参
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		WriteCamPar(runParams.hv_CamParam, hv_CamParam_Path);
		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		WritePose(runParams.hv_CamPose, hv_CamPose_Path);


		WriteTxt("标定板标定参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，标定板标定参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取联合标定标定参数
ErrorCode_Xml DataIO::ReadParams_JointCalibration(const string ConfigPath, const string XmlPath, RunParamsStruct_JointCalibrationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取联合标定检测参数hv_StartCamPar失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StartCamPar.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StartCamPar.Append(tempDoubleValue);
		}
		int typeCount = runParams.hv_StartCamPar.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("相机参数为空，读取联合标定相机参数hv_StartCamPar失败");
			return nullKeyName_Xml;
		}

		//参数读取
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Error", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_Error失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Error = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_Resolution失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Resolution = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabThickness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_CaltabThickness失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CaltabThickness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabDescrPath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取标定板标定参数hv_CaltabDescrPath失败");
			return errorCode;
		}
		runParams.hv_CaltabDescrPath = HTuple(valueXml.c_str());


		//读取相机内参和外参
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		HTuple isExist;
		FileExists(hv_CamParam_Path, &isExist);
		if (isExist > 0)
		{
			ReadCamPar(hv_CamParam_Path, &runParams.hv_CamParam);
		}
		else
		{
			runParams.hv_CamParam.Clear();
		}

		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		HTuple isExist1;
		FileExists(hv_CamPose_Path, &isExist1);
		if (isExist1 > 0)
		{
			ReadPose(hv_CamPose_Path, &runParams.hv_CamPose);
		}
		else
		{
			runParams.hv_CamPose.Clear();
		}


		WriteTxt("标定板标定参数读取成功");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，标定板标定参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存光度立体参数
int DataIO::WriteParams_PhotoStereo(const string ConfigPath, const string XmlPath, const RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//光度立体参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_PhotoStereoType", runParams.hv_PhotoStereoType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Slants1", to_string(runParams.hv_Slants1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Slants2", to_string(runParams.hv_Slants2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Slants3", to_string(runParams.hv_Slants3.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Slants4", to_string(runParams.hv_Slants4.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts1", to_string(runParams.hv_Tilts1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts2", to_string(runParams.hv_Tilts2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts3", to_string(runParams.hv_Tilts3.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts4", to_string(runParams.hv_Tilts4.TupleReal().D()));

		WriteTxt("光度立体参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，光度立体参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取光度立体参数
ErrorCode_Xml DataIO::ReadParams_PhotoStereo(const string ConfigPath, const string XmlPath, RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//光度立体读取
		//读取数组(数组按照逗号分割存储)

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_PhotoStereoType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_PhotoStereoType失败");
			return errorCode;
		}
		runParams.hv_PhotoStereoType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Sigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Slants1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Slants2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants3", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Slants3失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants3 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants4", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Slants4失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants4 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Tilts1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Tilts2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts3", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Tilts3失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts3 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts4", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取光度立体参数hv_Tilts4失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts4 = tempDoubleValue;


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，光度立体参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存傅里叶变换参数
int DataIO::WriteParams_FourierTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//傅里叶变换参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ResultType", runParams.hv_ResultType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", runParams.hv_FilterType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", to_string(runParams.hv_MaskWidth.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaskHeight", to_string(runParams.hv_MaskHeight.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Frequency", to_string(runParams.hv_Frequency.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BandMinFrequency", to_string(runParams.hv_BandMinFrequency.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BandMaxFrequency", to_string(runParams.hv_BandMaxFrequency.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMinSigma", to_string(runParams.hv_GaussBandMinSigma.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMaxSigma", to_string(runParams.hv_GaussBandMaxSigma.TupleReal().D()));

		WriteTxt("傅里叶变换参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，傅里叶变换参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取傅里叶变换参数
ErrorCode_Xml DataIO::ReadParams_FourierTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//傅里叶变换参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ResultType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_ResultType失败");
			return errorCode;
		}
		runParams.hv_ResultType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_FilterType失败");
			return errorCode;
		}
		runParams.hv_FilterType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_Sigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_MaskWidth失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MaskWidth = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_MaskHeight失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MaskHeight = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Frequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_Frequency失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Frequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BandMinFrequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_BandMinFrequency失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_BandMinFrequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BandMaxFrequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_BandMaxFrequency失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_BandMaxFrequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMinSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_GaussBandMinSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_GaussBandMinSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMaxSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取傅里叶变换参数hv_GaussBandMaxSigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_GaussBandMaxSigma = tempDoubleValue;


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，傅里叶变换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存仿射变换参数
int DataIO::WriteParams_AffineMatrix(const string ConfigPath, const string XmlPath, const RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//仿射变换参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type_CreateHomMat2D", runParams.hv_Type_CreateHomMat2D.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type_ApplyHomMat2D", runParams.hv_Type_ApplyHomMat2D.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ReverseAngle", to_string(runParams.hv_ReverseAngle.TupleInt().I()));

		WriteTxt("仿射变换参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，仿射变换参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取仿射变换参数
ErrorCode_Xml DataIO::ReadParams_AffineMatrix(const string ConfigPath, const string XmlPath, RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//仿射变换参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type_CreateHomMat2D", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取仿射变换参数hv_Type_CreateHomMat2D失败");
			return errorCode;
		}
		runParams.hv_Type_CreateHomMat2D = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type_ApplyHomMat2D", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取仿射变换参数hv_Type_ApplyHomMat2D失败");
			return errorCode;
		}
		runParams.hv_Type_ApplyHomMat2D = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ReverseAngle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取仿射变换参数hv_ReverseAngle失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ReverseAngle = tempIntValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，仿射变换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存裁图参数
int DataIO::WriteParams_CutImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//裁图参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutImgType", runParams.hv_CutImgType.S().Text());

		WriteTxt("裁图参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，裁图参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取裁图参数
ErrorCode_Xml DataIO::ReadParams_CutImg(const string ConfigPath, const string XmlPath, RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//裁图参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutImgType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取裁图参数hv_CutImgType失败");
			return errorCode;
		}
		runParams.hv_CutImgType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，裁图参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存创建Roi参数
int DataIO::WriteParams_CreateRoi(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//裁图参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TypeRoi", runParams.hv_TypeRoi.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Circle", to_string(runParams.hv_Row_Circle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Circle", to_string(runParams.hv_Column_Circle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Circle", to_string(runParams.hv_Radius_Circle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Ellipse", to_string(runParams.hv_Row_Ellipse.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Ellipse", to_string(runParams.hv_Column_Ellipse.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Ellipse", to_string(runParams.hv_Angle_Ellipse.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1_Ellipse", to_string(runParams.hv_Radius1_Ellipse.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2_Ellipse", to_string(runParams.hv_Radius2_Ellipse.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Rectangle1", to_string(runParams.hv_Row1_Rectangle1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Rectangle1", to_string(runParams.hv_Column1_Rectangle1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Rectangle1", to_string(runParams.hv_Row2_Rectangle1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Rectangle1", to_string(runParams.hv_Column2_Rectangle1.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Rectangle2", to_string(runParams.hv_Row_Rectangle2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Rectangle2", to_string(runParams.hv_Column_Rectangle2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Rectangle2", to_string(runParams.hv_Angle_Rectangle2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Length1_Rectangle2", to_string(runParams.hv_Length1_Rectangle2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Length2_Rectangle2", to_string(runParams.hv_Length2_Rectangle2.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowBegin_Line", to_string(runParams.hv_RowBegin_Line.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnBegin_Line", to_string(runParams.hv_ColumnBegin_Line.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowEnd_Line", to_string(runParams.hv_RowEnd_Line.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnEnd_Line", to_string(runParams.hv_ColumnEnd_Line.TupleReal().D()));


		WriteTxt("创建Roi保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，创建Roi参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取创建Roi参数
ErrorCode_Xml DataIO::ReadParams_CreateRoi(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//裁图参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi的hv_TypeRoi失败");
			return errorCode;
		}
		runParams.hv_TypeRoi = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Row_Circle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Column_Circle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Radius_Circle失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Row_Ellipse失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Column_Ellipse失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Angle_Ellipse失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Radius1_Ellipse失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius1_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Radius2_Ellipse失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius2_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Row1_Rectangle1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row1_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Column1_Rectangle1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column1_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Row2_Rectangle1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row2_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Column2_Rectangle1失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column2_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Row_Rectangle2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Column_Rectangle2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Angle_Rectangle2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length1_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Length1_Rectangle2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length1_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length2_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_Length2_Rectangle2失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length2_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowBegin_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_RowBegin_Line失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_RowBegin_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnBegin_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_ColumnBegin_Line失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ColumnBegin_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowEnd_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_RowEnd_Line失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_RowEnd_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnEnd_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_ColumnEnd_Line失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ColumnEnd_Line = tempDoubleValue;



		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，创建Roi参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
////保存颜色检测参数
//int DataIO::WriteParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//
//		if (runParams.hv_ColorHandle == NULL)
//		{
//			//句柄为空
//			//mutex1.unlock();
//			return 1;
//		}
//		//保存模型
//		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetect_MoreNumHandle.ggc";
//		WriteClassGmm(runParams.hv_ColorHandle, PathHandle);
//
//		//保存搜索区域
//		if (runParams.ho_SearchRegion.Key() != nullptr)
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
//		}
//
//		//写入数组(数组按照逗号分割存储)
//		int typeCount = runParams.hv_MeanValue_R.size();
//		if (runParams.hv_MeanValue_R.size() <= 0 || runParams.hv_MeanValue_G.size() <= 0 || runParams.hv_MeanValue_B.size() <= 0)
//		{
//			WriteTxt("颜色检测训练参数为空,保存数据失败");
//			return 1;
//		}
//		else
//		{
//			//写入R通道灰度值
//			if (typeCount == 1)
//			{
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", to_string(runParams.hv_MeanValue_R[0].TupleInt().I()));
//			}
//			else
//			{
//				HTuple typeTemp = runParams.hv_MeanValue_R[0] + ",";
//				for (int i = 1; i < typeCount; i++)
//				{
//					if (i == typeCount - 1)
//					{
//						typeTemp += runParams.hv_MeanValue_R[i];
//					}
//					else
//					{
//						typeTemp += runParams.hv_MeanValue_R[i] + ",";
//					}
//				}
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", typeTemp.S().Text());
//			}
//			//写入G通道灰度值
//			if (typeCount == 1)
//			{
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", to_string(runParams.hv_MeanValue_G[0].TupleInt().I()));
//			}
//			else
//			{
//				HTuple typeTemp = runParams.hv_MeanValue_G[0] + ",";
//				for (int i = 1; i < typeCount; i++)
//				{
//					if (i == typeCount - 1)
//					{
//						typeTemp += runParams.hv_MeanValue_G[i];
//					}
//					else
//					{
//						typeTemp += runParams.hv_MeanValue_G[i] + ",";
//					}
//				}
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", typeTemp.S().Text());
//			}
//			//写入B通道灰度值
//			if (typeCount == 1)
//			{
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", to_string(runParams.hv_MeanValue_B[0].TupleInt().I()));
//			}
//			else
//			{
//				HTuple typeTemp = runParams.hv_MeanValue_B[0] + ",";
//				for (int i = 1; i < typeCount; i++)
//				{
//					if (i == typeCount - 1)
//					{
//						typeTemp += runParams.hv_MeanValue_B[i];
//					}
//					else
//					{
//						typeTemp += runParams.hv_MeanValue_B[i] + ",";
//					}
//				}
//				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", typeTemp.S().Text());
//			}
//		}
//
//		//写入颜色名称
//		typeCount = runParams.hv_ColorName.size();
//		if (typeCount <= 0)
//		{
//			WriteTxt("颜色检测颜色名称训练参数为空,保存数据失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorName", runParams.hv_ColorName[0].S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ColorName[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ColorName[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ColorName[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorName", typeTemp.S().Text());
//		}
//		//mutex1.unlock();
//		return 0;
//	}
//	catch (...)
//	{
//		WriteTxt("XML保存代码崩溃，颜色检测参数保存失败!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////读取颜色检测参数
//ErrorCode_Xml DataIO::ReadParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//
//
//		//运行区域加载
//		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//		HTuple isExist;
//		FileExists(PathRunRegion, &isExist);
//		if (isExist > 0)
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion);
//			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
//		}
//		else
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion);
//			runParams.ho_SearchRegion.Clear();
//		}
//
//		//读取模型
//		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetect_MoreNumHandle.ggc";
//		ReadClassGmm(PathHandle, &runParams.hv_ColorHandle);
//
//
//		//读取数组(数组按照逗号分割存储)
//		//R通道灰度值
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML颜色检测参数hv_MeanValue_R失败");
//			runParams.hv_MeanValue_R.clear();
//			return errorCode;
//		}
//		if (valueXml != "null")
//		{
//			vector<string> typeArray;
//			stringToken(valueXml, ",", typeArray);
//			runParams.hv_MeanValue_R.clear();
//			for (int i = 0; i < typeArray.size(); i++)
//			{
//				xmlC.StrToInt(typeArray[i], tempIntValue);
//				runParams.hv_MeanValue_R.push_back(tempIntValue);
//			}
//		}
//		else
//		{
//			runParams.hv_MeanValue_R.clear();
//		}
//		//G通道灰度值
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML颜色检测参数hv_MeanValue_G失败");
//			runParams.hv_MeanValue_G.clear();
//			return errorCode;
//		}
//		if (valueXml != "null")
//		{
//			vector<string> typeArray;
//			stringToken(valueXml, ",", typeArray);
//			runParams.hv_MeanValue_G.clear();
//			for (int i = 0; i < typeArray.size(); i++)
//			{
//				xmlC.StrToInt(typeArray[i], tempIntValue);
//				runParams.hv_MeanValue_G.push_back(tempIntValue);
//			}
//		}
//		else
//		{
//			runParams.hv_MeanValue_G.clear();
//		}
//		//B通道灰度值
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML颜色检测参数hv_MeanValue_B失败");
//			runParams.hv_MeanValue_B.clear();
//			return errorCode;
//		}
//		if (valueXml != "null")
//		{
//			vector<string> typeArray;
//			stringToken(valueXml, ",", typeArray);
//			runParams.hv_MeanValue_B.clear();
//			for (int i = 0; i < typeArray.size(); i++)
//			{
//				xmlC.StrToInt(typeArray[i], tempIntValue);
//				runParams.hv_MeanValue_B.push_back(tempIntValue);
//			}
//		}
//		else
//		{
//			runParams.hv_MeanValue_B.clear();
//		}
//		//颜色名称
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorName", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML颜色检测参数hv_ColorName失败");
//			runParams.hv_ColorName.clear();
//			return errorCode;
//		}
//		if (valueXml != "null")
//		{
//			vector<string> typeArray;
//			stringToken(valueXml, ",", typeArray);
//			runParams.hv_ColorName.clear();
//			for (int i = 0; i < typeArray.size(); i++)
//			{
//				runParams.hv_ColorName.push_back(HTuple(typeArray[i].c_str()));
//			}
//		}
//		else
//		{
//			runParams.hv_ColorName.clear();
//		}
//
//
//		//mutex1.unlock();
//		return Ok_Xml;
//	}
//	catch (...)
//	{
//		WriteTxt("XML读取代码崩溃，颜色检测参数读取失败!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}
//保存颜色检测参数
int DataIO::WriteParams_ColorDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		if (runParams.hv_ColorHandle == NULL)
		{
			//句柄为空
			//mutex1.unlock();
			return 1;
		}
		//保存模型
		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetectHandle.ggc";
		WriteClassGmm(runParams.hv_ColorHandle, PathHandle);

		//保存搜索区域
		if (runParams.ho_SearchRegion.Key() != nullptr)
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}
		//保存训练区域
		if (runParams.ho_TrainRegions.Key() != nullptr)
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(runParams.ho_TrainRegions, PathRunRegion);
		}

		//写入R通道灰度值
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", to_string(runParams.hv_MeanValue_R.TupleInt().I()));

		//写入G通道灰度值
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", to_string(runParams.hv_MeanValue_G.TupleInt().I()));

		//写入B通道灰度值
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", to_string(runParams.hv_MeanValue_B.TupleInt().I()));

		//保存最小分数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(runParams.hv_MinScore.TupleReal().D()));
		if (runParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}


		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，颜色检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取颜色检测参数
ErrorCode_Xml DataIO::ReadParams_ColorDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		double tempDoubleValue;
		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;


		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		//训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_TrainRegions);
			ReadRegion(&runParams.ho_TrainRegions, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_TrainRegions);
			runParams.ho_TrainRegions.Clear();
		}

		//读取模型
		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetectHandle.ggc";
		ReadClassGmm(PathHandle, &runParams.hv_ColorHandle);

		//R通道灰度值
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_MeanValue_R失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_R = tempDoubleValue;

		//G通道灰度值
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_MeanValue_G失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_G = tempDoubleValue;

		//B通道灰度值
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_MeanValue_B失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_B = tempDoubleValue;

		//读取最小分数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建Roi参数hv_MinScore失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，颜色检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存拟合参数
int DataIO::WriteParams_FitDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//拟合参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FitType", runParams.hv_FitType.S().Text());

		WriteTxt("拟合参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，拟合参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取拟合参数
ErrorCode_Xml DataIO::ReadParams_FitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//拟合参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FitType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取拟合参数hv_FitType失败");
			return errorCode;
		}
		runParams.hv_FitType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，拟合参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存区域变换参数
int DataIO::WriteParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//转换类型参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TypeTrans", runParams.hv_TypeTrans.S().Text());

		WriteTxt("区域转换参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，区域转换参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取区域变换参数
ErrorCode_Xml DataIO::ReadParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//区域变换参数读取
		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeTrans", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取区域变换参数hv_TypeTrans失败");
			return errorCode;
		}
		runParams.hv_TypeTrans = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，区域变换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存创建轮廓参数
int DataIO::WriteParams_CreateContourDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//转换类型参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TypeCreateContour", runParams.hv_TypeCreateContour.S().Text());

		WriteTxt("创建轮廓参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，创建轮廓参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取创建轮廓参数
ErrorCode_Xml DataIO::ReadParams_CreateContourDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//区域变换参数读取
		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeCreateContour", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取创建轮廓参数hv_TypeCreateContour失败");
			return errorCode;
		}
		runParams.hv_TypeCreateContour = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，区域变换参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}


//保存交点参数
int DataIO::WriteParams_Intersection(const string ConfigPath, const string XmlPath, const RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IntersectType", runParams.hv_IntersectType.S().Text());

		WriteTxt("交点参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，交点参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取交点参数
ErrorCode_Xml DataIO::ReadParams_Intersection(const string ConfigPath, const string XmlPath, RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//交点参数读取

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IntersectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取交点参数hv_IntersectType失败");
			return errorCode;
		}
		runParams.hv_IntersectType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，交点参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存测量参数(避免使用科学计数法)
int DataIO::WriteParams_Measurement(const string ConfigPath, const string XmlPath, const RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureType", runParams.hv_MeasureType.S().Text());
		//保存像素当量
		//保留11位有效位数(不是小数位)
		std::stringstream ss;
		ss << std::setprecision(11) << runParams.hv_PixelMm.TupleReal().D();
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_PixelMm", ss.str());
		WriteTxt("测量参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，测量参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取测量参数
ErrorCode_Xml DataIO::ReadParams_Measurement(const string ConfigPath, const string XmlPath, RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取测量参数hv_MeasureType失败");
			return errorCode;
		}
		runParams.hv_MeasureType = HTuple(valueXml.c_str());
		//像素当量
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_PixelMm", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取测量参数hv_PixelMm失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_PixelMm = tempDoubleValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，测量参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存存图参数
int DataIO::WriteParams_SaveImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SavePath", runParams.hv_SavePath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SaveType", runParams.hv_SaveType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SaveMethod", runParams.hv_SaveMethod);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FileName", runParams.hv_FileName);

		WriteTxt("存图参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，存图参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取存图参数
ErrorCode_Xml DataIO::ReadParams_SaveImg(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SavePath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取存图参数hv_SavePath失败");
			return errorCode;
		}
		runParams.hv_SavePath = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SaveType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取存图参数hv_SaveType失败");
			return errorCode;
		}
		runParams.hv_SaveType = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SaveMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取存图参数hv_SaveMethod失败");
			return errorCode;
		}
		runParams.hv_SaveMethod = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FileName", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取存图参数hv_FileName失败");
			return errorCode;
		}
		runParams.hv_FileName = valueXml.c_str();


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，拟合参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存OKNG判断的参数
int DataIO::WriteParams_Judgement(const string ConfigPath, const string XmlPath, const RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		//写入数组(数组按照逗号分割存储)
		//
		//全局变量名字
		int typeCount = runParams.hv_GlobalName.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断模块参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GlobalName", runParams.hv_GlobalName[0]);
		}
		else
		{
			string typeTemp = runParams.hv_GlobalName[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_GlobalName[i];
				}
				else
				{
					typeTemp += runParams.hv_GlobalName[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GlobalName", typeTemp);
		}

		//最小值
		typeCount = runParams.hv_MinValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断模块参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue", to_string(runParams.hv_MinValue[0]));
		}
		else
		{
			string typeTemp = to_string(runParams.hv_MinValue[0]) + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += to_string(runParams.hv_MinValue[i]);
				}
				else
				{
					typeTemp += to_string(runParams.hv_MinValue[i]) + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue", typeTemp);
		}

		//最大值
		typeCount = runParams.hv_MaxValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断模块参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue", to_string(runParams.hv_MaxValue[0]));
		}
		else
		{
			string typeTemp = to_string(runParams.hv_MaxValue[0]) + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += to_string(runParams.hv_MaxValue[i]);
				}
				else
				{
					typeTemp += to_string(runParams.hv_MaxValue[i]) + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue", typeTemp);
		}

		//标签
		typeCount = runParams.hv_LabelStr.size();
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LabelStr", runParams.hv_LabelStr[0].S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_LabelStr[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_LabelStr[i];
				}
				else
				{
					typeTemp += runParams.hv_LabelStr[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LabelStr", typeTemp.S().Text());
		}

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AndOr", runParams.hv_AndOr);

		WriteTxt("OKNG判断模块参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，OKNG判断模块参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取OKNG判断的参数
ErrorCode_Xml DataIO::ReadParams_Judgement(const string ConfigPath, const string XmlPath, RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		double tempDoubleValue;
		string valueXml;
		ErrorCode_Xml errorCode;


		//读取数组(数组按照逗号分割存储)
		//
		//读取全局变量名字
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GlobalName", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取OKNG判断参数hv_GlobalName失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_GlobalName.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_GlobalName.push_back(typeArray[i]);
		}
		int typeCount = runParams.hv_GlobalName.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断参数为空，读取OKNG判断参数参数hv_GlobalName失败");
			return nullKeyName_Xml;
		}

		//读取最小值
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取OKNG判断参数hv_MinValue失败");
			return errorCode;
		}
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MinValue.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MinValue.push_back(tempDoubleValue);
		}
		typeCount = runParams.hv_MinValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断参数为空，读取OKNG判断参数参数hv_MinValue失败");
			return nullKeyName_Xml;
		}

		//读取最大值
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取OKNG判断参数hv_MaxValue失败");
			return errorCode;
		}
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MaxValue.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MaxValue.push_back(tempDoubleValue);
		}
		typeCount = runParams.hv_MaxValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断参数为空，读取OKNG判断参数参数hv_MaxValue失败");
			return nullKeyName_Xml;
		}

		//读取标签
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LabelStr", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取OKNG判断参数hv_LabelStr失败");
			return errorCode;
		}
		stringToken(valueXml, ",", typeArray);
		runParams.hv_LabelStr.clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_LabelStr.push_back(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_LabelStr.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG判断参数为空，读取OKNG判断参数参数hv_LabelStr失败");
			return nullKeyName_Xml;
		}

		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AndOr", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取OKNG判断参数hv_AndOr失败");
			return errorCode;
		}
		runParams.hv_AndOr = valueXml;


		WriteTxt("OKNG判断参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，OKNG判断参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存显示窗口参数
int DataIO::WriteParams_ImgShow(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LineWidth", to_string(runParams.hv_LineWidth.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Word_X", to_string(runParams.hv_Word_X.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Y", to_string(runParams.hv_Word_Y.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Size", to_string(runParams.hv_Word_Size.TupleInt().I()));

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		return -1;
	}
}
//读取显示窗口参数
ErrorCode_Xml DataIO::ReadParams_ImgShow(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LineWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取窗口显示参数hv_LineWidth失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_LineWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取窗口显示参数hv_Word_X失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Word_X = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取窗口显示参数hv_Word_Y失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Word_Y = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Size", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取窗口显示参数hv_Word_Size失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Word_Size = tempIntValue;


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//以下结构体和 RobotCalibDetect 里的结构体冲突，先屏蔽这边 20250507
////保存机器人定位引导参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID)
//int DataIO::WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//		//写入数组(数组按照逗号分割存储)
//		int typeCount = runParams.hv_NPointCamera_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("九点标定像素坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", to_string(runParams.hv_NPointCamera_X.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_NPointCamera_X[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_NPointCamera_X[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_NPointCamera_X[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_NPointCamera_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("九点标定像素坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", to_string(runParams.hv_NPointCamera_Y.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_NPointCamera_Y[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_NPointCamera_Y[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_NPointCamera_Y[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_NPointRobot_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("九点标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", to_string(runParams.hv_NPointRobot_X.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_NPointRobot_X[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_NPointRobot_X[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_NPointRobot_X[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_NPointRobot_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("九点标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", to_string(runParams.hv_NPointRobot_Y.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_NPointRobot_Y[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_NPointRobot_Y[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_NPointRobot_Y[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CenterCamera_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("旋转中心标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", to_string(runParams.hv_CenterCamera_X.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CenterCamera_X[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CenterCamera_X[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CenterCamera_X[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CenterCamera_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("旋转中心标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", to_string(runParams.hv_CenterCamera_Y.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CenterCamera_Y[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CenterCamera_Y[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CenterCamera_Y[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CenterRobot_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("旋转中心标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", to_string(runParams.hv_CenterRobot_X.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CenterRobot_X[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CenterRobot_X[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CenterRobot_X[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CenterRobot_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("旋转中心标定机器坐标为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", to_string(runParams.hv_CenterRobot_Y.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CenterRobot_Y[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CenterRobot_Y[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CenterRobot_Y[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CenterRobot_A.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("旋转中心标定机器角度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_A", to_string(runParams.hv_CenterRobot_A.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CenterRobot_A[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CenterRobot_A[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CenterRobot_A[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_A", typeTemp.S().Text());
//		}
//
//		//保存其余参数
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_X", to_string(runParams.hv_Center_X.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Y", to_string(runParams.hv_Center_Y.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Radius", to_string(runParams.hv_Center_Radius.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelX", to_string(runParams.hv_ModelX.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelY", to_string(runParams.hv_ModelY.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelA", to_string(runParams.hv_ModelA.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotX", to_string(runParams.hv_ModelPosition_RobotX.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotY", to_string(runParams.hv_ModelPosition_RobotY.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotA", to_string(runParams.hv_ModelPosition_RobotA.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotX", to_string(runParams.hv_CenterPosition_RobotX.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotY", to_string(runParams.hv_CenterPosition_RobotY.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotA", to_string(runParams.hv_CenterPosition_RobotA.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistancePP_Robot", to_string(runParams.hv_DistancePP_Robot.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleRotate_Robot", to_string(runParams.hv_AngleRotate_Robot.D()));
//		if (runParams.hv_IsCamFirst)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", "true");
//		}
//		else
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", "false");
//		}
//
//		if (runParams.hv_IsReverseAngle)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", "true");
//		}
//		else
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", "false");
//		}
//
//		if (runParams.hv_IsAbsCoordinate)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", "true");
//		}
//		else
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", "false");
//		}
//
//		if (runParams.hv_IsUseCenterCalib)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", "true");
//		}
//		else
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", "false");
//		}
//
//		//保存九点标定矩阵
//		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
//		WriteTuple(runParams.hv_HomMat2D, PathTuple);
//
//
//		WriteTxt("机器人标定参数保存成功!");
//		//mutex1.unlock();
//		return 0;
//	}
//	catch (...)
//	{
//		WriteTxt("XML保存代码崩溃，机器人标定参数保存失败!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////读取机器人定位参数(runParams 定位引导参数，processId 流程ID, nodeName 模块名称， processModbuleID 模块ID, value 输出读取到的值)
//ErrorCode_Xml DataIO::ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//		double tempDoubleValue;
//
//		//读取数组(数组按照逗号分割存储)
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_NPointCamera_X失败");
//			return errorCode;
//		}
//		vector<string> typeArray;
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_NPointCamera_X.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_NPointCamera_X.Append(tempDoubleValue);
//		}
//		int typeCount = runParams.hv_NPointCamera_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_NPointCamera_X失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_NPointCamera_Y失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_NPointCamera_Y.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_NPointCamera_Y.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_NPointCamera_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_NPointCamera_Y失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_NPointRobot_X失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_NPointRobot_X.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_NPointRobot_X.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_NPointRobot_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_NPointRobot_X失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_NPointRobot_Y失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_NPointRobot_Y.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_NPointRobot_Y.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_NPointRobot_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_NPointRobot_Y失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterCamera_X失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CenterCamera_X.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CenterCamera_X.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CenterCamera_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_CenterCamera_X失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterCamera_Y失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CenterCamera_Y.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CenterCamera_Y.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CenterCamera_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_CenterCamera_Y失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterRobot_X失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CenterRobot_X.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CenterRobot_X.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CenterRobot_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_CenterRobot_X失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterRobot_Y失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CenterRobot_Y.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CenterRobot_Y.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CenterRobot_Y.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_CenterRobot_Y失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_A", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterRobot_A失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CenterRobot_A.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CenterRobot_A.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CenterRobot_A.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("机器人标定参数为空，读取参数hv_CenterRobot_A失败");
//			return nullKeyName_Xml;
//		}
//
//
//		//读取其余参数
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_Center_X失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_X = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_Center_Y失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_Y = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Radius", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_Center_Radius失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_Radius = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelX失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelY失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelA失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelPosition_RobotX失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelPosition_RobotY失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_ModelPosition_RobotA失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterPosition_RobotX失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterPosition_RobotY失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_CenterPosition_RobotA失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistancePP_Robot", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_DistancePP_Robot失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_DistancePP_Robot = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleRotate_Robot", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_AngleRotate_Robot失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_AngleRotate_Robot = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_IsCamFirst失败");
//			return errorCode;
//		}
//		if (valueXml == "true")
//		{
//			runParams.hv_IsCamFirst = true;
//		}
//		else
//		{
//			runParams.hv_IsCamFirst = false;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsReverseAngle", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_IsReverseAngle失败");
//			return errorCode;
//		}
//		if (valueXml == "true")
//		{
//			runParams.hv_IsReverseAngle = true;
//		}
//		else
//		{
//			runParams.hv_IsReverseAngle = false;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsAbsCoordinate", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_IsAbsCoordinate失败");
//			return errorCode;
//		}
//		if (valueXml == "true")
//		{
//			runParams.hv_IsAbsCoordinate = true;
//		}
//		else
//		{
//			runParams.hv_IsAbsCoordinate = false;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseCenterCalib", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取机器人标定参数hv_IsUseCenterCalib失败");
//			return errorCode;
//		}
//		if (valueXml == "true")
//		{
//			runParams.hv_IsUseCenterCalib = true;
//		}
//		else
//		{
//			runParams.hv_IsUseCenterCalib = false;
//		}
//
//		//九点标定矩阵加载
//		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
//		HTuple isExist;
//		FileExists(PathTuple, &isExist);
//		if (isExist > 0)
//		{
//			ReadTuple(PathTuple, &runParams.hv_HomMat2D);
//		}
//
//
//		WriteTxt("机器人标定参数读取成功!");
//		//mutex1.unlock();
//		return Ok_Xml;
//
//	}
//	catch (...)
//	{
//		WriteTxt("XML读取代码崩溃，机器人标定参数读取失败!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}

////保存Pin针检测参数(避免使用科学计数法)
//int DataIO::WriteParams_PinDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//		//写入数组(数组按照逗号分割存储)
//		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("X方向基准参数为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", to_string(runParams.hv_StandardX_Array.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_StandardX_Array[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_StandardX_Array[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_StandardX_Array[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("Y方向基准参数为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", to_string(runParams.hv_StandardY_Array.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_StandardY_Array[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_StandardY_Array[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_StandardY_Array[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("Pin区域绑定的X基准编号为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", to_string(runParams.hv_X_BindingNum.I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_X_BindingNum[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_X_BindingNum[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_X_BindingNum[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("Pin区域绑定的Y基准编号为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", to_string(runParams.hv_Y_BindingNum.I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Y_BindingNum[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Y_BindingNum[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Y_BindingNum[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("基准类型为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", runParams.hv_BaseType.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_BaseType[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_BaseType[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_BaseType[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("直线起点row，或者圆心row为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", to_string(runParams.hv_Row1_Base.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Row1_Base[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Row1_Base[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Row1_Base[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("直线起点column，或者圆心column为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", to_string(runParams.hv_Column1_Base.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Column1_Base[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Column1_Base[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Column1_Base[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("直线终点row为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", to_string(runParams.hv_Row2_Base.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Row2_Base[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Row2_Base[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Row2_Base[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("直线终点column为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", to_string(runParams.hv_Column2_Base.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Column2_Base[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Column2_Base[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Column2_Base[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("圆半径为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", to_string(runParams.hv_Radius_Base.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Radius_Base[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Radius_Base[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Radius_Base[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("卡尺半长为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureLength1[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureLength1[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureLength1[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("卡尺半宽为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureLength2[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureLength2[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureLength2[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("平滑值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureSigma[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureSigma[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureSigma[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("边缘阈值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureThreshold[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureThreshold[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureThreshold[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("卡尺极性为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureTransition[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureTransition[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureTransition[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("边缘选择为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureSelect[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureSelect[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureSelect[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("卡尺个数为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureNum[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureNum[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureNum[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("最小得分为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MeasureMinScore[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MeasureMinScore[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MeasureMinScore[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("距离阈值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_DistanceThreshold[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_DistanceThreshold[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_DistanceThreshold[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("检测类型为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", runParams.hv_DetectType.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_DetectType[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_DetectType[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_DetectType[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("排序方式为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", runParams.hv_SortType.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_SortType[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_SortType[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_SortType[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("二值化类型为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ThresholdType[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ThresholdType[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ThresholdType[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("低阈值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_LowThreshold[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_LowThreshold[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_LowThreshold[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("高阈值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_HighThreshold[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_HighThreshold[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_HighThreshold[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("sigma为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_Sigma[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_Sigma[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_Sigma[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("滤波核宽度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CoreWidth[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CoreWidth[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("滤波核高度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CoreHeight[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CoreHeight[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("动态阈值分割图像对比度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_DynThresholdContrast[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_DynThresholdContrast[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_DynThresholdContrast[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("动态阈值极性选择为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_DynThresholdPolarity[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_DynThresholdPolarity[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_DynThresholdPolarity[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选面积Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_AreaFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_AreaFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_AreaFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选半长Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RecLen1Filter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RecLen1Filter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RecLen1Filter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选半宽Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RecLen2Filter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RecLen2Filter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RecLen2Filter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选行坐标Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RowFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RowFilter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RowFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RowFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选列坐标Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_ColumnFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_ColumnFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_ColumnFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_CircularityFilter_Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", to_string(runParams.hv_CircularityFilter_Min.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CircularityFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CircularityFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CircularityFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_RectangularityFilter_Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", to_string(runParams.hv_RectangularityFilter_Min.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_RectangularityFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_RectangularityFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_RectangularityFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_WidthFilter_Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_WidthFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_WidthFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_WidthFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_HeightFilter_Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_HeightFilter_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_HeightFilter_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_HeightFilter_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选面积Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_AreaFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_AreaFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_AreaFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选半长Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RecLen1Filter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RecLen1Filter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RecLen1Filter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选半宽Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RecLen2Filter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RecLen2Filter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RecLen2Filter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选行坐标Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_RowFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_RowFilter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_RowFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_RowFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选列坐标Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_ColumnFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_ColumnFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_ColumnFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_CircularityFilter_Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", to_string(runParams.hv_CircularityFilter_Max.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_CircularityFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_CircularityFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_CircularityFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_RectangularityFilter_Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", to_string(runParams.hv_RectangularityFilter_Max.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_RectangularityFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_RectangularityFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_RectangularityFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_WidthFilter_Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_WidthFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_WidthFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_WidthFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("筛选hv_HeightFilter_Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_HeightFilter_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_HeightFilter_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_HeightFilter_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("hv_FillUpShape_Min为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_FillUpShape_Min[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_FillUpShape_Min[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_FillUpShape_Min[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("hv_FillUpShape_Max为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
//		}
//		else
//		{
//			//保留11位有效位数(不是小数位)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[0].D();
//			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
//			//HTuple typeTemp = runParams.hv_FillUpShape_Max[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				std::stringstream ss;
//				ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[i].D();
//				if (i == typeCount - 1)
//				{
//					typeTemp += HTuple(ss.str().c_str());
//					//typeTemp += runParams.hv_FillUpShape_Max[i];
//				}
//				else
//				{
//					typeTemp += HTuple(ss.str().c_str()) + ",";
//					//typeTemp += runParams.hv_FillUpShape_Max[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("形态学类型为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", runParams.hv_OperatorType.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_OperatorType[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_OperatorType[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_OperatorType[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("形态学宽为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", to_string(runParams.hv_OperaRec_Width.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_OperaRec_Width[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_OperaRec_Width[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_OperaRec_Width[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("形态学高为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", to_string(runParams.hv_OperaRec_Height.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_OperaRec_Height[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_OperaRec_Height[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_OperaRec_Height[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("形态学半径为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", to_string(runParams.hv_OperaCir_Radius.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_OperaCir_Radius[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_OperaCir_Radius[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_OperaCir_Radius[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("hv_SelectAreaMax为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", to_string(runParams.hv_SelectAreaMax.I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_SelectAreaMax[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_SelectAreaMax[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_SelectAreaMax[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("hv_BlobCount为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", to_string(runParams.hv_BlobCount.TupleInt().I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_BlobCount[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_BlobCount[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_BlobCount[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", typeTemp.S().Text());
//		}
//
//		//保存其余参数
//		//允许的偏差范围
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", to_string(runParams.hv_Tolerance_X.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", to_string(runParams.hv_Tolerance_Y.D()));
//		//像素当量
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.D()));
//		//CSV路径
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", runParams.FilePath_Csv);
//
//		//保存模板匹配参数
//		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板匹配类型为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", runParams.hv_MatchMethod.S().Text());
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MatchMethod[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MatchMethod[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MatchMethod[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板起始角度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(runParams.hv_AngleStart.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_AngleStart[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_AngleStart[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_AngleStart[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板终止角度为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(runParams.hv_AngleExtent.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_AngleExtent[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_AngleExtent[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_AngleExtent[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板行缩放最小值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(runParams.hv_ScaleRMin.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ScaleRMin[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ScaleRMin[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ScaleRMin[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板行缩放最大值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(runParams.hv_ScaleRMax.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ScaleRMax[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ScaleRMax[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ScaleRMax[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板列缩放最小值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(runParams.hv_ScaleCMin.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ScaleCMin[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ScaleCMin[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ScaleCMin[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板列缩放最大值为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(runParams.hv_ScaleCMax.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_ScaleCMax[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_ScaleCMax[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_ScaleCMax[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板最小分数为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(runParams.hv_MinScore.D()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_MinScore[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_MinScore[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_MinScore[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", typeTemp.S().Text());
//		}
//
//		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("模板匹配个数为空，保存失败");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(runParams.hv_NumMatches.I()));
//		}
//		else
//		{
//			HTuple typeTemp = runParams.hv_NumMatches[0] + ",";
//			for (int i = 1; i < typeCount; i++)
//			{
//				if (i == typeCount - 1)
//				{
//					typeTemp += runParams.hv_NumMatches[i];
//				}
//				else
//				{
//					typeTemp += runParams.hv_NumMatches[i] + ",";
//				}
//			}
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", typeTemp.S().Text());
//		}
//
//		//保存模板匹配模型
//		if (runParams.hv_ModelID.TupleLength() > 0)
//		{
//			TypeParamsStruct_Pin mType;
//			for (int i = 0; i < runParams.hv_ModelID.TupleLength(); i++)
//			{
//				if (runParams.hv_DetectType[i] == mType.DetectType_Template)
//				{
//					if (runParams.hv_MatchMethod[i] == mType.NccModel)
//					{
//						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
//						WriteNccModel(runParams.hv_ModelID[i], modelIDPath);
//					}
//					else
//					{
//						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
//						WriteShapeModel(runParams.hv_ModelID[i], modelIDPath);
//					}
//				}
//			}
//		}
//		else
//		{
//			WriteTxt("模板匹配模型ID为空");
//		}
//
//		//保存运行区域
//		if (runParams.ho_SearchRegion.Key() == nullptr)
//		{
//			//图像为空，或者图像被clear
//			WriteTxt("搜索区域为空，保存失败");
//		}
//		else
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
//		}
//
//		//保存训练区域
//		if (runParams.ho_TrainRegion.Key() == nullptr)
//		{
//			//图像为空，或者图像被clear
//			WriteTxt("训练区域为空，保存失败");
//		}
//		else
//		{
//			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
//			WriteRegion(runParams.ho_TrainRegion, PathTrainRegion);
//		}
//
//
//		//保存位置修正模板匹配参数
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", runParams.hv_MatchMethod_PositionCorrection.S().Text());
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", to_string(runParams.hv_AngleStart_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", to_string(runParams.hv_AngleExtent_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", to_string(runParams.hv_ScaleRMin_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", to_string(runParams.hv_ScaleRMax_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", to_string(runParams.hv_ScaleCMin_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", to_string(runParams.hv_ScaleCMax_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", to_string(runParams.hv_MinScore_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", to_string(runParams.hv_NumMatches_PositionCorrection.TupleInt().I()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", to_string(runParams.hv_Check_PositionCorrection.TupleInt().I()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", to_string(runParams.hv_Row_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", to_string(runParams.hv_Column_PositionCorrection.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", to_string(runParams.hv_Angle_PositionCorrection.D()));
//		//保存位置修正模板匹配模型
//		try
//		{
//			TypeParamsStruct_Pin mType;
//			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
//			{
//				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//					+ "_ModelId_PositionCorrection.ncm";
//				WriteNccModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
//			}
//			else
//			{
//				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//					+ "_ModelId_PositionCorrection.shm";
//				WriteShapeModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
//			}
//
//		}
//		catch (...)
//		{
//			WriteTxt("位置修正模板匹配模型ID为空");
//		}
//		//保存位置修正的运行区域
//		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
//		{
//			//图像为空，或者图像被clear
//			WriteTxt("位置修正搜索区域为空，保存失败");
//		}
//		else
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
//			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
//		}
//		//保存位置修正的训练区域
//		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
//		{
//			//图像为空，或者图像被clear
//			WriteTxt("位置修正训练区域为空，保存失败");
//		}
//		else
//		{
//			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
//			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
//		}
//
//
//		WriteTxt("Pin针检测参数保存成功!");
//		//mutex1.unlock();
//		return 0;
//	}
//	catch (...)
//	{
//		WriteTxt("XML保存代码崩溃，Pin针检测参数保存失败!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////读取Pin针检测参数
//ErrorCode_Xml DataIO::ReadParams_PinDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml存取变量
//		//路径判断
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//		double tempDoubleValue;
//
//		//读取数组(数组按照逗号分割存储)
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取Pin检测参数hv_StandardX_Array失败");
//			return errorCode;
//		}
//		vector<string> typeArray;
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_StandardX_Array.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_StandardX_Array.Append(tempDoubleValue);
//		}
//		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_StandardX_Array失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读参数hv_StandardY_Array失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_StandardY_Array.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_StandardY_Array.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_StandardY_Array失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML参数hv_X_BindingNum失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_X_BindingNum.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_X_BindingNum.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("取参数hv_X_BindingNum失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Y_BindingNum失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Y_BindingNum.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_Y_BindingNum.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Y_BindingNum失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_BaseType失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_BaseType.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_BaseType.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_BaseType失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Row1_Base失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Row1_Base.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Row1_Base.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Row1_Base失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Column1_Base失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Column1_Base.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Column1_Base.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Column1_Base失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Row2_Base失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Row2_Base.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Row2_Base.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Row2_Base失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Column2_Base失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Column2_Base.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Column2_Base.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Column2_Base失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Radius_Base失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Radius_Base.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Radius_Base.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Radius_Base失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureLength1失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureLength1.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureLength1.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureLength1失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureLength2失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureLength2.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureLength2.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureLength2失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureSigma失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureSigma.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureSigma.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureSigma失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureThreshold失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureThreshold.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureThreshold.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureThreshold失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureTransition失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureTransition.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_MeasureTransition.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureTransition失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureSelect失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureSelect.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_MeasureSelect.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureSelect失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureNum失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureNum.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureNum.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureNum失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MeasureMinScore失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MeasureMinScore.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MeasureMinScore.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MeasureMinScore失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_DistanceThreshold失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_DistanceThreshold.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_DistanceThreshold.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_DistanceThreshold失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_DetectType失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_DetectType.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_DetectType.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_DetectType失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_SortType失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_SortType.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_SortType.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_SortType失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ThresholdType失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ThresholdType.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_ThresholdType.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ThresholdType失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_LowThreshold失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_LowThreshold.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_LowThreshold.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_LowThreshold失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_HighThreshold失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_HighThreshold.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_HighThreshold.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_HighThreshold失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Sigma失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_Sigma.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_Sigma.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_Sigma失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_CoreWidth失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CoreWidth.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CoreWidth.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_CoreWidth失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_CoreHeight失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CoreHeight.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CoreHeight.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_CoreHeight失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_DynThresholdContrast失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_DynThresholdContrast.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_DynThresholdContrast.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_DynThresholdContrast失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_DynThresholdPolarity失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_DynThresholdPolarity.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_DynThresholdPolarity.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_DynThresholdPolarity失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AreaFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_AreaFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_AreaFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_AreaFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RecLen1Filter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RecLen1Filter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RecLen1Filter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RecLen1Filter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RecLen2Filter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RecLen2Filter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RecLen2Filter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RecLen2Filter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RowFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RowFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RowFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RowFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ColumnFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ColumnFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ColumnFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ColumnFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_CircularityFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CircularityFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CircularityFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_CircularityFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RectangularityFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RectangularityFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RectangularityFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RectangularityFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_WidthFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_WidthFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_WidthFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_WidthFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_HeightFilter_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_HeightFilter_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_HeightFilter_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_HeightFilter_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AreaFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_AreaFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_AreaFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_AreaFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RecLen1Filter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RecLen1Filter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RecLen1Filter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RecLen1Filter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RecLen2Filter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RecLen2Filter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RecLen2Filter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RecLen2Filter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RowFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RowFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RowFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RowFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ColumnFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ColumnFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ColumnFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ColumnFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_CircularityFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_CircularityFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_CircularityFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_CircularityFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_RectangularityFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_RectangularityFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_RectangularityFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_RectangularityFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_WidthFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_WidthFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_WidthFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_WidthFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_HeightFilter_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_HeightFilter_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_HeightFilter_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_HeightFilter_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_FillUpShape_Min失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_FillUpShape_Min.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_FillUpShape_Min.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_FillUpShape_Min失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_FillUpShape_Max失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_FillUpShape_Max.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_FillUpShape_Max.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_FillUpShape_Max失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_OperatorType失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_OperatorType.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_OperatorType.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_OperatorType失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_OperaRec_Width失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_OperaRec_Width.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_OperaRec_Width.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_OperaRec_Width失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_OperaRec_Height失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_OperaRec_Height.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_OperaRec_Height.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_OperaRec_Height失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_OperaCir_Radius失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_OperaCir_Radius.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_OperaCir_Radius.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_OperaCir_Radius失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_SelectAreaMax失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_SelectAreaMax.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_SelectAreaMax.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_SelectAreaMax失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_BlobCount失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_BlobCount.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_BlobCount.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_BlobCount失败");
//			return nullKeyName_Xml;
//		}
//
//		//读取其余参数
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取Pin测参数hv_Tolerance_X失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Tolerance_X = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取Pin测参数hv_Tolerance_Y失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Tolerance_Y = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取Pin测参数hv_MmPixel失败");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_MmPixel = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取Pin测参数FilePath_Csv失败");
//			return errorCode;
//		}
//		runParams.FilePath_Csv = valueXml;
//
//
//		//读取模板匹配参数
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MatchMethod失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MatchMethod.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			runParams.hv_MatchMethod.Append(HTuple(typeArray[i].c_str()));
//		}
//		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MatchMethod失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AngleStart失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_AngleStart.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_AngleStart.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_AngleStart失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AngleExtent失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_AngleExtent.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_AngleExtent.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_AngleExtent失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleRMin失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ScaleRMin.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ScaleRMin.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ScaleRMin失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleRMax失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ScaleRMax.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ScaleRMax.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ScaleRMax失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleCMin失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ScaleCMin.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ScaleCMin.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ScaleCMin失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleCMax失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_ScaleCMax.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_ScaleCMax.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_ScaleCMax失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MinScore失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_MinScore.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
//			runParams.hv_MinScore.Append(tempDoubleValue);
//		}
//		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_MinScore失败");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_NumMatches失败");
//			return errorCode;
//		}
//		typeArray.clear();
//		stringToken(valueXml, ",", typeArray);
//		runParams.hv_NumMatches.Clear();
//		for (int i = 0; i < typeArray.size(); i++)
//		{
//			xmlC.StrToInt(typeArray[i], tempIntValue);
//			runParams.hv_NumMatches.Append(tempIntValue);
//		}
//		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("读取参数hv_NumMatches失败");
//			return nullKeyName_Xml;
//		}
//
//
//		//运行区域加载
//		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//		HTuple isExist;
//		FileExists(PathRunRegion, &isExist);
//		if (isExist > 0)
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion);
//			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
//		}
//		else
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion);
//			runParams.ho_SearchRegion.Clear();
//		}
//
//		//训练区域加载
//		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
//		FileExists(PathTrainRegion, &isExist);
//		if (isExist > 0)
//		{
//			GenEmptyObj(&runParams.ho_TrainRegion);
//			ReadRegion(&runParams.ho_TrainRegion, PathTrainRegion);
//		}
//		else
//		{
//			GenEmptyObj(&runParams.ho_TrainRegion);
//			runParams.ho_TrainRegion.Clear();
//		}
//
//		//读取模板匹配模型
//		runParams.hv_ModelID.Clear();
//		HTuple countRegion1;
//		CountObj(runParams.ho_SearchRegion, &countRegion1);
//		TypeParamsStruct_Pin mType;
//		for (int i = 0; i < countRegion1; i++)
//		{
//			if (runParams.hv_DetectType[i] == mType.DetectType_Template)
//			{
//				if (runParams.hv_MatchMethod[i] == mType.NccModel)
//				{
//					HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//						+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
//					HTuple ModelID;
//					ReadNccModel(modelIDPath, &ModelID);
//					runParams.hv_ModelID.Append(ModelID);
//				}
//				else if (runParams.hv_MatchMethod[i] == mType.ShapeModel)
//				{
//					HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//						+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
//					HTuple ModelID;
//					ReadShapeModel(modelIDPath, &ModelID);
//					runParams.hv_ModelID.Append(ModelID);
//				}
//				else
//				{
//					//计算没有此类型，也需要添加空句柄，保证数组长度一致
//					HTuple emptyID = NULL;
//					runParams.hv_ModelID.Append(emptyID);
//				}
//			}
//			else
//			{
//				//计算没有此类型，也需要添加空句柄，保证数组长度一致
//				HTuple emptyID = NULL;
//				runParams.hv_ModelID.Append(emptyID);
//			}
//		}
//
//
//
//		//读取位置修正模板匹配参数
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MatchMethod_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_MatchMethod_PositionCorrection.Clear();
//			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AngleStart_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_AngleStart_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_AngleStart_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_AngleExtent_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_AngleExtent_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_AngleExtent_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleRMin_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_ScaleRMin_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_ScaleRMin_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleRMax_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_ScaleRMax_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_ScaleRMax_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleCMin_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_ScaleCMin_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_ScaleCMin_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_ScaleCMax_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_ScaleCMax_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_ScaleCMax_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_MinScore_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_MinScore_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_MinScore_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_NumMatches_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_NumMatches_PositionCorrection.Clear();
//			xmlC.StrToInt(valueXml, tempIntValue);
//			runParams.hv_NumMatches_PositionCorrection = tempIntValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Check_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_Check_PositionCorrection.Clear();
//			xmlC.StrToInt(valueXml, tempIntValue);
//			runParams.hv_Check_PositionCorrection = tempIntValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Row_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_Row_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_Row_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Column_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_Column_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_Column_PositionCorrection = tempDoubleValue;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML读取参数hv_Angle_PositionCorrection失败");
//		}
//		else
//		{
//			runParams.hv_Angle_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
//		}
//
//		//位置修正搜索区域加载
//		PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
//		FileExists(PathRunRegion, &isExist);
//		if (isExist > 0)
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
//			ReadRegion(&runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
//		}
//		else
//		{
//			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
//			runParams.ho_SearchRegion_PositionCorrection.Clear();
//		}
//
//		//位置修正训练区域加载
//		PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
//		FileExists(PathTrainRegion, &isExist);
//		if (isExist > 0)
//		{
//			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
//			ReadRegion(&runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
//		}
//		else
//		{
//			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
//			runParams.ho_TrainRegion_PositionCorrection.Clear();
//		}
//		//读取位置修正，模板匹配模型
//		try
//		{
//			runParams.hv_ModelID_PositionCorrection.Clear();
//			TypeParamsStruct_Pin mType;
//			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
//			{
//				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//					+ "_ModelId_PositionCorrection.ncm";
//				HTuple ModelID;
//				ReadNccModel(modelIDPath, &ModelID);
//				runParams.hv_ModelID_PositionCorrection = ModelID;
//			}
//			else if (runParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel)
//			{
//				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
//					+ "_ModelId_PositionCorrection.shm";
//				HTuple ModelID;
//				ReadShapeModel(modelIDPath, &ModelID);
//				runParams.hv_ModelID_PositionCorrection = ModelID;
//			}
//			else
//			{
//				//计算没有此类型，也需要添加空句柄，保证数组长度一致
//				runParams.hv_ModelID_PositionCorrection = NULL;
//			}
//		}
//		catch (...)
//		{
//			WriteTxt("PCB检测参数hv_ModelID_PositionCorrection读取失败!");
//		}
//
//
//		WriteTxt("Pin检测参数读取成功!");
//		//mutex1.unlock();
//		return Ok_Xml;
//
//	}
//	catch (...)
//	{
//		WriteTxt("XML读取代码崩溃，Pin检测参数读取失败!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}
//保存Pcb检测参数(避免使用科学计数法)
int DataIO::WriteParams_PcbDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//写入数组(数组按照逗号分割存储)
		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("X方向基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", to_string(runParams.hv_StandardX_Array.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_StandardX_Array[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_StandardX_Array[i];
				}
				else
				{
					typeTemp += runParams.hv_StandardX_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("Y方向基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", to_string(runParams.hv_StandardY_Array.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_StandardY_Array[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_StandardY_Array[i];
				}
				else
				{
					typeTemp += runParams.hv_StandardY_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_StandardA_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("角度基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardA_Array", to_string(runParams.hv_StandardA_Array.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_StandardA_Array[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_StandardA_Array[i];
				}
				else
				{
					typeTemp += runParams.hv_StandardA_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardA_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_StandardArea_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("面积基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardArea_Array", to_string(runParams.hv_StandardArea_Array.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_StandardArea_Array[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_StandardArea_Array[i];
				}
				else
				{
					typeTemp += runParams.hv_StandardArea_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardArea_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("Pin区域绑定的X基准编号为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", to_string(runParams.hv_X_BindingNum.I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_X_BindingNum[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_X_BindingNum[i];
				}
				else
				{
					typeTemp += runParams.hv_X_BindingNum[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("Pin区域绑定的Y基准编号为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", to_string(runParams.hv_Y_BindingNum.I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_Y_BindingNum[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_Y_BindingNum[i];
				}
				else
				{
					typeTemp += runParams.hv_Y_BindingNum[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", typeTemp.S().Text());
		}

		typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", "null");
		}
		else
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", runParams.hv_BaseType.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_BaseType[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_BaseType[i];
					}
					else
					{
						typeTemp += runParams.hv_BaseType[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", to_string(runParams.hv_Row1_Base.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_Row1_Base[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_Row1_Base[i];
					}
					else
					{
						typeTemp += runParams.hv_Row1_Base[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", to_string(runParams.hv_Column1_Base.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_Column1_Base[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_Column1_Base[i];
					}
					else
					{
						typeTemp += runParams.hv_Column1_Base[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", to_string(runParams.hv_Row2_Base.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_Row2_Base[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_Row2_Base[i];
					}
					else
					{
						typeTemp += runParams.hv_Row2_Base[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", to_string(runParams.hv_Column2_Base.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_Column2_Base[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_Column2_Base[i];
					}
					else
					{
						typeTemp += runParams.hv_Column2_Base[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", typeTemp.S().Text());
			}
		}


		typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", to_string(runParams.hv_Radius_Base.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_Radius_Base[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_Radius_Base[i];
					}
					else
					{
						typeTemp += runParams.hv_Radius_Base[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", typeTemp.S().Text());
			}
		}


		typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureLength1[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureLength1[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureLength1[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureLength2[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureLength2[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureLength2[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureSigma[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureSigma[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureSigma[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureThreshold[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureThreshold[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureThreshold[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureTransition[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureTransition[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureTransition[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureSelect[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureSelect[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureSelect[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureNum[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureNum[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureNum[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_MeasureMinScore[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_MeasureMinScore[i];
					}
					else
					{
						typeTemp += runParams.hv_MeasureMinScore[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			if (typeCount == 1)
			{
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.D()));
			}
			else
			{
				HTuple typeTemp = runParams.hv_DistanceThreshold[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_DistanceThreshold[i];
					}
					else
					{
						typeTemp += runParams.hv_DistanceThreshold[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", typeTemp.S().Text());
			}
		}

		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("检测类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", runParams.hv_DetectType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_DetectType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DetectType[i];
				}
				else
				{
					typeTemp += runParams.hv_DetectType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("排序方式为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", runParams.hv_SortType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_SortType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_SortType[i];
				}
				else
				{
					typeTemp += runParams.hv_SortType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("二值化类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_ThresholdType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ThresholdType[i];
				}
				else
				{
					typeTemp += runParams.hv_ThresholdType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("低阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_LowThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_LowThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_LowThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("高阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_HighThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_HighThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_HighThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("sigma为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_Sigma[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_Sigma[i];
				}
				else
				{
					typeTemp += runParams.hv_Sigma[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核宽度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreWidth[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreWidth[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核高度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreHeight[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreHeight[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值分割图像对比度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdContrast[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdContrast[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdContrast[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值极性选择为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdPolarity[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选面积Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半长Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半宽Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选行坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RowFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选列坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_CircularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", to_string(runParams.hv_CircularityFilter_Min.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CircularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CircularityFilter_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_CircularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_RectangularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", to_string(runParams.hv_RectangularityFilter_Min.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_RectangularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_RectangularityFilter_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_RectangularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_WidthFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_HeightFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选面积Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半长Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半宽Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选行坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RowFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选列坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_CircularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", to_string(runParams.hv_CircularityFilter_Max.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CircularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CircularityFilter_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_CircularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_RectangularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", to_string(runParams.hv_RectangularityFilter_Max.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_RectangularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_RectangularityFilter_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_RectangularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_WidthFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_HeightFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", runParams.hv_OperatorType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperatorType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperatorType[i];
				}
				else
				{
					typeTemp += runParams.hv_OperatorType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学宽为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", to_string(runParams.hv_OperaRec_Width.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Width[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Width[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Width[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学高为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", to_string(runParams.hv_OperaRec_Height.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Height[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Height[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Height[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学半径为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", to_string(runParams.hv_OperaCir_Radius.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaCir_Radius[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaCir_Radius[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaCir_Radius[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", typeTemp.S().Text());
		}

		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_SelectAreaMax为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", to_string(runParams.hv_SelectAreaMax.I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_SelectAreaMax[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_SelectAreaMax[i];
				}
				else
				{
					typeTemp += runParams.hv_SelectAreaMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_BlobCount为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", to_string(runParams.hv_BlobCount.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_BlobCount[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_BlobCount[i];
				}
				else
				{
					typeTemp += runParams.hv_BlobCount[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", typeTemp.S().Text());
		}

		//保存其余参数
		//允许的偏差范围
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", to_string(runParams.hv_Tolerance_X.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", to_string(runParams.hv_Tolerance_Y.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_A", to_string(runParams.hv_Tolerance_A.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Area", to_string(runParams.hv_Tolerance_Area.D()));
		//像素当量
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.D()));

		//保存模板匹配参数
		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板匹配类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", runParams.hv_MatchMethod.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_MatchMethod[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_MatchMethod[i];
				}
				else
				{
					typeTemp += runParams.hv_MatchMethod[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板起始角度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(runParams.hv_AngleStart.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_AngleStart[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_AngleStart[i];
				}
				else
				{
					typeTemp += runParams.hv_AngleStart[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板终止角度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(runParams.hv_AngleExtent.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_AngleExtent[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_AngleExtent[i];
				}
				else
				{
					typeTemp += runParams.hv_AngleExtent[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板行缩放最小值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(runParams.hv_ScaleRMin.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ScaleRMin[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ScaleRMin[i];
				}
				else
				{
					typeTemp += runParams.hv_ScaleRMin[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板行缩放最大值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(runParams.hv_ScaleRMax.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ScaleRMax[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ScaleRMax[i];
				}
				else
				{
					typeTemp += runParams.hv_ScaleRMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板列缩放最小值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(runParams.hv_ScaleCMin.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ScaleCMin[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ScaleCMin[i];
				}
				else
				{
					typeTemp += runParams.hv_ScaleCMin[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板列缩放最大值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(runParams.hv_ScaleCMax.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ScaleCMax[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ScaleCMax[i];
				}
				else
				{
					typeTemp += runParams.hv_ScaleCMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板最小分数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(runParams.hv_MinScore.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_MinScore[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_MinScore[i];
				}
				else
				{
					typeTemp += runParams.hv_MinScore[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", typeTemp.S().Text());
		}

		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("模板匹配个数为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(runParams.hv_NumMatches.I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_NumMatches[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_NumMatches[i];
				}
				else
				{
					typeTemp += runParams.hv_NumMatches[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", typeTemp.S().Text());
		}

		//保存模板匹配模型
		if (runParams.hv_ModelID.TupleLength() > 0)
		{
			TypeParamsStruct_Pcb mType;
			for (int i = 0; i < runParams.hv_ModelID.TupleLength(); i++)
			{
				if (runParams.hv_DetectType[i] == mType.DetectType_Template)
				{
					if (runParams.hv_MatchMethod[i] == mType.NccModel)
					{
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
						WriteNccModel(runParams.hv_ModelID[i], modelIDPath);
					}
					else
					{
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
						WriteShapeModel(runParams.hv_ModelID[i], modelIDPath);
					}
				}
			}
		}
		else
		{
			WriteTxt("模板匹配模型ID为空");
		}

		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		//保存训练区域
		if (runParams.ho_TrainRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(runParams.ho_TrainRegion, PathTrainRegion);
		}


		//保存位置修正模板匹配参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", runParams.hv_MatchMethod_PositionCorrection.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", to_string(runParams.hv_AngleStart_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", to_string(runParams.hv_AngleExtent_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", to_string(runParams.hv_ScaleRMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", to_string(runParams.hv_ScaleRMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", to_string(runParams.hv_ScaleCMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", to_string(runParams.hv_ScaleCMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", to_string(runParams.hv_MinScore_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", to_string(runParams.hv_NumMatches_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", to_string(runParams.hv_Check_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", to_string(runParams.hv_Row_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", to_string(runParams.hv_Column_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", to_string(runParams.hv_Angle_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType_PositionCorrection", runParams.hv_SortType_PositionCorrection.S().Text());
		//保存位置修正模板匹配模型
		try
		{
			TypeParamsStruct_Pcb mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				WriteNccModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}
			else
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				WriteShapeModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}

		}
		catch (...)
		{
			WriteTxt("位置修正模板匹配模型ID为空");
		}
		//保存位置修正的运行区域
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//保存位置修正的训练区域
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}


		WriteTxt("Pin针检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，Pin针检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取Pcb检测参数
ErrorCode_Xml DataIO::ReadParams_PcbDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin检测参数hv_StandardX_Array失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardX_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardX_Array.Append(tempDoubleValue);
		}
		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_StandardX_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读参数hv_StandardY_Array失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardY_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardY_Array.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_StandardY_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardA_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读参数hv_StandardA_Array失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardA_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardA_Array.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_StandardA_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_StandardA_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardArea_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读参数hv_StandardArea_Array失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardArea_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardArea_Array.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_StandardArea_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_StandardArea_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML参数hv_X_BindingNum失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_X_BindingNum.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_X_BindingNum.Append(tempIntValue);
		}
		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("取参数hv_X_BindingNum失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Y_BindingNum失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Y_BindingNum.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_Y_BindingNum.Append(tempIntValue);
		}
		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_Y_BindingNum失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", valueXml);
		if (errorCode != Ok_Xml)
		{
			runParams.hv_BaseType = "null";
		}
		else
		{
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_BaseType.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				runParams.hv_BaseType.Append(HTuple(typeArray[i].c_str()));
			}
			typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				runParams.hv_BaseType = "null";
			}
		}

		if (runParams.hv_BaseType != "null")
		{
			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", valueXml);
			if (errorCode != Ok_Xml)
			{
				runParams.hv_Row1_Base.Clear();
				return errorCode;
			}
			else
			{
				typeArray.clear();
				stringToken(valueXml, ",", typeArray);
				runParams.hv_Row1_Base.Clear();
				for (int i = 0; i < typeArray.size(); i++)
				{
					xmlC.StrToDouble(typeArray[i], tempDoubleValue);
					runParams.hv_Row1_Base.Append(tempDoubleValue);
				}
				typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_Row1_Base.Clear();
					return nullKeyName_Xml;
				}
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", valueXml);
			if (errorCode != Ok_Xml)
			{
				runParams.hv_Column1_Base.Clear();
				return errorCode;
			}
			else
			{
				typeArray.clear();
				stringToken(valueXml, ",", typeArray);
				runParams.hv_Column1_Base.Clear();
				for (int i = 0; i < typeArray.size(); i++)
				{
					xmlC.StrToDouble(typeArray[i], tempDoubleValue);
					runParams.hv_Column1_Base.Append(tempDoubleValue);
				}
				typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_Column1_Base.Clear();
					return nullKeyName_Xml;
				}
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", valueXml);
			if (errorCode != Ok_Xml)
			{
				runParams.hv_Row2_Base.Clear();
				return errorCode;
			}
			else
			{
				typeArray.clear();
				stringToken(valueXml, ",", typeArray);
				runParams.hv_Row2_Base.Clear();
				for (int i = 0; i < typeArray.size(); i++)
				{
					xmlC.StrToDouble(typeArray[i], tempDoubleValue);
					runParams.hv_Row2_Base.Append(tempDoubleValue);
				}
				typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_Row2_Base.Clear();
					return nullKeyName_Xml;
				}
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", valueXml);
			if (errorCode != Ok_Xml)
			{
				runParams.hv_Column2_Base.Clear();
				return errorCode;
			}
			else
			{
				typeArray.clear();
				stringToken(valueXml, ",", typeArray);
				runParams.hv_Column2_Base.Clear();
				for (int i = 0; i < typeArray.size(); i++)
				{
					xmlC.StrToDouble(typeArray[i], tempDoubleValue);
					runParams.hv_Column2_Base.Append(tempDoubleValue);
				}
				typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_Column2_Base.Clear();
					return nullKeyName_Xml;
				}
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", valueXml);
			if (errorCode != Ok_Xml)
			{
				runParams.hv_Radius_Base.Clear();
				return errorCode;
			}
			else
			{
				typeArray.clear();
				stringToken(valueXml, ",", typeArray);
				runParams.hv_Radius_Base.Clear();
				for (int i = 0; i < typeArray.size(); i++)
				{
					xmlC.StrToDouble(typeArray[i], tempDoubleValue);
					runParams.hv_Radius_Base.Append(tempDoubleValue);
				}
				typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_Radius_Base.Clear();
					return nullKeyName_Xml;
				}
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureLength1失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureLength1.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureLength1.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureLength1失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureLength2失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureLength2.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureLength2.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureLength2失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureSigma失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureSigma.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureSigma.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureSigma失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureThreshold失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureThreshold.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureThreshold.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureThreshold失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureTransition失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureTransition.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				runParams.hv_MeasureTransition.Append(HTuple(typeArray[i].c_str()));
			}
			typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureTransition失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureSelect失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureSelect.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				runParams.hv_MeasureSelect.Append(HTuple(typeArray[i].c_str()));
			}
			typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureSelect失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureNum失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureNum.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureNum.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureNum失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_MeasureMinScore失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_MeasureMinScore.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_MeasureMinScore.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_MeasureMinScore失败");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML读取参数hv_DistanceThreshold失败");
				return errorCode;
			}
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_DistanceThreshold.Clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_DistanceThreshold.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
			if (typeCount == 0)
			{
				WriteTxt("读取参数hv_DistanceThreshold失败");
				return nullKeyName_Xml;
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DetectType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DetectType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_DetectType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DetectType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_SortType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SortType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_SortType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_SortType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ThresholdType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ThresholdType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ThresholdType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ThresholdType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_LowThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_LowThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_LowThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_LowThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HighThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HighThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_HighThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HighThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Sigma失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Sigma.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Sigma.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_Sigma失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreWidth失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreWidth.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreWidth.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreWidth失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreHeight失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreHeight.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreHeight.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreHeight失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdContrast失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdContrast.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_DynThresholdContrast.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdContrast失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdPolarity失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdPolarity.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_DynThresholdPolarity.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdPolarity失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AreaFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AreaFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen1Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen1Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen2Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen2Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RowFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RowFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ColumnFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ColumnFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CircularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CircularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RectangularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RectangularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WidthFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_WidthFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HeightFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HeightFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AreaFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AreaFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen1Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen1Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen2Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen2Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RowFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RowFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ColumnFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ColumnFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CircularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CircularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RectangularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RectangularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WidthFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_WidthFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HeightFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HeightFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperatorType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperatorType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_OperatorType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperatorType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Width失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Width.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Width.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Width失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Height失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Height.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Height.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Height失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaCir_Radius失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaCir_Radius.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaCir_Radius.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaCir_Radius失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_SelectAreaMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SelectAreaMax.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_SelectAreaMax.Append(tempIntValue);
		}
		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_SelectAreaMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_BlobCount失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_BlobCount.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_BlobCount.Append(tempIntValue);
		}
		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_BlobCount失败");
			return nullKeyName_Xml;
		}

		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin测参数hv_Tolerance_X失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_X = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin测参数hv_Tolerance_Y失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_Y = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_A", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin测参数hv_Tolerance_A失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_A = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Area", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin测参数hv_Tolerance_Area失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_Area = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取Pin测参数hv_MmPixel失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		//读取模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MatchMethod失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MatchMethod.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_MatchMethod.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_MatchMethod失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleStart失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AngleStart.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AngleStart.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AngleStart失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleExtent失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AngleExtent.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AngleExtent.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AngleExtent失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMin失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleRMin.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleRMin.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ScaleRMin失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleRMax.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleRMax.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ScaleRMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMin失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleCMin.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleCMin.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ScaleCMin失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleCMax.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleCMax.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ScaleCMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MinScore失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MinScore.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MinScore.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_MinScore失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_NumMatches失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NumMatches.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_NumMatches.Append(tempIntValue);
		}
		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_NumMatches失败");
			return nullKeyName_Xml;
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		//训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_TrainRegion);
			ReadRegion(&runParams.ho_TrainRegion, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_TrainRegion);
			runParams.ho_TrainRegion.Clear();
		}

		//读取模板匹配模型
		runParams.hv_ModelID.Clear();
		HTuple countRegion1;
		CountObj(runParams.ho_SearchRegion, &countRegion1);
		TypeParamsStruct_Pcb mType;
		for (int i = 0; i < countRegion1; i++)
		{
			if (runParams.hv_DetectType[i] == mType.DetectType_Template)
			{
				if (runParams.hv_MatchMethod[i] == mType.NccModel)
				{
					HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
						+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
					HTuple ModelID;
					ReadNccModel(modelIDPath, &ModelID);
					runParams.hv_ModelID.Append(ModelID);
				}
				else if (runParams.hv_MatchMethod[i] == mType.ShapeModel)
				{
					HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
						+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
					HTuple ModelID;
					ReadShapeModel(modelIDPath, &ModelID);
					runParams.hv_ModelID.Append(ModelID);
				}
				else
				{
					//计算没有此类型，也需要添加空句柄，保证数组长度一致
					HTuple emptyID = NULL;
					runParams.hv_ModelID.Append(emptyID);
				}
			}
			else
			{
				//计算没有此类型，也需要添加空句柄，保证数组长度一致
				HTuple emptyID = NULL;
				runParams.hv_ModelID.Append(emptyID);
			}
		}

		//读取位置修正模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MatchMethod_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleStart_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleStart_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleStart_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleExtent_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleExtent_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleExtent_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MinScore_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MinScore_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_MinScore_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_NumMatches_PositionCorrection失败");
		}
		else
		{
			runParams.hv_NumMatches_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_NumMatches_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Check_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Check_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_Check_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Row_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Row_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Row_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Column_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Column_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Column_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Angle_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_SortType_PositionCorrection失败");
		}
		else
		{
			runParams.hv_SortType_PositionCorrection.Clear();
			runParams.hv_SortType_PositionCorrection = HTuple(valueXml.c_str());
		}

		//位置修正搜索区域加载
		PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			ReadRegion(&runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			runParams.ho_SearchRegion_PositionCorrection.Clear();
		}

		//位置修正训练区域加载
		PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			ReadRegion(&runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			runParams.ho_TrainRegion_PositionCorrection.Clear();
		}
		//读取位置修正，模板匹配模型
		try
		{
			runParams.hv_ModelID_PositionCorrection.Clear();
			TypeParamsStruct_Pcb mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				HTuple ModelID;
				ReadNccModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else if (runParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				HTuple ModelID;
				ReadShapeModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else
			{
				//计算没有此类型，也需要添加空句柄，保证数组长度一致
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("PCB检测参数hv_ModelID_PositionCorrection读取失败!");
		}


		WriteTxt("Pin检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，Pin检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存ColorSort检测参数
int DataIO::WriteParams_ColorSortDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		int typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("二值化类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_ThresholdType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ThresholdType[i];
				}
				else
				{
					typeTemp += runParams.hv_ThresholdType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("低阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_LowThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_LowThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_LowThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("高阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_HighThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_HighThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_HighThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("sigma为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_Sigma[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_Sigma[i];
				}
				else
				{
					typeTemp += runParams.hv_Sigma[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核宽度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreWidth[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreWidth[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核高度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreHeight[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreHeight[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值分割图像对比度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdContrast[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdContrast[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdContrast[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值极性选择为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdPolarity[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", runParams.hv_OperatorType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperatorType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperatorType[i];
				}
				else
				{
					typeTemp += runParams.hv_OperatorType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学宽为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", to_string(runParams.hv_OperaRec_Width.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Width[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Width[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Width[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学高为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", to_string(runParams.hv_OperaRec_Height.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Height[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Height[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Height[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学半径为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", to_string(runParams.hv_OperaCir_Radius.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaCir_Radius[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaCir_Radius[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaCir_Radius[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", typeTemp.S().Text());
		}

		//****************************区域筛选****************************************************************
		int typeNum = 0;
		//写入区域筛选类型
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
		}
		else
		{
			//写入区域筛选类型(面积，圆度等等)
			if (typeCount == 1)
			{
				if (runParams.hv_FeaturesFilter_Type == "null")
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
				}
				else
				{
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", runParams.hv_FeaturesFilter_Type.S().Text());
				}
			}
			else
			{
				HTuple typeTemp = runParams.hv_FeaturesFilter_Type[0] + ",";
				for (int i = 1; i < typeCount; i++)
				{
					if (i == typeCount - 1)
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i];
					}
					else
					{
						typeTemp += runParams.hv_FeaturesFilter_Type[i] + ",";
					}
				}
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", typeTemp.S().Text());
			}

			//写入筛选参数最小值
			typeCount = runParams.hv_MinValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					//保留11位有效位数(不是小数位)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MinValue_Filter.TupleReal().D();
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", ss.str());
					//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", to_string(runParams.hv_MinValue_Filter.TupleReal().D()));
				}
				else
				{
					//保留11位有效位数(不是小数位)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MinValue_Filter[0].D();
					HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
					//HTuple typeTemp = runParams.hv_MinValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						std::stringstream ss;
						ss << std::setprecision(11) << runParams.hv_MinValue_Filter[i].D();
						if (i == typeCount - 1)
						{
							typeTemp += HTuple(ss.str().c_str());
							//typeTemp += runParams.hv_MinValue_Filter[i];
						}
						else
						{
							typeTemp += HTuple(ss.str().c_str()) + ",";
							//typeTemp += runParams.hv_MinValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", typeTemp.S().Text());
				}
			}


			//写入筛选参数最大值
			typeCount = runParams.hv_MaxValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					//保留11位有效位数(不是小数位)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MaxValue_Filter.TupleReal().D();
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", ss.str());
					//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", to_string(runParams.hv_MaxValue_Filter.TupleReal().D()));
				}
				else
				{
					//保留11位有效位数(不是小数位)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MaxValue_Filter[0].D();
					HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
					//HTuple typeTemp = runParams.hv_MaxValue_Filter[0] + ",";
					for (int i = 1; i < typeCount; i++)
					{
						std::stringstream ss;
						ss << std::setprecision(11) << runParams.hv_MaxValue_Filter[i].D();
						if (i == typeCount - 1)
						{
							typeTemp += HTuple(ss.str().c_str());
							//typeTemp += runParams.hv_MaxValue_Filter[i];
						}
						else
						{
							typeTemp += HTuple(ss.str().c_str()) + ",";
							//typeTemp += runParams.hv_MaxValue_Filter[i] + ",";
						}
					}
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", typeTemp.S().Text());
				}
			}
			//写入与或
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", runParams.hv_And_Or_Filter.S().Text());
		}

		//保存其余参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", to_string(runParams.hv_ColorValue1_Min.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", to_string(runParams.hv_ColorValue1_Max.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", to_string(runParams.hv_ColorValue2_Min.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", to_string(runParams.hv_ColorValue2_Max.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", to_string(runParams.hv_ColorValue3_Min.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", to_string(runParams.hv_ColorValue3_Max.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_Product", runParams.hv_Channel_Product.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorSort", runParams.hv_Channel_ColorSort.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", to_string(runParams.hv_ColorArea_Min.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", to_string(runParams.hv_ColorArea_Max.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_UseBlob", to_string(runParams.hv_UseBlob.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_UseUnionSortRegion", to_string(runParams.hv_UseUnionSortRegion.TupleInt().I()));
		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("ColorSort检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，ColorSort检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取ColorSort检测参数
ErrorCode_Xml DataIO::ReadParams_ColorSortDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		vector<string> typeArray;
		int typeCount = 0;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ThresholdType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ThresholdType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ThresholdType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ThresholdType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_LowThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_LowThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_LowThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_LowThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HighThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HighThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_HighThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HighThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Sigma失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Sigma.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Sigma.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_Sigma失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreWidth失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreWidth.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreWidth.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreWidth失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreHeight失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreHeight.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreHeight.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreHeight失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdContrast失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdContrast.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_DynThresholdContrast.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdContrast失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdPolarity失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdPolarity.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_DynThresholdPolarity.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdPolarity失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperatorType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperatorType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_OperatorType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperatorType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Width失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Width.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Width.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Width失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Height失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Height.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Height.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Height失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaCir_Radius失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaCir_Radius.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaCir_Radius.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaCir_Radius失败");
			return nullKeyName_Xml;
		}

		//读取区域筛选类型
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			runParams.hv_FeaturesFilter_Type = "area";
			runParams.hv_MinValue_Filter = 1;
			runParams.hv_MaxValue_Filter = 999999999;
			runParams.hv_And_Or_Filter = "and";
		}
		else
		{
			if (valueXml != "null")
			{
				vector<string> typeArray;
				stringToken(valueXml, ",", typeArray);
				runParams.hv_FeaturesFilter_Type.Clear();
				TypeParamsStruct_ColorSort codeTypeStruct;
				for (int i = 0; i < typeArray.size(); i++)
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Area_Filter);
				}
				int typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
				if (typeCount == 0)
				{
					runParams.hv_FeaturesFilter_Type = "area";
					runParams.hv_MinValue_Filter = 1;
					runParams.hv_MaxValue_Filter = 999999999;
					runParams.hv_And_Or_Filter = "and";
				}
				else
				{
					//读取筛选参数最小值
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_MinValue_Filter失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MinValue_Filter.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MinValue_Filter.Append(tempDoubleValue);
						}
					}

					//读取筛选参数最大值
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_MaxValue_Filter失败");
						return errorCode;
					}
					if (valueXml != "null")
					{
						vector<string> typeArray;
						stringToken(valueXml, ",", typeArray);
						runParams.hv_MaxValue_Filter.Clear();
						for (int i = 0; i < typeArray.size(); i++)
						{
							xmlC.StrToDouble(typeArray[i], tempDoubleValue);
							runParams.hv_MaxValue_Filter.Append(tempDoubleValue);
						}
					}

					//读取与或
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML读取形态学检测参数hv_And_Or_Filter失败");
						return errorCode;
					}
					runParams.hv_And_Or_Filter = HTuple(valueXml.c_str());
				}
			}
			else
			{
				runParams.hv_FeaturesFilter_Type = "area";
				runParams.hv_MinValue_Filter = 1;
				runParams.hv_MaxValue_Filter = 999999999;
				runParams.hv_And_Or_Filter = "and";
			}
		}
		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue1_Min失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue1_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue1_Max失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue1_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue2_Min失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue2_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue2_Max失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue2_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue3_Min失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue3_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorValue3_Max失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue3_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_Product", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Channel_Product失败");
			return errorCode;
		}
		runParams.hv_Channel_Product = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorSort", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Channel_ColorSort失败");
			return errorCode;
		}
		runParams.hv_Channel_ColorSort = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorArea_Min失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorArea_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_ColorArea_Max失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorArea_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_UseBlob", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_UseBlob失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_UseBlob = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_UseUnionSortRegion", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取hv_UseUnionSortRegion失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_UseUnionSortRegion = tempIntValue;

		//位置修正训练区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}

		WriteTxt("ColorSort检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，ColorSort检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存ColorExtract检测参数
int DataIO::WriteParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//保存位置修正模板匹配参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", runParams.hv_MatchMethod_PositionCorrection.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", to_string(runParams.hv_AngleStart_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", to_string(runParams.hv_AngleExtent_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", to_string(runParams.hv_ScaleRMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", to_string(runParams.hv_ScaleRMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", to_string(runParams.hv_ScaleCMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", to_string(runParams.hv_ScaleCMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", to_string(runParams.hv_MinScore_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", to_string(runParams.hv_NumMatches_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", to_string(runParams.hv_Check_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", to_string(runParams.hv_Row_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", to_string(runParams.hv_Column_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", to_string(runParams.hv_Angle_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_PositionCorrection", runParams.hv_Channel_PositionCorrection.S().Text());
		//保存位置修正模板匹配模型
		try
		{
			TypeParamsStruct_ColorExtract mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				WriteNccModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}
			else
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				WriteShapeModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}

		}
		catch (...)
		{
			WriteTxt("位置修正模板匹配模型ID为空");
		}
		//保存位置修正的运行区域
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//保存位置修正的训练区域
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}

		//保存颜色抽取参数
		int typeCount = runParams.hv_ColorValue1_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", to_string(runParams.hv_ColorValue1_Min.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue1_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue1_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue1_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorValue1_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", to_string(runParams.hv_ColorValue1_Max.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue1_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue1_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue1_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorValue2_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", to_string(runParams.hv_ColorValue2_Min.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue2_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue2_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue2_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorValue2_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", to_string(runParams.hv_ColorValue2_Max.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue2_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue2_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue2_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorValue3_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", to_string(runParams.hv_ColorValue3_Min.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue3_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue3_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue3_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorValue3_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", to_string(runParams.hv_ColorValue3_Max.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ColorValue3_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ColorValue3_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_ColorValue3_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorArea_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", ss.str());
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColorArea_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColorArea_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", ss.str());
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColorArea_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Channel_ColorExtract.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorExtract", runParams.hv_Channel_ColorExtract.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_Channel_ColorExtract[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_Channel_ColorExtract[i];
				}
				else
				{
					typeTemp += runParams.hv_Channel_ColorExtract[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorExtract", typeTemp.S().Text());
		}
		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			return 1;
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		return -1;
	}
}
//读取ColorExtract检测参数
ErrorCode_Xml DataIO::ReadParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		vector<string> typeArray;
		int typeCount = 0;

		//读取位置修正模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MatchMethod_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleStart_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleStart_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleStart_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleExtent_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleExtent_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleExtent_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MinScore_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MinScore_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_MinScore_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_NumMatches_PositionCorrection失败");
		}
		else
		{
			runParams.hv_NumMatches_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_NumMatches_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Check_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Check_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_Check_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Row_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Row_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Row_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Column_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Column_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Column_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Angle_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		//位置修正搜索区域加载
		HTuple isExist;
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			ReadRegion(&runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			runParams.ho_SearchRegion_PositionCorrection.Clear();
		}

		//位置修正训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			ReadRegion(&runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			runParams.ho_TrainRegion_PositionCorrection.Clear();
		}
		//读取位置修正，模板匹配模型
		try
		{
			runParams.hv_ModelID_PositionCorrection.Clear();
			TypeParamsStruct_ColorExtract mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				HTuple ModelID;
				ReadNccModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else if (runParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				HTuple ModelID;
				ReadShapeModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else
			{
				//计算没有此类型，也需要添加空句柄，保证数组长度一致
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("检测参数hv_ModelID_PositionCorrection读取失败!");
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Channel_PositionCorrection失败");
			return errorCode;
		}
		runParams.hv_Channel_PositionCorrection = HTuple(valueXml.c_str());

		//读取颜色抽取参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue1_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue1_Min.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue1_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue1_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue1_Max.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue1_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue2_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue2_Min.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue2_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue2_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue2_Max.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue2_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue3_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue3_Min.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue3_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorValue3_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ColorValue3_Max.Append(tempIntValue);
		}
		typeCount = runParams.hv_ColorValue3_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorArea_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColorArea_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColorArea_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColorArea_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColorArea_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColorArea_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorExtract", valueXml);
		if (errorCode != Ok_Xml)
		{
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Channel_ColorExtract.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_Channel_ColorExtract.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_Channel_ColorExtract.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			return nullKeyName_Xml;
		}

		//运行区域加载
		PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}


		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存MachineLearning检测参数
int DataIO::WriteParams_MachineLearning(const string ConfigPath, const string XmlPath, const RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//写入数组(数组按照逗号分割存储)
		int typeCount = runParams.hv_ImgFilePathArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_ImgFilePathArray为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePathArray", runParams.hv_ImgFilePathArray.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_ImgFilePathArray[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ImgFilePathArray[i];
				}
				else
				{
					typeTemp += runParams.hv_ImgFilePathArray[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePathArray", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ClassNameArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_ClassNameArray为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNameArray", runParams.hv_ClassNameArray.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_ClassNameArray[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ClassNameArray[i];
				}
				else
				{
					typeTemp += runParams.hv_ClassNameArray[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNameArray", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ClassIDArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_ClassIDArray为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassIDArray", to_string(runParams.hv_ClassIDArray.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_ClassIDArray[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ClassIDArray[i];
				}
				else
				{
					typeTemp += runParams.hv_ClassIDArray[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassIDArray", typeTemp.S().Text());
		}

		//保存指定NG的ID
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NGID", to_string(runParams.hv_NGID.TupleInt().I()));
		//保存合并所有的检测区域
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUnionDetectROI", to_string(runParams.hv_IsUnionDetectROI.TupleInt().I()));

		//保存小图检测ROI区域
		if (runParams.ho_DetectROIArray.size() == 0)
		{
			//图像为空，或者图像被clear
			WriteTxt("ho_DetectROIArray为空，保存失败");
		}
		else
		{
			int countVec = runParams.ho_DetectROIArray.size();
			for (int i = 0; i < countVec; i++)
			{
				HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(i) + "_DetectROI.hobj";
				WriteRegion(runParams.ho_DetectROIArray[i], PathRunRegion);
			}
		}
		//区域检测类型(手绘ROI或者Blob)
		typeCount = runParams.ho_DetectROIType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("检测类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ho_DetectROIType", runParams.ho_DetectROIType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.ho_DetectROIType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.ho_DetectROIType[i];
				}
				else
				{
					typeTemp += runParams.ho_DetectROIType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ho_DetectROIType", typeTemp.S().Text());
		}

		//保存Blob参数
		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("排序方式为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", runParams.hv_SortType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_SortType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_SortType[i];
				}
				else
				{
					typeTemp += runParams.hv_SortType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("二值化类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_ThresholdType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_ThresholdType[i];
				}
				else
				{
					typeTemp += runParams.hv_ThresholdType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("低阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_LowThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_LowThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_LowThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("高阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_HighThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_HighThreshold[i];
				}
				else
				{
					typeTemp += runParams.hv_HighThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("sigma为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_Sigma[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_Sigma[i];
				}
				else
				{
					typeTemp += runParams.hv_Sigma[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核宽度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreWidth[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreWidth[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("滤波核高度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CoreHeight[i];
				}
				else
				{
					typeTemp += runParams.hv_CoreHeight[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值分割图像对比度为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdContrast[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdContrast[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdContrast[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("动态阈值极性选择为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_DynThresholdPolarity[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i];
				}
				else
				{
					typeTemp += runParams.hv_DynThresholdPolarity[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选面积Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半长Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半宽Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选行坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RowFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选列坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_CircularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", to_string(runParams.hv_CircularityFilter_Min.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CircularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CircularityFilter_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_CircularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_RectangularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", to_string(runParams.hv_RectangularityFilter_Min.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_RectangularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_RectangularityFilter_Min[i];
				}
				else
				{
					typeTemp += runParams.hv_RectangularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_WidthFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_HeightFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选面积Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半长Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选半宽Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选行坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选列坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_CircularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", to_string(runParams.hv_CircularityFilter_Max.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_CircularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_CircularityFilter_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_CircularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_RectangularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", to_string(runParams.hv_RectangularityFilter_Max.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_RectangularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_RectangularityFilter_Max[i];
				}
				else
				{
					typeTemp += runParams.hv_RectangularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_WidthFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("筛选hv_HeightFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Min[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Min[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_FillUpShape_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Max[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[i].D();
				if (i == typeCount - 1)
				{
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Max[i];
				}
				else
				{
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", runParams.hv_OperatorType.S().Text());
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperatorType[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperatorType[i];
				}
				else
				{
					typeTemp += runParams.hv_OperatorType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学宽为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", to_string(runParams.hv_OperaRec_Width.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Width[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Width[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Width[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学高为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", to_string(runParams.hv_OperaRec_Height.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaRec_Height[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaRec_Height[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaRec_Height[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("形态学半径为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", to_string(runParams.hv_OperaCir_Radius.D()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_OperaCir_Radius[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_OperaCir_Radius[i];
				}
				else
				{
					typeTemp += runParams.hv_OperaCir_Radius[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", typeTemp.S().Text());
		}

		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_SelectAreaMax为空，保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", to_string(runParams.hv_SelectAreaMax.TupleInt().I()));
		}
		else
		{
			HTuple typeTemp = runParams.hv_SelectAreaMax[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += runParams.hv_SelectAreaMax[i];
				}
				else
				{
					typeTemp += runParams.hv_SelectAreaMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", typeTemp.S().Text());
		}

		//保存MLP训练运行参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Iterations", to_string(runParams.hv_Iterations.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumHidden", to_string(runParams.hv_NumHidden.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WeightTolerance", to_string(runParams.hv_WeightTolerance.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorTolerance", to_string(runParams.hv_ErrorTolerance.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Rejection", runParams.hv_Rejection.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorData", to_string(runParams.hv_ErrorData.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsAddNG", to_string(runParams.hv_IsAddNG.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsZoomSize", to_string(runParams.hv_IsZoomSize.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Width", to_string(runParams.hv_Zoom_Width.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Height", to_string(runParams.hv_Zoom_Height.TupleInt().I()));

		//保存MLP模型
		HTuple ClassifyIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
		WriteClassMlp(runParams.hv_ClassifyHandle, ClassifyIDPath);

		//保存位置修正模板匹配参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", runParams.hv_MatchMethod_PositionCorrection.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", to_string(runParams.hv_AngleStart_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", to_string(runParams.hv_AngleExtent_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", to_string(runParams.hv_ScaleRMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", to_string(runParams.hv_ScaleRMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", to_string(runParams.hv_ScaleCMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", to_string(runParams.hv_ScaleCMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", to_string(runParams.hv_MinScore_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", to_string(runParams.hv_NumMatches_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", to_string(runParams.hv_Check_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", to_string(runParams.hv_Row_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", to_string(runParams.hv_Column_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", to_string(runParams.hv_Angle_PositionCorrection.D()));
		//保存位置修正模板匹配模型
		try
		{
			TypeParamsStruct_MachineLearning mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				WriteNccModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}
			else
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				WriteShapeModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}

		}
		catch (...)
		{
			WriteTxt("位置修正模板匹配模型ID为空");
		}
		//保存位置修正的运行区域
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//保存位置修正的训练区域
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("位置修正训练区域为空，保存失败");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，Pin针检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取MachineLearning检测参数
ErrorCode_Xml DataIO::ReadParams_MachineLearning(const string ConfigPath, const string XmlPath, RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePathArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取检测参数hv_ImgFilePathArray失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ImgFilePathArray.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ImgFilePathArray.Append(HTuple(typeArray[i].c_str()));
		}
		int typeCount = runParams.hv_ImgFilePathArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ImgFilePathArray失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNameArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读参数hv_ClassNameArray失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ClassNameArray.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ClassNameArray.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ClassNameArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ClassNameArray失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassIDArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML参数hv_ClassIDArray失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ClassIDArray.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_ClassIDArray.Append(tempIntValue);
		}
		typeCount = runParams.hv_ClassIDArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("取参数hv_ClassIDArray失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NGID", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML参数hv_NGID失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NGID = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUnionDetectROI", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML参数hv_IsUnionDetectROI失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsUnionDetectROI = tempIntValue;

		//小图检测ROI区域加载
		runParams.ho_DetectROIArray.clear();
		int countVec = runParams.hv_ClassIDArray.TupleLength().I();
		for (int i = 0; i < countVec; i++)
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(i) + "_DetectROI.hobj";
			HTuple isExist;
			FileExists(PathRunRegion, &isExist);
			if (isExist > 0)
			{
				HObject regionT;
				ReadRegion(&regionT, PathRunRegion);
				runParams.ho_DetectROIArray.push_back(regionT);
			}
			else
			{
				return nullFile_Xml;
			}
		}

		//MLP参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Iterations", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Iterations失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Iterations = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumHidden", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_NumHidden失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NumHidden = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WeightTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WeightTolerance失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_WeightTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ErrorTolerance失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ErrorTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Rejection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Rejection失败");
			return errorCode;
		}
		runParams.hv_Rejection = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorData", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ErrorData失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ErrorData = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsAddNG", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_IsAddNG失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsAddNG = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsZoomSize", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_IsZoomSize失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsZoomSize = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Zoom_Width失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Zoom_Width = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Zoom_Height失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Zoom_Height = tempIntValue;

		//读取MLP模型
		HTuple ClassifyIdPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
		ReadClassMlp(ClassifyIdPath, &runParams.hv_ClassifyHandle);

		//读取区域检测类型(手绘ROI或者Blob)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ho_DetectROIType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数ho_DetectROIType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.ho_DetectROIType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.ho_DetectROIType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.ho_DetectROIType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数ho_DetectROIType失败");
			return nullKeyName_Xml;
		}

		//读取Blob参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_SortType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SortType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_SortType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_SortType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ThresholdType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ThresholdType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_ThresholdType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ThresholdType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_LowThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_LowThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_LowThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_LowThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HighThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HighThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_HighThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HighThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Sigma失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Sigma.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Sigma.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_Sigma失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreWidth失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreWidth.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreWidth.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreWidth失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CoreHeight失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreHeight.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreHeight.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CoreHeight失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdContrast失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdContrast.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_DynThresholdContrast.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdContrast失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_DynThresholdPolarity失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdPolarity.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_DynThresholdPolarity.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_DynThresholdPolarity失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AreaFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AreaFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen1Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen1Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen2Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen2Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RowFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RowFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ColumnFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ColumnFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CircularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CircularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RectangularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RectangularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WidthFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_WidthFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HeightFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HeightFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AreaFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_AreaFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen1Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen1Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RecLen2Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RecLen2Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RowFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RowFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ColumnFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_ColumnFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CircularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_CircularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_RectangularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_RectangularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WidthFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_WidthFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HeightFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_HeightFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_FillUpShape_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_FillUpShape_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperatorType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperatorType.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			runParams.hv_OperatorType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperatorType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Width失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Width.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Width.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Width失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaRec_Height失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Height.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Height.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaRec_Height失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OperaCir_Radius失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaCir_Radius.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaCir_Radius.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_OperaCir_Radius失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_SelectAreaMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SelectAreaMax.Clear();
		for (int i = 0; i < typeArray.size(); i++)
		{
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_SelectAreaMax.Append(tempIntValue);
		}
		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("读取参数hv_SelectAreaMax失败");
			return nullKeyName_Xml;
		}

		//读取位置修正模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MatchMethod_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleStart_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleStart_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleStart_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_AngleExtent_PositionCorrection失败");
		}
		else
		{
			runParams.hv_AngleExtent_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleExtent_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleRMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleRMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMin_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_ScaleCMax_PositionCorrection失败");
		}
		else
		{
			runParams.hv_ScaleCMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MinScore_PositionCorrection失败");
		}
		else
		{
			runParams.hv_MinScore_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_MinScore_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_NumMatches_PositionCorrection失败");
		}
		else
		{
			runParams.hv_NumMatches_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_NumMatches_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Check_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Check_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_Check_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Row_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Row_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Row_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Column_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Column_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Column_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_Angle_PositionCorrection失败");
		}
		else
		{
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		//位置修正搜索区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			ReadRegion(&runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			runParams.ho_SearchRegion_PositionCorrection.Clear();
		}

		//位置修正训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			ReadRegion(&runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}
		else
		{
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			runParams.ho_TrainRegion_PositionCorrection.Clear();
		}
		//读取位置修正，模板匹配模型
		try
		{
			runParams.hv_ModelID_PositionCorrection.Clear();
			TypeParamsStruct_MachineLearning mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				HTuple ModelID;
				ReadNccModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else if (runParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				HTuple ModelID;
				ReadShapeModel(modelIDPath, &ModelID);
				runParams.hv_ModelID_PositionCorrection = ModelID;
			}
			else
			{
				//计算没有此类型，也需要添加空句柄，保证数组长度一致
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("PCB检测参数hv_ModelID_PositionCorrection读取失败!");
		}



		WriteTxt("MachineLearing检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，MachineLearing检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存字符检测参数(OK返回0，NG返回1)
int DataIO::WriteParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//形状模板参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", to_string(RunParams.hv_NumLevels));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(RunParams.hv_AngleStart));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(RunParams.hv_AngleExtent));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(RunParams.hv_ScaleRMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(RunParams.hv_ScaleRMax));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(RunParams.hv_ScaleCMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(RunParams.hv_ScaleCMax));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", RunParams.hv_Metric);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", to_string(RunParams.hv_Contrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ContrastMin", to_string(RunParams.hv_ContrastMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SizeMin", to_string(RunParams.hv_SizeMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScoreMin", to_string(RunParams.hv_ScoreMin));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(RunParams.hv_NumMatches));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(RunParams.hv_MaxOverlap));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", RunParams.hv_SubPixel);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(RunParams.hv_Greediness));
		//差异模型参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorType", RunParams.hv_ErrorType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LigntDirtContrast", to_string(RunParams.hv_LightDirtContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LigntVarRadio", to_string(RunParams.hv_LightVarRadio));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DarkDirtContrast", to_string(RunParams.hv_DarkDirtContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DarkVarRadio", to_string(RunParams.hv_DarkVarRadio));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea", to_string(RunParams.hv_MinArea));
		//保留11位有效位数(不是小数位)
		std::stringstream ss;
		ss << std::setprecision(11) << RunParams.hv_MaxArea;
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea", ss.str());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_EdgeTolerance", to_string(RunParams.hv_EdgeTolerance));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "OkImgFilePath", RunParams.OkImgFilePath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelNum", to_string(RunParams.hv_ModelNum));
		//图像处理参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", RunParams.hv_FilterType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(RunParams.hv_Sigma));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralContrast", to_string(RunParams.hv_BilateralContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", to_string(RunParams.hv_MaskWidth));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "MaskHeight", to_string(RunParams.MaskHeight));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseFilter", to_string(RunParams.hv_IsUseFilter));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseNormalize", to_string(RunParams.hv_IsUseNormalize));

		//保存模板原始参数
		int typeCount = RunParams.Ori_RowRefs.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_RowRefs为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_RowRefs", to_string(HTuple(RunParams.Ori_RowRefs[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_RowRefs[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_RowRefs[i];
				}
				else
				{
					typeTemp += RunParams.Ori_RowRefs[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_RowRefs", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_ColRefs.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_ColRefs为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_ColRefs", to_string(HTuple(RunParams.Ori_ColRefs[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_ColRefs[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_ColRefs[i];
				}
				else
				{
					typeTemp += RunParams.Ori_ColRefs[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_ColRefs", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Widths_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Widths_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Widths_Rec", to_string(HTuple(RunParams.Ori_Widths_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Widths_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Widths_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Widths_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Widths_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Heights_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Heights_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Heights_Rec", to_string(HTuple(RunParams.Ori_Heights_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Heights_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Heights_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Heights_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Heights_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Row1s_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Row1s_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Row1s_Rec", to_string(HTuple(RunParams.Ori_Row1s_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Row1s_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Row1s_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Row1s_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Row1s_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Col1s_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Col1s_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Col1s_Rec", to_string(HTuple(RunParams.Ori_Col1s_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Col1s_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Col1s_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Col1s_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Col1s_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Row2s_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Row2s_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Row2s_Rec", to_string(HTuple(RunParams.Ori_Row2s_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Row2s_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Row2s_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Row2s_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Row2s_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.Ori_Col2s_Rec.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_Col2s_Rec为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Col2s_Rec", to_string(HTuple(RunParams.Ori_Col2s_Rec[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.Ori_Col2s_Rec[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.Ori_Col2s_Rec[i];
				}
				else
				{
					typeTemp += RunParams.Ori_Col2s_Rec[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "Ori_Col2s_Rec", typeTemp.S().Text());
		}

		typeCount = RunParams.hv_ForwardGrayValue.size();
		if (typeCount == 0)
		{
			WriteTxt("hv_ForwardGrayValue为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ForwardGrayValue", to_string(HTuple(RunParams.hv_ForwardGrayValue[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.hv_ForwardGrayValue[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.hv_ForwardGrayValue[i];
				}
				else
				{
					typeTemp += RunParams.hv_ForwardGrayValue[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ForwardGrayValue", typeTemp.S().Text());
		}

		typeCount = RunParams.hv_BackGroundGrayValue.size();
		if (typeCount == 0)
		{
			WriteTxt("hv_BackGroundGrayValue为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BackGroundGrayValue", to_string(HTuple(RunParams.hv_BackGroundGrayValue[0]).TupleReal().D()));
		}
		else
		{
			HTuple typeTemp = RunParams.hv_BackGroundGrayValue[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.hv_BackGroundGrayValue[i];
				}
				else
				{
					typeTemp += RunParams.hv_BackGroundGrayValue[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BackGroundGrayValue", typeTemp.S().Text());
		}


		//保存形状模板训练区域
		if (RunParams.TrainRegions.size() <= 0)
		{
			//图像为空，或者图像被clear
			WriteTxt("形状模板训练区域为空，保存失败");
			return 1;
		}
		else
		{
			for (int i = 0; i < RunParams.TrainRegions.size(); i++)
			{
				HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_TrainRegion.hobj";
				WriteRegion(RunParams.TrainRegions[i], PathTrainRegion);
			}
		}

		//保存差异模板训练区域
		if (RunParams.VariationModelROIs.size() <= 0)
		{
			//图像为空，或者图像被clear
			WriteTxt("差异模板训练区域为空，保存失败");
			return 1;
		}
		else
		{
			for (int i = 0; i < RunParams.VariationModelROIs.size(); i++)
			{
				HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_VariationModelRegion.hobj";
				WriteRegion(RunParams.VariationModelROIs[i], PathTrainRegion);
			}
		}

		//保存运行区域
		if (RunParams.ho_SearchRoi.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("模板搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRoi, PathRunRegion);
		}

		//保存形状模板模型文件
		if (RunParams.ShapeModel_ID.size() <= 0)
		{
			WriteTxt("形状模型为空，保存失败");
			return 1;
		}
		else
		{
			for (int i = 0; i < RunParams.ShapeModel_ID.size(); i++)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_ShapeModelId.shm";
				WriteShapeModel(RunParams.ShapeModel_ID[i], modelIDPath);
			}
		}
		//保存差异模板模型文件
		if (RunParams.VarModel_ID.size() <= 0)
		{
			WriteTxt("差异模型为空，保存失败");
			return 1;
		}
		else
		{
			for (int i = 0; i < RunParams.VarModel_ID.size(); i++)
			{
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_VarModelId.vam";
				WriteVariationModel(RunParams.VarModel_ID[i], modelIDPath);
			}
		}
		//保存训练的模板图，大图
		HTuple saveModelImgPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CharacterModelImg.bmp";
		WriteImage(RunParams.ho_ModelImg, "bmp", 0, saveModelImgPath);
		//保存训练小图
		RunParams.ModelCutImgPath.clear();
		for (int i = 0; i < RunParams.ModelCutImg.size(); i++)
		{
			HTuple savePath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_CharacterTrainImg.bmp";
			WriteImage(RunParams.ModelCutImg[i], "bmp", 0, savePath);
			RunParams.ModelCutImgPath.push_back(savePath.S().Text());
		}

		//保存训练小图路径
		typeCount = RunParams.ModelCutImgPath.size();
		if (typeCount == 0)
		{
			WriteTxt("ModelCutImgPath为空保存失败");
			return 1;
		}
		if (typeCount == 1)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ModelCutImgPath", RunParams.ModelCutImgPath[0]);
		}
		else
		{
			string typeTemp = RunParams.ModelCutImgPath[0] + ",";
			for (int i = 1; i < typeCount; i++)
			{
				if (i == typeCount - 1)
				{
					typeTemp += RunParams.ModelCutImgPath[i];
				}
				else
				{
					typeTemp += RunParams.ModelCutImgPath[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ModelCutImgPath", typeTemp);
		}

		WriteTxt("字符检测参数保存成功!");
		//mutex1.unlock();
		return 0;

	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，字符缺陷检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//加载字符检测参数(OK返回0，NG返回1)
ErrorCode_Xml DataIO::ReadParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//形状模板参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumLevels失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumLevels = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleStart失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_AngleExtent失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleRMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScaleCMax失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Metric失败");
			return errorCode;
		}
		RunParams.hv_Metric = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Contrast失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Contrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContrastMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ContrastMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_ContrastMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SizeMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_SizeMin失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_SizeMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScoreMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ScoreMin失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScoreMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_NumMatches失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MaxOverlap失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_SubPixel失败");
			return errorCode;
		}
		RunParams.hv_SubPixel = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Greediness失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;
		//读取差异模型参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ErrorType失败");
			return errorCode;
		}
		RunParams.hv_ErrorType = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LigntDirtContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_LigntDirtContrast失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_LightDirtContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LigntVarRadio", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_LigntVarRadio失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_LightVarRadio = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DarkDirtContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_DarkDirtContrast失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_DarkDirtContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DarkVarRadio", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_DarkVarRadio失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_DarkVarRadio = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MinArea失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinArea = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MaxArea失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxArea = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EdgeTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_EdgeTolerance失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_EdgeTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "OkImgFilePath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数OkImgFilePath失败");
			return errorCode;
		}
		RunParams.OkImgFilePath = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_ModelNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_ModelNum = tempIntValue;

		//图像处理参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_FilterType失败");
			return errorCode;
		}
		RunParams.hv_FilterType = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_Sigma失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_BilateralContrast失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_BilateralContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_MaskWidth失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_MaskWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "MaskHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数MaskHeight失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.MaskHeight = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseFilter", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_IsUseFilter失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseFilter = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseNormalize", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取模板检测参数hv_IsUseNormalize失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseNormalize = tempIntValue;

		//读取模板原始参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_RowRefs", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_RowRefs失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_RowRefs.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_RowRefs.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_ColRefs", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_ColRefs失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_ColRefs.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_ColRefs.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Widths_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Widths_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Widths_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Widths_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Heights_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Heights_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Heights_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Heights_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Row1s_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Row1s_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Row1s_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Row1s_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Col1s_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Col1s_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Col1s_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Col1s_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Row2s_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Row2s_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Row2s_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Row2s_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_Col2s_Rec", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数Ori_Col2s_Rec失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.Ori_Col2s_Rec.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.Ori_Col2s_Rec.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ForwardGrayValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数hv_ForwardGrayValue失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.hv_ForwardGrayValue.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.hv_ForwardGrayValue.push_back(tempDoubleValue);
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BackGroundGrayValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数hv_BackGroundGrayValue失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.hv_BackGroundGrayValue.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				RunParams.hv_BackGroundGrayValue.push_back(tempDoubleValue);
			}
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_SearchRoi);
			ReadRegion(&RunParams.ho_SearchRoi, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_SearchRoi);
			RunParams.ho_SearchRoi.Clear();
		}
		//形状模板训练区域加载
		RunParams.TrainRegions.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_TrainRegion.hobj";
			FileExists(PathTrainRegion, &isExist);
			if (isExist > 0)
			{
				HObject roi;
				ReadRegion(&roi, PathTrainRegion);
				RunParams.TrainRegions.push_back(roi);
			}
			else
			{
				HObject roi;
				GenEmptyObj(&roi);
				RunParams.TrainRegions.push_back(roi);
			}
		}
		//差异模板训练区域加载
		RunParams.VariationModelROIs.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_VariationModelRegion.hobj";
			FileExists(PathTrainRegion, &isExist);
			if (isExist > 0)
			{
				HObject roi;
				ReadRegion(&roi, PathTrainRegion);
				RunParams.VariationModelROIs.push_back(roi);
			}
			else
			{
				HObject roi;
				GenEmptyObj(&roi);
				RunParams.VariationModelROIs.push_back(roi);
			}
		}

		//读取形状模板模型文件
		RunParams.ShapeModel_ID.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_ShapeModelId.shm";
			HTuple handleID;
			ReadShapeModel(modelIDPath, &handleID);
			RunParams.ShapeModel_ID.push_back(handleID);
		}

		//读取差异模板模型文件
		RunParams.VarModel_ID.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_VarModelId.vam";
			HTuple handleID;
			ReadVariationModel(modelIDPath, &handleID);
			RunParams.VarModel_ID.push_back(handleID);
		}
		//读取训练的模板图，大图
		HTuple saveModelImgPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CharacterModelImg.bmp";
		ReadImage(&RunParams.ho_ModelImg, saveModelImgPath);
		//读取训练小图路径
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ModelCutImgPath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取字符检测参数ModelCutImgPath失败");
			return errorCode;
		}
		if (valueXml != "null")
		{
			vector<string> typeArray;
			stringToken(valueXml, ",", typeArray);
			RunParams.ModelCutImgPath.clear();
			for (int i = 0; i < typeArray.size(); i++)
			{
				RunParams.ModelCutImgPath.push_back(typeArray[i]);
			}
		}

		//读取训练小图
		RunParams.ModelCutImg.clear();
		for (int i = 0; i < RunParams.ModelCutImgPath.size(); i++)
		{
			HTuple savePath = HTuple(RunParams.ModelCutImgPath[i].c_str());
			HObject img;
			ReadImage(&img, savePath);
			RunParams.ModelCutImg.push_back(img);
		}

		WriteTxt("字符缺陷检测参数加载成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，字符缺陷检测参数加载失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存图像拆分参数(OK返回0，NG返回1)
int DataIO::WriteParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Ori", to_string(RunParams.hv_CutRows_Ori.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Ori", to_string(RunParams.hv_CutCols_Ori.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OverlapRate", to_string(RunParams.hv_OverlapRate.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Dest", to_string(RunParams.hv_CutRows_Dest.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Dest", to_string(RunParams.hv_CutCols_Dest.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthImg", to_string(RunParams.hv_WidthImg.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightImg", to_string(RunParams.hv_HeightImg.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutWidth", to_string(RunParams.hv_CutWidth.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutHeight", to_string(RunParams.hv_CutHeight.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsSplit", to_string(RunParams.hv_IsSplit.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutNum", to_string(RunParams.hv_CutNum.TupleInt().I()));

		WriteTxt("图像拆分参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，图像拆分参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//加载图像拆分参数(OK返回0，NG返回1)
ErrorCode_Xml DataIO::ReadParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//读取参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Ori", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutRows_Ori失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutRows_Ori = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Ori", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutCols_Ori失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutCols_Ori = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OverlapRate", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_OverlapRate失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_OverlapRate = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Dest", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutRows_Dest失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutRows_Dest = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Dest", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutCols_Dest失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutCols_Dest = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthImg", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_WidthImg失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_WidthImg = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightImg", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_HeightImg失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_HeightImg = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutWidth失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutHeight失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutHeight = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsSplit", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_IsSplit失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsSplit = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_CutNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutNum = tempIntValue;


		WriteTxt("图像拆分参数加载成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图像拆分参数加载失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//保存灰度计算参数(OK返回0，NG返回1)
int DataIO::WriteParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanGrayValue_Standard", to_string(RunParams.hv_MeanGrayValue_Standard.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Min", to_string(RunParams.hv_GrayValueOffset_Min.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Max", to_string(RunParams.hv_GrayValueOffset_Max.TupleInt().I()));
		if (RunParams.isFollow)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "true");
		}
		else
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "isFollow", "false");
		}
		//保存运行区域
		if (RunParams.ho_SearchRoi.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			WriteTxt("条码搜索区域为空，保存失败");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRoi, PathRunRegion);
		}
		WriteTxt("图像灰度计算参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，图像灰度计算参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//加载灰度计算参数(OK返回0，NG返回1)
ErrorCode_Xml DataIO::ReadParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//读取参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanGrayValue_Standard", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_MeanGrayValue_Standard失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_MeanGrayValue_Standard = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_GrayValueOffset_Min失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_GrayValueOffset_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取参数hv_GrayValueOffset_Max失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_GrayValueOffset_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取直线检测参数isFollow失败");
			return errorCode;
		}
		if (valueXml == "true")
		{
			RunParams.isFollow = true;
		}
		else
		{
			RunParams.isFollow = false;
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0)
		{
			GenEmptyObj(&RunParams.ho_SearchRoi);
			ReadRegion(&RunParams.ho_SearchRoi, PathRunRegion);
		}
		else
		{
			GenEmptyObj(&RunParams.ho_SearchRoi);
			RunParams.ho_SearchRoi.Clear();
		}

		WriteTxt("图像灰度计算参数加载成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，图像灰度计算参数加载失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//读取流程使用的模块字段
ErrorCode_Xml DataIO::ReadXML_Process_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_Process(XmlPath, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取流程参数失败");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，流程参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//保存全局变量的字段值和全局变量名字(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
int DataIO::WriteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXML_FieldValueAndGlobalName(XmlPath, processId, nodeName, processModbuleID, keyName);

		WriteTxt("全局参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，全局参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//插入每个流程，每个模块需要拿到全局变量的字段名称(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
int DataIO::WriteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数保存
		xmlC.InsertXML_FieldValue(XmlPath, processId, nodeName, processModbuleID, keyName);

		WriteTxt("全局参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML保存代码崩溃，全局参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}
//读取全局变量的字段名称
ErrorCode_Xml DataIO::ReadXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_FieldValue(XmlPath, processId, nodeName, processModbuleID, fieldValue);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取全局变量参数失败");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，全局变量参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//读取全局变量的字段值和全局变量名字
ErrorCode_Xml DataIO::ReadXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_FieldValueAndGlobalName(XmlPath, processId, nodeName, processModbuleID, fieldValue);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML读取全局变量参数失败");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML读取代码崩溃，全局变量参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//删除全局变量的字段名称
int DataIO::DeleteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数删除
		xmlC.DeleteXML_FieldValue(XmlPath);

		WriteTxt("全局参数删除成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML删除代码崩溃，全局参数删除失败!");
		//mutex1.unlock();
		return -1;
	}
}
//删除全局变量的字段值和全局变量名称
int DataIO::DeleteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//参数删除
		xmlC.DeleteXML_FieldValueAndGlobalName(XmlPath);

		WriteTxt("全局参数删除成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML删除代码崩溃，全局参数删除失败!");
		//mutex1.unlock();
		return -1;
	}
}


//写日志函数
int DataIO::WriteTxt(string content)
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
string DataIO::FolderPath()
{
	char strBuf[256] = { 0 };
	GetModuleFileNameA(NULL, strBuf, sizeof(strBuf));
	string strTemp = strBuf;
	if (strTemp.empty())
		return strTemp;

	strTemp = strReplaceAll(strTemp, "\\", "/");
	string::size_type pos = strTemp.rfind("/");
	if (string::npos != pos)
		strTemp = strTemp.substr(0, pos);
	else
		strTemp = "";

	return strTemp;
}

//返回一个不含strOld
string DataIO::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
{
	string::size_type pos = 0;
	string strTemp = strResource;
	do
	{
		if ((pos = strTemp.find(strOld)) != string::npos)
		{
			strTemp.replace(pos, strOld.length(), strNew);
		}
	} while (pos != string::npos);
	return strTemp;
}
//按照指定字符分割字符串到vector
void DataIO::stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken)
{
	string sCopy = sToBeToken;
	int iPosEnd = 0;
	while (true)
	{
		iPosEnd = sCopy.find(sSeperator);
		if (iPosEnd == -1)
		{
			vToken.push_back(sCopy);
			break;
		}
		vToken.push_back(sCopy.substr(0, iPosEnd));
		sCopy = sCopy.substr(iPosEnd + 1);
	}
}
