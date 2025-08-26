#include "DataIO.h"
DataIO::DataIO()
{
}
DataIO::~DataIO()
{
}
//·������
void DataIO::PathSet(const string ConfigPath, const string XmlPath)
{
	XmlClass2 xmlC;//xml��ȡ����

	// �ж�Ŀ¼���ļ��Ƿ���ڵı�ʶ��
	int mode = 0;
	if (_access(ConfigPath.c_str(), mode))
	{
		//system("mkdir head");
		_mkdir(ConfigPath.c_str());
	}

	//����XML(���ھͲ��ᴴ��)
	int Ret = xmlC.CreateXML(XmlPath);
	if (Ret != 0)
	{
		WriteTxt("�򿪲�������xml�ļ�ʧ��");
		//mutex1.unlock();
		return;
	}
}
//����һά�������
int DataIO::WriteParams_BarCode(const string ConfigPath, const string XmlPath, const RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//һά���������
		//д������(���鰴�ն��ŷָ�洢)
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", "auto");
			WriteTxt("һά������Ϊ�գ�Ĭ�ϱ���Ϊauto");
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
		//�����������
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

		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("������������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("һά�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������һά�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡһά�������
ErrorCode_Xml DataIO::ReadParams_BarCode(const string ConfigPath, const string XmlPath, RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//һά���������ȡ
		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_CodeType_Runʧ��");
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
			WriteTxt("һά������Ϊ�գ���ȡһά�������hv_CodeType_Runʧ��");
			return nullKeyName_Xml;
		}


		//��ȡ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_CodeNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_Toleranceʧ��");
			return errorCode;
		}
		runParams.hv_Tolerance = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_HeadCharʧ��");
			return errorCode;
		}
		runParams.hv_HeadChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_EndCharʧ��");
			return errorCode;
		}
		runParams.hv_EndChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_ContainCharʧ��");
			return errorCode;
		}
		runParams.hv_ContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_NotContainCharʧ��");
			return errorCode;
		}
		runParams.hv_NotContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_CodeLength_Runʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeLength_Run = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡһά�������hv_TimeOutʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����isFollowʧ��");
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
		//�����������
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


		WriteTxt("һά���������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������һά���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//�����ά�������
int DataIO::WriteParams_Code2d(const string ConfigPath, const string XmlPath, const RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��ά���������
		//д������(���鰴�ն��ŷָ�洢)
		int typeCount = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", "QR Code");
			WriteTxt("��ά������Ϊ�գ�Ĭ�ϱ���QR Code");
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
		//�����������
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
		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("��ά����������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("��ά�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ά�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ά�������
ErrorCode_Xml DataIO::ReadParams_Code2d(const string ConfigPath, const string XmlPath, RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ά���������ȡ
		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeType_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_CodeType_Runʧ��");
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
			WriteTxt("��ά������Ϊ�գ���ȡ��ά�������hv_CodeType_Runʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_CodeNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_Toleranceʧ��");
			return errorCode;
		}
		runParams.hv_Tolerance = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeadChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_HeadCharʧ��");
			return errorCode;
		}
		runParams.hv_HeadChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EndChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_EndCharʧ��");
			return errorCode;
		}
		runParams.hv_EndChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_ContainCharʧ��");
			return errorCode;
		}
		runParams.hv_ContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NotContainChar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_NotContainCharʧ��");
			return errorCode;
		}
		runParams.hv_NotContainChar = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CodeLength_Run", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_CodeLength_Runʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_CodeLength_Run = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ά�������hv_TimeOutʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����isFollowʧ��");
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
		//�����������
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

		WriteTxt("��ά���������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������,��ά���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����ģ�������(OK����0��NG����1)
int DataIO::WriteParams_Template(const string ConfigPath, const string XmlPath, const RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//ѵ����������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", RunParams.hv_MatchMethod.S().Text());
		//�ж�tuple���������ǲ�������
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

		//����ģ��ԭʼ����ͽǶ�
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", to_string(RunParams.hv_Angle_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", to_string(RunParams.hv_CenterRow_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", to_string(RunParams.hv_CenterCol_Original.TupleReal().D()));
		//�������в���
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(RunParams.hv_MinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(RunParams.hv_NumMatches.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(RunParams.hv_MaxOverlap.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", RunParams.hv_SubPixel.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(RunParams.hv_Greediness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(RunParams.hv_TimeOut.TupleInt().I()));


		//����ѵ������
		if (RunParams.ho_TrainRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ѵ������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(RunParams.ho_TrainRegion, PathTrainRegion);
		}
		//����ѵ��Сͼ
		if (RunParams.ho_CropModelImg.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ѵ��ͼƬΪ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CropModelImg.bmp";
			WriteImage(RunParams.ho_CropModelImg, "bmp", 0, PathTrainImg);
		}
		//������������
		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ģ����������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRegion, PathRunRegion);
		}

		//����ѵ���õ�ģ��IDģ��
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
		WriteTxt("ģ�����������ɹ�!");
		//mutex1.unlock();
		return 0;

	}
	catch (...)
	{
		WriteTxt("XML������������ģ�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//����ģ�������(OK����0��NG����1)
ErrorCode_Xml DataIO::ReadParams_Template(const string ConfigPath, const string XmlPath, RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��ȡѵ������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MatchMethodʧ��");
			return errorCode;
		}
		RunParams.hv_MatchMethod = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumLevelsʧ��");
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
			WriteTxt("XML��ȡģ�������hv_AngleStartʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleExtentʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_ScaleRMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_ScaleCMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_Optimizationʧ��");
			return errorCode;
		}
		RunParams.hv_Optimization = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Metricʧ��");
			return errorCode;
		}
		RunParams.hv_Metric = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Contrastʧ��");
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
			WriteTxt("XML��ȡģ�������hv_MinContrastʧ��");
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

		//��ȡģ��ԭʼ����ͽǶ�
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Angle_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Angle_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_CenterRow_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterRow_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_CenterCol_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterCol_Original = tempDoubleValue;

		//��ȡ���в���
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumMatchesʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MaxOverlapʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_SubPixelʧ��");
			return errorCode;
		}
		RunParams.hv_SubPixel = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Greedinessʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_TimeOutʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_TimeOut = tempIntValue;

		//�����������
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

		//ѵ���������
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
		//��ȡѵ��Сͼ
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

		//��ȡ����ģ��ģ��
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

		WriteTxt("ģ����������سɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ģ�����������ʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�������㷨����(OK����0��NG����1)
int DataIO::WriteParams_ImgDifference(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//ѵ����������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", RunParams.hv_MatchMethod.S().Text());
		//�ж�tuple���������ǲ�������
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

		//����ģ��ԭʼ����ͽǶ�
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", to_string(RunParams.hv_Angle_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", to_string(RunParams.hv_CenterRow_Original.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", to_string(RunParams.hv_CenterCol_Original.TupleReal().D()));
		//�������в���
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(RunParams.hv_MinScore.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(RunParams.hv_NumMatches.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", to_string(RunParams.hv_MaxOverlap.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", RunParams.hv_SubPixel.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", to_string(RunParams.hv_Greediness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", to_string(RunParams.hv_TimeOut.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", to_string(RunParams.hv_IsUsSearchRoi.TupleInt().I()));
		//����ѵ��ԭͼ
		if (RunParams.ho_ModelImg.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ѵ��ͼƬΪ�գ�����ʧ��");
			return 1;
		}
		else
		{
			HTuple PathTrainImg = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ModelImg.bmp";
			WriteImage(RunParams.ho_ModelImg, "bmp", 0, PathTrainImg);
		}
		//����ѵ������
		if (RunParams.ho_TrainRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ѵ������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(RunParams.ho_TrainRegion, PathTrainRegion);
		}
		//������������
		if (RunParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ģ����������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRegion, PathRunRegion);
		}

		//����ѵ���õ�ģ��IDģ��
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
		
		//�����ֲ���
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
		WriteTxt("XML������������ģ�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//���ز���㷨����(OK����0��NG����1)
ErrorCode_Xml DataIO::ReadParams_ImgDifference(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��ȡѵ������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MatchMethodʧ��");
			return errorCode;
		}
		RunParams.hv_MatchMethod = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumLevelsʧ��");
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
			WriteTxt("XML��ȡģ�������hv_AngleStartʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleExtentʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_ScaleRMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_ScaleCMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCStep", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCStepʧ��");
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
			WriteTxt("XML��ȡģ�������hv_Optimizationʧ��");
			return errorCode;
		}
		RunParams.hv_Optimization = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Metricʧ��");
			return errorCode;
		}
		RunParams.hv_Metric = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Contrastʧ��");
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
			WriteTxt("XML��ȡģ�������hv_MinContrastʧ��");
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

		//��ȡģ��ԭʼ����ͽǶ�
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Angle_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Angle_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRow_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_CenterRow_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterRow_Original = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCol_Original", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_CenterCol_Originalʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_CenterCol_Original = tempDoubleValue;

		//��ȡ���в���
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumMatchesʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MaxOverlapʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_SubPixelʧ��");
			return errorCode;
		}
		RunParams.hv_SubPixel = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Greedinessʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TimeOut", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_TimeOutʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_TimeOut = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUsSearchRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_IsUsSearchRoiʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUsSearchRoi = tempIntValue;

		//��ȡѵ��ԭͼ
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

		//�����������
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

		//ѵ���������
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

		//��ȡ����ģ��ģ��
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

		//��ȡ��ֲ���
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseGauss", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_IsUseGaussʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseGauss = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Sigmaʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Sigma = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Mult", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Multʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Mult = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Add", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Addʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Add = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterGrayMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterGrayMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterGrayMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterGrayMaxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterGrayMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterAreaMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterAreaMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterAreaMaxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterAreaMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterWidthMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterWidthMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterWidthMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterWidthMaxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterWidthMax = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterHeightMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterHeightMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterHeightMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterHeightMaxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_FilterHeightMax = tempIntValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ģ�����������ʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����ֱ�߼�����
int DataIO::WriteParams_Line(const string ConfigPath, const string XmlPath, const RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//�ұ߲�������
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

		WriteTxt("ֱ�߼���������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ֱ�߼���������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡֱ�߼�����
ErrorCode_Xml DataIO::ReadParams_Line(const string ConfigPath, const string XmlPath, RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//�ұ߲�����ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_Row1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_Column1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_Row2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_Column2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureLength1ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureLength2ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureTransitionʧ��");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureSelectʧ��");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_InstancesNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureMinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_DistanceThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MeasureInterpolationʧ��");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����hv_MmPixelʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����isFollowʧ��");
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

		WriteTxt("ֱ�߼�������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ֱ�߼�������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����Բ������
int DataIO::WriteParams_Circle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��Բ��������
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

		WriteTxt("Բ����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������Բ����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡԲ������
ErrorCode_Xml DataIO::ReadParams_Circle(const string ConfigPath, const string XmlPath, RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��Բ������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_Rowʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_Columnʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_Radiusʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureLength1ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureLength2ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureTransitionʧ��");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureSelectʧ��");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_InstancesNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureMinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_DistanceThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MeasureInterpolationʧ��");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_MmPixelʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������hv_DectTypeʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_DectType = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡԲ������isFollowʧ��");
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

		WriteTxt("Բ��������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������Բ��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//������μ�����
int DataIO::WriteParams_Rectangle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//�Ҿ��β�������
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

		WriteTxt("���μ���������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML���������������μ���������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ���μ�����
ErrorCode_Xml DataIO::ReadParams_Rectangle(const string ConfigPath, const string XmlPath, RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//�Ҿ��β�����ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_Rowʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_Columnʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_Angleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_Length1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_Length2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureLength1ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureLength2ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureTransitionʧ��");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureSelectʧ��");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_InstancesNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureMinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_DistanceThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����hv_MeasureInterpolationʧ��");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���μ�����isFollowʧ��");
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


		WriteTxt("���μ�������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ������������μ�������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//������Բ������
int DataIO::WriteParams_Ellipse(const string ConfigPath, const string XmlPath, const RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����Բ��������
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

		WriteTxt("��Բ����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������Բ����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��Բ������
ErrorCode_Xml DataIO::ReadParams_Ellipse(const string ConfigPath, const string XmlPath, RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����Բ������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_Rowʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_Columnʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_Angleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_Radius1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_Radius2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureLength1ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength1 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureLength2ʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureLength2 = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureTransitionʧ��");
			return errorCode;
		}
		runParams.hv_MeasureTransition = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureSelectʧ��");
			return errorCode;
		}
		runParams.hv_MeasureSelect = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_MeasureNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InstancesNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_InstancesNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_InstancesNum = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureMinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeasureMinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_DistanceThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_DistanceThreshold = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureInterpolation", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������hv_MeasureInterpolationʧ��");
			return errorCode;
		}
		runParams.hv_MeasureInterpolation = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��Բ������isFollowʧ��");
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
		WriteTxt("��Բ��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������Բ��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�����ֵ������
int DataIO::WriteParams_Binarization(const string ConfigPath, const string XmlPath, const RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
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
		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("��ֵ����������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}
		WriteTxt("��ֵ����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ֵ����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ֵ������
ErrorCode_Xml DataIO::ReadParams_Binarization(const string ConfigPath, const string XmlPath, RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_ThresholdTypeʧ��");
			return errorCode;
		}
		runParams.hv_ThresholdType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_LowThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_LowThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_HighThresholdʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_HighThreshold = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_Sigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_CoreWidthʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreWidth = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_CoreHeightʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CoreHeight = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_DynThresholdContrastʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_DynThresholdContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡBlob��ֵ��������hv_DynThresholdPolarityʧ��");
			return errorCode;
		}
		runParams.hv_DynThresholdPolarity = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����isFollowʧ��");
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
		//�����������
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

		WriteTxt("��ֵ��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������ֵ��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//������̬ѧ����
int DataIO::WriteParams_Morphology(const string ConfigPath, const string XmlPath, const RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//д������(���鰴�ն��ŷָ�洢)
		//*******************************�������****************************************************************
		//д�������������(���ͣ���ʴ�ȵ�)
		int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_RegionOperator_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", "null");
			WriteTxt("Blob�����������Ϊ��,�����ַ���null");
		}
		else
		{
			//д�������������(���ͣ���ʴ�ȵ�)
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

			//д���˲��˵Ŀ�
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


			//д���˲��˵ĸ�
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


			//д��Բ�νṹԪ�صİ뾶
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



			//д��׶������С���
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




			//д��׶����������
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
				WriteTxt("Blob���������������ɹ�");
			}
		}

		//****************************����ɸѡ****************************************************************
		typeNum = 0;
		//д������ɸѡ����
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_FeaturesFilter_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
			WriteTxt("Blob����ɸѡ����Ϊ��,�����ַ���null");
		}
		else
		{
			//д������ɸѡ����(�����Բ�ȵȵ�)
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

			//д��ɸѡ������Сֵ
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


			//д��ɸѡ�������ֵ
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


			//д�����
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", runParams.hv_And_Or_Filter.S().Text());

			if (typeNum > 0)
			{
				WriteTxt("Blob��̬ѧ����ɸѡ��������ɹ�!");
			}
		}

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������Blob��̬ѧ��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��̬ѧ����
ErrorCode_Xml DataIO::ReadParams_Morphology(const string ConfigPath, const string XmlPath, RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��̬ѧ������ȡ
		//������Ҫ���
		runParams.hv_RegionOperator_Type.Clear();
		runParams.hv_Height_Operator.Clear();
		runParams.hv_Width_Operator.Clear();
		runParams.hv_CircleRadius_Operator.Clear();
		runParams.hv_MinArea_FillUpShape_Operator.Clear();
		runParams.hv_MaxArea_FillUpShape_Operator.Clear();
		runParams.hv_FeaturesFilter_Type.Clear();
		runParams.hv_MinValue_Filter.Clear();
		runParams.hv_MaxValue_Filter.Clear();

		//��ȡ�����������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			WriteTxt("XML��ȡ��̬ѧ������hv_RegionOperator_Typeʧ��");
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
				if (typeArray[i] == "��������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Rectangle);
				}
				else if (typeArray[i] == "Բ������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Circle);
				}
				else if (typeArray[i] == "���θ�ʴ")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Rectangle);
				}
				else if (typeArray[i] == "Բ�θ�ʴ")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Circle);
				}
				else if (typeArray[i] == "���ο�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Rectangle);
				}
				else if (typeArray[i] == "Բ�ο�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Circle);
				}
				else if (typeArray[i] == "���α�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Rectangle);
				}
				else if (typeArray[i] == "Բ�α�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Circle);
				}
				else if (typeArray[i] == "��ñ������νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Rectangle);
				}
				else if (typeArray[i] == "��ñ����Բ�νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Circle);
				}
				else if (typeArray[i] == "��ñ������νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Rectangle);
				}
				else if (typeArray[i] == "��ñ����Բ�νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Circle);
				}
				else if (typeArray[i] == "�׶�ȫ�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Fillup);
				}
				else if (typeArray[i] == "�׶�������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_FillUp_Shape);
				}
				else if (typeArray[i] == "������ͨ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Connection);
				}
				else if (typeArray[i] == "��������ϲ�")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union1);
				}
				else if (typeArray[i] == "��������ϲ�")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union2);
				}
				else if (typeArray[i] == "����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Complement);
				}
				else if (typeArray[i] == "�")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Difference);
				}
				else if (typeArray[i] == "����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Intersection);
				}
				else if (typeArray[i] == "�ͼ�")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_ConcatObj);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("Blob�����������Ϊ�գ�������ȡʧ��");
			}
			else
			{
				int isContainRectangle = 0;
				int isContainCircle = 0;
				int isContainFillUpShape = 0;
				for (int i = 0; i < typeCount; i++)
				{
					if (runParams.hv_RegionOperator_Type[i] == "��������" || runParams.hv_RegionOperator_Type[i] == "���θ�ʴ" ||
						runParams.hv_RegionOperator_Type[i] == "���ο�����" || runParams.hv_RegionOperator_Type[i] == "���α�����" ||
						runParams.hv_RegionOperator_Type[i] == "��ñ������νṹ��" || runParams.hv_RegionOperator_Type[i] == "��ñ������νṹ��")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "Բ������" || runParams.hv_RegionOperator_Type[i] == "Բ�θ�ʴ" ||
						runParams.hv_RegionOperator_Type[i] == "Բ�ο�����" || runParams.hv_RegionOperator_Type[i] == "Բ�α�����" ||
						runParams.hv_RegionOperator_Type[i] == "��ñ����Բ�νṹ��" || runParams.hv_RegionOperator_Type[i] == "��ñ����Բ�νṹ��")
					{
						isContainCircle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "�׶�������")
					{
						isContainFillUpShape += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//��ȡ�˲��˿��
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Width_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_Width_Operatorʧ��");
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

					//��ȡ�˲��˸߶�
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Height_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_Height_Operatorʧ��");
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
					//��ȡԲ�νṹԪ�صİ뾶
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircleRadius_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_CircleRadius_Operatorʧ��");
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
					//��ȡ�����С���
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_MinArea_FillUpShape_Operatorʧ��");
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

					//��ȡ���������
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea_FillUpShape_Operator", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_MaxArea_FillUpShape_Operatorʧ��");
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
						WriteTxt("Blob����������Ͳ�����ȡ�ɹ�");
					}
				}
			}
		}
		else
		{
			runParams.hv_RegionOperator_Type.Clear();
		}

		//��ȡ����ɸѡ����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", valueXml);
		if ((errorCode != Ok_Xml) && (errorCode != nullKeyName_Xml))
		{
			WriteTxt("XML��ȡ��̬ѧ������hv_FeaturesFilter_Typeʧ��");
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
				if (typeArray[i] == "���")
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
				if (typeArray[i] == "�Ƕ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Angle_Filter);
				}
				if (typeArray[i] == "Բ��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Circularity_Filter);
				}
				if (typeArray[i] == "���ܶ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Compactness_Filter);
				}
				if (typeArray[i] == "͹��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Convexity_Filter);
				}
				if (typeArray[i] == "���ζ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rectangularity_Filter);
				}
				if (typeArray[i] == "�߶�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Height_Filter);
				}
				if (typeArray[i] == "���")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Width_Filter);
				}
				if (typeArray[i] == "��С���Բ�뾶")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_OuterRadius_Filter);
				}
				if (typeArray[i] == "����ڽ�Բ�뾶")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_InnerRadius_Filter);
				}
				if (typeArray[i] == "��С��Ӿذ볤")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len1_Filter);
				}
				if (typeArray[i] == "��С��Ӿذ��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len2_Filter);
				}
				if (typeArray[i] == "��С��ӾؽǶ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Angle_Filter);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("Blob����ɸѡ����Ϊ�գ�������ȡʧ��");
			}
			else
			{
				//��ȡɸѡ������Сֵ
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML��ȡ��̬ѧ������hv_MinValue_Filterʧ��");
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

				//��ȡɸѡ�������ֵ
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML��ȡ��̬ѧ������hv_MaxValue_Filterʧ��");
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

				//��ȡ���
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", valueXml);
				if (errorCode != Ok_Xml)
				{
					WriteTxt("XML��ȡ��̬ѧ������hv_And_Or_Filterʧ��");
					return errorCode;
				}
				runParams.hv_And_Or_Filter = HTuple(valueXml.c_str());

				if (typeNum > 0)
				{
					WriteTxt("Blob����ɸѡ���Ͳ�����ȡ�ɹ�");
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
		WriteTxt("XML��ȡ���������Blob��̬ѧ������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����Blob����
int DataIO::WriteParams_Blob(const string ConfigPath, const string XmlPath, const RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SetBlobCount", to_string(runParams.hv_SetBlobCount.TupleInt().I()));
		//��ֵ����������
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
		//д������(���鰴�ն��ŷָ�洢)
		//*******************************�������****************************************************************
		//д�������������(���ͣ���ʴ�ȵ�)
		int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_RegionOperator_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RegionOperator_Type", "null");
		}
		else
		{
			//д�������������(���ͣ���ʴ�ȵ�)
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

			//д���˲��˵Ŀ�
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


			//д���˲��˵ĸ�
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


			//д��Բ�νṹԪ�صİ뾶
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



			//д��׶������С���
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




			//д��׶����������
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

		//****************************����ɸѡ****************************************************************
		typeNum = 0;
		//д������ɸѡ����
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_FeaturesFilter_Type == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
		}
		else
		{
			//д������ɸѡ����(�����Բ�ȵȵ�)
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

			//д��ɸѡ������Сֵ
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


			//д��ɸѡ�������ֵ
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


			//д�����
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
		//������������
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
//��ȡBlob����
ErrorCode_Xml DataIO::ReadParams_Blob(const string ConfigPath, const string XmlPath, RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
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

		//��ֵ��������ȡ
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

		//��ȡ��������
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

		//��̬ѧ������ȡ
		runParams.hv_RegionOperator_Type.Clear();
		runParams.hv_Height_Operator.Clear();
		runParams.hv_Width_Operator.Clear();
		runParams.hv_CircleRadius_Operator.Clear();
		runParams.hv_MinArea_FillUpShape_Operator.Clear();
		runParams.hv_MaxArea_FillUpShape_Operator.Clear();
		runParams.hv_FeaturesFilter_Type.Clear();
		runParams.hv_MinValue_Filter.Clear();
		runParams.hv_MaxValue_Filter.Clear();

		//��ȡ�����������
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
				if (typeArray[i] == "��������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Rectangle);
				}
				else if (typeArray[i] == "Բ������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Dilation_Circle);
				}
				else if (typeArray[i] == "���θ�ʴ")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Rectangle);
				}
				else if (typeArray[i] == "Բ�θ�ʴ")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Erosion_Circle);
				}
				else if (typeArray[i] == "���ο�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Rectangle);
				}
				else if (typeArray[i] == "Բ�ο�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Opening_Circle);
				}
				else if (typeArray[i] == "���α�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Rectangle);
				}
				else if (typeArray[i] == "Բ�α�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Closing_Circle);
				}
				else if (typeArray[i] == "��ñ������νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Rectangle);
				}
				else if (typeArray[i] == "��ñ����Բ�νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_BottomHat_Circle);
				}
				else if (typeArray[i] == "��ñ������νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Rectangle);
				}
				else if (typeArray[i] == "��ñ����Բ�νṹ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_TopHat_Circle);
				}
				else if (typeArray[i] == "�׶�ȫ�����")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Fillup);
				}
				else if (typeArray[i] == "�׶�������")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_FillUp_Shape);
				}
				else if (typeArray[i] == "������ͨ��")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Connection);
				}
				else if (typeArray[i] == "��������ϲ�")
				{
					runParams.hv_RegionOperator_Type.Append(codeTypeStruct.hv_Union1);
				}
				else if (typeArray[i] == "����")
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
					if (runParams.hv_RegionOperator_Type[i] == "��������" || runParams.hv_RegionOperator_Type[i] == "���θ�ʴ" ||
						runParams.hv_RegionOperator_Type[i] == "���ο�����" || runParams.hv_RegionOperator_Type[i] == "���α�����" ||
						runParams.hv_RegionOperator_Type[i] == "��ñ������νṹ��" || runParams.hv_RegionOperator_Type[i] == "��ñ������νṹ��")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "Բ������" || runParams.hv_RegionOperator_Type[i] == "Բ�θ�ʴ" ||
						runParams.hv_RegionOperator_Type[i] == "Բ�ο�����" || runParams.hv_RegionOperator_Type[i] == "Բ�α�����" ||
						runParams.hv_RegionOperator_Type[i] == "��ñ����Բ�νṹ��" || runParams.hv_RegionOperator_Type[i] == "��ñ����Բ�νṹ��")
					{
						isContainCircle += 1;
					}
					else if (runParams.hv_RegionOperator_Type[i] == "�׶�������")
					{
						isContainFillUpShape += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//��ȡ�˲��˿��
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

					//��ȡ�˲��˸߶�
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
					//��ȡԲ�νṹԪ�صİ뾶
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
					//��ȡ�����С���
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

					//��ȡ���������
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

		//��ȡ����ɸѡ����
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
				if (typeArray[i] == "���")
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
				if (typeArray[i] == "�Ƕ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Angle_Filter);
				}
				if (typeArray[i] == "Բ��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Circularity_Filter);
				}
				if (typeArray[i] == "���ܶ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Compactness_Filter);
				}
				if (typeArray[i] == "͹��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Convexity_Filter);
				}
				if (typeArray[i] == "���ζ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rectangularity_Filter);
				}
				if (typeArray[i] == "�߶�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Height_Filter);
				}
				if (typeArray[i] == "���")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Width_Filter);
				}
				if (typeArray[i] == "��С���Բ�뾶")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_OuterRadius_Filter);
				}
				if (typeArray[i] == "��С��Ӿذ볤")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len1_Filter);
				}
				if (typeArray[i] == "��С��Ӿذ��")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Len2_Filter);
				}
				if (typeArray[i] == "��С��ӾؽǶ�")
				{
					runParams.hv_FeaturesFilter_Type.Append(codeTypeStruct.hv_Rect2Angle_Filter);
				}
			}
			int typeNum = 0;
			int typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount > 0)
			{
				//��ȡɸѡ������Сֵ
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

				//��ȡɸѡ�������ֵ
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

				//��ȡ���
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
//����ͼ����ǿ����
int DataIO::WriteParams_ImgProcessOne(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//д������(���鰴�ն��ŷָ�洢)

		//д��ͼ����ǿ����
		int typeCount = runParams.hv_ProcessType.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0 || runParams.hv_ProcessType == "null")
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", "null");
			WriteTxt("ͼ����ǿ����Ϊ��,�����ַ���null");
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

			//д���˲��˵Ŀ�
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


			//д���˲��˵ĸ�
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


			//д��Sigma
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


			//д��˫���˲��Ҷ���ֵ
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


			//д��Ҷ�����˷�����
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


			//д��Ҷ�����ӷ�����
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



		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ͼ����ǿ����Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		if (typeNum > 0)
		{
			WriteTxt("ͼ����ǿ����������ɹ�!");
		}

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ͼ����ǿ��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡͼ����ǿ����
ErrorCode_Xml DataIO::ReadParams_ImgProcessOne(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		//������ȡ
		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//ͼ����ǿ������ȡ
		//��ȡ����(���鰴�ն��ŷָ�洢)

		int typeNum = 0;
		//��ȡͼ����ǿ����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ����ǿ����hv_ProcessTypeʧ��");
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
				if (typeArray[i] == "��˹�˲�")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Gauss_Filter);
				}
				else if (typeArray[i] == "˫���˲�")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Bilateral_Filter);
				}
				else if (typeArray[i] == "��ֵ�˲�")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Mean_Image);
				}
				else if (typeArray[i] == "��ֵ�˲�")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Median_Rect);
				}
				else if (typeArray[i] == "������˲�")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Shock_Filter);
				}
				else if (typeArray[i] == "ֱ��ͼ����")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Equ_Histo_Image);
				}
				else if (typeArray[i] == "�Աȶ���ǿ")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Emphasize);
				}
				else if (typeArray[i] == "�Ҷ�����")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Scale_Image);
				}
				else if (typeArray[i] == "�Ҷ�������0-255")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Scale_ImageMax);
				}
				else if (typeArray[i] == "�Ҷȷ�ת")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Invert_Image);
				}
				else if (typeArray[i] == "�����任")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Log_Image);
				}
				else if (typeArray[i] == "ָ���任")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.Exp_Image);
				}
				else if (typeArray[i] == "�Ҷȸ�ʴ")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayErosion_Rect);
				}
				else if (typeArray[i] == "�Ҷ�����")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayDilation_Rect);
				}
				else if (typeArray[i] == "�Ҷȿ�����")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayOpening);
				}
				else if (typeArray[i] == "�Ҷȱ�����")
				{
					runParams.hv_ProcessType.Append(codeTypeStruct.GrayClosing);
				}
			}
			int typeCount = runParams.hv_ProcessType.TupleLength().TupleInt().I();
			typeNum = typeCount;
			if (typeCount == 0)
			{
				WriteTxt("ͼ����ǿ����Ϊ�գ�������ȡʧ��");
			}
			else
			{
				int isContainRectangle = 0;
				int isContainSigma = 0;
				int isContainBilateral = 0;
				int isContainScaleImg = 0;
				for (int i = 0; i < typeCount; i++)
				{
					if (runParams.hv_ProcessType[i] == "��ֵ�˲�" || runParams.hv_ProcessType[i] == "��ֵ�˲�" ||
						runParams.hv_ProcessType[i] == "�Աȶ���ǿ" || runParams.hv_ProcessType[i] == "�Ҷȸ�ʴ" ||
						runParams.hv_ProcessType[i] == "�Ҷ�����" || runParams.hv_ProcessType[i] == "�Ҷȿ�����" ||
						runParams.hv_ProcessType[i] == "�Ҷȱ�����")
					{
						isContainRectangle += 1;
					}
					else if (runParams.hv_ProcessType[i] == "��˹�˲�" || runParams.hv_ProcessType[i] == "������˲�")
					{
						isContainSigma += 1;

					}
					else if (runParams.hv_ProcessType[i] == "˫���˲�")
					{
						isContainSigma += 1;
						isContainBilateral += 1;
					}
					else if (runParams.hv_ProcessType[i] == "�Ҷ�����")
					{
						isContainScaleImg += 1;
					}
				}

				if (isContainRectangle > 0)
				{
					//��ȡ�˲��˿��
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_CoreWidthʧ��");
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

					//��ȡ�˲��˸߶�
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_CoreHeightʧ��");
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
					//��ȡSigma
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_Sigmaʧ��");
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
					//��ȡ˫���˲��Ҷ���ֵ
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralFilterThreshold", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_BilateralFilterThresholdʧ��");
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
					//��ȡ�Ҷ�����˷�����
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_MultValue", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_Scale_Image_MultValueʧ��");
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

					//��ȡ�Ҷ�����ӷ�����
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Scale_Image_AddValue", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡͼ����ǿ����hv_Scale_Image_AddValueʧ��");
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

		//�����������
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
			WriteTxt("ͼ����ǿ������ȡ�ɹ�!");
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼ����ǿ������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//������ͼ�������
int DataIO::WriteParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����ͼ�����������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", runParams.hv_ProcessType.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MultValue", to_string(runParams.hv_MultValue.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AddValue", to_string(runParams.hv_AddValue.TupleReal().D()));


		WriteTxt("��ͼ�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ͼ�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ͼ�������
ErrorCode_Xml DataIO::ReadParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ProcessType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ�������hv_ProcessTypeʧ��");
			return errorCode;
		}
		runParams.hv_ProcessType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MultValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ�������hv_MultValueʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MultValue = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AddValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ�������hv_AddValueʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_AddValue = tempDoubleValue;




		WriteTxt("��ͼ���������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������ͼ���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����ͼ��ת������
int DataIO::WriteParams_ImgConvert(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ImgConvertDetectType", runParams.hv_ImgConvertDetectType.S().Text());

		WriteTxt("ͼ��ת����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ͼ��ת����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡͼ��ת������
ErrorCode_Xml DataIO::ReadParams_ImgConvert(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;


		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgConvertDetectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ��ת������hv_ProcessTypeʧ��");
			return errorCode;
		}
		runParams.hv_ImgConvertDetectType = HTuple(valueXml.c_str());

		WriteTxt("ͼ��ת��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼ��ת��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����ͼ����ת����
int DataIO::WriteParams_ImgRotate(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", to_string(runParams.hv_Angle.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type", runParams.hv_Type.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MirrorDirection", runParams.hv_MirrorDirection.S().Text());

		WriteTxt("ͼ����ת��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ͼ����ת��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡͼ����ת����
ErrorCode_Xml DataIO::ReadParams_ImgRotate(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ����ת����hv_Angleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ����ת����hv_Typeʧ��");
			return errorCode;
		}
		runParams.hv_Type = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MirrorDirection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ����ת����hv_MirrorDirectionʧ��");
			return errorCode;
		}
		runParams.hv_MirrorDirection = HTuple(valueXml.c_str());

		WriteTxt("ͼ����ת������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼ����ת������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����ͼƬ�������
int DataIO::WriteParams_ImgClassify(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		int typeNum = 0;
		//д������(���鰴�ն��ŷָ�洢)
		//*******************************�������****************************************************************
		int typeCount = runParams.hv_ClassNames.size();
		typeNum = typeCount;
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", "null");
			WriteTxt("ͼƬ�����������Ϊ��,�����ַ���null");
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
			WriteTxt("ͼƬ�����������Ϊ��,�����ַ���null");
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

		//��������㷨
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ClassMethod", runParams.hv_ClassMethod.S().Text());

		//����ѵ���õ�ģ��
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


		WriteTxt("ͼƬ�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("ͼƬ���ౣ������������������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡͼƬ�������
ErrorCode_Xml DataIO::ReadParams_ImgClassify(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNames", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼƬ�������hv_ClassNamesʧ��");
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
			WriteTxt("ͼƬ�������Ϊ�գ���ȡͼƬ�������ʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePaths", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼƬ�������hv_ImgFilePathsʧ��");
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
			WriteTxt("ͼƬ�������Ϊ�գ���ȡͼƬ�������ʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ�����㷨
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡͼ��������hv_ClassMethodʧ��");
			return errorCode;
		}
		runParams.hv_ClassMethod = HTuple(valueXml.c_str());

		//��ȡģ��
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


		WriteTxt("ͼƬ����������سɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼƬ���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//���漫����任����
int DataIO::WriteParams_PolarTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_InnerRadius", to_string(runParams.hv_InnerRadius.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OuterRadius", to_string(runParams.hv_OuterRadius.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row", to_string(runParams.hv_Row.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column", to_string(runParams.hv_Column.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalWidth", to_string(runParams.hv_OriginalWidth.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalHeight", to_string(runParams.hv_OriginalHeight.TupleInt().I()));


		WriteTxt("������任��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������������任��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ������任����
ErrorCode_Xml DataIO::ReadParams_PolarTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_InnerRadius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_InnerRadiusʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_InnerRadius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OuterRadius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_OuterRadiusʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_OuterRadius = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_Rowʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_Columnʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_OriginalWidthʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_OriginalWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OriginalHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������任����hv_OriginalHeightʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_OriginalHeight = tempIntValue;



		WriteTxt("������任������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������������任������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����궨��궨����
int DataIO::WriteParams_CalibBoard(const string ConfigPath, const string XmlPath, const ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Error", to_string(resultParams.hv_Error.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", to_string(resultParams.hv_Resolution.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceCC", to_string(resultParams.hv_DistanceCC.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MapImgType", resultParams.hv_MapImgType.S().Text());



		//��������ڲκ����
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		WriteCamPar(resultParams.hv_CamParam, hv_CamParam_Path);
		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		WritePose(resultParams.hv_CamPose, hv_CamPose_Path);


		WriteTxt("�궨��궨��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML�������������궨��궨��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ�궨��궨����
ErrorCode_Xml DataIO::ReadParams_CalibBoard(const string ConfigPath, const string XmlPath, ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Error", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_Errorʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_Error = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_Resolutionʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_Resolution = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceCC", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_DistanceCCʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		resultParams.hv_DistanceCC = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MapImgType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_MapImgTypeʧ��");
			return errorCode;
		}
		resultParams.hv_MapImgType = HTuple(valueXml.c_str());


		//��ȡ����ڲκ����
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


		WriteTxt("�궨��궨������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ����������궨��궨������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�������ϱ궨�궨����
int DataIO::WriteParams_JointCalibration(const string ConfigPath, const string XmlPath, const RunParamsStruct_JointCalibrationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Error", to_string(runParams.hv_Error.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", to_string(runParams.hv_Resolution.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabThickness", to_string(runParams.hv_CaltabThickness.TupleReal().D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabDescrPath", runParams.hv_CaltabDescrPath.S().Text());
		//������Ҫ���궨���������
		int typeCount = runParams.hv_StartCamPar.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", "null");
			WriteTxt("�������Ϊ�գ�Ĭ�ϱ���Ϊnull");
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

		//����궨�������ڲκ����
		HTuple hv_CamParam_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CamPar.dat";
		WriteCamPar(runParams.hv_CamParam, hv_CamParam_Path);
		HTuple hv_CamPose_Path = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "CamPose.dat";
		WritePose(runParams.hv_CamPose, hv_CamPose_Path);


		WriteTxt("�궨��궨��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML�������������궨��궨��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ���ϱ궨�궨����
ErrorCode_Xml DataIO::ReadParams_JointCalibration(const string ConfigPath, const string XmlPath, RunParamsStruct_JointCalibrationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StartCamPar", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���ϱ궨������hv_StartCamParʧ��");
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
			WriteTxt("�������Ϊ�գ���ȡ���ϱ궨�������hv_StartCamParʧ��");
			return nullKeyName_Xml;
		}

		//������ȡ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Error", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_Errorʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Error = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Resolution", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_Resolutionʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Resolution = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabThickness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_CaltabThicknessʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_CaltabThickness = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CaltabDescrPath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�궨��궨����hv_CaltabDescrPathʧ��");
			return errorCode;
		}
		runParams.hv_CaltabDescrPath = HTuple(valueXml.c_str());


		//��ȡ����ڲκ����
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


		WriteTxt("�궨��궨������ȡ�ɹ�");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ����������궨��궨������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�������������
int DataIO::WriteParams_PhotoStereo(const string ConfigPath, const string XmlPath, const RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������������
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

		WriteTxt("��������������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������������������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ����������
ErrorCode_Xml DataIO::ReadParams_PhotoStereo(const string ConfigPath, const string XmlPath, RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��������ȡ
		//��ȡ����(���鰴�ն��ŷָ�洢)

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_PhotoStereoType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_PhotoStereoTypeʧ��");
			return errorCode;
		}
		runParams.hv_PhotoStereoType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Sigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Slants1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Slants2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants3", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Slants3ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants3 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Slants4", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Slants4ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Slants4 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Tilts1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Tilts2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts3", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Tilts3ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts3 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tilts4", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����������hv_Tilts4ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tilts4 = tempDoubleValue;


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������������������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//���渵��Ҷ�任����
int DataIO::WriteParams_FourierTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����Ҷ�任��������
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

		WriteTxt("����Ҷ�任��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML����������������Ҷ�任��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ����Ҷ�任����
ErrorCode_Xml DataIO::ReadParams_FourierTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����Ҷ�任������ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ResultType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_ResultTypeʧ��");
			return errorCode;
		}
		runParams.hv_ResultType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_FilterTypeʧ��");
			return errorCode;
		}
		runParams.hv_FilterType = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_Sigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_MaskWidthʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MaskWidth = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_MaskHeightʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MaskHeight = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Frequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_Frequencyʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Frequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BandMinFrequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_BandMinFrequencyʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_BandMinFrequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BandMaxFrequency", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_BandMaxFrequencyʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_BandMaxFrequency = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMinSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_GaussBandMinSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_GaussBandMinSigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GaussBandMaxSigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Ҷ�任����hv_GaussBandMaxSigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_GaussBandMaxSigma = tempDoubleValue;


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������Ҷ�任������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//�������任����
int DataIO::WriteParams_AffineMatrix(const string ConfigPath, const string XmlPath, const RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����任��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type_CreateHomMat2D", runParams.hv_Type_CreateHomMat2D.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Type_ApplyHomMat2D", runParams.hv_Type_ApplyHomMat2D.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ReverseAngle", to_string(runParams.hv_ReverseAngle.TupleInt().I()));

		WriteTxt("����任��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML����������������任��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ����任����
ErrorCode_Xml DataIO::ReadParams_AffineMatrix(const string ConfigPath, const string XmlPath, RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����任������ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type_CreateHomMat2D", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����任����hv_Type_CreateHomMat2Dʧ��");
			return errorCode;
		}
		runParams.hv_Type_CreateHomMat2D = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Type_ApplyHomMat2D", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����任����hv_Type_ApplyHomMat2Dʧ��");
			return errorCode;
		}
		runParams.hv_Type_ApplyHomMat2D = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ReverseAngle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����任����hv_ReverseAngleʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ReverseAngle = tempIntValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������任������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//�����ͼ����
int DataIO::WriteParams_CutImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��ͼ��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CutImgType", runParams.hv_CutImgType.S().Text());

		WriteTxt("��ͼ��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ͼ��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ͼ����
ErrorCode_Xml DataIO::ReadParams_CutImg(const string ConfigPath, const string XmlPath, RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ͼ������ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutImgType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ����hv_CutImgTypeʧ��");
			return errorCode;
		}
		runParams.hv_CutImgType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������ͼ������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//���洴��Roi����
int DataIO::WriteParams_CreateRoi(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��ͼ��������
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


		WriteTxt("����Roi����ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML����������������Roi��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ����Roi����
ErrorCode_Xml DataIO::ReadParams_CreateRoi(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ͼ������ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeRoi", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi��hv_TypeRoiʧ��");
			return errorCode;
		}
		runParams.hv_TypeRoi = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Row_Circleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Column_Circleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Circle", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Radius_Circleʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius_Circle = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Row_Ellipseʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Column_Ellipseʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Angle_Ellipseʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius1_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Radius1_Ellipseʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius1_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius2_Ellipse", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Radius2_Ellipseʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Radius2_Ellipse = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Row1_Rectangle1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row1_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Column1_Rectangle1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column1_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Row2_Rectangle1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row2_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Rectangle1", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Column2_Rectangle1ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column2_Rectangle1 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Row_Rectangle2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Row_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Column_Rectangle2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Column_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Angle_Rectangle2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Angle_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length1_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Length1_Rectangle2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length1_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Length2_Rectangle2", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_Length2_Rectangle2ʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Length2_Rectangle2 = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowBegin_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_RowBegin_Lineʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_RowBegin_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnBegin_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_ColumnBegin_Lineʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ColumnBegin_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowEnd_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_RowEnd_Lineʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_RowEnd_Line = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnEnd_Line", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_ColumnEnd_Lineʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ColumnEnd_Line = tempDoubleValue;



		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������Roi������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
////������ɫ������
//int DataIO::WriteParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//
//		if (runParams.hv_ColorHandle == NULL)
//		{
//			//���Ϊ��
//			//mutex1.unlock();
//			return 1;
//		}
//		//����ģ��
//		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetect_MoreNumHandle.ggc";
//		WriteClassGmm(runParams.hv_ColorHandle, PathHandle);
//
//		//������������
//		if (runParams.ho_SearchRegion.Key() != nullptr)
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
//		}
//
//		//д������(���鰴�ն��ŷָ�洢)
//		int typeCount = runParams.hv_MeanValue_R.size();
//		if (runParams.hv_MeanValue_R.size() <= 0 || runParams.hv_MeanValue_G.size() <= 0 || runParams.hv_MeanValue_B.size() <= 0)
//		{
//			WriteTxt("��ɫ���ѵ������Ϊ��,��������ʧ��");
//			return 1;
//		}
//		else
//		{
//			//д��Rͨ���Ҷ�ֵ
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
//			//д��Gͨ���Ҷ�ֵ
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
//			//д��Bͨ���Ҷ�ֵ
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
//		//д����ɫ����
//		typeCount = runParams.hv_ColorName.size();
//		if (typeCount <= 0)
//		{
//			WriteTxt("��ɫ�����ɫ����ѵ������Ϊ��,��������ʧ��");
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
//		WriteTxt("XML��������������ɫ����������ʧ��!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////��ȡ��ɫ������
//ErrorCode_Xml DataIO::ReadParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//
//
//		//�����������
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
//		//��ȡģ��
//		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetect_MoreNumHandle.ggc";
//		ReadClassGmm(PathHandle, &runParams.hv_ColorHandle);
//
//
//		//��ȡ����(���鰴�ն��ŷָ�洢)
//		//Rͨ���Ҷ�ֵ
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ɫ������hv_MeanValue_Rʧ��");
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
//		//Gͨ���Ҷ�ֵ
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ɫ������hv_MeanValue_Gʧ��");
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
//		//Bͨ���Ҷ�ֵ
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ɫ������hv_MeanValue_Bʧ��");
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
//		//��ɫ����
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorName", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ɫ������hv_ColorNameʧ��");
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
//		WriteTxt("XML��ȡ�����������ɫ��������ȡʧ��!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}
//������ɫ������
int DataIO::WriteParams_ColorDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		if (runParams.hv_ColorHandle == NULL)
		{
			//���Ϊ��
			//mutex1.unlock();
			return 1;
		}
		//����ģ��
		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetectHandle.ggc";
		WriteClassGmm(runParams.hv_ColorHandle, PathHandle);

		//������������
		if (runParams.ho_SearchRegion.Key() != nullptr)
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}
		//����ѵ������
		if (runParams.ho_TrainRegions.Key() != nullptr)
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(runParams.ho_TrainRegions, PathRunRegion);
		}

		//д��Rͨ���Ҷ�ֵ
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", to_string(runParams.hv_MeanValue_R.TupleInt().I()));

		//д��Gͨ���Ҷ�ֵ
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", to_string(runParams.hv_MeanValue_G.TupleInt().I()));

		//д��Bͨ���Ҷ�ֵ
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", to_string(runParams.hv_MeanValue_B.TupleInt().I()));

		//������С����
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
		WriteTxt("XML��������������ɫ����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ɫ������
ErrorCode_Xml DataIO::ReadParams_ColorDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		double tempDoubleValue;
		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;


		//�����������
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

		//ѵ���������
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

		//��ȡģ��
		HTuple PathHandle = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ColorDetectHandle.ggc";
		ReadClassGmm(PathHandle, &runParams.hv_ColorHandle);

		//Rͨ���Ҷ�ֵ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_R", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_MeanValue_Rʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_R = tempDoubleValue;

		//Gͨ���Ҷ�ֵ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_G", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_MeanValue_Gʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_G = tempDoubleValue;

		//Bͨ���Ҷ�ֵ
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanValue_B", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_MeanValue_Bʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MeanValue_B = tempDoubleValue;

		//��ȡ��С����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����Roi����hv_MinScoreʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MinScore = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����isFollowʧ��");
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
		WriteTxt("XML��ȡ�����������ɫ��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//������ϲ���
int DataIO::WriteParams_FitDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��ϲ�������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FitType", runParams.hv_FitType.S().Text());

		WriteTxt("��ϲ�������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ϲ�������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ϲ���
ErrorCode_Xml DataIO::ReadParams_FitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ϲ�����ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FitType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ϲ���hv_FitTypeʧ��");
			return errorCode;
		}
		runParams.hv_FitType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������ϲ�����ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//��������任����
int DataIO::WriteParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//ת�����Ͳ�������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TypeTrans", runParams.hv_TypeTrans.S().Text());

		WriteTxt("����ת����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML����������������ת����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ����任����
ErrorCode_Xml DataIO::ReadParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����任������ȡ
		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeTrans", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����任����hv_TypeTransʧ��");
			return errorCode;
		}
		runParams.hv_TypeTrans = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������任������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//���洴����������
int DataIO::WriteParams_CreateContourDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//ת�����Ͳ�������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_TypeCreateContour", runParams.hv_TypeCreateContour.S().Text());

		WriteTxt("����������������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML����������������������������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ������������
ErrorCode_Xml DataIO::ReadParams_CreateContourDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����任������ȡ
		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_TypeCreateContour", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������������hv_TypeCreateContourʧ��");
			return errorCode;
		}
		runParams.hv_TypeCreateContour = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������任������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}


//���潻�����
int DataIO::WriteParams_Intersection(const string ConfigPath, const string XmlPath, const RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IntersectType", runParams.hv_IntersectType.S().Text());

		WriteTxt("�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML�����������������������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ�������
ErrorCode_Xml DataIO::ReadParams_Intersection(const string ConfigPath, const string XmlPath, RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//���������ȡ

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IntersectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�������hv_IntersectTypeʧ��");
			return errorCode;
		}
		runParams.hv_IntersectType = HTuple(valueXml.c_str());


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ������������������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//�����������(����ʹ�ÿ�ѧ������)
int DataIO::WriteParams_Measurement(const string ConfigPath, const string XmlPath, const RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureType", runParams.hv_MeasureType.S().Text());
		//�������ص���
		//����11λ��Чλ��(����С��λ)
		std::stringstream ss;
		ss << std::setprecision(11) << runParams.hv_PixelMm.TupleReal().D();
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_PixelMm", ss.str());
		WriteTxt("������������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������������������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��������
ErrorCode_Xml DataIO::ReadParams_Measurement(const string ConfigPath, const string XmlPath, RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��������hv_MeasureTypeʧ��");
			return errorCode;
		}
		runParams.hv_MeasureType = HTuple(valueXml.c_str());
		//���ص���
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_PixelMm", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��������hv_PixelMmʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_PixelMm = tempDoubleValue;

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�������������������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�����ͼ����
int DataIO::WriteParams_SaveImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SavePath", runParams.hv_SavePath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SaveType", runParams.hv_SaveType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SaveMethod", runParams.hv_SaveMethod);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FileName", runParams.hv_FileName);

		WriteTxt("��ͼ��������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML��������������ͼ��������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡ��ͼ����
ErrorCode_Xml DataIO::ReadParams_SaveImg(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SavePath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ����hv_SavePathʧ��");
			return errorCode;
		}
		runParams.hv_SavePath = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SaveType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ����hv_SaveTypeʧ��");
			return errorCode;
		}
		runParams.hv_SaveType = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SaveMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ����hv_SaveMethodʧ��");
			return errorCode;
		}
		runParams.hv_SaveMethod = valueXml.c_str();

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FileName", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ��ͼ����hv_FileNameʧ��");
			return errorCode;
		}
		runParams.hv_FileName = valueXml.c_str();


		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ�����������ϲ�����ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����OKNG�жϵĲ���
int DataIO::WriteParams_Judgement(const string ConfigPath, const string XmlPath, const RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		//д������(���鰴�ն��ŷָ�洢)
		//
		//ȫ�ֱ�������
		int typeCount = runParams.hv_GlobalName.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG�ж�ģ�����Ϊ�գ�����ʧ��");
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

		//��Сֵ
		typeCount = runParams.hv_MinValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG�ж�ģ�����Ϊ�գ�����ʧ��");
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

		//���ֵ
		typeCount = runParams.hv_MaxValue.size();
		if (typeCount == 0)
		{
			WriteTxt("OKNG�ж�ģ�����Ϊ�գ�����ʧ��");
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

		//��ǩ
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

		WriteTxt("OKNG�ж�ģ���������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������OKNG�ж�ģ���������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡOKNG�жϵĲ���
ErrorCode_Xml DataIO::ReadParams_Judgement(const string ConfigPath, const string XmlPath, RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		double tempDoubleValue;
		string valueXml;
		ErrorCode_Xml errorCode;


		//��ȡ����(���鰴�ն��ŷָ�洢)
		//
		//��ȡȫ�ֱ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GlobalName", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡOKNG�жϲ���hv_GlobalNameʧ��");
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
			WriteTxt("OKNG�жϲ���Ϊ�գ���ȡOKNG�жϲ�������hv_GlobalNameʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ��Сֵ
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡOKNG�жϲ���hv_MinValueʧ��");
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
			WriteTxt("OKNG�жϲ���Ϊ�գ���ȡOKNG�жϲ�������hv_MinValueʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ���ֵ
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡOKNG�жϲ���hv_MaxValueʧ��");
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
			WriteTxt("OKNG�жϲ���Ϊ�գ���ȡOKNG�жϲ�������hv_MaxValueʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ��ǩ
		typeArray.clear();
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LabelStr", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡOKNG�жϲ���hv_LabelStrʧ��");
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
			WriteTxt("OKNG�жϲ���Ϊ�գ���ȡOKNG�жϲ�������hv_LabelStrʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AndOr", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡOKNG�жϲ���hv_AndOrʧ��");
			return errorCode;
		}
		runParams.hv_AndOr = valueXml;


		WriteTxt("OKNG�жϲ�����ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������OKNG�жϲ�����ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//������ʾ���ڲ���
int DataIO::WriteParams_ImgShow(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
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
//��ȡ��ʾ���ڲ���
ErrorCode_Xml DataIO::ReadParams_ImgShow(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LineWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������ʾ����hv_LineWidthʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_LineWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������ʾ����hv_Word_Xʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Word_X = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������ʾ����hv_Word_Yʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Word_Y = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Word_Size", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������ʾ����hv_Word_Sizeʧ��");
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

//���½ṹ��� RobotCalibDetect ��Ľṹ���ͻ����������� 20250507
////��������˶�λ��������(runParams ��λ����������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID)
//int DataIO::WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//		//д������(���鰴�ն��ŷָ�洢)
//		int typeCount = runParams.hv_NPointCamera_X.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("�ŵ�궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("�ŵ�궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("�ŵ�궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("�ŵ�궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("��ת���ı궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("��ת���ı궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("��ת���ı궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("��ת���ı궨��������Ϊ�գ�����ʧ��");
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
//			WriteTxt("��ת���ı궨�����Ƕ�Ϊ�գ�����ʧ��");
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
//		//�����������
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
//		//����ŵ�궨����
//		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
//		WriteTuple(runParams.hv_HomMat2D, PathTuple);
//
//
//		WriteTxt("�����˱궨��������ɹ�!");
//		//mutex1.unlock();
//		return 0;
//	}
//	catch (...)
//	{
//		WriteTxt("XML�����������������˱궨��������ʧ��!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////��ȡ�����˶�λ����(runParams ��λ����������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID, value �����ȡ����ֵ)
//ErrorCode_Xml DataIO::ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//		double tempDoubleValue;
//
//		//��ȡ����(���鰴�ն��ŷָ�洢)
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_NPointCamera_Xʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_NPointCamera_Xʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointCamera_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_NPointCamera_Yʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_NPointCamera_Yʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_NPointRobot_Xʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_NPointRobot_Xʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NPointRobot_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_NPointRobot_Yʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_NPointRobot_Yʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterCamera_Xʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_CenterCamera_Xʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterCamera_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterCamera_Yʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_CenterCamera_Yʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterRobot_Xʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_CenterRobot_Xʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterRobot_Yʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_CenterRobot_Yʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterRobot_A", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterRobot_Aʧ��");
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
//			WriteTxt("�����˱궨����Ϊ�գ���ȡ����hv_CenterRobot_Aʧ��");
//			return nullKeyName_Xml;
//		}
//
//
//		//��ȡ�������
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_Center_Xʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_X = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_Center_Yʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_Y = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Center_Radius", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_Center_Radiusʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Center_Radius = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelXʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelYʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelAʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelPosition_RobotXʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelPosition_RobotYʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelPosition_RobotA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_ModelPosition_RobotAʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_ModelPosition_RobotA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotX", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterPosition_RobotXʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotX = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotY", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterPosition_RobotYʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotY = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CenterPosition_RobotA", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_CenterPosition_RobotAʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_CenterPosition_RobotA = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistancePP_Robot", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_DistancePP_Robotʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_DistancePP_Robot = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleRotate_Robot", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_AngleRotate_Robotʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_AngleRotate_Robot = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsCamFirst", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ�����˱궨����hv_IsCamFirstʧ��");
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
//			WriteTxt("XML��ȡ�����˱궨����hv_IsReverseAngleʧ��");
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
//			WriteTxt("XML��ȡ�����˱궨����hv_IsAbsCoordinateʧ��");
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
//			WriteTxt("XML��ȡ�����˱궨����hv_IsUseCenterCalibʧ��");
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
//		//�ŵ�궨�������
//		HTuple PathTuple = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CalibId.tuple";
//		HTuple isExist;
//		FileExists(PathTuple, &isExist);
//		if (isExist > 0)
//		{
//			ReadTuple(PathTuple, &runParams.hv_HomMat2D);
//		}
//
//
//		WriteTxt("�����˱궨������ȡ�ɹ�!");
//		//mutex1.unlock();
//		return Ok_Xml;
//
//	}
//	catch (...)
//	{
//		WriteTxt("XML��ȡ��������������˱궨������ȡʧ��!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}

////����Pin�������(����ʹ�ÿ�ѧ������)
//int DataIO::WriteParams_PinDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//		//д������(���鰴�ն��ŷָ�洢)
//		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("X�����׼����Ϊ�գ�����ʧ��");
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
//			WriteTxt("Y�����׼����Ϊ�գ�����ʧ��");
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
//			WriteTxt("Pin����󶨵�X��׼���Ϊ�գ�����ʧ��");
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
//			WriteTxt("Pin����󶨵�Y��׼���Ϊ�գ�����ʧ��");
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
//			WriteTxt("��׼����Ϊ�գ�����ʧ��");
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
//			WriteTxt("ֱ�����row������Բ��rowΪ�գ�����ʧ��");
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
//			WriteTxt("ֱ�����column������Բ��columnΪ�գ�����ʧ��");
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
//			WriteTxt("ֱ���յ�rowΪ�գ�����ʧ��");
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
//			WriteTxt("ֱ���յ�columnΪ�գ�����ʧ��");
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
//			WriteTxt("Բ�뾶Ϊ�գ�����ʧ��");
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
//			WriteTxt("���߰볤Ϊ�գ�����ʧ��");
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
//			WriteTxt("���߰��Ϊ�գ�����ʧ��");
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
//			WriteTxt("ƽ��ֵΪ�գ�����ʧ��");
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
//			WriteTxt("��Ե��ֵΪ�գ�����ʧ��");
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
//			WriteTxt("���߼���Ϊ�գ�����ʧ��");
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
//			WriteTxt("��Եѡ��Ϊ�գ�����ʧ��");
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
//			WriteTxt("���߸���Ϊ�գ�����ʧ��");
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
//			WriteTxt("��С�÷�Ϊ�գ�����ʧ��");
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
//			WriteTxt("������ֵΪ�գ�����ʧ��");
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
//			WriteTxt("�������Ϊ�գ�����ʧ��");
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
//			WriteTxt("����ʽΪ�գ�����ʧ��");
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
//			WriteTxt("��ֵ������Ϊ�գ�����ʧ��");
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
//			WriteTxt("����ֵΪ�գ�����ʧ��");
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
//			WriteTxt("����ֵΪ�գ�����ʧ��");
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
//			WriteTxt("sigmaΪ�գ�����ʧ��");
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
//			WriteTxt("�˲��˿��Ϊ�գ�����ʧ��");
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
//			WriteTxt("�˲��˸߶�Ϊ�գ�����ʧ��");
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
//			WriteTxt("��̬��ֵ�ָ�ͼ��Աȶ�Ϊ�գ�����ʧ��");
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
//			WriteTxt("��̬��ֵ����ѡ��Ϊ�գ�����ʧ��");
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
//			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ�볤MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡhv_CircularityFilter_MinΪ�գ�����ʧ��");
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
//			WriteTxt("ɸѡhv_RectangularityFilter_MinΪ�գ�����ʧ��");
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
//			WriteTxt("ɸѡhv_WidthFilter_MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡhv_HeightFilter_MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ�볤MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡhv_CircularityFilter_MaxΪ�գ�����ʧ��");
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
//			WriteTxt("ɸѡhv_RectangularityFilter_MaxΪ�գ�����ʧ��");
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
//			WriteTxt("ɸѡhv_WidthFilter_MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("ɸѡhv_HeightFilter_MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("hv_FillUpShape_MinΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("hv_FillUpShape_MaxΪ�գ�����ʧ��");
//			return 1;
//		}
//		if (typeCount == 1)
//		{
//			//����11λ��Чλ��(����С��λ)
//			std::stringstream ss;
//			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
//			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
//			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
//		}
//		else
//		{
//			//����11λ��Чλ��(����С��λ)
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
//			WriteTxt("��̬ѧ����Ϊ�գ�����ʧ��");
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
//			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
//			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
//			WriteTxt("��̬ѧ�뾶Ϊ�գ�����ʧ��");
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
//			WriteTxt("hv_SelectAreaMaxΪ�գ�����ʧ��");
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
//			WriteTxt("hv_BlobCountΪ�գ�����ʧ��");
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
//		//�����������
//		//�����ƫ�Χ
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", to_string(runParams.hv_Tolerance_X.D()));
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", to_string(runParams.hv_Tolerance_Y.D()));
//		//���ص���
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.D()));
//		//CSV·��
//		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", runParams.FilePath_Csv);
//
//		//����ģ��ƥ�����
//		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
//		if (typeCount == 0)
//		{
//			WriteTxt("ģ��ƥ������Ϊ�գ�����ʧ��");
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
//			WriteTxt("ģ����ʼ�Ƕ�Ϊ�գ�����ʧ��");
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
//			WriteTxt("ģ����ֹ�Ƕ�Ϊ�գ�����ʧ��");
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
//			WriteTxt("ģ����������СֵΪ�գ�����ʧ��");
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
//			WriteTxt("ģ�����������ֵΪ�գ�����ʧ��");
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
//			WriteTxt("ģ����������СֵΪ�գ�����ʧ��");
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
//			WriteTxt("ģ�����������ֵΪ�գ�����ʧ��");
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
//			WriteTxt("ģ����С����Ϊ�գ�����ʧ��");
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
//			WriteTxt("ģ��ƥ�����Ϊ�գ�����ʧ��");
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
//		//����ģ��ƥ��ģ��
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
//			WriteTxt("ģ��ƥ��ģ��IDΪ��");
//		}
//
//		//������������
//		if (runParams.ho_SearchRegion.Key() == nullptr)
//		{
//			//ͼ��Ϊ�գ�����ͼ��clear
//			WriteTxt("��������Ϊ�գ�����ʧ��");
//		}
//		else
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
//			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
//		}
//
//		//����ѵ������
//		if (runParams.ho_TrainRegion.Key() == nullptr)
//		{
//			//ͼ��Ϊ�գ�����ͼ��clear
//			WriteTxt("ѵ������Ϊ�գ�����ʧ��");
//		}
//		else
//		{
//			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
//			WriteRegion(runParams.ho_TrainRegion, PathTrainRegion);
//		}
//
//
//		//����λ������ģ��ƥ�����
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
//		//����λ������ģ��ƥ��ģ��
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
//			WriteTxt("λ������ģ��ƥ��ģ��IDΪ��");
//		}
//		//����λ����������������
//		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
//		{
//			//ͼ��Ϊ�գ�����ͼ��clear
//			WriteTxt("λ��������������Ϊ�գ�����ʧ��");
//		}
//		else
//		{
//			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
//			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
//		}
//		//����λ��������ѵ������
//		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
//		{
//			//ͼ��Ϊ�գ�����ͼ��clear
//			WriteTxt("λ������ѵ������Ϊ�գ�����ʧ��");
//		}
//		else
//		{
//			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
//			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
//		}
//
//
//		WriteTxt("Pin�����������ɹ�!");
//		//mutex1.unlock();
//		return 0;
//	}
//	catch (...)
//	{
//		WriteTxt("XML������������Pin�����������ʧ��!");
//		//mutex1.unlock();
//		return -1;
//	}
//}
////��ȡPin�������
//ErrorCode_Xml DataIO::ReadParams_PinDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
//{
//	//mutex1.lock();
//	try
//	{
//		XmlClass2 xmlC;//xml��ȡ����
//		//·���ж�
//		PathSet(ConfigPath, XmlPath);
//
//		string valueXml;
//		ErrorCode_Xml errorCode;
//		int tempIntValue;
//		double tempDoubleValue;
//
//		//��ȡ����(���鰴�ն��ŷָ�洢)
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡPin������hv_StandardX_Arrayʧ��");
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
//			WriteTxt("��ȡ����hv_StandardX_Arrayʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML������hv_StandardY_Arrayʧ��");
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
//			WriteTxt("��ȡ����hv_StandardY_Arrayʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML����hv_X_BindingNumʧ��");
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
//			WriteTxt("ȡ����hv_X_BindingNumʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Y_BindingNumʧ��");
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
//			WriteTxt("��ȡ����hv_Y_BindingNumʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_BaseTypeʧ��");
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
//			WriteTxt("��ȡ����hv_BaseTypeʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Row1_Baseʧ��");
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
//			WriteTxt("��ȡ����hv_Row1_Baseʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Column1_Baseʧ��");
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
//			WriteTxt("��ȡ����hv_Column1_Baseʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Row2_Baseʧ��");
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
//			WriteTxt("��ȡ����hv_Row2_Baseʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Column2_Baseʧ��");
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
//			WriteTxt("��ȡ����hv_Column2_Baseʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Radius_Baseʧ��");
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
//			WriteTxt("��ȡ����hv_Radius_Baseʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureLength1ʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureLength1ʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureLength2ʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureLength2ʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureSigmaʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureSigmaʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureThresholdʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureThresholdʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureTransitionʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureTransitionʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureSelectʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureSelectʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureNumʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureNumʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MeasureMinScoreʧ��");
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
//			WriteTxt("��ȡ����hv_MeasureMinScoreʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_DistanceThresholdʧ��");
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
//			WriteTxt("��ȡ����hv_DistanceThresholdʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_DetectTypeʧ��");
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
//			WriteTxt("��ȡ����hv_DetectTypeʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_SortTypeʧ��");
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
//			WriteTxt("��ȡ����hv_SortTypeʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ThresholdTypeʧ��");
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
//			WriteTxt("��ȡ����hv_ThresholdTypeʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_LowThresholdʧ��");
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
//			WriteTxt("��ȡ����hv_LowThresholdʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_HighThresholdʧ��");
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
//			WriteTxt("��ȡ����hv_HighThresholdʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_Sigmaʧ��");
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
//			WriteTxt("��ȡ����hv_Sigmaʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_CoreWidthʧ��");
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
//			WriteTxt("��ȡ����hv_CoreWidthʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_CoreHeightʧ��");
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
//			WriteTxt("��ȡ����hv_CoreHeightʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_DynThresholdContrastʧ��");
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
//			WriteTxt("��ȡ����hv_DynThresholdContrastʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_DynThresholdPolarityʧ��");
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
//			WriteTxt("��ȡ����hv_DynThresholdPolarityʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_AreaFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_AreaFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RecLen1Filter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_RecLen1Filter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RecLen2Filter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_RecLen2Filter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RowFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_RowFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ColumnFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_ColumnFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_CircularityFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_CircularityFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RectangularityFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_RectangularityFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_WidthFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_WidthFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_HeightFilter_Minʧ��");
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
//			WriteTxt("��ȡ����hv_HeightFilter_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_AreaFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_AreaFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RecLen1Filter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_RecLen1Filter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RecLen2Filter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_RecLen2Filter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RowFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_RowFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ColumnFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_ColumnFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_CircularityFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_CircularityFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_RectangularityFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_RectangularityFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_WidthFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_WidthFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_HeightFilter_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_HeightFilter_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_FillUpShape_Minʧ��");
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
//			WriteTxt("��ȡ����hv_FillUpShape_Minʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_FillUpShape_Maxʧ��");
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
//			WriteTxt("��ȡ����hv_FillUpShape_Maxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_OperatorTypeʧ��");
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
//			WriteTxt("��ȡ����hv_OperatorTypeʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_OperaRec_Widthʧ��");
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
//			WriteTxt("��ȡ����hv_OperaRec_Widthʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_OperaRec_Heightʧ��");
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
//			WriteTxt("��ȡ����hv_OperaRec_Heightʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_OperaCir_Radiusʧ��");
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
//			WriteTxt("��ȡ����hv_OperaCir_Radiusʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_SelectAreaMaxʧ��");
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
//			WriteTxt("��ȡ����hv_SelectAreaMaxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_BlobCountʧ��");
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
//			WriteTxt("��ȡ����hv_BlobCountʧ��");
//			return nullKeyName_Xml;
//		}
//
//		//��ȡ�������
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡPin�����hv_Tolerance_Xʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Tolerance_X = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡPin�����hv_Tolerance_Yʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_Tolerance_Y = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡPin�����hv_MmPixelʧ��");
//			return errorCode;
//		}
//		xmlC.StrToDouble(valueXml, tempDoubleValue);
//		runParams.hv_MmPixel = tempDoubleValue;
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡPin�����FilePath_Csvʧ��");
//			return errorCode;
//		}
//		runParams.FilePath_Csv = valueXml;
//
//
//		//��ȡģ��ƥ�����
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MatchMethodʧ��");
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
//			WriteTxt("��ȡ����hv_MatchMethodʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_AngleStartʧ��");
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
//			WriteTxt("��ȡ����hv_AngleStartʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_AngleExtentʧ��");
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
//			WriteTxt("��ȡ����hv_AngleExtentʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ScaleRMinʧ��");
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
//			WriteTxt("��ȡ����hv_ScaleRMinʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ScaleRMaxʧ��");
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
//			WriteTxt("��ȡ����hv_ScaleRMaxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ScaleCMinʧ��");
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
//			WriteTxt("��ȡ����hv_ScaleCMinʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_ScaleCMaxʧ��");
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
//			WriteTxt("��ȡ����hv_ScaleCMaxʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MinScoreʧ��");
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
//			WriteTxt("��ȡ����hv_MinScoreʧ��");
//			return nullKeyName_Xml;
//		}
//
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_NumMatchesʧ��");
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
//			WriteTxt("��ȡ����hv_NumMatchesʧ��");
//			return nullKeyName_Xml;
//		}
//
//
//		//�����������
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
//		//ѵ���������
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
//		//��ȡģ��ƥ��ģ��
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
//					//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
//					HTuple emptyID = NULL;
//					runParams.hv_ModelID.Append(emptyID);
//				}
//			}
//			else
//			{
//				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
//				HTuple emptyID = NULL;
//				runParams.hv_ModelID.Append(emptyID);
//			}
//		}
//
//
//
//		//��ȡλ������ģ��ƥ�����
//		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
//		if (errorCode != Ok_Xml)
//		{
//			WriteTxt("XML��ȡ����hv_MatchMethod_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_AngleStart_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_AngleExtent_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_ScaleRMin_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_ScaleRMax_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_ScaleCMin_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_ScaleCMax_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_MinScore_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_NumMatches_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_Check_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_Row_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_Column_PositionCorrectionʧ��");
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
//			WriteTxt("XML��ȡ����hv_Angle_PositionCorrectionʧ��");
//		}
//		else
//		{
//			runParams.hv_Angle_PositionCorrection.Clear();
//			xmlC.StrToDouble(valueXml, tempDoubleValue);
//			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
//		}
//
//		//λ�����������������
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
//		//λ������ѵ���������
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
//		//��ȡλ��������ģ��ƥ��ģ��
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
//				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
//				runParams.hv_ModelID_PositionCorrection = NULL;
//			}
//		}
//		catch (...)
//		{
//			WriteTxt("PCB������hv_ModelID_PositionCorrection��ȡʧ��!");
//		}
//
//
//		WriteTxt("Pin��������ȡ�ɹ�!");
//		//mutex1.unlock();
//		return Ok_Xml;
//
//	}
//	catch (...)
//	{
//		WriteTxt("XML��ȡ���������Pin��������ȡʧ��!");
//		//mutex1.unlock();
//		return nullUnKnow_Xml;
//	}
//}
//����Pcb������(����ʹ�ÿ�ѧ������)
int DataIO::WriteParams_PcbDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//д������(���鰴�ն��ŷָ�洢)
		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("X�����׼����Ϊ�գ�����ʧ��");
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
			WriteTxt("Y�����׼����Ϊ�գ�����ʧ��");
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
			WriteTxt("�ǶȻ�׼����Ϊ�գ�����ʧ��");
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
			WriteTxt("�����׼����Ϊ�գ�����ʧ��");
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
			WriteTxt("Pin����󶨵�X��׼���Ϊ�գ�����ʧ��");
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
			WriteTxt("Pin����󶨵�Y��׼���Ϊ�գ�����ʧ��");
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
			WriteTxt("�������Ϊ�գ�����ʧ��");
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
			WriteTxt("����ʽΪ�գ�����ʧ��");
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
			WriteTxt("��ֵ������Ϊ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("sigmaΪ�գ�����ʧ��");
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
			WriteTxt("�˲��˿��Ϊ�գ�����ʧ��");
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
			WriteTxt("�˲��˸߶�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ�ָ�ͼ��Աȶ�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ����ѡ��Ϊ�գ�����ʧ��");
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
			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ�볤MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_CircularityFilter_MinΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_RectangularityFilter_MinΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_WidthFilter_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_HeightFilter_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ�볤MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_CircularityFilter_MaxΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_RectangularityFilter_MaxΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_WidthFilter_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_HeightFilter_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("hv_FillUpShape_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("hv_FillUpShape_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("��̬ѧ����Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ�뾶Ϊ�գ�����ʧ��");
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
			WriteTxt("hv_SelectAreaMaxΪ�գ�����ʧ��");
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
			WriteTxt("hv_BlobCountΪ�գ�����ʧ��");
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

		//�����������
		//�����ƫ�Χ
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", to_string(runParams.hv_Tolerance_X.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", to_string(runParams.hv_Tolerance_Y.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_A", to_string(runParams.hv_Tolerance_A.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Area", to_string(runParams.hv_Tolerance_Area.D()));
		//���ص���
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.D()));

		//����ģ��ƥ�����
		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("ģ��ƥ������Ϊ�գ�����ʧ��");
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
			WriteTxt("ģ����ʼ�Ƕ�Ϊ�գ�����ʧ��");
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
			WriteTxt("ģ����ֹ�Ƕ�Ϊ�գ�����ʧ��");
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
			WriteTxt("ģ����������СֵΪ�գ�����ʧ��");
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
			WriteTxt("ģ�����������ֵΪ�գ�����ʧ��");
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
			WriteTxt("ģ����������СֵΪ�գ�����ʧ��");
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
			WriteTxt("ģ�����������ֵΪ�գ�����ʧ��");
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
			WriteTxt("ģ����С����Ϊ�գ�����ʧ��");
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
			WriteTxt("ģ��ƥ�����Ϊ�գ�����ʧ��");
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

		//����ģ��ƥ��ģ��
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
			WriteTxt("ģ��ƥ��ģ��IDΪ��");
		}

		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("��������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		//����ѵ������
		if (runParams.ho_TrainRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ѵ������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(runParams.ho_TrainRegion, PathTrainRegion);
		}


		//����λ������ģ��ƥ�����
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
		//����λ������ģ��ƥ��ģ��
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
			WriteTxt("λ������ģ��ƥ��ģ��IDΪ��");
		}
		//����λ����������������
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ��������������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//����λ��������ѵ������
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ������ѵ������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}


		WriteTxt("Pin�����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������Pin�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡPcb������
ErrorCode_Xml DataIO::ReadParams_PcbDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin������hv_StandardX_Arrayʧ��");
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
			WriteTxt("��ȡ����hv_StandardX_Arrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML������hv_StandardY_Arrayʧ��");
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
			WriteTxt("��ȡ����hv_StandardY_Arrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardA_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML������hv_StandardA_Arrayʧ��");
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
			WriteTxt("��ȡ����hv_StandardA_Arrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardArea_Array", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML������hv_StandardArea_Arrayʧ��");
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
			WriteTxt("��ȡ����hv_StandardArea_Arrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML����hv_X_BindingNumʧ��");
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
			WriteTxt("ȡ����hv_X_BindingNumʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Y_BindingNumʧ��");
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
			WriteTxt("��ȡ����hv_Y_BindingNumʧ��");
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
				WriteTxt("XML��ȡ����hv_MeasureLength1ʧ��");
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
				WriteTxt("��ȡ����hv_MeasureLength1ʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureLength2ʧ��");
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
				WriteTxt("��ȡ����hv_MeasureLength2ʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureSigmaʧ��");
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
				WriteTxt("��ȡ����hv_MeasureSigmaʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureThresholdʧ��");
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
				WriteTxt("��ȡ����hv_MeasureThresholdʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureTransitionʧ��");
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
				WriteTxt("��ȡ����hv_MeasureTransitionʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureSelectʧ��");
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
				WriteTxt("��ȡ����hv_MeasureSelectʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureNumʧ��");
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
				WriteTxt("��ȡ����hv_MeasureNumʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_MeasureMinScoreʧ��");
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
				WriteTxt("��ȡ����hv_MeasureMinScoreʧ��");
				return nullKeyName_Xml;
			}

			errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
			if (errorCode != Ok_Xml)
			{
				WriteTxt("XML��ȡ����hv_DistanceThresholdʧ��");
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
				WriteTxt("��ȡ����hv_DistanceThresholdʧ��");
				return nullKeyName_Xml;
			}
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DetectTypeʧ��");
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
			WriteTxt("��ȡ����hv_DetectTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_SortTypeʧ��");
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
			WriteTxt("��ȡ����hv_SortTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ThresholdTypeʧ��");
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
			WriteTxt("��ȡ����hv_ThresholdTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_LowThresholdʧ��");
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
			WriteTxt("��ȡ����hv_LowThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HighThresholdʧ��");
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
			WriteTxt("��ȡ����hv_HighThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Sigmaʧ��");
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
			WriteTxt("��ȡ����hv_Sigmaʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreWidthʧ��");
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
			WriteTxt("��ȡ����hv_CoreWidthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreHeightʧ��");
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
			WriteTxt("��ȡ����hv_CoreHeightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdContrastʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdContrastʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdPolarityʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdPolarityʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AreaFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_AreaFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen1Filter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RecLen1Filter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen2Filter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RecLen2Filter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RowFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RowFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ColumnFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_ColumnFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CircularityFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_CircularityFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RectangularityFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RectangularityFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WidthFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_WidthFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HeightFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_HeightFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AreaFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_AreaFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen1Filter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RecLen1Filter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen2Filter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RecLen2Filter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RowFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RowFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ColumnFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_ColumnFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CircularityFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_CircularityFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RectangularityFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RectangularityFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WidthFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_WidthFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HeightFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_HeightFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Minʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Maxʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperatorTypeʧ��");
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
			WriteTxt("��ȡ����hv_OperatorTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Widthʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Widthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Heightʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Heightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaCir_Radiusʧ��");
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
			WriteTxt("��ȡ����hv_OperaCir_Radiusʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_SelectAreaMaxʧ��");
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
			WriteTxt("��ȡ����hv_SelectAreaMaxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_BlobCountʧ��");
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
			WriteTxt("��ȡ����hv_BlobCountʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin�����hv_Tolerance_Xʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_X = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin�����hv_Tolerance_Yʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_Y = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_A", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin�����hv_Tolerance_Aʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_A = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Area", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin�����hv_Tolerance_Areaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_Area = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡPin�����hv_MmPixelʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		//��ȡģ��ƥ�����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MatchMethodʧ��");
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
			WriteTxt("��ȡ����hv_MatchMethodʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AngleStartʧ��");
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
			WriteTxt("��ȡ����hv_AngleStartʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AngleExtentʧ��");
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
			WriteTxt("��ȡ����hv_AngleExtentʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ScaleRMinʧ��");
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
			WriteTxt("��ȡ����hv_ScaleRMinʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ScaleRMaxʧ��");
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
			WriteTxt("��ȡ����hv_ScaleRMaxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ScaleCMinʧ��");
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
			WriteTxt("��ȡ����hv_ScaleCMinʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ScaleCMaxʧ��");
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
			WriteTxt("��ȡ����hv_ScaleCMaxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MinScoreʧ��");
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
			WriteTxt("��ȡ����hv_MinScoreʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_NumMatchesʧ��");
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
			WriteTxt("��ȡ����hv_NumMatchesʧ��");
			return nullKeyName_Xml;
		}

		//�����������
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

		//ѵ���������
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

		//��ȡģ��ƥ��ģ��
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
					//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
					HTuple emptyID = NULL;
					runParams.hv_ModelID.Append(emptyID);
				}
			}
			else
			{
				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
				HTuple emptyID = NULL;
				runParams.hv_ModelID.Append(emptyID);
			}
		}

		//��ȡλ������ģ��ƥ�����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MatchMethod_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AngleStart_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_AngleExtent_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_MinScore_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_NumMatches_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Check_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Row_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Column_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Angle_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_SortType_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_SortType_PositionCorrection.Clear();
			runParams.hv_SortType_PositionCorrection = HTuple(valueXml.c_str());
		}

		//λ�����������������
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

		//λ������ѵ���������
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
		//��ȡλ��������ģ��ƥ��ģ��
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
				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("PCB������hv_ModelID_PositionCorrection��ȡʧ��!");
		}


		WriteTxt("Pin��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������Pin��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����ColorSort������
int DataIO::WriteParams_ColorSortDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		int typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("��ֵ������Ϊ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("sigmaΪ�գ�����ʧ��");
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
			WriteTxt("�˲��˿��Ϊ�գ�����ʧ��");
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
			WriteTxt("�˲��˸߶�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ�ָ�ͼ��Աȶ�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ����ѡ��Ϊ�գ�����ʧ��");
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
			WriteTxt("hv_FillUpShape_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("hv_FillUpShape_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("��̬ѧ����Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ�뾶Ϊ�գ�����ʧ��");
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

		//****************************����ɸѡ****************************************************************
		int typeNum = 0;
		//д������ɸѡ����
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		typeNum = typeCount;
		if (typeCount == 0)
		{
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FeaturesFilter_Type", "null");
		}
		else
		{
			//д������ɸѡ����(�����Բ�ȵȵ�)
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

			//д��ɸѡ������Сֵ
			typeCount = runParams.hv_MinValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					//����11λ��Чλ��(����С��λ)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MinValue_Filter.TupleReal().D();
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", ss.str());
					//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", to_string(runParams.hv_MinValue_Filter.TupleReal().D()));
				}
				else
				{
					//����11λ��Чλ��(����С��λ)
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


			//д��ɸѡ�������ֵ
			typeCount = runParams.hv_MaxValue_Filter.TupleLength().TupleInt().I();
			if (typeCount > 0)
			{
				if (typeCount == 1)
				{
					//����11λ��Чλ��(����С��λ)
					std::stringstream ss;
					ss << std::setprecision(11) << runParams.hv_MaxValue_Filter.TupleReal().D();
					xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", ss.str());
					//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", to_string(runParams.hv_MaxValue_Filter.TupleReal().D()));
				}
				else
				{
					//����11λ��Чλ��(����С��λ)
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
			//д�����
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", runParams.hv_And_Or_Filter.S().Text());
		}

		//�����������
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
		//������������
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("��������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		WriteTxt("ColorSort����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ColorSort����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡColorSort������
ErrorCode_Xml DataIO::ReadParams_ColorSortDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
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
			WriteTxt("XML��ȡ����hv_ThresholdTypeʧ��");
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
			WriteTxt("��ȡ����hv_ThresholdTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_LowThresholdʧ��");
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
			WriteTxt("��ȡ����hv_LowThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HighThresholdʧ��");
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
			WriteTxt("��ȡ����hv_HighThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Sigmaʧ��");
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
			WriteTxt("��ȡ����hv_Sigmaʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreWidthʧ��");
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
			WriteTxt("��ȡ����hv_CoreWidthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreHeightʧ��");
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
			WriteTxt("��ȡ����hv_CoreHeightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdContrastʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdContrastʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdPolarityʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdPolarityʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Minʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Maxʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperatorTypeʧ��");
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
			WriteTxt("��ȡ����hv_OperatorTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Widthʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Widthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Heightʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Heightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaCir_Radiusʧ��");
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
			WriteTxt("��ȡ����hv_OperaCir_Radiusʧ��");
			return nullKeyName_Xml;
		}

		//��ȡ����ɸѡ����
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
					//��ȡɸѡ������Сֵ
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinValue_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_MinValue_Filterʧ��");
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

					//��ȡɸѡ�������ֵ
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxValue_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_MaxValue_Filterʧ��");
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

					//��ȡ���
					errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_And_Or_Filter", valueXml);
					if (errorCode != Ok_Xml)
					{
						WriteTxt("XML��ȡ��̬ѧ������hv_And_Or_Filterʧ��");
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
		//��ȡ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue1_Minʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue1_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue1_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue1_Maxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue1_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue2_Minʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue2_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue2_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue2_Maxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue2_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue3_Minʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue3_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorValue3_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorValue3_Maxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorValue3_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_Product", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Channel_Productʧ��");
			return errorCode;
		}
		runParams.hv_Channel_Product = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_ColorSort", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Channel_ColorSortʧ��");
			return errorCode;
		}
		runParams.hv_Channel_ColorSort = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorArea_Minʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorArea_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_ColorArea_Maxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_ColorArea_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_UseBlob", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_UseBlobʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_UseBlob = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_UseUnionSortRegion", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡhv_UseUnionSortRegionʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_UseUnionSortRegion = tempIntValue;

		//λ������ѵ���������
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

		WriteTxt("ColorSort��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ColorSort��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����ColorExtract������
int DataIO::WriteParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����λ������ģ��ƥ�����
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
		//����λ������ģ��ƥ��ģ��
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
			WriteTxt("λ������ģ��ƥ��ģ��IDΪ��");
		}
		//����λ����������������
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ��������������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//����λ��������ѵ������
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ������ѵ������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}

		//������ɫ��ȡ����
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
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Min", ss.str());
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColorArea_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColorArea_Max", ss.str());
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
		//������������
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
//��ȡColorExtract������
ErrorCode_Xml DataIO::ReadParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		vector<string> typeArray;
		int typeCount = 0;

		//��ȡλ������ģ��ƥ�����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MatchMethod_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AngleStart_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_AngleExtent_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_MinScore_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_NumMatches_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Check_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Row_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Column_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Angle_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		//λ�����������������
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

		//λ������ѵ���������
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
		//��ȡλ��������ģ��ƥ��ģ��
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
				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("������hv_ModelID_PositionCorrection��ȡʧ��!");
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Channel_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Channel_PositionCorrectionʧ��");
			return errorCode;
		}
		runParams.hv_Channel_PositionCorrection = HTuple(valueXml.c_str());

		//��ȡ��ɫ��ȡ����
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

		//�����������
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

//����MachineLearning������
int DataIO::WriteParams_MachineLearning(const string ConfigPath, const string XmlPath, const RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//д������(���鰴�ն��ŷָ�洢)
		int typeCount = runParams.hv_ImgFilePathArray.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("hv_ImgFilePathArrayΪ�գ�����ʧ��");
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
			WriteTxt("hv_ClassNameArrayΪ�գ�����ʧ��");
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
			WriteTxt("hv_ClassIDArrayΪ�գ�����ʧ��");
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

		//����ָ��NG��ID
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NGID", to_string(runParams.hv_NGID.TupleInt().I()));
		//����ϲ����еļ������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUnionDetectROI", to_string(runParams.hv_IsUnionDetectROI.TupleInt().I()));

		//����Сͼ���ROI����
		if (runParams.ho_DetectROIArray.size() == 0)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ho_DetectROIArrayΪ�գ�����ʧ��");
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
		//����������(�ֻ�ROI����Blob)
		typeCount = runParams.ho_DetectROIType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("�������Ϊ�գ�����ʧ��");
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

		//����Blob����
		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0)
		{
			WriteTxt("����ʽΪ�գ�����ʧ��");
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
			WriteTxt("��ֵ������Ϊ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("����ֵΪ�գ�����ʧ��");
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
			WriteTxt("sigmaΪ�գ�����ʧ��");
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
			WriteTxt("�˲��˿��Ϊ�գ�����ʧ��");
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
			WriteTxt("�˲��˸߶�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ�ָ�ͼ��Աȶ�Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬��ֵ����ѡ��Ϊ�գ�����ʧ��");
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
			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ�볤MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_CircularityFilter_MinΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_RectangularityFilter_MinΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_WidthFilter_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_HeightFilter_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ�볤MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ���MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡ������MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_CircularityFilter_MaxΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_RectangularityFilter_MaxΪ�գ�����ʧ��");
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
			WriteTxt("ɸѡhv_WidthFilter_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("ɸѡhv_HeightFilter_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("hv_FillUpShape_MinΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("hv_FillUpShape_MaxΪ�գ�����ʧ��");
			return 1;
		}
		if (typeCount == 1)
		{
			//����11λ��Чλ��(����С��λ)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else
		{
			//����11λ��Чλ��(����С��λ)
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
			WriteTxt("��̬ѧ����Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ��Ϊ�գ�����ʧ��");
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
			WriteTxt("��̬ѧ�뾶Ϊ�գ�����ʧ��");
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
			WriteTxt("hv_SelectAreaMaxΪ�գ�����ʧ��");
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

		//����MLPѵ�����в���
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

		//����MLPģ��
		HTuple ClassifyIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
		WriteClassMlp(runParams.hv_ClassifyHandle, ClassifyIDPath);

		//����λ������ģ��ƥ�����
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
		//����λ������ģ��ƥ��ģ��
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
			WriteTxt("λ������ģ��ƥ��ģ��IDΪ��");
		}
		//����λ����������������
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ��������������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//����λ��������ѵ������
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("λ������ѵ������Ϊ�գ�����ʧ��");
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
		WriteTxt("XML������������Pin�����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡMachineLearning������
ErrorCode_Xml DataIO::ReadParams_MachineLearning(const string ConfigPath, const string XmlPath, RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//��ȡ����(���鰴�ն��ŷָ�洢)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ImgFilePathArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ������hv_ImgFilePathArrayʧ��");
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
			WriteTxt("��ȡ����hv_ImgFilePathArrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassNameArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML������hv_ClassNameArrayʧ��");
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
			WriteTxt("��ȡ����hv_ClassNameArrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ClassIDArray", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML����hv_ClassIDArrayʧ��");
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
			WriteTxt("ȡ����hv_ClassIDArrayʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NGID", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML����hv_NGIDʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NGID = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUnionDetectROI", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML����hv_IsUnionDetectROIʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsUnionDetectROI = tempIntValue;

		//Сͼ���ROI�������
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

		//MLP����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Iterations", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Iterationsʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Iterations = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumHidden", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_NumHiddenʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_NumHidden = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WeightTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WeightToleranceʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_WeightTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ErrorToleranceʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ErrorTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Rejection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Rejectionʧ��");
			return errorCode;
		}
		runParams.hv_Rejection = HTuple(valueXml.c_str());

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorData", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ErrorDataʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_ErrorData = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsAddNG", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_IsAddNGʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsAddNG = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsZoomSize", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_IsZoomSizeʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_IsZoomSize = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Zoom_Widthʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Zoom_Width = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Zoom_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Zoom_Heightʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runParams.hv_Zoom_Height = tempIntValue;

		//��ȡMLPģ��
		HTuple ClassifyIdPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_ClassifyId.gmc";
		ReadClassMlp(ClassifyIdPath, &runParams.hv_ClassifyHandle);

		//��ȡ����������(�ֻ�ROI����Blob)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ho_DetectROIType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����ho_DetectROITypeʧ��");
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
			WriteTxt("��ȡ����ho_DetectROITypeʧ��");
			return nullKeyName_Xml;
		}

		//��ȡBlob����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_SortTypeʧ��");
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
			WriteTxt("��ȡ����hv_SortTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ThresholdTypeʧ��");
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
			WriteTxt("��ȡ����hv_ThresholdTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_LowThresholdʧ��");
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
			WriteTxt("��ȡ����hv_LowThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HighThresholdʧ��");
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
			WriteTxt("��ȡ����hv_HighThresholdʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_Sigmaʧ��");
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
			WriteTxt("��ȡ����hv_Sigmaʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreWidthʧ��");
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
			WriteTxt("��ȡ����hv_CoreWidthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CoreHeightʧ��");
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
			WriteTxt("��ȡ����hv_CoreHeightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdContrastʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdContrastʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_DynThresholdPolarityʧ��");
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
			WriteTxt("��ȡ����hv_DynThresholdPolarityʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AreaFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_AreaFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen1Filter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RecLen1Filter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen2Filter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RecLen2Filter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RowFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RowFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ColumnFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_ColumnFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CircularityFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_CircularityFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RectangularityFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_RectangularityFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WidthFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_WidthFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HeightFilter_Minʧ��");
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
			WriteTxt("��ȡ����hv_HeightFilter_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AreaFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_AreaFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen1Filter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RecLen1Filter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RecLen2Filter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RecLen2Filter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RowFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RowFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_ColumnFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_ColumnFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CircularityFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_CircularityFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_RectangularityFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_RectangularityFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WidthFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_WidthFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HeightFilter_Maxʧ��");
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
			WriteTxt("��ȡ����hv_HeightFilter_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Minʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Minʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_FillUpShape_Maxʧ��");
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
			WriteTxt("��ȡ����hv_FillUpShape_Maxʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperatorTypeʧ��");
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
			WriteTxt("��ȡ����hv_OperatorTypeʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Widthʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Widthʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaRec_Heightʧ��");
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
			WriteTxt("��ȡ����hv_OperaRec_Heightʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OperaCir_Radiusʧ��");
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
			WriteTxt("��ȡ����hv_OperaCir_Radiusʧ��");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_SelectAreaMaxʧ��");
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
			WriteTxt("��ȡ����hv_SelectAreaMaxʧ��");
			return nullKeyName_Xml;
		}

		//��ȡλ������ģ��ƥ�����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MatchMethod_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_AngleStart_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_AngleExtent_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleRMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMin_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_ScaleCMax_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_MinScore_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_NumMatches_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Check_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Row_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Column_PositionCorrectionʧ��");
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
			WriteTxt("XML��ȡ����hv_Angle_PositionCorrectionʧ��");
		}
		else
		{
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		//λ�����������������
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

		//λ������ѵ���������
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
		//��ȡλ��������ģ��ƥ��ģ��
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
				//����û�д����ͣ�Ҳ��Ҫ��ӿվ������֤���鳤��һ��
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...)
		{
			WriteTxt("PCB������hv_ModelID_PositionCorrection��ȡʧ��!");
		}



		WriteTxt("MachineLearing��������ȡ�ɹ�!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������MachineLearing��������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//�����ַ�������(OK����0��NG����1)
int DataIO::WriteParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��״ģ���������
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
		//����ģ�Ͳ�������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorType", RunParams.hv_ErrorType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LigntDirtContrast", to_string(RunParams.hv_LightDirtContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LigntVarRadio", to_string(RunParams.hv_LightVarRadio));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DarkDirtContrast", to_string(RunParams.hv_DarkDirtContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DarkVarRadio", to_string(RunParams.hv_DarkVarRadio));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea", to_string(RunParams.hv_MinArea));
		//����11λ��Чλ��(����С��λ)
		std::stringstream ss;
		ss << std::setprecision(11) << RunParams.hv_MaxArea;
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea", ss.str());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_EdgeTolerance", to_string(RunParams.hv_EdgeTolerance));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "OkImgFilePath", RunParams.OkImgFilePath);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ModelNum", to_string(RunParams.hv_ModelNum));
		//ͼ�������
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", RunParams.hv_FilterType);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(RunParams.hv_Sigma));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralContrast", to_string(RunParams.hv_BilateralContrast));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", to_string(RunParams.hv_MaskWidth));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "MaskHeight", to_string(RunParams.MaskHeight));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseFilter", to_string(RunParams.hv_IsUseFilter));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseNormalize", to_string(RunParams.hv_IsUseNormalize));

		//����ģ��ԭʼ����
		int typeCount = RunParams.Ori_RowRefs.size();
		if (typeCount == 0)
		{
			WriteTxt("Ori_RowRefsΪ�ձ���ʧ��");
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
			WriteTxt("Ori_ColRefsΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Widths_RecΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Heights_RecΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Row1s_RecΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Col1s_RecΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Row2s_RecΪ�ձ���ʧ��");
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
			WriteTxt("Ori_Col2s_RecΪ�ձ���ʧ��");
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
			WriteTxt("hv_ForwardGrayValueΪ�ձ���ʧ��");
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
			WriteTxt("hv_BackGroundGrayValueΪ�ձ���ʧ��");
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


		//������״ģ��ѵ������
		if (RunParams.TrainRegions.size() <= 0)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("��״ģ��ѵ������Ϊ�գ�����ʧ��");
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

		//�������ģ��ѵ������
		if (RunParams.VariationModelROIs.size() <= 0)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("����ģ��ѵ������Ϊ�գ�����ʧ��");
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

		//������������
		if (RunParams.ho_SearchRoi.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("ģ����������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRoi, PathRunRegion);
		}

		//������״ģ��ģ���ļ�
		if (RunParams.ShapeModel_ID.size() <= 0)
		{
			WriteTxt("��״ģ��Ϊ�գ�����ʧ��");
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
		//�������ģ��ģ���ļ�
		if (RunParams.VarModel_ID.size() <= 0)
		{
			WriteTxt("����ģ��Ϊ�գ�����ʧ��");
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
		//����ѵ����ģ��ͼ����ͼ
		HTuple saveModelImgPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CharacterModelImg.bmp";
		WriteImage(RunParams.ho_ModelImg, "bmp", 0, saveModelImgPath);
		//����ѵ��Сͼ
		RunParams.ModelCutImgPath.clear();
		for (int i = 0; i < RunParams.ModelCutImg.size(); i++)
		{
			HTuple savePath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_CharacterTrainImg.bmp";
			WriteImage(RunParams.ModelCutImg[i], "bmp", 0, savePath);
			RunParams.ModelCutImgPath.push_back(savePath.S().Text());
		}

		//����ѵ��Сͼ·��
		typeCount = RunParams.ModelCutImgPath.size();
		if (typeCount == 0)
		{
			WriteTxt("ModelCutImgPathΪ�ձ���ʧ��");
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

		WriteTxt("�ַ�����������ɹ�!");
		//mutex1.unlock();
		return 0;

	}
	catch (...)
	{
		WriteTxt("XML�������������ַ�ȱ�ݼ���������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//�����ַ�������(OK����0��NG����1)
ErrorCode_Xml DataIO::ReadParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��״ģ�����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumLevels", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumLevelsʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumLevels = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleStartʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleStart = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_AngleExtentʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_AngleExtent = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleRMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleRMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScaleCMaxʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScaleCMax = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Metric", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Metricʧ��");
			return errorCode;
		}
		RunParams.hv_Metric = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Contrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Contrastʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_Contrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ContrastMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ContrastMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_ContrastMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SizeMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_SizeMinʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_SizeMin = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScoreMin", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ScoreMinʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_ScoreMin = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_NumMatchesʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_NumMatches = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxOverlap", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MaxOverlapʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxOverlap = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SubPixel", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_SubPixelʧ��");
			return errorCode;
		}
		RunParams.hv_SubPixel = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Greediness", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Greedinessʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Greediness = tempDoubleValue;
		//��ȡ����ģ�Ͳ���
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ErrorType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ErrorTypeʧ��");
			return errorCode;
		}
		RunParams.hv_ErrorType = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LigntDirtContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_LigntDirtContrastʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_LightDirtContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LigntVarRadio", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_LigntVarRadioʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_LightVarRadio = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DarkDirtContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_DarkDirtContrastʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_DarkDirtContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DarkVarRadio", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_DarkVarRadioʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_DarkVarRadio = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinArea", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MinAreaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MinArea = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaxArea", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MaxAreaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_MaxArea = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_EdgeTolerance", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_EdgeToleranceʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_EdgeTolerance = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "OkImgFilePath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������OkImgFilePathʧ��");
			return errorCode;
		}
		RunParams.OkImgFilePath = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ModelNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_ModelNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_ModelNum = tempIntValue;

		//ͼ�������
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FilterType", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_FilterTypeʧ��");
			return errorCode;
		}
		RunParams.hv_FilterType = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_Sigmaʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_Sigma = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BilateralContrast", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_BilateralContrastʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_BilateralContrast = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MaskWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_MaskWidthʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_MaskWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "MaskHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������MaskHeightʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.MaskHeight = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseFilter", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_IsUseFilterʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseFilter = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsUseNormalize", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡģ�������hv_IsUseNormalizeʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsUseNormalize = tempIntValue;

		//��ȡģ��ԭʼ����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "Ori_RowRefs", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�ַ�������Ori_RowRefsʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_ColRefsʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Widths_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Heights_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Row1s_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Col1s_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Row2s_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������Ori_Col2s_Recʧ��");
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
			WriteTxt("XML��ȡ�ַ�������hv_ForwardGrayValueʧ��");
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
			WriteTxt("XML��ȡ�ַ�������hv_BackGroundGrayValueʧ��");
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

		//�����������
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
		//��״ģ��ѵ���������
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
		//����ģ��ѵ���������
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

		//��ȡ��״ģ��ģ���ļ�
		RunParams.ShapeModel_ID.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_ShapeModelId.shm";
			HTuple handleID;
			ReadShapeModel(modelIDPath, &handleID);
			RunParams.ShapeModel_ID.push_back(handleID);
		}

		//��ȡ����ģ��ģ���ļ�
		RunParams.VarModel_ID.clear();
		for (int i = 0; i < RunParams.hv_ModelNum; i++)
		{
			HTuple modelIDPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_" + HTuple(to_string(i).c_str()) + "_VarModelId.vam";
			HTuple handleID;
			ReadVariationModel(modelIDPath, &handleID);
			RunParams.VarModel_ID.push_back(handleID);
		}
		//��ȡѵ����ģ��ͼ����ͼ
		HTuple saveModelImgPath = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_CharacterModelImg.bmp";
		ReadImage(&RunParams.ho_ModelImg, saveModelImgPath);
		//��ȡѵ��Сͼ·��
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ModelCutImgPath", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ�ַ�������ModelCutImgPathʧ��");
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

		//��ȡѵ��Сͼ
		RunParams.ModelCutImg.clear();
		for (int i = 0; i < RunParams.ModelCutImgPath.size(); i++)
		{
			HTuple savePath = HTuple(RunParams.ModelCutImgPath[i].c_str());
			HObject img;
			ReadImage(&img, savePath);
			RunParams.ModelCutImg.push_back(img);
		}

		WriteTxt("�ַ�ȱ�ݼ��������سɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ����������ַ�ȱ�ݼ���������ʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����ͼ���ֲ���(OK����0��NG����1)
int DataIO::WriteParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
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

		WriteTxt("ͼ���ֲ�������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ͼ���ֲ�������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//����ͼ���ֲ���(OK����0��NG����1)
ErrorCode_Xml DataIO::ReadParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��ȡ����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Ori", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutRows_Oriʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutRows_Ori = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Ori", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutCols_Oriʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutCols_Ori = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OverlapRate", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_OverlapRateʧ��");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		RunParams.hv_OverlapRate = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutRows_Dest", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutRows_Destʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutRows_Dest = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutCols_Dest", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutCols_Destʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutCols_Dest = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthImg", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_WidthImgʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_WidthImg = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightImg", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_HeightImgʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_HeightImg = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutWidth", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutWidthʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutWidth = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutHeight", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutHeightʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutHeight = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_IsSplit", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_IsSplitʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_IsSplit = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CutNum", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_CutNumʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_CutNum = tempIntValue;


		WriteTxt("ͼ���ֲ������سɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼ���ֲ�������ʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//����Ҷȼ������(OK����0��NG����1)
int DataIO::WriteParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
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
		//������������
		if (RunParams.ho_SearchRoi.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			WriteTxt("������������Ϊ�գ�����ʧ��");
		}
		else
		{
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + HTuple(to_string(processId).c_str()) + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(RunParams.ho_SearchRoi, PathRunRegion);
		}
		WriteTxt("ͼ��Ҷȼ����������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ͼ��Ҷȼ����������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//���ػҶȼ������(OK����0��NG����1)
ErrorCode_Xml DataIO::ReadParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;
		//��ȡ����
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeanGrayValue_Standard", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_MeanGrayValue_Standardʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_MeanGrayValue_Standard = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Min", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_GrayValueOffset_Minʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_GrayValueOffset_Min = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_GrayValueOffset_Max", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ����hv_GrayValueOffset_Maxʧ��");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		RunParams.hv_GrayValueOffset_Max = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "isFollow", valueXml);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡֱ�߼�����isFollowʧ��");
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

		//�����������
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

		WriteTxt("ͼ��Ҷȼ���������سɹ�!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ͼ��Ҷȼ����������ʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

//��ȡ����ʹ�õ�ģ���ֶ�
ErrorCode_Xml DataIO::ReadXML_Process_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_Process(XmlPath, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡ���̲���ʧ��");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ������������̲�����ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//����ȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
int DataIO::WriteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXML_FieldValueAndGlobalName(XmlPath, processId, nodeName, processModbuleID, keyName);

		WriteTxt("ȫ�ֲ�������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ȫ�ֲ�������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//����ÿ�����̣�ÿ��ģ����Ҫ�õ�ȫ�ֱ������ֶ�����(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
int DataIO::WriteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//��������
		xmlC.InsertXML_FieldValue(XmlPath, processId, nodeName, processModbuleID, keyName);

		WriteTxt("ȫ�ֲ�������ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XML������������ȫ�ֲ�������ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//��ȡȫ�ֱ������ֶ�����
ErrorCode_Xml DataIO::ReadXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_FieldValue(XmlPath, processId, nodeName, processModbuleID, fieldValue);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡȫ�ֱ�������ʧ��");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ȫ�ֱ���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//��ȡȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������
ErrorCode_Xml DataIO::ReadXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXML_FieldValueAndGlobalName(XmlPath, processId, nodeName, processModbuleID, fieldValue);
		if (errorCode != Ok_Xml)
		{
			WriteTxt("XML��ȡȫ�ֱ�������ʧ��");
			return errorCode;
		}

		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...)
	{
		WriteTxt("XML��ȡ���������ȫ�ֱ���������ȡʧ��!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}
//ɾ��ȫ�ֱ������ֶ�����
int DataIO::DeleteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����ɾ��
		xmlC.DeleteXML_FieldValue(XmlPath);

		WriteTxt("ȫ�ֲ���ɾ���ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XMLɾ�����������ȫ�ֲ���ɾ��ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}
//ɾ��ȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������
int DataIO::DeleteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath)
{
	//mutex1.lock();
	try
	{
		XmlClass2 xmlC;//xml��ȡ����
		//·���ж�
		PathSet(ConfigPath, XmlPath);
		//����ɾ��
		xmlC.DeleteXML_FieldValueAndGlobalName(XmlPath);

		WriteTxt("ȫ�ֲ���ɾ���ɹ�!");
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		WriteTxt("XMLɾ�����������ȫ�ֲ���ɾ��ʧ��!");
		//mutex1.unlock();
		return -1;
	}
}


//д��־����
int DataIO::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�
	in << p << "\r";
	in.close();//�ر��ļ�
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

//����һ������strOld
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
//����ָ���ַ��ָ��ַ�����vector
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
