#include "CircleDetect.h"

CircleDetect::CircleDetect()
{
	////ͨ����ָ�����ʵ����
	//m_Common = new Common();
}

CircleDetect::~CircleDetect()
{
	////�ͷ�ͨ����ָ����Դ
	//delete m_Common;
}

int CircleDetect::FindCircle(const HObject& ho_Image, const RunParamsStruct_Circle& RunParams, ResultParamsStruct_Circle& resultParams)
{
	//mutex1.lock();
	try
	{
		//����ս��
		resultParams.hv_CircleRowCenter.Clear(); resultParams.hv_CircleColumnCenter.Clear(); resultParams.hv_CircleRadius.Clear();
		resultParams.hv_PointsRow.Clear(); resultParams.hv_PointsColumn.Clear(); resultParams.hv_PointsNum.Clear(); resultParams.hv_DisToCircleArray.Clear();
		resultParams.hv_RetNum = 0; resultParams.hv_Roundness = 99999;
		GenEmptyObj(&resultParams.ho_Contour_Circle);
		GenEmptyObj(&resultParams.ho_Contour_Calipers);
		GenEmptyObj(&resultParams.ho_Contour_CrossPoints);

		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}


		HTuple hv_Index;            //ģ��id(����ͬһ��ģ��һ��ץ����Բ��hv_Index����[0,1])
		HTuple hv_MetrologyHandle;        //�������
		HTuple hv_Row3, hv_Column3, hv_Phi1, hv_Length11, hv_Length21, hv_Area3;
		//�����������
		CreateMetrologyModel(&hv_MetrologyHandle);
		//��ȡģ�ͳߴ�
		HTuple hv_Width, hv_Height;
		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		HTuple hv_Row_Affine, hv_Column_Affine;
		//�Ƿ���з���任��ץԲ����
		if (RunParams.isFollow)
		{
			if (RunParams.hv_HomMat2D == NULL)
			{
				mResultParams = resultParams;
				return 1;
			}
			//���ô���ľ�����з���任
			AffineTransPixel(RunParams.hv_HomMat2D, RunParams.hv_Row, RunParams.hv_Column, &hv_Row_Affine, &hv_Column_Affine);
		}
		else
		{
			hv_Row_Affine = RunParams.hv_Row;
			hv_Column_Affine = RunParams.hv_Column;
		}

		//��Ӳ�������
		SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
		AddMetrologyObjectCircleMeasure(hv_MetrologyHandle, hv_Row_Affine, hv_Column_Affine, RunParams.hv_Radius,
			RunParams.hv_MeasureLength1, RunParams.hv_MeasureLength2, RunParams.hv_MeasureSigma, RunParams.hv_MeasureThreshold.TupleInt(), "measure_transition", RunParams.hv_MeasureTransition, &hv_Index);
		//���ò�������Ĳ���
		//ģ�Ϳ��߳������߿�ƽ��ϵ�����Ҷ���ֵ�����ԣ������ģ��ʱ�Ѿ����ã����Բ����ٴ�����
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length1",
		//    hv_MeasureLength1);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length2",
		//   hv_MeasureLength2);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_sigma",
		//    hv_MeasureSigma);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_threshold", hv_MeasureThreshold);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_transition", hv_MeasureTransition);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_select", RunParams.hv_MeasureSelect);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_measures", RunParams.hv_MeasureNum.TupleInt());
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_instances", RunParams.hv_InstancesNum.TupleInt());
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "min_score", RunParams.hv_MeasureMinScore);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "distance_threshold", RunParams.hv_DistanceThreshold);
		//����ģ�Ͳ�ֵ�㷨(nearest_neighbor,bilinear,bicubic)
		//nearest_neighbor���������,�ٶ���죬Ч����Բ�
		//bilinear��˫���Բ�ֵ���ٶ��еȣ�Ч�����������ã���˫���β�ֵ��
		//bicubic��˫���β�ֵ���ٶ�������Ч�����
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_interpolation", RunParams.hv_MeasureInterpolation);

		//********************************************
		//ִ��ץԲģ��
		ApplyMetrologyModel(ho_Image, hv_MetrologyHandle);
		//��ȡץԲ���
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "row", &resultParams.hv_CircleRowCenter);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "column", &resultParams.hv_CircleColumnCenter);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "radius", &resultParams.hv_CircleRadius);

		//�ж��Ƿ��ҵ�Բ
		HTuple hv_Length;
		TupleLength(resultParams.hv_CircleRowCenter, &hv_Length);
		resultParams.hv_RetNum = hv_Length;
		if (hv_Length != 1)
		{
			//���ץԲģ��
			ClearMetrologyModel(hv_MetrologyHandle);
			//mutex1.unlock();
			mResultParams = resultParams;
			return 1;
		}
		//��ȡ�����ҵ������е�����
		GetMetrologyObjectMeasures(&resultParams.ho_Contour_Calipers, hv_MetrologyHandle, "all", "all", &resultParams.hv_PointsRow,
			&resultParams.hv_PointsColumn);
		TupleLength(resultParams.hv_PointsRow, &resultParams.hv_PointsNum);
		//���е��XLD����
		GenCrossContourXld(&resultParams.ho_Contour_CrossPoints, resultParams.hv_PointsRow, resultParams.hv_PointsColumn, 12, HTuple(45).TupleRad());


		//��ȡԲ����
		GetMetrologyObjectResultContour(&resultParams.ho_Contour_Circle, hv_MetrologyHandle, 0, "all", 1.5);
		//���ץԲģ��
		ClearMetrologyModel(hv_MetrologyHandle);

		//���ߵ㵽���Բ��Բ�ļ�ȥ�뾶�ľ���
		for (int n = 0; n < resultParams.hv_PointsNum; n++)
		{
			HTuple disPixel;
			DistancePp(resultParams.hv_PointsRow[n], resultParams.hv_PointsColumn[n], resultParams.hv_CircleRowCenter, resultParams.hv_CircleColumnCenter, &disPixel);
			HTuple offsetValue1 = (disPixel - resultParams.hv_CircleRadius).TupleAbs();
			HTuple disMm = offsetValue1 * RunParams.hv_MmPixel;
			resultParams.hv_DisToCircleArray.Append(disMm);
		}
		//����Բ��
		HTuple sortDis;
		TupleSort(resultParams.hv_DisToCircleArray, &sortDis);
		resultParams.hv_Roundness = sortDis[sortDis.TupleLength() - 1];
		if (RunParams.hv_DectType == 1)
		{
			GenRegionPoints(&resultParams.ho_Contour_Circle, resultParams.hv_PointsRow, resultParams.hv_PointsColumn);
			SmallestRectangle2(resultParams.ho_Contour_Circle, &hv_Row3, &hv_Column3, &hv_Phi1, &hv_Length11, &hv_Length21);
			GenRectangle2(&resultParams.ho_Contour_Circle, hv_Row3, hv_Column3, hv_Phi1, hv_Length11, hv_Length21);
			AreaCenter(resultParams.ho_Contour_Circle, &hv_Area3, &resultParams.hv_CircleRowCenter, &resultParams.hv_CircleColumnCenter);
			resultParams.hv_CircleRadius = 0;
			resultParams.hv_Roundness = 0;
		}

		//mutex1.unlock();
		mResultParams = resultParams;
		return 0;
	}
	catch (...)
	{
		//����ս��
		resultParams.hv_CircleRowCenter.Clear(); resultParams.hv_CircleColumnCenter.Clear(); resultParams.hv_CircleRadius.Clear();
		resultParams.hv_PointsRow.Clear(); resultParams.hv_PointsColumn.Clear(); resultParams.hv_PointsNum.Clear(); resultParams.hv_DisToCircleArray.Clear();
		resultParams.hv_RetNum = 0; resultParams.hv_Roundness = 99999;
		GenEmptyObj(&resultParams.ho_Contour_Circle);
		GenEmptyObj(&resultParams.ho_Contour_Calipers);
		GenEmptyObj(&resultParams.ho_Contour_CrossPoints);
		//mutex1.unlock();
		mResultParams = resultParams;
		return -1;
	}
}
//д��־����
int CircleDetect::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char* p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�
	in << p << "\r";
	in.close();//�ر��ļ�
	return 0;
}

//��ȡϵͳʱ��
vector<string> CircleDetect::get_current_time()
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

int CircleDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName)
{
	//XML��ȡ����
	ErrorCode_Xml errorCode = dataIOC.ReadParams_Circle(configPath, XMLPath, mRunParams, processID, modubleName, modubleID);
	if (errorCode != Ok_Xml)
	{
		return 1;
	}
	return 0;
}
std::vector<std::string> CircleDetect::getModubleResultTitleList()
{
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("X");
	modubleResultTitleList.push_back("Y");
	modubleResultTitleList.push_back("Distance");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> CircleDetect::getModubleResultList()
{
	std::vector<std::map<int, std::string>> results;
	int count = mResultParams.hv_RetNum.I();
	if (count > 0)
	{
		for (int i = 0; i < mResultParams.hv_PointsRow.TupleLength(); i++)
		{
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = formatDobleValue(mResultParams.hv_PointsColumn[i].D(), 2);
			result[index++] = formatDobleValue(mResultParams.hv_PointsRow[i].D(), 2);
			result[index++] = formatDobleValue(mResultParams.hv_DisToCircleArray[i].D(), 2);

			results.push_back(result);
		}
	}
	return results;
}
std::string CircleDetect::formatDobleValue(double val, int fixed)
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
