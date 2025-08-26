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


class XmlClass
{
public:
	XmlClass(string Path);
	~XmlClass();
	//����XML�ļ�(xmlPath xml·��)
	int createXML();
	//����(xmlPath xml·����stationName �������ƣ� section ���ڵ㣬key �ӽڵ㣬value ֵ )
	int InsertXMLNode(const string stationName, const string&  section, const string&  key, const string&  value);
	string GetXMLValue(const string stationName, const string&  section, const string&  key);
	int SetXMLValue(const string stationName, const string&  section, const string&  key, const string&  value);
	int DeleteSection_Key(const string stationName, const string&  section, const string&  key);
	int DeleteSection_AllKeys(const string stationName, const string&  section);
	int DeleteXMLValue();

	void StrToInt(const string&  str, int& num);
	void StrToDouble(const string&  str, double& num);
	void strToChar(const string&  str, char& num);
private:
	string FilePath;

};

