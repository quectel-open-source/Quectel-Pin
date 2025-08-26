#include "rs_motion.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QtCore/qcoreapplication.h>
#include <QtWidgets/qmessagebox.h>
#include <Windows.h> 
#include "LTDMC.h"
#include <thread>
#include <QtCore/qdir.h>
#include "qc_log.h"
#include "PinWindow.h"
#include "Pin3DMeasureWindow.h"
#include "qg_QSLiteWidget.h"
#include "BarCodeWindow.h"
#include "QRCodeWindow.h"
#include "TemplateWindow.h"
#include "BlobWindow.h"
#include "CircleWindow.h"
#include "ThreadGuard.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "ImageTypeConvert.h"
#include "Dmc1000.h"

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <unordered_set>
#include "Pin2.h"
#include "ModbusManager.h"


#define ROBOT 1
#define ERROR_VALUE 2

rs_motion* rs_motion::motion = new rs_motion();

void rs_motion::init()
{
    initAxisAndIO();

    //�������ʼ��
    for (const auto& name : LS_AxisName::allAxis)
    {
        m_Axis[name] = Axis_Config(); 
    }

    //DI������ʼ��
    for (const auto& name : LS_DI::allDI)
    {
        m_DI[name] = DI_Config();
    }

    //DO������ʼ��
    for (const auto& name : LS_DO::allDO)
    {
        m_DO[name] = DO_Config();
    }

    //���ò�����ʼ��
    //for (const auto& name : LS_ParamerName::allParamer)
    //{
    //    m_Paramer[name] = DeviceMoveit_Config();
    //}
    m_Paramer = DeviceMoveit_Config();

    //�����䷽����
    loadFormulaFromFile();
    ////��ʼ��3D���
    //init3dCamera();

    //m_pcMyCamera = std::make_unique<CMvCamera>();

    //������Ŀ����Ķ�Ӧ��ϵ
    switch (m_version)
    {
    case ENUM_VERSION::TwoPin:
    case ENUM_VERSION::LsPin:
    case ENUM_VERSION::BtPin:
        m_cameraMap[LS_CameraName::pin2D] = std::make_shared<CMvCamera>();
        break;
    case ENUM_VERSION::EdPin:
        //m_cameraMap[LS_CameraName::pin2D] = m_pcMyCamera;
        m_cameraMap[LS_CameraName::pin2D] = std::make_shared<CMvCamera>();
        m_cameraMap[LS_CameraName::ed_Up] = std::make_shared<CMvCamera>();
        m_cameraMap[LS_CameraName::ed_Down] = std::make_shared<CMvCamera>();
        m_cameraMap[LS_CameraName::ed_Location] = std::make_shared<CMvCamera>();
        break;
    case ENUM_VERSION::JmPin:
        m_cameraMap[LS_CameraName::pin2D] = std::make_shared<CMvCamera>();
        m_cameraMap[LS_CameraName::side_camera] = std::make_shared<CMvCamera>();
        break;
    case ENUM_VERSION::JmHan:
        m_cameraMap[LS_CameraName::pin2D] = std::make_shared<CMvCamera>();
        break;
    default:
        break;
    }

#if  ROBOT == 1
    //modbusRobot.connectToDevice("192.168.1.12");
#endif

    //��ʼ���Զ����������ݲ���
    initCsvPara();

    //���Ӻ�ɫ����˸
    connect(&m_redLightFlashing, &QTimer::timeout, this, &rs_motion::redLightFlashing_slot);
}

//���ٵ���
void rs_motion::destroyInstance()
{
    initAllVariable();
    motionClose();
    destroyCamera();
    if (motion) {
        delete motion;
        motion = nullptr;
    }

}

bool rs_motion::motionInit()
{
    bool result = false;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        if (d1000_board_init() <= 0)//���ƿ���ʼ��
        {
            machineLog->write(QString::fromStdString("���ƿ������쳣���������ü�Ӳ����"), Normal);
            machineLog->write(QString::fromStdString("���ƿ������쳣���������ü�Ӳ����"), Err);
        }
        else
            result = true;
    }
    else
    {
        WORD wCardNum;      //���忨��
        WORD arrwCardList[8];   //���忨������
        DWORD arrdwCardTypeList[8];   //�����������
        bool b1 = 0;
        if (dmc_board_init() <= 0)      //���ƿ��ĳ�ʼ������
        {    //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("���ƿ������쳣���������ü�Ӳ����"));
            machineLog->write(QString::fromStdString("���ƿ������쳣���������ü�Ӳ����"), Normal);
            machineLog->write(QString::fromStdString("���ƿ������쳣���������ü�Ӳ����"), Err);
        }
        else
            b1 = true;
        dmc_get_CardInfList(&wCardNum, arrdwCardTypeList, arrwCardList);    //��ȡ����ʹ�õĿ����б�
        //m_wCard = arrwCardList[0];

        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
        {

            //bool b2 = CANInit(0, 1);//��ʼ��CAN-IO
            bool b3 = CANInit(0, 2);

            //if (!b2)
            //{
            //    //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO��չ��1�����쳣���������ü�Ӳ����"));
            //    machineLog->write(QString::fromStdString("IO��չ��1�����쳣���������ü�Ӳ����"), Normal);
            //}
            if (!b3)
            {
                //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO��չ��2�����쳣���������ü�Ӳ����"));
                machineLog->write(QString::fromStdString("IO��չ�������쳣���������ü�Ӳ����"), Normal);
            }
            if (b1)
                InitAxes();

            result = b1 && b3;
        }
        else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            if (b1)
                InitAxes();
            result = b1;
        }
    }

    return (result == true);
}

bool rs_motion::CANInit(int CardId, int NodeId)
{
    WORD usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    nmc_get_connect_state(usCardNum, &usCanNum, &usCanStatus);
    if (usCanStatus != 1)
        nmc_set_connect_state(usCardNum, (WORD)NodeId, 1, 0);

    nmc_get_connect_state(usCardNum, &usCanNum, &usCanStatus);

    return (usCanStatus == 1);
}

//��������
void rs_motion::saveMotionConfig()
{
    // ����һ�� JSON ����
    QJsonObject rootObject;

    // ����������
    QJsonObject axisParams;

    for (auto it = m_Axis.begin(); it != m_Axis.end(); ++it) {
        const QString& axisName = it->first;      // ��������
        const Axis_Config& axisConfig = it->second; // ֵ��������

        QJsonObject axisObject;

        // �������õ�ÿ���ֶα��浽 JSON ������
        axisObject["cardNum"] = axisConfig.card;
        axisObject["axisNum"] = axisConfig.axisNum;
        axisObject["initPos"] = axisConfig.initPos;
        axisObject["minTravel"] = axisConfig.minTravel;
        axisObject["maxTravel"] = axisConfig.maxTravel;
        axisObject["homeDir"] = axisConfig.homeDir;
        axisObject["unit"] = axisConfig.unit;
        axisObject["minSpeed"] = axisConfig.minSpeed;
        axisObject["midSpeed"] = axisConfig.midSpeed;
        axisObject["maxSpeed"] = axisConfig.maxSpeed;
        axisObject["accTime"] = axisConfig.accTime;
        axisObject["decTime"] = axisConfig.decTime;
        axisObject["sTime"] = axisConfig.sTime;
        axisObject["stopIO"] = axisConfig.stopIO;

        // ʹ��������Ϊ JSON �ļ�
        axisParams[axisName] = axisObject;
    }

    // ���������ӵ�������
    rootObject["axisParams"] = axisParams;

    //**********************************************************************************************
    // ���� DI ����
    QJsonObject diParams;

    for (auto it = m_DI.begin(); it != m_DI.end(); ++it) {
        const QString& diName = it->first;        // ����DI ����
        const DI_Config& diConfig = it->second;  // ֵ��DI ����

        QJsonObject diObject;

        // �� DI ���õ�ÿ���ֶα��浽 JSON ������
        diObject["cardNum"] = diConfig.card;
        diObject["can"] = diConfig.can;
        diObject["channel"] = diConfig.channel;
        diObject["state"] = diConfig.state;

        // ʹ�� DI ������Ϊ JSON �ļ�
        diParams[diName] = diObject;
    }

    // �� DI ������ӵ�������
    rootObject["diParams"] = diParams;
    //**********************************************************************************************
    // ���� DO ����
    QJsonObject doParams;

    for (auto it = m_DO.begin(); it != m_DO.end(); ++it) {
        const QString& doName = it->first;        
        const DO_Config& dOConfig = it->second;  

        QJsonObject doObject;

        // �� DO ���õ�ÿ���ֶα��浽 JSON �����У�״̬�Ȳ��棬Ӧ���ò��ϣ������ֱ�Ӷ�ȡ��
        doObject["cardNum"] = dOConfig.card;
        doObject["can"] = dOConfig.can;
        doObject["channel"] = dOConfig.channel;
        doObject["level"] = dOConfig.level;

        // ʹ�� DO ������Ϊ JSON �ļ�
        doParams[doName] = doObject;
    }

    // �� DO ������ӵ�������
    rootObject["doParams"] = doParams;

    //**********************************************************************************************

    // ���� ���� ����
    QJsonObject deviceParams;

    deviceParams["BuffetTime2D"] = m_Paramer.BuffetTime2D;
    deviceParams["DelayTriggerTime2D"] = m_Paramer.DelayTriggerTime2D;
    deviceParams["HoldTime2D"] = m_Paramer.HoldTime2D;
    deviceParams["BuffetTime3D"] = m_Paramer.BuffetTime3D;
    deviceParams["DelayTriggerTime3D"] = m_Paramer.DelayTriggerTime3D;
    deviceParams["HoldTime3D"] = m_Paramer.HoldTime3D;
    deviceParams["suctionTime"] = m_Paramer.suctionTime;
    deviceParams["mesUrl"] = m_Paramer.mesUrl;
    deviceParams["ScanRow"] = m_Paramer.ScanRow;
    deviceParams["unLoadTime"] = m_Paramer.unLoadTime;
    deviceParams["modbusTcpIp"] = m_Paramer.modbusTcpIp;
    deviceParams["mesCSV"] = m_Paramer.mesCSV;
    deviceParams["ngPicPath"] = m_Paramer.ngPicPath;
    
    // �� ���� ������ӵ�������
    rootObject["deviceParams"] = deviceParams;

    //**********************************************************************************************

    // ���� ��λ��Ӧ��� ����
    QJsonObject cameraParams;

    //cameraParams["pin2D"] = QString::fromStdString(m_cameraName.pin2D);
    //if (m_version == ENUM_VERSION::EdPin)
    //{
    //    cameraParams["ed_Up"] = QString::fromStdString(m_cameraName.ed_Up);
    //    cameraParams["ed_Down"] = QString::fromStdString(m_cameraName.ed_Down);
    //}

    for (const auto& camera : m_cameraName)
    {
        cameraParams[camera.first] = QString::fromStdString(camera.second);
    }

    // �� ��λ��Ӧ��� ������ӵ�������
    rootObject["cameraParams"] = cameraParams;

    //**********************************************************************************************

    // �� JSON ����ת��Ϊ JSON �ĵ�
    QJsonDocument jsonDoc(rootObject);

    // ��ȡ��ǰ exe �ļ�����Ŀ¼
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/motionConfig.json";

    // д���ļ�
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented)); // ��ʽ��Ϊ���۵� JSON
    file.close();

    qDebug() << "Motion configuration saved to:" << filePath;
}

//������ȡ
void rs_motion::loadMotionConfig()
{
    // ��ȡ��ǰ exe �ļ�����Ŀ¼
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/motionConfig.json";

    // ���ļ�
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return;
    }

    // ��ȡ�ļ�����
    QByteArray jsonData = file.readAll();
    file.close();

    // ���� JSON �ĵ�
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // ��ȡ�������
    if (rootObject.contains("axisParams") && rootObject["axisParams"].isObject()) {
        QJsonObject axisParams = rootObject["axisParams"].toObject();
        for (auto it = axisParams.begin(); it != axisParams.end(); ++it) {
            QString axisName = it.key(); // ��ȡ������
            if (!it.value().isObject()) {
                continue;
            }

            QJsonObject axisObject = it.value().toObject();

            // ��ʼ����ǰ�������
            Axis_Config axisConfig;

            // ��ȡ���ֶ�
            if (axisObject.contains("cardNum") && axisObject["cardNum"].isDouble()) {
                axisConfig.card = axisObject["cardNum"].toInt();
            }

            if (axisObject.contains("axisNum") && axisObject["axisNum"].isDouble()) {
                axisConfig.axisNum = axisObject["axisNum"].toInt();
            }

            if (axisObject.contains("initPos") && axisObject["initPos"].isDouble()) {
                axisConfig.initPos = axisObject["initPos"].toDouble();
            }

            if (axisObject.contains("minTravel") && axisObject["minTravel"].isDouble()) {
                axisConfig.minTravel = axisObject["minTravel"].toDouble();
            }

            if (axisObject.contains("maxTravel") && axisObject["maxTravel"].isDouble()) {
                axisConfig.maxTravel = axisObject["maxTravel"].toDouble();
            }

            if (axisObject.contains("homeDir") && axisObject["homeDir"].isDouble()) {
                axisConfig.homeDir = axisObject["homeDir"].toInt();
            }

            if (axisObject.contains("unit") && axisObject["unit"].isDouble()) {
                axisConfig.unit = axisObject["unit"].toDouble();
            }

            if (axisObject.contains("minSpeed") && axisObject["minSpeed"].isDouble()) {
                axisConfig.minSpeed = axisObject["minSpeed"].toDouble();
            }

            if (axisObject.contains("midSpeed") && axisObject["midSpeed"].isDouble()) {
                axisConfig.midSpeed = axisObject["midSpeed"].toDouble();
            }

            if (axisObject.contains("maxSpeed") && axisObject["maxSpeed"].isDouble()) {
                axisConfig.maxSpeed = axisObject["maxSpeed"].toDouble();
            }

            if (axisObject.contains("accTime") && axisObject["accTime"].isDouble()) {
                axisConfig.accTime = axisObject["accTime"].toDouble();
            }

            if (axisObject.contains("decTime") && axisObject["decTime"].isDouble()) {
                axisConfig.decTime = axisObject["decTime"].toDouble();
            }

            if (axisObject.contains("sTime") && axisObject["sTime"].isDouble()) {
                axisConfig.sTime = axisObject["sTime"].toDouble();
            }

            if (axisObject.contains("stopIO") && axisObject["stopIO"].isDouble()) {
                axisConfig.stopIO = axisObject["stopIO"].toInt();
            }

            // ����������ӵ���ϣ��
            m_Axis[axisName] = axisConfig;
        }
    }

    //***************************************************************************************************************************
    // ��ȡ DI ����
    if (rootObject.contains("diParams") && rootObject["diParams"].isObject()) {
        QJsonObject diParams = rootObject["diParams"].toObject();
        for (auto it = diParams.begin(); it != diParams.end(); ++it) {
            QString diName = it.key(); // ��ȡ DI ����
            if (!it.value().isObject()) {
                continue;
            }

            QJsonObject diObject = it.value().toObject();

            // ��ʼ����ǰ DI ������
            DI_Config diConfig;

            // ��ȡ���ֶ�
            if (diObject.contains("cardNum") && diObject["cardNum"].isDouble()) {
                diConfig.card = diObject["cardNum"].toInt();
            }

            if (diObject.contains("can") && diObject["can"].isDouble()) {
                diConfig.can = diObject["can"].toInt();
            }

            if (diObject.contains("channel") && diObject["channel"].isDouble()) {
                diConfig.channel = diObject["channel"].toInt();
            }

            if (diObject.contains("state") && diObject["state"].isDouble()) {
                diConfig.state = diObject["state"].toInt();
            }

            // �� DI ������ӵ���ϣ��
            m_DI[diName] = diConfig;
        }
    }

    //***************************************************************************************************************************
    // ��ȡ DO ����
    if (rootObject.contains("doParams") && rootObject["doParams"].isObject()) {
        QJsonObject doParams = rootObject["doParams"].toObject();
        for (auto it = doParams.begin(); it != doParams.end(); ++it) {
            QString doName = it.key(); // ��ȡ DO ����

            // ȷ��ֵ��һ�� JSON ����
            if (!it.value().isObject()) {
                qWarning() << "Invalid DO configuration for:" << doName;
                continue; // ������Ч����
            }

            QJsonObject doObject = it.value().toObject();

            // ��ʼ����ǰ DO ������
            DO_Config doConfig;

            // ��ȡ���ֶ�
            if (doObject.contains("cardNum") && doObject["cardNum"].isDouble()) {
                doConfig.card = doObject["cardNum"].toInt();
            }

            if (doObject.contains("can") && doObject["can"].isDouble()) {
                doConfig.can = doObject["can"].toInt();
            }

            if (doObject.contains("channel") && doObject["channel"].isDouble()) {
                doConfig.channel = doObject["channel"].toInt();
            }

            if (doObject.contains("level") && doObject["level"].isDouble()) {
                doConfig.level = doObject["level"].toInt();
            }

            // �� DO ������ӵ���ϣ��
            m_DO[doName] = doConfig;
        }
    }

    //***************************************************************************************************************************
    // ��ȡ ���� ����
    //if (rootObject.contains("deviceParams") && rootObject["deviceParams"].isObject()) {
    //    QJsonObject params = rootObject["deviceParams"].toObject();
    //    for (auto it = params.begin(); it != params.end(); ++it) {
    //        QString name = it.key(); // ��ȡ DO ����

    //        // ȷ��ֵ��һ�� JSON ����
    //        if (!it.value().isObject()) {
    //            qWarning() << "Invalid DO configuration for:" << name;
    //            continue; // ������Ч����
    //        }

    //        QJsonObject object = it.value().toObject();

    //        // ��ʼ����ǰ ���� ������
    //        DeviceMoveit_Config config;

    //        // ��ȡ���ֶ�
    //        if (object.contains("BuffetTime") && object["BuffetTime"].isDouble()) {
    //            config.BuffetTime = object["BuffetTime"].toInt();
    //        }

    //        if (object.contains("DelayTriggerTime") && object["DelayTriggerTime"].isDouble()) {
    //            config.DelayTriggerTime = object["DelayTriggerTime"].toInt();
    //        }

    //        if (object.contains("HoldTime") && object["HoldTime"].isDouble()) {
    //            config.HoldTime = object["HoldTime"].toInt();
    //        }

    //        // �� DO ������ӵ���ϣ��
    //        m_Paramer[name] = config;
    //    }
    //}

    if (rootObject.contains("deviceParams") && rootObject["deviceParams"].isObject())
    {
        QJsonObject params = rootObject["deviceParams"].toObject();
        if (params.contains("BuffetTime2D") && params["BuffetTime2D"].isDouble()) {
            m_Paramer.BuffetTime2D = params["BuffetTime2D"].toInt();
        }
        if (params.contains("DelayTriggerTime2D") && params["DelayTriggerTime2D"].isDouble()) {
            m_Paramer.DelayTriggerTime2D = params["DelayTriggerTime2D"].toInt();
        }
        if (params.contains("HoldTime2D") && params["HoldTime2D"].isDouble()) {
            m_Paramer.HoldTime2D = params["HoldTime2D"].toInt();
        }
        if (params.contains("BuffetTime3D") && params["BuffetTime3D"].isDouble()) {
            m_Paramer.BuffetTime3D = params["BuffetTime3D"].toInt();
        }
        if (params.contains("DelayTriggerTime3D") && params["DelayTriggerTime3D"].isDouble()) {
            m_Paramer.DelayTriggerTime3D = params["DelayTriggerTime3D"].toInt();
        }
        if (params.contains("HoldTime3D") && params["HoldTime3D"].isDouble()) {
            m_Paramer.HoldTime3D = params["HoldTime3D"].toInt();
        }
        if (params.contains("suctionTime") && params["suctionTime"].isDouble()) {
            m_Paramer.suctionTime = params["suctionTime"].toInt();
        }
        if (params.contains("mesUrl")) {
            m_Paramer.mesUrl = params["mesUrl"].toString();
        }
        if (params.contains("ScanRow") && params["ScanRow"].toInt()) {
            m_Paramer.ScanRow = params["ScanRow"].toInt();
        }
        if (params.contains("unLoadTime") && params["unLoadTime"].toInt()) {
            m_Paramer.unLoadTime = params["unLoadTime"].toInt();
        }
        if (params.contains("modbusTcpIp")) {
            m_Paramer.modbusTcpIp = params["modbusTcpIp"].toString();
        }
        if (params.contains("mesCSV")) {
            m_Paramer.mesCSV = params["mesCSV"].toString();
        }
        if (params.contains("ngPicPath")) {
            m_Paramer.ngPicPath = params["ngPicPath"].toString();
        }
    }

    //**********************************************************************************************
    // ��ȡ ��λ��Ӧ��� ����
    if (rootObject.contains("cameraParams") && rootObject["cameraParams"].isObject())
    {
        QJsonObject params = rootObject["cameraParams"].toObject();
        //if (params.contains("pin2D") && params["pin2D"].isString()) {
        //    m_cameraName.pin2D = params["pin2D"].toString().toUtf8().data();
        //    
        //}
        //if (m_version == ENUM_VERSION::EdPin)
        //{
        //    if (params.contains("ed_Up") && params["ed_Up"].isString()) {
        //        m_cameraName.ed_Up = params["ed_Up"].toString().toUtf8().data();
        //    }
        //    if (params.contains("ed_Down") && params["ed_Down"].isString()) {
        //        m_cameraName.ed_Down = params["ed_Down"].toString().toUtf8().data();
        //    }
        //}
        for (const auto& name : LS_CameraName::allCamera)
        {
            if (params.contains(name) && params[name].isString()) {
                m_cameraName[name] = params[name].toString().toUtf8().data();
            }
        }
    }

    //**********************************************************************************************

    qDebug() << "Motion configuration loaded successfully from:" << filePath;
}

//��ȡSEVON�ź� ���ŷ�״̬
bool rs_motion::ReadAxisServeOn(int CardId, int AxisId)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        //���忨(�͵�ƽʹ��ʱ)
        return (dmc_read_sevon_pin((WORD)CardId, (WORD)AxisId) == 0);
    }
    else if(LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        //���߿�
        WORD state = 0;
        short rtn = nmc_get_axis_state_machine((WORD)CardId, (WORD)AxisId,&state);
        if (rtn != 0)
        {
            machineLog->write("���ŷ�״̬ nmc_get_axis_state_machine rtn = " + QString::number(rtn), Machine);
        }
        return (state == ENUM_ETHERCAT_STATE::OP_ENABLE);
    }
}

//���SEVON�ź� ���ŷ�
short rs_motion::WriteAxisServeOn(QString axis, bool isEnable)
{
    return WriteAxisServeOn((WORD)m_Axis[axis].card, (WORD)m_Axis[axis].axisNum, isEnable);
}

//���SEVON�ź�
short rs_motion::WriteAxisServeOn(int CardId, int AxisId, bool isEnable)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        return dmc_write_sevon_pin((WORD)CardId, (WORD)AxisId, isEnable ? (WORD)0 : (WORD)1);
    }
    else if(LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        short rtn = 0;
        if (isEnable)
        {
            rtn = nmc_set_axis_enable((WORD)CardId, (WORD)AxisId);
            if (rtn != 0)
            {
                machineLog->write("ʹ���ŷ� nmc_set_axis_enable rtn = " + QString::number(rtn), Machine);
            }
        }
        else
        {
            rtn = nmc_set_axis_disable((WORD)CardId, (WORD)AxisId);
            if (rtn != 0)
            {
                machineLog->write("ʹ���ŷ� nmc_set_axis_disable rtn = " + QString::number(rtn), Machine);
            }
        }
    }
}

void rs_motion::WriteAxis(int CardId, int AxisId,
    double StartVel, double RunVel,
    double AccTime, double DccTime, double StopVel,
    double STime)
{
    if (AxisId == -1) return;
    WORD card = CardId;
    WORD axis = AxisId;
    dmc_set_profile(card, axis, StartVel, RunVel, AccTime, DccTime, StopVel);//�����ٶȲ���
    dmc_set_s_profile(card, axis, 0, STime);                                    //����S���ٶȲ���
    // LTDMC.dmc_set_home_pin_logic(card, axis, 0, 0);//����ԭ��͵�ƽ��Ч
}

//ԭ���˶����������д������ WriteAxis��Ҳ����һ��
void rs_motion::WriteAxis(QString AxisName)
{
    // ��ȡ�������
    Axis_Config axis = m_Axis[AxisName];
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        // �����ٶȲ���
        dmc_set_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed * axis.unit,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
            axis.minSpeed * axis.unit,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
            axis.accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
            axis.decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
            axis.minSpeed * axis.unit   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
        );

        // ���� S ���ٶȲ���
        dmc_set_s_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0,               // S ���������ͣ�д��Ϊ 0��
            axis.sTime       // S ��ʱ�䣨�滻Ϊ�����е� sTime��
        );

        // ����ԭ����Ч��ƽ
        dmc_set_home_pin_logic(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0, // 0�͵�ƽ��Ч 1�ߵ�ƽ��Ч
            0  // ����������д��Ϊ 0��
        );

        int limitBit = 0;
        if (LSM->m_version == EdPin)
        {
            limitBit = 1;
            if (AxisName == LS_AxisName::U || AxisName == LS_AxisName::U1)
                limitBit = 0;

            ////���ھ���ֵ��������¼λ��
            //dmc_set_home_position((ushort)axis.card, (ushort)axis.axisNum,1, 0);
            //if (rtn != 0)
            //{
            //    machineLog->write(AxisName + " dmc_set_home_position rtn = " + QString::number(rtn), Normal);
            //}
        }
        // ����������λ��Ч��ƽ
        dmc_set_el_mode(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            1, // �Ƿ���������Ӳ��λ��д��Ϊ����
            limitBit, // Ӳ��λ��Ч��ƽ��0�͵�ƽ��Ч��д��Ϊ�͵�ƽ��Ч��
            1  // �ƶ���ʽ��������λ����ֹͣ
        );

        //// ӳ����ļ�ͣ�ź�
        //dmc_set_axis_io_map(
        //    (ushort)axis.card,
        //    (ushort)axis.axisNum,
        //    3,                // �ź����ͣ��ἱͣ�źţ�д��Ϊ 3��
        //    6,                // �� IO ӳ�����ͣ�ͨ������˿ڣ�д��Ϊ 6��
        //    (ushort)axis.stopIO,  // ��ͣ�źŶ�Ӧ�� IO ����
        //    0.01             // �˲�ʱ�䣨д��Ϊ 0.01��
        //);

        //if (LSM->m_version == EdPin)
        //{
        //    // ���ü�ͣģʽ
        //    dmc_set_emg_mode(
        //        (ushort)axis.card,
        //        (ushort)axis.axisNum,
        //        1, // �Ƿ�����ʹ�ü�ͣ�źţ�д��Ϊ���ã�
        //        1  // ��Ч��ƽ��0�͵�ƽ��Ч 1�ߵ�ƽ��Ч��д��Ϊ�ߵ�ƽ��Ч��
        //    );
        //}
        //else
        //{
        //    // ���ü�ͣģʽ
        //    dmc_set_emg_mode(
        //        (ushort)axis.card,
        //        (ushort)axis.axisNum,
        //        1, // �Ƿ�����ʹ�ü�ͣ�źţ�д��Ϊ���ã�
        //        RS2::IOStateEnum::High  // ��Ч��ƽ��0�͵�ƽ��Ч 1�ߵ�ƽ��Ч��д��Ϊ�ߵ�ƽ��Ч��
        //    );
        //}

        MapEmgIO(axis.card, axis.axisNum, axis.stopIO, RS2::IOStateEnum::High);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        if (LSM->m_version == LsPin && AxisName == LS_AxisName::U)
        {
            rtn = nmc_set_offset_pos((ushort)axis.card,(ushort)axis.axisNum, 0);
            if (rtn != 0)
            {
                machineLog->write("u nmc_set_offset_pos rtn = " + QString::number(rtn), Machine);
            }
        }
        else if (LSM->m_version != LsPin)
        {
            //ȫ�����ϵ��籣��
            rtn = nmc_set_offset_pos((ushort)axis.card, (ushort)axis.axisNum, 0);
            if (rtn != 0)
            {
                machineLog->write("u nmc_set_offset_pos rtn = " + QString::number(rtn), Machine);
            }
        }
        //����ָ��������嵱��
        rtn = dmc_set_equiv(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.unit   
            );
        if (rtn != 0)
        {
            machineLog->write("����ָ��������嵱�� dmc_set_equiv rtn = " + QString::number(rtn), Machine);
        }
        // �����ٶȲ���
        rtn = dmc_set_profile_unit(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
            axis.maxSpeed,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
            axis.accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
            axis.decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
            axis.minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
        );
        if (rtn != 0)
        {
            machineLog->write("�����ٶȲ��� dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
        }

        // ���� S ���ٶȲ���
        rtn = dmc_set_s_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0,               // S ���������ͣ�д��Ϊ 0��
            axis.sTime       // S ��ʱ�䣨�滻Ϊ�����е� sTime��
        );
        if (rtn != 0)
        {
            machineLog->write("���� S ���ٶȲ��� dmc_set_s_profile rtn = " + QString::number(rtn), Machine);
        }

        //���û�ԭ�����
        //����ģʽĬ�Ϸ���
        //WORD homeMode = 27;
        ////if (axis.homeDir == 1)
        ////    homeMode = 18;
        //rtn = nmc_set_home_profile(
        //    WORD(axis.card),
        //    WORD(axis.axisNum),
        //    homeMode,
        //    axis.minSpeed,
        //    axis.midSpeed,
        //    axis.accTime,
        //    axis.decTime,
        //    0   //����ƫ����Ĭ��0
        //);
        //if (rtn != 0)
        //{
        //    machineLog->write("���û�ԭ����� nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
        //}

        // ӳ����ļ�ͣ�ź�
        rtn = dmc_set_axis_io_map(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            3,                // �ź����ͣ��ἱͣ�źţ�д��Ϊ 3��
            6,                // �� IO ӳ�����ͣ�ͨ������˿ڣ�д��Ϊ 6��
            (ushort)axis.stopIO,  // ��ͣ�źŶ�Ӧ�� IO ����
            0.01             // �˲�ʱ�䣨д��Ϊ 0.01��
        );
        if (rtn != 0)
        {
            machineLog->write("ӳ����ļ�ͣ�ź� dmc_set_axis_io_map rtn = " + QString::number(rtn), Machine);
        }

        // ���ü�ͣģʽ
        rtn = dmc_set_emg_mode(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            1, // �Ƿ�����ʹ�ü�ͣ�źţ�д��Ϊ���ã�
            RS2::IOStateEnum::High  // ��Ч��ƽ��0�͵�ƽ��Ч 1�ߵ�ƽ��Ч��д��Ϊ�ߵ�ƽ��Ч��
        );
        if (rtn != 0)
        {
            machineLog->write("���ü�ͣģʽ dmc_set_emg_mode rtn = " + QString::number(rtn), Machine);
        }
    }
}

//д�����
void rs_motion::WriteAxisParm(QString Axis)
{
    Axis_Config config = m_Axis[Axis];
    bool SERON = ReadAxisServeOn(config.card, config.axisNum);
    //machineLog->write(Axis + " WriteAxisParm SERON = " + QString::number(SERON), Normal);
    if (SERON)
    {
        WriteAxisServeOn(config.card, config.axisNum, false);
    }
    WriteAxis(Axis);
    if(SERON)
        WriteAxisServeOn(config.card, config.axisNum, true);
    //machineLog->write(Axis + " �����óɹ�", Normal);
}

//��ͣIO����
void rs_motion::MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel)
{
    short rtn = 0;
    //ӳ����ļ�ͣ�ź�
    rtn = dmc_set_axis_io_map((ushort)CardId, (ushort)AxisId,
        3,//�ź����ͣ��ἱͣ�ź�
        6,//��IOӳ������:ͨ������˿�
        (ushort)IOindex,//IO����
        0.01);//�˲�ʱ��
    if (rtn != 0)
    {
        machineLog->write("MapEmgIO ӳ����ļ�ͣ�ź� dmc_set_axis_io_map rtn = " + QString::number(rtn), Machine);
    }
    rtn = dmc_set_emg_mode((ushort)CardId, (ushort)AxisId,
        1,//����ʹ�ü�ͣ�ź�
        validlevel);//��Ч��ƽ
    if (rtn != 0)
    {
        machineLog->write("MapEmgIO ���ü�ͣģʽ dmc_set_emg_mode rtn = " + QString::number(rtn), Machine);
    }
}


void rs_motion::InitAxes()
{
    for(auto axisName : LS_AxisName::allAxis)
    {
        Axis_Config config = m_Axis[axisName];
        WriteAxisParm(axisName);
        //if (m_version == EdPin)
        //{
        //    MapEmgIO(config.card, config.axisNum, config.stopIO, RS2::IOStateEnum::Low);
        //}
        //else
        //{
        //    MapEmgIO(config.card, config.axisNum, config.stopIO, RS2::IOStateEnum::High);
        //}
        MapEmgIO(config.card, config.axisNum, config.stopIO, RS2::IOStateEnum::High);
        Sleep(5);
        WriteAxisServeOn(config.card, config.axisNum, true);
    }
}

//��ȡ��������di�ź�
void rs_motion::getAllDI()
{
    //���DI�ź�
    for (const auto& name : LS_DI::allDI)
    {
        auto& diInfo = m_DI[name];
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        {
            auto value = d1000_in_bit(diInfo.channel);
            diInfo.state = value == 0 ? 1 : 0;
        }
        else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && diInfo.can > 0)
        {
            //���忨��չģʽ
            ushort value = 0;
            nmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.can, (ushort)diInfo.channel, &value);
            diInfo.state = value == 0 ? 1 : 0;
            //diInfo.state = value;
        }
        else
        {

            //���屾�ؿ�/���߿�
            short value = 0;
            value = dmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.channel);
            diInfo.state = value == 0 ? 1 : 0;
            //if (m_version == LsPin)
            //    diInfo.state = value == 0 ? 1:0;
            //else
            //    diInfo.state = value;
        }
    }
}

//��ȡ����DI
int rs_motion::getDI(QString name)
{
    auto& diInfo = m_DI[name];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        auto value = d1000_in_bit(diInfo.channel);
        diInfo.state = value == 0 ? 1 : 0;
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && diInfo.can > 0)
    {
        //���忨��չģʽ
        ushort value = 0;
        nmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.can, (ushort)diInfo.channel, &value);
        //diInfo.state = value;
        diInfo.state = value == 0 ? 1 : 0;
    }
    else
    {
        //���屾�ؿ�/���߿�
        short value = 0;
        value = dmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.channel);
        //if (m_version == LsPin)
        //    diInfo.state = value == 0 ? 1 : 0;
        //else
        //    diInfo.state = value;

        diInfo.state = value == 0 ? 1 : 0;
    }

    return diInfo.state;
}


//��ȡ��������do�ź�
void rs_motion::getAllDO()
{
    //���DO�ź�
    for (const auto& name : LS_DO::allDO)
    {
        auto& doInfo = m_DO[name];
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        {
            auto value = d1000_get_outbit(doInfo.channel);
            doInfo.state = doInfo.level ? value : (1 - value);
        }
        else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && doInfo.can > 0)
        {
            //���忨��չģʽ
            ushort value = 0;
            nmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.can, (ushort)doInfo.channel, &value);
            //doInfo.state = value;
            //�ߵ͵�ƽת��
            doInfo.state = doInfo.level ? value : (1 - value);
        }
        else
        {
            //���屾�ؿ�/���߿�
            short value = 0;
            value = dmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.channel);
            //�����������״̬
            if (value != 0 && value != 1)
                return;
            //doInfo.state = value;
            //�ߵ͵�ƽת��
            doInfo.state = doInfo.level ? value : (1 - value);

        }
    }
}

//��ȡ����DO
int rs_motion::getDO(QString name)
{
    auto& doInfo = m_DO[name];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        auto value = d1000_get_outbit(doInfo.channel);
        doInfo.state = doInfo.level ? value : (1 - value);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && doInfo.can > 0)
    {
        //���忨��չģʽ
        ushort value = 0;
        nmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.can, (ushort)doInfo.channel, &value);
        //doInfo.state = value;
        //�ߵ͵�ƽת��
        doInfo.state = doInfo.level ? value : (1 - value);

    }
    else
    {
        //���屾�ؿ�/���߿�
        short value = 0;
        value = dmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.channel);
        //�����������״̬
        if (value != 0 && value != 1)
            return doInfo.state;
        //doInfo.state = value;
        //�ߵ͵�ƽת��
        doInfo.state = doInfo.level ? value : (1 - value);

    }
    return doInfo.state;
}

//д�뵥��DO value����Ч������Ч����˼
void rs_motion::setDO(QString name, int value)
{
    // ���DO�Ƿ����
    if (m_DO.find(name) == m_DO.end()) {
        return;
    }

    auto& doInfo = m_DO[name];
    const bool isTrigger = (value == 1);
    doInfo.state = value;
    //�ߵ�ƽʹ������ֵ���͵�ƽȡ��
    const int outputLevel = isTrigger ? doInfo.level : (1 - doInfo.level);

    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_out_bit(doInfo.channel, outputLevel);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && doInfo.can > 0) {
        // ���忨��չģʽ��ʹ��nmc_write_outbit
        nmc_write_outbit(
            static_cast<ushort>(doInfo.card),
            static_cast<ushort>(doInfo.can),
            static_cast<ushort>(doInfo.channel),
            outputLevel
        );
    }
    else {
        // ���ؿ�/���߿�ͳһ����ʹ��dmc_write_outbit
        dmc_write_outbit(
            static_cast<ushort>(doInfo.card),
            static_cast<ushort>(doInfo.channel),
            outputLevel
        );
    }

    
   
    //machineLog->write(QString::fromStdString("do name : ") + name + QString::fromStdString(" = ") + QString::number(value), Normal);
}

//�رհ忨
void rs_motion::motionClose()
{
    ////m_isLoop = false;
    ////m_isPause = false;
    ////m_isEmergency = false;
    for(auto axis : LS_AxisName::allAxis)
    {
       AxisStop(axis); //ֹͣ������
       //WriteAxisServeOn(axis, false);   //��ʹ��
    }

    //������
    if(LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        d1000_board_close();
    else
    {
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
        {
            CANClose(1);
            CANClose(2);
        }
        dmc_board_close();
    }
}

//��ֹͣ
void rs_motion::AxisStop(QString axis)
{
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return;
    }
    AxisStop(m_Axis[axis].card, m_Axis[axis].axisNum);
}

//��ֹͣ
void rs_motion::AxisStop(int CardId, int AxisId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_decel_stop(AxisId);
    }
    else {
        dmc_stop((ushort)CardId, (ushort)AxisId, 0);// 0:����ֹͣ��1������ֹͣ
    }

}

//�����ἱͣ
void rs_motion::AxisEmgStop(int CardId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        for (auto axis : m_Axis)
        {
            d1000_Immediate_stop(axis.second.axisNum);
        }
    }
    else
    {
        dmc_emg_stop((ushort)CardId);
    }

}

//�ر�CAN��
void rs_motion::CANClose(int CardId)
{
    ushort usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    dmc_get_can_state(usCardNum, & usCanNum, & usCanStatus);

    //usCanNum ��Χ 1-8
    for (ushort i = usCanNum; i < usCanNum + 1; i++)
    {
        nmc_set_connect_state(usCardNum, i, 0, 0);//����CANͨѶ״̬���Ͽ� 
    }
}

//��������
int rs_motion::AxisHome(QString axis)
{
    short rtn = 0;
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    if (m_version == TwoPin)
    {
        // ���û�ԭģʽ
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // ���㷽��0���� 1�����滻Ϊ�����е� homeDir��
            0,                      // �����ٶ�ģʽ��0���� 1���٣�д��Ϊ���٣�
            1,                      // ����ģʽ��д��Ϊģʽ 1��
            2                       // ����������д��Ϊ 2��
        );

        // ���û��㵽��λ�Ƿ���
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0������ 1���ң�д��Ϊ���÷��ң�
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHome", Global);
        ////����λ�� �ͱ�����
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        //machineLog->write(axis + " is AxisClearPosition", Global);
    }
    else if (m_version == LsPin)
    {

        if (axis == LS_AxisName::LS_U)
        {           
            //��ת���ò�ͬ�Ļ��㷽ʽ
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(" U���ԭ���ٶ����ô���", Normal);
            rtn = PMove_abs(axis, 0);
            if (rtn != 0)
                machineLog->write(" U���ԭ�����", Normal);
            //Sleep(10);
        }
        else
        {
            WORD homeMode = 27;
            if (axis == LS_AxisName::LS_Y2)
            {
                homeMode = 17;
            }

            //if (axis.homeDir == 1)
            //    homeMode = 18;
            auto rtn = nmc_set_home_profile(
                WORD(m_Axis[axis].card),
                WORD(m_Axis[axis].axisNum),
                homeMode,
                m_Axis[axis].minSpeed,
                m_Axis[axis].midSpeed,
                m_Axis[axis].accTime,
                m_Axis[axis].decTime,
                0   //����ƫ����Ĭ��0
            );
            if (rtn != 0)
            {
                machineLog->write("���û�ԭ����� nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
            }

            rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
            machineLog->write(axis + " is AxisHome", Global);
        }
        //����λ�� �ͱ�����
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisClearPosition", Global);
    }
    else if (m_version == EdPin)
    {
        // ���û�ԭģʽ
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // ���㷽��0���� 1�����滻Ϊ�����е� homeDir��
            0,                      // �����ٶ�ģʽ��0���� 1���٣�д��Ϊ���٣�
            1,                      // ����ģʽ��д��Ϊģʽ 1��
            2                       // ����������д��Ϊ 2��
        );

        // ���û��㵽��λ�Ƿ���
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0������ 1���ң�д��Ϊ���÷��ң�
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        if (rtn != 0)
        {
            machineLog->write(axis + " ��ԭ�����", Normal);
        }
    }
    else if (m_version == BtPin)
    {
        if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        {
            int dir = m_Axis[axis].homeDir == 0 ? -1 : 1;
            rtn = d1000_home_move(m_Axis[axis].axisNum, m_Axis[axis].minSpeed * m_Axis[axis].unit, dir * m_Axis[axis].minSpeed * m_Axis[axis].unit, 0.1);
        }
    }
    else
    {
        //-------------------------- �������Ŀ���������ETherCat�ģ�ֱ��ȫ���������������ԭ�㣬Ȼ���������λ�õ�0�� ------------------------------
        switch (m_motionVersion)
        {
        case LS_EtherCat:
        {
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(axis + " ���ԭ���ٶ����ô���", Normal);
            rtn = PMove_abs(axis, 0);
            if (rtn != 0)
                machineLog->write(axis + " ���ԭ�����", Normal);
            break;
        }
        default:
            break;
        }
    }

    return rtn;
}

//��������(����)
int rs_motion::AxisHomeAsync(QString axis)
{
    short rtn = 0;
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    if (LSM->m_version == TwoPin)
    {
        // ���û�ԭģʽ
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // ���㷽��0���� 1�����滻Ϊ�����е� homeDir��
            0,                      // �����ٶ�ģʽ��0���� 1���٣�д��Ϊ���٣�
            1,                      // ����ģʽ��д��Ϊģʽ 1��
            2                       // ����������д��Ϊ 2��
        );

        // ���û��㵽��λ�Ƿ���
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0������ 1���ң�д��Ϊ���÷��ң�
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync", Global);
        ///�ȴ����˶����
        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            machineLog->write(axis + " is AxisHomeAsync IsRuning", Global);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        };
        //����λ�� �ͱ�����
        AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync AxisClearPosition", Global);
    }
    else if (LSM->m_version == LsPin)
    {

        if (axis == LS_AxisName::LS_U)
        {
            //��ת���ò�ͬ�Ļ��㷽ʽ
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(" U���ԭ���ٶ����ô���", Normal);
            rtn = PMove_abs(axis, 0);
            if(rtn != 0 )
                machineLog->write(" U���ԭ�����", Normal);
        }
        else
        {
            WORD homeMode = 27;
            if (axis == LS_AxisName::LS_Y2)
            {
                homeMode = 17;
            }

            //if (axis.homeDir == 1)
            //    homeMode = 18;
            auto rtn = nmc_set_home_profile(
                WORD(m_Axis[axis].card),
                WORD(m_Axis[axis].axisNum),
                homeMode,
                m_Axis[axis].minSpeed,
                m_Axis[axis].midSpeed,
                m_Axis[axis].accTime,
                m_Axis[axis].decTime,
                0   //����ƫ����Ĭ��0
            );
            if (rtn != 0)
            {
                machineLog->write("���û�ԭ����� nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
            }

            rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);

        }
        machineLog->write(axis + " is AxisHomeAsync", Global);
        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            machineLog->write(axis + " is AxisHomeAsync IsRuning", Global);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

        };
        //����λ�� �ͱ�����
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync AxisClearPosition", Global);
        
    }
    else if (LSM->m_version == EdPin)
    {
        // ���û�ԭģʽ
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // ���㷽��0���� 1�����滻Ϊ�����е� homeDir��
            0,                      // �����ٶ�ģʽ��0���� 1���٣�д��Ϊ���٣�
            1,                      // ����ģʽ��д��Ϊģʽ 1��
            2                       // ����������д��Ϊ 2��
        );

        // ���û��㵽��λ�Ƿ���
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0������ 1���ң�д��Ϊ���÷��ң�
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        if (rtn != 0)
        {
            machineLog->write(axis + " ��ԭ�����", Normal);
        }
        //machineLog->write(axis + " is AxisHomeAsync", Normal);

        ///�ȴ����˶����
        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        };
        AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
    }
    else if (LSM->m_version == BtPin)
    {
        rtn = d1000_home_move(m_Axis[axis].axisNum, m_Axis[axis].minSpeed, -m_Axis[axis].minSpeed, 0.1);
        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            machineLog->write(axis + " is AxisHomeAsync IsRuning", Global);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

        };
    }
    else
    {
	    //-------------------------- �������Ŀ���������ETherCat�ģ�ֱ��ȫ���������������ԭ�㣬Ȼ���������λ�õ�0�� ------------------------------
	    switch (m_motionVersion)
	    {
	    case LS_EtherCat:
	    {
	    	rtn = setSpeed(axis, m_Axis[axis].minSpeed);
	    	if (rtn != 0)
	    		machineLog->write(axis + " ���ԭ���ٶ����ô���", Normal);
	    	rtn = PMove_abs(axis, 0);
	    	if (rtn != 0)
	    		machineLog->write(axis + " ���ԭ�����", Normal);
	    	break;
	    }
	    default:
	    	break;
	    }

        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        };
    }
    return rtn;
}

//��������
int rs_motion::GoHome(int CardId, int AxisId)
{
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = dmc_home_move((ushort)CardId, (ushort)AxisId);
        if (rtn != 0)
            machineLog->write("�������� dmc_home_move rtn = " + QString::number(rtn), Normal);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = nmc_home_move((ushort)CardId, (ushort)AxisId);
        if(rtn != 0)
            machineLog->write("�� "+ QString::number(AxisId) + "�������� nmc_home_move rtn = " + QString::number(rtn), Normal);
    }
    return rtn;
}

//����λ�� �ͱ�����
void rs_motion::AxisClearPosition(int CardId, int AxisId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        dmc_set_position((ushort)CardId, (ushort)AxisId, 0);
        dmc_set_encoder((ushort)CardId, (ushort)AxisId, 0);
    }
    else if (m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        dmc_set_position_unit((ushort)CardId, (ushort)AxisId, 0);
        dmc_set_encoder_unit((ushort)CardId, (ushort)AxisId, 0);
    }
    else if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_set_command_pos(AxisId, 0);
    }
}

// ���ָ������˶�״̬������ true ��ʾ�������У�false ��ʾ��ֹͣ
bool rs_motion::IsRuning(int CardId, int AxisId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        return (d1000_check_done(AxisId) == 0);
    else
        return (dmc_check_done((ushort)CardId, (ushort)AxisId) == 0);
}

//ֹͣ������
void rs_motion::AxisStopAll()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis);
    }
}

//�����˶� 0:������1��������
int rs_motion::VMove(int CardId, int AxisId, bool pdir, double speed)
{
    ushort dir = 0;
    if (pdir)
        dir = 1;
    int rtn = 0;
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_start_sv_move(AxisId, speed, speed * dir, 0.1);
    }
    else
    {
        auto rtn = dmc_vmove((ushort)CardId, (ushort)AxisId, dir);
    }
    return rtn;
}

//�����˶� 0:������1��������
int rs_motion::AxisVmove(QString axis, bool postive , double speed)
{
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    int rtn = 0;
    if(m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        rtn = VMove(m_Axis[axis].card, m_Axis[axis].axisNum, postive, speed * m_Axis[axis].unit);
    else
        rtn = VMove(m_Axis[axis].card, m_Axis[axis].axisNum, postive);
    if (rtn != 0)
        machineLog->write(axis + " �㶯���󣬴����� = " + QString::number(rtn), Normal);
    return rtn;
}

// ʵʱ�������ٶ�(���߱���)
void rs_motion::WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed)
{
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = dmc_change_speed((ushort)CardId, (ushort)AxisId, RealSpeed, 0);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = dmc_change_speed_unit((ushort)CardId, (ushort)AxisId, RealSpeed, 0.01);
        if (rtn != 0)
            machineLog->write("���߱��� dmc_change_speed_unit rtn = " + QString::number(rtn), Machine);
    }
}

// �������ٶ�(���߱���)
void rs_motion::WriteAxisRunVel(QString axis, double runvel)
{
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return;
    }
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        WriteAxisRealSpeed(m_Axis[axis].card, m_Axis[axis].axisNum, runvel * m_Axis[axis].unit);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        WriteAxisRealSpeed(m_Axis[axis].card, m_Axis[axis].axisNum, runvel);
    }
}

//������/������λ��
double rs_motion::ReadPosition(int CardId, int AxisId)
{
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        return (dmc_get_position((ushort)CardId, (ushort)AxisId));   
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        double pos = 0;
        //����λ��
        //rtn = dmc_get_position_unit((ushort)CardId, (ushort)AxisId, &pos);
        //������λ��
        rtn = dmc_get_encoder_unit((ushort)CardId, (ushort)AxisId, &pos);
        if (rtn != 0)
            machineLog->write("������/����λ�� dmc_get_position_unit rtn = " + QString::number(rtn), Machine);
        return pos;
    }
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        return (d1000_get_command_pos(AxisId));
        //return (dmc_get_encoder((ushort)CardId, (ushort)AxisId));

    }
    
}

//��ָ����λ��
double rs_motion::ReadAxisP(QString axis)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum) / m_Axis[axis].unit);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        //�����ͷ��صľ�������λ�ã����ó�����
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum));
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum) / m_Axis[axis].unit);
    }
}

//��������λ��
void rs_motion::ReadAllAxisP()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        m_Axis[axis].position = std::round(ReadAxisP(axis) * 100.0) / 100;
    }
}

// ��������ٶ�ģʽ(���߱���)
void rs_motion::WriteAxisSpeedModel(QString axis, SpeedModeEnum model)
{
    // ��������Ƿ����
    if (m_Axis.find(axis) == m_Axis.end()) {
        return;
    }
    double setspeed = m_Axis[axis].minSpeed;
    switch (model)
    {
    case SpeedModeEnum::Low:
        setspeed = m_Axis[axis].minSpeed;
        break;
    case SpeedModeEnum::Mid:
        setspeed = m_Axis[axis].midSpeed;
        break;
    case SpeedModeEnum::High:
        setspeed = m_Axis[axis].maxSpeed;
        break;
    default:
        break;
    }

    WriteAxisRunVel(axis, setspeed);
    //machineLog->write("�ٶ�ģʽ ��" + QString::number(setspeed), Normal);
}

//�����ŷ�����״̬
bool rs_motion::ReadServOn(QString axis)
{
   bool res = ReadAxisServeOn(m_Axis[axis].card, m_Axis[axis].axisNum);
   return res;
}

// �ָ��������ٶ� �ڲ�����ʹ�ܺ���ʹ��
void rs_motion::ResetSetAxisParm(QString axis)
{
    auto SERON = ReadServOn(axis);
    //machineLog->write(axis + " ResetSetAxisParm SERON = " + QString::number(SERON), Normal);
    if (SERON)
        WriteAxisServeOn(axis, false);
    //ֱ�����в����������
    WriteAxis(axis);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (SERON) 
        WriteAxisServeOn(axis, true);
}

//�����ٶ�
int rs_motion::setSpeed(QString axisName, double speed)
{
    short rtn = 0;
    const auto& axis = m_Axis[axisName];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        // �����ٶȲ���
        rtn = dmc_set_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed * axis.unit,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
            speed * axis.unit,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
            axis.accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
            axis.decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
            axis.minSpeed * axis.unit   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
        );
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        // �����ٶȲ���
        rtn = dmc_set_profile_unit(
        (ushort)axis.card,
        (ushort)axis.axisNum,
        axis.minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
        speed,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
        axis.accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
        axis.decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
        axis.minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
        );
    }
    return rtn;
}

// ��ȡ���Ƿ񱨾�״̬
bool rs_motion::isAxisErc(QString axis) {
    bool res = isAxisErc(m_Axis[axis].card, m_Axis[axis].axisNum);
    return res;
}

// ��ȡ�ᱨ���˿ڵ�ƽ
bool rs_motion::isAxisErc(int CardId, int AxisId)
{
    bool res = false;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        res = (dmc_read_erc_pin((ushort)CardId, (ushort)AxisId) == 0);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        WORD state = 0;
        short rtn = nmc_get_axis_errcode((WORD)CardId, (WORD)AxisId, &state);
        if (rtn != 0)
        {
            machineLog->write("��ȡ�ᱨ�� nmc_get_axis_errcode rtn = " + QString::number(rtn), Machine);
        }
        if (state != 0)
        {
            //����
            res = true;
        }
    }
    return res;
}

// ����ᱨ��״̬
void rs_motion::AxisClear(QString axis)
{
    AxisClear(m_Axis[axis].card, m_Axis[axis].axisNum);
}

// ����ᱨ��״̬
void rs_motion::AxisClear(int CardId, int AxisId)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 0);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        ushort errorcode = 0;
        //������߱���
        nmc_get_errcode((ushort)CardId, 2, &errorcode);
        if (errorcode != 0)
        {
            nmc_clear_errcode((ushort)CardId ,2);
        }
        ushort errorcodeAxis = 0;
        //����ᱨ��
        nmc_get_axis_errcode((ushort)CardId, (ushort)AxisId, &errorcodeAxis);
        if (errorcodeAxis != 0)
        {
            nmc_clear_axis_errcode((ushort)CardId, (ushort)AxisId);
        }
    }
}

// ���������˶�
int rs_motion::PMove_abs(int CardId, int AxisId, double value)
{
    int rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = dmc_pmove((ushort)CardId, (ushort)AxisId, long(value), 1); //0���������ģʽ��1����������ģʽ
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = dmc_pmove_unit((ushort)CardId, (ushort)AxisId, value, 1);
    }
   
   return rtn;
}

// ���������˶� ���ᣩ
int rs_motion::PMove_abs(QString axis, double value, double speed)
{
    int rtn = 0;
    auto& config = m_Axis[axis];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = PMove_abs((ushort)config.card, (ushort)config.axisNum, value * config.unit);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = PMove_abs((ushort)config.card, (ushort)config.axisNum, value);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        rtn = d1000_set_pls_outmode(config.axisNum, 0);//pulse/dir ģʽ��������������Ч
        rtn = d1000_start_s_move(config.axisNum, value * config.unit, speed * config.unit, speed * config.unit, 0.1);
    }

    return rtn;
}

// ��������˶�
void rs_motion::PMove_rel(int CardId, int AxisId, double value)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        dmc_pmove((ushort)CardId, (ushort)AxisId, long(value), 0); //0���������ģʽ��1����������ģʽ
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        dmc_pmove_unit((ushort)CardId, (ushort)AxisId, value, 0);
    }
}

// ��������˶� ���ᣩ
void rs_motion::PMove_rel(QString axis, double value)
{
    auto& config = m_Axis[axis];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        PMove_rel((ushort)config.card, (ushort)config.axisNum, value * config.unit);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        PMove_rel((ushort)config.card, (ushort)config.axisNum, value);
    }
}

// �ᶨ�������˶�������ִ�У����ᵽλ�󷵻�
int rs_motion::AxisPmoveAsync(QString axis, double value, double speed)
{
    //int dis = (int)(value * m_Axis[axis].unit);
    //machineLog->write(axis + " value = " + QString::number(value) + " unit = " + QString::number(m_Axis[axis].unit) + " dis = "+ QString::number(dis), Normal);
    int rtn = PMove_abs(axis, value,speed);
    if (rtn != 0)
        return rtn;
    ///�ȴ����˶����
    while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum))
    {
        if(m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return 0;
}

// �ȴ��Ḵλ����ʼλ�ú󷵻�(����)
int rs_motion::AxisInitPAsync(QString axis)
{
    int rtn = PMove_abs(axis, m_Axis[axis].initPos);
    if (rtn != 0)
        return rtn;
    while (true)
    {
        if (fabs(ReadAxisP(axis) - m_Axis[axis].initPos) < ERROR_VALUE)
            break;
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return 0;
}

// �ᶨ�������˶�
int rs_motion::AxisPmove(QString axis, double value, double speed)
{
    int rtn = PMove_abs(axis, value, speed);
    return rtn;
}

// �Ḵλ����ʼλ��
int rs_motion::AxisInitP(QString axis)
{
    int rtn = 0;
    //int rtn = PMove_abs(m_Axis[axis].card, m_Axis[axis].axisNum, (int)m_Axis[axis].initPos * m_Axis[axis].unit);
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        rtn = PMove_abs(axis, m_Axis[axis].initPos, m_Axis[axis].minSpeed);
    }
    else
    {
        rtn = PMove_abs(axis, m_Axis[axis].initPos);
    }

    machineLog->write(axis + "is AxisInitP", Normal);
    return rtn;
}

//  ��λ �������״̬
int rs_motion::TaskReset()
{
    //����������ᱨ�� 
    for (auto axis : LS_AxisName::allAxis)
    {
        //�������
        AxisClear(axis);
        //����Ϊ��λ�ٶ�
        ResetSetAxisParm(axis);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        WriteAxisServeOn(axis, true);
    }

    //�����ȼӽ�λ
    setDO(LS_DO::DO_ClampCylinderL1, 1);
    setDO(LS_DO::DO_ClampCylinderL2, 1);
    setDO(LS_DO::DO_ClampCylinderR1, 1);
    setDO(LS_DO::DO_ClampCylinderR2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.Z);  //  Z  RZ �ȴ�����
    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.RZ);

    //  Z  RZ �ȴ�����
    if (AxisHomeAsync(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHomeAsync(LS_AxisName::RZ) != 0)
        return -1;

    AxisInitP(LS_AxisName::RZ); //ͬʱRZ �������λ   X U1 U2 ͬʱ����  

    //std::vector<std::future<int>> futures;

    //// �첽ִ��ÿ����Ļ���ͳ�ʼ��
    ////U1���� ����λ
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::U1));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::U1));

    ////U2���� ����Ϊ
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::U2));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::U2));

    ////X���첽 ��ԭ ������λ
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::X));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::X));
    //U1����
    //250717 �Ż���λU1��
    if (AxisHome(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 ���㱨��", Normal);
        return -1;
    }
    ////U2����
    //if (AxisHome(LS_AxisName::U2) != 0)
    //{
    //    machineLog->write("U2 ���㱨��", Normal);
    //    return -1;
    //}
    //X�����
    if (AxisHome(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���㱨��", Normal);
        return -1;
    }
    ////�ȴ����첽��λ
    //while (!isHomeP(LS_AxisName::U1)
    //    || !isHomeP(LS_AxisName::U2)
    //    || !isHomeP(LS_AxisName::X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    
    //250717 �Ż���λU1��
    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::U1)
        || isRuning(LS_AxisName::X))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::U1].card, m_Axis[LS_AxisName::U1].axisNum);
    //AxisClearPosition(m_Axis[LS_AxisName::U2].card, m_Axis[LS_AxisName::U2].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);


    ////�ȴ����첽��λ
    //while (isRuning(LS_AxisName::X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    //AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);


    //U1������λ
    if (AxisInitP(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 ���ϱ���", Normal);
        return -1;
    }
    ////U2������λ
    //if (AxisInitP(LS_AxisName::U2) != 0)
    //{
    //    machineLog->write("U2 ���ϱ���", Normal);
    //    return -1;
    //}
    //X�ᵽ����λ
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���ϱ���", Normal);
        return -1;
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //Y1 Y2��ͬʱ����
    if (AxisHome(LS_AxisName::Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Y2) != 0)
        return -1;
    ////�ȴ����첽��λ
    //while (!isHomeP(LS_AxisName::Y1)
    //    || !isHomeP(LS_AxisName::Y2))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    while (isRuning(LS_AxisName::Y1)
        || isRuning(LS_AxisName::Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::Y1].card, m_Axis[LS_AxisName::Y1].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y2].card, m_Axis[LS_AxisName::Y2].axisNum);

    //���㵽λ��ͬʱ����ʼλ Y1 Y2
    AxisInitP(LS_AxisName::Y1);
    AxisInitP(LS_AxisName::Y2);
    //�ȴ����첽��λ
    while (!isInitP(LS_AxisName::Y1)
        || !isInitP(LS_AxisName::Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    //// �ȴ������첽�������
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("�����첽����ʧ��", Normal);
    //        return -1;
    //    }

    //}

    //��� Z ����λ
    if (AxisInitPAsync(LS_AxisName::Z) != 0)
        return -1;
    //���׸�λ
    setDO(LS_DO::DO_ClampCylinderL1, 0);
    setDO(LS_DO::DO_ClampCylinderL2, 0);
    setDO(LS_DO::DO_ClampCylinderR1, 0);
    setDO(LS_DO::DO_ClampCylinderR2, 0);
    //ָʾ�Ƹ�λ
    ResetStationLight();
    ResetStationLight(false);
    LSM->m_isStart.store(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return 0;
}

//���N��λ
int rs_motion::TaskReset_LS()
{
    //����������ᱨ�� 
    for (auto axis : LS_AxisName::allAxis)
    {
        //�������
        AxisClear(axis);
        ////����Ϊ��λ�ٶ�
        //ResetSetAxisParm(axis);
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        //WriteAxisServeOn(axis, true);
    }
    //�ɿ�����
    setDO(LS_DO::LS_OUT_ClampSuck, 0);
    closeDO();

    ////�����ȼӽ�λ
    //setDO(LS_DO::DO_ClampCylinderL1, 1);
    //setDO(LS_DO::DO_ClampCylinderL2, 1);
    //setDO(LS_DO::DO_ClampCylinderR1, 1);
    //setDO(LS_DO::DO_ClampCylinderR2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.Z);  //  Z  RZ �ȴ�����
    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.RZ);

    //  Z  RZ �ȴ�����
    if (AxisHomeAsync(LS_AxisName::LS_Z) != 0)
        return -1;

    //Y1 Y2��ͬʱ����
    if (AxisHome(LS_AxisName::LS_Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::LS_Y2) != 0)
        return -1;
    

    ////���㵽λ��ͬʱ����ʼλ Y1 Y2
    //AxisInitP(LS_AxisName::Y1);
    //AxisInitP(LS_AxisName::Y2);
    ////�ȴ����첽��λ
    //while (!isInitP(LS_AxisName::Y1)
    //    || !isInitP(LS_AxisName::Y2))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    //�ȴ����첽��λ
    while (!isHomeP(LS_AxisName::LS_Y1)
        || !isHomeP(LS_AxisName::LS_Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //U����
    if (AxisHome(LS_AxisName::LS_U) != 0)
    {
        machineLog->write("U1 ���㱨��", Normal);
        return -1;
    }
    //if (AxisHomeAsync(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 ���㱨��", Normal);
    //    return -1;
    //}
    
    //X�����
    if (AxisHome(LS_AxisName::LS_X) != 0)
    {
        machineLog->write("X ���㱨��", Normal);
        return -1;
    }
    //�ȴ����첽��λ
    while (!isHomeP(LS_AxisName::LS_U) || 
        !isHomeP(LS_AxisName::LS_X))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    //    //�ȴ����첽��λ
    //while (!isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    ////U1������λ
    //if (AxisInitP(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 ���ϱ���", Normal);
    //    return -1;
    //}
    ////X�ᵽ����λ
    //if (AxisInitP(LS_AxisName::LS_X) != 0)
    //{
    //    machineLog->write("X ���ϱ���", Normal);
    //    return -1;
    //}


    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    //// �ȴ������첽�������
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("�����첽����ʧ��", Normal);
    //        return -1;
    //    }

    //}

    ////��� Z ����λ
    //if (AxisInitPAsync(LS_AxisName::Z) != 0)
    //    return -1;
    ////���׸�λ
    //setDO(LS_DO::DO_ClampCylinderL1, 0);
    //setDO(LS_DO::DO_ClampCylinderL2, 0);
    //setDO(LS_DO::DO_ClampCylinderR1, 0);
    //setDO(LS_DO::DO_ClampCylinderR2, 0);
    //ָʾ�Ƹ�λ
    //ResetStationLight();
    //ResetStationLight(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return 0;
}

//�׶��Ḵλ
int rs_motion::TaskReset_ED()
{
    //����������ᱨ�� 
    for (auto axis : LS_AxisName::allAxis)
    {
        //�������
        AxisClear(axis);
    }

    closeDO();

    //�����ȼӽ�λ
    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    setDO(LS_DO::ED_DO_PositionCyl2, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //  Z  Z1 Z2 �ȴ�����
    if (AxisHome(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z2) != 0)
        return -1;
    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Z)
        || isRuning(LS_AxisName::Z1)
        || isRuning(LS_AxisName::Z2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::Z].card, m_Axis[LS_AxisName::Z].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Z1].card, m_Axis[LS_AxisName::Z1].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Z2].card, m_Axis[LS_AxisName::Z2].axisNum);

    //X Y U U1��ͬʱ����
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U1) != 0)
        return -1;

    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Y)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::U)
        || isRuning(LS_AxisName::U1))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y].card, m_Axis[LS_AxisName::Y].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::U].card, m_Axis[LS_AxisName::U].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::U1].card, m_Axis[LS_AxisName::U1].axisNum);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ////U����
    //if (AxisHome(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 ���㱨��", Normal);
    //    return -1;
    //}
    //if (AxisHomeAsync(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 ���㱨��", Normal);
    //    return -1;
    //}

    ////X�����
    //if (AxisHome(LS_AxisName::LS_X) != 0)
    //{
    //    machineLog->write("X ���㱨��", Normal);
    //    return -1;
    //}
    ////�ȴ����첽��λ
    //while (!isHomeP(LS_AxisName::LS_U) ||
    //    !isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    //    //�ȴ����첽��λ
    //while (!isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    setSpeed(LS_AxisName::U, m_Axis[LS_AxisName::U].minSpeed);
    //U������λ
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U ���ϱ���", Normal);
        machineLog->write("U ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X�ᵽ����λ
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���ϱ���", Normal);
        machineLog->write("X ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y�ᵽ����λ
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y ���ϱ���", Normal);
        machineLog->write("Y ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::U1, m_Axis[LS_AxisName::U1].minSpeed);
    //U1������λ
    if (AxisInitP(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 ���ϱ���", Normal);
        machineLog->write("U1 ���ϱ���", Err);
        return -1;
    }

    while (isRuning(LS_AxisName::U)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::U1)
        || isRuning(LS_AxisName::Y))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    machineLog->write("�����ʼλ", Normal);


    //std::this_thread::sleep_for(std::chrono::milliseconds(10));


    //// �ȴ������첽�������
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("�����첽����ʧ��", Normal);
    //        return -1;
    //    }

    //}

    ////��� Z ����λ
    //if (AxisInitPAsync(LS_AxisName::Z) != 0)
    //    return -1;
    ////���׸�λ
    //setDO(LS_DO::DO_ClampCylinderL1, 0);
    //setDO(LS_DO::DO_ClampCylinderL2, 0);
    //setDO(LS_DO::DO_ClampCylinderR1, 0);
    //setDO(LS_DO::DO_ClampCylinderR2, 0);
    //ָʾ�Ƹ�λ
    //ResetStationLight();
    //ResetStationLight(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //�ɿ���ŷ�
    setDO(LS_DO::ED_DO_PositionCyl1, 0);
    setDO(LS_DO::ED_DO_PositionCyl2, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 0);
    //��������˱���
    MODBUS_TCP.writeIntCoil(100, 3, 0);
    Sleep(500);
    MODBUS_TCP.writeIntCoil(100, 3, 1);
    return 0;
}

//��̩��λ
int rs_motion::TaskReset_BT()
{
    ////����������ᱨ�� 
    //for (auto axis : LS_AxisName::allAxis)
    //{
    //    //�������
    //    AxisClear(axis);
    //}

    closeDO();

    //X Y1 Y2��ͬʱ����
    if (AxisHome(LS_AxisName::Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Y2) != 0)
        return -1;

    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Y1)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y1].card, m_Axis[LS_AxisName::Y1].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y2].card, m_Axis[LS_AxisName::Y2].axisNum);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X�ᵽ����λ
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���ϱ���", Normal);
        machineLog->write("X ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y1, m_Axis[LS_AxisName::Y1].minSpeed);
    //Y1�ᵽ����λ
    if (AxisInitP(LS_AxisName::Y1) != 0)
    {
        machineLog->write("Y1 ���ϱ���", Normal);
        machineLog->write("Y1 ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y2, m_Axis[LS_AxisName::Y2].minSpeed);
    //Y2������λ
    if (AxisInitP(LS_AxisName::Y2) != 0)
    {
        machineLog->write("Y2 ���ϱ���", Normal);
        machineLog->write("Y2 ���ϱ���", Err);
        return -1;
    }

    while (isRuning(LS_AxisName::Y1)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    machineLog->write("�����ʼλ", Normal);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //�ɿ���ŷ�
    return 0;
}

//����Pin��λ
int rs_motion::TaskReset_JMPin()
{
    //����������ᱨ�� 
    for (auto axis : LS_AxisName::allAxis)
    {
        //�������
        AxisClear(axis);
    }


    //  Z  Z1 Z2 �ȴ�����
    if (AxisHome(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z1) != 0)
        return -1;

    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Z)
        || isRuning(LS_AxisName::Z1))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::Z].card, m_Axis[LS_AxisName::Z].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Z1].card, m_Axis[LS_AxisName::Z1].axisNum);

    //X Y U��ͬʱ����
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;

    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Y)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::U)
        || isRuning(LS_AxisName::X1))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y].card, m_Axis[LS_AxisName::Y].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::U].card, m_Axis[LS_AxisName::U].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::X1].card, m_Axis[LS_AxisName::X1].axisNum);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //U1����
    if (AxisHomeAsync(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 ���㱨��", Normal);
        return -1;
    }
    setSpeed(LS_AxisName::U, m_Axis[LS_AxisName::U].minSpeed);
    //U������λ
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U ���ϱ���", Normal);
        machineLog->write("U ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X�ᵽ����λ
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���ϱ���", Normal);
        machineLog->write("X ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y�ᵽ����λ
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y ���ϱ���", Normal);
        machineLog->write("Y ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X1, m_Axis[LS_AxisName::X1].minSpeed);
    //X1�ᵽ����λ
    if (AxisInitP(LS_AxisName::X1) != 0)
    {
        machineLog->write("X1 ���ϱ���", Normal);
        machineLog->write("X1 ���ϱ���", Err);
        return -1;
    }

    while (isRuning(LS_AxisName::U)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::X1)
        || isRuning(LS_AxisName::Y))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    machineLog->write("�����ʼλ", Normal);

    return 0;
}

//�������츴λ
int rs_motion::TaskReset_JMHan()
{
    //����������ᱨ�� 
    for (auto axis : LS_AxisName::allAxis)
    {
        //�������
        AxisClear(axis);
    }

    //  Z �ȴ�����
    if (AxisHomeAsync(LS_AxisName::Z) != 0)
    {
        machineLog->write("Z ���㱨��", Normal);
        return -1;
    }
    AxisClearPosition(m_Axis[LS_AxisName::Z].card, m_Axis[LS_AxisName::Z].axisNum);

    //X Y U��ͬʱ����
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;

    //�ȴ����첽��λ
    while (isRuning(LS_AxisName::Y)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::U))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Y].card, m_Axis[LS_AxisName::Y].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::U].card, m_Axis[LS_AxisName::U].axisNum);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    setSpeed(LS_AxisName::U, m_Axis[LS_AxisName::U].minSpeed);
    //U������λ
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U ���ϱ���", Normal);
        machineLog->write("U ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X�ᵽ����λ
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X ���ϱ���", Normal);
        machineLog->write("X ���ϱ���", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y�ᵽ����λ
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y ���ϱ���", Normal);
        machineLog->write("Y ���ϱ���", Err);
        return -1;
    }

    while (isRuning(LS_AxisName::U)
        || isRuning(LS_AxisName::X)
        || isRuning(LS_AxisName::Y))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    machineLog->write("�����ʼλ", Normal);

    return 0;
}


// ��λ��λָʾ�ƣ���λ : true���ҹ�λ ��flase��
void rs_motion::ResetStationLight(bool isstationL)
{
    //��ʱ��֪�� Modbus ���ô���������
    //ModbusCtrl.Instance.ResetOK();
    if (isstationL)
    {
        //DriverCtrl.Instance.SetOut(ENUMOUT.OUT_LightOK_L, ENUMTRIGGER.Hight);
        //DriverCtrl.Instance.SetOut(ENUMOUT.OUT_LightNG_L, ENUMTRIGGER.Hight);
        setDO(LS_DO::DO_LightOK_L, 0);
        setDO(LS_DO::DO_LightNG_L, 0);
    }
    else
    {
        //DriverCtrl.Instance.SetOut(ENUMOUT.OUT_LightOK_R, ENUMTRIGGER.Hight);
        //DriverCtrl.Instance.SetOut(ENUMOUT.OUT_LightNG_R, ENUMTRIGGER.Hight);
        setDO(LS_DO::DO_LightOK_R, 0);
        setDO(LS_DO::DO_LightNG_R, 0);
    }
}

// �������״̬
int rs_motion::TaskGoWating(ENUMSTATION stations)
{
    ////���߸�λ�ٶ� 
    //Enum.GetValues<ENUMAXIS>().ToList().ForEach(AX = >
    //{
    //    DriverCtrl.Instance.WriteAxisSpeedModel(AX, SpeedModeEnum.Hight);
    //});

    //���߸�λ�ٶ� 
    for (auto axis : LS_AxisName::allAxis)
    {
        WriteAxisSpeedModel(axis, SpeedModeEnum::High);
    }

    //AxisInitPAsync(LS_AxisName::Z);
    //AxisInitP(LS_AxisName::RZ);
    if (AxisInitPAsync(LS_AxisName::Z) != 0)
        return -1;
    if (AxisInitPAsync(LS_AxisName::RZ) != 0)
        return -1;

    //�����ȼӽ�λ 
    if (stations == ENUMSTATION::L || stations == ENUMSTATION::Both)
    {
        setDO(LS_DO::DO_ClampCylinderL1, 1);
        setDO(LS_DO::DO_ClampCylinderL2, 1);
    }
    if (stations == ENUMSTATION::R || stations == ENUMSTATION::Both)
    {
        setDO(LS_DO::DO_ClampCylinderR1, 1);
        setDO(LS_DO::DO_ClampCylinderR2, 1);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //AxisInitP(ENUMAXIS.X);
    if (AxisInitPAsync(LS_AxisName::X) != 0)
        return -1;

    //U Y ���� ����λ�������λ
    switch (stations)
    {
    case ENUMSTATION::L:
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.U1);
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.Y1);
        if (AxisInitPAsync(LS_AxisName::U1) != 0)
            return -1;
        if (AxisInitPAsync(LS_AxisName::Y1) != 0)
            return -1;
        //���׸�λ
        setDO(LS_DO::DO_ClampCylinderL1, 0);
        setDO(LS_DO::DO_ClampCylinderL2, 0);
        break;
    case ENUMSTATION::R:
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.U2);
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.Y2);
        if (AxisInitPAsync(LS_AxisName::U2) != 0)
            return -1;
        if (AxisInitPAsync(LS_AxisName::Y2) != 0)
            return -1;
        //���׸�λ
        setDO(LS_DO::DO_ClampCylinderR1, 0);
        setDO(LS_DO::DO_ClampCylinderR2, 0);
        break;
    case ENUMSTATION::Both:
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.U1);
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.U2);

        AxisInitP(LS_AxisName::U1);
        AxisInitP(LS_AxisName::U2);
        //�ȴ����첽��λ
        while (!isInitP(LS_AxisName::U1)
            || !isInitP(LS_AxisName::U2))
        {
            if (m_isEmergency.load())
                return -1;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.Y1);
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.Y2);

        AxisInitP(LS_AxisName::Y1);
        AxisInitP(LS_AxisName::Y2);
        //�ȴ����첽��λ
        while (!isInitP(LS_AxisName::Y1)
            || !isInitP(LS_AxisName::Y2))
        {
            if (m_isEmergency.load())
                return -1;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        //���׸�λ
        setDO(LS_DO::DO_ClampCylinderL1, 0);
        setDO(LS_DO::DO_ClampCylinderL2, 0);
        setDO(LS_DO::DO_ClampCylinderR1, 0);
        setDO(LS_DO::DO_ClampCylinderR2, 0);
        break;
    default: break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 0;
}

//�Ƿ�ͣ�ڳ�ʼλ��
bool rs_motion::isInitP(QString axis)
{
    auto res1 = false;
    if (fabs(ReadAxisP(axis) - m_Axis[axis].initPos) < ERROR_VALUE)
        res1 = true;
    auto res2 = !isRuning(axis);
    return (res1 && res2);
}

//���Ƿ����˶���
bool rs_motion::isRuning(QString axis)
{
    return IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum);
}

//�Ƿ񵽴�ԭ��
bool rs_motion::isHomeP(QString axis)
{
    auto res1 = false;
    if (fabs(ReadAxisP(axis) - 0) < ERROR_VALUE)
        res1 = true;
    auto res2 = isRuning(axis);
    return (res1 && !res2);
}

//��ȡ�䷽����
void rs_motion::loadFormulaFromFile()
{
    // ��ȡFormula_ConfigĿ¼·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        return;  // ���Ŀ¼�������򷵻�
    }

    // ��ȡ�ļ����������ļ�
    dir.setFilter(QDir::Files);
    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        if (fileInfo.suffix() == "json") {
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray fileData = file.readAll();
                file.close();

                // ���� JSON ����
                QJsonDocument doc = QJsonDocument::fromJson(fileData);
                if (doc.isObject()) {
                    QJsonObject formulaJson = doc.object();
                    Formula_Config formulaConfig;

                    // ���� ListProcess_L
                    QJsonArray listProcessL = formulaJson["ListProcess_L"].toArray();
                    for (const QJsonValue& value : listProcessL) {
                        QJsonObject pointJson = value.toObject();
                        PointItemConfig point;
                        point.StationL = pointJson["StationL"].toInt();
                        point.PointType = pointJson["PointType"].toInt();
                        switch (m_version)
                        {
                        case ENUM_VERSION::TwoPin:
                            point.TargetPX = pointJson["TargetPX"].toDouble();
                            point.TargetPY = pointJson["TargetPY"].toDouble();
                            point.TargetPZ = pointJson["TargetPZ"].toDouble();
                            point.TargetPRZ = pointJson["TargetPRZ"].toDouble();
                            point.TargetPU = pointJson["TargetPU"].toDouble();
                            break;
                        case ENUM_VERSION::EdPin:
                            //�׶��������
                            point.TargetPAxis_1 = pointJson["TargetPAxis_1"].toDouble();
                            point.TargetPAxis_2 = pointJson["TargetPAxis_2"].toDouble();
                            point.TargetPAxis_3 = pointJson["TargetPAxis_3"].toDouble();
                            point.TargetPAxis_4 = pointJson["TargetPAxis_4"].toDouble();
                            point.TargetPAxis_5 = pointJson["TargetPAxis_5"].toDouble();
                            point.TargetPAxis_6 = pointJson["TargetPAxis_6"].toDouble();
                            point.TargetPAxis_7 = pointJson["TargetPAxis_7"].toDouble();
                            break;
                        case ENUM_VERSION::LsPin: 
                            //���N�����
                            point.TargetPAxis_1 = pointJson["TargetPAxis_1"].toDouble();
                            point.TargetPAxis_2 = pointJson["TargetPAxis_2"].toDouble();
                            point.TargetPAxis_3 = pointJson["TargetPAxis_3"].toDouble();
                            point.TargetPAxis_4 = pointJson["TargetPAxis_4"].toDouble();
                            point.TargetPAxis_5 = pointJson["TargetPAxis_5"].toDouble();
                            break;
                        case ENUM_VERSION::BtPin:
                            //��̩�����
                            point.TargetPAxis_1 = pointJson["TargetPAxis_1"].toDouble();
                            point.TargetPAxis_2 = pointJson["TargetPAxis_2"].toDouble();
                            point.TargetPAxis_3 = pointJson["TargetPAxis_3"].toDouble();
                            break;
                        default:
                            break;
                        }
                        point.SpeedModel = pointJson["SpeedModel"].toInt();
                        point.Vision = pointJson["Vision"].toString();
                        point.Tip = pointJson["Tip"].toString();
                        point.CloudName = pointJson["CloudName"].toString();
                        point.ProductName = pointJson["ProductName"].toString();
                        point.FeedAction = pointJson["FeedAction"].toInt();
                        point.CameraName = pointJson["CameraName"].toString();
                        point.PhotoName = pointJson["PhotoName"].toString();
                        point.VisionConfig = pointJson["VisionConfig"].toInt();
                        point.isResultMerging = pointJson["isResultMerging"].toInt();
                        point.scanRow3d = pointJson["scanRow3d"].toInt();
                        point.CircleBenchmark = pointJson["CircleBenchmark"].toDouble();
                        point.CircleUpError = pointJson["CircleUpError"].toDouble();
                        point.CircleDownError = pointJson["CircleDownError"].toDouble();
                        point.VisionTwo = pointJson["VisionTwo"].toString();
                        point.PixDistance = pointJson["PixDistance"].toDouble();
                        point.ExposureTime = pointJson["ExposureTime"].toInt();

                        formulaConfig.ListProcess_L.append(point);
                    }

                    if (m_version == ENUM_VERSION::TwoPin)
                    {
                        // ˫��λ�汾
                        // ���� ListProcess_R
                        QJsonArray listProcessR = formulaJson["ListProcess_R"].toArray();
                        for (const QJsonValue& value : listProcessR) {
                            QJsonObject pointJson = value.toObject();
                            PointItemConfig point;
                            point.StationL = pointJson["StationL"].toInt();
                            point.PointType = pointJson["PointType"].toInt();
                            point.TargetPX = pointJson["TargetPX"].toDouble();
                            point.TargetPY = pointJson["TargetPY"].toDouble();
                            point.TargetPZ = pointJson["TargetPZ"].toDouble();
                            point.TargetPRZ = pointJson["TargetPRZ"].toDouble();
                            point.TargetPU = pointJson["TargetPU"].toDouble();
                            point.SpeedModel = pointJson["SpeedModel"].toInt();
                            point.Vision = pointJson["Vision"].toString();
                            point.Tip = pointJson["Tip"].toString();
                            point.CloudName = pointJson["CloudName"].toString();
                            point.ProductName = pointJson["ProductName"].toString();
                            point.FeedAction = pointJson["FeedAction"].toInt();
                            point.CameraName = pointJson["CameraName"].toString();
                            point.PhotoName = pointJson["PhotoName"].toString();
                            point.VisionConfig = pointJson["VisionConfig"].toInt();
                            point.isResultMerging = pointJson["isResultMerging"].toInt();
                            point.scanRow3d = pointJson["scanRow3d"].toInt();
                            point.CircleBenchmark = pointJson["CircleBenchmark"].toDouble();
                            point.CircleUpError = pointJson["CircleUpError"].toDouble();
                            point.CircleDownError = pointJson["CircleDownError"].toDouble();
                            point.VisionTwo = pointJson["VisionTwo"].toString();
                            point.PixDistance = pointJson["PixDistance"].toDouble();
                            point.ExposureTime = pointJson["ExposureTime"].toInt();

                            formulaConfig.ListProcess_R.append(point);
                        }
                    }
                    // ���䷽��Ϣ���浽 m_Formula ��
                    QString formulaName = fileInfo.baseName();  // �ļ�����Ϊ�䷽����
                    m_Formula[formulaName] = formulaConfig;
                }
            }
        }
    }
}

// ���������䷽
void rs_motion::saveFormulaToFile()
{
    // ��ȡFormula_ConfigĿ¼·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (const auto& pair :m_Formula) {
        QString formulaName = pair.first;  // ��ȡ�䷽����
        rs_motion::Formula_Config formulaConfig = pair.second;  // ��ȡ�䷽����

        QJsonObject formulaJson;
        //ListProcess_L
        QJsonArray listProcessLArray;
        for (const auto& point : formulaConfig.ListProcess_L) {
            QJsonObject pointJson;
            pointJson["StationL"] = point.StationL;
            pointJson["PointType"] = point.PointType;
            switch (m_version)
            {
            case ENUM_VERSION::TwoPin:
                pointJson["TargetPX"] = point.TargetPX;
                pointJson["TargetPY"] = point.TargetPY;
                pointJson["TargetPZ"] = point.TargetPZ;
                pointJson["TargetPRZ"] = point.TargetPRZ;
                pointJson["TargetPU"] = point.TargetPU;
                break;
            case ENUM_VERSION::EdPin:
                //�׶��������
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                pointJson["TargetPAxis_6"] = point.TargetPAxis_6;
                pointJson["TargetPAxis_7"] = point.TargetPAxis_7;
                break;
            case ENUM_VERSION::LsPin: 
                //���N�����
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                break;
            case ENUM_VERSION::BtPin:
                //��̩�����
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                break;
            default:
                break;
            }
         
            pointJson["SpeedModel"] = point.SpeedModel;
            pointJson["Vision"] = point.Vision;
            pointJson["Tip"] = point.Tip;
            pointJson["CloudName"] = point.CloudName;
            pointJson["ProductName"] = point.ProductName;
            pointJson["FeedAction"] = point.FeedAction;
            pointJson["CameraName"] = point.CameraName;
            pointJson["PhotoName"] = point.PhotoName;
            pointJson["VisionConfig"] = point.VisionConfig;
            pointJson["isResultMerging"] = point.isResultMerging;
            pointJson["scanRow3d"] = point.scanRow3d;
            pointJson["CircleBenchmark"] = point.CircleBenchmark;
            pointJson["CircleUpError"] = point.CircleUpError;
            pointJson["CircleDownError"] = point.CircleDownError;
            pointJson["VisionTwo"] = point.VisionTwo;
            pointJson["PixDistance"] = point.PixDistance;
            pointJson["ExposureTime"] = point.ExposureTime;

            listProcessLArray.append(pointJson);
        }
        formulaJson["ListProcess_L"] = listProcessLArray;

        if (m_version == ENUM_VERSION::TwoPin)
        {
            // ˫��λ���
            //ListProcess_R
            QJsonArray listProcessRArray;
            for (const auto& point : formulaConfig.ListProcess_R) {
                QJsonObject pointJson;
                pointJson["StationL"] = point.StationL;
                pointJson["PointType"] = point.PointType;
                pointJson["TargetPX"] = point.TargetPX;
                pointJson["TargetPY"] = point.TargetPY;
                pointJson["TargetPZ"] = point.TargetPZ;
                pointJson["TargetPRZ"] = point.TargetPRZ;
                pointJson["TargetPU"] = point.TargetPU;
                pointJson["SpeedModel"] = point.SpeedModel;
                pointJson["Vision"] = point.Vision;
                pointJson["Tip"] = point.Tip;
                pointJson["CloudName"] = point.CloudName;
                pointJson["ProductName"] = point.ProductName;
                pointJson["FeedAction"] = point.FeedAction;
                pointJson["CameraName"] = point.CameraName;
                pointJson["PhotoName"] = point.PhotoName;
                pointJson["VisionConfig"] = point.VisionConfig;
                pointJson["isResultMerging"] = point.isResultMerging;
                pointJson["scanRow3d"] = point.scanRow3d;
                pointJson["CircleBenchmark"] = point.CircleBenchmark;
                pointJson["CircleUpError"] = point.CircleUpError;
                pointJson["CircleDownError"] = point.CircleDownError;
                pointJson["VisionTwo"] = point.VisionTwo;
                pointJson["PixDistance"] = point.PixDistance;
                pointJson["ExposureTime"] = point.ExposureTime;
                
                listProcessRArray.append(pointJson);
            }
            formulaJson["ListProcess_R"] = listProcessRArray;
        }

        // �� QJsonObject ת��Ϊ QJsonDocument
        QJsonDocument doc(formulaJson);

        // ���� JSON ���ݵ��ļ�
        QString filePath = dir.filePath(formulaName + ".json");
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }
    }
}

// ���嶯����������
int rs_motion::runAction(QList<PointItemConfig> listProcess)
{

#ifdef DEBUG_LHC
    //��ʼ������
    initAllVariable();
    barCodeIdentify(listProcess[0]);
    QrCodeIdentify(listProcess[1]);
    templateIdentify(listProcess[2]);

#else
    //��ʼ������
    initAllVariable();
    bool showPhoto = false;

    //���̿�ʼʱ��
    m_runStartTime = QDate::currentDate().toString("yyyyMMdd_") + QTime::currentTime().toString("hh-mm-ss");
    if (m_version != LsPin)
    {
        //���ͼƬ
        emit clearWindow_signal();
    }
    if (true)
    {
        //�����߳������������Զ�����
        std::vector<std::thread> threads;  //�洢�̵߳�����
        ThreadGuard guard(threads);  // ���������ڴ��������ڹ����߳�,��ֹ����return�̱߳���
        for (int i = 0; i < listProcess.size(); i++)
        {
            if (m_isEmergency.load())
            {
                return -1;
            }
            auto item = listProcess[i];
            if (i == 0)
            {
                if (!testSN.isEmpty())
                {
                    item.ModelName = testSN;
                }
                else if (!m_snReq.sn.isEmpty())
                {
                    item.ModelName = m_snReq.sn;
                }
                if (item.ModelName.isEmpty())
                {
                    //���û���ͺţ�д�䷽������
                    item.ModelName = m_forMulaName;
                }
                QString exePath = QCoreApplication::applicationDirPath();
                QString filePath = "/images/Result/";

                item.ModelName.replace('*', '@');         // �滻����*Ϊ@
                m_resultImgPath = exePath + filePath + item.ModelName + "/" + m_runStartTime;
                //ԭͼ·��
                m_originalImgPath = exePath + "/images/Original/" + item.ModelName + "/" + m_runStartTime;
            }

            int res = 0;

            if (m_version == LsPin && !m_snReq.sn.isEmpty())
            {
                listProcess[i].ModelName = m_snReq.sn;
            }
            else if (!LSM->testSN.isEmpty())
            {
                listProcess[i].ModelName = LSM->testSN;
            }

            //0611����
            //ǰ������
#if LOCAL_TEST == 0
            res = ItemPreProcess(item);
            if (res != 0)
                return res;

            //��������
            res = ItemMovingProcess(item);
            if (res != 0)
                return res;

            //��������
            res = ItemLastProcess(item);
            if (res != 0)
                return res;
#endif

            if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Point2D)
            {
                //2Dʶ��
#if LOCAL_TEST == 0
                auto& camera = m_cameraMap[item.CameraName];
                if (camera == nullptr)
                {
                    machineLog->write("PIN�������豸Ϊ�ա���", Normal);
                    continue;
                }
                if (!camera->m_outImg.IsInitialized())
                {
                    machineLog->write("2D��ƬΪ�ա���", Normal);
                    continue;
                }

                threads.emplace_back(&rs_motion::process2Didentify, this, camera->m_outImg, item);
#else
                HObject tpImg;
                if (item.ProductName == "kuan")
                {
                    //ReadImage(&tpImg, "C:\\Users\\67024\\Desktop\\2D\\1.bmp");
                    ReadImage(&tpImg, "D:\\use_test\\binarization\\1.bmp");
                }
                threads.emplace_back(&rs_motion::process2Didentify, this, tpImg, item);
#endif

                showPhoto = true;
            }
            else if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Point3D_identify)
            {
                //3dʶ��
                if (m_cloud_ori_map.empty())
                {
                    machineLog->write("��ɨ�����Ϊ�ա���", Normal);
                    continue;
                }
                if (!m_cloud_ori_map.count(item.CloudName))
                {
                    machineLog->write("δ�ҵ����� ��" + item.CloudName, Normal);
                    continue;
                }
                if (m_cloud_ori_map[item.CloudName] == nullptr)
                {
                    machineLog->write("���� ��" + item.CloudName + " Ϊ��", Normal);
                    continue;
                }
                // �����̲߳��洢��������
                std::function<void()> run = std::bind(
                    &rs_motion::process3Didentify,
                    this,
                    m_cloud_ori_map[item.CloudName],
                    item
                );
                threads.emplace_back(run);
                showPhoto = true;
            }
            else if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Visual_Identity)
            {
                if (item.VisionConfig == TempMatch)
                {
                    //ģ��ƥ��
                    threads.emplace_back(&rs_motion::templateIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == OneDimensional)
                {
                    //һά��ʶ��
                    threads.emplace_back(&rs_motion::barCodeIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == TwoDimensional)
                {
                    //��ά��ʶ��
                    threads.emplace_back(&rs_motion::QrCodeIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == BlobModel)
                {
                    //Blobʶ��
                    threads.emplace_back(&rs_motion::blobIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == CircleModel)
                {
                    //Բ���ģ��
                    threads.emplace_back(&rs_motion::circleIdentify, this, item);
                    showPhoto = true;
                }
            }
            else if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Circle_Measure)
            {
                //Բ���ģ��
                threads.emplace_back(&rs_motion::circleMeasure, this, item);
                showPhoto = true;
            }
           

            if (i + 1 < listProcess.size())
            {
                //��ǰִ��������1��ѡ����һ��
                if (item.StationL == ENUMSTATION::L)
                {
                    m_showStepIndex_L += 1;
                    emit updateStepIndex_signal();
                }
                else if (item.StationL == ENUMSTATION::R)
                {
                    m_showStepIndex_R += 1;
                    emit updateStepIndex_signal();
                }
            }
        }
    }
#endif

    //�ϲ�3D�����
    if (m_pinResult3D.size() > 1)
    {
        summary3dResult();
    }

    if (m_version == LsPin)
    {
        //���ͼƬ
        emit clearWindow_signal();
    }
    if (showPhoto)
    {
        //����ִ��������pin��ʶ�����Ӿ����
        int rtn = pinShowResult(m_runResult);
        if (rtn != 0)
        {
            machineLog->write("pin�������ʧ��", Normal);
            return rtn;
        }
        if (m_version == LsPin)
        {
            m_snResult.result = m_runResult.isNG ? "FAIL" : "PASS";
        }

        //��ȡ��ǰ��������״̬
        machineLog->write("��ǰ���̼��״̬��" + QString::number(m_runResult.isNG), Normal);
        //ˢ����ʾ�������
        emit upDataProductPara_signal(m_runResult);
        //�豸�е�mes����
        checkMesFunction();
    }

    if (m_isLoop.load())
    {
        //ѭ������
        m_showStepIndex_L = 0;
        m_showStepIndex_R = 0;
        emit updateStepIndex_signal();
    }

    return 0;
}

//ǰ������ 1. 3d��λִ��ǰ�������ӳ� ,��������������
int rs_motion::ItemPreProcess(PointItemConfig item)
{
    //3Dǰ�ö���
    if (item.PointType == ENUMPOINTTYPE::Point3D)
    {
        if (!m_pcMyCamera_3D)
        {
            // �����豸ʵ��
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
            if (m_pcMyCamera_3D == nullptr)
                return -1;
            //�豸��ʼ��
            m_pcMyCamera_3D->InitCamera3DLink();
            QTime time = QTime::currentTime();
            while (fabs(QTime::currentTime().secsTo(time)) < 20)
            {
                //��ȴ�20��
                if (m_isEmergency.load())
                    return -1;
                //�ж��Ƿ��ʼ�����
                if (m_pcMyCamera_3D->m_initSucess)
                    break;
            }
        }
        if (!m_pcMyCamera_3D->m_initSucess)
        {
            //û�гɹ���ʼ�����ٳ�ʼ��һ��
            m_pcMyCamera_3D->InitCamera3DLink();
            QTime time = QTime::currentTime();
            while (fabs(QTime::currentTime().secsTo(time)) < 20)
            {
                //��ȴ�20��
                if (m_isEmergency.load())
                    return -1;
                //�ж��Ƿ��ʼ�����
                if (m_pcMyCamera_3D->m_initSucess)
                    break;
            }
        }
        if (item.isSave3dCloud)
        {
            //�������
            m_pcMyCamera_3D->m_isSaveCloud = true;
            m_pcMyCamera_3D->m_cloudName = item.CloudName;
        }
        else
        {
            m_pcMyCamera_3D->m_isSaveCloud = false;
        }
        //�Ƿ��Ѿ����豸�ڵȴ�Ӳ����
        //if (!m_pcMyCamera_3D->getStartFlag())
        //{
        //    //��ʼ�ɼ�
        //    m_pcMyCamera_3D->StartProgram();
        //}

        //��������
        m_pcMyCamera_3D->AllocBuf(item.scanRow3d);
        //�����ع�
        if(item.ExposureTime > 0)
            m_pcMyCamera_3D->setExpsuretime(item.ExposureTime);
        //��ʼ�ɼ�
        m_pcMyCamera_3D->StartProgram();
        if (m_pcMyCamera_3D->m_cloud_ori != nullptr)
        {
            //����ϴ��Ѽ�¼�ĵ���
            m_pcMyCamera_3D->m_cloud_ori->clear();
        }

        switch (LSM->m_version){
        case ENUM_VERSION::TwoPin:
        {
            //��ʼ������ 
            setDO(LS_DO::DO_PlusCam3D1, 0);
            setDO(LS_DO::DO_PlusCam3D2, 0);
            setDO(LS_DO::DO_PlusCam3D3, 0);
            //��ʼ���������
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //����
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //�������� 
            QString axis = DetectPulsAxis(item);
            if (axis == LS_AxisName::X)
            {
                setDO(LS_DO::DO_PlusCam3D1, 1);
            }
            else if (axis == LS_AxisName::Y1)
            {
                setDO(LS_DO::DO_PlusCam3D2, 1);
            }
            else if (axis == LS_AxisName::Y2)
            {
                setDO(LS_DO::DO_PlusCam3D3, 1);
            }

            //�ӳٴ��� 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //�������
            setDO(LS_DO::DO_Cam3DTrigger, 1);
            break;
        }
        case ENUM_VERSION::LsPin:
        {
            //��ʼ���������
            setDO(LS_DO::LS_OUT_3DCamTrigger, 0);
            //����
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //�ӳٴ��� 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //�������
            setDO(LS_DO::LS_OUT_3DCamTrigger, 1);
            break;
        }
        case ENUM_VERSION::EdPin:
        {
            //��ʼ���������
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //����
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //�ӳٴ��� 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //�������
            setDO(LS_DO::DO_Cam3DTrigger, 1);
            break;
        }
        default:
            break;
        }
    }
    else if (item.PointType == ENUMPOINTTYPE::Feeding)
    {
        if (m_version == LsPin)
        {
            if (item.FeedAction == Suction)
            {
                ////�ȴ����ɿ�
                //setDO(LS_DO::LS_OUT_ClampBlow, 1);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                ////�رմ���
                //setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //�ɿ��ؾ�
                setDO(LS_DO::LS_OUT_ClampCyl1, 0);
                setDO(LS_DO::LS_OUT_ClampCyl2, 0);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                //��������
                setDO(LS_DO::LS_OUT_ClampSuck, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.suctionTime));
                QElapsedTimer materialTimer;
                materialTimer.start();  // ��ʼ��ʱ
                while (getDI(LS_DI::LS_IN_VacuumDetect) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // ��λ������
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("��צ��ռ�ⳬʱ��10����δ����", Normal);
                        machineLog->write("��צ��ռ�ⳬʱ��10����δ����", Err);
                        return -1;
                    }
                    Sleep(1);
                }
            }
            else if (item.FeedAction == Suction_2)
            {
                //�ɿ��ؾ�
                setDO(LS_DO::LS_OUT_ClampCyl1, 0);
                setDO(LS_DO::LS_OUT_ClampCyl2, 0);
                //����ؾ�ԭ���ź�
                QElapsedTimer materialTimer;
                materialTimer.start();  // ��ʼ��ʱ
                while (getDI(LS_DI::LS_IN_ClampCyl1_Home) == 0 || getDI(LS_DI::LS_IN_ClampCyl2_Home) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // ��λ������
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("�ؾ�ԭ���ⳬʱ��10����δ�ɿ�", Normal);
                        machineLog->write("�ؾ�ԭ���ⳬʱ��10����δ�ɿ�", Err);
                        return -1;  
                    }
                    Sleep(1);
                }
                ////�ȴ����ɿ�
                //setDO(LS_DO::LS_OUT_ClampBlow, 1);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                //�رմ���
                //setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //��������
                setDO(LS_DO::LS_OUT_ClampSuck, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.suctionTime));
                materialTimer.start();  // ��ʼ��ʱ
                while (getDI(LS_DI::LS_IN_VacuumDetect) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // ��λ������
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("��צ��ռ�ⳬʱ��10����δ����", Normal);
                        machineLog->write("��צ��ռ�ⳬʱ��10����δ����", Err);
                        return -1;
                    }
                    Sleep(1);
                }
                Sleep(200);
            }
            else if (item.FeedAction == Unloading)
            {
                //����
                setDO(LS_DO::LS_OUT_ClampSuck, 0);
                setDO(LS_DO::LS_OUT_ClampBlow, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.unLoadTime));
                setDO(LS_DO::LS_OUT_ClampBlow, 0);
            }
            else if (item.FeedAction == Unloading_2)
            {
                //����
                setDO(LS_DO::LS_OUT_ClampSuck, 0);
                setDO(LS_DO::LS_OUT_ClampBlow, 1);

                //����ؾ������ź�
                QElapsedTimer materialTimer;
                //ģ��
                materialTimer.start();  // ��ʼ��ʱ
                while (getDI(LS_DI::LS_IN_MaterialDetect) == 1)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // ��λ������
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("�ؾ߼�ⳬʱ��10����δ��⵽����", Normal);
                        machineLog->write("�ؾ߼�ⳬʱ��10����δ��⵽����", Err);
                        return -1;  // ���ض����������ֳ�ʱ
                    }
                    Sleep(1);
                }
                //������ʱ
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.unLoadTime));
                setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //std::this_thread::sleep_for(std::chrono::milliseconds(500));
                //�н��ؾ�
                setDO(LS_DO::LS_OUT_ClampCyl1, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                setDO(LS_DO::LS_OUT_ClampCyl2, 1);
                //����ؾ߶����ź�(����ԭ���ź��Ƿ���ʧȥ��⣬�����źŲ��ȶ�)
                materialTimer.start();  // ��ʼ��ʱ
                while (getDI(LS_DI::LS_IN_ClampCyl1_Home) == 1 || getDI(LS_DI::LS_IN_ClampCyl2_Home) == 1)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // ��λ������
                    if (materialTimer.elapsed() > 20000) {
                        machineLog->write("�ؾ߶����ⳬʱ��20����δ�н�", Normal);
                        machineLog->write("�ؾ߶����ⳬʱ��20����δ�н�", Err);
                        return -1;
                    }
                    Sleep(1);
                }

            }
        }
    }

    return 0;
}

// �ж϶�Ӧ�������ᣨ˫��pin����
QString rs_motion::DetectPulsAxis(PointItemConfig item)
{
    return (fabs(item.TargetPX - ReadAxisP(LS_AxisName::X)) > 2) ? LS_AxisName::X : (item.StationL == ENUMSTATION::L ? LS_AxisName::Y1 : LS_AxisName::Y2);
}

//�ж����˶��Ƿ�����
bool rs_motion::isMovePostive(QString axis, double target)
{
    return (ReadAxisP(axis) - target <= 0);
}

// ItemPart �ᶯ��
int rs_motion::ItemMovingProcess(PointItemConfig item)
{
    if (item.PointType == Point3D_identify || item.PointType == Visual_Identity || item.PointType == Circle_Measure)
        return 0;
    int rtn = 0;
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        //˫��pin��������˶�ģʽ
        rtn = pin2MotionAction(item);
        break;
    case ENUM_VERSION::LsPin:
        //���N�����˶�ģʽ
        rtn = pinLsAction(item);
        break;
    case ENUM_VERSION::EdPin:
        //�׶�������˶�ģʽ
        rtn = pinEdAction(item);
        break;
    case ENUM_VERSION::JmPin:
        //���������˶�ģʽ
        rtn = pinJmAction(item);
        break;
    case ENUM_VERSION::JmHan:
        //������������˶�ģʽ
        rtn = hanJmAction(item);
        break;
    default:
        break;
    }
    if (rtn != 0)
        return rtn;
    return 0;
}



// ItemPart ��������
//  1. 3d��λִ�к󱣳֣�ȡ��
//  2. 2d��λִ�к󣬵�λ���� �ӳ� ���ֵƹ�
int rs_motion::ItemLastProcess(PointItemConfig item)
{
    //���ò���
    DeviceMoveit_Config Moveitinf = m_Paramer;
    int rtn = 0;
    QTime time = QTime::currentTime();
    //��Դ
    QString lightDo = LS_DO::DO_Light2DTrigger;
    switch ((ENUMPOINTTYPE)item.PointType)
    {
    case ENUMPOINTTYPE::Point2D:
        //region 2D
        //��¼��ǰ�Ӿ��䷽
        m_visionFormula_2D.push_back(item.Vision);
        //����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //��Դ����  �ߵ�ƽ����
        switch (m_version)
        {
        case LsPin:
            lightDo = LS_DO::LS_OUT_2DLightTrigger;
            break;
        case TwoPin:
        case BtPin:
            lightDo = LS_DO::DO_Light2DTrigger;
            break;
        case EdPin:
            //�׶���
            if (item.CameraName == LS_CameraName::ed_Location)
                lightDo = LS_DO::ED_DO_LightPositioningTrigger;
            else if (item.CameraName == LS_CameraName::pin2D)
                lightDo = LS_DO::DO_Light2DTrigger;
            else if (item.CameraName == LS_CameraName::ed_Up)
                lightDo = LS_DO::ED_DO_LightFrontTrigger;
            else if (item.CameraName == LS_CameraName::ed_Down)
                lightDo = LS_DO::ED_DO_LightRearTrigger;
            break;
        default:
            break;
        }
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        setDO(lightDo, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //����Դ
        setDO(lightDo, 1);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 1);
        }
        //�ӳٴ���
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        //�������

        if (m_cameraName[item.CameraName].empty())
        {
            machineLog->write("�������" + item.CameraName + "���û�ȡʧ�ܣ���", Normal);
            machineLog->write("�������" + item.CameraName + "���û�ȡʧ�ܣ���", Err);
            return -1;
        }
        rtn = catPhoto2D(m_cameraName[item.CameraName], m_cameraMap[item.CameraName], item.ExposureTime);
        if (rtn != 0)
        {
            machineLog->write("Pin2DͼƬ����ʧ�ܣ��������� ��" + QString::number(rtn), Normal);
            machineLog->write("Pin2DͼƬ����ʧ�ܣ��������� ��" + QString::number(rtn), Err);
            return rtn;
        }
        //��Դ����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //�رչ�Դ
        setDO(lightDo, 0);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        break;
    case ENUMPOINTTYPE::Point3D:
        //��¼��ǰ�Ӿ��䷽
        m_visionFormula_3D.push_back(item.Vision);
        switch (m_version)
        {
        case TwoPin:
            //ȡ���������
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //ȡ������  
            setDO(LS_DO::DO_PlusCam3D1, 0);
            setDO(LS_DO::DO_PlusCam3D2, 0);
            setDO(LS_DO::DO_PlusCam3D3, 0);
            break;
        case LsPin:
            //ȡ���������
            setDO(LS_DO::LS_OUT_3DCamTrigger, 0);
            break;
        case EdPin:
            //ȡ���������
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            break;
        default:
            break;
        }
        //���嵽λ����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime3D));
        time = QTime::currentTime();
        while (fabs(QTime::currentTime().secsTo(time)) < 10)
        {
            //��ȴ�10��
            if (m_isEmergency.load())
                return -1;
            //�ж��Ƿ�ɹ��ɼ�3D����
            if (m_pcMyCamera_3D->m_cloud_ori != nullptr)
            {
                if (m_pcMyCamera_3D->m_cloud_ori->size() > 0)
                {
                    //�ر�3D����߳�
                    m_pcMyCamera_3D->StopProgram();
                    auto cloud = m_pcMyCamera_3D->m_cloud_ori->makeShared();
                    m_cloud_ori_map[item.CloudName] = cloud;
                    //if (!m_pcMyCamera_3D->depth_color_img.empty())
                    //{
                    //    machineLog->write("- - - ->>>>>>>>ͼƬ���", Normal);
                    //    cv::imwrite("3Dtest.bmp", m_pcMyCamera_3D->depth_color_img);
                    //}
                    break;
                }
            }
        }
        if (m_pcMyCamera_3D->m_cloud_ori == nullptr)
        {
            machineLog->write("3D���Ʋɼ���ʱ���˳��˶�!!", Normal);
            machineLog->write("3D���Ʋɼ���ʱ���˳��˶�!!", Err);
            return -1;
        }
        if (m_pcMyCamera_3D->m_cloud_ori->size() <= 0)
        {
            machineLog->write("3D���Ʋɼ����������˳��˶�!!", Normal);
            machineLog->write("3D���Ʋɼ����������˳��˶�!!", Err);
            return -1;
        }
        break;
    case ENUMPOINTTYPE::PointNone: break;
    case ENUMPOINTTYPE::TakePhoto:
    {
        //��������
        if (!m_cameraName.count(item.CameraName) ||
            !m_cameraMap.count(item.CameraName) ||
            !m_cameraMap[item.CameraName]) {
            machineLog->write("���δ���ã�" + item.CameraName, Normal);
            machineLog->write("���δ���ã�" + item.CameraName, Err);
            return -1;
        }
        //����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //��Դ����  �ߵ�ƽ����
        switch (m_version)
        {
        case LsPin:
            lightDo = LS_DO::LS_OUT_2DLightTrigger;
            break;
        case BtPin:
        case TwoPin:
            lightDo = LS_DO::DO_Light2DTrigger;
            break;
        case EdPin:
            //�׶���
            if (item.CameraName == LS_CameraName::ed_Location)
                lightDo = LS_DO::ED_DO_LightPositioningTrigger;
            else if (item.CameraName == LS_CameraName::pin2D)
                lightDo = LS_DO::DO_Light2DTrigger;
            else if (item.CameraName == LS_CameraName::ed_Up)
                lightDo = LS_DO::ED_DO_LightFrontTrigger;
            else if (item.CameraName == LS_CameraName::ed_Down)
                lightDo = LS_DO::ED_DO_LightRearTrigger;
            break;
        default:
            break;
        }
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        setDO(lightDo, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //����Դ
        setDO(lightDo, 1);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 1);
        }
        //�ӳٴ���
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        rtn = catPhoto2D(m_cameraName[item.CameraName], m_cameraMap[item.CameraName], item.ExposureTime);
        if (rtn != 0)
        {
            machineLog->write(item.CameraName + "����ʧ�ܣ��������� ��" + QString::number(rtn), Normal);
            machineLog->write(item.CameraName + "����ʧ�ܣ��������� ��" + QString::number(rtn), Err);
            return rtn;
        }
        m_photo_map[item.PhotoName].Clear();
        m_photo_map[item.PhotoName] = m_cameraMap[item.CameraName]->m_outImg.Clone();
        machineLog->write("���ճɹ�", Normal);
        //��Դ����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //�رչ�Դ
        setDO(lightDo, 0);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //�׶��������Ҫͬʱ�����¹�Դ
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        if (m_saveImageEnabled)
        {
            rs_FunctionTool funtool;
            funtool.saveOriginalPhotoToFile(m_photo_map[item.PhotoName], LSM->m_originalImgPath, item.PhotoName);
        }
        break;
    }
    default:
        break;
    }



    return 0;
}

// 2D�����������
int rs_motion::catPhoto2D(std::string cameraName, std::shared_ptr<CMvCamera>& camera, int exposureTime)
{
    int rtn = 0;
    //����ʵ��
    if (camera == nullptr)
    {    
        // �����豸ʵ��
        camera = std::make_unique<CMvCamera>();
    }


    machineLog->write(" camera->m_bOpenDevice = " + QString::number(camera->m_bOpenDevice), Normal);
    //���豸
    rtn = camera->OpenDevices_Action(cameraName);
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " ������豸ʧ�ܣ���", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " ������豸ʧ�ܣ���", Err);
        return rtn;
    }

    if (exposureTime > 0)
    {
        rtn = camera->SetExposureTime(exposureTime);
        if (rtn != 0)
        {
            machineLog->write(" �����ع�ʱ��ʧ�ܣ���", Normal);
        }
    }
    

    //��ʼ�ɼ�
    rtn = camera->StartGrabbing_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " ��ʼ�ɼ�ʧ�ܣ���", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " ��ʼ�ɼ�ʧ�ܣ���", Err);
        return rtn;
    }
   
    //��ʼ����
    rtn = camera->TakePhoto_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " ����ʧ�ܣ���", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " ����ʧ�ܣ���", Err);
        return rtn;
    }

    //ֹͣ�ɼ�
    rtn = camera->StopGrabbing_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " ֹͣ�ɼ�ʧ�ܣ���", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " ֹͣ�ɼ�ʧ�ܣ���", Err);
        return rtn;
    }

    camera->m_outImg.Clear();
    camera->m_outImg = camera->convertToHalconImage(camera->m_pSaveImageBuf, camera->m_stImageInfo.nWidth, camera->m_stImageInfo.nHeight, 1);

    ////����ͼƬ
    //rtn = camera->SaveImage_Action();
    //if (rtn != 0)
    //{
    //    machineLog->write("2D��Ƭ����ʧ�ܣ���", Normal);
    //    return rtn;
    //}
    return 0;

}

// ��ȡ��ɵ��ļ�·��
QString rs_motion::getOldestFileInDirectory(const QString& directoryPath) {
    QDir dir(directoryPath);
    if (!dir.exists()) {
        machineLog->write("û���ҵ���·����" + directoryPath, Normal);
        return QString();
    }

    // ��ȡĿ¼�е������ļ�
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    if (fileList.isEmpty()) {
        machineLog->write("��·�������ļ���" + directoryPath, Normal);
        return QString();
    }

    // ��ʼ����ɵ��ļ����޸�ʱ��
    QFileInfo oldestFile = fileList.first();
    QDateTime oldestDateTime = oldestFile.lastModified();

    // �����ļ��б��ҵ��޸�ʱ����ɵ��ļ�
    for (const QFileInfo& fileInfo : fileList) {
        QDateTime fileDateTime = fileInfo.lastModified();
        if (fileDateTime < oldestDateTime) {
            oldestDateTime = fileDateTime;
            oldestFile = fileInfo;
        }
    }

    // ��������ļ�������·��
    return oldestFile.absoluteFilePath();
}

// ����ļ���
bool rs_motion::deleteAllFilesInDirectory(const QString& directoryPath) {

    QDir dir(directoryPath);
    if (!dir.exists()) {
        qWarning() << "Directory does not exist:" << directoryPath;
        return false;
    }

    if (!dir.isReadable()) {
        qWarning() << "Directory is not readable:" << directoryPath;
        return false;
    }

    // ��ȡĿ¼�е������ļ�
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    if (fileList.isEmpty()) {
        return true; // Ŀ¼Ϊ�գ�����ɾ��
    }

    // �����ļ��б����ɾ���ļ�
    bool success = true;
    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        if (!QFile::remove(filePath)) {
            success = false;
        }
    }

    return success;
}

// pin���չʾ isNG:���������Ƿ����NGƷ
int rs_motion::pinShowResult(Run_Result& reslut)
{
    int rtn = 0;
    //machineLog->write("pin�������...", Normal);
    if (!m_pinResult2D.isEmpty() && m_pinResult3D.isEmpty())
    {
        //��2D�����ʾ
        for (int i = 0; i < m_pinResult2D.size(); i++)
        {
            emit showPin_signal(m_pinResult2D[i].first, m_pinResult2D[i].second);    //����2Dpin��
            if (m_pinResult2D[i].first.hv_Ret == 0)
                reslut.isNG_2D = true;
        }
    }
    else if (!m_pinResult3D.isEmpty() && m_pinResult2D.isEmpty())
    {
        //��3D�����ʾ
        machineLog->write("33333 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
        for (int i = 0; i < m_pinResult3D.size(); i++)
        {
            emit showPin3D_signal(m_pinResult3D[i].first, m_pinResult3D[i].second);    //����3Dpin��

            if (m_pinResult3D[i].first.isOK == false)
                reslut.isNG_3D = true;

            //isNG = false;
        }
    }
    else if (!m_pinResult2D.isEmpty() && !m_pinResult3D.isEmpty())
    {
        // ��Ͻ����2D��3D���ǿգ�
        std::unordered_map<QString, std::pair<ResultParamsStruct_Pin3DMeasure, PointItemConfig>> product3DMap;
        for (const auto& result3D : m_pinResult3D) {
            //��ȡ��3D���н��
            product3DMap[result3D.second.ProductName] = result3D;
        }
        std::unordered_set<QString> allSame;
        for (const auto& result2D : m_pinResult2D) {
            const QString& productName = result2D.second.ProductName;
            auto it = product3DMap.find(productName);
            if (it != product3DMap.end()) {
                //�ϲ���ʾ
                emit showPin2Dand3D_signal(result2D.first, result2D.second, it->second.first, it->second.second);
                allSame.insert(productName);
                if (result2D.first.hv_Ret == 0)
                    reslut.isNG_2D = true;
                if (it->second.first.isOK == false)
                    reslut.isNG_3D = true;
            }
            else {
                emit showPin_signal(result2D.first, result2D.second);   //����2Dpin��
                if (result2D.first.hv_Ret == 0)
                    reslut.isNG_2D = true;
            }
        }

        for (auto result3D : m_pinResult3D)
        {
            //��ʾû����Ҫ�ϲ���3Dͼ
            if (!allSame.count(result3D.second.ProductName))
            {
                emit showPin3D_signal(result3D.first, result3D.second);    //����3Dpin��
                if (result3D.first.isOK == false)
                    reslut.isNG_3D = true;
            }
        }
    }

    if (reslut.isNG_2D || reslut.isNG_3D || reslut.isNG_Blob || reslut.isNG_Circle || reslut.isNG_OneDimensional
        || reslut.isNG_Temple || reslut.isNG_TwoDimensional || reslut.isNG_CircleMeasure)
        reslut.isNG = true;

    ////�ع�
    //reslut.isNG = false;

    if (!m_resultMergingImg.empty())
    {
        //��ʾ�ϲ���ͼƬ
        for (auto message : m_resultMergingImg)
        {
            emit showVision_signal(message.second);
        }
        //emit showAllVision_signal(m_resultMergingImg);
    }

    return 0;
}

//3D�豸��ʼ��
void rs_motion::init3dCamera()
{
    try {
        if (!m_pcMyCamera_3D)
        {
            // �����豸ʵ��
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
            if (m_pcMyCamera_3D == nullptr)
                return;
            //�豸��ʼ��
            m_pcMyCamera_3D->InitCamera3DLink();
            //QTime time = QTime::currentTime();
            //while (fabs(QTime::currentTime().secsTo(time)) < 20)
            //{
            //    //��ȴ�20��
            //    if (m_isEmergency.load())
            //        return;
            //    //�ж��Ƿ��ʼ�����
            //    if (m_pcMyCamera_3D->m_initSucess)
            //        break;
            //}
        }
    }
    catch (...) {
        machineLog->write("3d�����ʼ���쳣", Normal);
        return;
    }
}


// ���� 3D ���Ƶ��߼�����
void rs_motion::process3Didentify(pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPoint, PointItemConfig item)
{
    int rtn = 0;
    if (m_isEmergency.load())
    {
        return;
    }
    if (!cloudPoint->empty())
    {
        if (m_pcMyCamera_3D == nullptr)
        {
            machineLog->write("û�д���3D���..", Normal);
            return;
        }
        auto cloud = cloudPoint->makeShared();
        if (cloud->empty())
        {
            machineLog->write("û���ҵ��䷽ " + item.Vision + " ��pin����..", Normal);
            return;
        }
        // ��pin
        ResultParamsStruct_Pin3DMeasure result;
        result.depthImg = m_pcMyCamera_3D->depth_color_img.clone();
        // ����ͼƬ����
        machineLog->write("- - - ->>>>>>>>��ʼ����3DͼƬ", Normal);

        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        //�Ľڵ���
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };

        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString exePath = QCoreApplication::applicationDirPath();
        QString configDirPath = exePath + "/3D_formulation";
        QString xmlDirPath = configDirPath + "/" + filename;
        if (!QFile::exists(xmlDirPath)) {
            machineLog->write(QString("3D�䷽�ļ�ȱʧ: %1").arg(xmlDirPath), Normal);
            return;
        }


#ifdef DEBUG_LHC
        if (m_pcMyCamera_3D == nullptr)
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
        //����ͼ
        std::string str = "D:/��Զ/3D/data/cloud_ori.pcd";
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloudtemp(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::io::loadPCDFile(str, *cloudtemp);
        // ���� PinWindow ʵ����ʹ�õڶ������캯��
        Pin3DMeasureWindow pin_3d(cloudtemp, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
        pin_3d.hide();
        pin_3d.ori_cloud = cloudtemp;
        pin_3d.LoadShowCloud(cloudtemp);
#else
        auto Pin3DMeasureParams1 = new Pin3DMeasure(modubleName, modubleID, processID);
        auto modubleDatas1 = new modubleDatas();
        // ���� PinWindow ʵ����ʹ�õڶ������캯��
        //Pin3DMeasureWindow pin_3d(cloud, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
        //pin_3d.hide();

#endif // DEBUG_LHC

        //// ��ȡ�ļ���
        //pin_3d.ConfigPath = configDirPath.toUtf8().constData();
        //pin_3d.modubleDatas1->setGetConfigPath(pin_3d.ConfigPath);
        //pin_3d.XmlPath = xmlDirPath.toUtf8().constData();
        //pin_3d.modubleDatas1->setConfigPath(pin_3d.XmlPath);
        // ������
        modubleDatas1->setGetConfigPath(configDirPath.toUtf8().constData());
        modubleDatas1->setConfigPath(xmlDirPath.toUtf8().constData());
        ErrorCode_Xml errorCode;
        RunParamsStruct_Pin3DMeasure RunParamsPin3DMeasure;
        errorCode = modubleDatas1->ReadParams_Pin3DMeasure(RunParamsPin3DMeasure, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("3D������ȡʧ��..", Normal);
            if (Pin3DMeasureParams1)
                delete Pin3DMeasureParams1;
            if (modubleDatas1)
                delete modubleDatas1;
            return;
        }

        rtn = Pin3DMeasureParams1->PinMeasureAlgo(cloud, RunParamsPin3DMeasure, result);
        if (rtn != 0)
        {
            machineLog->write("3D����pin��ʧ��..", Normal);
            if (Pin3DMeasureParams1)
                delete Pin3DMeasureParams1;
            if (modubleDatas1)
                delete modubleDatas1;
            return;
        }
        result.depthImg = m_pcMyCamera_3D->depth_color_img;
        machineLog->write(" result.PinFlag.size() = " + QString::number(result.PinFlag.size()), Normal);
        //ȡ���
        m_pinResult3D.push_back(std::make_pair(result, item));
        //�鿴�Ƿ�����Ҫ������CSV������
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = result.isOK? "OK" : "NG";
        }

        if (Pin3DMeasureParams1)
            delete Pin3DMeasureParams1;
        if (modubleDatas1)
            delete modubleDatas1;
    }
    else
    {
        machineLog->write("- - - ->>>>>>>>��ȡ��3D����Ϊ��", Normal);
    }

    return;
}


// �ϲ�3D�����
void rs_motion::summary3dResult()
{
    machineLog->write("�ϲ�3D�����..", Normal);
    machineLog->write("11111 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
    for (int i = 0; i < m_pinResult3D.size(); i++)
    {
        for (int j = m_pinResult3D.size() - 1; j > i; j--)
        {
            QString res1 = m_pinResult3D[i].second.ProductName;
            QString res2 = m_pinResult3D[j].second.ProductName;
            // ȷ���Ƿ�ʹ�õ�ͬһ��ͷ��Ȼ��ϲ����
            if (res1 == res2 && m_pinResult3D[i].first.PinFlag.size() == m_pinResult3D[j].first.PinFlag.size())
            {
                for (int z = 0; z < m_pinResult3D[i].first.PinHeight.size(); z++)
                {
                    //if (m_pinResult3D[i].first.PinHeight[z] == -2 && m_pinResult3D[j].first.PinHeight[z] != -2)
                    //{
                    //    //���ȡ����Ľ��
                    //    m_pinResult3D[i].first.PinHeight[z] = m_pinResult3D[j].first.PinHeight[z];
                    //    m_pinResult3D[i].first.PinFlag[z] = m_pinResult3D[j].first.PinFlag[z];
                    //}
                    if (m_pinResult3D[i].first.PinHeight[z] == 0 && m_pinResult3D[j].first.PinHeight[z] != 0)
                    {
                        //���ȡ����Ľ��
                        m_pinResult3D[i].first.PinHeight[z] = m_pinResult3D[j].first.PinHeight[z];
                        m_pinResult3D[i].first.PinFlag[z] = m_pinResult3D[j].first.PinFlag[z];
                    }
                }
                m_pinResult3D.removeAt(j);
            }
            
        }
    }

    for (int i = 0; i < m_pinResult3D.size(); i++)
    {
        //����3D���ݽ��NG����OK
        m_pinResult3D[i].first.isOK = true;
        for (int j = 0; j < m_pinResult3D[i].first.PinFlag.size(); j++)
        {
            //������
            if (m_pinResult3D[i].first.PinFlag[j] == false)
            {
                m_pinResult3D[i].first.isOK = false;
                break;
            }
        }
    }
    machineLog->write("22222 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
}

// ���� 2D ʶ����߼�����
void rs_motion::process2Didentify(HObject img, PointItemConfig item)
{
    if (!img.IsInitialized())
    {
        return;
    }
    if (m_isEmergency.load())
    {
        return;
    }

    int rtn = 0;
    QString exePath = QCoreApplication::applicationDirPath();
    //int processID = 1;
    //int modubleID = 2;
    //std::string modubleName = "Module1";
    int processID = 0;
    int modubleID = 0;
    std::string modubleName = item.Vision.toStdString();
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
    };
    // ��ȡ��Ӧ���䷽·��
    QString filename = item.Vision + ".xml";
    QString configDirPath = exePath + "/2D_formulation";
    QString xmlDirPath = configDirPath + "/" + filename;

    //// ���� PinWindow ʵ����ʹ�õڶ������캯��
    //PinWindow pin_2d(img, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
    ////pin_2d.hide();

    //// ��ȡ�ļ���
    //pin_2d.ConfigPath = configDirPath.toUtf8().constData();
    //pin_2d.dllDatas1->setGetConfigPath(pin_2d.ConfigPath);
    //pin_2d.XmlPath = xmlDirPath.toUtf8().constData();
    //pin_2d.dllDatas1->setConfigPath(pin_2d.XmlPath);
    //// ��pin
    //rtn = pin_2d.slot_PinDetect();

    auto dllDatas1 = new DllDatas();
    auto PinDetect1 = new PinDetect();
    try {
        int ret = 1;
        //XML��ȡ����
        dllDatas1->setConfigPath(xmlDirPath.toUtf8().constData());
        dllDatas1->setGetConfigPath(configDirPath.toUtf8().constData());
        RunParamsStruct_Pin RunParams_Pin;
        ErrorCode_Xml errorCode = dllDatas1->ReadParams_PinDetect(RunParams_Pin, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml) {
            //XML��ȡʧ��
            if (dllDatas1)
                delete dllDatas1;
            if (PinDetect1)
                delete PinDetect1;
            return;
        }
        //�����㷨
        PinDetect1->resultParams.ngIdx.clear(); //��һ��ngģ��
        int resultTP = PinDetect1->PinDetectFunc(img, RunParams_Pin, PinDetect1->resultParams);
    }
    catch (...) {
        if (dllDatas1)
            delete dllDatas1;
        if (PinDetect1)
            delete PinDetect1;
        return ;
    }

    if (rtn == -1)
    {
        machineLog->write("����pin��ʧ��..", Normal);
        if (dllDatas1)
            delete dllDatas1;
        if (PinDetect1)
            delete PinDetect1;
        return;
    }
    auto result = PinDetect1->resultParams;
    //ȡ�����ͼƬ
    result.ho_Image = img;
    //ȡ���
    m_pinResult2D.push_back(std::make_pair(result, item));
    //�鿴�Ƿ�����Ҫ������CSV������
    auto isOK = result.hv_Ret == 1;
    if (m_ResultToCsv.count(item.Tip))
    {
        m_ResultToCsv[item.Tip] = isOK ? "OK" : "NG";
    }
    machineLog->write("����pin�����", Normal);

    if (dllDatas1)
        delete dllDatas1;
    if (PinDetect1)
        delete PinDetect1;
    return;
}

// �������
void rs_motion::destroyCamera()
{
    if (m_pcMyCamera_3D)
    {
        //�ر��豸
        m_pcMyCamera_3D->StopProgram();
        //m_pcMyCamera_3D.reset();
    }

    //��������ֵ�
    for (auto& camera : m_cameraMap)
    {
        if (camera.second)
        {
            camera.second->StopGrabbing_Action();
            camera.second->CloseDevice_Action();
        }
    }
    m_cameraMap.clear();
}

// ˫��pin�������
int rs_motion::pin2MotionAction(PointItemConfig item)
{
    int rtn = 0;
    QString axisY = item.StationL == ENUMSTATION::L ? LS_AxisName::Y1 : LS_AxisName::Y2;
    //QString axisU = item.StationL == ENUMSTATION::L ? LS_AxisName::U1 : LS_AxisName::U2;

    //250717 ����λ��ת�ӻ�ȥ
    QString axisU = LS_AxisName::U1;

    //�ж�Z���ƶ�����
    bool isPostiveMove_Z = isMovePostive(LS_AxisName::Z, item.TargetPZ);

    //��ִ��Z���ջض���
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(LS_AxisName::Z, item.TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        WriteAxisSpeedModel(LS_AxisName::Z, (SpeedModeEnum)item.SpeedModel);
    }

    //�������첽ִ��
    rtn = AxisPmove(axisU, item.TargetPU);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(LS_AxisName::RZ, item.TargetPRZ);
    if (rtn != 0)
    {
        machineLog->write("����ִ��RZ�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(LS_AxisName::X, item.TargetPX);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY, item.TargetPY);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y�ᶯ�����󣡣�" , Normal);
        return rtn;
    }
    //������������ٶ�
    for (auto axis : LS_AxisName::allAxis)
    {
        WriteAxisSpeedModel(axis, (SpeedModeEnum)item.SpeedModel);
    }

    QTime time = QTime::currentTime();

    //250717 ����λ��ת�ӻ�ȥ
    //�ȴ������ᵽλ
    while (isRuning(LS_AxisName::X)
        || isRuning(axisY)
        || isRuning(LS_AxisName::RZ)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 30�볬ʱ����ʱԤ��   
        if (time.secsTo(QTime::currentTime()) > 30)
        {
            machineLog->write("�ᵽλ��ʱ����", Normal);
            break;
        }
    }

    ////�ȴ������ᵽλ
    //while (isRuning(LS_AxisName::X)
    //    || isRuning(axisY)
    //    || isRuning(LS_AxisName::RZ))
    //{
    //    if (m_isEmergency.load())
    //    {
    //        machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
    //        return -1;
    //    }

    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //    // 30�볬ʱ����ʱԤ��   
    //    if (time.secsTo(QTime::currentTime()) > 30)
    //    {
    //        machineLog->write("�ᵽλ��ʱ����", Normal);
    //        break;
    //    }
    //}

    //��ִ��Z�������λ����
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(LS_AxisName::Z, item.TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    return 0;
}


// ���Npin�������
int rs_motion::pinLsAction(PointItemConfig item)
{
    int rtn = 0;
    const QString axisX = LS_AxisName::LS_X;
    const QString axisY1 = LS_AxisName::LS_Y1;
    const QString axisY2 = LS_AxisName::LS_Y2;
    const QString axisU = LS_AxisName::LS_U;
    const QString axisZ = LS_AxisName::LS_Z;

    const double TargetPX = item.TargetPAxis_1;
    const double TargetPY1 = item.TargetPAxis_2;
    const double TargetPY2 = item.TargetPAxis_3;
    const double TargetPU = item.TargetPAxis_4;
    const double TargetPZ = item.TargetPAxis_5;

    //�����������ٶ�
    auto model = (SpeedModeEnum)item.SpeedModel;
    for (auto axis : LS_AxisName::allAxis)
    {
        double setspeed = m_Axis[axis].minSpeed;
        switch (model)
        {
        case SpeedModeEnum::Low:
            setspeed = m_Axis[axis].minSpeed;
            break;
        case SpeedModeEnum::Mid:
            setspeed = m_Axis[axis].midSpeed;
            break;
        case SpeedModeEnum::High:
            setspeed = m_Axis[axis].maxSpeed;
            break;
        default:
            break;
        }
        setSpeed(axis, setspeed);
    }

    //�ж�Z���ƶ�����
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);

    //��ִ��Z���ջض���
    if (!isPostiveMove_Z)
    {
        //rtn = AxisPmoveAsync(axisZ, TargetPZ);
        //if (rtn != 0)
        //{
        //    machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
        //    return rtn;
        //}
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);

        rtn = AxisPmove(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        //�ȴ������ᵽλ
        while (isRuning(axisZ))
        {
            if (m_isEmergency.load())
            {
                machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
                return -1;
            }
            if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
            {
                m_isEmergency.store(true);
                machineLog->write("ȡ������գ���", Normal);
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    //�������첽ִ��
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY1, TargetPY1);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y1�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY2, TargetPY2);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y2�ᶯ�����󣡣�", Normal);
        return rtn;
    }
    ////������������ٶ�
    //for (auto axis : LS_AxisName::allAxis)
    //{
    //    WriteAxisSpeedModel(axis, (SpeedModeEnum)item.SpeedModel);
    //}

    QTime time = QTime::currentTime();
    //�ȴ������ᵽλ
    while (isRuning(axisX)
        || isRuning(axisY1)
        || isRuning(axisY2)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 30�볬ʱ����ʱԤ��   
        if (time.secsTo(QTime::currentTime()) > 30)
        {
            machineLog->write("�ᵽλ��ʱ����", Normal);
            break;
        }
        if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
        {
            m_isEmergency.store(true);
            machineLog->write("ȡ������գ���", Normal);
            return -1;
        }
    }

    //��ִ��Z�������λ����
    if (isPostiveMove_Z)
    {
        //rtn = AxisPmoveAsync(axisZ, TargetPZ);
        //if (rtn != 0)
        //{
        //    machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
        //    return rtn;
        //}
        rtn = AxisPmove(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        //�ȴ������ᵽλ
        while (isRuning(axisZ))
        {
            if (m_isEmergency.load())
            {
                machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
                return -1;
            }
            if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
            {
                m_isEmergency.store(true);
                machineLog->write("ȡ������գ���", Normal);
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    return 0;
}

// �׶���pin�������
int rs_motion::pinEdAction(PointItemConfig item)
{
    int rtn = 0;
    const QString axisX = LS_AxisName::X;
    const QString axisY = LS_AxisName::Y;
    const QString axisU = LS_AxisName::U;
    const QString axisU1 = LS_AxisName::U1;
    const QString axisZ = LS_AxisName::Z;
    const QString axisZ1 = LS_AxisName::Z1;
    const QString axisZ2 = LS_AxisName::Z2;

    const double TargetPX = item.TargetPAxis_1;
    const double TargetPY = item.TargetPAxis_2;
    const double TargetPU = item.TargetPAxis_3;
    const double TargetPU1 = item.TargetPAxis_4;
    const double TargetPZ = item.TargetPAxis_5;
    const double TargetPZ1 = item.TargetPAxis_6;
    const double TargetPZ2 = item.TargetPAxis_7;

    //�����������ٶ�
    auto model = (SpeedModeEnum)item.SpeedModel;
    for (auto axis : LS_AxisName::allAxis)
    {
        double setspeed = m_Axis[axis].minSpeed;
        switch (model)
        {
        case SpeedModeEnum::Low:
            setspeed = m_Axis[axis].minSpeed;
            break;
        case SpeedModeEnum::Mid:
            setspeed = m_Axis[axis].midSpeed;
            break;
        case SpeedModeEnum::High:
            setspeed = m_Axis[axis].maxSpeed;
            break;
        default:
            break;
        }
        setSpeed(axis, setspeed);
    }

    //�ж�Z���ƶ�����
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //��ִ��Z���ջض���
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //�ж�Z1���ƶ�����
    bool isPostiveMove_Z1 = isMovePostive(axisZ1, TargetPZ1);
    //��ִ��Z1���ջض���
    if (!isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z1���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //�ж�Z2���ƶ�����
    bool isPostiveMove_Z2 = isMovePostive(axisZ2, TargetPZ2);
    //��ִ��Z1���ջض���
    if (!isPostiveMove_Z2)
    {
        rtn = AxisPmoveAsync(axisZ2, TargetPZ2);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z2���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //�������첽ִ��
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisU1, TargetPU1);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U1�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //�ȴ������ᵽλ
    while (isRuning(axisX)
        || isRuning(axisY)
        || isRuning(axisU1)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60�볬ʱ����ʱԤ��   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("�ᵽλ��ʱ����", Normal);
            break;
        }
    }

    //��ִ��Z�������λ����
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z1������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z2)
    {
        rtn = AxisPmoveAsync(axisZ2, TargetPZ2);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z2������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    return 0;
}

// ��ʼ���ᣬIO��
void rs_motion::initAxisAndIO()
{
    //��ȷ�ϰ汾
    switch (m_version)
    {
    case ENUM_VERSION::TwoPin:
        //˫��Pin��汾
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderL1);  // �ξ�1����1
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderL2);  // �ξ�1����2
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderR1);  // �ξ�2����1
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderR2);  // �ξ�2����2
        LS_DO::allDO.push_back(LS_DO::DO_Lock1);            // ������1
        LS_DO::allDO.push_back(LS_DO::DO_Lock2);            // ������2
        LS_DO::allDO.push_back(LS_DO::DO_Lock3);            // ������3
        LS_DO::allDO.push_back(LS_DO::DO_LightFlood);       // ������
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D1);       // 3D����-X
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D2);       // 3D����-Y1
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D3);       // 3D����-Y2
        LS_DO::allDO.push_back(LS_DO::DO_ReadCodeTrigger1); // ɨ��ǹ����1
        LS_DO::allDO.push_back(LS_DO::DO_ReadCodeTrigger2); // ɨ��ǹ����2
        LS_DO::allDO.push_back(LS_DO::DO_LightOK_L);        // ָʾ��1-��
        LS_DO::allDO.push_back(LS_DO::DO_LightNG_L);        // ָʾ��1-��
        LS_DO::allDO.push_back(LS_DO::DO_LightOK_R);        // ָʾ��2-��
        LS_DO::allDO.push_back(LS_DO::DO_LightNG_R);        // ָʾ��2-��
        break;
    case ENUM_VERSION::EdPin:
        //�׶���汾
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y);
        LS_AxisName::allAxis.push_back(LS_AxisName::U);
        LS_AxisName::allAxis.push_back(LS_AxisName::U1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z2);

        //DO���пɸ��õģ�ֱ����ӣ�
        LS_DO::allDO.push_back(LS_DO::ED_DO_PositionCyl1);     
        LS_DO::allDO.push_back(LS_DO::ED_DO_PositionCyl2);     
        LS_DO::allDO.push_back(LS_DO::ED_DO_ClampCyl1_Valve1); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_ClampCyl1_Valve2); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_CamFrontTrigger);  
        LS_DO::allDO.push_back(LS_DO::ED_DO_CamRearTrigger);   
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightFrontTrigger);
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightRearTrigger); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightPositioningTrigger);
        // �����������ź�ע��
        LS_DO::allDO.push_back(LS_DO::ED_DO_FeedBeltStart);               // ����Ƥ������
        LS_DO::allDO.push_back(LS_DO::ED_DO_DischargeBeltStart);          // ����Ƥ������
        LS_DO::allDO.push_back(LS_DO::ED_DO_NGBeltStart);                 // NGƤ������

        //DI
        LS_DI::allDI.clear();
        // ��ť��
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnStart);          // ������ť
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnStop);           // ֹͣ��ť
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnReset);          // ��λ��ť
        LS_DI::allDI.push_back(LS_DI::ED_DI_SwitchAutoMode);    // �Զ�ģʽ��ť
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnEStop1);        // ��ͣ��ť1
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnEStop2);        // ��ͣ��ť2

        // ����λ�ü��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl1_Home); // ��λ����1ԭ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl1_Work); // ��λ����1����
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl2_Home); // ��λ����2ԭ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl2_Work); // ��λ����2����
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl1_Home);    // �н�����1ԭ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl1_Work);    // �н�����1����
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl2_Home);    // �н�����2ԭ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl2_Work);    // �н�����2����
        // ��ȫ�ż��
        LS_DI::allDI.push_back(LS_DI::ED_DI_ProdDetect);        // ��Ʒ���޼��
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed1);      // ���ϲలȫ��1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed2);      // ���ϲలȫ��2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed3);      // ���ϲలȫ��3
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed4);      // ���ϲలȫ��4
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge1); // ���ϲలȫ��1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge2); // ���ϲలȫ��2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge3); // ���ϲలȫ��3
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge4); // ���ϲలȫ��4
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_NG1);        // NGƤ���లȫ��1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_NG2);        // NGƤ���లȫ��2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Rear1);      // ���氲ȫ��1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Rear2);      // ���氲ȫ��2
        // �����ź�
        LS_DI::allDI.push_back(LS_DI::ED_DI_AirPressureAlarm);   // CDA����ѹ������
        // Ƥ��������ź�ע��
        LS_DI::allDI.push_back(LS_DI::ED_DI_FeedBeltMaterialDetect);      // ����Ƥ����Ʒ���ϼ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_FeedBeltArrivalDetect);       // ����Ƥ����Ʒ��λ���
        LS_DI::allDI.push_back(LS_DI::ED_DI_DischargeBeltReleaseDetect);  // ����Ƥ����Ʒ���ϼ��
        LS_DI::allDI.push_back(LS_DI::ED_DI_DischargeBeltArrivalDetect);  // ����Ƥ����Ʒ��λ���
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltReleaseDetect_Left);    // NGƤ����Ʒ���ϼ��1����
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltReleaseDetect_Right);   // NGƤ����Ʒ���ϼ��2���ң�
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltFullDetect);            // NGƤ����Ʒ���ϼ��
        break;
    case ENUM_VERSION::LsPin:
        //���N�汾
        //����
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::LS_X);
        LS_AxisName::allAxis.push_back(LS_AxisName::LS_Y1);
        LS_AxisName::allAxis.push_back(LS_AxisName::LS_Y2);
        LS_AxisName::allAxis.push_back(LS_AxisName::LS_U);
        LS_AxisName::allAxis.push_back(LS_AxisName::LS_Z);
        //DI
        LS_DI::allDI.clear();
        LS_DI::allDI.push_back(LS_DI::LS_IN_BtnSetUp);    
        LS_DI::allDI.push_back(LS_DI::LS_IN_BtnStop);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_BtnReset);    
        LS_DI::allDI.push_back(LS_DI::LS_IN_BtnEmergency);
        LS_DI::allDI.push_back(LS_DI::LS_IN_BtnLight);    
        LS_DI::allDI.push_back(LS_DI::LS_IN_MainAirSupply);
        LS_DI::allDI.push_back(LS_DI::LS_IN_ClampCyl1_Home); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_ClampCyl1_Work); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_ClampCyl2_Home); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_ClampCyl2_Work); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_VacuumDetect);   
        LS_DI::allDI.push_back(LS_DI::LS_IN_MaterialDetect); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateFront1);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateFront2);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateFront3);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateFront4);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateRear1);      
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateRear2);      
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateRear3);      
        LS_DI::allDI.push_back(LS_DI::LS_IN_GateRear4);   
        LS_DI::allDI.push_back(LS_DI::LS_IN_SMEMA_UpFront_AllowOut);  
        LS_DI::allDI.push_back(LS_DI::LS_IN_SMEMA_LowFront_AllowOut); 
        LS_DI::allDI.push_back(LS_DI::LS_IN_SMEMA_UpRear_AllowOut);   
        LS_DI::allDI.push_back(LS_DI::LS_IN_SMEMA_LowRear_AllowOut);  
        LS_DI::allDI.push_back(LS_DI::LS_IN_UpperMaterialDetect);     
        LS_DI::allDI.push_back(LS_DI::LS_IN_UpperOutputDetect);       
        LS_DI::allDI.push_back(LS_DI::LS_IN_LowerOutputDetect);       
        LS_DI::allDI.push_back(LS_DI::LS_IN_BlockCyl1_Work);          
        LS_DI::allDI.push_back(LS_DI::LS_IN_BlockCyl1_Home);          
        LS_DI::allDI.push_back(LS_DI::LS_IN_BlockCyl2_Work);          
        LS_DI::allDI.push_back(LS_DI::LS_IN_BlockCyl2_Home);          
        LS_DI::allDI.push_back(LS_DI::LS_IN_LiftCyl_Work);            
        LS_DI::allDI.push_back(LS_DI::LS_IN_LiftCyl_Home);  
        LS_DI::allDI.push_back(LS_DI::LS_IN_LowerMaterialDetect);
        LS_DI::allDI.push_back(LS_DI::LS_IN_UpperCheckProduct);
        
        //DO
        // �������
        LS_DO::allDO.clear();
        LS_DO::allDO.push_back(LS_DO::LS_OUT_RedLight);       
        LS_DO::allDO.push_back(LS_DO::LS_OUT_YellowLight);    
        LS_DO::allDO.push_back(LS_DO::LS_OUT_GreenLight);     
        LS_DO::allDO.push_back(LS_DO::LS_OUT_Buzzer);         
        LS_DO::allDO.push_back(LS_DO::LS_OUT_StartBtnLED);    
        LS_DO::allDO.push_back(LS_DO::LS_OUT_StopBtnLED);     
        LS_DO::allDO.push_back(LS_DO::LS_OUT_ResetBtnLED);    
        LS_DO::allDO.push_back(LS_DO::LS_OUT_LightBtnLED);    
        LS_DO::allDO.push_back(LS_DO::LS_OUT_ClampSuck);     
        LS_DO::allDO.push_back(LS_DO::LS_OUT_ClampBlow);      
        LS_DO::allDO.push_back(LS_DO::LS_OUT_Lighting);       
        LS_DO::allDO.push_back(LS_DO::LS_OUT_ClampCyl1);      
        LS_DO::allDO.push_back(LS_DO::LS_OUT_ClampCyl2);      
        LS_DO::allDO.push_back(LS_DO::LS_OUT_2DLightTrigger); 
        LS_DO::allDO.push_back(LS_DO::LS_OUT_2DCamTrigger);   
        LS_DO::allDO.push_back(LS_DO::LS_OUT_3DCamTrigger);   
        LS_DO::allDO.push_back(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady);    // Q6.0
        LS_DO::allDO.push_back(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived); // Q6.1
        LS_DO::allDO.push_back(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady);     // Q6.2
        LS_DO::allDO.push_back(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady);    // Q6.3
        LS_DO::allDO.push_back(LS_DO::LS_OUT_UpperMotor_Run);             // Q6.4
        LS_DO::allDO.push_back(LS_DO::LS_OUT_UpperMotor_Dir);             // Q6.5
        LS_DO::allDO.push_back(LS_DO::LS_OUT_LowerMotor_Run);             // Q6.6
        LS_DO::allDO.push_back(LS_DO::LS_OUT_LowerMotor_Dir);             // Q6.7
        LS_DO::allDO.push_back(LS_DO::LS_OUT_BlockCyl_Activate);          // Q7.0
        LS_DO::allDO.push_back(LS_DO::LS_OUT_LiftCyl_Activate);           // Q7.1
        break;
    case ENUM_VERSION::BtPin:
        //��̩�汾
        //����
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y2);
        break;
    case ENUM_VERSION::JmPin:
        //����pin�汾
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::X1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y);
        LS_AxisName::allAxis.push_back(LS_AxisName::U);
        LS_AxisName::allAxis.push_back(LS_AxisName::U1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::JmHan:
        //��������汾
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y1);
        LS_AxisName::allAxis.push_back(LS_AxisName::U);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z);
        break;
    default:
        break;
    }
}

// ���������Ƽ����ϼ��
int rs_motion::feedingAction()
{
    if (m_version == LsPin)
    {
        QElapsedTimer materialTimer;
        //���N�汾�ı�������������
        //�ȴ�����������
        while (getDI(LS_DI::LS_IN_UpperMaterialDetect) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //��Ҫ���ź�
        setDO(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady, 1);
        //���ǰ����������ź�
        while (getDI(LS_DI::LS_IN_SMEMA_UpFront_AllowOut) == 0 )
        {
            //ģ���ź�
            if (LSM->inDI)
                break;
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //�򿪵��
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 1);
        //��������ź�
        while (getDI(LS_DI::LS_IN_UpperMaterialDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(500);
        ////ģ��
        //Sleep(5000);
        //�ر�Ҫ���ź�
        setDO(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady, 0);
        //�رյ��
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        Sleep(500);
        //����������
        setDO(LS_DO::LS_OUT_LiftCyl_Activate, 1);
        //����������׶����ź�
        materialTimer.start();  // ��ʼ��ʱ
        while (getDI(LS_DI::LS_IN_LiftCyl_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("�������׶����ⳬʱ��10����δ��ɶ�����������", Normal);
                machineLog->write("�������׶����ⳬʱ��10����δ��ɶ�����������", Err);
                return -1;
            }
            Sleep(5);
        }
        //����Ƿ�����
        while (getDI(LS_DI::LS_IN_UpperCheckProduct) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 3000) {
                machineLog->write("�ؾ����ϼ�ⳬʱ��3����δ��⵽�ؾߣ�������һ��վ", Normal);
                return 10086;
            }
            Sleep(5);
        }
        if (!notQR)
        {
            //tcpɨ��
            ls_tcpMess.clear();
            auto appwindow = Pin2::getAppWindow();
            appwindow->m_clientQRCode.sendMsg("T");
            materialTimer.start();
            while (ls_tcpMess.isEmpty())
            {
                if (m_isEmergency.load())
                    return -1;
                if (!ls_tcpMess.isEmpty())
                    break;
                if (materialTimer.elapsed() > 10000) {
                    machineLog->write("ɨ�볬ʱ��10����δɨ��ɹ���������һ��վ", Normal);
                    return 10086;
                }
                Sleep(5);
            }
            if (ls_tcpMess == "FAIL")
            {
                machineLog->write("ɨ���ȡ��Ϣʧ�ܣ���������һ��վ", Normal);
                return 10086;
            }
            else
            {
                m_snReq.sn = ls_tcpMess;
            }
        }
        else
        {
            //��ɨ���ù̶�SN
            m_snReq.sn = LSM->testSN;
        }
        if (!notMes)
        {
            //m_snReq.sn = "G11101LS2020020101-022504250121";
            
            //�û��˺�
            m_snReq.op = m_productPara.UserAccout;

            // ���ü��ӿ�
            SnCheckResponse res = check_sn_process(
                "http://10.23.164.56:8001/MainWebForm.aspx",
                m_snReq
            );

            // ������
            if (res.code == 200) {
                machineLog->write("SN���ͨ��, �����룺" + res.main_sn, Mes);

            }
            else {
                machineLog->write("SN���ʧ�ܣ��������ݣ�" + res.msg + " ������һ��վ", Mes);
                return 10086;
            }
        }


    }
    return 0;
}

// ���������Ƽ����ϼ��
int rs_motion::unloadingAction()
{
    if (m_version == LsPin)
    {
        //���N�汾�ı�������������
        //���������½�
        setDO(LS_DO::LS_OUT_LiftCyl_Activate, 0);
        //��ⶥ�����赲����ԭ���ź�
        QElapsedTimer materialTimer;
        materialTimer.start();  // ��ʼ��ʱ
        while (getDI(LS_DI::LS_IN_LiftCyl_Home) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("�������׶�ԭ���ⳬʱ��10����δ��ɶ����½�����", Normal);
                machineLog->write("�������׶�ԭ���ⳬʱ��10����δ��ɶ����½�����", Err);
                return -1;
            }
            Sleep(5);
        }
        Sleep(200);
        //�ر��赲
        setDO(LS_DO::LS_OUT_BlockCyl_Activate, 1);
        //����赲���׶����ź�
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_BlockCyl1_Home) == 1)
        {
           if (m_isEmergency.load())
               return -1;
           if (materialTimer.elapsed() > 10000) {
               machineLog->write("�赲���׶������ⳬʱ��10����δ����赲�½�����", Normal);
               machineLog->write("�赲���׶������ⳬʱ��10����δ����赲�½�����", Err);
               return -1;
           }
           Sleep(5);
        }
        //�������ź�
        setDO(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady, 1);
        //��������������ź�
        while (getDI(LS_DI::LS_IN_SMEMA_UpRear_AllowOut) == 0)
        {
            //ģ���ź�
            if (LSM->outDI)
                break;
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //�򿪵��
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 1);
        ////ģ��
        //Sleep(5000);
        //�����Ҫ���źŽ�����ͣ
        while (getDI(LS_DI::LS_IN_SMEMA_UpRear_AllowOut) == 1 || LSM->outDI)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //�������ź�
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_UpperOutputDetect) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("�����������źż�ⳬʱ��10����δ��ɳ��ϣ���", Normal);
                machineLog->write("�����������źż�ⳬʱ��10����δ��ɳ��ϣ���", Err);
                return -1;
            }
            Sleep(5);
        }
        Sleep(500);
        //�رյ��
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        //�ر������ź�
        setDO(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady, 0);
        //���赲
        setDO(LS_DO::LS_OUT_BlockCyl_Activate, 0);
        //����赲���׶����ź�
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_BlockCyl1_Home) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("�赲���׶�ԭ���ⳬʱ��10����δ����赲��������", Normal);
                machineLog->write("�赲���׶�ԭ���ⳬʱ��10����δ����赲��������", Err);
                return -1;
            }
            Sleep(5);
        }

    }
    return 0;
}

// �²㱶��������
int rs_motion::downFeedingAction()
{
    if (LSM->m_cardInitStatus)
    {
        machineLog->write("�²㱶�������ȴ���ʼ����", Normal);
        //�ȴ���ʼ
        while (!m_isStart.load())
        {
            if (m_isEmergency.load() || m_isRunStop.load())
                return -1;
            Sleep(100);
        }
    }
    else
        return -1;
    while (m_isStart.load())
    //while (LSM->m_cardInitStatus)
    {
        machineLog->write("�²㱶��������ʼ����", Normal);
        //����������һֱ���м��
        if (m_isEmergency.load() || m_isRunStop.load())
            return -1;
        //�򿪸��ߺ����������
        setDO(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady, 1);
        machineLog->write("�²㱶���������ߺ���������ϣ��ȴ�����ź�", Normal);
        //�ȴ�����ź�
        while (m_DI[LS_DI::LS_IN_SMEMA_LowRear_AllowOut].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //���²���
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 1);
        machineLog->write("�²㱶���������²������ȴ��²������ź�", Normal);
        //�ȴ��²�����ź�
        while (m_DI[LS_DI::LS_IN_LowerMaterialDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        //�ȴ��²���ȫ�����ź�
        while (m_DI[LS_DI::LS_IN_LowerMaterialDetect].state == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //�ر�Ҫ���ź�
        setDO(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady, 0);
        machineLog->write("�²㱶�������ر�Ҫ���źţ��ȴ��²���ȫ�����ź�", Normal);

        //���Ϻ���1.5��
        Sleep(1500);
        //�ر��²���
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 0);
        //�򿪸���ǰ����������
        setDO(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived, 1);
        machineLog->write("�²㱶�������򿪸���ǰ���������ϣ��ȴ�ǰ���ź�", Normal);
        //�ȴ�ǰ���ź�
        while (m_DI[LS_DI::LS_IN_SMEMA_LowFront_AllowOut].state == 0)
        ////ģ��
        //while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //���²���
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 1);
        //����²�����ź�
        machineLog->write("�²㱶��������ʼ����", Normal);
        //�������ź�
        while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        machineLog->write("�²㱶����������Ƿ���ȫ����", Normal);
        //��ȫ����
        while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        //�رյ��
        setDO(LS_DO::LS_OUT_LowerMotor_Run, 0);
        //�رո���ǰ����������
        setDO(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived, 0);
        machineLog->write("�²㱶�������������", Normal);
        Sleep(5);
    }
}

// һά��ʶ��
int rs_motion::barCodeIdentify(PointItemConfig item)
{
    BarCodeDetect* barCodeParams1 = new BarCodeDetect();
#ifdef DEBUG_LHC
    item.PhotoName = "1";
    HObject aaa;
    ReadImage(&aaa, "C:\\Users\\67024\\Desktop\\ed_pic\\1.bmp");
    //Rgb1ToGray(aaa, &aaa);
    m_photo_map[item.PhotoName] = aaa;
#endif // DEBUG_LHC
    try {
#if LOCAL_TEST == 1
        HObject ho_img;
        ReadImage(&ho_img, "C:\\Users\\erik.lin\\Desktop\\test\\20250808_10-33-18\\up1.bmp");
#else
        auto ho_img = m_photo_map[item.PhotoName];
#endif
        if (ho_img.Key() == nullptr)
        {
            //ͼ��Ϊ�գ�����ͼ��clear
            machineLog->write("һά����ͼ��Ϊ��", Normal);
            if (barCodeParams1)
            {
                delete barCodeParams1;
                barCodeParams1 = nullptr;
            }
            return -1;
        }
        int rtn = 0;
        QString exePath = QCoreApplication::applicationDirPath();
        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };
        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/BarCode_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //// ���� PinWindow ʵ����ʹ�õڶ������캯��
        //BarCodeWindow barDlg(ho_img, processID, modubleID, modubleName);
        //barDlg.hide();

        //// ��ȡ�ļ���
        //barDlg.ConfigPath = configDirPath.toUtf8().constData();
        //barDlg.XmlPath = xmlDirPath.toUtf8().constData();

        //int ret = 1;
        ////ʶ��һά��
        //ret = barDlg.slot_FindBarCode();

        int ret = 1;
        //XML��ȡ����
        RunParamsStruct_BarCode hv_RunParamsBarCode;
        BarCodeDetect* barCodeParams1 = new BarCodeDetect();
        DataIO dataIOC;//������ȡ����

        ErrorCode_Xml errorCode = dataIOC.ReadParams_BarCode(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsBarCode, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("һά�������ȡʧ�ܣ��䷽��" + item.Vision, Normal);
            m_runResult.isNG_OneDimensional = true;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (barCodeParams1)
            {
                delete barCodeParams1;
                barCodeParams1 = nullptr;
            }

            return ret;
        }
        //�����㷨
        ret = barCodeParams1->FindBarCodeFunc(ho_img, hv_RunParamsBarCode, barCodeParams1->mResultParams);
        auto result = barCodeParams1->mResultParams;

        //bool isOK = reslut.hv_RetNum > 0 ? true : false;
        //ȡ���
        //m_barCodeResult.push_back(std::make_pair(result, item));

        //��ʾ����������ǵ�����ʾ���ǽ����ʾ
        if (item.isResultMerging)
        {
            //�����ʾ
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //���ǵ�һ��Ҫ�ںϵ�ͼ
                m_resultMergingImg[item.PhotoName].img = ho_img;
               
            }

            if (result.hv_RetNum > 0)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
                //����ı�
                QString text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                //m_snResult.sn = text;

                //�鿴�Ƿ�����Ҫ������CSV������
                if (m_ResultToCsv.count(item.Tip))
                {
                    m_ResultToCsv[item.Tip] = text;
                }
            }
            else
            {
                //NG
                m_resultMergingImg[item.PhotoName].isNG = true;
                m_runResult.isNG_OneDimensional = true;
                item.isNG = true;
            }
            std::lock_guard<std::mutex> lock(m_visionMutex);  // ����
            m_resultMergingImg[item.PhotoName].BarMes.push_back(make_pair(item, result));
        }
        else
        {
            //ֱ����ʾ
            Vision_Message message;
            message.img = ho_img;
            message.isNG = result.hv_RetNum.I() > 0 ? false : true;
            item.isNG = message.isNG;
            message.BarMes.push_back(make_pair(item, result));
            m_runResult.isNG_OneDimensional = message.isNG;
            if (!message.isNG)
            {
                //����ı�
                QString text = QString::fromUtf8(result.hv_CodeContent.S().Text());

                //�鿴�Ƿ�����Ҫ������CSV������
                if (m_ResultToCsv.count(item.Tip))
                {
                    m_ResultToCsv[item.Tip] = text;
                }
            }
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////������ͼ
            //if (!item.Tip.isEmpty())
            //{
            //    //���ȱ�ע��
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Tip);
            //}
            //else
            //{
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Vision);
            //}
            emit showVision_signal(message);
        }
    }
    catch (const HalconCpp::HException& e) {
        if (barCodeParams1)
        {
            delete barCodeParams1;
            barCodeParams1 = nullptr;
        }
        machineLog->write("Error show barCodeIdentify image:" + QString(e.ErrorMessage().Text()), Normal);
    }
    if (barCodeParams1)
    {
        delete barCodeParams1;
        barCodeParams1 = nullptr;
    }
    return 0;
}

// ��ά��ʶ��
int rs_motion::QrCodeIdentify(PointItemConfig item)
{
#ifdef DEBUG_LHC
    item.PhotoName = "1";
    HObject aaa;
    //ReadImage(&aaa, "C:\\Users\\67024\\Desktop\\ed_pic\\1.bmp");
    //m_photo_map[item.PhotoName] = aaa;
#endif // DEBUG_LHC
    QRCodeDetect* code2dParams1 = new QRCodeDetect();
    try {

#if LOCAL_TEST == 1
        HObject ho_img;
        ReadImage(&ho_img, "C:\\Users\\erik.lin\\Desktop\\test\\20250808_10-33-18\\up1.bmp");
#else
        auto ho_img = m_photo_map[item.PhotoName];
#endif
        if (ho_img.Key() == nullptr || !ho_img.IsInitialized())
        {
            //ͼ��Ϊ�գ�����ͼ��clear
            machineLog->write("��ά����ͼ��Ϊ��", Normal);
            if (code2dParams1)
            {
                delete code2dParams1;
                code2dParams1 = nullptr;
            }
            return -1;
        }
        //HObject tempImg;
        //CopyImage(tempImg, &ho_img);
        //HObject tempImg = ho_img.Clone();
        int rtn = 0;
        QString exePath = QCoreApplication::applicationDirPath();
        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };
        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/QRCode_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //---------------------------------------------------------------------
        //// ���� QRCodeWindow ʵ����ʹ�õڶ������캯��
        //QRCodeWindow barDlg(ho_img, processID, modubleID, modubleName);
        //barDlg.hide();

        //// ��ȡ�ļ���
        //barDlg.ConfigPath = configDirPath.toUtf8().constData();
        //barDlg.XmlPath = xmlDirPath.toUtf8().constData();


        //int ret = 0;
        ////ʶ���ά��
        //ret = barDlg.slot_FindCode2d();
        //auto result = barDlg.code2dParams1->mResultParams;
        //m_snResult.sn = result.hv_CodeContent.S();
        //---------------------------------------------------------------------

        //bool isOK = ret == 0 ? true : false;
        //ȡ���
        //m_qrCodeResult.push_back(std::make_pair(result, item));


        int ret = 1;
        //XML��ȡ����
        RunParamsStruct_Code2d hv_RunParamsCode2d;
        DataIO dataIOC;//������ȡ����

        hv_RunParamsCode2d.hv_CodeType_Run.Clear();
        ErrorCode_Xml errorCode = dataIOC.ReadParams_Code2d(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCode2d, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("��ά�������ȡʧ�ܣ��䷽��" + item.Vision, Normal);
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            m_runResult.isNG_TwoDimensional = true;
            emit showNG_signal(ho_img, item, true);
            if (code2dParams1)
            {
                delete code2dParams1;
                code2dParams1 = nullptr;
            }

            return ret;
        }
        //�����㷨
        code2dParams1->FindCode2dFunc(ho_img, hv_RunParamsCode2d, code2dParams1->mResultParams);
        auto result = code2dParams1->mResultParams;
        //if (result.hv_RetNum.I() == 0)
        //{
        //    machineLog->write("��ά��ʶ��ʧ�ܣ��䷽��" + item.Vision, Normal);
        //    if (code2dParams1)
        //    {
        //        delete code2dParams1;
        //        code2dParams1 = nullptr;
        //    }

        //    return ret;
        //}

        //��ʾ����������ǵ�����ʾ���ǽ����ʾ
        if (item.isResultMerging)
        {
            //�����ʾ
            std::lock_guard<std::mutex> lock(m_visionMutex);
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //���ǵ�һ��Ҫ�ںϵ�ͼ
                m_resultMergingImg[item.PhotoName].img = ho_img;
            }

            if (result.hv_RetNum > 0)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
                m_runResult.modelName = result.hv_CodeContent.S();

                //����ı�
                QString text = "";
                if (!result.hv_CodeContent.S().IsEmpty())
                {
                    //text = result.hv_CodeContent.S();
                    text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                }
                //m_snResult.sn = text;

                //�鿴�Ƿ�����Ҫ������CSV������
                if (m_ResultToCsv.count(item.Tip))
                {
                    //std::lock_guard<std::mutex> lock(m_visionMutex);
                    m_ResultToCsv[item.Tip] = text;
                }
            }
            else
            {
                //NG
                m_resultMergingImg[item.PhotoName].isNG = true;
                m_runResult.isNG_TwoDimensional = true;
                item.isNG = true;
            }
            m_resultMergingImg[item.PhotoName].QRMes.push_back(make_pair(item, result));

        }
        else
        {
            //ֱ����ʾ
            Vision_Message message;
            message.img = ho_img;
            message.isNG = result.hv_RetNum.I() > 0 ? false : true;
            item.isNG = message.isNG;
            message.QRMes.push_back(make_pair(item, result));
            m_runResult.isNG_TwoDimensional = message.isNG;
            if (!message.isNG)
            {
                m_runResult.modelName = result.hv_CodeContent.S();
                //����ı�
                QString text = "";
                if (!result.hv_CodeContent.S().IsEmpty())
                {
                    //text = result.hv_CodeContent.S();
                    text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                }
                //m_snResult.sn = text;

                //�鿴�Ƿ�����Ҫ������CSV������
                if (m_ResultToCsv.count(item.Tip))
                {
                    //std::lock_guard<std::mutex> lock(m_visionMutex);
                    m_ResultToCsv[item.Tip] = text;
                }
            }

            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////������ͼ
            //if (!item.Tip.isEmpty())
            //{
            //    //���ȱ�ע��
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Tip);
            //}
            //else
            //{
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Vision);
            //}
            emit showVision_signal(message);
        }

        //---------------------------- ��mes���� --------------------------
        //if (!LSM->notMes && item.Tip == "SN")
        //{
        //    if (item.isNG)
        //    {
        //        //ֱ��NG
        //        m_mesResponse_ED.state = false;
        //        m_mesResponse_ED.description = "��ά��ʶ��δ���ҵ�SN";
        //    }
        //    else
        //    {
        //        QString text = "";
        //        if (!result.hv_CodeContent.S().IsEmpty())
        //        {
        //            //text = result.hv_CodeContent.S();
        //            text = QString::fromUtf8(result.hv_CodeContent.S().Text());
        //        }
        //        // ׼����������
        //        MesCheckInRequest_ED request;
        //        request.sn = text;
        //        //request.IP = "192.168.1.100";
        //        //request.Slot = "A3";

        //        // ��������
        //        m_mesResponse_ED = LSM->mes_check_in_ED("http://192.168.100.205:311", request);

        //        // ������Ӧ
        //        if (m_mesResponse_ED.state) {
        //            machineLog->write("У��ͨ�������Կ�ʼ����", Normal);
        //        }
        //        else {
        //            machineLog->write("У��ʧ�ܣ�" + m_mesResponse_ED.description, Normal);
        //        }
        //    }
        //}
        //
       
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error show QrCodeIdentify image:" + QString(e.ErrorMessage().Text()), Normal);
        if (code2dParams1)
        {
            delete code2dParams1;
            code2dParams1 = nullptr;
        }
    }
    if (code2dParams1)
    {
        delete code2dParams1;
        code2dParams1 = nullptr;
    }
    return 0;
}

// ģ��ƥ��
int rs_motion::templateIdentify(PointItemConfig item)
{
    TemplateDetect* TemplateDetect1 = new TemplateDetect();
    try {
#if LOCAL_TEST == 1
        HObject ho_img;
        ReadImage(&ho_img, "C:\\Users\\erik.lin\\Desktop\\test\\20250808_10-33-18\\up1.bmp");
#else
        auto ho_img = m_photo_map[item.PhotoName];
#endif
        if (ho_img.Key() == nullptr || !ho_img.IsInitialized())
        {
            //ͼ��Ϊ�գ�����ͼ��clear
            machineLog->write("ģ��ƥ��ͼ��Ϊ��", Normal);
            return -1;
        }
        //HObject tempImg;
        //CopyImage(tempImg, &ho_img);
        //HObject tempImg = ho_img.Clone();
        int rtn = 0;
        QString exePath = QCoreApplication::applicationDirPath();
        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };
        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Template_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //--------------------------------------------------------------------------
        //TemplateWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// ��ȡ�ļ���
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();

        //int ret = 0;
        ////ģ��ƥ��
        //ret = tempDlg.slot_FindTemplate();
        //auto result = tempDlg.TemplateDetect1->mResultParams;
        //auto runPara = tempDlg.hv_RunParamsTemplate;
        //bool isOK = result.hv_RetNum.I() == runPara.hv_NumMatches.I();
        //--------------------------------------------------------------------------
        //ȡ���
        //m_TemplateResult.push_back(std::make_pair(tempDlg.TemplateDetect1->mResultParams, item));

        int ret = 1;
        //XML��ȡ����
        RunParamsStruct_Template hv_RunParamsTemplate;
        DataIO dataIOC;//������ȡ����

        ErrorCode_Xml errorCode = dataIOC.ReadParams_Template(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsTemplate, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("ģ��ƥ�������ȡʧ�ܣ��䷽��" + item.Vision, Normal);
            m_runResult.isNG_Temple = true;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (TemplateDetect1)
            {
                delete TemplateDetect1;
                TemplateDetect1 = nullptr;
            }
            return -1;
        }
        //�����㷨
        rtn = TemplateDetect1->FindTemplate(ho_img, hv_RunParamsTemplate, TemplateDetect1->mResultParams);

        auto result = TemplateDetect1->mResultParams;
        bool isOK = result.hv_RetNum.I() == hv_RunParamsTemplate.hv_NumMatches.I();

        //�鿴�Ƿ�����Ҫ������CSV������
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isOK ? "OK":"NG";
        }

        //��ʾ����������ǵ�����ʾ���ǽ����ʾ
        if (item.isResultMerging)
        {
            //�����ʾ
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //���ǵ�һ��Ҫ�ںϵ�ͼ
                m_resultMergingImg[item.PhotoName].img = ho_img;
            }

            if (isOK)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
            }
            else
            {
                //NG
                m_resultMergingImg[item.PhotoName].isNG = true;
                m_runResult.isNG_Temple = true;
                item.isNG = true;
            }
            //m_resultMergingImg[item.PhotoName].TempMes.push_back(make_pair(item, result));
            std::lock_guard<std::mutex> lock(m_visionMutex);  // ����
            m_resultMergingImg[item.PhotoName].TempMes.emplace_back(make_pair(item, result));
            //auto& tempVec = m_resultMergingImg[item.PhotoName].TempMes;
            //tempVec.emplace_back(std::make_pair(std::move(item), std::move(result)));
        }
        else
        {
            //ֱ����ʾ
            Vision_Message message;
            message.img = ho_img;
            message.isNG = isOK ? false : true;
            item.isNG = message.isNG;
            message.TempMes.push_back(make_pair(item, result));
            m_runResult.isNG_Temple = message.isNG;
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////������ͼ
            //if (!item.Tip.isEmpty())
            //{
            //    //���ȱ�ע��
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Tip);
            //}
            //else
            //{
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Vision);
            //}
            emit showVision_signal(message);


        }

    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error show templateIdentify image:" + QString(e.ErrorMessage().Text()), Normal);
        if (TemplateDetect1)
        {
            delete TemplateDetect1;
            TemplateDetect1 = nullptr;
        }
    }
    if (TemplateDetect1)
    {
        delete TemplateDetect1;
        TemplateDetect1 = nullptr;
    }
    return 0;
}

// blob����
int rs_motion::blobIdentify(PointItemConfig item)
{
    BlobDetect* BlobDetect1 = new BlobDetect();
    try {
        auto ho_img = m_photo_map[item.PhotoName];
        if (ho_img.Key() == nullptr)
        {
            //ͼ��Ϊ�գ�����ͼ��clear
            machineLog->write("blob����ͼ��Ϊ��", Normal);
            if (BlobDetect1)
            {
                delete BlobDetect1;
                BlobDetect1 = nullptr;
            }
            return -1;
        }
        //HObject tempImg;
        //CopyImage(tempImg, &ho_img);
        HObject tempImg = ho_img;
        int rtn = 0;
        QString exePath = QCoreApplication::applicationDirPath();
        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };
        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Blob_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //BlobWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// ��ȡ�ļ���
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();


        //int ret = 0;
        //blob����
        //ret = tempDlg.slot_BlobDetect();

        int ret = 1;
        //XML��ȡ����
        RunParamsStruct_Blob hv_RunParamsBlob;
        DataIO dataIOC;//������ȡ����

        ErrorCode_Xml errorCode = dataIOC.ReadParams_Blob(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsBlob, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("Blob������ȡʧ�ܣ��䷽��" + item.Vision, Normal);
            m_runResult.isNG_Blob = true;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (BlobDetect1)
            {
                delete BlobDetect1;
                BlobDetect1 = nullptr;
            }
            return -1;
        }
        //�����㷨
        ret = BlobDetect1->BlobFunc(ho_img, hv_RunParamsBlob, BlobDetect1->mResultParams);

        auto result = BlobDetect1->mResultParams;
        bool isOK = ret == 0 ? true : false;
        //ȡ���
        //m_BolbResult.push_back(std::make_pair(tempDlg.BlobDetect1->mResultParams, item));

        //�鿴�Ƿ�����Ҫ������CSV������
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isOK ? "OK" : "NG";
        }

        //��ʾ����������ǵ�����ʾ���ǽ����ʾ
        if (item.isResultMerging)
        {
            //�����ʾ
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //���ǵ�һ��Ҫ�ںϵ�ͼ
                m_resultMergingImg[item.PhotoName].img = tempImg;

            }

            if (isOK)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
            }
            else
            {
                //NG
                m_resultMergingImg[item.PhotoName].isNG = true;
                m_runResult.isNG_Blob = true;
                item.isNG = true;
            }
            std::lock_guard<std::mutex> lock(m_visionMutex);  // ����
            m_resultMergingImg[item.PhotoName].BlobMes.push_back(make_pair(item, result));
        }
        else
        {
            //ֱ����ʾ
            Vision_Message message;
            message.img = ho_img;
            message.isNG = isOK ? false : true;
            item.isNG = message.isNG;
            message.BlobMes.push_back(make_pair(item, result));
            m_runResult.isNG_Blob = message.isNG;
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////������ͼ
            //if (!item.Tip.isEmpty())
            //{
            //    //���ȱ�ע��
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Tip);
            //}
            //else
            //{
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Vision);
            //}
            emit showVision_signal(message);
        }
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("Error show blobIdentify image:" + QString(e.ErrorMessage().Text()), Normal);
        if (BlobDetect1)
        {
            delete BlobDetect1;
            BlobDetect1 = nullptr;
        }
    }
    if (BlobDetect1)
    {
        delete BlobDetect1;
        BlobDetect1 = nullptr;
    }
    return 0;
}

// Բ���
int rs_motion::circleIdentify(PointItemConfig item)
{
    auto ho_img = m_photo_map[item.PhotoName];
    if (ho_img.Key() == nullptr)
    {
        //ͼ��Ϊ�գ�����ͼ��clear
        machineLog->write("Բ������ͼ��Ϊ��", Normal);
        return -1;
    }
    //HObject tempImg;
    //CopyImage(tempImg, &ho_img);
    HObject tempImg = ho_img;
    int rtn = 0;
    QString exePath = QCoreApplication::applicationDirPath();
    //int processID = 1;
    //int modubleID = 2;
    //std::string modubleName = "Module1";
    int processID = 0;
    int modubleID = 0;
    std::string modubleName = item.Vision.toStdString();
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
    };
    // ��ȡ��Ӧ���䷽·��
    QString filename = item.Vision + ".xml";
    QString configDirPath = exePath + "/Vision_formulation/Circle_formulation/";
    QString xmlDirPath = configDirPath + filename;

    CircleWindow tempDlg(ho_img, processID, modubleID, modubleName);
    tempDlg.hide();

    // ��ȡ�ļ���
    tempDlg.ConfigPath = configDirPath.toUtf8().constData();
    tempDlg.XmlPath = xmlDirPath.toUtf8().constData();


    int ret = 0;
    //blob����
    ret = tempDlg.slot_FindCircle();
    auto result = tempDlg.CircleDetect1->mResultParams;
    bool isNG = ret != 0 ? true : false;
    m_runResult.isNG_Circle = isNG;
    //ȡ���
    //m_CircleResult.push_back(std::make_pair(tempDlg.CircleDetect1->mResultParams, item));
    //try {
    //    //��ʾ����������ǵ�����ʾ���ǽ����ʾ
    //    if (item.isResultMerging)
    //    {
    //        //�����ʾ
    //        if (!m_resultMergingImg.count(item.PhotoName))
    //        {
    //            //���ǵ�һ��Ҫ�ںϵ�ͼ
    //            if (result.hv_RetNum > 0)
    //            {
    //                //OK
    //                HObject ho_ShowObj;
    //                // ������ɫ��RGB=0,0,255��
    //                HTuple color;
    //                color[0] = 0;   // Red
    //                color[1] = 255;   // Green
    //                color[2] = 0; // Blue
    //                // ����������ɫ  
    //                PaintRegion(result.ho_Contour_Circle, ho_img, &ho_ShowObj, color, "margin");
    //                m_resultMergingImg[item.PhotoName].first = ho_ShowObj;
    //                m_resultMergingImg[item.PhotoName].second = false;
    //            }
    //            else
    //            {
    //                //NG
    //                m_resultMergingImg[item.PhotoName].first = ho_img;
    //                m_resultMergingImg[item.PhotoName].second = true;
    //            }
    //        }
    //        else
    //        {
    //            //ȡ֮ǰ��ͼȥ�ں�
    //            if (result.hv_RetNum > 0)
    //            {
    //                //OK
    //                HObject ho_ShowObj;
    //                // ������ɫ��RGB=0,0,255��
    //                HTuple color;
    //                color[0] = 0;   // Red
    //                color[1] = 255;   // Green
    //                color[2] = 0; // Blue
    //                // ����������ɫ  
    //                PaintRegion(result.ho_Contour_Circle, m_resultMergingImg[item.PhotoName].first, &m_resultMergingImg[item.PhotoName].first, color, "margin");
    //            }
    //            else
    //            {
    //                //NG
    //                m_resultMergingImg[item.PhotoName].second = true;
    //            }
    //        }
    //    }
    //    else
    //    {
    //        if (result.hv_RetNum > 0 && result.ho_Contour_Circle.IsInitialized())
    //        {
    //            //OK
    //            HObject ho_ShowObj;
    //            // ������ɫ��RGB=0,0,255��
    //            HTuple color;
    //            color[0] = 0;   // Red
    //            color[1] = 255;   // Green
    //            color[2] = 0; // Blue
    //            // ����������ɫ  
    //            PaintRegion(result.ho_Contour_Circle, ho_img, &ho_ShowObj, color, "margin");
    //            // ת��Ϊ OpenCV Mat ����ʾ
    //            emit showVision_signal(ho_ShowObj, false);
    //        }
    //        else
    //        {
    //            //NG
    //            emit showVision_signal(ho_img, true);
    //        }
    //    }
    //}
    //catch (const HalconCpp::HException& e) {
    //    machineLog->write("Error show circleIdentify image:" + QString(e.ErrorMessage().Text()), Normal);
    //}
    return 0;
}

// Բ���
int rs_motion::circleMeasure(PointItemConfig item)
{
    CircleDetect* CircleDetect1_1 = new CircleDetect();
    CircleDetect* CircleDetect1_2 = new CircleDetect();
    try
    {
#if LOCAL_TEST == 1
        HObject ho_img;
        ReadImage(&ho_img, "C:\\Users\\erik.lin\\Desktop\\test\\20250808_10-33-18\\up1.bmp");
#else
        auto ho_img = m_photo_map[item.PhotoName];
#endif

        //WriteImage(ho_img, "bmp", 0, "C:\\Users\\User\\Desktop\\222\\1.bmp");
        if (ho_img.Key() == nullptr)
        {
            //ͼ��Ϊ�գ�����ͼ��clear
            machineLog->write("Բ������ͼ��Ϊ��", Normal);
            if (CircleDetect1_1)
            {
                delete CircleDetect1_1;
                CircleDetect1_1 = nullptr;
            }
            if (CircleDetect1_2)
            {
                delete CircleDetect1_2;
                CircleDetect1_2 = nullptr;
            }
            return -1;
        }
        int rtn = 0;
        QString exePath = QCoreApplication::applicationDirPath();
        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };
        // ��ȡ��Ӧ���䷽·��
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Circle_formulation/";
        QString xmlDirPath = configDirPath + filename;

        //------------------------------------------------------------------------
        //CircleWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// ��ȡ��һ��Բ�ļ���
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();
        //int ret = 0;
        ////Բ���
        //ret = tempDlg.slot_FindCircle();
        //auto result = tempDlg.CircleDetect1->mResultParams;
        //bool isNG = ret != 0 ? true : false;
        //m_runResult.isNG_CircleMeasure = isNG;
        //if (isNG)
        //{
        //    machineLog->write("Բ����һ��Բ���ʧ��", Normal);
        //    ResultParamsStruct_Circle temp;
        //    emit showCircleMeasure_signal(result, temp, item, 0, isNG);
        //    return -1;
        //}
        //------------------------------------------------------------------------

        //��һ��Բ XML��ȡ����
        RunParamsStruct_Circle hv_RunParamsCircle_1;
        DataIO dataIOC;//������ȡ����
        ErrorCode_Xml errorCode = dataIOC.ReadParams_Circle(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCircle_1, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("Բ����һ��Բ������ȡʧ�ܣ��䷽��" + item.Vision, Normal);
            m_runResult.isNG_CircleMeasure = true;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (CircleDetect1_1)
            {
                delete CircleDetect1_1;
                CircleDetect1_1 = nullptr;
            }
            if (CircleDetect1_2)
            {
                delete CircleDetect1_2;
                CircleDetect1_2 = nullptr;
            }
            return -1;
        }
        //�����㷨
        auto ret = CircleDetect1_1->FindCircle(ho_img, hv_RunParamsCircle_1, CircleDetect1_1->mResultParams);
        auto result = CircleDetect1_1->mResultParams;
        bool isNG = ret != 0 ? true : false;
        if(!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;
        if (isNG)
        {
            machineLog->write("Բ����һ��Բ���ʧ��", Normal);
            m_runResult.isNG_CircleMeasure = isNG;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (CircleDetect1_1)
            {
                delete CircleDetect1_1;
                CircleDetect1_1 = nullptr;
            }
            if (CircleDetect1_2)
            {
                delete CircleDetect1_2;
                CircleDetect1_2 = nullptr;
            }
            return -1;
        }

        //------------------------------------------------------------------------
        ////  ��ȡ�ڶ���Բ�ļ���
        //filename = item.VisionTwo + ".xml";
        //xmlDirPath = configDirPath + "/" + filename;
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();
        ////ģ������
        //tempDlg.nodeName = item.VisionTwo.toStdString();
        //ret = tempDlg.slot_FindCircle();
        //auto resultTwo = tempDlg.CircleDetect1->mResultParams;
        //isNG = ret != 0 ? true : false;
        //m_runResult.isNG_CircleMeasure = isNG;
        //if (isNG)
        //{
        //    machineLog->write("Բ���ڶ���Բ���ʧ��", Normal);
        //    emit showCircleMeasure_signal(result, resultTwo, item, 0, isNG);
        //    return -1;
        //}
        //------------------------------------------------------------------------

        //�ڶ���Բ XML��ȡ����
        filename = item.VisionTwo + ".xml";
        xmlDirPath = configDirPath + "/" + filename;
        modubleName = item.VisionTwo.toStdString();

        RunParamsStruct_Circle hv_RunParamsCircle_2;
        errorCode = dataIOC.ReadParams_Circle(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCircle_2, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("Բ���ڶ���Բ������ȡʧ�ܣ��䷽��" + item.VisionTwo, Normal);
            m_runResult.isNG_CircleMeasure = isNG;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (CircleDetect1_1)
            {
                delete CircleDetect1_1;
                CircleDetect1_1 = nullptr;
            }
            if (CircleDetect1_2)
            {
                delete CircleDetect1_2;
                CircleDetect1_2 = nullptr;
            }
            return -1;
        }
        //�����㷨
        ret = CircleDetect1_2->FindCircle(ho_img, hv_RunParamsCircle_2, CircleDetect1_2->mResultParams);
        auto resultTwo = CircleDetect1_2->mResultParams;
        isNG = ret != 0 ? true : false;
        if (!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;
        if (isNG)
        {
            machineLog->write("Բ���ڶ���Բ���ʧ��", Normal);
            m_runResult.isNG_CircleMeasure = isNG;
            if (m_ResultToCsv.count(item.Tip))
            {
                m_ResultToCsv[item.Tip] = "NG";
            }
            emit showNG_signal(ho_img, item, true);
            if (CircleDetect1_1)
            {
                delete CircleDetect1_1;
                CircleDetect1_1 = nullptr;
            }
            if (CircleDetect1_2)
            {
                delete CircleDetect1_2;
                CircleDetect1_2 = nullptr;
            }

            return -1;
        }

        //���
        auto x1 = result.hv_CircleRowCenter.D();
        auto y1 = result.hv_CircleColumnCenter.D();

        auto x2 = resultTwo.hv_CircleRowCenter.D();
        auto y2 = resultTwo.hv_CircleColumnCenter.D();

        //����
        double dis = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        //ת����
        //dis *= 0.025;
        dis /= item.PixDistance;
        //�����Ƿ����Ҫ��
        if (dis < item.CircleBenchmark + item.CircleUpError && dis > item.CircleBenchmark - item.CircleDownError)
        {
            isNG = false;
        }
        else
        {
            isNG = true;
        }

        //�����NG����Ҫ����ΪOK
        if(!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;

        //�鿴�Ƿ�����Ҫ������CSV������
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isNG ? "NG" : "OK";
        }

        //չʾͼƬ
        emit showCircleMeasure_signal(result, resultTwo, item, dis, isNG);
        if (CircleDetect1_1)
        {
            delete CircleDetect1_1;
            CircleDetect1_1 = nullptr;
        }
        if (CircleDetect1_2)
        {
            delete CircleDetect1_2;
            CircleDetect1_2 = nullptr;
        }
    }
    catch (const HalconCpp::HException& e)
    {
        machineLog->write("Error show circleMeasure image:" + QString(e.ErrorMessage().Text()), Normal);
        if (CircleDetect1_1)
        {
            delete CircleDetect1_1;
            CircleDetect1_1 = nullptr;
        }
        if (CircleDetect1_2)
        {
            delete CircleDetect1_2;
            CircleDetect1_2 = nullptr;
        }
    }
   
    return 0;
}

//��DO
void rs_motion::closeDO()
{
    if (m_version == LsPin)
    {
        //��ԭDO
        setDO(LS_DO::LS_OUT_ClampBlow, 0);
        setDO(LS_DO::LS_OUT_ClampCyl1, 0);
        setDO(LS_DO::LS_OUT_ClampCyl2, 0);
        setDO(LS_DO::LS_OUT_2DLightTrigger, 0);
        setDO(LS_DO::LS_OUT_UpperMotor_Run, 0);
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        setDO(LS_DO::LS_OUT_LowerMotor_Run, 0);
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 0);
        setDO(LS_DO::LS_OUT_BlockCyl_Activate, 0);
        setDO(LS_DO::LS_OUT_LiftCyl_Activate, 0);
        setDO(LS_DO::LS_OUT_2DCamTrigger, 0);
        setDO(LS_DO::LS_OUT_3DCamTrigger, 0);
        //�رձ��������
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 0);
        setDO(LS_DO::LS_OUT_UpperMotor_Run, 0);
        setDO(LS_DO::LS_OUT_LowerMotor_Run, 0);
        setDO(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady, 0);
        setDO(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived, 0);
        setDO(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady, 0);
        setDO(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady, 0);
    }
    else if (m_version == EdPin)
    {
        //��ԭDO
        //setDO(LS_DO::ED_DO_PositionCyl1, 0);
        //setDO(LS_DO::ED_DO_PositionCyl2, 0);
        //setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 0);
        //setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 0);
        setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        setDO(LS_DO::ED_DO_LightRearTrigger, 0);
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 0);
        setDO(LS_DO::ED_DO_FeedBeltStart, 0);
        setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
        setDO(LS_DO::ED_DO_NGBeltStart, 0);
        setDO(LS_DO::DO_Light2DTrigger, 0);
        //����ź�
        MODBUS_TCP.writeIntCoil(134, 0, 0);
        MODBUS_TCP.writeIntCoil(134, 1, 0);
        MODBUS_TCP.writeIntCoil(134, 2, 0);
        MODBUS_TCP.writeIntCoil(134, 3, 0);
        MODBUS_TCP.writeIntCoil(134, 4, 0);
        MODBUS_TCP.writeIntCoil(134, 5, 0);
    }
}

//��ɫ��ָʾ�� 0���̵ƣ�1���Ƶƣ�2�����
void rs_motion::setThreeColorLight(int color,bool isClose)
{
    if (isClose)
    {
        if (m_version == LsPin)
        {
            setDO(LS_DO::LS_OUT_RedLight, 0);
            setDO(LS_DO::LS_OUT_YellowLight, 0);
            setDO(LS_DO::LS_OUT_GreenLight, 0);
        }
        else if (m_version == TwoPin || m_version == EdPin)
        {
            setDO(LS_DO::DO_LightRed, 0);
            setDO(LS_DO::DO_LightYellow, 0);
            setDO(LS_DO::DO_LightGreen, 0);
        }
    }
    else
    {
        if (m_version == LsPin)
        {
            switch (color)
            {
            case 0:
                setDO(LS_DO::LS_OUT_RedLight, 0);
                setDO(LS_DO::LS_OUT_YellowLight, 0);
                setDO(LS_DO::LS_OUT_GreenLight, 1);
                break;
            case 1:
                setDO(LS_DO::LS_OUT_RedLight, 0);
                setDO(LS_DO::LS_OUT_YellowLight, 1);
                setDO(LS_DO::LS_OUT_GreenLight, 0);
                break;
            case 2:
                setDO(LS_DO::LS_OUT_RedLight, 1);
                setDO(LS_DO::LS_OUT_YellowLight, 0);
                setDO(LS_DO::LS_OUT_GreenLight, 0);
                break;
            default:
                break;
            }
        }
        else if (m_version == TwoPin || m_version == EdPin)
        {
            switch (color)
            {
            case 0:
                setDO(LS_DO::DO_LightRed, 0);
                setDO(LS_DO::DO_LightYellow, 0);
                setDO(LS_DO::DO_LightGreen, 1);
                break;
            case 1:
                setDO(LS_DO::DO_LightRed, 0);
                setDO(LS_DO::DO_LightYellow, 1);
                setDO(LS_DO::DO_LightGreen, 0);
                break;
            case 2:
                setDO(LS_DO::DO_LightRed, 1);
                setDO(LS_DO::DO_LightYellow, 0);
                setDO(LS_DO::DO_LightGreen, 0);
                break;
            default:
                break;
            }
        }
    }
   
}

//��ťָʾ�� type 0��������1����λ��2��ֹͣ��3����������4������
void rs_motion::setButtonLight(int type, int val)
{

    if (m_version == LsPin)
    {
        if (val == 1)
        {
            //��
            switch (type)
            {
            case 0:
                setDO(LS_DO::LS_OUT_StopBtnLED, 0);
                setDO(LS_DO::LS_OUT_ResetBtnLED, 0);
                setDO(LS_DO::LS_OUT_StartBtnLED, val);
                break;
            case 1:
                setDO(LS_DO::LS_OUT_StopBtnLED, 0);
                setDO(LS_DO::LS_OUT_StartBtnLED, 0);
                setDO(LS_DO::LS_OUT_ResetBtnLED, val);
                break;
            case 2:
                setDO(LS_DO::LS_OUT_StartBtnLED, 0);
                setDO(LS_DO::LS_OUT_ResetBtnLED, 0);
                setDO(LS_DO::LS_OUT_StopBtnLED, val);
                break;
            case 3:
                setDO(LS_DO::LS_OUT_Buzzer, val);
                break;
            case 4:
                setDO(LS_DO::LS_OUT_Lighting, val);
                setDO(LS_DO::LS_OUT_LightBtnLED, val);
                break;
            default:
                break;
            }
        }
        else
        {
            //��
            switch (type)
            {
            case 0:
                setDO(LS_DO::LS_OUT_StartBtnLED, 0);
                break;
            case 1:
                setDO(LS_DO::LS_OUT_ResetBtnLED, 0);
                break;
            case 2:
                setDO(LS_DO::LS_OUT_StopBtnLED, 0);
                break;
            case 3:
                setDO(LS_DO::LS_OUT_Buzzer, 0);
                break;
            case 4:
                setDO(LS_DO::LS_OUT_Lighting, 0);
                setDO(LS_DO::LS_OUT_LightBtnLED, 0);
                break;
            default:
                break;
            }
        }
    }
    else if (m_version == TwoPin || m_version == EdPin)
    {
        if (val == 1)
        {
            //��
            switch (type)
            {
            case 0:
                setDO(LS_DO::DO_StopBtnLED, 0);
                setDO(LS_DO::DO_StartBtnLED, val);
                break;
            case 2:
                setDO(LS_DO::DO_StopBtnLED, val);
                setDO(LS_DO::DO_StartBtnLED, 0);
                break;
            case 3:
                setDO(LS_DO::DO_Buzzer, val);
                break;
            default:
                break;
            }
        }
        else
        {
            //��
            switch (type)
            {
            case 0:
                setDO(LS_DO::DO_StartBtnLED, 0);
                break;
            case 2:
                setDO(LS_DO::DO_StopBtnLED, 0);
                break;
            case 3:
                setDO(LS_DO::DO_Buzzer, 0);
                break;
            default:
                break;
            }
        }
    }
}


// ȫ�������������ȷ���̰߳�ȫ��
Q_GLOBAL_STATIC(QNetworkAccessManager, globalManager)
SnCheckResponse rs_motion::check_sn_process(const QString& url, const SnCheckRequest& req) {
    QNetworkRequest netRequest;

    // ��֤������URL
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        return { 500, "��Ч��URL��ʽ", "", "N", "N", "N" };
    }
    netRequest.setUrl(requestUrl);

    // ��������ͷ
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ================== ����JSON���� ==================
    QJsonObject requestJson;
    requestJson["cmd"] = req.cmd;
    requestJson["p"] = req.p;
    requestJson["station_name"] = req.station_name;
    requestJson["sn"] = req.sn;
    requestJson["op"] = req.op;

    // ��ѡ�ֶδ���
    if (!req.csn.isEmpty()) requestJson["csn"] = req.csn;
    if (!req.tooling_no.isEmpty()) requestJson["tooling_no"] = req.tooling_no;
    if (!req.machine_no.isEmpty()) requestJson["machine_no"] = req.machine_no;
    if (!req.first_sign.isEmpty()) requestJson["first_sign"] = req.first_sign;

    //��վ�ϴ��������Ϣ����� mesin_debug.json ����鿴
    QString jsonString = QJsonDocument(requestJson).toJson();
    // ��ȡӦ�ó���Ŀ¼·��
    QString appDir = QCoreApplication::applicationDirPath();
    // ������־Ŀ¼·��
    QDir logDir(appDir + "/mes_log");

    // ȷ��Ŀ¼���ڣ�������ʱ�Զ�������
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {  // ʹ��mkpathȷ�������༶Ŀ¼����Ȼ����ֻ��һ����
            qCritical() << "�޷�������־Ŀ¼:" << logDir.absolutePath();
        }
    }

    // ���������ļ�·��
    QString filePath = logDir.filePath("mesin_debug.json");
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << jsonString;
        file.close();
        qDebug() << "JSON saved to mesin_debug.json";
    }

    // ================== ����POST���� ==================
    QNetworkReply* reply = globalManager->post(
        netRequest,
        QJsonDocument(requestJson).toJson()  // �ؼ�������ת��QByteArray
    );

    //// ================== ͬ���ȴ���Ӧ ==================
    //QEventLoop loop;
    //QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    //loop.exec();

    // ================== ������ʱ���� ==================
    QEventLoop loop;
    QTimer timeoutTimer;

    // ����5�볬ʱ
    const int timeoutMs = 5000;

    // ���������ؼ��ź�
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("mes����ʱ��URL: " + url, Normal);
        if (reply->isRunning()) {
            reply->abort();
        }
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== ��Ӧ���� ==================
    SnCheckResponse result;
    bool isTimeout = !timeoutTimer.isActive(); // ����Ƿ񴥷���ʱ


    // ================== ������Ӧ���� ==================
    SnCheckResponse resp;
    if (isTimeout) {
        resp.code = 500;
        machineLog->write("mes����10Sδ��Ӧ!!", Mes);
    }
    if (reply->error() != QNetworkReply::NoError) {
        resp.msg = "�������: " + reply->errorString();
        reply->deleteLater();
        return resp;
    }

    // ����JSON
    QJsonParseError parseError{};
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        resp.msg = "JSON��������: " + parseError.errorString();
        reply->deleteLater();
        return resp;
    }

    // ��ȡ�ؼ��ֶ�
    QJsonObject resObj = doc.object();
    resp.code = resObj.value("code").toInt(500);
    resp.msg = resObj.value("msg").toString();
    resp.main_sn = resObj.value("main_sn").toString();
    resp.first_sign = resObj.value("first_sign").toString("N");
    resp.rh_first_sign = resObj.value("rh_first_sign").toString("N");
    resp.IsStay = resObj.value("IsStay").toString("N");
    machineLog->write("pin���վ��Ϣ��sn :  " + req.sn, Mes);
    machineLog->write("pin���վ��Ϣ��code :  " + resp.code, Mes);
    machineLog->write("pin���վ��Ϣ��msg :  " + resp.msg, Mes);

    //��վ�ϴ�mes�����������Ϣ����� mesinBack_debug.json ����鿴
    QString filePath2 = logDir.filePath("mesinBack_debug.json");
    jsonString = QJsonDocument(doc).toJson();
    QFile file2(filePath2);
    if (file2.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file2);
        stream << jsonString;
        file2.close();
        qDebug() << "JSON saved to mesinBack_debug.json";
    }

    reply->deleteLater();
    return resp;
}

SnCheckResponse rs_motion::post_result_process(const QString& url, const SnResultRequest& req) {
    QNetworkRequest netRequest;

    // ================== 1. URL��Ч��У�� ==================
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        machineLog->write("��Ч��URL��ʽ: " + url, Normal);
        return { 500, "Invalid URL format", "", "N", "N", "N" };
    }
    netRequest.setUrl(requestUrl);

    // ================== 2. ����ͷ���� ==================
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //netRequest.setRawHeader("X-Request-Source", "QtClient/1.0"); // �Զ���ͷ

    // ================== 3. �����ֲ�JSON���� ==================
    QJsonObject rootJson;
    // 3.1 �����ֶ�
    rootJson["cmd"] = req.cmd;
    rootJson["station_name"] = req.station_name;
    rootJson["sn"] = req.sn;
    rootJson["op"] = req.op;
    rootJson["op_time"] = req.op_time;
    rootJson["result"] = req.result;
    rootJson["start_time"] = req.start_time;
    rootJson["stop_time"] = req.stop_time;

    // 3.2 ת���������ݼ���
    QJsonArray testDataArray;
    for (const TestResult& testItem : req.result_data) {
        // ��֤��������Ч��
        if (testItem.name.isEmpty() || testItem.result.isEmpty()) {
            machineLog->write("��Ч�Ĳ����������������", Mes);
            continue;
        }
        testDataArray.append(testItem.toJson());
    }
    rootJson["result_data"] = testDataArray;

    //��վ�ϴ��������Ϣ����� mesout_debug.json ����鿴
    QString jsonString = QJsonDocument(rootJson).toJson();
    // ��ȡӦ�ó���Ŀ¼·��
    QString appDir = QCoreApplication::applicationDirPath();
    // ������־Ŀ¼·��
    QDir logDir(appDir + "/mes_log");

    // ȷ��Ŀ¼���ڣ�������ʱ�Զ�������
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {  // ʹ��mkpathȷ�������༶Ŀ¼����Ȼ����ֻ��һ����
            qCritical() << "�޷�������־Ŀ¼:" << logDir.absolutePath();
        }
    }
    // ���������ļ�·��
    QString filePath = logDir.filePath("mesout_debug.json");
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << jsonString;
        file.close();
        qDebug() << "JSON saved to mesout_debug.json";
    }
    //QString jsonString = QJsonDocument(rootJson).toJson(QJsonDocument::Compact);
    //QFile file("mesout_debug.json");
    //if (file.open(QIODevice::WriteOnly)) {
    //    QTextStream stream(&file);
    //    stream << jsonString;
    //    file.close();
    //    qDebug() << "JSON saved to mesout_debug.json";
    //}

    // ================== 4. ������������ ==================
    QNetworkReply* reply = globalManager->post(
        netRequest,
        QJsonDocument(rootJson).toJson(QJsonDocument::Compact) // ���ո�ʽ
    );

    // ================== 5. ��ʱ���ƻ��� ==================
    QEventLoop eventLoop;
    QTimer timeoutTimer;
    const int timeoutSeconds = 10; // 10�볬ʱ

    // 5.1 �����ź�
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        if (reply->isRunning()) {
            machineLog->write("����ʱ����ֹ����: " + url, Mes);
            reply->abort();
        }
        eventLoop.quit();
        });

    // 5.2 ������ʱ��
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutSeconds * 1000);

    // 5.3 �����¼�ѭ������ֹ�����û����룩
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    // ================== 6. ��Ӧ���� ==================
    SnCheckResponse response;
    const bool isTimeout = !timeoutTimer.isActive();

    // 6.1 ����ʱ
    if (isTimeout) {
        response.code = 504;
        response.msg = "Request timeout after " + QString::number(timeoutSeconds) + "s";
        machineLog->write("MES�ӿ���Ӧ��ʱ", Mes);
        reply->deleteLater();
        return response;
    }

    // 6.2 �����������
    if (reply->error() != QNetworkReply::NoError) {
        response.code = reply->error();
        response.msg = "Network error: " + reply->errorString();
        machineLog->write("�������: " + reply->errorString(), Mes);
        reply->deleteLater();
        return response;
    }

    // 6.3 ��ȡ��Ӧ����
    const QByteArray responseData = reply->readAll();
    if (responseData.isEmpty()) {
        response.code = 502;
        response.msg = "Empty response from server";
        machineLog->write("�յ�����Ӧ", Mes);
        reply->deleteLater();
        return response;
    }

    // 6.4 ����JSON
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        response.code = 503;
        response.msg = "JSON parse error: " + parseError.errorString();
        machineLog->write("��Ӧ����ʧ��: " + parseError.errorString(), Mes);
        reply->deleteLater();
        return response;
    }

    // 6.5 ��ȡҵ������
    const QJsonObject resObj = doc.object();
    response.code = resObj["code"].toInt(500); // Ĭ��500����
    response.msg = resObj["msg"].toString("Unknown error");

    // 6.6 ��ȡ��ϸ�ֶ�
    if (resObj.contains("data")) {
        const QJsonObject dataObj = resObj["data"].toObject();
        response.main_sn = dataObj["main_sn"].toString();
        response.first_sign = dataObj["first_sign"].toString("N");
        response.rh_first_sign = dataObj["rh_first_sign"].toString("N");
        response.IsStay = dataObj["IsStay"].toString("N");
    }

    // 6.7 ��¼�ɹ���־
    if (response.code == 200) {
        machineLog->write("pin���վ��Ϣ�ϴ��ɹ���sn :  " + req.sn, Mes);
        machineLog->write("pin���վ��Ϣ�ϴ��ɹ���msg :  " + response.msg, Mes);
    }
    else {
        machineLog->write("ҵ�����,������msg: " + response.msg, Mes);
        machineLog->write("ҵ�����,������sn: " + req.sn, Mes);
    }

    reply->deleteLater();
    return response;
}

int rs_motion::robotCheck(int err) {
    switch (err)
    {
    case 1:
        machineLog->write("A�����Ѿ�������OR������쳣", Normal);
        machineLog->write("A�����Ѿ�������OR������쳣", Err);
        return -1;
        break;
    case 2:
        machineLog->write("B�����Ѿ�������OR������쳣", Normal);
        machineLog->write("B�����Ѿ�������OR������쳣", Err);
        return -1;
        break;
    case 3:
        machineLog->write("A����ȡ���쳣", Normal);
        machineLog->write("A����ȡ���쳣", Err);
        return -1;
        break;
    case 4:
        machineLog->write("B����ȡ���쳣", Normal);
        machineLog->write("B����ȡ���쳣", Err);
        return -1;
        break;
    case 5:
        machineLog->write("Aצ�������쳣 OR Bצ���׽��쳣", Normal);
        machineLog->write("Aצ�������쳣 OR Bצ���׽��쳣", Err);
        return -1;
        break;
    case 6:
        machineLog->write("Aצ���׽��쳣 OR Bצ�������쳣", Normal);
        machineLog->write("Aצ���׽��쳣 OR Bצ�������쳣", Err);
        return -1;
        break;
    case 7:
        machineLog->write("�����˲����κ�һԭ�㣬���ֶ���ԭ��", Normal);
        machineLog->write("�����˲����κ�һԭ�㣬���ֶ���ԭ��", Err);
        return -1;
        break;
    case 8:
        machineLog->write("����������źţ�������ֹ", Normal);
        machineLog->write("����������źţ�������ֹ", Err);
        return -1;
        break;

    default:
        break;
    }
    return 0;
};

//ʵʱ��ػ�����Ƥ��ȡ��
int rs_motion::runRobotLoad_Belt()
{
    QElapsedTimer materialTimer;
    int ngCount = 0;
    //�ȴ������̵߳ı�־λ����
    Sleep(1000);
    machineLog->write("�ȴ�Ƥ������", Normal);
    m_isLoadErr.store(false);

    //���������Ƿ񱨴�
    while (m_isStart.load() && !LSM->m_notRobot)
    {
        //����������һֱ���м��
        if (m_isEmergency.load() || m_isRunStop.load())
            return -1;

        if (getDI(LS_DI::ED_DI_FeedBeltArrivalDetect) == 0)
        {
            //Ƥ����û���ϣ��Ƚ���
            //machineLog->write("�ȴ�Ƥ������", Normal);
            if (getDI(LS_DI::ED_DI_FeedBeltMaterialDetect) == 0)
                continue;
            //����Ƥ��
            setDO(LS_DO::ED_DO_FeedBeltStart, 1);
            //Ƥ�������ź�
            machineLog->write("�ȴ�Ƥ������", Normal);
            while (getDI(LS_DI::ED_DI_FeedBeltArrivalDetect) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                Sleep(5);
            }
            //���俪�������λ��װ���ϣ�����ʱ
            Sleep(350);
            //ֹͣƤ��
            setDO(LS_DO::ED_DO_FeedBeltStart, 0);
        }

        bool haveProduct = false;
        MODBUS_TCP.readIntCoil(34, 6, haveProduct);
        if (haveProduct)
        {
            //�������ϻ����ڷ����У���ȡ
            continue;
        }
        if (ngCount > 2)
        {
            //LSM->m_isEmergency = true;
            machineLog->write("Ƥ�����϶�λʧ�ܣ���", Normal);
            machineLog->write("Ƥ�����϶�λʧ�ܣ���", Err);
            //����������
            LSM->setThreeColorLight(2);
            for (int i = 0; i < 2; i++)
            {
                LSM->setButtonLight(3, 1);
                Sleep(1000);
                LSM->setButtonLight(3, 0);
                Sleep(500);
            }
            m_isLoadErr.store(true);
            return -1;
        }
        //---------------------------- ���� --------------------------------------
        m_photo_map[LS_CameraName::ed_Location].Clear();
        if (!m_cameraName.count(LS_CameraName::ed_Location) ||
            !m_cameraMap.count(LS_CameraName::ed_Location) ||
            !m_cameraMap[LS_CameraName::ed_Location]) {
            machineLog->write("���δ���ã�" + LS_CameraName::ed_Location, Normal);
            ngCount += 1;
            //m_isEmergency.store(true);
            continue;
        }
        DeviceMoveit_Config Moveitinf = m_Paramer;
        //����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //��Դ����  �ߵ�ƽ����
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 1);
        //�ӳٴ���
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        auto rtn = catPhoto2D(m_cameraName[LS_CameraName::ed_Location], m_cameraMap[LS_CameraName::ed_Location]);
        if (rtn != 0)
        {
            machineLog->write("��λ��� ����ʧ�ܣ��������� ��" + QString::number(rtn), Normal);
            ngCount += 1;
            //m_isEmergency.store(true);
            Sleep(5000);
            continue;
        }
        m_photo_map[LS_CameraName::ed_Location].Clear();
        m_photo_map[LS_CameraName::ed_Location] = m_cameraMap[LS_CameraName::ed_Location]->m_outImg.Clone();
        machineLog->write("��λ������ճɹ�", Normal);
        //��Դ����
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //�رչ�Դ
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 0);


        //----------------------------- ��λ�㷨 ------------------------------------
        Mat photo = ImageTypeConvert::HObject2Mat(m_photo_map[LS_CameraName::ed_Location]);
        auto timestamp = QDateTime::currentMSecsSinceEpoch();

        QString appPath = QCoreApplication::applicationDirPath();
        auto robotDetect = new RobotCalibDetect();
        //���в���
        RunParamsStruct_RobotCalib tmpRun;
        //�������
        ResultParamsStruct_RobotCalib tmpResult;
        int processId = 0;	//����ID
        string nodeName = LSM->m_forMulaName.toStdString();	//ģ������
        int processModbuleID = 0;	//ģ��ID

        string ConfigPath = "";
        string XmlPath = "";
        if (!LSM->m_forMulaName.isEmpty())
        {
            //ȡͬ�䷽�ı궨�ļ�
            ConfigPath = appPath.toStdString() + "/Vision_formulation/Robot/";
            XmlPath = appPath.toStdString() + "/Vision_formulation/Robot/" + LSM->m_forMulaName.toStdString() + ".xml";
        }
        else
        {
            ConfigPath = appPath.toStdString() + "/Vision_formulation/Robot/";
            XmlPath = appPath.toStdString() + "/Vision_formulation/Robot/Config.xml";
        }
        //�����в���
        auto errorCode = robotDetect->ReadParams_RobotCalib(ConfigPath, XmlPath, tmpRun, processId, nodeName, processModbuleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("������λ������ȡʧ�� ����", Normal);
            //m_isEmergency.store(true);
            ngCount += 1;
            continue;
        }

        int res = robotDetect->RobotLocate(photo, tmpRun, tmpResult);
        if (res != 0)
        {
            machineLog->write("�����Ӿ���λʧ�� ����", Normal);
            //m_isEmergency.store(true);
            ngCount += 1;
            continue;
        }

        QImage showQImg = ImageTypeConvert::Mat2QImage(tmpResult.DestImg);
        emit showImage_signal(showQImg);
        RobotResult result;

        result.original = photo;
        result.display = tmpResult.DestImg;
        result.x = std::round(tmpResult.hv_X_Robot * 100) / 100;
        result.y = std::round(tmpResult.hv_Y_Robot * 100) / 100;
        result.a = std::round(tmpResult.hv_A_Robot * 100) / 100;
        machineLog->write("result.x = " + QString::number(result.x), Normal);
        machineLog->write("result.y = " + QString::number(result.y), Normal);
        machineLog->write("result.a = " + QString::number(result.a), Normal);

        result.timestamp = timestamp;
        result.consume = 1000;
        ////�������˷����ξ�
        //int num = LSM->m_forMulaName.toInt();
        //if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
        //    && num != 5 && num != 6 && num != 7 &&
        //    num != 8 && num != 9)
        //{
        //    machineLog->write("�䷽�޶�Ӧ�ξߣ���", Normal);
        //    continue;
        //}

        //�������˷����ξߣ��ģ�
        if (LSM->m_formulaToRobot.count(LSM->m_forMulaName))
        {
            int num = LSM->m_formulaToRobot[LSM->m_forMulaName];
            if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
                && num != 5 && num != 6 && num != 7 &&
                num != 8 && num != 9)
            {
                machineLog->write("�䷽�޶�Ӧ�ξ߻����˷�������", Normal);
                continue;
            }

            MODBUS_TCP.writeRegisterInt(138, num);
        }
        else
        {
            machineLog->write("δ�ҵ���Ӧ�䷽�Ļ����˷����������Ӧ��ϵ����", Normal);
            continue;
        }
       

        //�������˷�������
        MODBUS_TCP.writeRegisterFloat(144, result.x);
        MODBUS_TCP.writeRegisterFloat(146, result.y);
        MODBUS_TCP.writeRegisterFloat(148, result.a);

        //�������˷�������ȡ��
        MODBUS_TCP.writeIntCoil(134, 0, 1);
        //�ȴ�������ȡ��
        machineLog->write("�ȴ�������ȡ��", Normal);
        bool feedInBeltPickDone = false;
        MODBUS_TCP.readIntCoil(34,0, feedInBeltPickDone);
        while (!feedInBeltPickDone)
        {
            auto err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            Sleep(5);
            MODBUS_TCP.readIntCoil(34, 0, feedInBeltPickDone);
            //һֱ����ȡ��
            MODBUS_TCP.writeIntCoil(134, 0, 1);
        }
        //����ȡ��
        MODBUS_TCP.writeIntCoil(134, 0, 0);

        MODBUS_TCP.readIntCoil(34, 0, feedInBeltPickDone); 
        materialTimer.start();
        while (feedInBeltPickDone)
        {
            //���ź�
            auto err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 0, feedInBeltPickDone);
            //һֱ����
            MODBUS_TCP.writeIntCoil(134, 0, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ�����������Ƥ������ź�����ʱ��" + QString::number(ERROR_TIME_2) + " ������δ����", Normal);
                machineLog->write("�ȴ�����������Ƥ������ź�����ʱ��" + QString::number(ERROR_TIME_2) + " ������δ����", Err);
                //machineLog->write("modbus 35 bit 3 ��ȡ��ʱ��" + QString::number(ERROR_TIME) + " �����δ�ɹ���ȡ", Normal);
                m_isEmergency.store(true);
                return -1;
            }
        }
        //���ƫ��λ��
        MODBUS_TCP.writeRegisterFloat(144, 0);
        MODBUS_TCP.writeRegisterFloat(146, 0);
        MODBUS_TCP.writeRegisterFloat(148, 0);
        machineLog->write("������ȡ�����..", Normal);
        if (LSM->m_isOneStep.load())
        {
            //��������
            break;
        }

        Sleep(3000);
    }
}

//�������״η��ϵ��ؾߺ���
int rs_motion::runRobotCheck()
{
    QElapsedTimer materialTimer;
    //����ؾ����ϣ������ϣ�ֱ������
    if (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        //�н�����
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ����", Normal);
                machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ����", Err);
                return -1;
            }
            Sleep(5);
        }

        setDO(LS_DO::ED_DO_PositionCyl1, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        materialTimer.start();  // ��ʼ��ʱ
        while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ����", Normal);
                machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ����", Err);
                return -1;
            }
            Sleep(5);
        }
        setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
        setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
        materialTimer.start();  // ��ʼ��ʱ
        while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�����", Normal);
                machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�����", Err);
                return -1;
            }
            Sleep(5);
        }
        return 0;
    }
    ////�ȴ��ؾ�����
    //while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    Sleep(5);
    //}
    //�ȴ���������
    bool haveProduct = false;
    MODBUS_TCP.readIntCoil(34, 6, haveProduct);
    while (!haveProduct)
    {
        if (m_isEmergency.load())
            return -1;
        MODBUS_TCP.readIntCoil(34, 6, haveProduct);
        Sleep(5);
    }

    //---------------------------- Ƥ��ȡ����� �����˷��� --------------------------------------
    //��ȷ���ξ��Ƿ��ڵȴ�λ��������ǣ�Ҫ���ߵ��ȴ�λ
    if (fabs(ReadAxisP(LS_AxisName::Y) - m_Axis[LS_AxisName::Y].initPos) > ERROR_VALUE)
    {
        AxisInitPAsync(LS_AxisName::Y);
    }
    if (fabs(ReadAxisP(LS_AxisName::U1) - m_Axis[LS_AxisName::U1].initPos) > ERROR_VALUE)
    {
        AxisInitPAsync(LS_AxisName::U1);
    }

    m_isRobotLoading.store(true);
    //�������˷����������
    //modbusRobot.writeBit(134, 1, 1);
    MODBUS_TCP.writeIntCoil(134, 1, 1);
    //�ȴ������˷���
    machineLog->write("�ȴ������˷���", Normal);
    bool fixturePlaceDone = false;
    MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
    materialTimer.start();
    while (fixturePlaceDone == 0)
    {
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 1, 1);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ������˷��ϳ�ʱ��" + QString::number(ERROR_TIME_2) + " �����δ�ɹ���ȡ", Normal);
            machineLog->write("�ȴ������˷��ϳ�ʱ��" + QString::number(ERROR_TIME_2) + " �����δ�ɹ���ȡ", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //�رշ����ź�
    MODBUS_TCP.writeIntCoil(134, 1, 0);
    MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
    while (fixturePlaceDone)
    {
        //���ź�
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 1, 0);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ������˷�����ɳ�ʱ��" + QString::number(ERROR_TIME_2) + " �����δ�ɹ���ȡ", Normal);
            machineLog->write("�ȴ������˷�����ɳ�ʱ��" + QString::number(ERROR_TIME_2) + " �����δ�ɹ���ȡ", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }

    //---------------------------- ������� ���ϼ��,�򿪶�λ���н�����--------------------------------------
    while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ����", Normal);
            machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ����", Err);
            return -1;
        }
        Sleep(5);
    }

    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ����", Normal);
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ����", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�����", Normal);
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�����", Err);
            return -1;
        }
        Sleep(5);
    }
    Sleep(200);
    m_isRobotLoading.store(false);
    return 0;
}

//���������Ϻ���
int rs_motion::runRobotUnLoading()
{
    //����ξ���û���ϣ��Ͳ���ȥȡ�ϣ�ֱ�ӵ���
    if (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        //����ȡ��
        return 0;
    }
    QElapsedTimer materialTimer;
    //---------------------------- �ɿ��ξ� --------------------------------------
    //������ȡ����
    m_isRobotLoading.store(true);
    setDO(LS_DO::ED_DO_PositionCyl1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Home) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ�ɿ��ξߣ���", Normal);
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ�ɿ��ξߣ���", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 0);
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampCyl1_Home) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Home) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ�ɿ��ξߣ���", Normal);
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ�ɿ��ξߣ���", Err);
            return -1;
        }
        Sleep(5);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    //---------------------------- �ξ�ȡ�� ������ͨѶ�������źż�� --------------------------------------
  

    bool fixturePickDone = false;
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("ȡ��ǰ fixturePickDone��" + QString::number(fixturePickDone), Normal);
    //����ȡ��
    MODBUS_TCP.writeIntCoil(134, 2, 1);
    Sleep(100);
    machineLog->write("�ȴ��������ξ�ȡ��", Normal);
    //�ȴ��������ξ�ȡ�����
    materialTimer.start();
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("ȡ���� fixturePickDone��" + QString::number(fixturePickDone), Normal);
    while (!fixturePickDone)
    {
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 2, 1);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ��������ξ�ȡ�ϣ�" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
            machineLog->write("�ȴ��������ξ�ȡ�ϣ�" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //��ջ������ξ�ȡ���ź�
    MODBUS_TCP.writeIntCoil(134, 2, 0);
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("ȡ�Ϻ� fixturePickDone��" + QString::number(fixturePickDone), Normal);
    materialTimer.start();
    while (fixturePickDone)
    {
        //���ź�
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 2, 0);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ��������ξ�ȡ������ź���ճ�ʱ��" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
            machineLog->write("�ȴ��������ξ�ȡ������ź���ճ�ʱ��" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    machineLog->write("���ź� fixturePickDone��" + QString::number(fixturePickDone), Normal);
    //�ξ����ϼ��
    materialTimer.start();
    while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ξ߲�Ʒȡ�ϼ�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��ɼ�⵽��Ʒ��ȡ��", Normal);
            machineLog->write("�ξ߲�Ʒȡ�ϼ�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��ɼ�⵽��Ʒ��ȡ��", Err);
            return -1;
        }
        Sleep(5);
    }
    machineLog->write("�������ξ�ȡ�����...", Normal);
    //������ȡ�����
    m_isRobotLoading.store(false);
    return 0;
}

//���������ϵ�Ƥ������
int rs_motion::runRobotUnLoad_Belt(bool isNG)
{
    QElapsedTimer materialTimer;
    m_isRobotLoading.store(true);
    //����
    //isNG = false;

    //���NG��MESûͨ������NG
    if (isNG || !LSM->m_mesResponse_ED.state)
    {
        //���NGƤ���Ƿ�����
        if (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1
            && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
            && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            //���ϱ���
            machineLog->write("NGƤ�����ϣ�����ȡ�ϣ���", Normal);
            machineLog->write("NGƤ�����ϣ�����ȡ�ϣ���", Err);
            while (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
            {
                //�������û�б��ÿ����ÿ��˾ͼ���
                if (m_isEmergency.load())
                    return -1;
                Sleep(5);
            }
            //return -1;
        }
        //���Ƥ���Ƿ���Ҫ��ǰ�˶�
        if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            //�ұߺ���߶����ϣ�NGƤ����ǰ�ߵ�����ⲻ����
            setDO(LS_DO::ED_DO_NGBeltStart, 1);
            machineLog->write("����NGƤ��...", Normal);
            materialTimer.start();
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 || getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("����" + QString::number(ERROR_TIME) + " �����NGƤ���Լ�����ϣ���鿴�Ƿ����ϣ���", Normal);
                    machineLog->write("����" + QString::number(ERROR_TIME) + " �����NGƤ���Լ�����ϣ���鿴�Ƿ����ϣ���", Err);
                    setDO(LS_DO::ED_DO_NGBeltStart, 0);
                    return -1;
                }
                Sleep(5);
            }
            Sleep(100);
            setDO(LS_DO::ED_DO_NGBeltStart, 0);
            machineLog->write("ֹͣNGƤ��...", Normal);
        }
        //���NG���ź�
        if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
        {
            //�����
            MODBUS_TCP.writeIntCoil(134, 4, 1);
            Sleep(200);
            machineLog->write("�����˷�����NGƤ��...", Normal);
            bool ngBeltLeftPlaceDone = false;
            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            materialTimer.start();
            while (!ngBeltLeftPlaceDone)
            {
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                Sleep(5);
                MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
                Sleep(5);
                MODBUS_TCP.writeIntCoil(134, 4, 1);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
                    machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //����Ƿ���ϳɹ�
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("NGƤ����Ʒ���ϼ��1���󣩼�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ", Normal);
                    machineLog->write("NGƤ����Ʒ���ϼ��1���󣩼�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ", Err);
                    return -1;
                }
                Sleep(5);
            }

            machineLog->write("�����˷�����NG���...", Normal);
            MODBUS_TCP.writeIntCoil(134, 4, 0);

            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            materialTimer.start();
            while (ngBeltLeftPlaceDone)
            {
                //���ź�
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
                //һֱ����
                MODBUS_TCP.writeIntCoil(134, 4, 0);
                Sleep(5);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("�ȴ������˷�����NGƤ��������źų�ʱ��" + QString::number(ERROR_TIME_2), Normal);
                    machineLog->write("�ȴ������˷�����NGƤ��������źų�ʱ��" + QString::number(ERROR_TIME_2), Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //����������
            for (int i = 0; i < 3; i++)
            {
                LSM->setButtonLight(3, 1);
                Sleep(500);
                LSM->setButtonLight(3, 0);
                Sleep(500);
            }
        }
        else if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
        {
            //���ұ�
            MODBUS_TCP.writeIntCoil(134, 5, 1);
            Sleep(200);
            machineLog->write("�����˷�����NGƤ��...", Normal);
            bool ngBeltRightPlaceDone = false;
            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            materialTimer.start();
            while (!ngBeltRightPlaceDone)
            {
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                Sleep(5);
                MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
                Sleep(5);
                MODBUS_TCP.writeIntCoil(134, 5, 1);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
                    machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }
           
            //����Ƿ���ϳɹ�
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("NGƤ����Ʒ���ϼ��2���ң���ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ", Normal);
                    machineLog->write("NGƤ����Ʒ���ϼ��2���ң���ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ", Err);
                    return -1;
                }
                Sleep(5);
            }

            machineLog->write("�����˷�����NG���...", Normal);
            MODBUS_TCP.writeIntCoil(134, 5, 0);

            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            materialTimer.start();
            while (ngBeltRightPlaceDone)
            {
                //���ź�
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
                //һֱ����
                MODBUS_TCP.writeIntCoil(134, 5, 0);
                Sleep(5);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("�ȴ������˷�����NGƤ��������źų�ʱ��" + QString::number(ERROR_TIME_2), Normal);
                    machineLog->write("�ȴ������˷�����NGƤ��������źų�ʱ��" + QString::number(ERROR_TIME_2), Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //����������
            for (int i = 0; i < 3; i++)
            {
                LSM->setButtonLight(3, 1);
                Sleep(500);
                LSM->setButtonLight(3, 0);
                Sleep(500);
            }
        }

    }
    else
    {
        //������Ƥ�����Ƿ�����
        if (getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1)
        {
            machineLog->write("����Ƥ�����ϴ����ϣ�����������Ƥ��...", Normal);
            setDO(LS_DO::ED_DO_DischargeBeltStart, 1);
            materialTimer.start();
            while (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                //if (materialTimer.elapsed() > ERROR_TIME) {
                //    machineLog->write("����" + QString::number(ERROR_TIME) + " ��������Ƥ�����ϴ��Լ�����ϣ���鿴Ƥ���Ƿ��쳣����", Normal);
                //    setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
                //    return -1;
                //}
                Sleep(5);
            }
            Sleep(100);
            setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
        }
        //�����˷��ϳ���Ƥ��
        MODBUS_TCP.writeIntCoil(134, 3, 1);
        Sleep(100);
        machineLog->write("�����˷��ϳ���Ƥ��...", Normal);
        bool dischargeBeltPlaceDone = false;
        MODBUS_TCP.readIntCoil(34, 3, dischargeBeltPlaceDone);
        materialTimer.start();
        while (!dischargeBeltPlaceDone)
        {
            auto err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            Sleep(5);
            MODBUS_TCP.readIntCoil(34, 3, dischargeBeltPlaceDone);
            Sleep(5);
            MODBUS_TCP.writeIntCoil(134, 3, 1);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷��ϳ���Ƥ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
                machineLog->write("�ȴ������˷��ϳ���Ƥ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("�����˷��ϳ���Ƥ�����...", Normal);
        MODBUS_TCP.writeIntCoil(134, 3, 0);
        //�������˷�������ź��Ƿ�����
        MODBUS_TCP.readIntCoil(34, 3, dischargeBeltPlaceDone);
        while (dischargeBeltPlaceDone)
        {
            auto err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            Sleep(5);
            MODBUS_TCP.readIntCoil(34, 3, dischargeBeltPlaceDone);
            Sleep(5);
            MODBUS_TCP.writeIntCoil(134, 3, 0);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷�������ź��������ʱ��" + QString::number(ERROR_TIME_2) + " ������δ����", Normal);
                machineLog->write("�ȴ������˷�������ź��������ʱ��" + QString::number(ERROR_TIME_2) + " ������δ����", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        //Sleep(100);
        //����Ƿ���ϳɹ�
        while (getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("����Ƥ����Ʒ���ϼ�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��⵽��Ʒ", Normal);
                machineLog->write("����Ƥ����Ʒ���ϼ�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��⵽��Ʒ", Err);
                return -1;
            }
            Sleep(5);
        }
        if (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
        {
            //Ƥ���������Ѿ����ϣ�������
            return 0;
        }
        //��������Ƥ��
        machineLog->write("��������Ƥ��...", Normal);
        setDO(LS_DO::ED_DO_DischargeBeltStart, 1);
        //Sleep(1000);
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("�ȴ�����Ƥ����Ʒ��λ��⣺" + QString::number(ERROR_TIME) + " �������δ��λ����", Normal);
                machineLog->write("�ȴ�����Ƥ����Ʒ��λ��⣺" + QString::number(ERROR_TIME) + " �������δ��λ����", Err);
                setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
                return -1;
            }
            Sleep(5);
        }
        Sleep(100);
        machineLog->write("ֹͣ����Ƥ��...", Normal);
        setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
    }
    m_isRobotLoading.store(false);
    return 0;
}

//�����˷��ϵ��ξ�
int rs_motion::runRobotLoad_Tool()
{
    QElapsedTimer materialTimer;
    //�������û����ֱ�ӷ���
    bool haveProduct = false;
    MODBUS_TCP.readIntCoil(34, 6, haveProduct);
    if (!haveProduct)
    {
        return 0;
    }
    
    //�ȴ��ؾ�����
    while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        if (m_isEmergency.load())
            return -1;
        Sleep(5);
    }
    //---------------------------- �ξ�ȡ����� �����˷����ξ� --------------------------------------
    m_isRobotLoading.store(true);
    //�������˷����������
    MODBUS_TCP.writeIntCoil(134, 1, 1);
    //�ȴ������˷���
    machineLog->write("�ȴ������˷���", Normal);
    bool fixturePlaceDone = false;
    MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
    materialTimer.start();
    while (fixturePlaceDone == 0)
    {
        int err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 1, 1);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ������˷����ξߣ�" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
            machineLog->write("�ȴ������˷����ξߣ�" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //�رշ����ź�
    MODBUS_TCP.writeIntCoil(134, 1, 0);
    MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
    materialTimer.start();
    while (fixturePlaceDone)
    {
        int err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 1, 0);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("�ȴ������˷����ξ�����ź����㳬ʱ��" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
            machineLog->write("�ȴ������˷����ξ�����ź����㳬ʱ��" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }

    //---------------------------- ������� ���ϼ��,�򿪶�λ���н�����--------------------------------------
    while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ", Normal);
            machineLog->write("�ξ߲�Ʒ��ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ��ɼ�⵽��Ʒ", Err);
            return -1;
        }
        Sleep(5);
    }

    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ", Normal);
            machineLog->write("�ξ߶�λ���׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߶�λ", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    materialTimer.start();  // ��ʼ��ʱ
    while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�", Normal);
            machineLog->write("�ξ߼н����׼�ⳬʱ��" + QString::number(ERROR_TIME) + " ������δ����ξ߼н�", Err);
            return -1;
        }
        Sleep(5);
    }
    Sleep(200);
    m_isRobotLoading.store(false);
    return 0;
}

//����������
int rs_motion::runRobotClearHand()
{
    //ͳһ��NGƤ��
    QElapsedTimer materialTimer;
    //���NGƤ���Ƿ�����
    if (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1
        && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
        && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
    {
        //���ϱ���
        machineLog->write("NGƤ�����ϣ�����ȡ�ϣ���", Normal);
        while (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
        {
            //�������û�б��ÿ����ÿ��˾ͼ���
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //return -1;
    }
    //���Ƥ���Ƿ���Ҫ��ǰ�˶�
    if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
    {
        //�ұߺ���߶����ϣ�NGƤ����ǰ�ߵ�����ⲻ����
        setDO(LS_DO::ED_DO_NGBeltStart, 1);
        machineLog->write("����NGƤ��...", Normal);
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 || getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("����" + QString::number(ERROR_TIME) + " �����NGƤ���Լ�����ϣ���鿴�Ƿ����ϣ���", Normal);
                machineLog->write("����" + QString::number(ERROR_TIME) + " �����NGƤ���Լ�����ϣ���鿴�Ƿ����ϣ���", Err);
                setDO(LS_DO::ED_DO_NGBeltStart, 0);
                return -1;
            }
            Sleep(5);
        }
        Sleep(100);
        setDO(LS_DO::ED_DO_NGBeltStart, 0);
        machineLog->write("ֹͣNGƤ��...", Normal);
    }
    //���NG���ź�
    if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
    {
        //�����
        MODBUS_TCP.writeIntCoil(134, 4, 1);
        machineLog->write("�����˷�����NGƤ��...", Normal);
        Sleep(500);
        bool ngBeltLeftPlaceDone = false;
        MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
        materialTimer.start();
        while (!ngBeltLeftPlaceDone)
        {
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (res != 0)
            {
                m_isEmergency.store(true);
                return -1;
            }
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            Sleep(5);
            MODBUS_TCP.writeIntCoil(134, 4, 1);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
                machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("�����˷�����NG���...", Normal);
        MODBUS_TCP.writeIntCoil(134, 4, 0);

        MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
        materialTimer.start();
        while (ngBeltLeftPlaceDone)
        {
            //���ź�
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            //һֱ����
            MODBUS_TCP.writeIntCoil(134, 4, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷�����NGƤ�����źų�ʱ��" + QString::number(ERROR_TIME_2), Normal);
                machineLog->write("�ȴ������˷�����NGƤ�����źų�ʱ��" + QString::number(ERROR_TIME_2), Err);
                m_isEmergency.store(true);
                return -1;
            }
        }

        //����Ƿ���ϳɹ�
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("NGƤ����Ʒ���ϼ��1���󣩼�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ����", Normal);
                machineLog->write("NGƤ����Ʒ���ϼ��1���󣩼�ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ����", Err);
                m_isEmergency.store(true);
                return -1;
            }
            Sleep(5);
        }
    }
    else if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
    {
        //���ұ�
        MODBUS_TCP.writeIntCoil(134, 5, 1);
        machineLog->write("�����˷�����NGƤ��...", Normal);
        Sleep(500);
        bool ngBeltRightPlaceDone = false;
        MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
        materialTimer.start();
        while (ngBeltRightPlaceDone)
        {
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            Sleep(5);
            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            Sleep(5);
            MODBUS_TCP.writeIntCoil(134, 5, 1);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Normal);
                machineLog->write("�ȴ������˷�����NGƤ����" + QString::number(ERROR_TIME_2) + " ������δ���ȡ��", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("�����˷�����NG���...", Normal);
        MODBUS_TCP.writeIntCoil(134, 5, 0);

        MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
        materialTimer.start();
        while (ngBeltRightPlaceDone)
        {
            //���ź�
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            //һֱ����
            MODBUS_TCP.writeIntCoil(134, 5, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("�ȴ������˷�����NGƤ�����źų�ʱ��" + QString::number(ERROR_TIME_2), Normal);
                machineLog->write("�ȴ������˷�����NGƤ�����źų�ʱ��" + QString::number(ERROR_TIME_2), Err);
                m_isEmergency.store(true);
                return -1;
            }
        }

        //����Ƿ���ϳɹ�
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("NGƤ����Ʒ���ϼ��2���ң���ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ����", Normal);
                machineLog->write("NGƤ����Ʒ���ϼ��2���ң���ⳬʱ��" + QString::number(ERROR_TIME_2) + " ������δ��⵽��Ʒ����", Err);
                return -1;
            }
            Sleep(5);
        }
    }
    return 0;
}

//������ʼ���������
void rs_motion::initAllVariable()
{
    //������
    m_runResult.clear();
    //����Ѿ���¼���Ӿ��䷽
    m_visionFormula_2D.clear();
    m_visionFormula_3D.clear();
    //����Ѿ���¼2D�Ľ��
    for (auto temp2d : m_pinResult2D)
    {
        temp2d.first.clear();
    }
    m_pinResult2D.clear();
    for (auto photo : m_photo_map)
    {
        if (photo.second.IsInitialized())
        {
            photo.second.Clear();
        }
    }
    m_photo_map.clear();
    // ���3D��������
    m_pinResult3D.clear();
    for (auto photo3D : m_cloud_ori_map)
    {
        if (!photo3D.second->empty())
        {
            //photo3D.second->clear();
            photo3D.second.reset();
        }
    }
    m_cloud_ori_map.clear();
    //������Ҫ�ϲ���ͼƬ
    for (auto photo : m_resultMergingImg)
    {
        photo.second.clear();
    }
    m_resultMergingImg.clear();
    if (m_pcMyCamera_3D)
    {
        //m_pcMyCamera_3D->m_cloud_ori_list.clear();
        //m_pcMyCamera_3D->m_cloud_ori_list.shrink_to_fit();

        for (auto photo3D : m_pcMyCamera_3D->m_cloud_ori_list)
        {
            if (!photo3D->empty())
            {
                //photo3D.second->clear();
                photo3D.reset();
            }
        }
        m_pcMyCamera_3D->m_cloud_ori_list.clear();
    }
    for (auto camera : m_cameraMap)
    {
        if (camera.second)
        {
            camera.second->m_outImg.Clear();
        }
    }
    //�����Ѽ�¼�ĵ�������
    for (auto& pair : m_ResultToCsv) {
        pair.second.clear(); 
    }
}

//��ɫ����˸
void rs_motion::redLightFlashing_slot()
{
    int state = 0;
    if (m_version == LsPin)
    {
        state = getDO(LS_DO::LS_OUT_RedLight);
    }
    else if (m_version == TwoPin || m_version == EdPin)
    {
        state = getDO(LS_DO::DO_LightRed);
    }
    if (state == 1)
    {
        //��
        setThreeColorLight(2, true);
    }
    else
    {
        //��
        setThreeColorLight(2);
    }
}

// ��ʼ��CSV�ṹ����
void rs_motion::initCsvPara()
{
    m_ResultToCsv["name"] = "";
    m_ResultToCsv["SN"] = "";
    m_ResultToCsv["��Ʒ��"] = "";
    m_ResultToCsv["�ͻ���"] = "";
    m_ResultToCsv["��Ʒ���ظ���"] = "";
    m_ResultToCsv["�ͻ����ظ���"] = "";
    m_ResultToCsv["������1"] = "";
    m_ResultToCsv["������2"] = "";
    m_ResultToCsv["������3"] = "";
    m_ResultToCsv["������4"] = "";
    m_ResultToCsv["2D���"] = "";
    m_ResultToCsv["3D���"] = "";
    m_ResultToCsv["��˿���1"] = "";
    m_ResultToCsv["��˿���2"] = "";
    m_ResultToCsv["��˿���3"] = "";
    m_ResultToCsv["��˿���4"] = "";
    m_ResultToCsv["��ǩ�ַ����"] = "";
    m_ResultToCsv["���ۼ��"] = "";
    m_ResultToCsv["��ˮ͸��Ĥ���"] = "";
    m_ResultToCsv["�ܷ⽺���"] = "";
    m_ResultToCsv["Բ�����1"] = "";
    m_ResultToCsv["Բ�����2"] = "";
    m_ResultToCsv["Բ�����3"] = "";
    m_ResultToCsv["Բ�����4"] = "";
    m_ResultToCsv["����ʱ��"] = "";
    m_ResultToCsv["����ʱ��"] = "";
    m_ResultToCsv["ͼƬ·��"] = "";
    m_ResultToCsv["������"] = "";
    m_ResultToCsv["���ս��"] = "";
    m_ResultToCsv["����"] = "";

    m_csvOrder.push_back("name");
    m_csvOrder.push_back("����");
    m_csvOrder.push_back("SN");
    m_csvOrder.push_back("��Ʒ��");
    m_csvOrder.push_back("�ͻ���");
    m_csvOrder.push_back("��Ʒ���ظ���");
    m_csvOrder.push_back("�ͻ����ظ���");
    m_csvOrder.push_back("������1");
    m_csvOrder.push_back("������2");
    m_csvOrder.push_back("������3");
    m_csvOrder.push_back("������4");
    m_csvOrder.push_back("2D���");
    m_csvOrder.push_back("3D���");
    m_csvOrder.push_back("��˿���1");
    m_csvOrder.push_back("��˿���2");
    m_csvOrder.push_back("��˿���3");
    m_csvOrder.push_back("��˿���4");
    m_csvOrder.push_back("��ǩ�ַ����");
    m_csvOrder.push_back("���ۼ��");
    m_csvOrder.push_back("��ˮ͸��Ĥ���");
    m_csvOrder.push_back("�ܷ⽺���");
    m_csvOrder.push_back("Բ�����1");
    m_csvOrder.push_back("Բ�����2");
    m_csvOrder.push_back("Բ�����3");
    m_csvOrder.push_back("Բ�����4");
    m_csvOrder.push_back("����ʱ��");
    m_csvOrder.push_back("����ʱ��");
    m_csvOrder.push_back("ͼƬ·��");
    m_csvOrder.push_back("������");
    m_csvOrder.push_back("���ս��");
}


// ͨ���豸���MES����
void rs_motion::checkMesFunction()
{
    if (m_version == EdPin)
    {
        //�׶��� �Զ�����csv
        if (!m_Paramer.mesCSV.isEmpty())
        {
            QString sn_tp = "";

            //if (!m_requestSn.isEmpty())
            //    sn_tp = m_requestSn;
            if (!m_ResultToCsv["SN"].isEmpty())
            {
                sn_tp = m_ResultToCsv["SN"];
                m_requestSn = m_ResultToCsv["SN"];
            }
            else
            {
                m_requestSn.clear();
            }
            //�����ò��Ե�SN
            if (!testSN.isEmpty())
            {
                sn_tp = testSN;
                m_ResultToCsv["SN"] = testSN;
            }

            m_ResultToCsv["name"] = m_forMulaName;
            //m_ResultToCsv["SN"] = sn_tp;
            m_ResultToCsv["����ʱ��"] = m_runStartTime;
            m_ResultToCsv["����ʱ��"] = m_runStartTime;
            m_ResultToCsv["ͼƬ·��"] = m_resultImgPath;
            m_ResultToCsv["���ս��"] = m_runResult.isNG ? "NG" : "OK";
            m_ResultToCsv["2D���"] = m_runResult.isNG_2D ? "NG" : "OK";
            m_ResultToCsv["3D���"] = m_runResult.isNG_3D ? "NG" : "OK";
            m_ResultToCsv["������"] = m_productPara.UserAccout;
            m_ResultToCsv["����"] = m_productPara.WorkOrderCode;
            QString filePath = m_Paramer.mesCSV + "/" + sn_tp + "_" + m_runStartTime;
            //������CSV
            m_functionTool.saveCsvInThread(filePath, m_ResultToCsv, m_csvOrder);

            //----------------------------��mes����--------------------------
            if (!LSM->notMes && !LSM->m_Paramer.mesUrl.isEmpty())
            {
                QString productSN = m_ResultToCsv["��Ʒ��"];
                if (sn_tp.isEmpty())
                {
                    //ֱ��NG
                    m_mesResponse_ED.state = false;
                    m_mesResponse_ED.description = "��ά��ʶ��δ���ҵ�SN��";
                    m_runResult.isNG = true;
                    machineLog->write("��ά��ʶ��δ���ҵ�SN�룬�޷��ϴ�MES", Mes);
                }
                else
                {
                    // ׼����������
                    MesCheckInRequest_ED request;
                    request.sn = sn_tp;
                    request.productSn = productSN;
                    //request.IP = "192.168.1.100";
                    //request.Slot = "A3";

                    // ��������
                    rs_FunctionTool tool;
                    m_mesResponse_ED = tool.mes_check_in_ED(LSM->m_Paramer.mesUrl, request);

                    // ������Ӧ
                    if (m_mesResponse_ED.state) {
                        machineLog->write("MESУ��ͨ��", Mes);

                        // ��m_ResultToCsvӳ�䵽�ṹ��
                        MesCheckInFullRequest_ED mesRequest = tool.mapToMesRequest(m_ResultToCsv);
                        mesRequest.WorkOrderCode = m_productPara.WorkOrderCode;
                        // ���ù�վ����
                        MesCheckInResponse_ED response = tool.mes_full_check_in(LSM->m_Paramer.mesUrl, mesRequest);

                        if (response.state) {
                            machineLog->write("MES��վ�ɹ�", Mes);
                            return;
                        }
                        else {
                            machineLog->write("MES��վʧ�ܣ�" + m_mesResponse_ED.description, Mes);
                            m_runResult.isNG = true;
                            return;
                        }
                    }
                    else {
                        machineLog->write("MESУ��ʧ�ܣ�" + m_mesResponse_ED.description, Mes);
                        m_runResult.isNG = true;
                        return;
                    }
                }
            }

        }
    }
}

// ����pin�������
int rs_motion::pinJmAction(PointItemConfig item)
{
    int rtn = 0;
    const QString axisX = LS_AxisName::X;
    const QString axisX1 = LS_AxisName::X1;
    const QString axisY = LS_AxisName::Y;
    const QString axisU = LS_AxisName::U;
    const QString axisU1 = LS_AxisName::U1;
    const QString axisZ = LS_AxisName::Z;
    const QString axisZ1 = LS_AxisName::Z1;

    const double TargetPX = item.TargetPAxis_1;
    const double TargetPX1 = item.TargetPAxis_2;
    const double TargetPY = item.TargetPAxis_3;
    const double TargetPU = item.TargetPAxis_4;
    const double TargetPU1 = item.TargetPAxis_5;
    const double TargetPZ = item.TargetPAxis_6;
    const double TargetPZ1 = item.TargetPAxis_7;


    //�����������ٶ�
    auto model = (SpeedModeEnum)item.SpeedModel;
    for (auto axis : LS_AxisName::allAxis)
    {
        double setspeed = m_Axis[axis].minSpeed;
        switch (model)
        {
        case SpeedModeEnum::Low:
            setspeed = m_Axis[axis].minSpeed;
            break;
        case SpeedModeEnum::Mid:
            setspeed = m_Axis[axis].midSpeed;
            break;
        case SpeedModeEnum::High:
            setspeed = m_Axis[axis].maxSpeed;
            break;
        default:
            break;
        }
        setSpeed(axis, setspeed);
    }

    //�ж�Z���ƶ�����
    if (TargetPZ < m_Axis[axisZ].minTravel || TargetPZ > m_Axis[axisZ].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽZ������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽZ������λ���������󣡣�", Err);
        return -1;
    }
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //��ִ��Z���ջض���
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //�ж�Z1���ƶ�����
    if (TargetPZ1 < m_Axis[axisZ1].minTravel || TargetPZ1 > m_Axis[axisZ1].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽZ1������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽZ1������λ���������󣡣�", Err);
        return -1;
    }
    bool isPostiveMove_Z1 = isMovePostive(axisZ1, TargetPZ1);
    //��ִ��Z1���ջض���
    if (!isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z1���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //�������첽ִ��
    if (TargetPU < m_Axis[axisU].minTravel || TargetPU > m_Axis[axisU].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽU������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽU������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX < m_Axis[axisX].minTravel || TargetPX > m_Axis[axisX].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽX������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽX������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX1 < m_Axis[axisX1].minTravel || TargetPX1 > m_Axis[axisX1].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽX1������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽX1������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisX1, TargetPX1);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return -1;
    }
    if (TargetPY < m_Axis[axisY].minTravel || TargetPY > m_Axis[axisY].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽY������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽY������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPU1 < m_Axis[axisU1].minTravel || TargetPU1 > m_Axis[axisU1].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽU1������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽU1������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisU1, TargetPU1);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U1�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //�ȴ������ᵽλ
    while (isRuning(axisX)
        || isRuning(axisX1)
        || isRuning(axisY)
        || isRuning(axisU1)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60�볬ʱ����ʱԤ��   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("�ᵽλ��ʱ����", Normal);
            break;
        }
    }

    //��ִ��Z�������λ����
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z1������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    return 0;
}

// ���������������
int rs_motion::hanJmAction(PointItemConfig item)
{
    int rtn = 0;
    const QString axisX = LS_AxisName::X;
    const QString axisY = LS_AxisName::Y;
    const QString axisU = LS_AxisName::U;
    const QString axisZ = LS_AxisName::Z;

    const double TargetPX = item.TargetPAxis_1;
    const double TargetPY = item.TargetPAxis_2;
    const double TargetPU = item.TargetPAxis_4;
    const double TargetPZ = item.TargetPAxis_5;

    //�����������ٶ�
    auto model = (SpeedModeEnum)item.SpeedModel;
    for (auto axis : LS_AxisName::allAxis)
    {
        double setspeed = m_Axis[axis].minSpeed;
        switch (model)
        {
        case SpeedModeEnum::Low:
            setspeed = m_Axis[axis].minSpeed;
            break;
        case SpeedModeEnum::Mid:
            setspeed = m_Axis[axis].midSpeed;
            break;
        case SpeedModeEnum::High:
            setspeed = m_Axis[axis].maxSpeed;
            break;
        default:
            break;
        }
        setSpeed(axis, setspeed);
    }

    //�ж�Z���ƶ�����
    if (TargetPZ < m_Axis[axisZ].minTravel || TargetPZ > m_Axis[axisZ].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽZ������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽZ������λ���������󣡣�", Err);
        return -1;
    }
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //��ִ��Z���ջض���
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z���ջض������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //�������첽ִ��
    if (TargetPU < m_Axis[axisU].minTravel || TargetPU > m_Axis[axisU].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽU������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽU������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("����ִ��U�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX < m_Axis[axisX].minTravel || TargetPX > m_Axis[axisX].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽX������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽX������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("����ִ��X�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }

    if (TargetPY < m_Axis[axisY].minTravel || TargetPY > m_Axis[axisY].maxTravel)
    {
        machineLog->write("ʾ��λ�ó�ԽY������λ���������󣡣�", Normal);
        machineLog->write("ʾ��λ�ó�ԽY������λ���������󣡣�", Err);
        return -1;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("����ִ��Y�ᶯ�����󣡣������� ��" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //�ȴ������ᵽλ
    while (isRuning(axisX)
        || isRuning(axisY)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("�ȴ������ᵽλ���󣡣�", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60�볬ʱ����ʱԤ��   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("�ᵽλ��ʱ����", Normal);
            break;
        }
    }

    //��ִ��Z�������λ����
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("ִ��Z������������󣡣������� ��" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    return 0;
}

