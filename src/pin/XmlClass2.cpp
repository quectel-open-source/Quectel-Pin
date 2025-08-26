#include "XmlClass2.h"
XmlClass2::XmlClass2()
{
}
XmlClass2::~XmlClass2()
{}

//function��create a xml file����XML�ļ�
//param��FilePath.c_str():xml�ļ�·��
//return��0,�ɹ�����0��ʧ��
int XmlClass2::CreateXML(const string FilePath)
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

//����(stationId ����Id�� sectionName ���ڵ����ƣ�sectionId ���ڵ�Id, keyName �ӽڵ����ƣ�value ֵ )
//return��0 �ɹ�; �� 0 ʧ��
int XmlClass2::InsertXMLNode(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, const string& value)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		int isNotExit_Process = 0;
		int isNotExit_Section = 0;
		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			//����������ƽڵ�
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
			doc.InsertEndChild(stationNameNode);

			//���ģ��ڵ�
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

			//����ӽڵ㣬�������ӽڵ��ֵ
			XMLElement* keyNode = doc.NewElement(keyName.c_str());
			keyNode->InsertEndChild(doc.NewText(value.c_str()));

			//�ӽڵ���ӵ����ڵ�
			SectionNode->InsertEndChild(keyNode);

			//ģ��ڵ���ӵ��������ƽڵ�
			stationNameNode->InsertEndChild(SectionNode);

			//����
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//��ȡ����Id
			string IdTemp = StationNameNode->Attribute("process");
			while (IdTemp != to_string(processId))
			{
				//��ȡ��һ�����̽ڵ�
				StationNameNode = StationNameNode->NextSiblingElement("config");
				if (StationNameNode == NULL)
				{
					isNotExit_Process += 1;
					break;
				}
				IdTemp = StationNameNode->Attribute("process");
				if (IdTemp == to_string(processId))
				{
					isNotExit_Process = 0;
				}
			}
			//isNotExit_Process > 0˵��ָ�������̽ڵ㲻����
			if (isNotExit_Process > 0)
			{
				//����������ƽڵ�
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
				doc.InsertEndChild(stationNameNode);

				//���ģ��ڵ�
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

				//����ӽڵ㣬�������ӽڵ��ֵ
				XMLElement* keyNode = doc.NewElement(keyName.c_str());
				keyNode->InsertEndChild(doc.NewText(value.c_str()));

				//�ӽڵ���ӵ����ڵ�
				SectionNode->InsertEndChild(keyNode);

				//ģ��ڵ���ӵ��������ƽڵ�
				stationNameNode->InsertEndChild(SectionNode);

				//����
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//��ȡ��һ��ģ��ڵ�
				XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
				if (SectionNode == NULL)
				{
					return 1;
				}
				//��ȡģ������
				string nameTemp = SectionNode->Attribute("nodeName");
				//��ȡģ��Id
				IdTemp = SectionNode->Attribute("nodeProcessID");
				while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
				{
					//��ȡ��һ��ģ��ڵ�
					SectionNode = SectionNode->NextSiblingElement("node");
					if (SectionNode == NULL)
					{
						isNotExit_Section += 1;
						break;
					}
					nameTemp = SectionNode->Attribute("nodeName");
					IdTemp = SectionNode->Attribute("nodeProcessID");
					if (nameTemp == nodeName || IdTemp == to_string(processModbuleID))
					{
						isNotExit_Section = 0;
					}
				}
				//isNotExit_Section > 0˵��ָ����ģ��ڵ㲻����
				if (isNotExit_Section > 0)
				{
					//���ģ��ڵ�
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

					//����ӽڵ㣬�������ӽڵ��ֵ
					XMLElement* keyNode = doc.NewElement(keyName.c_str());
					keyNode->InsertEndChild(doc.NewText(value.c_str()));

					//�ӽڵ���ӵ����ڵ�
					SectionNode->InsertEndChild(keyNode);

					//ģ��ڵ���ӵ��������ƽڵ�
					StationNameNode->InsertEndChild(SectionNode);

					//����
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//��ȡ�����ڵ㣬�ж��Ƿ����
					XMLElement* KeyNode = SectionNode->FirstChildElement(keyName.c_str());
					if (KeyNode == NULL)
					{
						//����ӽڵ㣬�������ӽڵ��ֵ
						XMLElement* keyNode = doc.NewElement(keyName.c_str());
						string valueTemp;
						if (value == "" || value == "null")
						{
							valueTemp = "null";
						}
						else
						{
							valueTemp = value;
						}
						keyNode->InsertEndChild(doc.NewText(valueTemp.c_str()));
						//�ӽڵ���ӵ����ڵ�
						SectionNode->InsertEndChild(keyNode);

						//ģ��ڵ���ӵ��������ƽڵ�
						StationNameNode->InsertEndChild(SectionNode);

						//����
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//�޸��ӽڵ��ֵ
						string valueTemp;
						if (value == "" || value == "null")
						{
							valueTemp = "null";
						}
						else
						{
							valueTemp = value;
						}
						KeyNode->SetText(valueTemp.c_str());
				
						//����
						return doc.SaveFile(FilePath.c_str());
					}
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
ErrorCode_Xml XmlClass2::GetXMLValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, string& value)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return nullProcess_Xml;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}
		//��ȡģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		//��ȡģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return nullNodeNameOrID_Xml;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}

		//��ȡ�����ڵ�
		XMLElement* KeyNode = SectionNode->FirstChildElement(keyName.c_str());
		if (KeyNode == NULL)
		{
			return nullKeyName_Xml;
		}

		value = KeyNode->GetText();
		return Ok_Xml;
	}
	catch (...)
	{
		return nullUnKnow_Xml;
	}
}
//��ȡ����ʹ�õ�ģ���ֶ�
ErrorCode_Xml XmlClass2::GetXML_Process(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}
		
		//��ȡ��һ�����̵�����
		vector<string> vecName;
		vector<int> vecModbuleID;
		//��ȡģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//��ȡģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		do
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				break;
			}
	
			nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));
		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);


		vecName.clear();
		vecModbuleID.clear();
		do
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}
	
			//��ȡ����Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//��ȡ��һ��ģ��ڵ�
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//��ȡģ������
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//��ȡģ��Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));
		
			do
			{
				//��ȡ��һ��ģ��ڵ�
				SectionNode = SectionNode->NextSiblingElement("node");
				if (SectionNode == NULL)
				{
					break;
				}
				nameTemp = SectionNode->Attribute("nodeName");
				vecName.push_back(nameTemp);
				IdTemp = SectionNode->Attribute("nodeProcessID");
				vecModbuleID.push_back(atoi(IdTemp.c_str()));
			} while (true);
		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);



		return Ok_Xml;
	}
	catch (...)
	{
		return nullUnKnow_Xml;
	}
}
//����ȫ�ֱ������ֶκ�ȫ�ֱ���������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
int XmlClass2::InsertXML_FieldValueAndGlobalName(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//��ȡȫ�ֱ���XML�ڵ�
		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValueNode == NULL)
		{
			//���ȫ�ֱ������ƽڵ�
			XMLElement* globalValueNode = doc.NewElement("globalValueAndName");
			doc.InsertEndChild(globalValueNode);

			//����������ƽڵ�
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id

			//���ģ��ڵ�
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

			//����ӽڵ㣬�������ӽڵ��ֵ
			XMLElement* keyNode = doc.NewElement("fieldValue");
			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

			//�ӽڵ���ӵ����ڵ�
			SectionNode->InsertEndChild(keyNode);

			//ģ��ڵ���ӵ��������ƽڵ�
			stationNameNode->InsertEndChild(SectionNode);

			//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
			globalValueNode->InsertEndChild(stationNameNode);

			//����
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			int isNotExit_Process = 0;
			int isNotExit_Section = 0;
			//��ȡ��һ�����̽ڵ�
			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
			if (StationNameNode == NULL)
			{
				//����������ƽڵ�
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id

				//���ģ��ڵ�
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

				//����ӽڵ㣬�������ӽڵ��ֵ
				XMLElement* keyNode = doc.NewElement("fieldValue");
				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

				//�ӽڵ���ӵ����ڵ�
				SectionNode->InsertEndChild(keyNode);

				//ģ��ڵ���ӵ��������ƽڵ�
				stationNameNode->InsertEndChild(SectionNode);

				//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
				GlobalValueNode->InsertEndChild(stationNameNode);

				//����
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//��ȡ����Id
				string IdTemp = StationNameNode->Attribute("process");
				while (IdTemp != to_string(processId))
				{
					//��ȡ��һ�����̽ڵ�
					StationNameNode = StationNameNode->NextSiblingElement("config");
					if (StationNameNode == NULL)
					{
						isNotExit_Process += 1;
						break;
					}
					IdTemp = StationNameNode->Attribute("process");
					if (IdTemp == to_string(processId))
					{
						isNotExit_Process = 0;
					}
				}
				//isNotExit_Process > 0˵��ָ�������̽ڵ㲻����
				if (isNotExit_Process > 0)
				{
					//����������ƽڵ�
					XMLElement* stationNameNode = doc.NewElement("config");
					stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id


					//���ģ��ڵ�
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

					//����ӽڵ㣬�������ӽڵ��ֵ
					XMLElement* keyNode = doc.NewElement("fieldValue");
					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

					//�ӽڵ���ӵ����ڵ�
					SectionNode->InsertEndChild(keyNode);

					//ģ��ڵ���ӵ��������ƽڵ�
					stationNameNode->InsertEndChild(SectionNode);

					//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
					GlobalValueNode->InsertEndChild(stationNameNode);

					//����
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//��ȡ��һ��ģ��ڵ�
					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
					if (SectionNode == NULL)
					{
						//���ģ��ڵ�
						XMLElement* SectionNode = doc.NewElement("node");
						SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

						//����ӽڵ㣬�������ӽڵ��ֵ
						XMLElement* keyNode = doc.NewElement("fieldValue");
						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

						//�ӽڵ���ӵ����ڵ�
						SectionNode->InsertEndChild(keyNode);

						//ģ��ڵ���ӵ��������ƽڵ�
						StationNameNode->InsertEndChild(SectionNode);

						//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
						GlobalValueNode->InsertEndChild(StationNameNode);

						//����
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//��ȡģ������
						string nameTemp = SectionNode->Attribute("nodeName");
						//��ȡģ��Id
						IdTemp = SectionNode->Attribute("nodeProcessID");
						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
						{
							//��ȡ��һ��ģ��ڵ�
							SectionNode = SectionNode->NextSiblingElement("node");
							if (SectionNode == NULL)
							{
								isNotExit_Section += 1;
								break;
							}
							nameTemp = SectionNode->Attribute("nodeName");
							IdTemp = SectionNode->Attribute("nodeProcessID");
							if (nameTemp == nodeName || IdTemp == to_string(processModbuleID))
							{
								isNotExit_Section = 0;
							}
						}
						//isNotExit_Section > 0˵��ָ����ģ��ڵ㲻����
						if (isNotExit_Section > 0)
						{
							//���ģ��ڵ�
							XMLElement* SectionNode = doc.NewElement("node");
							SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

							//����ӽڵ㣬�������ӽڵ��ֵ
							XMLElement* keyNode = doc.NewElement("fieldValue");
							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

							//�ӽڵ���ӵ����ڵ�
							SectionNode->InsertEndChild(keyNode);

							//ģ��ڵ���ӵ��������ƽڵ�
							StationNameNode->InsertEndChild(SectionNode);

							//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
							GlobalValueNode->InsertEndChild(StationNameNode);

							//����
							return doc.SaveFile(FilePath.c_str());
						}
						else
						{
							//����ӽڵ㣬�������ӽڵ��ֵ
							XMLElement* keyNode = doc.NewElement("fieldValue");
							string valueTemp;
							if (keyName == "" || keyName == "null")
							{
								valueTemp = "null";
							}
							else
							{
								valueTemp = keyName;
							}
							keyNode->InsertEndChild(doc.NewText(valueTemp.c_str()));
							//�ӽڵ���ӵ����ڵ�
							SectionNode->InsertEndChild(keyNode);

							//ģ��ڵ���ӵ��������ƽڵ�
							StationNameNode->InsertEndChild(SectionNode);

							//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
							GlobalValueNode->InsertEndChild(StationNameNode);

							//����
							return doc.SaveFile(FilePath.c_str());

						}
					}
				}
			}
		}
	}
	catch (...)
	{
		return -1;
	}
}
//����ÿ�����̣�ÿ��ģ����Ҫ�õ�ȫ�ֱ������ֶ�(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
int XmlClass2::InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//��ȡȫ�ֱ���XML�ڵ�
		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValue");
		if (GlobalValueNode == NULL)
		{
			//���ȫ�ֱ������ƽڵ�
			XMLElement* globalValueNode = doc.NewElement("globalValue");
			doc.InsertEndChild(globalValueNode);

			//����������ƽڵ�
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id

			//���ģ��ڵ�
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

			//����ӽڵ㣬�������ӽڵ��ֵ
			XMLElement* keyNode = doc.NewElement("fieldValue");
			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

			//�ӽڵ���ӵ����ڵ�
			SectionNode->InsertEndChild(keyNode);

			//ģ��ڵ���ӵ��������ƽڵ�
			stationNameNode->InsertEndChild(SectionNode);

			//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
			globalValueNode->InsertEndChild(stationNameNode);

			//����
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			int isNotExit_Process = 0;
			int isNotExit_Section = 0;
			//��ȡ��һ�����̽ڵ�
			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
			if (StationNameNode == NULL)
			{
				//����������ƽڵ�
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id

				//���ģ��ڵ�
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

				//����ӽڵ㣬�������ӽڵ��ֵ
				XMLElement* keyNode = doc.NewElement("fieldValue");
				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

				//�ӽڵ���ӵ����ڵ�
				SectionNode->InsertEndChild(keyNode);

				//ģ��ڵ���ӵ��������ƽڵ�
				stationNameNode->InsertEndChild(SectionNode);

				//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
				GlobalValueNode->InsertEndChild(stationNameNode);

				//����
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//��ȡ����Id
				string IdTemp = StationNameNode->Attribute("process");
				while (IdTemp != to_string(processId))
				{
					//��ȡ��һ�����̽ڵ�
					StationNameNode = StationNameNode->NextSiblingElement("config");
					if (StationNameNode == NULL)
					{
						isNotExit_Process += 1;
						break;
					}
					IdTemp = StationNameNode->Attribute("process");
					if (IdTemp == to_string(processId))
					{
						isNotExit_Process = 0;
					}
				}
				//isNotExit_Process > 0˵��ָ�������̽ڵ㲻����
				if (isNotExit_Process > 0)
				{
					//����������ƽڵ�
					XMLElement* stationNameNode = doc.NewElement("config");
					stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
		

					//���ģ��ڵ�
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

					//����ӽڵ㣬�������ӽڵ��ֵ
					XMLElement* keyNode = doc.NewElement("fieldValue");
					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

					//�ӽڵ���ӵ����ڵ�
					SectionNode->InsertEndChild(keyNode);

					//ģ��ڵ���ӵ��������ƽڵ�
					stationNameNode->InsertEndChild(SectionNode);

					//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
					GlobalValueNode->InsertEndChild(stationNameNode);

					//����
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//��ȡ��һ��ģ��ڵ�
					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
					if (SectionNode == NULL)
					{
						//���ģ��ڵ�
						XMLElement* SectionNode = doc.NewElement("node");
						SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

						//����ӽڵ㣬�������ӽڵ��ֵ
						XMLElement* keyNode = doc.NewElement("fieldValue");
						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

						//�ӽڵ���ӵ����ڵ�
						SectionNode->InsertEndChild(keyNode);

						//ģ��ڵ���ӵ��������ƽڵ�
						StationNameNode->InsertEndChild(SectionNode);

						//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
						GlobalValueNode->InsertEndChild(StationNameNode);

						//����
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//��ȡģ������
						string nameTemp = SectionNode->Attribute("nodeName");
						//��ȡģ��Id
						IdTemp = SectionNode->Attribute("nodeProcessID");
						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
						{
							//��ȡ��һ��ģ��ڵ�
							SectionNode = SectionNode->NextSiblingElement("node");
							if (SectionNode == NULL)
							{
								isNotExit_Section += 1;
								break;
							}
							nameTemp = SectionNode->Attribute("nodeName");
							IdTemp = SectionNode->Attribute("nodeProcessID");
							if (nameTemp == nodeName || IdTemp == to_string(processModbuleID))
							{
								isNotExit_Section = 0;
							}
						}
						//isNotExit_Section > 0˵��ָ����ģ��ڵ㲻����
						if (isNotExit_Section > 0)
						{
							//���ģ��ڵ�
							XMLElement* SectionNode = doc.NewElement("node");
							SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID

							//����ӽڵ㣬�������ӽڵ��ֵ
							XMLElement* keyNode = doc.NewElement("fieldValue");
							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

							//�ӽڵ���ӵ����ڵ�
							SectionNode->InsertEndChild(keyNode);

							//ģ��ڵ���ӵ��������ƽڵ�
							StationNameNode->InsertEndChild(SectionNode);

							//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
							GlobalValueNode->InsertEndChild(StationNameNode);

							//����
							return doc.SaveFile(FilePath.c_str());
						}
						else
						{
							//����ӽڵ㣬�������ӽڵ��ֵ
							XMLElement* keyNode = doc.NewElement("fieldValue");
							string valueTemp;
							if (keyName == "" || keyName == "null")
							{
								valueTemp = "null";
							}
							else
							{
								valueTemp = keyName;
							}
							keyNode->InsertEndChild(doc.NewText(valueTemp.c_str()));
							//�ӽڵ���ӵ����ڵ�
							SectionNode->InsertEndChild(keyNode);

							//ģ��ڵ���ӵ��������ƽڵ�
							StationNameNode->InsertEndChild(SectionNode);

							//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
							GlobalValueNode->InsertEndChild(StationNameNode);

							//����
							return doc.SaveFile(FilePath.c_str());
					
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		return -1;
	}
}
//��ȡȫ�ֱ������ֶ�(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����, fieldValue ȫ�ֱ����ֶ�����)
ErrorCode_Xml XmlClass2::GetXML_FieldValue(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldName)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ"globalValue"�ڵ�
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValue");
		if (GlobalValuNode == NULL)
		{
			return nullConfig_Xml;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = GlobalValuNode->FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}

		//��ȡ��һ�����̵�����
		vector<string> vecName;
		vector<int> vecModbuleID;
		vector<string> vecField;
		vector<vector<string> > fieldValue;
		//��ȡ��һ��ģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//��ȡ��һ��ģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		vecField.clear();
		//��ȡ��һ��ȫ�ֱ����ֶνڵ�
		XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
		if (FieldValueNode == NULL)
		{
			return nullNode_Xml;
		}
		//��ȡ��һ��ȫ�ֱ����ֶ�����
		vecField.push_back(FieldValueNode->GetText());
		do
		{
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				break;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
		} while (true);
		fieldValue.push_back(vecField);
	
		
		//��ȡ��һ��ģ��ڵ�
		do
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				break;
			}

			nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);
		
		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);
		fieldName.push_back(fieldValue);
	


		vecName.clear();
		vecModbuleID.clear();
		vecField.clear();
		fieldValue.clear();
		do
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}

			//��ȡ����Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//��ȡ��һ��ģ��ڵ�
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//��ȡģ������
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//��ȡģ��Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);
		

			do
			{
				//��ȡ��һ��ģ��ڵ�
				SectionNode = SectionNode->NextSiblingElement("node");
				if (SectionNode == NULL)
				{
					break;
				}
				nameTemp = SectionNode->Attribute("nodeName");
				vecName.push_back(nameTemp);
				IdTemp = SectionNode->Attribute("nodeProcessID");
				vecModbuleID.push_back(atoi(IdTemp.c_str()));
			
				vecField.clear();
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					return nullNode_Xml;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
				do
				{
					//��ȡ��һ��ȫ�ֱ����ֶνڵ�
					FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
					if (FieldValueNode == NULL)
					{
						break;
					}
					//��ȡ��һ��ȫ�ֱ����ֶ�����
					vecField.push_back(FieldValueNode->GetText());
				} while (true);
				fieldValue.push_back(vecField);
			
			} while (true);
		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);
		fieldName.push_back(fieldValue);
	


		return Ok_Xml;
	}
	catch (...)
	{
		return nullUnKnow_Xml;
	}
}
//��ȡȫ�ֱ������ֶ�ֵ������(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����, fieldValue ȫ�ֱ����ֶ�����)
ErrorCode_Xml XmlClass2::GetXML_FieldValueAndGlobalName(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldName)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ"globalValueAndName"�ڵ�
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValuNode == NULL)
		{
			return nullConfig_Xml;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = GlobalValuNode->FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}

		//��ȡ��һ�����̵�����
		vector<string> vecName;
		vector<int> vecModbuleID;
		vector<string> vecField;
		vector<vector<string> > fieldValue;
		//��ȡ��һ��ģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//��ȡ��һ��ģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		vecField.clear();
		//��ȡ��һ��ȫ�ֱ����ֶνڵ�
		XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
		if (FieldValueNode == NULL)
		{
			return nullNode_Xml;
		}
		//��ȡ��һ��ȫ�ֱ����ֶ�����
		vecField.push_back(FieldValueNode->GetText());
		do
		{
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				break;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
		} while (true);
		fieldValue.push_back(vecField);


		//��ȡ��һ��ģ��ڵ�
		do
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				break;
			}

			nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);

		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);
		fieldName.push_back(fieldValue);



		vecName.clear();
		vecModbuleID.clear();
		vecField.clear();
		fieldValue.clear();
		do
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}

			//��ȡ����Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//��ȡ��һ��ģ��ڵ�
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//��ȡģ������
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//��ȡģ��Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//��ȡ��һ��ȫ�ֱ����ֶνڵ�
			XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//��ȡ��һ��ȫ�ֱ����ֶ�����
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);


			do
			{
				//��ȡ��һ��ģ��ڵ�
				SectionNode = SectionNode->NextSiblingElement("node");
				if (SectionNode == NULL)
				{
					break;
				}
				nameTemp = SectionNode->Attribute("nodeName");
				vecName.push_back(nameTemp);
				IdTemp = SectionNode->Attribute("nodeProcessID");
				vecModbuleID.push_back(atoi(IdTemp.c_str()));

				vecField.clear();
				//��ȡ��һ��ȫ�ֱ����ֶνڵ�
				XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					return nullNode_Xml;
				}
				//��ȡ��һ��ȫ�ֱ����ֶ�����
				vecField.push_back(FieldValueNode->GetText());
				do
				{
					//��ȡ��һ��ȫ�ֱ����ֶνڵ�
					FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
					if (FieldValueNode == NULL)
					{
						break;
					}
					//��ȡ��һ��ȫ�ֱ����ֶ�����
					vecField.push_back(FieldValueNode->GetText());
				} while (true);
				fieldValue.push_back(vecField);

			} while (true);
		} while (true);
		nodeName.push_back(vecName);
		processModbuleID.push_back(vecModbuleID);
		fieldName.push_back(fieldValue);



		return Ok_Xml;
	}
	catch (...)
	{
		return nullUnKnow_Xml;
	}
}
//ɾ��ȫ�ֱ������ֶ�
int XmlClass2::DeleteXML_FieldValue(const string FilePath)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ"globalValue"�ڵ�
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValue");
		if (GlobalValuNode == NULL)
		{
			return 0;
		}

		//ɾ��
		GlobalValuNode->DeleteChildren();//ɾ���ڵ�����к��ӽڵ�
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
//ɾ��ȫ�ֱ������ֶκ�����
int XmlClass2::DeleteXML_FieldValueAndGlobalName(const string FilePath)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//��ȡ"globalValueAndName"�ڵ�
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValuNode == NULL)
		{
			return 0;
		}

		//ɾ��
		GlobalValuNode->DeleteChildren();//ɾ���ڵ�����к��ӽڵ�
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

////����ÿ�����̣�ÿ��ģ����Ҫ�õ�ȫ�ֱ������ֶ�����(processId ����Id�� nodeName ���ڵ����ƣ�processModbuleID ���ڵ�Id, keyName �ӽڵ�����)
//int XmlClass2::InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
//{
//	try
//	{
//		//��ȡXML
//		XMLDocument doc;
//		if (doc.LoadFile(FilePath.c_str()) != 0)
//		{
//			cout << "load xml file failed" << endl;
//			return 1;
//		}
//
//		//��ȡȫ�ֱ���XML�ڵ�
//		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValue");
//		if (GlobalValueNode == NULL)
//		{
//			//���ȫ�ֱ������ƽڵ�
//			XMLElement* globalValueNode = doc.NewElement("globalValue");
//			doc.InsertEndChild(globalValueNode);
//
//			//����������ƽڵ�
//			XMLElement* stationNameNode = doc.NewElement("config");
//			stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
//
//			//���ģ��ڵ�
//			XMLElement* SectionNode = doc.NewElement("node");
//			SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
//			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID
//
//			//����ӽڵ㣬�������ӽڵ��ֵ
//			XMLElement* keyNode = doc.NewElement("fieldValue");
//			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//			//�ӽڵ���ӵ����ڵ�
//			SectionNode->InsertEndChild(keyNode);
//
//			//ģ��ڵ���ӵ��������ƽڵ�
//			stationNameNode->InsertEndChild(SectionNode);
//
//			//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//			globalValueNode->InsertEndChild(stationNameNode);
//
//			//����
//			return doc.SaveFile(FilePath.c_str());
//		}
//		else
//		{
//			int isNotExit_Process = 0;
//			int isNotExit_Section = 0;
//			//��ȡ��һ�����̽ڵ�
//			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
//			if (StationNameNode == NULL)
//			{
//				//����������ƽڵ�
//				XMLElement* stationNameNode = doc.NewElement("config");
//				stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
//
//				//���ģ��ڵ�
//				XMLElement* SectionNode = doc.NewElement("node");
//				SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
//				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID
//
//				//����ӽڵ㣬�������ӽڵ��ֵ
//				XMLElement* keyNode = doc.NewElement("fieldValue");
//				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//				//�ӽڵ���ӵ����ڵ�
//				SectionNode->InsertEndChild(keyNode);
//
//				//ģ��ڵ���ӵ��������ƽڵ�
//				stationNameNode->InsertEndChild(SectionNode);
//
//				//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//				GlobalValueNode->InsertEndChild(stationNameNode);
//
//				//����
//				return doc.SaveFile(FilePath.c_str());
//			}
//			else
//			{
//				//��ȡ����Id
//				string IdTemp = StationNameNode->Attribute("process");
//				while (IdTemp != to_string(processId))
//				{
//					//��ȡ��һ�����̽ڵ�
//					StationNameNode = StationNameNode->NextSiblingElement("config");
//					if (StationNameNode == NULL)
//					{
//						isNotExit_Process += 1;
//						break;
//					}
//					IdTemp = StationNameNode->Attribute("process");
//					if (IdTemp == to_string(processId))
//					{
//						isNotExit_Process = 0;
//					}
//				}
//				//isNotExit_Process > 0˵��ָ�������̽ڵ㲻����
//				if (isNotExit_Process > 0)
//				{
//					//����������ƽڵ�
//					XMLElement* stationNameNode = doc.NewElement("config");
//					stationNameNode->SetAttribute("process", to_string(processId).c_str());//�������Id
//
//
//					//���ģ��ڵ�
//					XMLElement* SectionNode = doc.NewElement("node");
//					SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
//					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID
//
//					//����ӽڵ㣬�������ӽڵ��ֵ
//					XMLElement* keyNode = doc.NewElement("fieldValue");
//					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//					//�ӽڵ���ӵ����ڵ�
//					SectionNode->InsertEndChild(keyNode);
//
//					//ģ��ڵ���ӵ��������ƽڵ�
//					stationNameNode->InsertEndChild(SectionNode);
//
//					//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//					GlobalValueNode->InsertEndChild(stationNameNode);
//
//					//����
//					return doc.SaveFile(FilePath.c_str());
//				}
//				else
//				{
//					//��ȡ��һ��ģ��ڵ�
//					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
//					if (SectionNode == NULL)
//					{
//						//���ģ��ڵ�
//						XMLElement* SectionNode = doc.NewElement("node");
//						SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
//						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID
//
//						//����ӽڵ㣬�������ӽڵ��ֵ
//						XMLElement* keyNode = doc.NewElement("fieldValue");
//						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//						//�ӽڵ���ӵ����ڵ�
//						SectionNode->InsertEndChild(keyNode);
//
//						//ģ��ڵ���ӵ��������ƽڵ�
//						StationNameNode->InsertEndChild(SectionNode);
//
//						//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//						GlobalValueNode->InsertEndChild(StationNameNode);
//
//						//����
//						return doc.SaveFile(FilePath.c_str());
//					}
//					else
//					{
//						//��ȡģ������
//						string nameTemp = SectionNode->Attribute("nodeName");
//						//��ȡģ��Id
//						IdTemp = SectionNode->Attribute("nodeProcessID");
//						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
//						{
//							//��ȡ��һ��ģ��ڵ�
//							SectionNode = SectionNode->NextSiblingElement("node");
//							if (SectionNode == NULL)
//							{
//								isNotExit_Section += 1;
//								break;
//							}
//							nameTemp = SectionNode->Attribute("nodeName");
//							IdTemp = SectionNode->Attribute("nodeProcessID");
//							if (nameTemp == nodeName || IdTemp == to_string(processModbuleID))
//							{
//								isNotExit_Section = 0;
//							}
//						}
//						//isNotExit_Section > 0˵��ָ����ģ��ڵ㲻����
//						if (isNotExit_Section > 0)
//						{
//							//���ģ��ڵ�
//							XMLElement* SectionNode = doc.NewElement("node");
//							SectionNode->SetAttribute("nodeName", nodeName.c_str());//�������nodeName
//							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//�������nodeProcessID
//
//							//����ӽڵ㣬�������ӽڵ��ֵ
//							XMLElement* keyNode = doc.NewElement("fieldValue");
//							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//							//�ӽڵ���ӵ����ڵ�
//							SectionNode->InsertEndChild(keyNode);
//
//							//ģ��ڵ���ӵ��������ƽڵ�
//							StationNameNode->InsertEndChild(SectionNode);
//
//							//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//							GlobalValueNode->InsertEndChild(StationNameNode);
//
//							//����
//							return doc.SaveFile(FilePath.c_str());
//						}
//						else
//						{
//							//��ȡ�����ڵ㣬�ж��Ƿ����
//							XMLElement* KeyNode = SectionNode->FirstChildElement("fieldValue");
//							if (KeyNode == NULL)
//							{
//								//����ӽڵ㣬�������ӽڵ��ֵ
//								XMLElement* keyNode = doc.NewElement("fieldValue");
//								string valueTemp;
//								if (keyName == "" || keyName == "null")
//								{
//									valueTemp = "null";
//								}
//								else
//								{
//									valueTemp = keyName;
//								}
//								keyNode->InsertEndChild(doc.NewText(valueTemp.c_str()));
//								//�ӽڵ���ӵ����ڵ�
//								SectionNode->InsertEndChild(keyNode);
//
//								//ģ��ڵ���ӵ��������ƽڵ�
//								StationNameNode->InsertEndChild(SectionNode);
//
//								//���̽ڵ���ӵ�ȫ�ֱ����ڵ�
//								GlobalValueNode->InsertEndChild(StationNameNode);
//
//								//����
//								return doc.SaveFile(FilePath.c_str());
//							}
//							else
//							{
//								//�޸��ӽڵ��ֵ
//								string valueTemp;
//								if (keyName == "" || keyName == "null")
//								{
//									valueTemp = "null";
//								}
//								else
//								{
//									valueTemp = keyName;
//								}
//								KeyNode->SetText(valueTemp.c_str());
//
//								//����
//								return doc.SaveFile(FilePath.c_str());
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	catch (...)
//	{
//		return -1;
//	}
//}



//ɾ�����ڵ���ָ���ӽڵ�
//return 0 OK,��0 NG
int XmlClass2::DeleteSection_Key(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//doc.DeleteNode(root);//ɾ��xml���нڵ�

		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return 1;
		}

		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return 1;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return 1;
		}
		//��ȡģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		//��ȡģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return 1;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}

		//��ȡ�����ڵ�
		XMLElement* KeyNode = SectionNode->FirstChildElement(keyName.c_str());
		if (KeyNode == NULL)
		{
			return 1;
		}
		//ɾ��
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
	catch (...)
	{
		return -1;
	}
}
//ɾ�����ڵ�����������ӽڵ�
//return 0 OK,��0 NG
int XmlClass2::DeleteSection_AllKeys(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID)
{
	try
	{
		//��ȡXML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//��ȡ��һ�����̽ڵ�
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return 1;
		}

		//��ȡ����Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//��ȡ��һ�����̽ڵ�
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return 1;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//��ȡ��һ��ģ��ڵ�
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return 1;
		}
		//��ȡģ������
		string nameTemp = SectionNode->Attribute("nodeName");
		//��ȡģ��Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//��ȡ��һ��ģ��ڵ�
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return 1;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}
		//ɾ��
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
	catch (...)
	{
		return -1;
	}
}
//ɾ��XMLȫ������
//return 0 OK,��0 NG
int XmlClass2::DeleteXMLValue(const string FilePath)
{
	try
	{
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			return 1;
		}
		doc.DeleteChildren();
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
void XmlClass2::StrToInt(const string&  str, int& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}

void XmlClass2::StrToDouble(const string&  str, double& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}

void XmlClass2::strToChar(const string&  str, char& num)
{
	stringstream stream;
	stream << str;
	stream >> num;
}
