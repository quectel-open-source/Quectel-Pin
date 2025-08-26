#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <thread>
#include <string>
#include <mutex>
#include <io.h>
#include <direct.h>
#include "XmlClass2.h"
#include "dataDetect.h"


using namespace std;
using namespace HalconCpp;
class DataIO
{
public:
	DataIO();
	~DataIO();
	//����һά�������(runParams һά�������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID)
	int WriteParams_BarCode(const string ConfigPath, const string XmlPath, const RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡһά�������(runParams һά�������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID, value �����ȡ����ֵ)
	ErrorCode_Xml ReadParams_BarCode(const string ConfigPath, const string XmlPath, RunParamsStruct_BarCode &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�����ά�������
	int WriteParams_Code2d(const string ConfigPath, const string XmlPath, const RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ά�������
	ErrorCode_Xml ReadParams_Code2d(const string ConfigPath, const string XmlPath, RunParamsStruct_Code2d &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ģ��ƥ�����(OK����0��NG����1)
	int WriteParams_Template(const string ConfigPath, const string XmlPath, const RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ģ��ƥ�����(OK����0��NG����1)
	ErrorCode_Xml ReadParams_Template(const string ConfigPath, const string XmlPath, RunParamsStruct_Template &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	
	//����ģ��ƥ�����(OK����0��NG����1)
	int WriteParams_ImgDifference(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ģ��ƥ�����(OK����0��NG����1)
	ErrorCode_Xml ReadParams_ImgDifference(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgDifference &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	
	//����ֱ�߼�����
	int WriteParams_Line(const string ConfigPath, const string XmlPath, const RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡֱ�߼�����
	ErrorCode_Xml ReadParams_Line(const string ConfigPath, const string XmlPath, RunParamsStruct_Line &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����Բ������
	int WriteParams_Circle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡԲ������
	ErrorCode_Xml ReadParams_Circle(const string ConfigPath, const string XmlPath, RunParamsStruct_Circle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������μ�����
	int WriteParams_Rectangle(const string ConfigPath, const string XmlPath, const RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ���μ�����
	ErrorCode_Xml ReadParams_Rectangle(const string ConfigPath, const string XmlPath, RunParamsStruct_Rectangle &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������Բ������
	int WriteParams_Ellipse(const string ConfigPath, const string XmlPath, const RunParamsStruct_Ellipse &runParams, const int& processId, const const string& nodeName, const int& processModbuleID);
	//��ȡ��Բ������
	ErrorCode_Xml ReadParams_Ellipse(const string ConfigPath, const string XmlPath, RunParamsStruct_Ellipse &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//�����ֵ������
	int WriteParams_Binarization(const string ConfigPath, const string XmlPath, const RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ֵ������
	ErrorCode_Xml ReadParams_Binarization(const string ConfigPath, const string XmlPath, RunParamsStruct_BinarizationDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������̬ѧ����
	int WriteParams_Morphology(const string ConfigPath, const string XmlPath, const RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��̬ѧ����
	ErrorCode_Xml ReadParams_Morphology(const string ConfigPath, const string XmlPath, RunParamsStruct_MorphologyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����Blob����
	int WriteParams_Blob(const string ConfigPath, const string XmlPath, const RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡBlob����
	ErrorCode_Xml ReadParams_Blob(const string ConfigPath, const string XmlPath, RunParamsStruct_Blob &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ͼ����ǿ����
	int WriteParams_ImgProcessOne(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡͼ����ǿ����
	ErrorCode_Xml ReadParams_ImgProcessOne(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessOneDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������ͼ�������
	int WriteParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const const string& nodeName, const int& processModbuleID);
	//��ȡ��ͼ�������
	ErrorCode_Xml ReadParams_ImgProcessTwo(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgProcessTwoDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ͼ��ת������
	int WriteParams_ImgConvert(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡͼ��ת������
	ErrorCode_Xml ReadParams_ImgConvert(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgConvertDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ͼ����ת����
	int WriteParams_ImgRotate(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡͼ����ת����
	ErrorCode_Xml ReadParams_ImgRotate(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgRotateDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ͼƬ�������
	int WriteParams_ImgClassify(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡͼƬ�������
	ErrorCode_Xml ReadParams_ImgClassify(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgClassifyDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//���漫����任����
	int WriteParams_PolarTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ������任����
	ErrorCode_Xml ReadParams_PolarTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_PolarTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����궨��궨����
	int WriteParams_CalibBoard(const string ConfigPath, const string XmlPath, const ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ�궨��궨����
	ErrorCode_Xml ReadParams_CalibBoard(const string ConfigPath, const string XmlPath, ResultParamsStruct_CalibBoardDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�������������
	int WriteParams_PhotoStereo(const string ConfigPath, const string XmlPath, const RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ����������
	ErrorCode_Xml ReadParams_PhotoStereo(const string ConfigPath, const string XmlPath, RunParamsStruct_PhotoStereo &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//���渵��Ҷ�任����
	int WriteParams_FourierTrans(const string ConfigPath, const string XmlPath, const RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ����Ҷ�任����
	ErrorCode_Xml ReadParams_FourierTrans(const string ConfigPath, const string XmlPath, RunParamsStruct_FourierTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�������任����
	int WriteParams_AffineMatrix(const string ConfigPath, const string XmlPath, const RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ����任����
	ErrorCode_Xml ReadParams_AffineMatrix(const string ConfigPath, const string XmlPath, RunParamsStruct_AffineMatrixDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�����ͼ����
	int WriteParams_CutImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ͼ����
	ErrorCode_Xml ReadParams_CutImg(const string ConfigPath, const string XmlPath, RunParamsStruct_CutImgDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//���洴��Roi����
	int WriteParams_CreateRoi(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ����Roi����
	ErrorCode_Xml ReadParams_CreateRoi(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateRoiDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////������ɫ������
	//int WriteParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////��ȡ��ɫ������
	//ErrorCode_Xml ReadParams_ColorDetect_MoreNum(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect_MoreNum &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������ɫ������
	int WriteParams_ColorDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ɫ������
	ErrorCode_Xml ReadParams_ColorDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//������ϲ���
	int WriteParams_FitDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ϲ���
	ErrorCode_Xml ReadParams_FitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_FitDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//���潻�����
	int WriteParams_Intersection(const string ConfigPath, const string XmlPath, const RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ�������
	ErrorCode_Xml ReadParams_Intersection(const string ConfigPath, const string XmlPath, RunParamsStruct_IntersectionDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�����������
	int WriteParams_Measurement(const string ConfigPath, const string XmlPath, const RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��������
	ErrorCode_Xml ReadParams_Measurement(const string ConfigPath, const string XmlPath, RunParamsStruct_MeasurementDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//�����ͼ����
	int WriteParams_SaveImg(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ͼ����
	ErrorCode_Xml ReadParams_SaveImg(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSaveDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//����OKNG�жϵĲ���
	int WriteParams_Judgement(const string ConfigPath, const string XmlPath, const RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡOKNG�жϵĲ���
	ErrorCode_Xml ReadParams_Judgement(const string ConfigPath, const string XmlPath, RunParamsStruct_Judgement &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//������ʾ���ڲ���
	int WriteParams_ImgShow(const string ConfigPath, const string XmlPath, const RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ��ʾ���ڲ���
	ErrorCode_Xml ReadParams_ImgShow(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgShowDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//��������任����
	int WriteParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ����任����
	ErrorCode_Xml ReadParams_ShapeTransDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ShapeTransDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//���洴����������
	int WriteParams_CreateContourDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ������������
	ErrorCode_Xml ReadParams_CreateContourDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CreateContourDetect &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////��������˶�λ��������(runParams ��λ����������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID)
	//int WriteParams_RobotCalib(const string ConfigPath, const string XmlPath, const RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////��ȡ�����˶�λ����(runParams ��λ����������processId ����ID, nodeName ģ�����ƣ� processModbuleID ģ��ID, value �����ȡ����ֵ)
	//ErrorCode_Xml ReadParams_RobotCalib(const string ConfigPath, const string XmlPath, RunParamsStruct_RobotCalib &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//�������ϱ궨�궨����
	int WriteParams_JointCalibration(const string ConfigPath, const string XmlPath, const RunParamsStruct_JointCalibrationDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡ���ϱ궨�궨����
	ErrorCode_Xml ReadParams_JointCalibration(const string ConfigPath, const string XmlPath, RunParamsStruct_JointCalibrationDetect &resultParams, const int& processId, const string& nodeName, const int& processModbuleID);

	////����Pin�������
	//int WriteParams_PinDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	////��ȡPin�������
	//ErrorCode_Xml ReadParams_PinDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pin &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//����Pcb������
	int WriteParams_PcbDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡPcb������
	ErrorCode_Xml ReadParams_PcbDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_Pcb &runParams, const int& processId, const string& nodeName, const int& processModbuleID);


	//����MachineLearning������
	int WriteParams_MachineLearning(const string ConfigPath, const string XmlPath, const RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡMachineLearning������
	ErrorCode_Xml ReadParams_MachineLearning(const string ConfigPath, const string XmlPath, RunParamsStruct_MachineLearning &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//�����ַ�������(OK����0��NG����1)
	int WriteParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//�����ַ�������(OK����0��NG����1)
	ErrorCode_Xml ReadParams_CharacterDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_CharacterDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//����ͼ���ֲ���(OK����0��NG����1)
	int WriteParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//����ͼ���ֲ���(OK����0��NG����1)
	ErrorCode_Xml ReadParams_ImgSplitDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ImgSplitDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	
	//����Ҷȼ������(OK����0��NG����1)
	int WriteParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//���ػҶȼ������(OK����0��NG����1)
	ErrorCode_Xml ReadParams_GrayValueDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_GrayValueDetect &RunParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//����ColorSort������
	int WriteParams_ColorSortDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡColorSort������
	ErrorCode_Xml ReadParams_ColorSortDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorSort &runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//����ColorExtract������
	int WriteParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, const RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID);
	//��ȡColorExtract������
	ErrorCode_Xml ReadParams_ColorExtractDetect(const string ConfigPath, const string XmlPath, RunParamsStruct_ColorExtract &runParams, const int& processId, const string& nodeName, const int& processModbuleID);



	//��ȡ����ʹ�õ�ģ���ֶ�
	ErrorCode_Xml ReadXML_Process_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID);
	//����ȫ�ֱ������ֶ�����(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
	int WriteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//����ȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
	int WriteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//��ȡȫ�ֱ������ֶ�����
	ErrorCode_Xml ReadXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//��ȡȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������
	ErrorCode_Xml ReadXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//ɾ��ȫ�ֱ������ֶ�����
	int DeleteXML_FieldValue_GlobalValue(const string ConfigPath, const string XmlPath);
	//ɾ��ȫ�ֱ������ֶ�ֵ��ȫ�ֱ�������
	int DeleteXML_FieldValueAndGlobalName_GlobalValue(const string ConfigPath, const string XmlPath);


private:

	//д��־����
	int WriteTxt(string content);
	//����һ������strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	//����ָ���ַ��ָ��ַ�����vector
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);
	//·������
	void PathSet(const string ConfigPath, const string XmlPath);
	string FolderPath();
private:
	//mutex mutex1;//�������õ��߳�ͬ������

};

