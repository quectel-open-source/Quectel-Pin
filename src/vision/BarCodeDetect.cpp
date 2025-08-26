#include "BarCodeDetect.h"
BarCodeDetect::BarCodeDetect()
{
	////ͨ����ָ�����ʵ����
	//m_Common = new Common();
}

BarCodeDetect::~BarCodeDetect()
{
	////�ͷ�ͨ����ָ����Դ
	//delete m_Common;
}

//ʶ������
int BarCodeDetect::FindBarCodeFunc(const HObject &ho_Image, const RunParamsStruct_BarCode &runParams, ResultParamsStruct_BarCode &resultParams)
{
	try
	{
		//��ս��
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
			//ͼ��Ϊ�գ�����ͼ��clear
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}
		//�ȼ����,ÿ��ֵ����һ�����
		HTuple hv_Overall_Quality = 0;	//ɨ�跴��������ȼ�,ÿ��ɨ���ߵ�ɨ�跴���������ȼ�������ƽ��ֵ
		HTuple hv_GRADE_DECODE = 1;	    //�����ŵķ����ʷֲ����Ը��ݷ�����ϵ�Ĳο������㷨���н���ʱ������Ϊ4����������Ϊ0
		HTuple hv_GRADE_CONTRAST = 2;	//�����ʷֲ�����Сֵ�����ֵ֮��ķ�Χ��ǿ�ҵĶԱȻ�����õ����֡�
		HTuple hv_GRADE_MINIMAL_REFLECTANCE = 3;	//���ɨ�跴���ʷֲ��е���ͷ�����ֵ���ڻ�����������ֵ��0.5��������Ϊ4�����򣬽�����ֵ0��
		HTuple hv_GRADE_MINIMAL_EDGE_CONTRAST = 4;	//�κ���������Ԫ��֮��ĶԱȣ������������ռ仹�ǿռ䵽������С��Ե�ԱȶȶԷ����ʷֲ��в����ı�Ե�Աȶ�ֵ����Сֵ���зּ���
		HTuple hv_GRADE_MODULATION = 5;		//ָʾ������Ԫ�ص�����ж�ǿ�������ʹԪ�ص�����ռ�ķ������ȷ�����Ӷ������ߵ��Ƶȼ���
		HTuple hv_GRADE_DEFECTS = 6;		//��ӳԪ�غͰ������ڻҶ�ֵ�ֲ��Ĳ������ԡ�
		HTuple hv_GRADE_DECODABILITY = 7;		//��ӳԪ�ؿ����Ϊ��Ӧ���Ŷ���ı�ƿ�ȵ�ƫ�
		HTuple hv_GRADE_ADDITIONAL_REQS = 8;	//��������������ض�Ҫ��ĵȼ�������Ҫ�漰����İ�����������ʱҲ�������/խ�ȡ��ַ����϶������ģʽ���йء�


		HTuple hv_BarCodeHandle; //����ʶ����
		HObject mho_Image;
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//��������Ϊ�գ����߱�clear(��������Ϊ�գ�Ĭ��ȫͼ����)
			mho_Image = ho_Image;
		}
		else
		{
			HObject RegionAffine;
			GenEmptyObj(&RegionAffine);
			//�Ƿ���з���任���������
			if (runParams.isFollow)
			{
				if (runParams.hv_HomMat2D == NULL)
				{
					mResultParams = resultParams;
					return 1;
				}
				//���ô���ľ�����з���任
				AffineTransRegion(runParams.ho_SearchRegion, &RegionAffine, runParams.hv_HomMat2D, "nearest_neighbor");
			}
			else
			{
				RegionAffine = runParams.ho_SearchRegion;
			}
			ReduceDomain(ho_Image, RegionAffine, &mho_Image);
		}

		//����������ģ�;����
		CreateBarCodeModel(HTuple(), HTuple(), &hv_BarCodeHandle);
		if (runParams.hv_Tolerance.TupleEqual(HTuple("low")) == 1)
		{
			//����Ҫ���ҵ��������С��Ȳ�����Ҳ���Ǻ�ɫ�Ͱ�ɫ���� ������ɨ�跽�����С���
			SetBarCodeParam(hv_BarCodeHandle, "element_size_min", 1.5);
			//����Ҫ��������Ľ�����̵��м����Ƿ񱣴棬1�ǣ�0����
			SetBarCodeParam(hv_BarCodeHandle, "persistence", 1);
			//��������ʹ������ɨ���߽��м��㣬�����ͬ����Ϊ�����ȷ
			SetBarCodeParam(hv_BarCodeHandle, "min_identical_scanlines", 2);
		}
		////����Ҫ���ҵ��������С��Ȳ�����Ҳ���Ǻ�ɫ�Ͱ�ɫ���� ������ɨ�跽�����С���
		//SetBarCodeParam(hv_BarCodeHandle, "element_size_min", 1.5);
		////����Ҫ��������Ľ�����̵��м����Ƿ񱣴棬1�ǣ�0����
		//SetBarCodeParam(hv_BarCodeHandle, "persistence", 1);
		////��������ʹ������ɨ���߽��м��㣬�����ͬ����Ϊ�����ȷ
		//SetBarCodeParam(hv_BarCodeHandle, "min_identical_scanlines", 2);
		//����ɨ�����(����Ϊ0����ɨ����������)
		SetBarCodeParam(hv_BarCodeHandle, "stop_after_result_num", runParams.hv_CodeNum.TupleInt());
		//�ֱ���ɨ���ߵĿ�ʼ��ֹͣģʽʱ��ǿ��ִ�п��ݣ����ߡ������ϸ񣨡��͡�����������������ڴ����ͼ���г��������ķ��ţ�
		//������ݵı�׼ͨ��������������ļ����ᡣ��һ���棬�����ܵ���������ͼ�������������ŵ�ͼ���н��д����⡣���̶Ƚϵ͵ı�׼����˶Դ������³���ԣ�
		//�����ܻή���������ʡ�Ŀǰ�������������Code 128��GS1 - 128������
		SetBarCodeParam(hv_BarCodeHandle, "start_stop_tolerance", runParams.hv_Tolerance);
		//����ɨ�볬ʱʱ��
		SetBarCodeParam(hv_BarCodeHandle, "timeout", runParams.hv_TimeOut.TupleInt());

		//��ͼƬ�п�ʼ�������룬�ַ����������DecodedDataStrings���������򱣴���SymbolRegions
		//'auto'�Զ�������������
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
				//��������
				GetBarCodeResult(hv_BarCodeHandle, i, "decoded_types", &hv_CodeTypeTemp);

				//��������ȼ�
				HTuple hv_QualityLabels, hv_Quality, hv_QualityValues;
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_labels", &hv_QualityLabels);
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_values", &hv_QualityValues);
				//GetBarCodeResult(hv_BarCodeHandle, i, "quality_isoiec15416_float_grades", &hv_Quality);
				hv_Quality = 4;
				
				//���˴�������
				//���������ַ�����
				HTuple CodeLengthTemp;//�������볤����ʱ����
				TupleStrlen(hv_CodeContentTemp[i], &CodeLengthTemp);
				if (CodeLengthTemp.TupleInt().I() < runParams.hv_CodeLength_Run.TupleInt())
				{
					//���볤�Ȳ����㣬�˳���ǰѭ��
					continue;
				}
				if (runParams.hv_ContainChar != "" && runParams.hv_ContainChar != "null")
				{
					//�����ַ����Ƿ�����ض��ַ�
					HTuple hv_Position;
					TupleStrchr(hv_CodeContentTemp[i], runParams.hv_ContainChar, &hv_Position);
					if (hv_Position.TupleInt().I() == -1)
					{
						//���������˳���ǰѭ��
						continue;
					}
				}
				if (runParams.hv_NotContainChar != "" && runParams.hv_NotContainChar != "null")
				{
					//�����ַ����Ƿ񲻰����ض��ַ�
					HTuple hv_Position;
					TupleStrchr(hv_CodeContentTemp[i], runParams.hv_NotContainChar, &hv_Position);
					if (hv_Position.TupleInt().I() != -1)
					{
						//�����˲�Ӧ�ð������ַ����˳���ǰѭ��
						continue;
					}
				}
				if (runParams.hv_HeadChar != "" && runParams.hv_HeadChar != "null")
				{
					//�����ַ����Ƿ��ض��ַ���ͷ
					//��ȡ�ַ����ĵ�һ���ַ�
					HTuple hv_Substring;
					TupleStrFirstN(hv_CodeContentTemp[i], 0, &hv_Substring);
					HTuple hv_Equal;
					TupleEqual(hv_Substring, runParams.hv_HeadChar, &hv_Equal);
					if (hv_Equal.TupleInt().I() == 0)
					{
						//û�����ض��ַ���ͷ���˳���ǰѭ��
						continue;
					}
				}
				if (runParams.hv_EndChar != "" && runParams.hv_EndChar != "null")
				{
					//�����ַ����Ƿ��ض��ַ���β
					//��ȡ�ַ��������һ���ַ�
					HTuple hv_Substring;
					TupleStrLastN(hv_CodeContentTemp[i], CodeLengthTemp.TupleInt().I() - 1, &hv_Substring);
					HTuple hv_Equal;
					TupleEqual(hv_Substring, runParams.hv_EndChar, &hv_Equal);
					if (hv_Equal.TupleInt().I() == 0)
					{
						//û�����ض��ַ���β���˳���ǰѭ��
						continue;
					}
				}


				//��ȡ���������м�������������
				ho_CodeRegions.Clear();
				GetBarCodeObject(&ho_CodeRegions, hv_BarCodeHandle, HTuple(hv_CodeIds[i]),
					"candidate_regions");
				HObject RegionTrans;
				ShapeTrans(ho_CodeRegions, &RegionTrans, "rectangle2");
				ConcatObj(resultParams.ho_RegionBarCode, RegionTrans, &resultParams.ho_RegionBarCode);
				//��ȡ���������м�����ɨ����
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
			resultParams.hv_RetNum = RetNumTemp;//�ҵ����������
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
		//��ս��
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

//д��־����
int BarCodeDetect::WriteTxt(string content)
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

//��ȡϵͳʱ��
vector<string> BarCodeDetect::get_current_time()
{
	/*�ܾ�ȷ������*/
	/*
	#include <windows.h>
	#include <iostream>
	*/
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //��ȡ��ǰʱ�� �ɾ�ȷ��ms

	char tmpbuff[16];
	/*��*/
	sprintf(tmpbuff, "%d", st.wYear);
	std::string year = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wMonth);
	std::string month = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wDay);
	std::string day = tmpbuff;
	/*ʱ*/
	sprintf(tmpbuff, "%d", st.wHour);
	std::string hour = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wMinute);
	std::string minute = tmpbuff;
	/*��*/
	sprintf(tmpbuff, "%d", st.wSecond);
	std::string second = tmpbuff;
	/*����*/
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
	//XML��ȡ����
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
	modubleResultTitleList.push_back("��������");
	modubleResultTitleList.push_back("��������");
	modubleResultTitleList.push_back("���볤��");
	modubleResultTitleList.push_back("X����");
	modubleResultTitleList.push_back("Y����");
	modubleResultTitleList.push_back("�����ȼ�");

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

