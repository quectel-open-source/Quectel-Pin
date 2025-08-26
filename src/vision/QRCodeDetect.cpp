#include "QRCodeDetect.h"
QRCodeDetect::QRCodeDetect()
{
	////ͨ����ָ�����ʵ����
	//m_Common = new Common();
}

QRCodeDetect::~QRCodeDetect()
{
	////�ͷ�ͨ����ָ����Դ
	//delete m_Common;
}

//ʶ���ά��
int QRCodeDetect::FindCode2dFunc(const HObject &ho_Image, const RunParamsStruct_Code2d &runParams, ResultParamsStruct_Code2d &resultParams)
{
	//mutex1.lock();
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
		GenEmptyObj(&resultParams.ho_ContourCode2d);
		GenEmptyObj(&resultParams.ho_RegionCode2d);

		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}

		CodeTypeStruct_Code2d Code2dTypeStruct;
		HObject mho_Image;
		//�ȼ����,ÿ��ֵ����һ�����
		//��ά���ӡ������һ������ʮ����Ԫ�ص�Ԫ����������
		//�������������Աȶȡ����ơ��̶�ͼ���𻵡����롢���򲻾����ԡ����񲻾����ԣ�δʹ�õľ�������ԣ�ȡ���ӡ�������ԱȶȾ����ԡ��׾�����
		HTuple hv_GRADE_Overall_Quality = 0;	//ɨ�跴��������ȼ�,ÿ��ɨ���ߵ�ɨ�跴���������ȼ�������ƽ��ֵ
		HTuple hv_GRADE_Contrast = 1;	//�����ʷֲ�����Сֵ�����ֵ֮��ķ�Χ��ǿ�ҵĶԱȻ�����õ����֡�
		HTuple hv_GRADE_Modulation = 2;		//ָʾ������Ԫ�ص�����ж�ǿ�������ʹԪ�ص�����ռ�ķ������ȷ�����Ӷ������ߵ��Ƶȼ���
		HTuple hv_GRADE_Fixed_Pattern_Damage = 3;
		HTuple hv_GRADE_Decode = 4;	    //�����ŵķ����ʷֲ����Ը��ݷ�����ϵ�Ĳο������㷨���н���ʱ������Ϊ4����������Ϊ0
		HTuple hv_GRADE_Axial_Nonuniformity = 5;
		HTuple hv_GRADE_Grid_Nonuniformity = 6;
		HTuple hv_GRADE_Unused_Error_Correction = 7;
		HTuple hv_GRADE_Reflectance_Margin = 8;
		HTuple hv_GRADE_Print_Growth = 9;
		HTuple hv_GRADE_Contrast_Uniformity = 10;
		HTuple hv_GRADE_Aperture = 11;

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

		//��Ҫʶ��Ķ�ά����������
		int Code2dTypeNum = runParams.hv_CodeType_Run.TupleLength().TupleInt().I();
		for (int j = 0; j < Code2dTypeNum; j++)
		{
			HTuple hv_DataCodeHandle;
			HTuple hv_ResultHandles;
			//������ά��ģ��
			CreateDataCode2dModel(runParams.hv_CodeType_Run[j], HTuple(), HTuple(), &hv_DataCodeHandle);
			//���ò������Ա�洢�м���
			SetDataCode2dParam(hv_DataCodeHandle, "persistence", 1);
			//������ǿ�Ͷ���ģ��(��standard_rerecognition������enhanced_egnition������maximum_recognity��������ģ�Ͳ�������Ϊ��׼����ǿ��������ֻ���Ĭ������֮һ)
			//ע�⣺����˲��������������б�һ�����ã���˲�������λ�ڵ�һ��λ��
			SetDataCode2dParam(hv_DataCodeHandle, "default_parameters", "enhanced_recognition");
			//���볬ʱʱ��ms
			SetDataCode2dParam(hv_DataCodeHandle, "timeout", runParams.hv_TimeOut.TupleInt());
			//******************���²��������������͵��붼�������õģ���ʱ����**********************************************************************************
			if (runParams.hv_CodeType_Run[j] == Code2dTypeStruct.DM || runParams.hv_CodeType_Run[j] == Code2dTypeStruct.Aztec)
			{
				if (runParams.hv_CodeType_Run[j] == Code2dTypeStruct.DM)
				{
					//��ͼ�����ڵ��������ǣ�ʶ������('low', 'high', 'any'),high�ᵼ��ʱ��������ʶ��������
					SetDataCode2dParam(hv_DataCodeHandle, "finder_pattern_tolerance", "any");
					//���ھֲ���������£�ɨ���㷨('low'�ٶȸ��죬³���Բ�ǿ, 'high'�ٶ�����³����ǿ, 'any')
					SetDataCode2dParam(hv_DataCodeHandle, "contrast_tolerance", "any");
					//�������ڷ��ż��ĺ�ѡ�����ѡ�� ���˲�������Ϊ'extensive'���������ɵĺ�ѡ������������Ӷ����Ӽ�⵽����Ŀ����ԡ�
					//���ǣ���ѡ�ߵ�����Խ�࣬����ʱ��ʱ���Խ���� ���'candidate_selection'������Ϊ'default'�����ʹ�ø��ٵĺ�ѡ���� �ڴ��������£������С�ĺ�ѡ���Ͼ��㹻�ˡ�
					SetDataCode2dParam(hv_DataCodeHandle, "candidate_selection", "extensive");
				}
				else
				{
					//��ͼ�����ڵ��������ǣ�ʶ������('low', 'high', 'any'),high�ᵼ��ʱ��������ʶ��������
					SetDataCode2dParam(hv_DataCodeHandle, "finder_pattern_tolerance", runParams.hv_Tolerance);
				}
			}
			//��ά�뱳��֮�伫�ԣ�light_on_dark��dark_on_light��any
			SetDataCode2dParam(hv_DataCodeHandle, "polarity", "any");

			//��ά��ͼ����ģ�����С��С,����С(������Ϊ��λ)
			//SetDataCode2dParam(hv_DataCodeHandle, (HTuple("module_size_min").Append("module_size_max")),(HTuple(6).Append(20)));
			//��ά��ͼ����ģ��֮��ļ�϶('no', 'yes', 'any')
			//SetDataCode2dParam(hv_DataCodeHandle, "module_gap", "no");
			//���������Ƿ񱻾������ܱ�����('no', 'yes', 'any')
			SetDataCode2dParam(hv_DataCodeHandle, "mirrored", "any");

			//���зǳ�С��ģ���С������������³���ԡ���������small_moduless_robustness������Ϊ��high�������ӽ���ģ���С�ǳ�С�����ݴ���Ŀ����ԡ�
			//���⣬����������£���Ӧ��Ӧ�ص�����Сģ���С����ˡ�module_size_min���͡�module_width_min����PDF417��Ӧ�ֱ�����ΪԤ�ڵ���Сģ���С�Ϳ�ȡ�
			//����small_modules_robustness������Ϊ��high��������������find_data_code_2d���ڲ��ڴ�ʹ��������ˣ���Ĭ������£�
			//��small_modules_robustness��Ӧ����Ϊ��low����
			SetDataCode2dParam(hv_DataCodeHandle, "small_modules_robustness", "low");

			//����ģ��Ĵ�С�Ƿ���ܱ仯(���ض���Χ��)�� ���������������ͬ���㷨����������ģ�������λ�á� �����������Ϊ'fixed'�����ʹ��һ���Ⱦ�����
			//����ɱ�ģ���С('variable')������ֻ�������ģʽ�Ľ���һ����롣
			//���ˡ�any���������ַ�������һ����һ���ز��ԡ� ��ע�⣬���'finder_pattern_tolerance'����Ϊ'high'�� 'module_grid'��ֵ�ᱻ���ԡ� ����������£�������һ���Ⱦ�����
			//SetDataCode2dParam(hv_DataCodeHandle, "module_grid", "any");
			//**********************************************************************************************************************************************************
			//��ȡ��ά��,���ö�ȡ����
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

					//��������ȼ�
					HTuple hv_QualityLabels, hv_Quality, hv_QualityValues;
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415_labels", &hv_QualityLabels);
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415_values", &hv_QualityValues);
					//GetDataCode2dResults(hv_DataCodeHandle, hv_ResultHandles[i], "quality_isoiec15415", &hv_Quality);
					hv_Quality = 4;
					
					//��ȡ���������м�������������
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
				resultParams.hv_RetNum += RetNumTemp;//�ҵ����������
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
		//��ս��
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
//д��־����
int QRCodeDetect::WriteTxt(string content)
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
vector<string> QRCodeDetect::get_current_time()
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

int QRCodeDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML��ȡ����
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
	modubleResultTitleList.push_back("��ά������");
	modubleResultTitleList.push_back("��ά������");
	modubleResultTitleList.push_back("��ά�볤��");
	modubleResultTitleList.push_back("X����");
	modubleResultTitleList.push_back("Y����");
	modubleResultTitleList.push_back("�����ȼ�");

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

