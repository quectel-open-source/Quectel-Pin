#pragma once
#pragma execution_character_set("utf-8")
#include <iostream>
#include <map>
#include <QObject>
#include <unordered_map>
#include "rs.h"
#include <QtCore/qstring.h>
#include <climits> 
#include "MvCamera.h"
#include "Camera3D.h"
#include "datas.h"
#include "VisionTools.h"
#include "ThreadGuard.h"
#include "QtcpClass.h"
#include "dataDetect.h"
#include "ModbusTcpClient.h"
#include "rs_FunctionTool.h"

#define LSM rs_motion::getInstance()

class rs_motion : public QObject
{
    Q_OBJECT
private:
	rs_motion() = default;
	static rs_motion* motion;

public:
	rs_motion(const rs_motion& tp) = delete;
	rs_motion& operator=(const rs_motion& tp) = delete;
	static rs_motion* getInstance()
	{
		//if (motion == nullptr)
		//{
		//	motion = new rs_motion();
		//}
		//改为饿汉，可能后面涉及多线程
		return motion;
	};

    void destroyInstance();

	//参数初始化
	void init();
	//控制卡初始化
	bool motionInit();
	//参数保存
	void saveMotionConfig();
	//参数读取
	void loadMotionConfig();
	//canIO初始化
	bool CANInit(int CardId, int NodeId);
	bool ReadAxisServeOn(int CardId, int AxisId);
	short WriteAxisServeOn(int CardId, int AxisId, bool isEnable);
    short WriteAxisServeOn(QString axis, bool isEnable);
    void WriteAxis(QString AxisName);
    void WriteAxis(int CardId, int AxisId,
        double StartVel, double RunVel,
        double AccTime, double DccTime, double StopVel,
        double STime);
	void WriteAxisParm(QString Axis);
    void MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel);
    void InitAxes();
    void getAllDI();
    int getDI(QString name);
    void getAllDO();
    int getDO(QString name);
    void setDO(QString name,int value);
    void motionClose();
    void AxisStop(QString axis);
    void AxisStop(int CardId, int AxisId);
    void AxisEmgStop(int CardId);
    void CANClose(int CardId);
    //轴名回零
    int AxisHome(QString axis);
    //轴名回零(阻塞)
    int AxisHomeAsync(QString axis);
    int GoHome(int CardId, int AxisId);
    void AxisClearPosition(int CardId, int AxisId);
    bool IsRuning(int CardId, int AxisId);
    void AxisStopAll();
    //speed 用于1000版本板卡,其他版本通过setSpeed写到轴参数中
    int VMove(int CardId, int AxisId, bool pdir = true, double speed = 0);
    //speed 用于1000版本板卡,其他版本通过setSpeed写到轴参数中
    int AxisVmove(QString axis, bool postive, double speed = 0);
    void WriteAxisRunVel(QString axis, double runvel);

    double ReadPosition(int CardId, int AxisId);
    double ReadAxisP(QString axis);
    void ReadAllAxisP();
    void WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed);
    void WriteAxisSpeedModel(QString axis, SpeedModeEnum model);
    void ResetSetAxisParm(QString axis);
    bool ReadServOn(QString axis);
    //设置速度
    int setSpeed(QString axisName, double speed);
    // 读取轴是否报警状态
    bool isAxisErc(QString axis);
    // 读取轴报警端口电平
    bool isAxisErc(int CardId, int AxisId);
    // 清除轴报警状态
    void AxisClear(QString axis);
    void AxisClear(int CardId, int AxisId);
    // 定长绝对运动
    int PMove_abs(int CardId, int AxisId, double value);
    // 定长绝对运动（轴） (speed 用于1000版本板卡)
    int PMove_abs(QString axis, double value,double speed = 0);
    // 定长相对运动
    void PMove_rel(int CardId, int AxisId, double value);
    // 定长相对运动（轴）
    void PMove_rel(QString axis, double value);
    // 轴定长绝对运动，阻塞执行，待轴到位后返回 (speed 用于1000版本板卡)
    int AxisPmoveAsync(QString axis, double value, double speed = 0);
    // 等待轴复位到初始位置后返回(阻塞)
    int AxisInitPAsync(QString axis);
    // 轴复位到初始位置
    int AxisInitP(QString axis);
    // 复位动作
    int TaskReset();
    int TaskReset_LS();
    int TaskReset_ED();
    int TaskReset_BT();
    int TaskReset_JMPin();
    int TaskReset_JMHan();
    // 复位工位指示灯
    void ResetStationLight(bool isstationL = true);
    // 进入待料状态
    int TaskGoWating(ENUMSTATION stations);
    bool isInitP(QString axis);
    bool isRuning(QString axis);
    //是否到达原点
    bool isHomeP(QString axis);
    // 保存流程配方
    void saveFormulaToFile();
    // 加载流程配方
    void loadFormulaFromFile();
    // 整体动作运行流程
    int runAction(QList<PointItemConfig> listProcess);
    //前置流程 1. 3d点位执行前消抖，延迟 ,开启触发和脉冲
    int ItemPreProcess(PointItemConfig item);
    QString DetectPulsAxis(PointItemConfig item);
    //判断轴运动是否正向
    bool isMovePostive(QString axis, double target);
    // ItemPart 轴动作
    int ItemMovingProcess(PointItemConfig item);
    // 轴定长绝对运动 (speed 用于1000版本板卡)
    int AxisPmove(QString axis, double value,double speed = 0);
    // ItemPart 后置流程
    //  1. 3d点位执行后保持，取消
    //  2. 2d点位执行后，到位消抖 延迟 保持灯光
    int ItemLastProcess(PointItemConfig item);
    // 2D相机拍照流程
    int catPhoto2D(std::string cameraName, std::shared_ptr<CMvCamera>& camera,int exposureTime = 0);
    // 获取最旧的文件路径
    QString getOldestFileInDirectory(const QString& directoryPath);
    // 清空文件夹
    bool deleteAllFilesInDirectory(const QString& directoryPath);
    // pin结果展示
    int pinShowResult(Run_Result& reslut);
    //3D设备初始化
    void init3dCamera();
    // 处理 3D 点云的逻辑函数
    void process3Didentify(pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPoint, PointItemConfig item);
    // 3D结果融合
    void summary3dResult();
    // 处理 2D 识别的逻辑函数
    void process2Didentify(HObject img, PointItemConfig item);
    // 销毁相机
    void destroyCamera();
    // 双轨pin标机动作
    int pin2MotionAction(PointItemConfig item);
    // 立昇pin标机动作
    int pinLsAction(PointItemConfig item);
    // 易鼎丰pin标机动作
    int pinEdAction(PointItemConfig item);
    // 金脉pin标机动作
    int pinJmAction(PointItemConfig item);
    // 金脉焊缝检测机动作
    int hanJmAction(PointItemConfig item);
    // 初始化轴，IO等
    void initAxisAndIO();
    // 倍速链控制及上料监控
    int feedingAction();
    // 倍速链控制及下料监控
    int unloadingAction();
    // 下层倍速链控制
    int downFeedingAction();
    // 一维码识别
    int barCodeIdentify(PointItemConfig item);
    // 二维码识别
    int QrCodeIdentify(PointItemConfig item);
    // 模板匹配
    int templateIdentify(PointItemConfig item);
    // Blob分析
    int blobIdentify(PointItemConfig item);
    // 圆检测
    int circleIdentify(PointItemConfig item);
    // 圆测距
    int circleMeasure(PointItemConfig item);
    //关DO
    void closeDO();
    //三色灯指示灯 0：绿灯，1：黄灯，2：红灯
    void setThreeColorLight(int color,bool isClose = false);
    //按钮指示灯 0：启动，1：复位，2：停止，3：蜂鸣器，4：照明
    void setButtonLight(int type,int val);
    //接收检查SN
    SnCheckResponse check_sn_process(const QString& url, const SnCheckRequest& req);
    //过站检查
    SnCheckResponse post_result_process(const QString& url, const SnResultRequest& req);
    //机器人首次放料到载具函数
    int runRobotCheck();
    //机器人下料函数
    int runRobotUnLoading();
    //实时监控机器人皮带取料
    int runRobotLoad_Belt();
    //机器人报警检测
    int robotCheck(int err);
    //机器人下料到皮带函数
    int runRobotUnLoad_Belt(bool isNG);
    //机器人放料到治具
    int runRobotLoad_Tool();
    //机器人清料
    int runRobotClearHand();
    //清理及初始化缓存变量
    void initAllVariable();
    // 初始化CSV结构参数
    void initCsvPara();
    // 通过设备检测MES流程
    void checkMesFunction();
public:
    //软件版本
    int m_version{ ENUM_VERSION::TwoPin };
    //板卡版本
    int m_motionVersion{ ENUM_MOTIONVERSION::LS_Pulse };
    //板卡状态
    bool m_cardInitStatus{ false };
    //动作类型
    int m_triggerMotionTpyes{ 0 };

    //可以运行标志位 （不开的话检测来料了也不触发运行）
    bool m_isRunOK = false;
    //运行标志位
    std::atomic<bool> m_isStart = false;
    std::atomic<bool> m_isLoop = false;
    //回零标志位
    std::atomic<bool> m_isHomming = false;
    //回零完成标志位
    std::atomic<bool> m_isHomed = false;
    //暂停标志位
    bool m_isPause = false;
    //停止标志位(停止机器动作，来料了也不能运行)
    std::atomic<bool> m_isRunStop = false;
    //取消所有动作标志位 （急停，停止）
    std::atomic<bool> m_isEmergency = false;
    //使能标志位
    bool isServeOn = false;
    //单步运行标志位
    std::atomic<bool> m_isOneStep = false;
    //相机类
    //std::shared_ptr<CMvCamera> m_pcMyCamera = nullptr;
    //std::unique_ptr<Camera3D> m_pcMyCamera_3D = nullptr;
    std::shared_ptr<VisionTools> m_pcMyCamera_3D = nullptr;

    //相机名对应的相机实例字典
    std::unordered_map<QString, std::shared_ptr<CMvCamera>> m_cameraMap;

    //配方信息
    struct Formula_Config
    {
        //int id = -1;
        //QString name = "";
        QList<PointItemConfig> ListProcess_L;
        QList<PointItemConfig> ListProcess_R;
    };
    //配方列表
    std::unordered_map<QString,Formula_Config> m_Formula;
    //需要运行的步骤列表
    QList<PointItemConfig> m_runListProcess_L;
    QList<PointItemConfig> m_runListProcess_R;
    //运行的视觉配方顺序列表
    QList<QString> m_visionFormula_2D;
    QList<QString> m_visionFormula_3D;
    //执行步骤索引
    int m_showStepIndex_L = 0;
    int m_showStepIndex_R = 0;
    //pin 2D检测结果
    QList<std::pair<ResultParamsStruct_Pin, PointItemConfig>> m_pinResult2D;
    //pin 3D检测结果
    QList<std::pair<ResultParamsStruct_Pin3DMeasure, PointItemConfig>> m_pinResult3D;
    // 各个点云名的3D点云(不允许重复命名点云却内容不同)
    std::unordered_map< QString,pcl::PointCloud<pcl::PointXYZ>::Ptr> m_cloud_ori_map;
    // 各个2D照片
    std::unordered_map< QString, HObject> m_photo_map;
    //相机名 QString：工位中文名；std::string MVS相机名
    std::unordered_map<QString, std::string> m_cameraName;
    //一维码检测结果
    QList<std::pair<ResultParamsStruct_BarCode, PointItemConfig>> m_barCodeResult;
    //二维码检测结果
    QList<std::pair<ResultParamsStruct_Code2d, PointItemConfig>> m_qrCodeResult;
    //模板匹配检测结果
    QList<std::pair<ResultParamsStruct_Template, PointItemConfig>> m_TemplateResult;
    //blob检测结果
    QList<std::pair<ResultParamsStruct_Blob, PointItemConfig>> m_BolbResult;
    //圆检测结果
    QList<std::pair<ResultParamsStruct_Circle, PointItemConfig>> m_CircleResult;
    //mes 过站前检查
    SnCheckRequest m_snReq;
    //mes 过站信息
    SnResultRequest m_snResult;
    //扫码获取的信息
    QString ls_tcpMess = "";
    QMutex m_networkMutex;
    //需要和并结果的图片
    std::unordered_map<QString, Vision_Message> m_resultMergingImg;
    bool inDI = false;
    bool outDI = false;
    bool notMes = false;
    bool notQR = false;
    bool m_notDoor = false;
    bool m_notRobot = false;
    QString testSN = "";
    QString m_forMulaName = "";
    //易鼎丰机器人读取的变量结构体
    ED_Robot_Connect m_RobotModbus;
    //运行结果
    Run_Result m_runResult;
    //流程开始时间
    QString m_runStartTime;
    //机器人取料锁
    std::mutex m_robotLoadingMutex;  // 互斥锁
    std::atomic<bool> m_robotLoadingRunning{ false };  // 原子状态标志（可选）
    //各轴回零标志
    AXIS_HOME m_axisHomeState;
    //机器人放料或取料中
    std::atomic<bool> m_isRobotLoading = false;
    //结果图路径
    QString m_resultImgPath;
    //原图路径
    QString m_originalImgPath;
    //机器人皮带进料报警标志位
    std::atomic<bool> m_isLoadErr = false;
    //红色指示灯闪烁
    QTimer m_redLightFlashing;
    //易鼎丰 mes 进站前检查返回信息
    MesCheckInResponse_ED m_mesResponse_ED;
    //检测后导出的数据(易鼎丰提出)
    std::unordered_map<QString, QString> m_ResultToCsv;
    //导出的表头顺序
    std::vector<QString> m_csvOrder;
    //函数工具类
    rs_FunctionTool m_functionTool;
    //SN号
    QString m_requestSn;
    //视觉检测锁
    std::mutex m_visionMutex;
    //保存原图
    bool m_saveImageEnabled;
    //重大报错代码
    QString m_errCode;
    //配方和机器人方案对应(易鼎丰提出)
    std::unordered_map<QString, int> m_formulaToRobot;

	//轴配置属性
    struct Axis_Config
    {
        // --- 基础参数（需要保存和读取） ---
        // 卡号
        int card = 0;
        // 轴号
        int axisNum = -1;
        // 初始位
        double initPos = 0.0;
        // 最小行程
        double minTravel = 0.0;
        // 最大行程
        double maxTravel = 0.0;
        // 回零方向
        int homeDir = 0;
        // 脉冲当量
        double unit = 0.0;
        // 最小速
        double minSpeed = 0.0;
        // 中速
        double midSpeed = 0.0;
        // 最大速
        double maxSpeed = 0.0;
        // 加速时间
        double accTime = 0.0;
        // 减速时间
        double decTime = 0.0;
        // S 时间
        double sTime = 0.0;
        // 急停 IO
        int stopIO = -1;

        // --- 补充参数（可能需要保存和读取，但是之前运控软件参数页面没有，先不管） ---
        // 轴类型 (0: 虚拟轴, 1: 脉冲轴, 2: 预留, 3: 编码轴, 4: 脉冲+EZ)
        int axisType = 0;
        // 运动方式 (0: 相对运动, 1: 绝对运动)
        int moveModel = 0;
        // 停止速度
        double stopVel = 0.0;

        // 回零模式
        unsigned short homeModel = SpeedModeEnum::Low;
        // 回零速度
        double homeVel = 0.0;
        // 回零爬行速度
        double homeCreepVel = 0.0;

        // 原点信号通道
        int orgChannel = -1;
        // 原点信号有效电平 (0: 低电平, 1: 高电平)
        int orgValidLevel = 1;

        // 是否激活正负硬限位
        bool isActiveHardEl = true;

        // 正向硬限位通道
        int elpHardChannel = -1;
        // 正向硬限位有效电平 (0: 低电平, 1: 高电平)
        int elpValidLevel = 0;
        // 正向软限位设定值
        //int elpSoftValue = std::numeric_limits<int>::max();
        int elpSoftValue = INT_MAX;

        // 负向硬限位通道
        int elnHardChannel = -1;
        // 负向硬限位有效电平 (0: 低电平, 1: 高电平)
        int elnValidLevel = 1;
        // 负向软限位设定值
        //int elnSoftValue = std::numeric_limits<int>::min();
        int elnSoftValue = INT_MIN;

        // 伺服急停通道有效电平 (0: 低电平, 1: 高电平)
        int emgValidLevel = 0;

        // --- 补充参数（仅程序运行时使用，不需要保存和读取） ---
        // 当前轴位置
        double position = 0;
        // 脉冲模式设定值 (0-3: 脉冲+方向, 4-7: 双脉冲)
        int plusModel = 0;

        // --- 说明 ---
        // 上述参数中，未明确需要保存和读取的，放置在“仅程序运行时使用”的注释下
    };


	//轴参数
	std::unordered_map<QString, Axis_Config> m_Axis;

	//DI配置属性
	struct DI_Config
	{
		//卡号
		int card = 0;
		// CAN 节点
		int can = -1;
		//通道
		int channel = 0;
		//状态
		int state = 0;
	};

	//DI功能对应的板卡配置
	std::unordered_map<QString, DI_Config> m_DI;

	//DO配置属性
	struct DO_Config
	{
		//卡号
		int card = 0;
		// CAN 节点
		int can = -1;
		//通道
		int channel = 0;
        //有效电平
        int level = 0;
		//状态
		int state = 1;
	};

	//DI功能对应的板卡配置
	std::unordered_map<QString, DO_Config> m_DO;

    struct DeviceMoveit_Config
    {
        //------------------------------------ 2D
        // 2D消抖时长
        int BuffetTime2D = 500;
        // 2D延迟触发时长
        int DelayTriggerTime2D = 20;
        // 2D光源保持时长/3D脉冲保持时长
        int HoldTime2D = 20;
        //------------------------------------ 3D
        // 3D消抖时长
        int BuffetTime3D = 500;
        // 3D延迟触发时长
        int DelayTriggerTime3D = 20;
        // 3D光源保持时长/3D脉冲保持时长
        int HoldTime3D = 20;
        int ScanRow = 2000;
        //------------------------------------ 取料
        //吸料延时
        int suctionTime = 500;
        //------------------------------------ 放料
        //放料延时
        int unLoadTime = 500;
        //------------------------------------ 通讯
        //mes url
        QString mesUrl = "";
        //ModbusTcp 地址
        QString modbusTcpIp = "";
        //mes csv
        QString mesCSV = "";
        //mes ngPicPath
        QString ngPicPath = "";
    };
    //参数配置
    DeviceMoveit_Config m_Paramer;

    //// 功能对应的相机
    //struct Camera_Name
    //{
    //    std::string pin2D = "";
    //    //易鼎丰域上相机
    //    std::string ed_Up = "";
    //    //易鼎丰域下相机
    //    std::string ed_Down = "";
    //};

    //Camera_Name m_cameraName;

    // 配方生产参数
    struct ProductParameter
    {
        //已生产数量
        int ProductCount = 0;
        //NG数量
        int NGCount = 0;
        //OK数量
        int OKCount = 0;
        //直通率
        double FirstPassRate = 0;
        //用户名
        QString UserAccout = "";
        //工单
        QString WorkOrderCode = "";
    };

    ProductParameter m_productPara;

signals:
    void showPin_signal(ResultParamsStruct_Pin result, PointItemConfig config);    //单独2Dpin针
    void showPin2Dand3D_signal(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D和3Dpin针
    void showPin3D_signal(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //单独3Dpin针
    void updateStepIndex_signal();
    //刷新显示结果数据
    void upDataProductPara_signal(Run_Result runResult);
    //视觉功能的纯结果显示视图
    //void showVision_signal(HObject img,bool isNG);
    void showVision_signal(Vision_Message message);
    //清除图片展示
    void clearWindow_signal();
    //单纯显示图片
    void showImage_signal(QImage img);
    //写modbus值 regAddr:地址； bitPos:位地址； type = 0 : 位；= 1 ：值；= 2：浮点数
    void writeModbus_signal(int regAddr, int bitPos, float value, int type);
    //读modbus值 regAddr:地址； bitPos:位地址；type = 0 : 位；= 1 ：值；= 2：浮点数
    void readModbus_signal(int regAddr, int bitPos, int type);
    // 显示圆测距的图片
    void showCircleMeasure_signal(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config,double dis,bool isNG);
    // 所有视觉模块显示
    void showAllVision_signal(std::unordered_map<QString, Vision_Message> message);
    // 工作状态 state 0:待机；1:运行；2:停止
    void showWorkState_signal(int state);
    void upDateQgraphicsView_signal();
    //直接显示报错NG的图片
    void showNG_signal(HObject photo, PointItemConfig config,bool isNG = false);

public slots:
    void redLightFlashing_slot();
};

