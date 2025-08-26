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
	Ok_Xml,						//没有错误
	nullFile_Xml,				//Xml文件路径不存在
	nullConfig_Xml,				//config节点不存在
	nullProcess_Xml,			//流程ID，Process不存在
	nullNode_Xml,				//node节点不存在
	nullNodeNameOrID_Xml,		//模块名称或者ID，NodeNameOrID不存在
	nullKeyName_Xml,			//参数名KeyName，不存在
	nullUnKnow_Xml				//Catch捕捉的错误，有可能是属性不存在代码跳Catch
};
class XmlClass2
{
public:
	XmlClass2();
	~XmlClass2();
	//创建XML文件(xmlPath xml路径)
	int CreateXML(const string FilePath);
	//插入(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称，value 值 )
	int InsertXMLNode(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, const string& value);
	//读取XML内容(processId 流程Id， sectionName 父节点名称，sectionId 父节点Id, keyName 子节点名称, value 输出读取到的值)
	ErrorCode_Xml GetXMLValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, string& value);
	
	//读取流程使用的模块字段
	ErrorCode_Xml GetXML_Process(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID);
	//插入全局变量的字段(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
	int InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//插入全局变量的字段和全局变量的名称(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
	int InsertXML_FieldValueAndGlobalName(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//读取全局变量的字段(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称, fieldValue 全局变量字段名称)
	ErrorCode_Xml GetXML_FieldValue(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//读取全局变量的字段值和名字(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称, fieldValue 全局变量字段名称)
	ErrorCode_Xml GetXML_FieldValueAndGlobalName(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldValue);
	//删除全局变量的字段
	int DeleteXML_FieldValue(const string FilePath);
	//删除全局变量的字段和名字
	int DeleteXML_FieldValueAndGlobalName(const string FilePath);


	//删除指定流程下的指定模块的指定参数
	int DeleteSection_Key(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName);
	//删除指定流程下的指定模块的所有参数
	int DeleteSection_AllKeys(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID);
	//删除XML全部内容
	int DeleteXMLValue(const string FilePath);
	//数据转换
	void StrToInt(const string&  str, int& num);
	void StrToDouble(const string&  str, double& num);
	void strToChar(const string&  str, char& num);
	
};

