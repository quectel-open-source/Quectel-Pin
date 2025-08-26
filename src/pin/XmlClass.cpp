#include "XmlClass.h"

XmlClass::XmlClass(string Path)
{
	FilePath = Path;
}
XmlClass::~XmlClass()
{}

//function：create a xml file创建XML文件
//param：FilePath.c_str():xml文件路径
//return：0,成功；非0，失败
int XmlClass::createXML()
{
	try
	{
		// 判定目录或文件是否存在的标识符
		int mode = 0;
		if (_access(FilePath.c_str(), mode))
		{
			const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
			XMLDocument doc;
			doc.Parse(declaration);//会覆盖xml所有内容

			//添加申明可以使用如下两行
			//XMLDeclaration* declaration=doc.NewDeclaration();
			//doc.InsertFirstChild(declaration);

			XMLElement* root = doc.NewElement("DBUSER");
			doc.InsertEndChild(root);

			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//xml文件已存在
			return 0;
		}
	}
	catch (...)
	{
		return 1;
	}
}

//FilePath.c_str() xml 文件路径, section 父节点，key 子节点， value 保存的值
//return：0 成功; 非 0 失败
int XmlClass::InsertXMLNode( const string stationName, const string&  section, const string&  key, const string&  value)
{
	try
	{
		//XML读取
		XMLDocument doc;
		int ret = doc.LoadFile(FilePath.c_str());
		if (ret != 0)
		{
			return ret;
		}
		//获取XML基础点
		XMLElement* root = doc.RootElement();
		//获取流程名称节点
		XMLElement* stationNameNode_Temp = root->FirstChildElement(stationName.c_str());
		//判断流程名称节点是否存在，不存在则插入
		if (stationNameNode_Temp == NULL)
		{
			//添加流程名称节点
			XMLElement* stationNameNode = doc.NewElement(stationName.c_str());
			root->InsertEndChild(stationNameNode);

			//添加父节点
			XMLElement* SectionNode = doc.NewElement(section.c_str());
			//SectionNode->SetAttribute("Name", user.userName.c_str());//添加属性暂时屏蔽


			//添加子节点，并设置子节点的值
			XMLElement* keyNode = doc.NewElement(key.c_str());
			keyNode->InsertEndChild(doc.NewText(value.c_str()));

			//子节点添加到父节点
			SectionNode->InsertEndChild(keyNode);

			//父节点添加到流程名称节点
			stationNameNode->InsertEndChild(SectionNode);

			//保存
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//流程名称节点已经存在
			//判断父节点是否存在，不存在则插入
			XMLElement* SectionNode_Temp = stationNameNode_Temp->FirstChildElement(section.c_str());
			if (SectionNode_Temp == NULL)
			{
				//添加父节点
				XMLElement* SectionNode = doc.NewElement(section.c_str());
				//SectionNode->SetAttribute("Name", user.userName.c_str());//添加属性暂时屏蔽


				//添加子节点，并设置子节点的值
				XMLElement* keyNode = doc.NewElement(key.c_str());
				keyNode->InsertEndChild(doc.NewText(value.c_str()));

				//子节点添加到父节点
				SectionNode->InsertEndChild(keyNode);

				//父节点添加到流程名称节点
				stationNameNode_Temp->InsertEndChild(SectionNode);

				//保存
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//判断子节点是否存在，不存在则插入。存在则修改其值
				XMLElement* keyNode_Temp = SectionNode_Temp->FirstChildElement(key.c_str());
				if (keyNode_Temp == NULL)
				{
					//添加子节点，并设置子节点的值
					XMLElement* keyNode = doc.NewElement(key.c_str());
					keyNode->InsertEndChild(doc.NewText(value.c_str()));

					//子节点添加到父节点
					SectionNode_Temp->InsertEndChild(keyNode);

					//保存
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					SetXMLValue(stationName, section, key, value);
					return 0;
				}
			}
		}
	}
	catch (...)
	{
		return -1;
	}
}
//读取指定节点内容
string XmlClass::GetXMLValue(const string stationName, const string&  section, const string&  key)
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return "null";
		}

		XMLElement* root = doc.RootElement();
		//流程名称节点
		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode == NULL)
		{
			return "null";
		}
		//父节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement(section.c_str());
		if (SectionNode == NULL)
		{
			return "null";
		}
		XMLElement* KeyNode = SectionNode->FirstChildElement(key.c_str());
		if (KeyNode == NULL)
		{
			return "null";
		}

		return KeyNode->GetText();
	}
	catch (...)
	{
		return "null";
	}
}

//写入指定节点内容
//return 0 OK,非0 NG
int XmlClass::SetXMLValue(const string stationName, const string&  section, const string&  key, const string&  value)
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		XMLElement* root = doc.RootElement();
		//流程名称节点
		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode == NULL)
		{
			return 1;
		}
		//父节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement(section.c_str());
		if (SectionNode == NULL)
		{
			return 1;
		}
		XMLElement* KeyNode = SectionNode->FirstChildElement(key.c_str());
		if (KeyNode == NULL)
		{
			return 1;
		}
		KeyNode->SetText(value.c_str());

		if (doc.SaveFile(FilePath.c_str()) == 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (...)
	{
		return -1;
	}
}

//删除父节点下指定子节点
//return 0 OK,非0 NG
int XmlClass::DeleteSection_Key(const string stationName, const string&  section, const string&  key)
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}
		XMLElement* root = doc.RootElement();
		//doc.DeleteNode(root);//删除xml所有节点

		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode != NULL)
		{
			XMLElement* SectionNode = StationNameNode->FirstChildElement(section.c_str());
			if (SectionNode != NULL)
			{
				//userNode->DeleteAttribute("Password");//删除属性
				XMLElement* KeyNode = SectionNode->FirstChildElement(key.c_str());
				if (KeyNode != NULL)
				{
					SectionNode->DeleteChild(KeyNode); //删除指定节点
					//SectionNode->DeleteChildren();//删除节点的所有孩子节点
					if (doc.SaveFile(FilePath.c_str()) == 0)
					{
						return 0;
					}
					else
					{
						return 1;
					}
				}
				else
				{
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}

	}
	catch (...)
	{
		return -1;
	}
}
//删除父节点下面的所有子节点
//return 0 OK,非0 NG
int XmlClass::DeleteSection_AllKeys(const string stationName, const string&  section)
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}
		XMLElement* root = doc.RootElement();

		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode != NULL)
		{
			XMLElement* SectionNode = StationNameNode->FirstChildElement(section.c_str());
			if (SectionNode != NULL)
			{
				SectionNode->DeleteChildren();//删除节点的所有孩子节点
				if (doc.SaveFile(FilePath.c_str()) == 0)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}
	catch (...)
	{
		return -1;
	}
}
//删除XML全部内容
//return 0 OK,非0 NG
int XmlClass::DeleteXMLValue()
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			return 1;
		}
		XMLElement* root = doc.RootElement();
		doc.DeleteNode(root);//删除xml所有节点

		XMLElement* root0 = doc.NewElement("DBUSER");
		doc.InsertEndChild(root0);

		if (doc.SaveFile(FilePath.c_str()) == 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (...)
	{
		return -1;
	}
}
void XmlClass::StrToInt(const string&  str, int& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}

void XmlClass::StrToDouble(const string&  str, double& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}

void XmlClass::strToChar(const string&  str, char& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}
