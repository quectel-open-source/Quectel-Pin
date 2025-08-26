#include "XmlClass2.h"
XmlClass2::XmlClass2()
{
}
XmlClass2::~XmlClass2()
{}

//function：create a xml file创建XML文件
//param：FilePath.c_str():xml文件路径
//return：0,成功；非0，失败
int XmlClass2::CreateXML(const string FilePath)
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

//插入(stationId 流程Id， sectionName 父节点名称，sectionId 父节点Id, keyName 子节点名称，value 值 )
//return：0 成功; 非 0 失败
int XmlClass2::InsertXMLNode(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, const string& value)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		int isNotExit_Process = 0;
		int isNotExit_Section = 0;
		//获取第一个流程节点
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			//添加流程名称节点
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
			doc.InsertEndChild(stationNameNode);

			//添加模块节点
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

			//添加子节点，并设置子节点的值
			XMLElement* keyNode = doc.NewElement(keyName.c_str());
			keyNode->InsertEndChild(doc.NewText(value.c_str()));

			//子节点添加到父节点
			SectionNode->InsertEndChild(keyNode);

			//模块节点添加到流程名称节点
			stationNameNode->InsertEndChild(SectionNode);

			//保存
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			//获取流程Id
			string IdTemp = StationNameNode->Attribute("process");
			while (IdTemp != to_string(processId))
			{
				//获取下一个流程节点
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
			//isNotExit_Process > 0说明指定的流程节点不存在
			if (isNotExit_Process > 0)
			{
				//添加流程名称节点
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
				doc.InsertEndChild(stationNameNode);

				//添加模块节点
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

				//添加子节点，并设置子节点的值
				XMLElement* keyNode = doc.NewElement(keyName.c_str());
				keyNode->InsertEndChild(doc.NewText(value.c_str()));

				//子节点添加到父节点
				SectionNode->InsertEndChild(keyNode);

				//模块节点添加到流程名称节点
				stationNameNode->InsertEndChild(SectionNode);

				//保存
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//获取第一个模块节点
				XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
				if (SectionNode == NULL)
				{
					return 1;
				}
				//获取模块名称
				string nameTemp = SectionNode->Attribute("nodeName");
				//获取模块Id
				IdTemp = SectionNode->Attribute("nodeProcessID");
				while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
				{
					//获取下一个模块节点
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
				//isNotExit_Section > 0说明指定的模块节点不存在
				if (isNotExit_Section > 0)
				{
					//添加模块节点
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

					//添加子节点，并设置子节点的值
					XMLElement* keyNode = doc.NewElement(keyName.c_str());
					keyNode->InsertEndChild(doc.NewText(value.c_str()));

					//子节点添加到父节点
					SectionNode->InsertEndChild(keyNode);

					//模块节点添加到流程名称节点
					StationNameNode->InsertEndChild(SectionNode);

					//保存
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//获取参数节点，判断是否存在
					XMLElement* KeyNode = SectionNode->FirstChildElement(keyName.c_str());
					if (KeyNode == NULL)
					{
						//添加子节点，并设置子节点的值
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
						//子节点添加到父节点
						SectionNode->InsertEndChild(keyNode);

						//模块节点添加到流程名称节点
						StationNameNode->InsertEndChild(SectionNode);

						//保存
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//修改子节点的值
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
				
						//保存
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
//读取指定节点内容
ErrorCode_Xml XmlClass2::GetXMLValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName, string& value)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return nullProcess_Xml;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}
		//获取模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		//获取模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//获取下一个模块节点
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return nullNodeNameOrID_Xml;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}

		//获取参数节点
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
//读取流程使用的模块字段
ErrorCode_Xml XmlClass2::GetXML_Process(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}
		
		//获取第一个流程的内容
		vector<string> vecName;
		vector<int> vecModbuleID;
		//获取模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//获取模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		do
		{
			//获取下一个模块节点
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
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}
	
			//获取流程Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//获取第一个模块节点
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//获取模块名称
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//获取模块Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));
		
			do
			{
				//获取下一个模块节点
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
//插入全局变量的字段和全局变量的名称(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
int XmlClass2::InsertXML_FieldValueAndGlobalName(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//获取全局变量XML节点
		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValueNode == NULL)
		{
			//添加全局变量名称节点
			XMLElement* globalValueNode = doc.NewElement("globalValueAndName");
			doc.InsertEndChild(globalValueNode);

			//添加流程名称节点
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id

			//添加模块节点
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

			//添加子节点，并设置子节点的值
			XMLElement* keyNode = doc.NewElement("fieldValue");
			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

			//子节点添加到父节点
			SectionNode->InsertEndChild(keyNode);

			//模块节点添加到流程名称节点
			stationNameNode->InsertEndChild(SectionNode);

			//流程节点添加到全局变量节点
			globalValueNode->InsertEndChild(stationNameNode);

			//保存
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			int isNotExit_Process = 0;
			int isNotExit_Section = 0;
			//获取第一个流程节点
			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
			if (StationNameNode == NULL)
			{
				//添加流程名称节点
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id

				//添加模块节点
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

				//添加子节点，并设置子节点的值
				XMLElement* keyNode = doc.NewElement("fieldValue");
				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

				//子节点添加到父节点
				SectionNode->InsertEndChild(keyNode);

				//模块节点添加到流程名称节点
				stationNameNode->InsertEndChild(SectionNode);

				//流程节点添加到全局变量节点
				GlobalValueNode->InsertEndChild(stationNameNode);

				//保存
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//获取流程Id
				string IdTemp = StationNameNode->Attribute("process");
				while (IdTemp != to_string(processId))
				{
					//获取下一个流程节点
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
				//isNotExit_Process > 0说明指定的流程节点不存在
				if (isNotExit_Process > 0)
				{
					//添加流程名称节点
					XMLElement* stationNameNode = doc.NewElement("config");
					stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id


					//添加模块节点
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

					//添加子节点，并设置子节点的值
					XMLElement* keyNode = doc.NewElement("fieldValue");
					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

					//子节点添加到父节点
					SectionNode->InsertEndChild(keyNode);

					//模块节点添加到流程名称节点
					stationNameNode->InsertEndChild(SectionNode);

					//流程节点添加到全局变量节点
					GlobalValueNode->InsertEndChild(stationNameNode);

					//保存
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//获取第一个模块节点
					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
					if (SectionNode == NULL)
					{
						//添加模块节点
						XMLElement* SectionNode = doc.NewElement("node");
						SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

						//添加子节点，并设置子节点的值
						XMLElement* keyNode = doc.NewElement("fieldValue");
						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

						//子节点添加到父节点
						SectionNode->InsertEndChild(keyNode);

						//模块节点添加到流程名称节点
						StationNameNode->InsertEndChild(SectionNode);

						//流程节点添加到全局变量节点
						GlobalValueNode->InsertEndChild(StationNameNode);

						//保存
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//获取模块名称
						string nameTemp = SectionNode->Attribute("nodeName");
						//获取模块Id
						IdTemp = SectionNode->Attribute("nodeProcessID");
						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
						{
							//获取下一个模块节点
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
						//isNotExit_Section > 0说明指定的模块节点不存在
						if (isNotExit_Section > 0)
						{
							//添加模块节点
							XMLElement* SectionNode = doc.NewElement("node");
							SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

							//添加子节点，并设置子节点的值
							XMLElement* keyNode = doc.NewElement("fieldValue");
							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

							//子节点添加到父节点
							SectionNode->InsertEndChild(keyNode);

							//模块节点添加到流程名称节点
							StationNameNode->InsertEndChild(SectionNode);

							//流程节点添加到全局变量节点
							GlobalValueNode->InsertEndChild(StationNameNode);

							//保存
							return doc.SaveFile(FilePath.c_str());
						}
						else
						{
							//添加子节点，并设置子节点的值
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
							//子节点添加到父节点
							SectionNode->InsertEndChild(keyNode);

							//模块节点添加到流程名称节点
							StationNameNode->InsertEndChild(SectionNode);

							//流程节点添加到全局变量节点
							GlobalValueNode->InsertEndChild(StationNameNode);

							//保存
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
//插入每个流程，每个模块需要拿到全局变量的字段(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
int XmlClass2::InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//获取全局变量XML节点
		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValue");
		if (GlobalValueNode == NULL)
		{
			//添加全局变量名称节点
			XMLElement* globalValueNode = doc.NewElement("globalValue");
			doc.InsertEndChild(globalValueNode);

			//添加流程名称节点
			XMLElement* stationNameNode = doc.NewElement("config");
			stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id

			//添加模块节点
			XMLElement* SectionNode = doc.NewElement("node");
			SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

			//添加子节点，并设置子节点的值
			XMLElement* keyNode = doc.NewElement("fieldValue");
			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

			//子节点添加到父节点
			SectionNode->InsertEndChild(keyNode);

			//模块节点添加到流程名称节点
			stationNameNode->InsertEndChild(SectionNode);

			//流程节点添加到全局变量节点
			globalValueNode->InsertEndChild(stationNameNode);

			//保存
			return doc.SaveFile(FilePath.c_str());
		}
		else
		{
			int isNotExit_Process = 0;
			int isNotExit_Section = 0;
			//获取第一个流程节点
			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
			if (StationNameNode == NULL)
			{
				//添加流程名称节点
				XMLElement* stationNameNode = doc.NewElement("config");
				stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id

				//添加模块节点
				XMLElement* SectionNode = doc.NewElement("node");
				SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

				//添加子节点，并设置子节点的值
				XMLElement* keyNode = doc.NewElement("fieldValue");
				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

				//子节点添加到父节点
				SectionNode->InsertEndChild(keyNode);

				//模块节点添加到流程名称节点
				stationNameNode->InsertEndChild(SectionNode);

				//流程节点添加到全局变量节点
				GlobalValueNode->InsertEndChild(stationNameNode);

				//保存
				return doc.SaveFile(FilePath.c_str());
			}
			else
			{
				//获取流程Id
				string IdTemp = StationNameNode->Attribute("process");
				while (IdTemp != to_string(processId))
				{
					//获取下一个流程节点
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
				//isNotExit_Process > 0说明指定的流程节点不存在
				if (isNotExit_Process > 0)
				{
					//添加流程名称节点
					XMLElement* stationNameNode = doc.NewElement("config");
					stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
		

					//添加模块节点
					XMLElement* SectionNode = doc.NewElement("node");
					SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

					//添加子节点，并设置子节点的值
					XMLElement* keyNode = doc.NewElement("fieldValue");
					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

					//子节点添加到父节点
					SectionNode->InsertEndChild(keyNode);

					//模块节点添加到流程名称节点
					stationNameNode->InsertEndChild(SectionNode);

					//流程节点添加到全局变量节点
					GlobalValueNode->InsertEndChild(stationNameNode);

					//保存
					return doc.SaveFile(FilePath.c_str());
				}
				else
				{
					//获取第一个模块节点
					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
					if (SectionNode == NULL)
					{
						//添加模块节点
						XMLElement* SectionNode = doc.NewElement("node");
						SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

						//添加子节点，并设置子节点的值
						XMLElement* keyNode = doc.NewElement("fieldValue");
						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

						//子节点添加到父节点
						SectionNode->InsertEndChild(keyNode);

						//模块节点添加到流程名称节点
						StationNameNode->InsertEndChild(SectionNode);

						//流程节点添加到全局变量节点
						GlobalValueNode->InsertEndChild(StationNameNode);

						//保存
						return doc.SaveFile(FilePath.c_str());
					}
					else
					{
						//获取模块名称
						string nameTemp = SectionNode->Attribute("nodeName");
						//获取模块Id
						IdTemp = SectionNode->Attribute("nodeProcessID");
						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
						{
							//获取下一个模块节点
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
						//isNotExit_Section > 0说明指定的模块节点不存在
						if (isNotExit_Section > 0)
						{
							//添加模块节点
							XMLElement* SectionNode = doc.NewElement("node");
							SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID

							//添加子节点，并设置子节点的值
							XMLElement* keyNode = doc.NewElement("fieldValue");
							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));

							//子节点添加到父节点
							SectionNode->InsertEndChild(keyNode);

							//模块节点添加到流程名称节点
							StationNameNode->InsertEndChild(SectionNode);

							//流程节点添加到全局变量节点
							GlobalValueNode->InsertEndChild(StationNameNode);

							//保存
							return doc.SaveFile(FilePath.c_str());
						}
						else
						{
							//添加子节点，并设置子节点的值
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
							//子节点添加到父节点
							SectionNode->InsertEndChild(keyNode);

							//模块节点添加到流程名称节点
							StationNameNode->InsertEndChild(SectionNode);

							//流程节点添加到全局变量节点
							GlobalValueNode->InsertEndChild(StationNameNode);

							//保存
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
//读取全局变量的字段(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称, fieldValue 全局变量字段名称)
ErrorCode_Xml XmlClass2::GetXML_FieldValue(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldName)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取"globalValue"节点
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValue");
		if (GlobalValuNode == NULL)
		{
			return nullConfig_Xml;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = GlobalValuNode->FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}

		//获取第一个流程的内容
		vector<string> vecName;
		vector<int> vecModbuleID;
		vector<string> vecField;
		vector<vector<string> > fieldValue;
		//获取第一个模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//获取第一个模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		vecField.clear();
		//获取第一个全局变量字段节点
		XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
		if (FieldValueNode == NULL)
		{
			return nullNode_Xml;
		}
		//获取第一个全局变量字段名称
		vecField.push_back(FieldValueNode->GetText());
		do
		{
			//获取下一个全局变量字段节点
			FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				break;
			}
			//获取下一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
		} while (true);
		fieldValue.push_back(vecField);
	
		
		//获取下一个模块节点
		do
		{
			//获取下一个模块节点
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
			//获取第一个全局变量字段节点
			FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//获取第一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//获取下一个全局变量字段节点
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//获取下一个全局变量字段名称
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
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}

			//获取流程Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//获取第一个模块节点
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//获取模块名称
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//获取模块Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//获取第一个全局变量字段节点
			XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//获取第一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//获取下一个全局变量字段节点
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//获取下一个全局变量字段名称
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);
		

			do
			{
				//获取下一个模块节点
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
				//获取第一个全局变量字段节点
				XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					return nullNode_Xml;
				}
				//获取第一个全局变量字段名称
				vecField.push_back(FieldValueNode->GetText());
				do
				{
					//获取下一个全局变量字段节点
					FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
					if (FieldValueNode == NULL)
					{
						break;
					}
					//获取下一个全局变量字段名称
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
//读取全局变量的字段值和名字(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称, fieldValue 全局变量字段名称)
ErrorCode_Xml XmlClass2::GetXML_FieldValueAndGlobalName(const string FilePath, vector<int>& processId, vector<vector<string> >& nodeName, vector<vector<int> >& processModbuleID, vector<vector<vector<string> > >& fieldName)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取"globalValueAndName"节点
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValuNode == NULL)
		{
			return nullConfig_Xml;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = GlobalValuNode->FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return nullConfig_Xml;
		}
		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		processId.push_back(atoi(IdTemp.c_str()));

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return nullNode_Xml;
		}

		//获取第一个流程的内容
		vector<string> vecName;
		vector<int> vecModbuleID;
		vector<string> vecField;
		vector<vector<string> > fieldValue;
		//获取第一个模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		vecName.push_back(nameTemp);
		//获取第一个模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		vecModbuleID.push_back(atoi(IdTemp.c_str()));

		vecField.clear();
		//获取第一个全局变量字段节点
		XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
		if (FieldValueNode == NULL)
		{
			return nullNode_Xml;
		}
		//获取第一个全局变量字段名称
		vecField.push_back(FieldValueNode->GetText());
		do
		{
			//获取下一个全局变量字段节点
			FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				break;
			}
			//获取下一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
		} while (true);
		fieldValue.push_back(vecField);


		//获取下一个模块节点
		do
		{
			//获取下一个模块节点
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
			//获取第一个全局变量字段节点
			FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//获取第一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//获取下一个全局变量字段节点
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//获取下一个全局变量字段名称
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
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				break;
			}

			//获取流程Id
			IdTemp = StationNameNode->Attribute("process");
			processId.push_back(atoi(IdTemp.c_str()));

			//获取第一个模块节点
			XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
			if (SectionNode == NULL)
			{
				return nullNode_Xml;
			}

			//获取模块名称
			string nameTemp = SectionNode->Attribute("nodeName");
			vecName.push_back(nameTemp);
			//获取模块Id
			IdTemp = SectionNode->Attribute("nodeProcessID");
			vecModbuleID.push_back(atoi(IdTemp.c_str()));

			vecField.clear();
			//获取第一个全局变量字段节点
			XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
			if (FieldValueNode == NULL)
			{
				return nullNode_Xml;
			}
			//获取第一个全局变量字段名称
			vecField.push_back(FieldValueNode->GetText());
			do
			{
				//获取下一个全局变量字段节点
				FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					break;
				}
				//获取下一个全局变量字段名称
				vecField.push_back(FieldValueNode->GetText());
			} while (true);
			fieldValue.push_back(vecField);


			do
			{
				//获取下一个模块节点
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
				//获取第一个全局变量字段节点
				XMLElement* FieldValueNode = SectionNode->FirstChildElement("fieldValue");
				if (FieldValueNode == NULL)
				{
					return nullNode_Xml;
				}
				//获取第一个全局变量字段名称
				vecField.push_back(FieldValueNode->GetText());
				do
				{
					//获取下一个全局变量字段节点
					FieldValueNode = FieldValueNode->NextSiblingElement("fieldValue");
					if (FieldValueNode == NULL)
					{
						break;
					}
					//获取下一个全局变量字段名称
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
//删除全局变量的字段
int XmlClass2::DeleteXML_FieldValue(const string FilePath)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取"globalValue"节点
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValue");
		if (GlobalValuNode == NULL)
		{
			return 0;
		}

		//删除
		GlobalValuNode->DeleteChildren();//删除节点的所有孩子节点
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
//删除全局变量的字段和名字
int XmlClass2::DeleteXML_FieldValueAndGlobalName(const string FilePath)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return nullFile_Xml;
		}

		//获取"globalValueAndName"节点
		XMLElement* GlobalValuNode = doc.FirstChildElement("globalValueAndName");
		if (GlobalValuNode == NULL)
		{
			return 0;
		}

		//删除
		GlobalValuNode->DeleteChildren();//删除节点的所有孩子节点
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

////插入每个流程，每个模块需要拿到全局变量的字段名称(processId 流程Id， nodeName 父节点名称，processModbuleID 父节点Id, keyName 子节点名称)
//int XmlClass2::InsertXML_FieldValue(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
//{
//	try
//	{
//		//读取XML
//		XMLDocument doc;
//		if (doc.LoadFile(FilePath.c_str()) != 0)
//		{
//			cout << "load xml file failed" << endl;
//			return 1;
//		}
//
//		//获取全局变量XML节点
//		XMLElement* GlobalValueNode = doc.FirstChildElement("globalValue");
//		if (GlobalValueNode == NULL)
//		{
//			//添加全局变量名称节点
//			XMLElement* globalValueNode = doc.NewElement("globalValue");
//			doc.InsertEndChild(globalValueNode);
//
//			//添加流程名称节点
//			XMLElement* stationNameNode = doc.NewElement("config");
//			stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
//
//			//添加模块节点
//			XMLElement* SectionNode = doc.NewElement("node");
//			SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
//			SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID
//
//			//添加子节点，并设置子节点的值
//			XMLElement* keyNode = doc.NewElement("fieldValue");
//			keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//			//子节点添加到父节点
//			SectionNode->InsertEndChild(keyNode);
//
//			//模块节点添加到流程名称节点
//			stationNameNode->InsertEndChild(SectionNode);
//
//			//流程节点添加到全局变量节点
//			globalValueNode->InsertEndChild(stationNameNode);
//
//			//保存
//			return doc.SaveFile(FilePath.c_str());
//		}
//		else
//		{
//			int isNotExit_Process = 0;
//			int isNotExit_Section = 0;
//			//获取第一个流程节点
//			XMLElement* StationNameNode = GlobalValueNode->FirstChildElement("config");
//			if (StationNameNode == NULL)
//			{
//				//添加流程名称节点
//				XMLElement* stationNameNode = doc.NewElement("config");
//				stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
//
//				//添加模块节点
//				XMLElement* SectionNode = doc.NewElement("node");
//				SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
//				SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID
//
//				//添加子节点，并设置子节点的值
//				XMLElement* keyNode = doc.NewElement("fieldValue");
//				keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//				//子节点添加到父节点
//				SectionNode->InsertEndChild(keyNode);
//
//				//模块节点添加到流程名称节点
//				stationNameNode->InsertEndChild(SectionNode);
//
//				//流程节点添加到全局变量节点
//				GlobalValueNode->InsertEndChild(stationNameNode);
//
//				//保存
//				return doc.SaveFile(FilePath.c_str());
//			}
//			else
//			{
//				//获取流程Id
//				string IdTemp = StationNameNode->Attribute("process");
//				while (IdTemp != to_string(processId))
//				{
//					//获取下一个流程节点
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
//				//isNotExit_Process > 0说明指定的流程节点不存在
//				if (isNotExit_Process > 0)
//				{
//					//添加流程名称节点
//					XMLElement* stationNameNode = doc.NewElement("config");
//					stationNameNode->SetAttribute("process", to_string(processId).c_str());//添加属性Id
//
//
//					//添加模块节点
//					XMLElement* SectionNode = doc.NewElement("node");
//					SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
//					SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID
//
//					//添加子节点，并设置子节点的值
//					XMLElement* keyNode = doc.NewElement("fieldValue");
//					keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//					//子节点添加到父节点
//					SectionNode->InsertEndChild(keyNode);
//
//					//模块节点添加到流程名称节点
//					stationNameNode->InsertEndChild(SectionNode);
//
//					//流程节点添加到全局变量节点
//					GlobalValueNode->InsertEndChild(stationNameNode);
//
//					//保存
//					return doc.SaveFile(FilePath.c_str());
//				}
//				else
//				{
//					//获取第一个模块节点
//					XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
//					if (SectionNode == NULL)
//					{
//						//添加模块节点
//						XMLElement* SectionNode = doc.NewElement("node");
//						SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
//						SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID
//
//						//添加子节点，并设置子节点的值
//						XMLElement* keyNode = doc.NewElement("fieldValue");
//						keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//						//子节点添加到父节点
//						SectionNode->InsertEndChild(keyNode);
//
//						//模块节点添加到流程名称节点
//						StationNameNode->InsertEndChild(SectionNode);
//
//						//流程节点添加到全局变量节点
//						GlobalValueNode->InsertEndChild(StationNameNode);
//
//						//保存
//						return doc.SaveFile(FilePath.c_str());
//					}
//					else
//					{
//						//获取模块名称
//						string nameTemp = SectionNode->Attribute("nodeName");
//						//获取模块Id
//						IdTemp = SectionNode->Attribute("nodeProcessID");
//						while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
//						{
//							//获取下一个模块节点
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
//						//isNotExit_Section > 0说明指定的模块节点不存在
//						if (isNotExit_Section > 0)
//						{
//							//添加模块节点
//							XMLElement* SectionNode = doc.NewElement("node");
//							SectionNode->SetAttribute("nodeName", nodeName.c_str());//添加属性nodeName
//							SectionNode->SetAttribute("nodeProcessID", to_string(processModbuleID).c_str());//添加属性nodeProcessID
//
//							//添加子节点，并设置子节点的值
//							XMLElement* keyNode = doc.NewElement("fieldValue");
//							keyNode->InsertEndChild(doc.NewText(keyName.c_str()));
//
//							//子节点添加到父节点
//							SectionNode->InsertEndChild(keyNode);
//
//							//模块节点添加到流程名称节点
//							StationNameNode->InsertEndChild(SectionNode);
//
//							//流程节点添加到全局变量节点
//							GlobalValueNode->InsertEndChild(StationNameNode);
//
//							//保存
//							return doc.SaveFile(FilePath.c_str());
//						}
//						else
//						{
//							//获取参数节点，判断是否存在
//							XMLElement* KeyNode = SectionNode->FirstChildElement("fieldValue");
//							if (KeyNode == NULL)
//							{
//								//添加子节点，并设置子节点的值
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
//								//子节点添加到父节点
//								SectionNode->InsertEndChild(keyNode);
//
//								//模块节点添加到流程名称节点
//								StationNameNode->InsertEndChild(SectionNode);
//
//								//流程节点添加到全局变量节点
//								GlobalValueNode->InsertEndChild(StationNameNode);
//
//								//保存
//								return doc.SaveFile(FilePath.c_str());
//							}
//							else
//							{
//								//修改子节点的值
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
//								//保存
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



//删除父节点下指定子节点
//return 0 OK,非0 NG
int XmlClass2::DeleteSection_Key(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID, const string& keyName)
{
	try
	{
		//doc.DeleteNode(root);//删除xml所有节点

		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return 1;
		}

		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return 1;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return 1;
		}
		//获取模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		//获取模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//获取下一个模块节点
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return 1;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}

		//获取参数节点
		XMLElement* KeyNode = SectionNode->FirstChildElement(keyName.c_str());
		if (KeyNode == NULL)
		{
			return 1;
		}
		//删除
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
	catch (...)
	{
		return -1;
	}
}
//删除父节点下面的所有子节点
//return 0 OK,非0 NG
int XmlClass2::DeleteSection_AllKeys(const string FilePath, const int& processId, const string& nodeName, const int& processModbuleID)
{
	try
	{
		//读取XML
		XMLDocument doc;
		if (doc.LoadFile(FilePath.c_str()) != 0)
		{
			cout << "load xml file failed" << endl;
			return 1;
		}

		//获取第一个流程节点
		XMLElement* StationNameNode = doc.FirstChildElement("config");
		if (StationNameNode == NULL)
		{
			return 1;
		}

		//获取流程Id
		string IdTemp = StationNameNode->Attribute("process");
		while (IdTemp != to_string(processId))
		{
			//获取下一个流程节点
			StationNameNode = StationNameNode->NextSiblingElement("config");
			if (StationNameNode == NULL)
			{
				return 1;
			}
			IdTemp = StationNameNode->Attribute("process");
		}

		//获取第一个模块节点
		XMLElement* SectionNode = StationNameNode->FirstChildElement("node");
		if (SectionNode == NULL)
		{
			return 1;
		}
		//获取模块名称
		string nameTemp = SectionNode->Attribute("nodeName");
		//获取模块Id
		IdTemp = SectionNode->Attribute("nodeProcessID");
		while (nameTemp != nodeName || IdTemp != to_string(processModbuleID))
		{
			//获取下一个模块节点
			SectionNode = SectionNode->NextSiblingElement("node");
			if (SectionNode == NULL)
			{
				return 1;
			}
			nameTemp = SectionNode->Attribute("nodeName");
			IdTemp = SectionNode->Attribute("nodeProcessID");
		}
		//删除
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
	catch (...)
	{
		return -1;
	}
}
//删除XML全部内容
//return 0 OK,非0 NG
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
