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

    //轴参数初始化
    for (const auto& name : LS_AxisName::allAxis)
    {
        m_Axis[name] = Axis_Config(); 
    }

    //DI参数初始化
    for (const auto& name : LS_DI::allDI)
    {
        m_DI[name] = DI_Config();
    }

    //DO参数初始化
    for (const auto& name : LS_DO::allDO)
    {
        m_DO[name] = DO_Config();
    }

    //配置参数初始化
    //for (const auto& name : LS_ParamerName::allParamer)
    //{
    //    m_Paramer[name] = DeviceMoveit_Config();
    //}
    m_Paramer = DeviceMoveit_Config();

    //加载配方参数
    loadFormulaFromFile();
    ////初始化3D相机
    //init3dCamera();

    //m_pcMyCamera = std::make_unique<CMvCamera>();

    //建立项目相机的对应关系
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

    //初始化自动导出的内容参数
    initCsvPara();

    //连接红色灯闪烁
    connect(&m_redLightFlashing, &QTimer::timeout, this, &rs_motion::redLightFlashing_slot);
}

//销毁单例
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
        if (d1000_board_init() <= 0)//控制卡初始化
        {
            machineLog->write(QString::fromStdString("控制卡连接异常，请检查配置及硬件！"), Normal);
            machineLog->write(QString::fromStdString("控制卡连接异常，请检查配置及硬件！"), Err);
        }
        else
            result = true;
    }
    else
    {
        WORD wCardNum;      //定义卡数
        WORD arrwCardList[8];   //定义卡号数组
        DWORD arrdwCardTypeList[8];   //定义各卡类型
        bool b1 = 0;
        if (dmc_board_init() <= 0)      //控制卡的初始化操作
        {    //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("控制卡连接异常，请检查配置及硬件！"));
            machineLog->write(QString::fromStdString("控制卡连接异常，请检查配置及硬件！"), Normal);
            machineLog->write(QString::fromStdString("控制卡连接异常，请检查配置及硬件！"), Err);
        }
        else
            b1 = true;
        dmc_get_CardInfList(&wCardNum, arrdwCardTypeList, arrwCardList);    //获取正在使用的卡号列表
        //m_wCard = arrwCardList[0];

        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
        {

            //bool b2 = CANInit(0, 1);//初始化CAN-IO
            bool b3 = CANInit(0, 2);

            //if (!b2)
            //{
            //    //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO扩展卡1连接异常，请检查配置及硬件！"));
            //    machineLog->write(QString::fromStdString("IO扩展卡1连接异常，请检查配置及硬件！"), Normal);
            //}
            if (!b3)
            {
                //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO扩展卡2连接异常，请检查配置及硬件！"));
                machineLog->write(QString::fromStdString("IO扩展卡连接异常，请检查配置及硬件！"), Normal);
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

//参数保存
void rs_motion::saveMotionConfig()
{
    // 创建一个 JSON 对象
    QJsonObject rootObject;

    // 保存各轴参数
    QJsonObject axisParams;

    for (auto it = m_Axis.begin(); it != m_Axis.end(); ++it) {
        const QString& axisName = it->first;      // 键：轴名
        const Axis_Config& axisConfig = it->second; // 值：轴配置

        QJsonObject axisObject;

        // 将轴配置的每个字段保存到 JSON 对象中
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

        // 使用轴名作为 JSON 的键
        axisParams[axisName] = axisObject;
    }

    // 将轴参数添加到根对象
    rootObject["axisParams"] = axisParams;

    //**********************************************************************************************
    // 保存 DI 参数
    QJsonObject diParams;

    for (auto it = m_DI.begin(); it != m_DI.end(); ++it) {
        const QString& diName = it->first;        // 键：DI 名称
        const DI_Config& diConfig = it->second;  // 值：DI 配置

        QJsonObject diObject;

        // 将 DI 配置的每个字段保存到 JSON 对象中
        diObject["cardNum"] = diConfig.card;
        diObject["can"] = diConfig.can;
        diObject["channel"] = diConfig.channel;
        diObject["state"] = diConfig.state;

        // 使用 DI 名称作为 JSON 的键
        diParams[diName] = diObject;
    }

    // 将 DI 参数添加到根对象
    rootObject["diParams"] = diParams;
    //**********************************************************************************************
    // 保存 DO 参数
    QJsonObject doParams;

    for (auto it = m_DO.begin(); it != m_DO.end(); ++it) {
        const QString& doName = it->first;        
        const DO_Config& dOConfig = it->second;  

        QJsonObject doObject;

        // 将 DO 配置的每个字段保存到 JSON 对象中（状态先不存，应该用不上，后面会直接读取）
        doObject["cardNum"] = dOConfig.card;
        doObject["can"] = dOConfig.can;
        doObject["channel"] = dOConfig.channel;
        doObject["level"] = dOConfig.level;

        // 使用 DO 名称作为 JSON 的键
        doParams[doName] = doObject;
    }

    // 将 DO 参数添加到根对象
    rootObject["doParams"] = doParams;

    //**********************************************************************************************

    // 保存 配置 参数
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
    
    // 将 配置 参数添加到根对象
    rootObject["deviceParams"] = deviceParams;

    //**********************************************************************************************

    // 保存 工位对应相机 参数
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

    // 将 工位对应相机 参数添加到根对象
    rootObject["cameraParams"] = cameraParams;

    //**********************************************************************************************

    // 将 JSON 对象转换为 JSON 文档
    QJsonDocument jsonDoc(rootObject);

    // 获取当前 exe 文件所在目录
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/motionConfig.json";

    // 写入文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented)); // 格式化为美观的 JSON
    file.close();

    qDebug() << "Motion configuration saved to:" << filePath;
}

//参数读取
void rs_motion::loadMotionConfig()
{
    // 获取当前 exe 文件所在目录
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/motionConfig.json";

    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return;
    }

    // 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();

    // 解析 JSON 文档
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // 读取各轴参数
    if (rootObject.contains("axisParams") && rootObject["axisParams"].isObject()) {
        QJsonObject axisParams = rootObject["axisParams"].toObject();
        for (auto it = axisParams.begin(); it != axisParams.end(); ++it) {
            QString axisName = it.key(); // 获取轴名称
            if (!it.value().isObject()) {
                continue;
            }

            QJsonObject axisObject = it.value().toObject();

            // 初始化当前轴的配置
            Axis_Config axisConfig;

            // 读取各字段
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

            // 将轴配置添加到哈希表
            m_Axis[axisName] = axisConfig;
        }
    }

    //***************************************************************************************************************************
    // 读取 DI 参数
    if (rootObject.contains("diParams") && rootObject["diParams"].isObject()) {
        QJsonObject diParams = rootObject["diParams"].toObject();
        for (auto it = diParams.begin(); it != diParams.end(); ++it) {
            QString diName = it.key(); // 获取 DI 名称
            if (!it.value().isObject()) {
                continue;
            }

            QJsonObject diObject = it.value().toObject();

            // 初始化当前 DI 的配置
            DI_Config diConfig;

            // 读取各字段
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

            // 将 DI 配置添加到哈希表
            m_DI[diName] = diConfig;
        }
    }

    //***************************************************************************************************************************
    // 读取 DO 参数
    if (rootObject.contains("doParams") && rootObject["doParams"].isObject()) {
        QJsonObject doParams = rootObject["doParams"].toObject();
        for (auto it = doParams.begin(); it != doParams.end(); ++it) {
            QString doName = it.key(); // 获取 DO 名称

            // 确保值是一个 JSON 对象
            if (!it.value().isObject()) {
                qWarning() << "Invalid DO configuration for:" << doName;
                continue; // 跳过无效配置
            }

            QJsonObject doObject = it.value().toObject();

            // 初始化当前 DO 的配置
            DO_Config doConfig;

            // 读取各字段
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

            // 将 DO 配置添加到哈希表
            m_DO[doName] = doConfig;
        }
    }

    //***************************************************************************************************************************
    // 读取 配置 参数
    //if (rootObject.contains("deviceParams") && rootObject["deviceParams"].isObject()) {
    //    QJsonObject params = rootObject["deviceParams"].toObject();
    //    for (auto it = params.begin(); it != params.end(); ++it) {
    //        QString name = it.key(); // 获取 DO 名称

    //        // 确保值是一个 JSON 对象
    //        if (!it.value().isObject()) {
    //            qWarning() << "Invalid DO configuration for:" << name;
    //            continue; // 跳过无效配置
    //        }

    //        QJsonObject object = it.value().toObject();

    //        // 初始化当前 配置 的配置
    //        DeviceMoveit_Config config;

    //        // 读取各字段
    //        if (object.contains("BuffetTime") && object["BuffetTime"].isDouble()) {
    //            config.BuffetTime = object["BuffetTime"].toInt();
    //        }

    //        if (object.contains("DelayTriggerTime") && object["DelayTriggerTime"].isDouble()) {
    //            config.DelayTriggerTime = object["DelayTriggerTime"].toInt();
    //        }

    //        if (object.contains("HoldTime") && object["HoldTime"].isDouble()) {
    //            config.HoldTime = object["HoldTime"].toInt();
    //        }

    //        // 将 DO 配置添加到哈希表
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
    // 读取 工位对应相机 参数
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

//读取SEVON信号 读伺服状态
bool rs_motion::ReadAxisServeOn(int CardId, int AxisId)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        //脉冲卡(低电平使能时)
        return (dmc_read_sevon_pin((WORD)CardId, (WORD)AxisId) == 0);
    }
    else if(LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        //总线卡
        WORD state = 0;
        short rtn = nmc_get_axis_state_machine((WORD)CardId, (WORD)AxisId,&state);
        if (rtn != 0)
        {
            machineLog->write("读伺服状态 nmc_get_axis_state_machine rtn = " + QString::number(rtn), Machine);
        }
        return (state == ENUM_ETHERCAT_STATE::OP_ENABLE);
    }
}

//输出SEVON信号 开伺服
short rs_motion::WriteAxisServeOn(QString axis, bool isEnable)
{
    return WriteAxisServeOn((WORD)m_Axis[axis].card, (WORD)m_Axis[axis].axisNum, isEnable);
}

//输出SEVON信号
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
                machineLog->write("使能伺服 nmc_set_axis_enable rtn = " + QString::number(rtn), Machine);
            }
        }
        else
        {
            rtn = nmc_set_axis_disable((WORD)CardId, (WORD)AxisId);
            if (rtn != 0)
            {
                machineLog->write("使能伺服 nmc_set_axis_disable rtn = " + QString::number(rtn), Machine);
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
    dmc_set_profile(card, axis, StartVel, RunVel, AccTime, DccTime, StopVel);//设置速度参数
    dmc_set_s_profile(card, axis, 0, STime);                                    //设置S段速度参数
    // LTDMC.dmc_set_home_pin_logic(card, axis, 0, 0);//设置原点低电平有效
}

//原来运动控制软件就写了两个 WriteAxis，也重载一下
void rs_motion::WriteAxis(QString AxisName)
{
    // 获取轴的配置
    Axis_Config axis = m_Axis[AxisName];
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        // 设置速度参数
        dmc_set_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed * axis.unit,  // 起始速度（替换为配置中的 minSpeed）
            axis.minSpeed * axis.unit,  // 运行速度（替换为配置中的 maxSpeed）
            axis.accTime,               // 加速时间（替换为配置中的 accTime）
            axis.decTime,               // 减速时间（替换为配置中的 decTime）
            axis.minSpeed * axis.unit   // 停止速度（替换为配置中的 minSpeed）
        );

        // 设置 S 段速度参数
        dmc_set_s_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0,               // S 段曲线类型（写死为 0）
            axis.sTime       // S 段时间（替换为配置中的 sTime）
        );

        // 设置原点有效电平
        dmc_set_home_pin_logic(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0, // 0低电平有效 1高电平有效
            0  // 保留参数（写死为 0）
        );

        int limitBit = 0;
        if (LSM->m_version == EdPin)
        {
            limitBit = 1;
            if (AxisName == LS_AxisName::U || AxisName == LS_AxisName::U1)
                limitBit = 0;

            ////用于绝对值编码器记录位置
            //dmc_set_home_position((ushort)axis.card, (ushort)axis.axisNum,1, 0);
            //if (rtn != 0)
            //{
            //    machineLog->write(AxisName + " dmc_set_home_position rtn = " + QString::number(rtn), Normal);
            //}
        }
        // 设置正负限位有效电平
        dmc_set_el_mode(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            1, // 是否允许正负硬限位（写死为允许）
            limitBit, // 硬限位有效电平：0低电平有效（写死为低电平有效）
            1  // 制动方式：正负限位减速停止
        );

        //// 映射轴的急停信号
        //dmc_set_axis_io_map(
        //    (ushort)axis.card,
        //    (ushort)axis.axisNum,
        //    3,                // 信号类型：轴急停信号（写死为 3）
        //    6,                // 轴 IO 映射类型：通用输入端口（写死为 6）
        //    (ushort)axis.stopIO,  // 急停信号对应的 IO 索引
        //    0.01             // 滤波时间（写死为 0.01）
        //);

        //if (LSM->m_version == EdPin)
        //{
        //    // 设置急停模式
        //    dmc_set_emg_mode(
        //        (ushort)axis.card,
        //        (ushort)axis.axisNum,
        //        1, // 是否允许使用急停信号（写死为启用）
        //        1  // 有效电平：0低电平有效 1高电平有效（写死为高电平有效）
        //    );
        //}
        //else
        //{
        //    // 设置急停模式
        //    dmc_set_emg_mode(
        //        (ushort)axis.card,
        //        (ushort)axis.axisNum,
        //        1, // 是否允许使用急停信号（写死为启用）
        //        RS2::IOStateEnum::High  // 有效电平：0低电平有效 1高电平有效（写死为高电平有效）
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
            //全部加上掉电保持
            rtn = nmc_set_offset_pos((ushort)axis.card, (ushort)axis.axisNum, 0);
            if (rtn != 0)
            {
                machineLog->write("u nmc_set_offset_pos rtn = " + QString::number(rtn), Machine);
            }
        }
        //设置指定轴的脉冲当量
        rtn = dmc_set_equiv(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.unit   
            );
        if (rtn != 0)
        {
            machineLog->write("设置指定轴的脉冲当量 dmc_set_equiv rtn = " + QString::number(rtn), Machine);
        }
        // 设置速度参数
        rtn = dmc_set_profile_unit(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed,  // 起始速度（替换为配置中的 minSpeed）
            axis.maxSpeed,  // 运行速度（替换为配置中的 maxSpeed）
            axis.accTime,               // 加速时间（替换为配置中的 accTime）
            axis.decTime,               // 减速时间（替换为配置中的 decTime）
            axis.minSpeed   // 停止速度（替换为配置中的 minSpeed）
        );
        if (rtn != 0)
        {
            machineLog->write("设置速度参数 dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
        }

        // 设置 S 段速度参数
        rtn = dmc_set_s_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            0,               // S 段曲线类型（写死为 0）
            axis.sTime       // S 段时间（替换为配置中的 sTime）
        );
        if (rtn != 0)
        {
            machineLog->write("设置 S 段速度参数 dmc_set_s_profile rtn = " + QString::number(rtn), Machine);
        }

        //设置回原点参数
        //回零模式默认反向
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
        //    0   //回零偏移先默认0
        //);
        //if (rtn != 0)
        //{
        //    machineLog->write("设置回原点参数 nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
        //}

        // 映射轴的急停信号
        rtn = dmc_set_axis_io_map(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            3,                // 信号类型：轴急停信号（写死为 3）
            6,                // 轴 IO 映射类型：通用输入端口（写死为 6）
            (ushort)axis.stopIO,  // 急停信号对应的 IO 索引
            0.01             // 滤波时间（写死为 0.01）
        );
        if (rtn != 0)
        {
            machineLog->write("映射轴的急停信号 dmc_set_axis_io_map rtn = " + QString::number(rtn), Machine);
        }

        // 设置急停模式
        rtn = dmc_set_emg_mode(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            1, // 是否允许使用急停信号（写死为启用）
            RS2::IOStateEnum::High  // 有效电平：0低电平有效 1高电平有效（写死为高电平有效）
        );
        if (rtn != 0)
        {
            machineLog->write("设置急停模式 dmc_set_emg_mode rtn = " + QString::number(rtn), Machine);
        }
    }
}

//写轴参数
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
    //machineLog->write(Axis + " 轴配置成功", Normal);
}

//急停IO配置
void rs_motion::MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel)
{
    short rtn = 0;
    //映射轴的急停信号
    rtn = dmc_set_axis_io_map((ushort)CardId, (ushort)AxisId,
        3,//信号类型：轴急停信号
        6,//轴IO映射类型:通用输入端口
        (ushort)IOindex,//IO索引
        0.01);//滤波时间
    if (rtn != 0)
    {
        machineLog->write("MapEmgIO 映射轴的急停信号 dmc_set_axis_io_map rtn = " + QString::number(rtn), Machine);
    }
    rtn = dmc_set_emg_mode((ushort)CardId, (ushort)AxisId,
        1,//允许使用急停信号
        validlevel);//有效电平
    if (rtn != 0)
    {
        machineLog->write("MapEmgIO 设置急停模式 dmc_set_emg_mode rtn = " + QString::number(rtn), Machine);
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

//获取更新所有di信号
void rs_motion::getAllDI()
{
    //监控DI信号
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
            //脉冲卡扩展模式
            ushort value = 0;
            nmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.can, (ushort)diInfo.channel, &value);
            diInfo.state = value == 0 ? 1 : 0;
            //diInfo.state = value;
        }
        else
        {

            //脉冲本地卡/总线卡
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

//获取单个DI
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
        //脉冲卡扩展模式
        ushort value = 0;
        nmc_read_inbit((ushort)diInfo.card, (ushort)diInfo.can, (ushort)diInfo.channel, &value);
        //diInfo.state = value;
        diInfo.state = value == 0 ? 1 : 0;
    }
    else
    {
        //脉冲本地卡/总线卡
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


//获取更新所有do信号
void rs_motion::getAllDO()
{
    //监控DO信号
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
            //脉冲卡扩展模式
            ushort value = 0;
            nmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.can, (ushort)doInfo.channel, &value);
            //doInfo.state = value;
            //高低电平转换
            doInfo.state = doInfo.level ? value : (1 - value);
        }
        else
        {
            //脉冲本地卡/总线卡
            short value = 0;
            value = dmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.channel);
            //如果报错，不改状态
            if (value != 0 && value != 1)
                return;
            //doInfo.state = value;
            //高低电平转换
            doInfo.state = doInfo.level ? value : (1 - value);

        }
    }
}

//获取单个DO
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
        //脉冲卡扩展模式
        ushort value = 0;
        nmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.can, (ushort)doInfo.channel, &value);
        //doInfo.state = value;
        //高低电平转换
        doInfo.state = doInfo.level ? value : (1 - value);

    }
    else
    {
        //脉冲本地卡/总线卡
        short value = 0;
        value = dmc_read_outbit((ushort)doInfo.card, (ushort)doInfo.channel);
        //如果报错，不改状态
        if (value != 0 && value != 1)
            return doInfo.state;
        //doInfo.state = value;
        //高低电平转换
        doInfo.state = doInfo.level ? value : (1 - value);

    }
    return doInfo.state;
}

//写入单个DO value是有效还是无效的意思
void rs_motion::setDO(QString name, int value)
{
    // 检查DO是否存在
    if (m_DO.find(name) == m_DO.end()) {
        return;
    }

    auto& doInfo = m_DO[name];
    const bool isTrigger = (value == 1);
    doInfo.state = value;
    //高电平使用输入值，低电平取反
    const int outputLevel = isTrigger ? doInfo.level : (1 - doInfo.level);

    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_out_bit(doInfo.channel, outputLevel);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse && doInfo.can > 0) {
        // 脉冲卡扩展模式，使用nmc_write_outbit
        nmc_write_outbit(
            static_cast<ushort>(doInfo.card),
            static_cast<ushort>(doInfo.can),
            static_cast<ushort>(doInfo.channel),
            outputLevel
        );
    }
    else {
        // 本地卡/总线卡统一处理，使用dmc_write_outbit
        dmc_write_outbit(
            static_cast<ushort>(doInfo.card),
            static_cast<ushort>(doInfo.channel),
            outputLevel
        );
    }

    
   
    //machineLog->write(QString::fromStdString("do name : ") + name + QString::fromStdString(" = ") + QString::number(value), Normal);
}

//关闭板卡
void rs_motion::motionClose()
{
    ////m_isLoop = false;
    ////m_isPause = false;
    ////m_isEmergency = false;
    for(auto axis : LS_AxisName::allAxis)
    {
       AxisStop(axis); //停止所有轴
       //WriteAxisServeOn(axis, false);   //下使能
    }

    //断连接
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

//轴停止
void rs_motion::AxisStop(QString axis)
{
    // 检查轴名是否存在
    if (m_Axis.find(axis) == m_Axis.end()) {
        return;
    }
    AxisStop(m_Axis[axis].card, m_Axis[axis].axisNum);
}

//轴停止
void rs_motion::AxisStop(int CardId, int AxisId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        d1000_decel_stop(AxisId);
    }
    else {
        dmc_stop((ushort)CardId, (ushort)AxisId, 0);// 0:减速停止，1：紧急停止
    }

}

//所有轴急停
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

//关闭CAN卡
void rs_motion::CANClose(int CardId)
{
    ushort usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    dmc_get_can_state(usCardNum, & usCanNum, & usCanStatus);

    //usCanNum 范围 1-8
    for (ushort i = usCanNum; i < usCanNum + 1; i++)
    {
        nmc_set_connect_state(usCardNum, i, 0, 0);//设置CAN通讯状态，断开 
    }
}

//轴名回零
int rs_motion::AxisHome(QString axis)
{
    short rtn = 0;
    // 检查轴名是否存在
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    if (m_version == TwoPin)
    {
        // 设置回原模式
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // 回零方向：0负向 1正向（替换为配置中的 homeDir）
            0,                      // 回零速度模式：0低速 1高速（写死为低速）
            1,                      // 回零模式（写死为模式 1）
            2                       // 保留参数（写死为 2）
        );

        // 设置回零到限位是否反找
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0不反找 1反找（写死为启用反找）
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHome", Global);
        ////清零位置 和编码器
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        //machineLog->write(axis + " is AxisClearPosition", Global);
    }
    else if (m_version == LsPin)
    {

        if (axis == LS_AxisName::LS_U)
        {           
            //旋转轴用不同的回零方式
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(" U轴回原点速度设置错误", Normal);
            rtn = PMove_abs(axis, 0);
            if (rtn != 0)
                machineLog->write(" U轴回原点错误", Normal);
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
                0   //回零偏移先默认0
            );
            if (rtn != 0)
            {
                machineLog->write("设置回原点参数 nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
            }

            rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
            machineLog->write(axis + " is AxisHome", Global);
        }
        //清零位置 和编码器
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisClearPosition", Global);
    }
    else if (m_version == EdPin)
    {
        // 设置回原模式
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // 回零方向：0负向 1正向（替换为配置中的 homeDir）
            0,                      // 回零速度模式：0低速 1高速（写死为低速）
            1,                      // 回零模式（写死为模式 1）
            2                       // 保留参数（写死为 2）
        );

        // 设置回零到限位是否反找
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0不反找 1反找（写死为启用反找）
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        if (rtn != 0)
        {
            machineLog->write(axis + " 回原点错误", Normal);
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
        //-------------------------- 后面的项目，如果是用ETherCat的，直接全部先用雷赛软件定原点，然后软件用走位置到0点 ------------------------------
        switch (m_motionVersion)
        {
        case LS_EtherCat:
        {
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(axis + " 轴回原点速度设置错误", Normal);
            rtn = PMove_abs(axis, 0);
            if (rtn != 0)
                machineLog->write(axis + " 轴回原点错误", Normal);
            break;
        }
        default:
            break;
        }
    }

    return rtn;
}

//轴名回零(阻塞)
int rs_motion::AxisHomeAsync(QString axis)
{
    short rtn = 0;
    // 检查轴名是否存在
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    if (LSM->m_version == TwoPin)
    {
        // 设置回原模式
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // 回零方向：0负向 1正向（替换为配置中的 homeDir）
            0,                      // 回零速度模式：0低速 1高速（写死为低速）
            1,                      // 回零模式（写死为模式 1）
            2                       // 保留参数（写死为 2）
        );

        // 设置回零到限位是否反找
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0不反找 1反找（写死为启用反找）
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync", Global);
        ///等待轴运动完成
        while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum)) {
            if (m_isEmergency.load())
                return -1;
            machineLog->write(axis + " is AxisHomeAsync IsRuning", Global);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        };
        //清零位置 和编码器
        AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync AxisClearPosition", Global);
    }
    else if (LSM->m_version == LsPin)
    {

        if (axis == LS_AxisName::LS_U)
        {
            //旋转轴用不同的回零方式
            rtn = setSpeed(axis, m_Axis[axis].minSpeed);
            if (rtn != 0)
                machineLog->write(" U轴回原点速度设置错误", Normal);
            rtn = PMove_abs(axis, 0);
            if(rtn != 0 )
                machineLog->write(" U轴回原点错误", Normal);
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
                0   //回零偏移先默认0
            );
            if (rtn != 0)
            {
                machineLog->write("设置回原点参数 nmc_set_home_profile rtn = " + QString::number(rtn), Machine);
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
        //清零位置 和编码器
        //AxisClearPosition(m_Axis[axis].card, m_Axis[axis].axisNum);
        machineLog->write(axis + " is AxisHomeAsync AxisClearPosition", Global);
        
    }
    else if (LSM->m_version == EdPin)
    {
        // 设置回原模式
        dmc_set_homemode(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            m_Axis[axis].homeDir == 1 ? (ushort)1 : (ushort)0,  // 回零方向：0负向 1正向（替换为配置中的 homeDir）
            0,                      // 回零速度模式：0低速 1高速（写死为低速）
            1,                      // 回零模式（写死为模式 1）
            2                       // 保留参数（写死为 2）
        );

        // 设置回零到限位是否反找
        dmc_set_home_el_return(
            (ushort)m_Axis[axis].card,
            (ushort)m_Axis[axis].axisNum,
            1 // 0不反找 1反找（写死为启用反找）
        );

        rtn = GoHome(m_Axis[axis].card, m_Axis[axis].axisNum);
        if (rtn != 0)
        {
            machineLog->write(axis + " 回原点错误", Normal);
        }
        //machineLog->write(axis + " is AxisHomeAsync", Normal);

        ///等待轴运动完成
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
	    //-------------------------- 后面的项目，如果是用ETherCat的，直接全部先用雷赛软件定原点，然后软件用走位置到0点 ------------------------------
	    switch (m_motionVersion)
	    {
	    case LS_EtherCat:
	    {
	    	rtn = setSpeed(axis, m_Axis[axis].minSpeed);
	    	if (rtn != 0)
	    		machineLog->write(axis + " 轴回原点速度设置错误", Normal);
	    	rtn = PMove_abs(axis, 0);
	    	if (rtn != 0)
	    		machineLog->write(axis + " 轴回原点错误", Normal);
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

//参数回零
int rs_motion::GoHome(int CardId, int AxisId)
{
    short rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = dmc_home_move((ushort)CardId, (ushort)AxisId);
        if (rtn != 0)
            machineLog->write("参数回零 dmc_home_move rtn = " + QString::number(rtn), Normal);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = nmc_home_move((ushort)CardId, (ushort)AxisId);
        if(rtn != 0)
            machineLog->write("轴 "+ QString::number(AxisId) + "参数回零 nmc_home_move rtn = " + QString::number(rtn), Normal);
    }
    return rtn;
}

//清零位置 和编码器
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

// 检查指定轴的运动状态，返回 true 表示正在运行，false 表示已停止
bool rs_motion::IsRuning(int CardId, int AxisId)
{
    if (m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        return (d1000_check_done(AxisId) == 0);
    else
        return (dmc_check_done((ushort)CardId, (ushort)AxisId) == 0);
}

//停止所有轴
void rs_motion::AxisStopAll()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis);
    }
}

//定速运动 0:负方向，1：正方向
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

//定速运动 0:负方向，1：正方向
int rs_motion::AxisVmove(QString axis, bool postive , double speed)
{
    // 检查轴名是否存在
    if (m_Axis.find(axis) == m_Axis.end()) {
        return -1;
    }
    int rtn = 0;
    if(m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
        rtn = VMove(m_Axis[axis].card, m_Axis[axis].axisNum, postive, speed * m_Axis[axis].unit);
    else
        rtn = VMove(m_Axis[axis].card, m_Axis[axis].axisNum, postive);
    if (rtn != 0)
        machineLog->write(axis + " 点动错误，错误码 = " + QString::number(rtn), Normal);
    return rtn;
}

// 实时设置轴速度(在线变速)
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
            machineLog->write("在线变速 dmc_change_speed_unit rtn = " + QString::number(rtn), Machine);
    }
}

// 配置轴速度(在线变速)
void rs_motion::WriteAxisRunVel(QString axis, double runvel)
{
    // 检查轴名是否存在
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

//读脉冲/编码器位置
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
        //命令位置
        //rtn = dmc_get_position_unit((ushort)CardId, (ushort)AxisId, &pos);
        //编码器位置
        rtn = dmc_get_encoder_unit((ushort)CardId, (ushort)AxisId, &pos);
        if (rtn != 0)
            machineLog->write("读脉冲/命令位置 dmc_get_position_unit rtn = " + QString::number(rtn), Machine);
        return pos;
    }
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        return (d1000_get_command_pos(AxisId));
        //return (dmc_get_encoder((ushort)CardId, (ushort)AxisId));

    }
    
}

//读指定轴位置
double rs_motion::ReadAxisP(QString axis)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum) / m_Axis[axis].unit);
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        //总线型返回的就是命令位置，不用除当量
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum));
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse_1000)
    {
        return (ReadPosition(m_Axis[axis].card, m_Axis[axis].axisNum) / m_Axis[axis].unit);
    }
}

//读所有轴位置
void rs_motion::ReadAllAxisP()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        m_Axis[axis].position = std::round(ReadAxisP(axis) * 100.0) / 100;
    }
}

// 配置轴的速度模式(在线变速)
void rs_motion::WriteAxisSpeedModel(QString axis, SpeedModeEnum model)
{
    // 检查轴名是否存在
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
    //machineLog->write("速度模式 ：" + QString::number(setspeed), Normal);
}

//读轴伺服开关状态
bool rs_motion::ReadServOn(QString axis)
{
   bool res = ReadAxisServeOn(m_Axis[axis].card, m_Axis[axis].axisNum);
   return res;
}

// 恢复配置轴速度 内部先下使能后上使能
void rs_motion::ResetSetAxisParm(QString axis)
{
    auto SERON = ReadServOn(axis);
    //machineLog->write(axis + " ResetSetAxisParm SERON = " + QString::number(SERON), Normal);
    if (SERON)
        WriteAxisServeOn(axis, false);
    //直接所有参数重新设吧
    WriteAxis(axis);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (SERON) 
        WriteAxisServeOn(axis, true);
}

//设置速度
int rs_motion::setSpeed(QString axisName, double speed)
{
    short rtn = 0;
    const auto& axis = m_Axis[axisName];
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        // 设置速度参数
        rtn = dmc_set_profile(
            (ushort)axis.card,
            (ushort)axis.axisNum,
            axis.minSpeed * axis.unit,  // 起始速度（替换为配置中的 minSpeed）
            speed * axis.unit,  // 运行速度（替换为配置中的 maxSpeed）
            axis.accTime,               // 加速时间（替换为配置中的 accTime）
            axis.decTime,               // 减速时间（替换为配置中的 decTime）
            axis.minSpeed * axis.unit   // 停止速度（替换为配置中的 minSpeed）
        );
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        // 设置速度参数
        rtn = dmc_set_profile_unit(
        (ushort)axis.card,
        (ushort)axis.axisNum,
        axis.minSpeed,  // 起始速度（替换为配置中的 minSpeed）
        speed,  // 运行速度（替换为配置中的 maxSpeed）
        axis.accTime,               // 加速时间（替换为配置中的 accTime）
        axis.decTime,               // 减速时间（替换为配置中的 decTime）
        axis.minSpeed   // 停止速度（替换为配置中的 minSpeed）
        );
    }
    return rtn;
}

// 读取轴是否报警状态
bool rs_motion::isAxisErc(QString axis) {
    bool res = isAxisErc(m_Axis[axis].card, m_Axis[axis].axisNum);
    return res;
}

// 读取轴报警端口电平
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
            machineLog->write("读取轴报警 nmc_get_axis_errcode rtn = " + QString::number(rtn), Machine);
        }
        if (state != 0)
        {
            //报警
            res = true;
        }
    }
    return res;
}

// 清除轴报警状态
void rs_motion::AxisClear(QString axis)
{
    AxisClear(m_Axis[axis].card, m_Axis[axis].axisNum);
}

// 清除轴报警状态
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
        //清除总线报警
        nmc_get_errcode((ushort)CardId, 2, &errorcode);
        if (errorcode != 0)
        {
            nmc_clear_errcode((ushort)CardId ,2);
        }
        ushort errorcodeAxis = 0;
        //清除轴报警
        nmc_get_axis_errcode((ushort)CardId, (ushort)AxisId, &errorcodeAxis);
        if (errorcodeAxis != 0)
        {
            nmc_clear_axis_errcode((ushort)CardId, (ushort)AxisId);
        }
    }
}

// 定长绝对运动
int rs_motion::PMove_abs(int CardId, int AxisId, double value)
{
    int rtn = 0;
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        rtn = dmc_pmove((ushort)CardId, (ushort)AxisId, long(value), 1); //0：相对坐标模式，1：绝对坐标模式
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        rtn = dmc_pmove_unit((ushort)CardId, (ushort)AxisId, value, 1);
    }
   
   return rtn;
}

// 定长绝对运动 （轴）
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
        rtn = d1000_set_pls_outmode(config.axisNum, 0);//pulse/dir 模式，脉冲上升沿有效
        rtn = d1000_start_s_move(config.axisNum, value * config.unit, speed * config.unit, speed * config.unit, 0.1);
    }

    return rtn;
}

// 定长相对运动
void rs_motion::PMove_rel(int CardId, int AxisId, double value)
{
    if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_Pulse)
    {
        dmc_pmove((ushort)CardId, (ushort)AxisId, long(value), 0); //0：相对坐标模式，1：绝对坐标模式
    }
    else if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
    {
        dmc_pmove_unit((ushort)CardId, (ushort)AxisId, value, 0);
    }
}

// 定长相对运动 （轴）
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

// 轴定长绝对运动，阻塞执行，待轴到位后返回
int rs_motion::AxisPmoveAsync(QString axis, double value, double speed)
{
    //int dis = (int)(value * m_Axis[axis].unit);
    //machineLog->write(axis + " value = " + QString::number(value) + " unit = " + QString::number(m_Axis[axis].unit) + " dis = "+ QString::number(dis), Normal);
    int rtn = PMove_abs(axis, value,speed);
    if (rtn != 0)
        return rtn;
    ///等待轴运动完成
    while (IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum))
    {
        if(m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return 0;
}

// 等待轴复位到初始位置后返回(阻塞)
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

// 轴定长绝对运动
int rs_motion::AxisPmove(QString axis, double value, double speed)
{
    int rtn = PMove_abs(axis, value, speed);
    return rtn;
}

// 轴复位到初始位置
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

//  复位 进入待料状态
int rs_motion::TaskReset()
{
    //先清除所有轴报警 
    for (auto axis : LS_AxisName::allAxis)
    {
        //清除报警
        AxisClear(axis);
        //设置为复位速度
        ResetSetAxisParm(axis);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        WriteAxisServeOn(axis, true);
    }

    //气缸先加紧位
    setDO(LS_DO::DO_ClampCylinderL1, 1);
    setDO(LS_DO::DO_ClampCylinderL2, 1);
    setDO(LS_DO::DO_ClampCylinderR1, 1);
    setDO(LS_DO::DO_ClampCylinderR2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.Z);  //  Z  RZ 等待回零
    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.RZ);

    //  Z  RZ 等待回零
    if (AxisHomeAsync(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHomeAsync(LS_AxisName::RZ) != 0)
        return -1;

    AxisInitP(LS_AxisName::RZ); //同时RZ 进入待料位   X U1 U2 同时回零  

    //std::vector<std::future<int>> futures;

    //// 异步执行每个轴的回零和初始化
    ////U1回零 带料位
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::U1));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::U1));

    ////U2回零 带料为
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::U2));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::U2));

    ////X轴异步 回原 到待料位
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisHomeAsync, this, LS_AxisName::X));
    //futures.push_back(std::async(std::launch::async, &rs_motion::AxisInitPAsync, this, LS_AxisName::X));
    //U1回零
    //250717 放回左工位U1轴
    if (AxisHome(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 回零报错", Normal);
        return -1;
    }
    ////U2回零
    //if (AxisHome(LS_AxisName::U2) != 0)
    //{
    //    machineLog->write("U2 回零报错", Normal);
    //    return -1;
    //}
    //X轴回零
    if (AxisHome(LS_AxisName::X) != 0)
    {
        machineLog->write("X 回零报错", Normal);
        return -1;
    }
    ////等待轴异步到位
    //while (!isHomeP(LS_AxisName::U1)
    //    || !isHomeP(LS_AxisName::U2)
    //    || !isHomeP(LS_AxisName::X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    
    //250717 放回左工位U1轴
    //等待轴异步到位
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


    ////等待轴异步到位
    //while (isRuning(LS_AxisName::X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    //AxisClearPosition(m_Axis[LS_AxisName::X].card, m_Axis[LS_AxisName::X].axisNum);


    //U1到待料位
    if (AxisInitP(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 待料报错", Normal);
        return -1;
    }
    ////U2到待料位
    //if (AxisInitP(LS_AxisName::U2) != 0)
    //{
    //    machineLog->write("U2 待料报错", Normal);
    //    return -1;
    //}
    //X轴到待料位
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X 待料报错", Normal);
        return -1;
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //Y1 Y2轴同时回零
    if (AxisHome(LS_AxisName::Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Y2) != 0)
        return -1;
    ////等待轴异步到位
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

    //回零到位后，同时到初始位 Y1 Y2
    AxisInitP(LS_AxisName::Y1);
    AxisInitP(LS_AxisName::Y2);
    //等待轴异步到位
    while (!isInitP(LS_AxisName::Y1)
        || !isInitP(LS_AxisName::Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    //// 等待所有异步任务完成
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("回零异步任务失败", Normal);
    //        return -1;
    //    }

    //}

    //最后 Z 待料位
    if (AxisInitPAsync(LS_AxisName::Z) != 0)
        return -1;
    //气缸复位
    setDO(LS_DO::DO_ClampCylinderL1, 0);
    setDO(LS_DO::DO_ClampCylinderL2, 0);
    setDO(LS_DO::DO_ClampCylinderR1, 0);
    setDO(LS_DO::DO_ClampCylinderR2, 0);
    //指示灯复位
    ResetStationLight();
    ResetStationLight(false);
    LSM->m_isStart.store(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return 0;
}

//立昇复位
int rs_motion::TaskReset_LS()
{
    //先清除所有轴报警 
    for (auto axis : LS_AxisName::allAxis)
    {
        //清除报警
        AxisClear(axis);
        ////设置为复位速度
        //ResetSetAxisParm(axis);
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        //WriteAxisServeOn(axis, true);
    }
    //松开吸气
    setDO(LS_DO::LS_OUT_ClampSuck, 0);
    closeDO();

    ////气缸先加紧位
    //setDO(LS_DO::DO_ClampCylinderL1, 1);
    //setDO(LS_DO::DO_ClampCylinderL2, 1);
    //setDO(LS_DO::DO_ClampCylinderR1, 1);
    //setDO(LS_DO::DO_ClampCylinderR2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.Z);  //  Z  RZ 等待回零
    //await DriverCtrl.Instance.AxisHomeAsync(ENUMAXIS.RZ);

    //  Z  RZ 等待回零
    if (AxisHomeAsync(LS_AxisName::LS_Z) != 0)
        return -1;

    //Y1 Y2轴同时回零
    if (AxisHome(LS_AxisName::LS_Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::LS_Y2) != 0)
        return -1;
    

    ////回零到位后，同时到初始位 Y1 Y2
    //AxisInitP(LS_AxisName::Y1);
    //AxisInitP(LS_AxisName::Y2);
    ////等待轴异步到位
    //while (!isInitP(LS_AxisName::Y1)
    //    || !isInitP(LS_AxisName::Y2))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    //等待轴异步到位
    while (!isHomeP(LS_AxisName::LS_Y1)
        || !isHomeP(LS_AxisName::LS_Y2))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //U回零
    if (AxisHome(LS_AxisName::LS_U) != 0)
    {
        machineLog->write("U1 回零报错", Normal);
        return -1;
    }
    //if (AxisHomeAsync(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 回零报错", Normal);
    //    return -1;
    //}
    
    //X轴回零
    if (AxisHome(LS_AxisName::LS_X) != 0)
    {
        machineLog->write("X 回零报错", Normal);
        return -1;
    }
    //等待轴异步到位
    while (!isHomeP(LS_AxisName::LS_U) || 
        !isHomeP(LS_AxisName::LS_X))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    //    //等待轴异步到位
    //while (!isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    ////U1到待料位
    //if (AxisInitP(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 待料报错", Normal);
    //    return -1;
    //}
    ////X轴到待料位
    //if (AxisInitP(LS_AxisName::LS_X) != 0)
    //{
    //    machineLog->write("X 待料报错", Normal);
    //    return -1;
    //}


    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    //// 等待所有异步任务完成
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("回零异步任务失败", Normal);
    //        return -1;
    //    }

    //}

    ////最后 Z 待料位
    //if (AxisInitPAsync(LS_AxisName::Z) != 0)
    //    return -1;
    ////气缸复位
    //setDO(LS_DO::DO_ClampCylinderL1, 0);
    //setDO(LS_DO::DO_ClampCylinderL2, 0);
    //setDO(LS_DO::DO_ClampCylinderR1, 0);
    //setDO(LS_DO::DO_ClampCylinderR2, 0);
    //指示灯复位
    //ResetStationLight();
    //ResetStationLight(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return 0;
}

//易鼎丰复位
int rs_motion::TaskReset_ED()
{
    //先清除所有轴报警 
    for (auto axis : LS_AxisName::allAxis)
    {
        //清除报警
        AxisClear(axis);
    }

    closeDO();

    //气缸先加紧位
    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    setDO(LS_DO::ED_DO_PositionCyl2, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //  Z  Z1 Z2 等待回零
    if (AxisHome(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z2) != 0)
        return -1;
    //等待轴异步到位
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

    //X Y U U1轴同时回零
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U1) != 0)
        return -1;

    //等待轴异步到位
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
    ////U回零
    //if (AxisHome(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 回零报错", Normal);
    //    return -1;
    //}
    //if (AxisHomeAsync(LS_AxisName::LS_U) != 0)
    //{
    //    machineLog->write("U1 回零报错", Normal);
    //    return -1;
    //}

    ////X轴回零
    //if (AxisHome(LS_AxisName::LS_X) != 0)
    //{
    //    machineLog->write("X 回零报错", Normal);
    //    return -1;
    //}
    ////等待轴异步到位
    //while (!isHomeP(LS_AxisName::LS_U) ||
    //    !isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    //    //等待轴异步到位
    //while (!isHomeP(LS_AxisName::LS_X))
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}

    setSpeed(LS_AxisName::U, m_Axis[LS_AxisName::U].minSpeed);
    //U到待料位
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U 待料报错", Normal);
        machineLog->write("U 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X轴到待料位
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X 待料报错", Normal);
        machineLog->write("X 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y轴到待料位
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y 待料报错", Normal);
        machineLog->write("Y 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::U1, m_Axis[LS_AxisName::U1].minSpeed);
    //U1到待料位
    if (AxisInitP(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 待料报错", Normal);
        machineLog->write("U1 待料报错", Err);
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
    machineLog->write("到达初始位", Normal);


    //std::this_thread::sleep_for(std::chrono::milliseconds(10));


    //// 等待所有异步任务完成
    //for (auto& future : futures) {
    //    auto rtn = future.get();
    //    if (rtn != 0)
    //    {
    //        machineLog->write("回零异步任务失败", Normal);
    //        return -1;
    //    }

    //}

    ////最后 Z 待料位
    //if (AxisInitPAsync(LS_AxisName::Z) != 0)
    //    return -1;
    ////气缸复位
    //setDO(LS_DO::DO_ClampCylinderL1, 0);
    //setDO(LS_DO::DO_ClampCylinderL2, 0);
    //setDO(LS_DO::DO_ClampCylinderR1, 0);
    //setDO(LS_DO::DO_ClampCylinderR2, 0);
    //指示灯复位
    //ResetStationLight();
    //ResetStationLight(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //松开电磁阀
    setDO(LS_DO::ED_DO_PositionCyl1, 0);
    setDO(LS_DO::ED_DO_PositionCyl2, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 0);
    //清除机器人报错
    MODBUS_TCP.writeIntCoil(100, 3, 0);
    Sleep(500);
    MODBUS_TCP.writeIntCoil(100, 3, 1);
    return 0;
}

//博泰复位
int rs_motion::TaskReset_BT()
{
    ////先清除所有轴报警 
    //for (auto axis : LS_AxisName::allAxis)
    //{
    //    //清除报警
    //    AxisClear(axis);
    //}

    closeDO();

    //X Y1 Y2轴同时回零
    if (AxisHome(LS_AxisName::Y1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Y2) != 0)
        return -1;

    //等待轴异步到位
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
    //X轴到待料位
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X 待料报错", Normal);
        machineLog->write("X 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y1, m_Axis[LS_AxisName::Y1].minSpeed);
    //Y1轴到待料位
    if (AxisInitP(LS_AxisName::Y1) != 0)
    {
        machineLog->write("Y1 待料报错", Normal);
        machineLog->write("Y1 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y2, m_Axis[LS_AxisName::Y2].minSpeed);
    //Y2到待料位
    if (AxisInitP(LS_AxisName::Y2) != 0)
    {
        machineLog->write("Y2 待料报错", Normal);
        machineLog->write("Y2 待料报错", Err);
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
    machineLog->write("到达初始位", Normal);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //松开电磁阀
    return 0;
}

//金脉Pin复位
int rs_motion::TaskReset_JMPin()
{
    //先清除所有轴报警 
    for (auto axis : LS_AxisName::allAxis)
    {
        //清除报警
        AxisClear(axis);
    }


    //  Z  Z1 Z2 等待回零
    if (AxisHome(LS_AxisName::Z) != 0)
        return -1;
    if (AxisHome(LS_AxisName::Z1) != 0)
        return -1;

    //等待轴异步到位
    while (isRuning(LS_AxisName::Z)
        || isRuning(LS_AxisName::Z1))
    {
        if (m_isEmergency.load())
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    AxisClearPosition(m_Axis[LS_AxisName::Z].card, m_Axis[LS_AxisName::Z].axisNum);
    AxisClearPosition(m_Axis[LS_AxisName::Z1].card, m_Axis[LS_AxisName::Z1].axisNum);

    //X Y U轴同时回零
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X1) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;

    //等待轴异步到位
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
    //U1回零
    if (AxisHomeAsync(LS_AxisName::U1) != 0)
    {
        machineLog->write("U1 回零报错", Normal);
        return -1;
    }
    setSpeed(LS_AxisName::U, m_Axis[LS_AxisName::U].minSpeed);
    //U到待料位
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U 待料报错", Normal);
        machineLog->write("U 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X轴到待料位
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X 待料报错", Normal);
        machineLog->write("X 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y轴到待料位
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y 待料报错", Normal);
        machineLog->write("Y 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X1, m_Axis[LS_AxisName::X1].minSpeed);
    //X1轴到待料位
    if (AxisInitP(LS_AxisName::X1) != 0)
    {
        machineLog->write("X1 待料报错", Normal);
        machineLog->write("X1 待料报错", Err);
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
    machineLog->write("到达初始位", Normal);

    return 0;
}

//金脉焊缝复位
int rs_motion::TaskReset_JMHan()
{
    //先清除所有轴报警 
    for (auto axis : LS_AxisName::allAxis)
    {
        //清除报警
        AxisClear(axis);
    }

    //  Z 等待回零
    if (AxisHomeAsync(LS_AxisName::Z) != 0)
    {
        machineLog->write("Z 回零报错", Normal);
        return -1;
    }
    AxisClearPosition(m_Axis[LS_AxisName::Z].card, m_Axis[LS_AxisName::Z].axisNum);

    //X Y U轴同时回零
    if (AxisHome(LS_AxisName::Y) != 0)
        return -1;
    if (AxisHome(LS_AxisName::X) != 0)
        return -1;
    if (AxisHome(LS_AxisName::U) != 0)
        return -1;

    //等待轴异步到位
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
    //U到待料位
    if (AxisInitP(LS_AxisName::U) != 0)
    {
        machineLog->write("U 待料报错", Normal);
        machineLog->write("U 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::X, m_Axis[LS_AxisName::X].minSpeed);
    //X轴到待料位
    if (AxisInitP(LS_AxisName::X) != 0)
    {
        machineLog->write("X 待料报错", Normal);
        machineLog->write("X 待料报错", Err);
        return -1;
    }
    setSpeed(LS_AxisName::Y, m_Axis[LS_AxisName::Y].minSpeed);
    //Y轴到待料位
    if (AxisInitP(LS_AxisName::Y) != 0)
    {
        machineLog->write("Y 待料报错", Normal);
        machineLog->write("Y 待料报错", Err);
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
    machineLog->write("到达初始位", Normal);

    return 0;
}


// 复位工位指示灯（左工位 : true，右工位 ：flase）
void rs_motion::ResetStationLight(bool isstationL)
{
    //暂时不知道 Modbus 的用处，先屏蔽
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

// 进入待料状态
int rs_motion::TaskGoWating(ENUMSTATION stations)
{
    ////在线复位速度 
    //Enum.GetValues<ENUMAXIS>().ToList().ForEach(AX = >
    //{
    //    DriverCtrl.Instance.WriteAxisSpeedModel(AX, SpeedModeEnum.Hight);
    //});

    //在线复位速度 
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

    //气缸先加紧位 
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

    //U Y 气缸 按工位进入待料位
    switch (stations)
    {
    case ENUMSTATION::L:
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.U1);
        //await DriverCtrl.Instance.AxisInitPAsync(ENUMAXIS.Y1);
        if (AxisInitPAsync(LS_AxisName::U1) != 0)
            return -1;
        if (AxisInitPAsync(LS_AxisName::Y1) != 0)
            return -1;
        //气缸复位
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
        //气缸复位
        setDO(LS_DO::DO_ClampCylinderR1, 0);
        setDO(LS_DO::DO_ClampCylinderR2, 0);
        break;
    case ENUMSTATION::Both:
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.U1);
        //DriverCtrl.Instance.AxisInitP(ENUMAXIS.U2);

        AxisInitP(LS_AxisName::U1);
        AxisInitP(LS_AxisName::U2);
        //等待轴异步到位
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
        //等待轴异步到位
        while (!isInitP(LS_AxisName::Y1)
            || !isInitP(LS_AxisName::Y2))
        {
            if (m_isEmergency.load())
                return -1;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        //气缸复位
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

//是否停在初始位置
bool rs_motion::isInitP(QString axis)
{
    auto res1 = false;
    if (fabs(ReadAxisP(axis) - m_Axis[axis].initPos) < ERROR_VALUE)
        res1 = true;
    auto res2 = !isRuning(axis);
    return (res1 && res2);
}

//轴是否在运动中
bool rs_motion::isRuning(QString axis)
{
    return IsRuning(m_Axis[axis].card, m_Axis[axis].axisNum);
}

//是否到达原点
bool rs_motion::isHomeP(QString axis)
{
    auto res1 = false;
    if (fabs(ReadAxisP(axis) - 0) < ERROR_VALUE)
        res1 = true;
    auto res2 = isRuning(axis);
    return (res1 && !res2);
}

//读取配方参数
void rs_motion::loadFormulaFromFile()
{
    // 获取Formula_Config目录路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        return;  // 如果目录不存在则返回
    }

    // 获取文件夹中所有文件
    dir.setFilter(QDir::Files);
    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        if (fileInfo.suffix() == "json") {
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray fileData = file.readAll();
                file.close();

                // 解析 JSON 数据
                QJsonDocument doc = QJsonDocument::fromJson(fileData);
                if (doc.isObject()) {
                    QJsonObject formulaJson = doc.object();
                    Formula_Config formulaConfig;

                    // 解析 ListProcess_L
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
                            //易鼎丰轴参数
                            point.TargetPAxis_1 = pointJson["TargetPAxis_1"].toDouble();
                            point.TargetPAxis_2 = pointJson["TargetPAxis_2"].toDouble();
                            point.TargetPAxis_3 = pointJson["TargetPAxis_3"].toDouble();
                            point.TargetPAxis_4 = pointJson["TargetPAxis_4"].toDouble();
                            point.TargetPAxis_5 = pointJson["TargetPAxis_5"].toDouble();
                            point.TargetPAxis_6 = pointJson["TargetPAxis_6"].toDouble();
                            point.TargetPAxis_7 = pointJson["TargetPAxis_7"].toDouble();
                            break;
                        case ENUM_VERSION::LsPin: 
                            //立昇轴参数
                            point.TargetPAxis_1 = pointJson["TargetPAxis_1"].toDouble();
                            point.TargetPAxis_2 = pointJson["TargetPAxis_2"].toDouble();
                            point.TargetPAxis_3 = pointJson["TargetPAxis_3"].toDouble();
                            point.TargetPAxis_4 = pointJson["TargetPAxis_4"].toDouble();
                            point.TargetPAxis_5 = pointJson["TargetPAxis_5"].toDouble();
                            break;
                        case ENUM_VERSION::BtPin:
                            //博泰轴参数
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
                        // 双工位版本
                        // 解析 ListProcess_R
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
                    // 将配方信息保存到 m_Formula 中
                    QString formulaName = fileInfo.baseName();  // 文件名即为配方名称
                    m_Formula[formulaName] = formulaConfig;
                }
            }
        }
    }
}

// 保存流程配方
void rs_motion::saveFormulaToFile()
{
    // 获取Formula_Config目录路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (const auto& pair :m_Formula) {
        QString formulaName = pair.first;  // 获取配方名称
        rs_motion::Formula_Config formulaConfig = pair.second;  // 获取配方数据

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
                //易鼎丰轴参数
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                pointJson["TargetPAxis_6"] = point.TargetPAxis_6;
                pointJson["TargetPAxis_7"] = point.TargetPAxis_7;
                break;
            case ENUM_VERSION::LsPin: 
                //立昇轴参数
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                break;
            case ENUM_VERSION::BtPin:
                //博泰轴参数
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
            // 双工位标机
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

        // 将 QJsonObject 转换为 QJsonDocument
        QJsonDocument doc(formulaJson);

        // 保存 JSON 数据到文件
        QString filePath = dir.filePath(formulaName + ".json");
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }
    }
}

// 整体动作运行流程
int rs_motion::runAction(QList<PointItemConfig> listProcess)
{

#ifdef DEBUG_LHC
    //初始化变量
    initAllVariable();
    barCodeIdentify(listProcess[0]);
    QrCodeIdentify(listProcess[1]);
    templateIdentify(listProcess[2]);

#else
    //初始化变量
    initAllVariable();
    bool showPhoto = false;

    //流程开始时间
    m_runStartTime = QDate::currentDate().toString("yyyyMMdd_") + QTime::currentTime().toString("hh-mm-ss");
    if (m_version != LsPin)
    {
        //清除图片
        emit clearWindow_signal();
    }
    if (true)
    {
        //构建线程作用域，用于自动析构
        std::vector<std::thread> threads;  //存储线程的容器
        ThreadGuard guard(threads);  // 守卫对象在此作用域内管理线程,防止报错return线程崩溃
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
                    //如果没有型号，写配方方案名
                    item.ModelName = m_forMulaName;
                }
                QString exePath = QCoreApplication::applicationDirPath();
                QString filePath = "/images/Result/";

                item.ModelName.replace('*', '@');         // 替换所有*为@
                m_resultImgPath = exePath + filePath + item.ModelName + "/" + m_runStartTime;
                //原图路径
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

            //0611测试
            //前置流程
#if LOCAL_TEST == 0
            res = ItemPreProcess(item);
            if (res != 0)
                return res;

            //动作流程
            res = ItemMovingProcess(item);
            if (res != 0)
                return res;

            //后置流程
            res = ItemLastProcess(item);
            if (res != 0)
                return res;
#endif

            if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Point2D)
            {
                //2D识别
#if LOCAL_TEST == 0
                auto& camera = m_cameraMap[item.CameraName];
                if (camera == nullptr)
                {
                    machineLog->write("PIN检测相机设备为空。。", Normal);
                    continue;
                }
                if (!camera->m_outImg.IsInitialized())
                {
                    machineLog->write("2D相片为空。。", Normal);
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
                //3d识别
                if (m_cloud_ori_map.empty())
                {
                    machineLog->write("已扫描点云为空。。", Normal);
                    continue;
                }
                if (!m_cloud_ori_map.count(item.CloudName))
                {
                    machineLog->write("未找到点云 ：" + item.CloudName, Normal);
                    continue;
                }
                if (m_cloud_ori_map[item.CloudName] == nullptr)
                {
                    machineLog->write("点云 ：" + item.CloudName + " 为空", Normal);
                    continue;
                }
                // 创建线程并存储到容器中
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
                    //模板匹配
                    threads.emplace_back(&rs_motion::templateIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == OneDimensional)
                {
                    //一维码识别
                    threads.emplace_back(&rs_motion::barCodeIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == TwoDimensional)
                {
                    //二维码识别
                    threads.emplace_back(&rs_motion::QrCodeIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == BlobModel)
                {
                    //Blob识别
                    threads.emplace_back(&rs_motion::blobIdentify, this, item);
                    showPhoto = true;
                }
                else if (item.VisionConfig == CircleModel)
                {
                    //圆检测模块
                    threads.emplace_back(&rs_motion::circleIdentify, this, item);
                    showPhoto = true;
                }
            }
            else if ((ENUMPOINTTYPE)item.PointType == ENUMPOINTTYPE::Circle_Measure)
            {
                //圆测距模块
                threads.emplace_back(&rs_motion::circleMeasure, this, item);
                showPhoto = true;
            }
           

            if (i + 1 < listProcess.size())
            {
                //当前执行行数加1并选中下一行
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

    //合并3D检测结果
    if (m_pinResult3D.size() > 1)
    {
        summary3dResult();
    }

    if (m_version == LsPin)
    {
        //清除图片
        emit clearWindow_signal();
    }
    if (showPhoto)
    {
        //整体执行完后进行pin针识别处理视觉结果
        int rtn = pinShowResult(m_runResult);
        if (rtn != 0)
        {
            machineLog->write("pin针结果输出失败", Normal);
            return rtn;
        }
        if (m_version == LsPin)
        {
            m_snResult.result = m_runResult.isNG ? "FAIL" : "PASS";
        }

        //获取当前流程整体状态
        machineLog->write("当前流程检测状态：" + QString::number(m_runResult.isNG), Normal);
        //刷新显示结果数据
        emit upDataProductPara_signal(m_runResult);
        //设备中的mes流程
        checkMesFunction();
    }

    if (m_isLoop.load())
    {
        //循环运行
        m_showStepIndex_L = 0;
        m_showStepIndex_R = 0;
        emit updateStepIndex_signal();
    }

    return 0;
}

//前置流程 1. 3d点位执行前消抖，延迟 ,开启触发和脉冲
int rs_motion::ItemPreProcess(PointItemConfig item)
{
    //3D前置动作
    if (item.PointType == ENUMPOINTTYPE::Point3D)
    {
        if (!m_pcMyCamera_3D)
        {
            // 创建设备实例
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
            if (m_pcMyCamera_3D == nullptr)
                return -1;
            //设备初始化
            m_pcMyCamera_3D->InitCamera3DLink();
            QTime time = QTime::currentTime();
            while (fabs(QTime::currentTime().secsTo(time)) < 20)
            {
                //最长等待20秒
                if (m_isEmergency.load())
                    return -1;
                //判断是否初始化完成
                if (m_pcMyCamera_3D->m_initSucess)
                    break;
            }
        }
        if (!m_pcMyCamera_3D->m_initSucess)
        {
            //没有成功初始化，再初始化一下
            m_pcMyCamera_3D->InitCamera3DLink();
            QTime time = QTime::currentTime();
            while (fabs(QTime::currentTime().secsTo(time)) < 20)
            {
                //最长等待20秒
                if (m_isEmergency.load())
                    return -1;
                //判断是否初始化完成
                if (m_pcMyCamera_3D->m_initSucess)
                    break;
            }
        }
        if (item.isSave3dCloud)
        {
            //保存点云
            m_pcMyCamera_3D->m_isSaveCloud = true;
            m_pcMyCamera_3D->m_cloudName = item.CloudName;
        }
        else
        {
            m_pcMyCamera_3D->m_isSaveCloud = false;
        }
        //是否已经打开设备在等待硬触发
        //if (!m_pcMyCamera_3D->getStartFlag())
        //{
        //    //开始采集
        //    m_pcMyCamera_3D->StartProgram();
        //}

        //设置行数
        m_pcMyCamera_3D->AllocBuf(item.scanRow3d);
        //设置曝光
        if(item.ExposureTime > 0)
            m_pcMyCamera_3D->setExpsuretime(item.ExposureTime);
        //开始采集
        m_pcMyCamera_3D->StartProgram();
        if (m_pcMyCamera_3D->m_cloud_ori != nullptr)
        {
            //清除上次已记录的点云
            m_pcMyCamera_3D->m_cloud_ori->clear();
        }

        switch (LSM->m_version){
        case ENUM_VERSION::TwoPin:
        {
            //初始化脉冲 
            setDO(LS_DO::DO_PlusCam3D1, 0);
            setDO(LS_DO::DO_PlusCam3D2, 0);
            setDO(LS_DO::DO_PlusCam3D3, 0);
            //初始化相机触发
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //消抖
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //启动脉冲 
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

            //延迟触发 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //触发相机
            setDO(LS_DO::DO_Cam3DTrigger, 1);
            break;
        }
        case ENUM_VERSION::LsPin:
        {
            //初始化相机触发
            setDO(LS_DO::LS_OUT_3DCamTrigger, 0);
            //消抖
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //延迟触发 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //触发相机
            setDO(LS_DO::LS_OUT_3DCamTrigger, 1);
            break;
        }
        case ENUM_VERSION::EdPin:
        {
            //初始化相机触发
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //消抖
            int buffetTime = m_Paramer.BuffetTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(buffetTime));
            //延迟触发 
            int delayTriggerTime = m_Paramer.DelayTriggerTime3D;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayTriggerTime));
            //触发相机
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
                ////先吹气松开
                //setDO(LS_DO::LS_OUT_ClampBlow, 1);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                ////关闭吹气
                //setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //松开载具
                setDO(LS_DO::LS_OUT_ClampCyl1, 0);
                setDO(LS_DO::LS_OUT_ClampCyl2, 0);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                //开启吸气
                setDO(LS_DO::LS_OUT_ClampSuck, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.suctionTime));
                QElapsedTimer materialTimer;
                materialTimer.start();  // 开始计时
                while (getDI(LS_DI::LS_IN_VacuumDetect) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // 单位：毫秒
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("夹爪真空检测超时：10秒内未吸紧", Normal);
                        machineLog->write("夹爪真空检测超时：10秒内未吸紧", Err);
                        return -1;
                    }
                    Sleep(1);
                }
            }
            else if (item.FeedAction == Suction_2)
            {
                //松开载具
                setDO(LS_DO::LS_OUT_ClampCyl1, 0);
                setDO(LS_DO::LS_OUT_ClampCyl2, 0);
                //检测载具原点信号
                QElapsedTimer materialTimer;
                materialTimer.start();  // 开始计时
                while (getDI(LS_DI::LS_IN_ClampCyl1_Home) == 0 || getDI(LS_DI::LS_IN_ClampCyl2_Home) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // 单位：毫秒
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("载具原点检测超时：10秒内未松开", Normal);
                        machineLog->write("载具原点检测超时：10秒内未松开", Err);
                        return -1;  
                    }
                    Sleep(1);
                }
                ////先吹气松开
                //setDO(LS_DO::LS_OUT_ClampBlow, 1);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                //关闭吹气
                //setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //开启吸气
                setDO(LS_DO::LS_OUT_ClampSuck, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.suctionTime));
                materialTimer.start();  // 开始计时
                while (getDI(LS_DI::LS_IN_VacuumDetect) == 0)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // 单位：毫秒
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("夹爪真空检测超时：10秒内未吸紧", Normal);
                        machineLog->write("夹爪真空检测超时：10秒内未吸紧", Err);
                        return -1;
                    }
                    Sleep(1);
                }
                Sleep(200);
            }
            else if (item.FeedAction == Unloading)
            {
                //吹气
                setDO(LS_DO::LS_OUT_ClampSuck, 0);
                setDO(LS_DO::LS_OUT_ClampBlow, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.unLoadTime));
                setDO(LS_DO::LS_OUT_ClampBlow, 0);
            }
            else if (item.FeedAction == Unloading_2)
            {
                //吹气
                setDO(LS_DO::LS_OUT_ClampSuck, 0);
                setDO(LS_DO::LS_OUT_ClampBlow, 1);

                //检测载具有料信号
                QElapsedTimer materialTimer;
                //模拟
                materialTimer.start();  // 开始计时
                while (getDI(LS_DI::LS_IN_MaterialDetect) == 1)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // 单位：毫秒
                    if (materialTimer.elapsed() > 10000) {
                        machineLog->write("载具检测超时：10秒内未检测到物料", Normal);
                        machineLog->write("载具检测超时：10秒内未检测到物料", Err);
                        return -1;  // 用特定错误码区分超时
                    }
                    Sleep(1);
                }
                //吹气延时
                std::this_thread::sleep_for(std::chrono::milliseconds(LSM->m_Paramer.unLoadTime));
                setDO(LS_DO::LS_OUT_ClampBlow, 0);
                //std::this_thread::sleep_for(std::chrono::milliseconds(500));
                //夹紧载具
                setDO(LS_DO::LS_OUT_ClampCyl1, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                setDO(LS_DO::LS_OUT_ClampCyl2, 1);
                //检测载具动点信号(改用原点信号是否消失去检测，动点信号不稳定)
                materialTimer.start();  // 开始计时
                while (getDI(LS_DI::LS_IN_ClampCyl1_Home) == 1 || getDI(LS_DI::LS_IN_ClampCyl2_Home) == 1)
                {
                    if (m_isEmergency.load())
                        return -1;
                    // 单位：毫秒
                    if (materialTimer.elapsed() > 20000) {
                        machineLog->write("载具动点检测超时：20秒内未夹紧", Normal);
                        machineLog->write("载具动点检测超时：20秒内未夹紧", Err);
                        return -1;
                    }
                    Sleep(1);
                }

            }
        }
    }

    return 0;
}

// 判断对应的脉冲轴（双轨pin机）
QString rs_motion::DetectPulsAxis(PointItemConfig item)
{
    return (fabs(item.TargetPX - ReadAxisP(LS_AxisName::X)) > 2) ? LS_AxisName::X : (item.StationL == ENUMSTATION::L ? LS_AxisName::Y1 : LS_AxisName::Y2);
}

//判断轴运动是否正向
bool rs_motion::isMovePostive(QString axis, double target)
{
    return (ReadAxisP(axis) - target <= 0);
}

// ItemPart 轴动作
int rs_motion::ItemMovingProcess(PointItemConfig item)
{
    if (item.PointType == Point3D_identify || item.PointType == Visual_Identity || item.PointType == Circle_Measure)
        return 0;
    int rtn = 0;
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        //双轨pin标机的轴运动模式
        rtn = pin2MotionAction(item);
        break;
    case ENUM_VERSION::LsPin:
        //立昇的轴运动模式
        rtn = pinLsAction(item);
        break;
    case ENUM_VERSION::EdPin:
        //易鼎丰的轴运动模式
        rtn = pinEdAction(item);
        break;
    case ENUM_VERSION::JmPin:
        //金脉的轴运动模式
        rtn = pinJmAction(item);
        break;
    case ENUM_VERSION::JmHan:
        //金脉焊缝的轴运动模式
        rtn = hanJmAction(item);
        break;
    default:
        break;
    }
    if (rtn != 0)
        return rtn;
    return 0;
}



// ItemPart 后置流程
//  1. 3d点位执行后保持，取消
//  2. 2d点位执行后，到位消抖 延迟 保持灯光
int rs_motion::ItemLastProcess(PointItemConfig item)
{
    //配置参数
    DeviceMoveit_Config Moveitinf = m_Paramer;
    int rtn = 0;
    QTime time = QTime::currentTime();
    //光源
    QString lightDo = LS_DO::DO_Light2DTrigger;
    switch ((ENUMPOINTTYPE)item.PointType)
    {
    case ENUMPOINTTYPE::Point2D:
        //region 2D
        //记录当前视觉配方
        m_visionFormula_2D.push_back(item.Vision);
        //消抖
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //光源触发  高电平触发
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
            //易鼎丰
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
            //易鼎丰下相机要同时开上下光源
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        setDO(lightDo, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //开光源
        setDO(lightDo, 1);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //易鼎丰下相机要同时开上下光源
            setDO(LS_DO::ED_DO_LightFrontTrigger, 1);
        }
        //延迟触发
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        //触发相机

        if (m_cameraName[item.CameraName].empty())
        {
            machineLog->write("相机名：" + item.CameraName + "配置获取失败！！", Normal);
            machineLog->write("相机名：" + item.CameraName + "配置获取失败！！", Err);
            return -1;
        }
        rtn = catPhoto2D(m_cameraName[item.CameraName], m_cameraMap[item.CameraName], item.ExposureTime);
        if (rtn != 0)
        {
            machineLog->write("Pin2D图片拍照失败！！错误码 ：" + QString::number(rtn), Normal);
            machineLog->write("Pin2D图片拍照失败！！错误码 ：" + QString::number(rtn), Err);
            return rtn;
        }
        //光源保持
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //关闭光源
        setDO(lightDo, 0);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //易鼎丰下相机要同时开上下光源
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        break;
    case ENUMPOINTTYPE::Point3D:
        //记录当前视觉配方
        m_visionFormula_3D.push_back(item.Vision);
        switch (m_version)
        {
        case TwoPin:
            //取消相机触发
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            //取消脉冲  
            setDO(LS_DO::DO_PlusCam3D1, 0);
            setDO(LS_DO::DO_PlusCam3D2, 0);
            setDO(LS_DO::DO_PlusCam3D3, 0);
            break;
        case LsPin:
            //取消相机触发
            setDO(LS_DO::LS_OUT_3DCamTrigger, 0);
            break;
        case EdPin:
            //取消相机触发
            setDO(LS_DO::DO_Cam3DTrigger, 0);
            break;
        default:
            break;
        }
        //脉冲到位保持
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime3D));
        time = QTime::currentTime();
        while (fabs(QTime::currentTime().secsTo(time)) < 10)
        {
            //最长等待10秒
            if (m_isEmergency.load())
                return -1;
            //判断是否成功采集3D点云
            if (m_pcMyCamera_3D->m_cloud_ori != nullptr)
            {
                if (m_pcMyCamera_3D->m_cloud_ori->size() > 0)
                {
                    //关闭3D相机线程
                    m_pcMyCamera_3D->StopProgram();
                    auto cloud = m_pcMyCamera_3D->m_cloud_ori->makeShared();
                    m_cloud_ori_map[item.CloudName] = cloud;
                    //if (!m_pcMyCamera_3D->depth_color_img.empty())
                    //{
                    //    machineLog->write("- - - ->>>>>>>>图片输出", Normal);
                    //    cv::imwrite("3Dtest.bmp", m_pcMyCamera_3D->depth_color_img);
                    //}
                    break;
                }
            }
        }
        if (m_pcMyCamera_3D->m_cloud_ori == nullptr)
        {
            machineLog->write("3D点云采集超时，退出运动!!", Normal);
            machineLog->write("3D点云采集超时，退出运动!!", Err);
            return -1;
        }
        if (m_pcMyCamera_3D->m_cloud_ori->size() <= 0)
        {
            machineLog->write("3D点云采集数量错误，退出运动!!", Normal);
            machineLog->write("3D点云采集数量错误，退出运动!!", Err);
            return -1;
        }
        break;
    case ENUMPOINTTYPE::PointNone: break;
    case ENUMPOINTTYPE::TakePhoto:
    {
        //拍照流程
        if (!m_cameraName.count(item.CameraName) ||
            !m_cameraMap.count(item.CameraName) ||
            !m_cameraMap[item.CameraName]) {
            machineLog->write("相机未配置：" + item.CameraName, Normal);
            machineLog->write("相机未配置：" + item.CameraName, Err);
            return -1;
        }
        //消抖
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //光源触发  高电平触发
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
            //易鼎丰
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
            //易鼎丰下相机要同时开上下光源
            setDO(LS_DO::ED_DO_LightFrontTrigger, 0);
        }
        setDO(lightDo, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //开光源
        setDO(lightDo, 1);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //易鼎丰下相机要同时开上下光源
            setDO(LS_DO::ED_DO_LightFrontTrigger, 1);
        }
        //延迟触发
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        rtn = catPhoto2D(m_cameraName[item.CameraName], m_cameraMap[item.CameraName], item.ExposureTime);
        if (rtn != 0)
        {
            machineLog->write(item.CameraName + "拍照失败！！错误码 ：" + QString::number(rtn), Normal);
            machineLog->write(item.CameraName + "拍照失败！！错误码 ：" + QString::number(rtn), Err);
            return rtn;
        }
        m_photo_map[item.PhotoName].Clear();
        m_photo_map[item.PhotoName] = m_cameraMap[item.CameraName]->m_outImg.Clone();
        machineLog->write("拍照成功", Normal);
        //光源保持
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //关闭光源
        setDO(lightDo, 0);
        if (item.CameraName == LS_CameraName::ed_Down)
        {
            //易鼎丰下相机要同时开上下光源
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

// 2D相机拍照流程
int rs_motion::catPhoto2D(std::string cameraName, std::shared_ptr<CMvCamera>& camera, int exposureTime)
{
    int rtn = 0;
    //创建实例
    if (camera == nullptr)
    {    
        // 创建设备实例
        camera = std::make_unique<CMvCamera>();
    }


    machineLog->write(" camera->m_bOpenDevice = " + QString::number(camera->m_bOpenDevice), Normal);
    //打开设备
    rtn = camera->OpenDevices_Action(cameraName);
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " 打开相机设备失败！！", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " 打开相机设备失败！！", Err);
        return rtn;
    }

    if (exposureTime > 0)
    {
        rtn = camera->SetExposureTime(exposureTime);
        if (rtn != 0)
        {
            machineLog->write(" 设置曝光时间失败！！", Normal);
        }
    }
    

    //开始采集
    rtn = camera->StartGrabbing_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " 开始采集失败！！", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " 开始采集失败！！", Err);
        return rtn;
    }
   
    //开始拍照
    rtn = camera->TakePhoto_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " 拍照失败！！", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " 拍照失败！！", Err);
        return rtn;
    }

    //停止采集
    rtn = camera->StopGrabbing_Action();
    if (rtn != 0)
    {
        machineLog->write(QString::fromStdString(cameraName) + " 停止采集失败！！", Normal);
        machineLog->write(QString::fromStdString(cameraName) + " 停止采集失败！！", Err);
        return rtn;
    }

    camera->m_outImg.Clear();
    camera->m_outImg = camera->convertToHalconImage(camera->m_pSaveImageBuf, camera->m_stImageInfo.nWidth, camera->m_stImageInfo.nHeight, 1);

    ////保存图片
    //rtn = camera->SaveImage_Action();
    //if (rtn != 0)
    //{
    //    machineLog->write("2D相片保存失败！！", Normal);
    //    return rtn;
    //}
    return 0;

}

// 获取最旧的文件路径
QString rs_motion::getOldestFileInDirectory(const QString& directoryPath) {
    QDir dir(directoryPath);
    if (!dir.exists()) {
        machineLog->write("没有找到此路径：" + directoryPath, Normal);
        return QString();
    }

    // 获取目录中的所有文件
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    if (fileList.isEmpty()) {
        machineLog->write("此路径中无文件：" + directoryPath, Normal);
        return QString();
    }

    // 初始化最旧的文件和修改时间
    QFileInfo oldestFile = fileList.first();
    QDateTime oldestDateTime = oldestFile.lastModified();

    // 遍历文件列表，找到修改时间最旧的文件
    for (const QFileInfo& fileInfo : fileList) {
        QDateTime fileDateTime = fileInfo.lastModified();
        if (fileDateTime < oldestDateTime) {
            oldestDateTime = fileDateTime;
            oldestFile = fileInfo;
        }
    }

    // 返回最旧文件的完整路径
    return oldestFile.absoluteFilePath();
}

// 清空文件夹
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

    // 获取目录中的所有文件
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    if (fileList.isEmpty()) {
        return true; // 目录为空，无需删除
    }

    // 遍历文件列表，逐个删除文件
    bool success = true;
    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        if (!QFile::remove(filePath)) {
            success = false;
        }
    }

    return success;
}

// pin结果展示 isNG:整个流程是否存在NG品
int rs_motion::pinShowResult(Run_Result& reslut)
{
    int rtn = 0;
    //machineLog->write("pin针结果输出...", Normal);
    if (!m_pinResult2D.isEmpty() && m_pinResult3D.isEmpty())
    {
        //纯2D结果显示
        for (int i = 0; i < m_pinResult2D.size(); i++)
        {
            emit showPin_signal(m_pinResult2D[i].first, m_pinResult2D[i].second);    //单独2Dpin针
            if (m_pinResult2D[i].first.hv_Ret == 0)
                reslut.isNG_2D = true;
        }
    }
    else if (!m_pinResult3D.isEmpty() && m_pinResult2D.isEmpty())
    {
        //纯3D结果显示
        machineLog->write("33333 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
        for (int i = 0; i < m_pinResult3D.size(); i++)
        {
            emit showPin3D_signal(m_pinResult3D[i].first, m_pinResult3D[i].second);    //单独3Dpin针

            if (m_pinResult3D[i].first.isOK == false)
                reslut.isNG_3D = true;

            //isNG = false;
        }
    }
    else if (!m_pinResult2D.isEmpty() && !m_pinResult3D.isEmpty())
    {
        // 混合结果（2D和3D均非空）
        std::unordered_map<QString, std::pair<ResultParamsStruct_Pin3DMeasure, PointItemConfig>> product3DMap;
        for (const auto& result3D : m_pinResult3D) {
            //先取出3D所有结果
            product3DMap[result3D.second.ProductName] = result3D;
        }
        std::unordered_set<QString> allSame;
        for (const auto& result2D : m_pinResult2D) {
            const QString& productName = result2D.second.ProductName;
            auto it = product3DMap.find(productName);
            if (it != product3DMap.end()) {
                //合并显示
                emit showPin2Dand3D_signal(result2D.first, result2D.second, it->second.first, it->second.second);
                allSame.insert(productName);
                if (result2D.first.hv_Ret == 0)
                    reslut.isNG_2D = true;
                if (it->second.first.isOK == false)
                    reslut.isNG_3D = true;
            }
            else {
                emit showPin_signal(result2D.first, result2D.second);   //单独2Dpin针
                if (result2D.first.hv_Ret == 0)
                    reslut.isNG_2D = true;
            }
        }

        for (auto result3D : m_pinResult3D)
        {
            //显示没有需要合并的3D图
            if (!allSame.count(result3D.second.ProductName))
            {
                emit showPin3D_signal(result3D.first, result3D.second);    //单独3Dpin针
                if (result3D.first.isOK == false)
                    reslut.isNG_3D = true;
            }
        }
    }

    if (reslut.isNG_2D || reslut.isNG_3D || reslut.isNG_Blob || reslut.isNG_Circle || reslut.isNG_OneDimensional
        || reslut.isNG_Temple || reslut.isNG_TwoDimensional || reslut.isNG_CircleMeasure)
        reslut.isNG = true;

    ////必过
    //reslut.isNG = false;

    if (!m_resultMergingImg.empty())
    {
        //显示合并的图片
        for (auto message : m_resultMergingImg)
        {
            emit showVision_signal(message.second);
        }
        //emit showAllVision_signal(m_resultMergingImg);
    }

    return 0;
}

//3D设备初始化
void rs_motion::init3dCamera()
{
    try {
        if (!m_pcMyCamera_3D)
        {
            // 创建设备实例
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
            if (m_pcMyCamera_3D == nullptr)
                return;
            //设备初始化
            m_pcMyCamera_3D->InitCamera3DLink();
            //QTime time = QTime::currentTime();
            //while (fabs(QTime::currentTime().secsTo(time)) < 20)
            //{
            //    //最长等待20秒
            //    if (m_isEmergency.load())
            //        return;
            //    //判断是否初始化完成
            //    if (m_pcMyCamera_3D->m_initSucess)
            //        break;
            //}
        }
    }
    catch (...) {
        machineLog->write("3d相机初始化异常", Normal);
        return;
    }
}


// 处理 3D 点云的逻辑函数
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
            machineLog->write("没有创建3D相机..", Normal);
            return;
        }
        auto cloud = cloudPoint->makeShared();
        if (cloud->empty())
        {
            machineLog->write("没有找到配方 " + item.Vision + " 的pin点云..", Normal);
            return;
        }
        // 找pin
        ResultParamsStruct_Pin3DMeasure result;
        result.depthImg = m_pcMyCamera_3D->depth_color_img.clone();
        // 进行图片处理
        machineLog->write("- - - ->>>>>>>>开始处理3D图片", Normal);

        //int processID = 1;
        //int modubleID = 2;
        //std::string modubleName = "Module1";
        //改节点名
        int processID = 0;
        int modubleID = 0;
        std::string modubleName = item.Vision.toStdString();
        std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        };

        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString exePath = QCoreApplication::applicationDirPath();
        QString configDirPath = exePath + "/3D_formulation";
        QString xmlDirPath = configDirPath + "/" + filename;
        if (!QFile::exists(xmlDirPath)) {
            machineLog->write(QString("3D配方文件缺失: %1").arg(xmlDirPath), Normal);
            return;
        }


#ifdef DEBUG_LHC
        if (m_pcMyCamera_3D == nullptr)
            m_pcMyCamera_3D = std::make_shared<VisionTools>();
        //测试图
        std::string str = "D:/移远/3D/data/cloud_ori.pcd";
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloudtemp(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::io::loadPCDFile(str, *cloudtemp);
        // 创建 PinWindow 实例，使用第二个构造函数
        Pin3DMeasureWindow pin_3d(cloudtemp, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
        pin_3d.hide();
        pin_3d.ori_cloud = cloudtemp;
        pin_3d.LoadShowCloud(cloudtemp);
#else
        auto Pin3DMeasureParams1 = new Pin3DMeasure(modubleName, modubleID, processID);
        auto modubleDatas1 = new modubleDatas();
        // 创建 PinWindow 实例，使用第二个构造函数
        //Pin3DMeasureWindow pin_3d(cloud, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
        //pin_3d.hide();

#endif // DEBUG_LHC

        //// 提取文件名
        //pin_3d.ConfigPath = configDirPath.toUtf8().constData();
        //pin_3d.modubleDatas1->setGetConfigPath(pin_3d.ConfigPath);
        //pin_3d.XmlPath = xmlDirPath.toUtf8().constData();
        //pin_3d.modubleDatas1->setConfigPath(pin_3d.XmlPath);
        // 读参数
        modubleDatas1->setGetConfigPath(configDirPath.toUtf8().constData());
        modubleDatas1->setConfigPath(xmlDirPath.toUtf8().constData());
        ErrorCode_Xml errorCode;
        RunParamsStruct_Pin3DMeasure RunParamsPin3DMeasure;
        errorCode = modubleDatas1->ReadParams_Pin3DMeasure(RunParamsPin3DMeasure, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("3D参数读取失败..", Normal);
            if (Pin3DMeasureParams1)
                delete Pin3DMeasureParams1;
            if (modubleDatas1)
                delete modubleDatas1;
            return;
        }

        rtn = Pin3DMeasureParams1->PinMeasureAlgo(cloud, RunParamsPin3DMeasure, result);
        if (rtn != 0)
        {
            machineLog->write("3D查找pin针失败..", Normal);
            if (Pin3DMeasureParams1)
                delete Pin3DMeasureParams1;
            if (modubleDatas1)
                delete modubleDatas1;
            return;
        }
        result.depthImg = m_pcMyCamera_3D->depth_color_img;
        machineLog->write(" result.PinFlag.size() = " + QString::number(result.PinFlag.size()), Normal);
        //取结果
        m_pinResult3D.push_back(std::make_pair(result, item));
        //查看是否有需要导出到CSV的数据
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
        machineLog->write("- - - ->>>>>>>>获取的3D点云为空", Normal);
    }

    return;
}


// 合并3D检测结果
void rs_motion::summary3dResult()
{
    machineLog->write("合并3D检测结果..", Normal);
    machineLog->write("11111 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
    for (int i = 0; i < m_pinResult3D.size(); i++)
    {
        for (int j = m_pinResult3D.size() - 1; j > i; j--)
        {
            QString res1 = m_pinResult3D[i].second.ProductName;
            QString res2 = m_pinResult3D[j].second.ProductName;
            // 确定是否使用的同一插头，然后合并结果
            if (res1 == res2 && m_pinResult3D[i].first.PinFlag.size() == m_pinResult3D[j].first.PinFlag.size())
            {
                for (int z = 0; z < m_pinResult3D[i].first.PinHeight.size(); z++)
                {
                    //if (m_pinResult3D[i].first.PinHeight[z] == -2 && m_pinResult3D[j].first.PinHeight[z] != -2)
                    //{
                    //    //结果取有针的结果
                    //    m_pinResult3D[i].first.PinHeight[z] = m_pinResult3D[j].first.PinHeight[z];
                    //    m_pinResult3D[i].first.PinFlag[z] = m_pinResult3D[j].first.PinFlag[z];
                    //}
                    if (m_pinResult3D[i].first.PinHeight[z] == 0 && m_pinResult3D[j].first.PinHeight[z] != 0)
                    {
                        //结果取有针的结果
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
        //更新3D数据结果NG还是OK
        m_pinResult3D[i].first.isOK = true;
        for (int j = 0; j < m_pinResult3D[i].first.PinFlag.size(); j++)
        {
            //输出结果
            if (m_pinResult3D[i].first.PinFlag[j] == false)
            {
                m_pinResult3D[i].first.isOK = false;
                break;
            }
        }
    }
    machineLog->write("22222 m_pinResult3D.size() = " + QString::number(m_pinResult3D.size()), Normal);
}

// 处理 2D 识别的逻辑函数
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
    // 获取对应的配方路径
    QString filename = item.Vision + ".xml";
    QString configDirPath = exePath + "/2D_formulation";
    QString xmlDirPath = configDirPath + "/" + filename;

    //// 创建 PinWindow 实例，使用第二个构造函数
    //PinWindow pin_2d(img, processID, modubleID, modubleName, refreshConfig, configDirPath.toUtf8().constData());
    ////pin_2d.hide();

    //// 提取文件名
    //pin_2d.ConfigPath = configDirPath.toUtf8().constData();
    //pin_2d.dllDatas1->setGetConfigPath(pin_2d.ConfigPath);
    //pin_2d.XmlPath = xmlDirPath.toUtf8().constData();
    //pin_2d.dllDatas1->setConfigPath(pin_2d.XmlPath);
    //// 找pin
    //rtn = pin_2d.slot_PinDetect();

    auto dllDatas1 = new DllDatas();
    auto PinDetect1 = new PinDetect();
    try {
        int ret = 1;
        //XML读取参数
        dllDatas1->setConfigPath(xmlDirPath.toUtf8().constData());
        dllDatas1->setGetConfigPath(configDirPath.toUtf8().constData());
        RunParamsStruct_Pin RunParams_Pin;
        ErrorCode_Xml errorCode = dllDatas1->ReadParams_PinDetect(RunParams_Pin, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml) {
            //XML读取失败
            if (dllDatas1)
                delete dllDatas1;
            if (PinDetect1)
                delete PinDetect1;
            return;
        }
        //调用算法
        PinDetect1->resultParams.ngIdx.clear(); //清一下ng模块
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
        machineLog->write("查找pin针失败..", Normal);
        if (dllDatas1)
            delete dllDatas1;
        if (PinDetect1)
            delete PinDetect1;
        return;
    }
    auto result = PinDetect1->resultParams;
    //取处理的图片
    result.ho_Image = img;
    //取结果
    m_pinResult2D.push_back(std::make_pair(result, item));
    //查看是否有需要导出到CSV的数据
    auto isOK = result.hv_Ret == 1;
    if (m_ResultToCsv.count(item.Tip))
    {
        m_ResultToCsv[item.Tip] = isOK ? "OK" : "NG";
    }
    machineLog->write("查找pin针完成", Normal);

    if (dllDatas1)
        delete dllDatas1;
    if (PinDetect1)
        delete PinDetect1;
    return;
}

// 销毁相机
void rs_motion::destroyCamera()
{
    if (m_pcMyCamera_3D)
    {
        //关闭设备
        m_pcMyCamera_3D->StopProgram();
        //m_pcMyCamera_3D.reset();
    }

    //清理相机字典
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

// 双轨pin标机动作
int rs_motion::pin2MotionAction(PointItemConfig item)
{
    int rtn = 0;
    QString axisY = item.StationL == ENUMSTATION::L ? LS_AxisName::Y1 : LS_AxisName::Y2;
    //QString axisU = item.StationL == ENUMSTATION::L ? LS_AxisName::U1 : LS_AxisName::U2;

    //250717 把左工位旋转加回去
    QString axisU = LS_AxisName::U1;

    //判断Z轴移动方向
    bool isPostiveMove_Z = isMovePostive(LS_AxisName::Z, item.TargetPZ);

    //先执行Z轴收回动作
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(LS_AxisName::Z, item.TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        WriteAxisSpeedModel(LS_AxisName::Z, (SpeedModeEnum)item.SpeedModel);
    }

    //其他轴异步执行
    rtn = AxisPmove(axisU, item.TargetPU);
    if (rtn != 0)
    {
        machineLog->write("触发执行U轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(LS_AxisName::RZ, item.TargetPRZ);
    if (rtn != 0)
    {
        machineLog->write("触发执行RZ轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(LS_AxisName::X, item.TargetPX);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY, item.TargetPY);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y轴动作错误！！" , Normal);
        return rtn;
    }
    //在线设置轴的速度
    for (auto axis : LS_AxisName::allAxis)
    {
        WriteAxisSpeedModel(axis, (SpeedModeEnum)item.SpeedModel);
    }

    QTime time = QTime::currentTime();

    //250717 把左工位旋转加回去
    //等待所有轴到位
    while (isRuning(LS_AxisName::X)
        || isRuning(axisY)
        || isRuning(LS_AxisName::RZ)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("等待所有轴到位错误！！", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 30秒超时做超时预警   
        if (time.secsTo(QTime::currentTime()) > 30)
        {
            machineLog->write("轴到位超时！！", Normal);
            break;
        }
    }

    ////等待所有轴到位
    //while (isRuning(LS_AxisName::X)
    //    || isRuning(axisY)
    //    || isRuning(LS_AxisName::RZ))
    //{
    //    if (m_isEmergency.load())
    //    {
    //        machineLog->write("等待所有轴到位错误！！", Normal);
    //        return -1;
    //    }

    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //    // 30秒超时做超时预警   
    //    if (time.secsTo(QTime::currentTime()) > 30)
    //    {
    //        machineLog->write("轴到位超时！！", Normal);
    //        break;
    //    }
    //}

    //后执行Z轴伸出到位动作
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(LS_AxisName::Z, item.TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    return 0;
}


// 立昇pin标机动作
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

    //所有轴设置速度
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

    //判断Z轴移动方向
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);

    //先执行Z轴收回动作
    if (!isPostiveMove_Z)
    {
        //rtn = AxisPmoveAsync(axisZ, TargetPZ);
        //if (rtn != 0)
        //{
        //    machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
        //    return rtn;
        //}
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);

        rtn = AxisPmove(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        //等待所有轴到位
        while (isRuning(axisZ))
        {
            if (m_isEmergency.load())
            {
                machineLog->write("等待所有轴到位错误！！", Normal);
                return -1;
            }
            if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
            {
                m_isEmergency.store(true);
                machineLog->write("取料破真空！！", Normal);
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    //其他轴异步执行
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("触发执行U轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY1, TargetPY1);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y1轴动作错误！！", Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY2, TargetPY2);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y2轴动作错误！！", Normal);
        return rtn;
    }
    ////在线设置轴的速度
    //for (auto axis : LS_AxisName::allAxis)
    //{
    //    WriteAxisSpeedModel(axis, (SpeedModeEnum)item.SpeedModel);
    //}

    QTime time = QTime::currentTime();
    //等待所有轴到位
    while (isRuning(axisX)
        || isRuning(axisY1)
        || isRuning(axisY2)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("等待所有轴到位错误！！", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 30秒超时做超时预警   
        if (time.secsTo(QTime::currentTime()) > 30)
        {
            machineLog->write("轴到位超时！！", Normal);
            break;
        }
        if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
        {
            m_isEmergency.store(true);
            machineLog->write("取料破真空！！", Normal);
            return -1;
        }
    }

    //后执行Z轴伸出到位动作
    if (isPostiveMove_Z)
    {
        //rtn = AxisPmoveAsync(axisZ, TargetPZ);
        //if (rtn != 0)
        //{
        //    machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
        //    return rtn;
        //}
        rtn = AxisPmove(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        //等待所有轴到位
        while (isRuning(axisZ))
        {
            if (m_isEmergency.load())
            {
                machineLog->write("等待所有轴到位错误！！", Normal);
                return -1;
            }
            if (item.FeedAction == Suction && getDI(LS_DI::LS_IN_VacuumDetect) == 0)
            {
                m_isEmergency.store(true);
                machineLog->write("取料破真空！！", Normal);
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    return 0;
}

// 易鼎丰pin标机动作
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

    //所有轴设置速度
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

    //判断Z轴移动方向
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //先执行Z轴收回动作
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //判断Z1轴移动方向
    bool isPostiveMove_Z1 = isMovePostive(axisZ1, TargetPZ1);
    //先执行Z1轴收回动作
    if (!isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("执行Z1轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //判断Z2轴移动方向
    bool isPostiveMove_Z2 = isMovePostive(axisZ2, TargetPZ2);
    //先执行Z1轴收回动作
    if (!isPostiveMove_Z2)
    {
        rtn = AxisPmoveAsync(axisZ2, TargetPZ2);
        if (rtn != 0)
        {
            machineLog->write("执行Z2轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //其他轴异步执行
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("触发执行U轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    rtn = AxisPmove(axisU1, TargetPU1);
    if (rtn != 0)
    {
        machineLog->write("触发执行U1轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //等待所有轴到位
    while (isRuning(axisX)
        || isRuning(axisY)
        || isRuning(axisU1)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("等待所有轴到位错误！！", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60秒超时做超时预警   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("轴到位超时！！", Normal);
            break;
        }
    }

    //后执行Z轴伸出到位动作
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("执行Z1轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z2)
    {
        rtn = AxisPmoveAsync(axisZ2, TargetPZ2);
        if (rtn != 0)
        {
            machineLog->write("执行Z2轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    return 0;
}

// 初始化轴，IO等
void rs_motion::initAxisAndIO()
{
    //先确认版本
    switch (m_version)
    {
    case ENUM_VERSION::TwoPin:
        //双轨Pin针版本
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderL1);  // 治具1气缸1
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderL2);  // 治具1气缸2
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderR1);  // 治具2气缸1
        LS_DO::allDO.push_back(LS_DO::DO_ClampCylinderR2);  // 治具2气缸2
        LS_DO::allDO.push_back(LS_DO::DO_Lock1);            // 电子锁1
        LS_DO::allDO.push_back(LS_DO::DO_Lock2);            // 电子锁2
        LS_DO::allDO.push_back(LS_DO::DO_Lock3);            // 电子锁3
        LS_DO::allDO.push_back(LS_DO::DO_LightFlood);       // 照明灯
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D1);       // 3D脉冲-X
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D2);       // 3D脉冲-Y1
        LS_DO::allDO.push_back(LS_DO::DO_PlusCam3D3);       // 3D脉冲-Y2
        LS_DO::allDO.push_back(LS_DO::DO_ReadCodeTrigger1); // 扫码枪触发1
        LS_DO::allDO.push_back(LS_DO::DO_ReadCodeTrigger2); // 扫码枪触发2
        LS_DO::allDO.push_back(LS_DO::DO_LightOK_L);        // 指示灯1-绿
        LS_DO::allDO.push_back(LS_DO::DO_LightNG_L);        // 指示灯1-红
        LS_DO::allDO.push_back(LS_DO::DO_LightOK_R);        // 指示灯2-绿
        LS_DO::allDO.push_back(LS_DO::DO_LightNG_R);        // 指示灯2-红
        break;
    case ENUM_VERSION::EdPin:
        //易鼎丰版本
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y);
        LS_AxisName::allAxis.push_back(LS_AxisName::U);
        LS_AxisName::allAxis.push_back(LS_AxisName::U1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Z2);

        //DO（有可复用的，直接添加）
        LS_DO::allDO.push_back(LS_DO::ED_DO_PositionCyl1);     
        LS_DO::allDO.push_back(LS_DO::ED_DO_PositionCyl2);     
        LS_DO::allDO.push_back(LS_DO::ED_DO_ClampCyl1_Valve1); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_ClampCyl1_Valve2); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_CamFrontTrigger);  
        LS_DO::allDO.push_back(LS_DO::ED_DO_CamRearTrigger);   
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightFrontTrigger);
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightRearTrigger); 
        LS_DO::allDO.push_back(LS_DO::ED_DO_LightPositioningTrigger);
        // 启动控制类信号注册
        LS_DO::allDO.push_back(LS_DO::ED_DO_FeedBeltStart);               // 来料皮带启动
        LS_DO::allDO.push_back(LS_DO::ED_DO_DischargeBeltStart);          // 出料皮带启动
        LS_DO::allDO.push_back(LS_DO::ED_DO_NGBeltStart);                 // NG皮带启动

        //DI
        LS_DI::allDI.clear();
        // 按钮类
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnStart);          // 启动按钮
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnStop);           // 停止按钮
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnReset);          // 复位按钮
        LS_DI::allDI.push_back(LS_DI::ED_DI_SwitchAutoMode);    // 自动模式旋钮
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnEStop1);        // 急停按钮1
        LS_DI::allDI.push_back(LS_DI::ED_DI_BtnEStop2);        // 急停按钮2

        // 气缸位置检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl1_Home); // 定位气缸1原点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl1_Work); // 定位气缸1动点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl2_Home); // 定位气缸2原点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampPosCyl2_Work); // 定位气缸2动点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl1_Home);    // 夹紧气缸1原点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl1_Work);    // 夹紧气缸1动点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl2_Home);    // 夹紧气缸2原点
        LS_DI::allDI.push_back(LS_DI::ED_DI_ClampCyl2_Work);    // 夹紧气缸2动点
        // 安全门检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_ProdDetect);        // 产品有无检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed1);      // 来料侧安全门1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed2);      // 来料侧安全门2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed3);      // 来料侧安全门3
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Feed4);      // 来料侧安全门4
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge1); // 出料侧安全门1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge2); // 出料侧安全门2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge3); // 出料侧安全门3
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Discharge4); // 出料侧安全门4
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_NG1);        // NG皮带侧安全门1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_NG2);        // NG皮带侧安全门2
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Rear1);      // 背面安全门1
        LS_DI::allDI.push_back(LS_DI::ED_DI_SafDoor_Rear2);      // 背面安全门2
        // 报警信号
        LS_DI::allDI.push_back(LS_DI::ED_DI_AirPressureAlarm);   // CDA进气压力报警
        // 皮带检测类信号注册
        LS_DI::allDI.push_back(LS_DI::ED_DI_FeedBeltMaterialDetect);      // 来料皮带产品来料检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_FeedBeltArrivalDetect);       // 来料皮带产品到位检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_DischargeBeltReleaseDetect);  // 出料皮带产品放料检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_DischargeBeltArrivalDetect);  // 出料皮带产品到位检测
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltReleaseDetect_Left);    // NG皮带产品放料检测1（左）
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltReleaseDetect_Right);   // NG皮带产品放料检测2（右）
        LS_DI::allDI.push_back(LS_DI::ED_DI_NGBeltFullDetect);            // NG皮带产品满料检测
        break;
    case ENUM_VERSION::LsPin:
        //立昇版本
        //轴名
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
        // 清空容器
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
        //博泰版本
        //轴名
        LS_AxisName::allAxis.clear();
        LS_AxisName::allAxis.push_back(LS_AxisName::X);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y1);
        LS_AxisName::allAxis.push_back(LS_AxisName::Y2);
        break;
    case ENUM_VERSION::JmPin:
        //金脉pin版本
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
        //金脉焊缝版本
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

// 倍速链控制及上料监控
int rs_motion::feedingAction()
{
    if (m_version == LsPin)
    {
        QElapsedTimer materialTimer;
        //立昇版本的倍速链控制来料
        //等待上料区无料
        while (getDI(LS_DI::LS_IN_UpperMaterialDetect) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //打开要料信号
        setDO(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady, 1);
        //检测前机允许出料信号
        while (getDI(LS_DI::LS_IN_SMEMA_UpFront_AllowOut) == 0 )
        {
            //模拟信号
            if (LSM->inDI)
                break;
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //打开电机
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 1);
        //检测有料信号
        while (getDI(LS_DI::LS_IN_UpperMaterialDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(500);
        ////模拟
        //Sleep(5000);
        //关闭要料信号
        setDO(LS_DO::LS_OUT_SMEMA_UpFront_FeedReady, 0);
        //关闭电机
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        Sleep(500);
        //打开升降气缸
        setDO(LS_DO::LS_OUT_LiftCyl_Activate, 1);
        //检测升降气缸动点信号
        materialTimer.start();  // 开始计时
        while (getDI(LS_DI::LS_IN_LiftCyl_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("升降气缸动点检测超时：10秒内未完成顶升上升！！", Normal);
                machineLog->write("升降气缸动点检测超时：10秒内未完成顶升上升！！", Err);
                return -1;
            }
            Sleep(5);
        }
        //检测是否有料
        while (getDI(LS_DI::LS_IN_UpperCheckProduct) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 3000) {
                machineLog->write("载具有料检测超时：3秒内未检测到载具，流到下一工站", Normal);
                return 10086;
            }
            Sleep(5);
        }
        if (!notQR)
        {
            //tcp扫码
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
                    machineLog->write("扫码超时：10秒内未扫码成功，流到下一工站", Normal);
                    return 10086;
                }
                Sleep(5);
            }
            if (ls_tcpMess == "FAIL")
            {
                machineLog->write("扫码获取信息失败！！流到下一工站", Normal);
                return 10086;
            }
            else
            {
                m_snReq.sn = ls_tcpMess;
            }
        }
        else
        {
            //不扫码用固定SN
            m_snReq.sn = LSM->testSN;
        }
        if (!notMes)
        {
            //m_snReq.sn = "G11101LS2020020101-022504250121";
            
            //用户账号
            m_snReq.op = m_productPara.UserAccout;

            // 调用检查接口
            SnCheckResponse res = check_sn_process(
                "http://10.23.164.56:8001/MainWebForm.aspx",
                m_snReq
            );

            // 处理结果
            if (res.code == 200) {
                machineLog->write("SN检查通过, 主条码：" + res.main_sn, Mes);

            }
            else {
                machineLog->write("SN检查失败，返回内容：" + res.msg + " 流到下一工站", Mes);
                return 10086;
            }
        }


    }
    return 0;
}

// 倍速链控制及下料监控
int rs_motion::unloadingAction()
{
    if (m_version == LsPin)
    {
        //立昇版本的倍速链控制走料
        //升降气缸下降
        setDO(LS_DO::LS_OUT_LiftCyl_Activate, 0);
        //检测顶升和阻挡气缸原点信号
        QElapsedTimer materialTimer;
        materialTimer.start();  // 开始计时
        while (getDI(LS_DI::LS_IN_LiftCyl_Home) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("升降气缸动原点检测超时：10秒内未完成顶升下降！！", Normal);
                machineLog->write("升降气缸动原点检测超时：10秒内未完成顶升下降！！", Err);
                return -1;
            }
            Sleep(5);
        }
        Sleep(200);
        //关闭阻挡
        setDO(LS_DO::LS_OUT_BlockCyl_Activate, 1);
        //检测阻挡气缸动点信号
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_BlockCyl1_Home) == 1)
        {
           if (m_isEmergency.load())
               return -1;
           if (materialTimer.elapsed() > 10000) {
               machineLog->write("阻挡气缸动动点检测超时：10秒内未完成阻挡下降！！", Normal);
               machineLog->write("阻挡气缸动动点检测超时：10秒内未完成阻挡下降！！", Err);
               return -1;
           }
           Sleep(5);
        }
        //打开送料信号
        setDO(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady, 1);
        //检测后机允许收料信号
        while (getDI(LS_DI::LS_IN_SMEMA_UpRear_AllowOut) == 0)
        {
            //模拟信号
            if (LSM->outDI)
                break;
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //打开电机
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 1);
        ////模拟
        //Sleep(5000);
        //检测后机要料信号结束再停
        while (getDI(LS_DI::LS_IN_SMEMA_UpRear_AllowOut) == 1 || LSM->outDI)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //检测出料信号
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_UpperOutputDetect) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("倍速链出料信号检测超时：10秒内未完成出料！！", Normal);
                machineLog->write("倍速链出料信号检测超时：10秒内未完成出料！！", Err);
                return -1;
            }
            Sleep(5);
        }
        Sleep(500);
        //关闭电机
        setDO(LS_DO::LS_OUT_UpperMotor_Dir, 0);
        //关闭送料信号
        setDO(LS_DO::LS_OUT_SMEMA_UpRear_FeedReady, 0);
        //打开阻挡
        setDO(LS_DO::LS_OUT_BlockCyl_Activate, 0);
        //检测阻挡气缸动点信号
        materialTimer.start();
        while (getDI(LS_DI::LS_IN_BlockCyl1_Home) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > 10000) {
                machineLog->write("阻挡气缸动原点检测超时：10秒内未完成阻挡上升！！", Normal);
                machineLog->write("阻挡气缸动原点检测超时：10秒内未完成阻挡上升！！", Err);
                return -1;
            }
            Sleep(5);
        }

    }
    return 0;
}

// 下层倍速链控制
int rs_motion::downFeedingAction()
{
    if (LSM->m_cardInitStatus)
    {
        machineLog->write("下层倍速链：等待开始运行", Normal);
        //等待开始
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
        machineLog->write("下层倍速链：开始运行", Normal);
        //启动工作后一直进行监控
        if (m_isEmergency.load() || m_isRunStop.load())
            return -1;
        //打开告诉后机可以送料
        setDO(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady, 1);
        machineLog->write("下层倍速链：告诉后机可以送料，等待后机信号", Normal);
        //等待后机信号
        while (m_DI[LS_DI::LS_IN_SMEMA_LowRear_AllowOut].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //打开下层电机
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 1);
        machineLog->write("下层倍速链：打开下层电机，等待下层有料信号", Normal);
        //等待下层进料信号
        while (m_DI[LS_DI::LS_IN_LowerMaterialDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        //等待下层完全进料信号
        while (m_DI[LS_DI::LS_IN_LowerMaterialDetect].state == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //关闭要料信号
        setDO(LS_DO::LS_OUT_SMEMA_LowRear_FeedReady, 0);
        machineLog->write("下层倍速链：关闭要料信号，等待下层完全进料信号", Normal);

        //有料后走1.5秒
        Sleep(1500);
        //关闭下层电机
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 0);
        //打开告诉前机可以入料
        setDO(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived, 1);
        machineLog->write("下层倍速链：打开告诉前机可以入料，等待前机信号", Normal);
        //等待前机信号
        while (m_DI[LS_DI::LS_IN_SMEMA_LowFront_AllowOut].state == 0)
        ////模拟
        //while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //打开下层电机
        setDO(LS_DO::LS_OUT_LowerMotor_Dir, 1);
        //检测下层出料信号
        machineLog->write("下层倍速链：开始出料", Normal);
        //检测出料信号
        while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 0)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        machineLog->write("下层倍速链：检测是否完全出料", Normal);
        //完全出料
        while (m_DI[LS_DI::LS_IN_LowerOutputDetect].state == 1)
        {
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        Sleep(1000);
        //关闭电机
        setDO(LS_DO::LS_OUT_LowerMotor_Run, 0);
        //关闭告诉前机可以入料
        setDO(LS_DO::LS_OUT_SMEMA_LowFront_FeedArrived, 0);
        machineLog->write("下层倍速链：出料完成", Normal);
        Sleep(5);
    }
}

// 一维码识别
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
            //图像为空，或者图像被clear
            machineLog->write("一维码检测图像为空", Normal);
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
        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/BarCode_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //// 创建 PinWindow 实例，使用第二个构造函数
        //BarCodeWindow barDlg(ho_img, processID, modubleID, modubleName);
        //barDlg.hide();

        //// 提取文件名
        //barDlg.ConfigPath = configDirPath.toUtf8().constData();
        //barDlg.XmlPath = xmlDirPath.toUtf8().constData();

        //int ret = 1;
        ////识别一维码
        //ret = barDlg.slot_FindBarCode();

        int ret = 1;
        //XML读取参数
        RunParamsStruct_BarCode hv_RunParamsBarCode;
        BarCodeDetect* barCodeParams1 = new BarCodeDetect();
        DataIO dataIOC;//参数存取对象

        ErrorCode_Xml errorCode = dataIOC.ReadParams_BarCode(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsBarCode, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("一维码参数读取失败，配方：" + item.Vision, Normal);
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
        //调用算法
        ret = barCodeParams1->FindBarCodeFunc(ho_img, hv_RunParamsBarCode, barCodeParams1->mResultParams);
        auto result = barCodeParams1->mResultParams;

        //bool isOK = reslut.hv_RetNum > 0 ? true : false;
        //取结果
        //m_barCodeResult.push_back(std::make_pair(result, item));

        //显示结果，看看是单独显示还是结合显示
        if (item.isResultMerging)
        {
            //结合显示
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //这是第一个要融合的图
                m_resultMergingImg[item.PhotoName].img = ho_img;
               
            }

            if (result.hv_RetNum > 0)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
                //结果文本
                QString text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                //m_snResult.sn = text;

                //查看是否有需要导出到CSV的数据
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
            std::lock_guard<std::mutex> lock(m_visionMutex);  // 加锁
            m_resultMergingImg[item.PhotoName].BarMes.push_back(make_pair(item, result));
        }
        else
        {
            //直接显示
            Vision_Message message;
            message.img = ho_img;
            message.isNG = result.hv_RetNum.I() > 0 ? false : true;
            item.isNG = message.isNG;
            message.BarMes.push_back(make_pair(item, result));
            m_runResult.isNG_OneDimensional = message.isNG;
            if (!message.isNG)
            {
                //结果文本
                QString text = QString::fromUtf8(result.hv_CodeContent.S().Text());

                //查看是否有需要导出到CSV的数据
                if (m_ResultToCsv.count(item.Tip))
                {
                    m_ResultToCsv[item.Tip] = text;
                }
            }
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////保存结果图
            //if (!item.Tip.isEmpty())
            //{
            //    //优先备注名
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

// 二维码识别
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
            //图像为空，或者图像被clear
            machineLog->write("二维码检测图像为空", Normal);
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
        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/QRCode_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //---------------------------------------------------------------------
        //// 创建 QRCodeWindow 实例，使用第二个构造函数
        //QRCodeWindow barDlg(ho_img, processID, modubleID, modubleName);
        //barDlg.hide();

        //// 提取文件名
        //barDlg.ConfigPath = configDirPath.toUtf8().constData();
        //barDlg.XmlPath = xmlDirPath.toUtf8().constData();


        //int ret = 0;
        ////识别二维码
        //ret = barDlg.slot_FindCode2d();
        //auto result = barDlg.code2dParams1->mResultParams;
        //m_snResult.sn = result.hv_CodeContent.S();
        //---------------------------------------------------------------------

        //bool isOK = ret == 0 ? true : false;
        //取结果
        //m_qrCodeResult.push_back(std::make_pair(result, item));


        int ret = 1;
        //XML读取参数
        RunParamsStruct_Code2d hv_RunParamsCode2d;
        DataIO dataIOC;//参数存取对象

        hv_RunParamsCode2d.hv_CodeType_Run.Clear();
        ErrorCode_Xml errorCode = dataIOC.ReadParams_Code2d(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCode2d, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("二维码参数读取失败，配方：" + item.Vision, Normal);
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
        //调用算法
        code2dParams1->FindCode2dFunc(ho_img, hv_RunParamsCode2d, code2dParams1->mResultParams);
        auto result = code2dParams1->mResultParams;
        //if (result.hv_RetNum.I() == 0)
        //{
        //    machineLog->write("二维码识别失败，配方：" + item.Vision, Normal);
        //    if (code2dParams1)
        //    {
        //        delete code2dParams1;
        //        code2dParams1 = nullptr;
        //    }

        //    return ret;
        //}

        //显示结果，看看是单独显示还是结合显示
        if (item.isResultMerging)
        {
            //结合显示
            std::lock_guard<std::mutex> lock(m_visionMutex);
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //这是第一个要融合的图
                m_resultMergingImg[item.PhotoName].img = ho_img;
            }

            if (result.hv_RetNum > 0)
            {
                //OK
                m_resultMergingImg[item.PhotoName].isNG = false;
                m_runResult.modelName = result.hv_CodeContent.S();

                //结果文本
                QString text = "";
                if (!result.hv_CodeContent.S().IsEmpty())
                {
                    //text = result.hv_CodeContent.S();
                    text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                }
                //m_snResult.sn = text;

                //查看是否有需要导出到CSV的数据
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
            //直接显示
            Vision_Message message;
            message.img = ho_img;
            message.isNG = result.hv_RetNum.I() > 0 ? false : true;
            item.isNG = message.isNG;
            message.QRMes.push_back(make_pair(item, result));
            m_runResult.isNG_TwoDimensional = message.isNG;
            if (!message.isNG)
            {
                m_runResult.modelName = result.hv_CodeContent.S();
                //结果文本
                QString text = "";
                if (!result.hv_CodeContent.S().IsEmpty())
                {
                    //text = result.hv_CodeContent.S();
                    text = QString::fromUtf8(result.hv_CodeContent.S().Text());
                }
                //m_snResult.sn = text;

                //查看是否有需要导出到CSV的数据
                if (m_ResultToCsv.count(item.Tip))
                {
                    //std::lock_guard<std::mutex> lock(m_visionMutex);
                    m_ResultToCsv[item.Tip] = text;
                }
            }

            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////保存结果图
            //if (!item.Tip.isEmpty())
            //{
            //    //优先备注名
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Tip);
            //}
            //else
            //{
            //    temp.saveResultPhotoToFile(qimg, LSM->m_resultImgPath, item.Vision);
            //}
            emit showVision_signal(message);
        }

        //---------------------------- 加mes功能 --------------------------
        //if (!LSM->notMes && item.Tip == "SN")
        //{
        //    if (item.isNG)
        //    {
        //        //直接NG
        //        m_mesResponse_ED.state = false;
        //        m_mesResponse_ED.description = "二维码识别未查找到SN";
        //    }
        //    else
        //    {
        //        QString text = "";
        //        if (!result.hv_CodeContent.S().IsEmpty())
        //        {
        //            //text = result.hv_CodeContent.S();
        //            text = QString::fromUtf8(result.hv_CodeContent.S().Text());
        //        }
        //        // 准备请求数据
        //        MesCheckInRequest_ED request;
        //        request.sn = text;
        //        //request.IP = "192.168.1.100";
        //        //request.Slot = "A3";

        //        // 发送请求
        //        m_mesResponse_ED = LSM->mes_check_in_ED("http://192.168.100.205:311", request);

        //        // 处理响应
        //        if (m_mesResponse_ED.state) {
        //            machineLog->write("校验通过，可以开始测试", Normal);
        //        }
        //        else {
        //            machineLog->write("校验失败：" + m_mesResponse_ED.description, Normal);
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

// 模板匹配
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
            //图像为空，或者图像被clear
            machineLog->write("模板匹配图像为空", Normal);
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
        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Template_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //--------------------------------------------------------------------------
        //TemplateWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// 提取文件名
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();

        //int ret = 0;
        ////模板匹配
        //ret = tempDlg.slot_FindTemplate();
        //auto result = tempDlg.TemplateDetect1->mResultParams;
        //auto runPara = tempDlg.hv_RunParamsTemplate;
        //bool isOK = result.hv_RetNum.I() == runPara.hv_NumMatches.I();
        //--------------------------------------------------------------------------
        //取结果
        //m_TemplateResult.push_back(std::make_pair(tempDlg.TemplateDetect1->mResultParams, item));

        int ret = 1;
        //XML读取参数
        RunParamsStruct_Template hv_RunParamsTemplate;
        DataIO dataIOC;//参数存取对象

        ErrorCode_Xml errorCode = dataIOC.ReadParams_Template(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsTemplate, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("模板匹配参数读取失败，配方：" + item.Vision, Normal);
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
        //调用算法
        rtn = TemplateDetect1->FindTemplate(ho_img, hv_RunParamsTemplate, TemplateDetect1->mResultParams);

        auto result = TemplateDetect1->mResultParams;
        bool isOK = result.hv_RetNum.I() == hv_RunParamsTemplate.hv_NumMatches.I();

        //查看是否有需要导出到CSV的数据
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isOK ? "OK":"NG";
        }

        //显示结果，看看是单独显示还是结合显示
        if (item.isResultMerging)
        {
            //结合显示
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //这是第一个要融合的图
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
            std::lock_guard<std::mutex> lock(m_visionMutex);  // 加锁
            m_resultMergingImg[item.PhotoName].TempMes.emplace_back(make_pair(item, result));
            //auto& tempVec = m_resultMergingImg[item.PhotoName].TempMes;
            //tempVec.emplace_back(std::make_pair(std::move(item), std::move(result)));
        }
        else
        {
            //直接显示
            Vision_Message message;
            message.img = ho_img;
            message.isNG = isOK ? false : true;
            item.isNG = message.isNG;
            message.TempMes.push_back(make_pair(item, result));
            m_runResult.isNG_Temple = message.isNG;
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////保存结果图
            //if (!item.Tip.isEmpty())
            //{
            //    //优先备注名
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

// blob分析
int rs_motion::blobIdentify(PointItemConfig item)
{
    BlobDetect* BlobDetect1 = new BlobDetect();
    try {
        auto ho_img = m_photo_map[item.PhotoName];
        if (ho_img.Key() == nullptr)
        {
            //图像为空，或者图像被clear
            machineLog->write("blob分析图像为空", Normal);
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
        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Blob_formulation/";
        QString xmlDirPath = configDirPath  + filename;

        //BlobWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// 提取文件名
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();


        //int ret = 0;
        //blob分析
        //ret = tempDlg.slot_BlobDetect();

        int ret = 1;
        //XML读取参数
        RunParamsStruct_Blob hv_RunParamsBlob;
        DataIO dataIOC;//参数存取对象

        ErrorCode_Xml errorCode = dataIOC.ReadParams_Blob(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsBlob, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("Blob参数读取失败，配方：" + item.Vision, Normal);
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
        //调用算法
        ret = BlobDetect1->BlobFunc(ho_img, hv_RunParamsBlob, BlobDetect1->mResultParams);

        auto result = BlobDetect1->mResultParams;
        bool isOK = ret == 0 ? true : false;
        //取结果
        //m_BolbResult.push_back(std::make_pair(tempDlg.BlobDetect1->mResultParams, item));

        //查看是否有需要导出到CSV的数据
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isOK ? "OK" : "NG";
        }

        //显示结果，看看是单独显示还是结合显示
        if (item.isResultMerging)
        {
            //结合显示
            if (!m_resultMergingImg.count(item.PhotoName))
            {
                //这是第一个要融合的图
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
            std::lock_guard<std::mutex> lock(m_visionMutex);  // 加锁
            m_resultMergingImg[item.PhotoName].BlobMes.push_back(make_pair(item, result));
        }
        else
        {
            //直接显示
            Vision_Message message;
            message.img = ho_img;
            message.isNG = isOK ? false : true;
            item.isNG = message.isNG;
            message.BlobMes.push_back(make_pair(item, result));
            m_runResult.isNG_Blob = message.isNG;
            //rs_FunctionTool temp;
            //QImage qimg = temp.showVision_slot(message);
            //emit showImage_signal(qimg);
            ////保存结果图
            //if (!item.Tip.isEmpty())
            //{
            //    //优先备注名
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

// 圆检测
int rs_motion::circleIdentify(PointItemConfig item)
{
    auto ho_img = m_photo_map[item.PhotoName];
    if (ho_img.Key() == nullptr)
    {
        //图像为空，或者图像被clear
        machineLog->write("圆检测分析图像为空", Normal);
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
    // 获取对应的配方路径
    QString filename = item.Vision + ".xml";
    QString configDirPath = exePath + "/Vision_formulation/Circle_formulation/";
    QString xmlDirPath = configDirPath + filename;

    CircleWindow tempDlg(ho_img, processID, modubleID, modubleName);
    tempDlg.hide();

    // 提取文件名
    tempDlg.ConfigPath = configDirPath.toUtf8().constData();
    tempDlg.XmlPath = xmlDirPath.toUtf8().constData();


    int ret = 0;
    //blob分析
    ret = tempDlg.slot_FindCircle();
    auto result = tempDlg.CircleDetect1->mResultParams;
    bool isNG = ret != 0 ? true : false;
    m_runResult.isNG_Circle = isNG;
    //取结果
    //m_CircleResult.push_back(std::make_pair(tempDlg.CircleDetect1->mResultParams, item));
    //try {
    //    //显示结果，看看是单独显示还是结合显示
    //    if (item.isResultMerging)
    //    {
    //        //结合显示
    //        if (!m_resultMergingImg.count(item.PhotoName))
    //        {
    //            //这是第一个要融合的图
    //            if (result.hv_RetNum > 0)
    //            {
    //                //OK
    //                HObject ho_ShowObj;
    //                // 定义蓝色（RGB=0,0,255）
    //                HTuple color;
    //                color[0] = 0;   // Red
    //                color[1] = 255;   // Green
    //                color[2] = 0; // Blue
    //                // 覆盖区域颜色  
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
    //            //取之前的图去融合
    //            if (result.hv_RetNum > 0)
    //            {
    //                //OK
    //                HObject ho_ShowObj;
    //                // 定义蓝色（RGB=0,0,255）
    //                HTuple color;
    //                color[0] = 0;   // Red
    //                color[1] = 255;   // Green
    //                color[2] = 0; // Blue
    //                // 覆盖区域颜色  
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
    //            // 定义蓝色（RGB=0,0,255）
    //            HTuple color;
    //            color[0] = 0;   // Red
    //            color[1] = 255;   // Green
    //            color[2] = 0; // Blue
    //            // 覆盖区域颜色  
    //            PaintRegion(result.ho_Contour_Circle, ho_img, &ho_ShowObj, color, "margin");
    //            // 转换为 OpenCV Mat 并显示
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

// 圆测距
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
            //图像为空，或者图像被clear
            machineLog->write("圆检测分析图像为空", Normal);
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
        // 获取对应的配方路径
        QString filename = item.Vision + ".xml";
        QString configDirPath = exePath + "/Vision_formulation/Circle_formulation/";
        QString xmlDirPath = configDirPath + filename;

        //------------------------------------------------------------------------
        //CircleWindow tempDlg(ho_img, processID, modubleID, modubleName);
        //tempDlg.hide();

        //// 提取第一个圆文件名
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();
        //int ret = 0;
        ////圆检测
        //ret = tempDlg.slot_FindCircle();
        //auto result = tempDlg.CircleDetect1->mResultParams;
        //bool isNG = ret != 0 ? true : false;
        //m_runResult.isNG_CircleMeasure = isNG;
        //if (isNG)
        //{
        //    machineLog->write("圆测距第一个圆检测失败", Normal);
        //    ResultParamsStruct_Circle temp;
        //    emit showCircleMeasure_signal(result, temp, item, 0, isNG);
        //    return -1;
        //}
        //------------------------------------------------------------------------

        //第一个圆 XML读取参数
        RunParamsStruct_Circle hv_RunParamsCircle_1;
        DataIO dataIOC;//参数存取对象
        ErrorCode_Xml errorCode = dataIOC.ReadParams_Circle(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCircle_1, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("圆测距第一个圆参数读取失败，配方：" + item.Vision, Normal);
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
        //调用算法
        auto ret = CircleDetect1_1->FindCircle(ho_img, hv_RunParamsCircle_1, CircleDetect1_1->mResultParams);
        auto result = CircleDetect1_1->mResultParams;
        bool isNG = ret != 0 ? true : false;
        if(!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;
        if (isNG)
        {
            machineLog->write("圆测距第一个圆检测失败", Normal);
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
        ////  提取第二个圆文件名
        //filename = item.VisionTwo + ".xml";
        //xmlDirPath = configDirPath + "/" + filename;
        //tempDlg.ConfigPath = configDirPath.toUtf8().constData();
        //tempDlg.XmlPath = xmlDirPath.toUtf8().constData();
        ////模块名称
        //tempDlg.nodeName = item.VisionTwo.toStdString();
        //ret = tempDlg.slot_FindCircle();
        //auto resultTwo = tempDlg.CircleDetect1->mResultParams;
        //isNG = ret != 0 ? true : false;
        //m_runResult.isNG_CircleMeasure = isNG;
        //if (isNG)
        //{
        //    machineLog->write("圆测距第二个圆检测失败", Normal);
        //    emit showCircleMeasure_signal(result, resultTwo, item, 0, isNG);
        //    return -1;
        //}
        //------------------------------------------------------------------------

        //第二个圆 XML读取参数
        filename = item.VisionTwo + ".xml";
        xmlDirPath = configDirPath + "/" + filename;
        modubleName = item.VisionTwo.toStdString();

        RunParamsStruct_Circle hv_RunParamsCircle_2;
        errorCode = dataIOC.ReadParams_Circle(configDirPath.toUtf8().constData(), xmlDirPath.toUtf8().constData(), hv_RunParamsCircle_2, processID, modubleName, modubleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("圆测距第二个圆参数读取失败，配方：" + item.VisionTwo, Normal);
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
        //调用算法
        ret = CircleDetect1_2->FindCircle(ho_img, hv_RunParamsCircle_2, CircleDetect1_2->mResultParams);
        auto resultTwo = CircleDetect1_2->mResultParams;
        isNG = ret != 0 ? true : false;
        if (!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;
        if (isNG)
        {
            machineLog->write("圆测距第二个圆检测失败", Normal);
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

        //测距
        auto x1 = result.hv_CircleRowCenter.D();
        auto y1 = result.hv_CircleColumnCenter.D();

        auto x2 = resultTwo.hv_CircleRowCenter.D();
        auto y2 = resultTwo.hv_CircleColumnCenter.D();

        //像素
        double dis = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        //转世界
        //dis *= 0.025;
        dis /= item.PixDistance;
        //计算是否符合要求
        if (dis < item.CircleBenchmark + item.CircleUpError && dis > item.CircleBenchmark - item.CircleDownError)
        {
            isNG = false;
        }
        else
        {
            isNG = true;
        }

        //如果有NG，不要覆盖为OK
        if(!m_runResult.isNG_CircleMeasure)
            m_runResult.isNG_CircleMeasure = isNG;

        //查看是否有需要导出到CSV的数据
        if (m_ResultToCsv.count(item.Tip))
        {
            m_ResultToCsv[item.Tip] = isNG ? "NG" : "OK";
        }

        //展示图片
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

//关DO
void rs_motion::closeDO()
{
    if (m_version == LsPin)
    {
        //复原DO
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
        //关闭倍速链电机
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
        //复原DO
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
        //清除信号
        MODBUS_TCP.writeIntCoil(134, 0, 0);
        MODBUS_TCP.writeIntCoil(134, 1, 0);
        MODBUS_TCP.writeIntCoil(134, 2, 0);
        MODBUS_TCP.writeIntCoil(134, 3, 0);
        MODBUS_TCP.writeIntCoil(134, 4, 0);
        MODBUS_TCP.writeIntCoil(134, 5, 0);
    }
}

//三色灯指示灯 0：绿灯，1：黄灯，2：红灯
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

//按钮指示灯 type 0：启动，1：复位，2：停止，3：蜂鸣器，4：照明
void rs_motion::setButtonLight(int type, int val)
{

    if (m_version == LsPin)
    {
        if (val == 1)
        {
            //开
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
            //关
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
            //开
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
            //关
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


// 全局网络管理器（确保线程安全）
Q_GLOBAL_STATIC(QNetworkAccessManager, globalManager)
SnCheckResponse rs_motion::check_sn_process(const QString& url, const SnCheckRequest& req) {
    QNetworkRequest netRequest;

    // 验证并设置URL
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        return { 500, "无效的URL格式", "", "N", "N", "N" };
    }
    netRequest.setUrl(requestUrl);

    // 设置请求头
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ================== 构建JSON数据 ==================
    QJsonObject requestJson;
    requestJson["cmd"] = req.cmd;
    requestJson["p"] = req.p;
    requestJson["station_name"] = req.station_name;
    requestJson["sn"] = req.sn;
    requestJson["op"] = req.op;

    // 可选字段处理
    if (!req.csn.isEmpty()) requestJson["csn"] = req.csn;
    if (!req.tooling_no.isEmpty()) requestJson["tooling_no"] = req.tooling_no;
    if (!req.machine_no.isEmpty()) requestJson["machine_no"] = req.machine_no;
    if (!req.first_sign.isEmpty()) requestJson["first_sign"] = req.first_sign;

    //进站上传输出的信息输出到 mesin_debug.json 方便查看
    QString jsonString = QJsonDocument(requestJson).toJson();
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    // 构建日志目录路径
    QDir logDir(appDir + "/mes_log");

    // 确保目录存在（不存在时自动创建）
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {  // 使用mkpath确保创建多级目录（虽然这里只有一级）
            qCritical() << "无法创建日志目录:" << logDir.absolutePath();
        }
    }

    // 构建完整文件路径
    QString filePath = logDir.filePath("mesin_debug.json");
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << jsonString;
        file.close();
        qDebug() << "JSON saved to mesin_debug.json";
    }

    // ================== 发送POST请求 ==================
    QNetworkReply* reply = globalManager->post(
        netRequest,
        QJsonDocument(requestJson).toJson()  // 关键：必须转成QByteArray
    );

    //// ================== 同步等待响应 ==================
    //QEventLoop loop;
    //QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    //loop.exec();

    // ================== 新增超时控制 ==================
    QEventLoop loop;
    QTimer timeoutTimer;

    // 设置5秒超时
    const int timeoutMs = 5000;

    // 连接三个关键信号
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("mes请求超时，URL: " + url, Normal);
        if (reply->isRunning()) {
            reply->abort();
        }
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== 响应处理 ==================
    SnCheckResponse result;
    bool isTimeout = !timeoutTimer.isActive(); // 检测是否触发超时


    // ================== 解析响应数据 ==================
    SnCheckResponse resp;
    if (isTimeout) {
        resp.code = 500;
        machineLog->write("mes请求超10S未响应!!", Mes);
    }
    if (reply->error() != QNetworkReply::NoError) {
        resp.msg = "网络错误: " + reply->errorString();
        reply->deleteLater();
        return resp;
    }

    // 解析JSON
    QJsonParseError parseError{};
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        resp.msg = "JSON解析错误: " + parseError.errorString();
        reply->deleteLater();
        return resp;
    }

    // 提取关键字段
    QJsonObject resObj = doc.object();
    resp.code = resObj.value("code").toInt(500);
    resp.msg = resObj.value("msg").toString();
    resp.main_sn = resObj.value("main_sn").toString();
    resp.first_sign = resObj.value("first_sign").toString("N");
    resp.rh_first_sign = resObj.value("rh_first_sign").toString("N");
    resp.IsStay = resObj.value("IsStay").toString("N");
    machineLog->write("pin针进站信息，sn :  " + req.sn, Mes);
    machineLog->write("pin针进站信息，code :  " + resp.code, Mes);
    machineLog->write("pin针进站信息，msg :  " + resp.msg, Mes);

    //进站上传mes返回输出的信息输出到 mesinBack_debug.json 方便查看
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

    // ================== 1. URL有效性校验 ==================
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        machineLog->write("无效的URL格式: " + url, Normal);
        return { 500, "Invalid URL format", "", "N", "N", "N" };
    }
    netRequest.setUrl(requestUrl);

    // ================== 2. 请求头配置 ==================
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //netRequest.setRawHeader("X-Request-Source", "QtClient/1.0"); // 自定义头

    // ================== 3. 构建分层JSON数据 ==================
    QJsonObject rootJson;
    // 3.1 基础字段
    rootJson["cmd"] = req.cmd;
    rootJson["station_name"] = req.station_name;
    rootJson["sn"] = req.sn;
    rootJson["op"] = req.op;
    rootJson["op_time"] = req.op_time;
    rootJson["result"] = req.result;
    rootJson["start_time"] = req.start_time;
    rootJson["stop_time"] = req.stop_time;

    // 3.2 转换测试数据集合
    QJsonArray testDataArray;
    for (const TestResult& testItem : req.result_data) {
        // 验证测试项有效性
        if (testItem.name.isEmpty() || testItem.result.isEmpty()) {
            machineLog->write("无效的测试数据项，跳过处理", Mes);
            continue;
        }
        testDataArray.append(testItem.toJson());
    }
    rootJson["result_data"] = testDataArray;

    //出站上传输出的信息输出到 mesout_debug.json 方便查看
    QString jsonString = QJsonDocument(rootJson).toJson();
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    // 构建日志目录路径
    QDir logDir(appDir + "/mes_log");

    // 确保目录存在（不存在时自动创建）
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {  // 使用mkpath确保创建多级目录（虽然这里只有一级）
            qCritical() << "无法创建日志目录:" << logDir.absolutePath();
        }
    }
    // 构建完整文件路径
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

    // ================== 4. 发送网络请求 ==================
    QNetworkReply* reply = globalManager->post(
        netRequest,
        QJsonDocument(rootJson).toJson(QJsonDocument::Compact) // 紧凑格式
    );

    // ================== 5. 超时控制机制 ==================
    QEventLoop eventLoop;
    QTimer timeoutTimer;
    const int timeoutSeconds = 10; // 10秒超时

    // 5.1 连接信号
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        if (reply->isRunning()) {
            machineLog->write("请求超时，终止连接: " + url, Mes);
            reply->abort();
        }
        eventLoop.quit();
        });

    // 5.2 启动定时器
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutSeconds * 1000);

    // 5.3 进入事件循环（禁止处理用户输入）
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    // ================== 6. 响应处理 ==================
    SnCheckResponse response;
    const bool isTimeout = !timeoutTimer.isActive();

    // 6.1 处理超时
    if (isTimeout) {
        response.code = 504;
        response.msg = "Request timeout after " + QString::number(timeoutSeconds) + "s";
        machineLog->write("MES接口响应超时", Mes);
        reply->deleteLater();
        return response;
    }

    // 6.2 处理网络错误
    if (reply->error() != QNetworkReply::NoError) {
        response.code = reply->error();
        response.msg = "Network error: " + reply->errorString();
        machineLog->write("网络错误: " + reply->errorString(), Mes);
        reply->deleteLater();
        return response;
    }

    // 6.3 读取响应数据
    const QByteArray responseData = reply->readAll();
    if (responseData.isEmpty()) {
        response.code = 502;
        response.msg = "Empty response from server";
        machineLog->write("收到空响应", Mes);
        reply->deleteLater();
        return response;
    }

    // 6.4 解析JSON
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        response.code = 503;
        response.msg = "JSON parse error: " + parseError.errorString();
        machineLog->write("响应解析失败: " + parseError.errorString(), Mes);
        reply->deleteLater();
        return response;
    }

    // 6.5 提取业务数据
    const QJsonObject resObj = doc.object();
    response.code = resObj["code"].toInt(500); // 默认500错误
    response.msg = resObj["msg"].toString("Unknown error");

    // 6.6 提取详细字段
    if (resObj.contains("data")) {
        const QJsonObject dataObj = resObj["data"].toObject();
        response.main_sn = dataObj["main_sn"].toString();
        response.first_sign = dataObj["first_sign"].toString("N");
        response.rh_first_sign = dataObj["rh_first_sign"].toString("N");
        response.IsStay = dataObj["IsStay"].toString("N");
    }

    // 6.7 记录成功日志
    if (response.code == 200) {
        machineLog->write("pin针过站信息上传成功，sn :  " + req.sn, Mes);
        machineLog->write("pin针过站信息上传成功，msg :  " + response.msg, Mes);
    }
    else {
        machineLog->write("业务错误,错误码msg: " + response.msg, Mes);
        machineLog->write("业务错误,错误码sn: " + req.sn, Mes);
    }

    reply->deleteLater();
    return response;
}

int rs_motion::robotCheck(int err) {
    switch (err)
    {
    case 1:
        machineLog->write("A吸盘已经检测掉料OR吸真空异常", Normal);
        machineLog->write("A吸盘已经检测掉料OR吸真空异常", Err);
        return -1;
        break;
    case 2:
        machineLog->write("B吸盘已经检测掉料OR吸真空异常", Normal);
        machineLog->write("B吸盘已经检测掉料OR吸真空异常", Err);
        return -1;
        break;
    case 3:
        machineLog->write("A吸盘取料异常", Normal);
        machineLog->write("A吸盘取料异常", Err);
        return -1;
        break;
    case 4:
        machineLog->write("B吸盘取料异常", Normal);
        machineLog->write("B吸盘取料异常", Err);
        return -1;
        break;
    case 5:
        machineLog->write("A爪气缸升异常 OR B爪气缸降异常", Normal);
        machineLog->write("A爪气缸升异常 OR B爪气缸降异常", Err);
        return -1;
        break;
    case 6:
        machineLog->write("A爪气缸降异常 OR B爪气缸升异常", Normal);
        machineLog->write("A爪气缸降异常 OR B爪气缸升异常", Err);
        return -1;
        break;
    case 7:
        machineLog->write("机器人不在任何一原点，请手动回原点", Normal);
        machineLog->write("机器人不在任何一原点，请手动回原点", Err);
        return -1;
        break;
    case 8:
        machineLog->write("无允许放料信号，清料终止", Normal);
        machineLog->write("无允许放料信号，清料终止", Err);
        return -1;
        break;

    default:
        break;
    }
    return 0;
};

//实时监控机器人皮带取料
int rs_motion::runRobotLoad_Belt()
{
    QElapsedTimer materialTimer;
    int ngCount = 0;
    //等待启动线程的标志位成立
    Sleep(1000);
    machineLog->write("等待皮带来料", Normal);
    m_isLoadErr.store(false);

    //检测机器人是否报错
    while (m_isStart.load() && !LSM->m_notRobot)
    {
        //启动工作后一直进行监控
        if (m_isEmergency.load() || m_isRunStop.load())
            return -1;

        if (getDI(LS_DI::ED_DI_FeedBeltArrivalDetect) == 0)
        {
            //皮带上没有料，先进料
            //machineLog->write("等待皮带来料", Normal);
            if (getDI(LS_DI::ED_DI_FeedBeltMaterialDetect) == 0)
                continue;
            //启动皮带
            setDO(LS_DO::ED_DO_FeedBeltStart, 1);
            //皮带到料信号
            machineLog->write("等待皮带到料", Normal);
            while (getDI(LS_DI::ED_DI_FeedBeltArrivalDetect) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                Sleep(5);
            }
            //对射开关与相机位置装不上，加延时
            Sleep(350);
            //停止皮带
            setDO(LS_DO::ED_DO_FeedBeltStart, 0);
        }

        bool haveProduct = false;
        MODBUS_TCP.readIntCoil(34, 6, haveProduct);
        if (haveProduct)
        {
            //手上有料或正在放料中，不取
            continue;
        }
        if (ngCount > 2)
        {
            //LSM->m_isEmergency = true;
            machineLog->write("皮带进料定位失败！！", Normal);
            machineLog->write("皮带进料定位失败！！", Err);
            //触发蜂鸣器
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
        //---------------------------- 拍照 --------------------------------------
        m_photo_map[LS_CameraName::ed_Location].Clear();
        if (!m_cameraName.count(LS_CameraName::ed_Location) ||
            !m_cameraMap.count(LS_CameraName::ed_Location) ||
            !m_cameraMap[LS_CameraName::ed_Location]) {
            machineLog->write("相机未配置：" + LS_CameraName::ed_Location, Normal);
            ngCount += 1;
            //m_isEmergency.store(true);
            continue;
        }
        DeviceMoveit_Config Moveitinf = m_Paramer;
        //消抖
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.BuffetTime2D));
        //光源触发  高电平触发
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 1);
        //延迟触发
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.DelayTriggerTime2D));
        auto rtn = catPhoto2D(m_cameraName[LS_CameraName::ed_Location], m_cameraMap[LS_CameraName::ed_Location]);
        if (rtn != 0)
        {
            machineLog->write("定位相机 拍照失败！！错误码 ：" + QString::number(rtn), Normal);
            ngCount += 1;
            //m_isEmergency.store(true);
            Sleep(5000);
            continue;
        }
        m_photo_map[LS_CameraName::ed_Location].Clear();
        m_photo_map[LS_CameraName::ed_Location] = m_cameraMap[LS_CameraName::ed_Location]->m_outImg.Clone();
        machineLog->write("定位相机拍照成功", Normal);
        //光源保持
        std::this_thread::sleep_for(std::chrono::milliseconds(Moveitinf.HoldTime2D));
        //关闭光源
        setDO(LS_DO::ED_DO_LightPositioningTrigger, 0);


        //----------------------------- 定位算法 ------------------------------------
        Mat photo = ImageTypeConvert::HObject2Mat(m_photo_map[LS_CameraName::ed_Location]);
        auto timestamp = QDateTime::currentMSecsSinceEpoch();

        QString appPath = QCoreApplication::applicationDirPath();
        auto robotDetect = new RobotCalibDetect();
        //运行参数
        RunParamsStruct_RobotCalib tmpRun;
        //结果参数
        ResultParamsStruct_RobotCalib tmpResult;
        int processId = 0;	//流程ID
        string nodeName = LSM->m_forMulaName.toStdString();	//模块名称
        int processModbuleID = 0;	//模块ID

        string ConfigPath = "";
        string XmlPath = "";
        if (!LSM->m_forMulaName.isEmpty())
        {
            //取同配方的标定文件
            ConfigPath = appPath.toStdString() + "/Vision_formulation/Robot/";
            XmlPath = appPath.toStdString() + "/Vision_formulation/Robot/" + LSM->m_forMulaName.toStdString() + ".xml";
        }
        else
        {
            ConfigPath = appPath.toStdString() + "/Vision_formulation/Robot/";
            XmlPath = appPath.toStdString() + "/Vision_formulation/Robot/Config.xml";
        }
        //读运行参数
        auto errorCode = robotDetect->ReadParams_RobotCalib(ConfigPath, XmlPath, tmpRun, processId, nodeName, processModbuleID);
        if (errorCode != Ok_Xml)
        {
            machineLog->write("机器定位参数读取失败 ！！", Normal);
            //m_isEmergency.store(true);
            ngCount += 1;
            continue;
        }

        int res = robotDetect->RobotLocate(photo, tmpRun, tmpResult);
        if (res != 0)
        {
            machineLog->write("机器视觉定位失败 ！！", Normal);
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
        ////给机器人发送治具
        //int num = LSM->m_forMulaName.toInt();
        //if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
        //    && num != 5 && num != 6 && num != 7 &&
        //    num != 8 && num != 9)
        //{
        //    machineLog->write("配方无对应治具！！", Normal);
        //    continue;
        //}

        //给机器人发送治具（改）
        if (LSM->m_formulaToRobot.count(LSM->m_forMulaName))
        {
            int num = LSM->m_formulaToRobot[LSM->m_forMulaName];
            if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
                && num != 5 && num != 6 && num != 7 &&
                num != 8 && num != 9)
            {
                machineLog->write("配方无对应治具机器人方案！！", Normal);
                continue;
            }

            MODBUS_TCP.writeRegisterInt(138, num);
        }
        else
        {
            machineLog->write("未找到对应配方的机器人方案，请检查对应关系！！", Normal);
            continue;
        }
       

        //给机器人发送坐标
        MODBUS_TCP.writeRegisterFloat(144, result.x);
        MODBUS_TCP.writeRegisterFloat(146, result.y);
        MODBUS_TCP.writeRegisterFloat(148, result.a);

        //给机器人发送允许取料
        MODBUS_TCP.writeIntCoil(134, 0, 1);
        //等待机器人取料
        machineLog->write("等待机器人取料", Normal);
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
            //一直让他取料
            MODBUS_TCP.writeIntCoil(134, 0, 1);
        }
        //结束取料
        MODBUS_TCP.writeIntCoil(134, 0, 0);

        MODBUS_TCP.readIntCoil(34, 0, feedInBeltPickDone); 
        materialTimer.start();
        while (feedInBeltPickDone)
        {
            //清信号
            auto err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 0, feedInBeltPickDone);
            //一直清料
            MODBUS_TCP.writeIntCoil(134, 0, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("等待机器人来料皮带完成信号清理超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未清理", Normal);
                machineLog->write("等待机器人来料皮带完成信号清理超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未清理", Err);
                //machineLog->write("modbus 35 bit 3 读取超时：" + QString::number(ERROR_TIME) + " 毫秒后还未成功读取", Normal);
                m_isEmergency.store(true);
                return -1;
            }
        }
        //清除偏移位置
        MODBUS_TCP.writeRegisterFloat(144, 0);
        MODBUS_TCP.writeRegisterFloat(146, 0);
        MODBUS_TCP.writeRegisterFloat(148, 0);
        machineLog->write("机器人取料完成..", Normal);
        if (LSM->m_isOneStep.load())
        {
            //单步运行
            break;
        }

        Sleep(3000);
    }
}

//机器人首次放料到载具函数
int rs_motion::runRobotCheck()
{
    QElapsedTimer materialTimer;
    //如果载具有料，不放料，直接运行
    if (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        //夹紧气缸
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品！！", Normal);
                machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品！！", Err);
                return -1;
            }
            Sleep(5);
        }

        setDO(LS_DO::ED_DO_PositionCyl1, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        materialTimer.start();  // 开始计时
        while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位！！", Normal);
                machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位！！", Err);
                return -1;
            }
            Sleep(5);
        }
        setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
        setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
        materialTimer.start();  // 开始计时
        while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧！！", Normal);
                machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧！！", Err);
                return -1;
            }
            Sleep(5);
        }
        return 0;
    }
    ////等待载具无料
    //while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    //{
    //    if (m_isEmergency.load())
    //        return -1;
    //    Sleep(5);
    //}
    //等待手上有料
    bool haveProduct = false;
    MODBUS_TCP.readIntCoil(34, 6, haveProduct);
    while (!haveProduct)
    {
        if (m_isEmergency.load())
            return -1;
        MODBUS_TCP.readIntCoil(34, 6, haveProduct);
        Sleep(5);
    }

    //---------------------------- 皮带取料完成 机器人放料 --------------------------------------
    //先确定治具是否在等待位，如果不是，要先走到等待位
    if (fabs(ReadAxisP(LS_AxisName::Y) - m_Axis[LS_AxisName::Y].initPos) > ERROR_VALUE)
    {
        AxisInitPAsync(LS_AxisName::Y);
    }
    if (fabs(ReadAxisP(LS_AxisName::U1) - m_Axis[LS_AxisName::U1].initPos) > ERROR_VALUE)
    {
        AxisInitPAsync(LS_AxisName::U1);
    }

    m_isRobotLoading.store(true);
    //给机器人发送允许放料
    //modbusRobot.writeBit(134, 1, 1);
    MODBUS_TCP.writeIntCoil(134, 1, 1);
    //等待机器人放料
    machineLog->write("等待机器人放料", Normal);
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
            machineLog->write("等待机器人放料超时：" + QString::number(ERROR_TIME_2) + " 毫秒后还未成功读取", Normal);
            machineLog->write("等待机器人放料超时：" + QString::number(ERROR_TIME_2) + " 毫秒后还未成功读取", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //关闭放料信号
    MODBUS_TCP.writeIntCoil(134, 1, 0);
    MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
    while (fixturePlaceDone)
    {
        //清信号
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 1, fixturePlaceDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 1, 0);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("等待机器人放料完成超时：" + QString::number(ERROR_TIME_2) + " 毫秒后还未成功读取", Normal);
            machineLog->write("等待机器人放料完成超时：" + QString::number(ERROR_TIME_2) + " 毫秒后还未成功读取", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }

    //---------------------------- 放料完成 有料检测,打开定位，夹紧气缸--------------------------------------
    while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品！！", Normal);
            machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品！！", Err);
            return -1;
        }
        Sleep(5);
    }

    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位！！", Normal);
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位！！", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧！！", Normal);
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧！！", Err);
            return -1;
        }
        Sleep(5);
    }
    Sleep(200);
    m_isRobotLoading.store(false);
    return 0;
}

//机器人下料函数
int rs_motion::runRobotUnLoading()
{
    //如果治具上没有料，就不用去取料，直接等料
    if (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        //重新取料
        return 0;
    }
    QElapsedTimer materialTimer;
    //---------------------------- 松开治具 --------------------------------------
    //机器人取料中
    m_isRobotLoading.store(true);
    setDO(LS_DO::ED_DO_PositionCyl1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Home) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未松开治具！！", Normal);
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未松开治具！！", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 0);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 0);
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampCyl1_Home) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Home) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未松开治具！！", Normal);
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未松开治具！！", Err);
            return -1;
        }
        Sleep(5);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    //---------------------------- 治具取料 机器人通讯交互及信号检测 --------------------------------------
  

    bool fixturePickDone = false;
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("取料前 fixturePickDone：" + QString::number(fixturePickDone), Normal);
    //允许取料
    MODBUS_TCP.writeIntCoil(134, 2, 1);
    Sleep(100);
    machineLog->write("等待机器人治具取料", Normal);
    //等待机器人治具取料完成
    materialTimer.start();
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("取料中 fixturePickDone：" + QString::number(fixturePickDone), Normal);
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
            machineLog->write("等待机器人治具取料：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
            machineLog->write("等待机器人治具取料：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //清空机器人治具取料信号
    MODBUS_TCP.writeIntCoil(134, 2, 0);
    MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
    machineLog->write("取料后 fixturePickDone：" + QString::number(fixturePickDone), Normal);
    materialTimer.start();
    while (fixturePickDone)
    {
        //清信号
        auto err = MODBUS_TCP.readRegisterInt(38);
        auto res = robotCheck(err);
        if (m_isEmergency.load() || res != 0)
            return -1;
        Sleep(5);
        MODBUS_TCP.readIntCoil(34, 2, fixturePickDone);
        Sleep(5);
        MODBUS_TCP.writeIntCoil(134, 2, 0);
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("等待机器人治具取料完成信号清空超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
            machineLog->write("等待机器人治具取料完成信号清空超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    machineLog->write("清信号 fixturePickDone：" + QString::number(fixturePickDone), Normal);
    //治具有料检测
    materialTimer.start();
    while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME_2) {
            machineLog->write("治具产品取料检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成检测到产品被取走", Normal);
            machineLog->write("治具产品取料检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成检测到产品被取走", Err);
            return -1;
        }
        Sleep(5);
    }
    machineLog->write("机器人治具取料完成...", Normal);
    //机器人取料完成
    m_isRobotLoading.store(false);
    return 0;
}

//机器人下料到皮带函数
int rs_motion::runRobotUnLoad_Belt(bool isNG)
{
    QElapsedTimer materialTimer;
    m_isRobotLoading.store(true);
    //测试
    //isNG = false;

    //结果NG或MES没通过，放NG
    if (isNG || !LSM->m_mesResponse_ED.state)
    {
        //检测NG皮带是否满料
        if (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1
            && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
            && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            //满料报警
            machineLog->write("NG皮带满料，请先取料！！", Normal);
            machineLog->write("NG皮带满料，请先取料！！", Err);
            while (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
            {
                //检测料有没有被拿开，拿开了就继续
                if (m_isEmergency.load())
                    return -1;
                Sleep(5);
            }
            //return -1;
        }
        //检测皮带是否需要往前运动
        if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            //右边和左边都有料，NG皮带往前走到都检测不到料
            setDO(LS_DO::ED_DO_NGBeltStart, 1);
            machineLog->write("启动NG皮带...", Normal);
            materialTimer.start();
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 || getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("启动" + QString::number(ERROR_TIME) + " 毫秒后NG皮带仍检测有料，请查看是否满料！！", Normal);
                    machineLog->write("启动" + QString::number(ERROR_TIME) + " 毫秒后NG皮带仍检测有料，请查看是否满料！！", Err);
                    setDO(LS_DO::ED_DO_NGBeltStart, 0);
                    return -1;
                }
                Sleep(5);
            }
            Sleep(100);
            setDO(LS_DO::ED_DO_NGBeltStart, 0);
            machineLog->write("停止NG皮带...", Normal);
        }
        //检测NG左信号
        if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
        {
            //放左边
            MODBUS_TCP.writeIntCoil(134, 4, 1);
            Sleep(200);
            machineLog->write("机器人放料左NG皮带...", Normal);
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
                    machineLog->write("等待机器人放料左NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
                    machineLog->write("等待机器人放料左NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //检测是否放料成功
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("NG皮带产品放料检测1（左）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品", Normal);
                    machineLog->write("NG皮带产品放料检测1（左）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品", Err);
                    return -1;
                }
                Sleep(5);
            }

            machineLog->write("机器人放料左NG完成...", Normal);
            MODBUS_TCP.writeIntCoil(134, 4, 0);

            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            materialTimer.start();
            while (ngBeltLeftPlaceDone)
            {
                //清信号
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
                //一直清料
                MODBUS_TCP.writeIntCoil(134, 4, 0);
                Sleep(5);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("等待机器人放料左NG皮带完成清信号超时：" + QString::number(ERROR_TIME_2), Normal);
                    machineLog->write("等待机器人放料左NG皮带完成清信号超时：" + QString::number(ERROR_TIME_2), Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //触发蜂鸣器
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
            //放右边
            MODBUS_TCP.writeIntCoil(134, 5, 1);
            Sleep(200);
            machineLog->write("机器人放料右NG皮带...", Normal);
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
                    machineLog->write("等待机器人放料右NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
                    machineLog->write("等待机器人放料右NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }
           
            //检测是否放料成功
            while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("NG皮带产品放料检测2（右）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品", Normal);
                    machineLog->write("NG皮带产品放料检测2（右）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品", Err);
                    return -1;
                }
                Sleep(5);
            }

            machineLog->write("机器人放料右NG完成...", Normal);
            MODBUS_TCP.writeIntCoil(134, 5, 0);

            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            materialTimer.start();
            while (ngBeltRightPlaceDone)
            {
                //清信号
                auto err = MODBUS_TCP.readRegisterInt(38);
                auto res = robotCheck(err);
                if (m_isEmergency.load() || res != 0)
                    return -1;
                MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
                //一直清料
                MODBUS_TCP.writeIntCoil(134, 5, 0);
                Sleep(5);
                if (materialTimer.elapsed() > ERROR_TIME_2) {
                    machineLog->write("等待机器人放料右NG皮带完成清信号超时：" + QString::number(ERROR_TIME_2), Normal);
                    machineLog->write("等待机器人放料右NG皮带完成清信号超时：" + QString::number(ERROR_TIME_2), Err);
                    m_isEmergency.store(true);
                    return -1;
                }
            }

            //触发蜂鸣器
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
        //检测出料皮带上是否有料
        if (getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1)
        {
            machineLog->write("出料皮带放料处有料，先启动出料皮带...", Normal);
            setDO(LS_DO::ED_DO_DischargeBeltStart, 1);
            materialTimer.start();
            while (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 0)
            {
                if (m_isEmergency.load())
                    return -1;
                //if (materialTimer.elapsed() > ERROR_TIME) {
                //    machineLog->write("启动" + QString::number(ERROR_TIME) + " 毫秒后出料皮带放料处仍检测有料，请查看皮带是否异常！！", Normal);
                //    setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
                //    return -1;
                //}
                Sleep(5);
            }
            Sleep(100);
            setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
        }
        //机器人放料出料皮带
        MODBUS_TCP.writeIntCoil(134, 3, 1);
        Sleep(100);
        machineLog->write("机器人放料出料皮带...", Normal);
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
                machineLog->write("等待机器人放料出料皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
                machineLog->write("等待机器人放料出料皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("机器人放料出料皮带完成...", Normal);
        MODBUS_TCP.writeIntCoil(134, 3, 0);
        //读机器人放料完成信号是否清理
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
                machineLog->write("等待机器人放料完成信号完成清理超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未清理", Normal);
                machineLog->write("等待机器人放料完成信号完成清理超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未清理", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        //Sleep(100);
        //检测是否放料成功
        while (getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("出料皮带产品放料检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未检测到产品", Normal);
                machineLog->write("出料皮带产品放料检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未检测到产品", Err);
                return -1;
            }
            Sleep(5);
        }
        if (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
        {
            //皮带到料区已经有料，不送料
            return 0;
        }
        //启动出料皮带
        machineLog->write("启动出料皮带...", Normal);
        setDO(LS_DO::ED_DO_DischargeBeltStart, 1);
        //Sleep(1000);
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("等待出料皮带产品到位检测：" + QString::number(ERROR_TIME) + " 毫秒后仍未到位！！", Normal);
                machineLog->write("等待出料皮带产品到位检测：" + QString::number(ERROR_TIME) + " 毫秒后仍未到位！！", Err);
                setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
                return -1;
            }
            Sleep(5);
        }
        Sleep(100);
        machineLog->write("停止出料皮带...", Normal);
        setDO(LS_DO::ED_DO_DischargeBeltStart, 0);
    }
    m_isRobotLoading.store(false);
    return 0;
}

//机器人放料到治具
int rs_motion::runRobotLoad_Tool()
{
    QElapsedTimer materialTimer;
    //如果手上没有料直接放料
    bool haveProduct = false;
    MODBUS_TCP.readIntCoil(34, 6, haveProduct);
    if (!haveProduct)
    {
        return 0;
    }
    
    //等待载具无料
    while (getDI(LS_DI::ED_DI_ProdDetect) == 1)
    {
        if (m_isEmergency.load())
            return -1;
        Sleep(5);
    }
    //---------------------------- 治具取料完成 机器人放料治具 --------------------------------------
    m_isRobotLoading.store(true);
    //给机器人发送允许放料
    MODBUS_TCP.writeIntCoil(134, 1, 1);
    //等待机器人放料
    machineLog->write("等待机器人放料", Normal);
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
            machineLog->write("等待机器人放料治具：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
            machineLog->write("等待机器人放料治具：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }
    //关闭放料信号
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
            machineLog->write("等待机器人放料治具完成信号清零超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
            machineLog->write("等待机器人放料治具完成信号清零超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
            m_isEmergency.store(true);
            return -1;
        }
    }

    //---------------------------- 放料完成 有料检测,打开定位，夹紧气缸--------------------------------------
    while (getDI(LS_DI::ED_DI_ProdDetect) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品", Normal);
            machineLog->write("治具产品检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成检测到产品", Err);
            return -1;
        }
        Sleep(5);
    }

    setDO(LS_DO::ED_DO_PositionCyl1, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampPosCyl1_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位", Normal);
            machineLog->write("治具定位气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具定位", Err);
            return -1;
        }
        Sleep(5);
    }
    setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
    setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
    materialTimer.start();  // 开始计时
    while (getDI(LS_DI::ED_DI_ClampCyl1_Work) == 0 || getDI(LS_DI::ED_DI_ClampCyl2_Work) == 0)
    {
        if (m_isEmergency.load())
            return -1;
        if (materialTimer.elapsed() > ERROR_TIME) {
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧", Normal);
            machineLog->write("治具夹紧气缸检测超时：" + QString::number(ERROR_TIME) + " 毫秒内未完成治具夹紧", Err);
            return -1;
        }
        Sleep(5);
    }
    Sleep(200);
    m_isRobotLoading.store(false);
    return 0;
}

//机器人清料
int rs_motion::runRobotClearHand()
{
    //统一放NG皮带
    QElapsedTimer materialTimer;
    //检测NG皮带是否满料
    if (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1
        && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
        && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
    {
        //满料报警
        machineLog->write("NG皮带满料，请先取料！！", Normal);
        while (getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
        {
            //检测料有没有被拿开，拿开了就继续
            if (m_isEmergency.load())
                return -1;
            Sleep(5);
        }
        //return -1;
    }
    //检测皮带是否需要往前运动
    if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 && getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
    {
        //右边和左边都有料，NG皮带往前走到都检测不到料
        setDO(LS_DO::ED_DO_NGBeltStart, 1);
        machineLog->write("启动NG皮带...", Normal);
        materialTimer.start();
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1 || getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("启动" + QString::number(ERROR_TIME) + " 毫秒后NG皮带仍检测有料，请查看是否满料！！", Normal);
                machineLog->write("启动" + QString::number(ERROR_TIME) + " 毫秒后NG皮带仍检测有料，请查看是否满料！！", Err);
                setDO(LS_DO::ED_DO_NGBeltStart, 0);
                return -1;
            }
            Sleep(5);
        }
        Sleep(100);
        setDO(LS_DO::ED_DO_NGBeltStart, 0);
        machineLog->write("停止NG皮带...", Normal);
    }
    //检测NG左信号
    if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
    {
        //放左边
        MODBUS_TCP.writeIntCoil(134, 4, 1);
        machineLog->write("机器人放料左NG皮带...", Normal);
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
                machineLog->write("等待机器人放料左NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
                machineLog->write("等待机器人放料左NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("机器人放料左NG完成...", Normal);
        MODBUS_TCP.writeIntCoil(134, 4, 0);

        MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
        materialTimer.start();
        while (ngBeltLeftPlaceDone)
        {
            //清信号
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 4, ngBeltLeftPlaceDone);
            //一直清料
            MODBUS_TCP.writeIntCoil(134, 4, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("等待机器人放料左NG皮带清信号超时：" + QString::number(ERROR_TIME_2), Normal);
                machineLog->write("等待机器人放料左NG皮带清信号超时：" + QString::number(ERROR_TIME_2), Err);
                m_isEmergency.store(true);
                return -1;
            }
        }

        //检测是否放料成功
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("NG皮带产品放料检测1（左）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品！！", Normal);
                machineLog->write("NG皮带产品放料检测1（左）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品！！", Err);
                m_isEmergency.store(true);
                return -1;
            }
            Sleep(5);
        }
    }
    else if (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
    {
        //放右边
        MODBUS_TCP.writeIntCoil(134, 5, 1);
        machineLog->write("机器人放料右NG皮带...", Normal);
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
                machineLog->write("等待机器人放料右NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Normal);
                machineLog->write("等待机器人放料右NG皮带：" + QString::number(ERROR_TIME_2) + " 毫秒内未完成取料", Err);
                m_isEmergency.store(true);
                return -1;
            }
        }
        machineLog->write("机器人放料右NG完成...", Normal);
        MODBUS_TCP.writeIntCoil(134, 5, 0);

        MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
        materialTimer.start();
        while (ngBeltRightPlaceDone)
        {
            //清信号
            int err = MODBUS_TCP.readRegisterInt(38);
            auto res = robotCheck(err);
            if (m_isEmergency.load() || res != 0)
                return -1;
            MODBUS_TCP.readIntCoil(34, 5, ngBeltRightPlaceDone);
            //一直清料
            MODBUS_TCP.writeIntCoil(134, 5, 0);
            Sleep(5);
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("等待机器人放料右NG皮带清信号超时：" + QString::number(ERROR_TIME_2), Normal);
                machineLog->write("等待机器人放料右NG皮带清信号超时：" + QString::number(ERROR_TIME_2), Err);
                m_isEmergency.store(true);
                return -1;
            }
        }

        //检测是否放料成功
        while (getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 0)
        {
            if (m_isEmergency.load())
                return -1;
            if (materialTimer.elapsed() > ERROR_TIME_2) {
                machineLog->write("NG皮带产品放料检测2（右）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品！！", Normal);
                machineLog->write("NG皮带产品放料检测2（右）检测超时：" + QString::number(ERROR_TIME_2) + " 毫秒内未检测到产品！！", Err);
                return -1;
            }
            Sleep(5);
        }
    }
    return 0;
}

//清理及初始化缓存变量
void rs_motion::initAllVariable()
{
    //清除结果
    m_runResult.clear();
    //清空已经记录的视觉配方
    m_visionFormula_2D.clear();
    m_visionFormula_3D.clear();
    //清空已经记录2D的结果
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
    // 清空3D点云内容
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
    //清理需要合并的图片
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
    //清理已记录的导出数据
    for (auto& pair : m_ResultToCsv) {
        pair.second.clear(); 
    }
}

//红色灯闪烁
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
        //关
        setThreeColorLight(2, true);
    }
    else
    {
        //开
        setThreeColorLight(2);
    }
}

// 初始化CSV结构参数
void rs_motion::initCsvPara()
{
    m_ResultToCsv["name"] = "";
    m_ResultToCsv["SN"] = "";
    m_ResultToCsv["产品码"] = "";
    m_ResultToCsv["客户码"] = "";
    m_ResultToCsv["产品码重复码"] = "";
    m_ResultToCsv["客户码重复码"] = "";
    m_ResultToCsv["其他码1"] = "";
    m_ResultToCsv["其他码2"] = "";
    m_ResultToCsv["其他码3"] = "";
    m_ResultToCsv["其他码4"] = "";
    m_ResultToCsv["2D结果"] = "";
    m_ResultToCsv["3D结果"] = "";
    m_ResultToCsv["螺丝检测1"] = "";
    m_ResultToCsv["螺丝检测2"] = "";
    m_ResultToCsv["螺丝检测3"] = "";
    m_ResultToCsv["螺丝检测4"] = "";
    m_ResultToCsv["标签字符检测"] = "";
    m_ResultToCsv["卡扣检测"] = "";
    m_ResultToCsv["防水透气膜检测"] = "";
    m_ResultToCsv["密封胶检测"] = "";
    m_ResultToCsv["圆测距检测1"] = "";
    m_ResultToCsv["圆测距检测2"] = "";
    m_ResultToCsv["圆测距检测3"] = "";
    m_ResultToCsv["圆测距检测4"] = "";
    m_ResultToCsv["创建时间"] = "";
    m_ResultToCsv["更新时间"] = "";
    m_ResultToCsv["图片路径"] = "";
    m_ResultToCsv["生产人"] = "";
    m_ResultToCsv["最终结果"] = "";
    m_ResultToCsv["工单"] = "";

    m_csvOrder.push_back("name");
    m_csvOrder.push_back("工单");
    m_csvOrder.push_back("SN");
    m_csvOrder.push_back("产品码");
    m_csvOrder.push_back("客户码");
    m_csvOrder.push_back("产品码重复码");
    m_csvOrder.push_back("客户码重复码");
    m_csvOrder.push_back("其他码1");
    m_csvOrder.push_back("其他码2");
    m_csvOrder.push_back("其他码3");
    m_csvOrder.push_back("其他码4");
    m_csvOrder.push_back("2D结果");
    m_csvOrder.push_back("3D结果");
    m_csvOrder.push_back("螺丝检测1");
    m_csvOrder.push_back("螺丝检测2");
    m_csvOrder.push_back("螺丝检测3");
    m_csvOrder.push_back("螺丝检测4");
    m_csvOrder.push_back("标签字符检测");
    m_csvOrder.push_back("卡扣检测");
    m_csvOrder.push_back("防水透气膜检测");
    m_csvOrder.push_back("密封胶检测");
    m_csvOrder.push_back("圆测距检测1");
    m_csvOrder.push_back("圆测距检测2");
    m_csvOrder.push_back("圆测距检测3");
    m_csvOrder.push_back("圆测距检测4");
    m_csvOrder.push_back("创建时间");
    m_csvOrder.push_back("更新时间");
    m_csvOrder.push_back("图片路径");
    m_csvOrder.push_back("生产人");
    m_csvOrder.push_back("最终结果");
}


// 通过设备检测MES流程
void rs_motion::checkMesFunction()
{
    if (m_version == EdPin)
    {
        //易鼎丰 自动导出csv
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
            //优先用测试的SN
            if (!testSN.isEmpty())
            {
                sn_tp = testSN;
                m_ResultToCsv["SN"] = testSN;
            }

            m_ResultToCsv["name"] = m_forMulaName;
            //m_ResultToCsv["SN"] = sn_tp;
            m_ResultToCsv["创建时间"] = m_runStartTime;
            m_ResultToCsv["更新时间"] = m_runStartTime;
            m_ResultToCsv["图片路径"] = m_resultImgPath;
            m_ResultToCsv["最终结果"] = m_runResult.isNG ? "NG" : "OK";
            m_ResultToCsv["2D结果"] = m_runResult.isNG_2D ? "NG" : "OK";
            m_ResultToCsv["3D结果"] = m_runResult.isNG_3D ? "NG" : "OK";
            m_ResultToCsv["生产人"] = m_productPara.UserAccout;
            m_ResultToCsv["工单"] = m_productPara.WorkOrderCode;
            QString filePath = m_Paramer.mesCSV + "/" + sn_tp + "_" + m_runStartTime;
            //导出到CSV
            m_functionTool.saveCsvInThread(filePath, m_ResultToCsv, m_csvOrder);

            //----------------------------加mes功能--------------------------
            if (!LSM->notMes && !LSM->m_Paramer.mesUrl.isEmpty())
            {
                QString productSN = m_ResultToCsv["产品码"];
                if (sn_tp.isEmpty())
                {
                    //直接NG
                    m_mesResponse_ED.state = false;
                    m_mesResponse_ED.description = "二维码识别未查找到SN码";
                    m_runResult.isNG = true;
                    machineLog->write("二维码识别未查找到SN码，无法上传MES", Mes);
                }
                else
                {
                    // 准备请求数据
                    MesCheckInRequest_ED request;
                    request.sn = sn_tp;
                    request.productSn = productSN;
                    //request.IP = "192.168.1.100";
                    //request.Slot = "A3";

                    // 发送请求
                    rs_FunctionTool tool;
                    m_mesResponse_ED = tool.mes_check_in_ED(LSM->m_Paramer.mesUrl, request);

                    // 处理响应
                    if (m_mesResponse_ED.state) {
                        machineLog->write("MES校验通过", Mes);

                        // 将m_ResultToCsv映射到结构体
                        MesCheckInFullRequest_ED mesRequest = tool.mapToMesRequest(m_ResultToCsv);
                        mesRequest.WorkOrderCode = m_productPara.WorkOrderCode;
                        // 调用过站函数
                        MesCheckInResponse_ED response = tool.mes_full_check_in(LSM->m_Paramer.mesUrl, mesRequest);

                        if (response.state) {
                            machineLog->write("MES过站成功", Mes);
                            return;
                        }
                        else {
                            machineLog->write("MES过站失败：" + m_mesResponse_ED.description, Mes);
                            m_runResult.isNG = true;
                            return;
                        }
                    }
                    else {
                        machineLog->write("MES校验失败：" + m_mesResponse_ED.description, Mes);
                        m_runResult.isNG = true;
                        return;
                    }
                }
            }

        }
    }
}

// 金脉pin标机动作
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


    //所有轴设置速度
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

    //判断Z轴移动方向
    if (TargetPZ < m_Axis[axisZ].minTravel || TargetPZ > m_Axis[axisZ].maxTravel)
    {
        machineLog->write("示教位置超越Z轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越Z轴软限位，动作错误！！", Err);
        return -1;
    }
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //先执行Z轴收回动作
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //判断Z1轴移动方向
    if (TargetPZ1 < m_Axis[axisZ1].minTravel || TargetPZ1 > m_Axis[axisZ1].maxTravel)
    {
        machineLog->write("示教位置超越Z1轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越Z1轴软限位，动作错误！！", Err);
        return -1;
    }
    bool isPostiveMove_Z1 = isMovePostive(axisZ1, TargetPZ1);
    //先执行Z1轴收回动作
    if (!isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("执行Z1轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    //其他轴异步执行
    if (TargetPU < m_Axis[axisU].minTravel || TargetPU > m_Axis[axisU].maxTravel)
    {
        machineLog->write("示教位置超越U轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越U轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("触发执行U轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX < m_Axis[axisX].minTravel || TargetPX > m_Axis[axisX].maxTravel)
    {
        machineLog->write("示教位置超越X轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越X轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX1 < m_Axis[axisX1].minTravel || TargetPX1 > m_Axis[axisX1].maxTravel)
    {
        machineLog->write("示教位置超越X1轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越X1轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisX1, TargetPX1);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return -1;
    }
    if (TargetPY < m_Axis[axisY].minTravel || TargetPY > m_Axis[axisY].maxTravel)
    {
        machineLog->write("示教位置超越Y轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越Y轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPU1 < m_Axis[axisU1].minTravel || TargetPU1 > m_Axis[axisU1].maxTravel)
    {
        machineLog->write("示教位置超越U1轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越U1轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisU1, TargetPU1);
    if (rtn != 0)
    {
        machineLog->write("触发执行U1轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //等待所有轴到位
    while (isRuning(axisX)
        || isRuning(axisX1)
        || isRuning(axisY)
        || isRuning(axisU1)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("等待所有轴到位错误！！", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60秒超时做超时预警   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("轴到位超时！！", Normal);
            break;
        }
    }

    //后执行Z轴伸出到位动作
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }
    if (isPostiveMove_Z1)
    {
        rtn = AxisPmoveAsync(axisZ1, TargetPZ1);
        if (rtn != 0)
        {
            machineLog->write("执行Z1轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    return 0;
}

// 金脉焊缝检测机动作
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

    //所有轴设置速度
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

    //判断Z轴移动方向
    if (TargetPZ < m_Axis[axisZ].minTravel || TargetPZ > m_Axis[axisZ].maxTravel)
    {
        machineLog->write("示教位置超越Z轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越Z轴软限位，动作错误！！", Err);
        return -1;
    }
    bool isPostiveMove_Z = isMovePostive(axisZ, TargetPZ);
    //先执行Z轴收回动作
    if (!isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴收回动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
        //WriteAxisSpeedModel(axisZ, (SpeedModeEnum)item.SpeedModel);
    }

    //其他轴异步执行
    if (TargetPU < m_Axis[axisU].minTravel || TargetPU > m_Axis[axisU].maxTravel)
    {
        machineLog->write("示教位置超越U轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越U轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisU, TargetPU);
    if (rtn != 0)
    {
        machineLog->write("触发执行U轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }
    if (TargetPX < m_Axis[axisX].minTravel || TargetPX > m_Axis[axisX].maxTravel)
    {
        machineLog->write("示教位置超越X轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越X轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisX, TargetPX);
    if (rtn != 0)
    {
        machineLog->write("触发执行X轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }

    if (TargetPY < m_Axis[axisY].minTravel || TargetPY > m_Axis[axisY].maxTravel)
    {
        machineLog->write("示教位置超越Y轴软限位，动作错误！！", Normal);
        machineLog->write("示教位置超越Y轴软限位，动作错误！！", Err);
        return -1;
    }
    rtn = AxisPmove(axisY, TargetPY);
    if (rtn != 0)
    {
        machineLog->write("触发执行Y轴动作错误！！错误码 ：" + QString::number(rtn), Normal);
        return rtn;
    }

    QTime time = QTime::currentTime();
    //等待所有轴到位
    while (isRuning(axisX)
        || isRuning(axisY)
        || isRuning(axisU))
    {
        if (m_isEmergency.load())
        {
            machineLog->write("等待所有轴到位错误！！", Normal);
            return -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // 60秒超时做超时预警   
        if (time.secsTo(QTime::currentTime()) > 60)
        {
            machineLog->write("轴到位超时！！", Normal);
            break;
        }
    }

    //后执行Z轴伸出到位动作
    if (isPostiveMove_Z)
    {
        rtn = AxisPmoveAsync(axisZ, TargetPZ);
        if (rtn != 0)
        {
            machineLog->write("执行Z轴伸出动作错误！！错误码 ：" + QString::number(rtn), Normal);
            return rtn;
        }
    }

    return 0;
}

