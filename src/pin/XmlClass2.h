#pragma once
#include <iostream>
#include "tinyxml2.h"
#include <string.h>
#include <string>
#include <io.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
using std::ifstream;
using std::ofstream;
using std::endl;
using std::cout;
using std::stringstream;
using namespace tinyxml2;
using namespace std;

enum ErrorCode_Xml
{
	Ok_Xml,						//û�д���
	nullFile_Xml,				//Xml�ļ�·��������
	nullConfig_Xml,				//config�ڵ㲻����
	nullProcess_Xml,			//����ID��Process������
	nullNode_Xml,				//node�ڵ㲻����
	nullNodeNameOrID_Xml,		//ģ�����ƻ���ID��NodeNameOrID������
	nullKeyName_Xml,			//������KeyName��������
	nullUnKnow_Xml				//Catch��׽�Ĵ����п��������Բ����ڴ�����Catch
};
class XmlClass2
{
public:
	XmlClass2();
	~XmlClass2();
	//����XML�ļ�(xmlPath xml·��)
	int CreateXML(const string FilePath);
	//����(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ����ƣ�value ֵ )
	int InsertXMLNode(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, const string& value);
	//��ȡXML����(processId ����Id�� sectionName ���ڵ����ƣ�sectionId ���ڵ�Id, keyName �ӽڵ�����, value �����ȡ����ֵ)
	ErrorCode_Xml GetXMLValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, string& value);
	
	//��ȡ����ʹ�õ�ģ���ֶ�
	ErrorCode_Xml GetXML_Process(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID);
	//����ȫ�ֱ������ֶ�(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
	int InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//����ȫ�ֱ������ֶκ�ȫ�ֱ���������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
	int InsertXML_FieldValueAndGlobalName(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//��ȡȫ�ֱ������ֶ�(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����, fieldValue ȫ�ֱ����ֶ�����)
	ErrorCode_Xml GetXML_FieldValue(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//��ȡȫ�ֱ������ֶ�ֵ������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����, fieldValue ȫ�ֱ����ֶ�����)
	ErrorCode_Xml GetXML_FieldValueAndGlobalName(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//ɾ��ȫ�ֱ������ֶ�
	int DeleteXML_FieldValue(const string FilePath);
	//ɾ��ȫ�ֱ������ֶκ�����
	int DeleteXML_FieldValueAndGlobalName(const string FilePath);


	//ɾ��ָ�������µ�ָ��ģ���ָ������
	int DeleteSection_Key(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//ɾ��ָ�������µ�ָ��ģ������в���
	int DeleteSection_AllKeys(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID);
	//ɾ��XMLȫ������
	int DeleteXMLValue(const string FilePath);
	//����ת��
	void StrToInt(const string&  str, int& num);
	void StrToDouble(const string&  str, double& num);
	void strToChar(const string&  str, char& num);
	
};

