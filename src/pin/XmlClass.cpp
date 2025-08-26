#include "XmlClass.h"

XmlClass::XmlClass(string Path)
{
	FilePath = Path;
}
XmlClass::~XmlClass()
{}

//function��create a xml file����XML�ļ�
//param��FilePath.c_str():xml�ļ�·��
//return��0,�ɹ�����0��ʧ��
int XmlClass::createXML()
{
	try
	{
		// �ж�Ŀ¼���ļ��Ƿ���ڵı�ʶ��
		int mode = 0;
		if (_access(FilePath.c_str(), mode))
		{
			const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
			XMLDocument doc;
			doc.Parse(declaration);//�Ḳ��xml��������

			//�����������ʹ����������
			//XMLDeclaration* declaration=doc.NewDeclaration();
			//doc.InsertFirstChild(declaration);

			XMLElement* root = doc.NewElement("DBUSER");
			doc.InsertEndChild(root);

			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//xml�ļ��Ѵ���
			return 0;
		}
	}
	catch (...)
	{
		return 1;
	}
}

//FilePath.c_str() xml �ļ�·��, section ���ڵ㣬key �ӽڵ㣬 value �����ֵ
//return��0 �ɹ�; �� 0 ʧ��
int XmlClass::InsertXMLNode( const string stationName, const string&  section, const string&  key, const string&  value)
{
	try
	{
		//XML��ȡ
		XMLDocument doc;
		int ret = doc.LoadFile(FilePath.c_str());
		if (ret != 0)
		{
			return ret;
		}
		//��ȡXML������
		XMLElement* root = doc.RootElement();
		//��ȡ�������ƽڵ�
		XMLElement* stationNameNode_Temp = root->FirstChildElement(stationName.c_str());
		//�ж��������ƽڵ��Ƿ���ڣ������������
		if (stationNameNode_Temp == NULL)
		{
			//����������ƽڵ�
			XMLElement* stationNameNode = doc.NewElement(stationName.c_str());
			root->InsertEndChild(stationNameNode);

			//��Ӹ��ڵ�
			XMLElement* SectionNode = doc.NewElement(section.c_str());
			//SectionNode->SetAttribute("Name", user.userName.c_str());//���������ʱ����


			//����ӽڵ㣬�������ӽڵ��ֵ
			XMLElement* keyNode = doc.NewElement(key.c_str());
			keyNode->InsertEndChild(doc.NewText(value.c_str()));

			//�ӽڵ���ӵ����ڵ�
			SectionNode->InsertEndChild(keyNode);

			//���ڵ���ӵ��������ƽڵ�
			stationNameNode->InsertEndChild(SectionNode);

			//����
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//�������ƽڵ��Ѿ�����
			//�жϸ��ڵ��Ƿ���ڣ������������
			XMLElement* SectionNode_Temp = stationNameNode_Temp->FirstChildElement(section.c_str());
			if (SectionNode_Temp == NULL)
			{
				//��Ӹ��ڵ�
				XMLElement* SectionNode = doc.NewElement(section.c_str());
				//SectionNode->SetAttribute("Name", user.userName.c_str());//���������ʱ����


				//����ӽڵ㣬�������ӽڵ��ֵ
				XMLElement* keyNode = doc.NewElement(key.c_str());
				keyNode->InsertEndChild(doc.NewText(value.c_str()));

				//�ӽڵ���ӵ����ڵ�
				SectionNode->InsertEndChild(keyNode);

				//���ڵ���ӵ��������ƽڵ�
				stationNameNode_Temp->InsertEndChild(SectionNode);

				//����
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//�ж��ӽڵ��Ƿ���ڣ�����������롣�������޸���ֵ
				XMLElement* keyNode_Temp = SectionNode_Temp->FirstChildElement(key.c_str());
				if (keyNode_Temp == NULL)
				{
					//����ӽڵ㣬�������ӽڵ��ֵ
					XMLElement* keyNode = doc.NewElement(key.c_str());
					keyNode->InsertEndChild(doc.NewText(value.c_str()));

					//�ӽڵ���ӵ����ڵ�
					SectionNode_Temp->InsertEndChild(keyNode);

					//����
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
//��ȡָ���ڵ�����
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
		//�������ƽڵ�
		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode == NULL)
		{
			return "null";
		}
		//���ڵ�
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

//д��ָ���ڵ�����
//return 0 OK,��0 NG
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
		//�������ƽڵ�
		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode == NULL)
		{
			return 1;
		}
		//���ڵ�
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

//ɾ�����ڵ���ָ���ӽڵ�
//return 0 OK,��0 NG
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
		//doc.DeleteNode(root);//ɾ��xml���нڵ�

		XMLElement* StationNameNode = root->FirstChildElement(stationName.c_str());
		if (StationNameNode != NULL)
		{
			XMLElement* SectionNode = StationNameNode->FirstChildElement(section.c_str());
			if (SectionNode != NULL)
			{
				//userNode->DeleteAttribute("Password");//ɾ������
				XMLElement* KeyNode = SectionNode->FirstChildElement(key.c_str());
				if (KeyNode != NULL)
				{
					SectionNode->DeleteChild(KeyNode); //ɾ��ָ���ڵ�
					//SectionNode->DeleteChildren();//ɾ���ڵ�����к��ӽڵ�
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
//ɾ�����ڵ�����������ӽڵ�
//return 0 OK,��0 NG
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
				SectionNode->DeleteChildren();//ɾ���ڵ�����к��ӽڵ�
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
//ɾ��XMLȫ������
//return 0 OK,��0 NG
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
		doc.DeleteNode(root);//ɾ��xml���нڵ�

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
