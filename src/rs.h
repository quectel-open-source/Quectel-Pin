#pragma once
#pragma execution_character_set("utf-8")
#include <QString>
#include <QList>
#include <QJsonObject> 
#include <QJsonArray>
#include "dataDetect.h"
#include <QImage>

#define ERROR_TIME 5000
#define ERROR_TIME_2 30000
#define LOCAL_TEST 0
class RS2 {
public:
    /**
    * ActionFlag. 动作标志
    */
    enum ActionFlag
    {
        NONE_ActionFlagDR = 0,
        InitMotion,
        X_Homing,
        Y_Homing,
        Y1_Homing,
        Y2_Homing,
        Z_Homing,
        Z1_Homing,
        Z2_Homing,
        RZ_Homing,
        U_Homing,
        U1_Homing,
        U2_Homing,
        ResetAll,
        Left_Run,
        Right_Run,
        Both_Run,
        LS_Run,
        ED_Run,
        BT_Run,
        JmPin_Run,
        JmHan_Run
    };

    /**
    * IOStateEnum. IO电平
    */
    enum IOStateEnum
    {
        Low = 0,
        High
    };

};

/**
* SpeedModeEnum. 速度模式
*/
enum SpeedModeEnum
{
    // 低速模式
    Low = 0,
    // 中速模式
    Mid,
    // 高速模式
    High
};

/**
* ENUMSTATION. 机位
*/
enum ENUMSTATION
{
    L = 0,
    R,
    Both
};

/**
* ENUMPOINTTYPE. 配方动作点
*/
enum ENUMPOINTTYPE
{
    //过渡点
    PointNone = 0,
    //2D pin针识别点
    Point2D,
    //3D扫描点
    Point3D,
    //3D识别
    Point3D_identify,
    //取放料
    Feeding,
    //拍照
    TakePhoto,
    //视觉模块
    Visual_Identity,
    //圆测距
    Circle_Measure,
};

/**
* ENUM_VERSION. 软件版本
*/
enum ENUM_VERSION
{
    //双轨pin标机
    TwoPin = 0,
    //易鼎丰
    EdPin,
    //立昇
    LsPin,
    //博泰
    BtPin,
    //金脉Pin
    JmPin,
    //金脉Han
    JmHan
};


/**
* ENUM_FEEDACTION. 取放料动作
*/
enum ENUM_FEEDACTION
{
    // 扫码Mes确认
    MesCheck = 0,
    // 抓料_倍速链
    Suction,
    // 放料_倍速链
    Unloading,
    //抓料_载具
    Suction_2,
    // 放料_载具
    Unloading_2,
};

/**
* ENUM_VISION. 视觉模块
*/
enum ENUM_VISION
{
    // 模板匹配
    TempMatch = 0,
    //一维码模块
    OneDimensional,
    //二维码模块
    TwoDimensional,
    //Blob模块
    BlobModel,
    //圆检测模块
    CircleModel
};

/**
* ENUM_MOTIONVERSION. 板卡版本
*/
enum ENUM_MOTIONVERSION
{
    //雷赛脉冲卡
    LS_Pulse = 0,
    //雷赛总线卡
    LS_EtherCat,
    //雷赛1000系列脉冲卡
    LS_Pulse_1000,
};

/**
* ENUM_ETHERCAT_STATE. 轴状态
*/
enum ENUM_ETHERCAT_STATE
{
    //未启动状态
    NOT_READY_SWITCH_ON = 0,
    //启动禁止状态
    SWITCH_ON_DISABLE,
    //准备启动状态
    READY_TO_SWITCH_ON,
    //启动状态
    SWITCH_ON,
    //操作使能状态
    OP_ENABLE,
    //停止状态
    QUICK_STOP,
    //错误触发状态
    FAULT_ACTIVE,
    //错误状态
    FAULT
};

/**
* ENUM_USER. 用户
*/
enum ENUM_USER
{
    //普通用户
    Normal_User = 0,
    Admin_User
};

/**
* Axis. 雷赛轴名
*/
namespace LS_AxisName {
    //*************************************** 所有轴，不再单独对项目命名轴
	extern QString X;
    extern QString X1;
    extern QString X2;
    extern QString Y;
	extern QString Y1;
	extern QString Y2;
	extern QString Z;
    extern QString Z1;   
    extern QString Z2;   
	extern QString RZ;
    extern QString U;    
	extern QString U1;
	extern QString U2;

    //*************************************** 立昇专机的轴
    extern QString LS_X;
    extern QString LS_Y1;   //载具移动轴
    extern QString LS_Y2;   //相机移动轴
    extern QString LS_U;    //3D相机旋转轴
    extern QString LS_Z;

	extern QList<QString> allAxis;
};


/**
* DI
*/
namespace LS_DI
{
    //*************************************** 双轨pin标机
    // 按钮信号
    extern QString IN_BtnSetUpL;  // 左启动按钮
    extern QString IN_BtnSetUpR;  // 右启动按钮
    extern QString IN_BtnInit;    // 初始化按钮
    extern QString IN_BtnStop;    // 急停按钮
    extern QString IN_BtnPause;   // 暂停按钮
    extern QString IN_BtnReset;   // 复位按钮

    // 气缸信号
    extern QString IN_OUT_ClampCylinderL1InitP;  // 治具1气缸1原点
    extern QString IN_OUT_ClampCylinderL1EndP;  // 治具1气缸1动点
    extern QString IN_OUT_ClampCylinderL2InitP; // 治具1气缸2原点
    extern QString IN_OUT_ClampCylinderL2EndP; // 治具1气缸2动点
    extern QString IN_OUT_ClampCylinderR1InitP; // 治具2气缸1原点
    extern QString IN_OUT_ClampCylinderR1EndP; // 治具2气缸1动点
    extern QString IN_OUT_ClampCylinderR2InitP; // 治具2气缸2原点
    extern QString IN_OUT_ClampCylinderR2EndP; // 治具2气缸2动点

    // 光栅、安全锁、门信号
    extern QString IN_SafGrate1;  // 安全光栅1
    extern QString IN_SafLock1;   // 安全锁1
    extern QString IN_SafLock2;   // 安全锁2
    extern QString IN_SafLock3;   // 安全锁3
    extern QString IN_Door1;      // 门禁1
    extern QString IN_Door2;      // 门禁2
    extern QString IN_Door3;      // 门禁3
    extern QString IN_Door4;      // 门禁4
    extern QString IN_Door5;      // 门禁5
    extern QString IN_Door6;      // 门禁6

    // 其他信号
    extern QString IN_LPrdOK;     // 治具1防呆信号
    extern QString IN_RPrdOK;     // 治具2防呆信号
    extern QString IN_NG1;        // NG1
    extern QString IN_NG2;        // NG2
    extern QString IN_NG3;        // NG3

    //*************************************** 立昇
    // 按钮信号
    extern QString LS_IN_BtnSetUp;       // 启动按钮
    extern QString LS_IN_BtnStop;        // 停止按钮
    extern QString LS_IN_BtnReset;        // 复位按钮
    extern QString LS_IN_BtnEmergency;    // 急停按钮
    extern QString LS_IN_BtnLight;        // 照明按钮
    extern QString LS_IN_MainAirSupply;      // 机台总气源

    // 载具夹紧信号
    extern QString LS_IN_ClampCyl1_Home;     // 载具夹紧气缸1原点
    extern QString LS_IN_ClampCyl1_Work;     // 载具夹紧气缸1动点
    extern QString LS_IN_ClampCyl2_Home;     // 载具夹紧气缸2原点
    extern QString LS_IN_ClampCyl2_Work;     // 载具夹紧气缸2动点

    // 检测信号
    extern QString LS_IN_VacuumDetect;       // 夹爪真空检测
    extern QString LS_IN_MaterialDetect;     // 载具有料检测

    // 门禁信号
    extern QString LS_IN_GateFront1;       // 前1门禁开关
    extern QString LS_IN_GateFront2;       // 前2门禁开关
    extern QString LS_IN_GateFront3;       // 前3门禁开关
    extern QString LS_IN_GateFront4;       // 前4门禁开关
    extern QString LS_IN_GateRear1;        // 后1门禁开关
    extern QString LS_IN_GateRear2;        // 后2门禁开关
    extern QString LS_IN_GateRear3;        // 后3门禁开关
    extern QString LS_IN_GateRear4;        // 后4门禁开关

    // SMEMA & 新增气缸信号(倍速链部分)
    extern QString LS_IN_SMEMA_UpFront_AllowOut; // I8.0 SMEMA 上 前机允许出料
    extern QString LS_IN_SMEMA_LowFront_AllowOut;// I8.1 SMEMA 下 前机允许出料
    extern QString LS_IN_SMEMA_UpRear_AllowOut; // I8.2 SMEMA 上 后机允许出料
    extern QString LS_IN_SMEMA_LowRear_AllowOut; // I8.3 SMEMA 下 后机允许出料
    extern QString LS_IN_UpperMaterialDetect;    // I8.4 上层到料感应
    extern QString LS_IN_UpperOutputDetect;      // I8.5 上层出料感应
    extern QString LS_IN_LowerOutputDetect;      // I8.6 下层出料感应
    extern QString LS_IN_BlockCyl1_Work;        // I8.7 阻挡气缸1动点
    extern QString LS_IN_BlockCyl1_Home;        // I9.0 阻挡气缸1原点 1是阻挡状态
    extern QString LS_IN_BlockCyl2_Work;        // I9.1 阻挡气缸2动点
    extern QString LS_IN_BlockCyl2_Home;        // I9.2 阻挡气缸2原点
    extern QString LS_IN_LiftCyl_Work;          // I9.3 顶升动点
    extern QString LS_IN_LiftCyl_Home;          // I9.4 顶升原点
    extern QString LS_IN_LowerMaterialDetect;          // I9.5 下层有料感应
    extern QString LS_IN_UpperCheckProduct;     //载具上有料检测

    //------------------------------------------------ 易鼎丰专用DI信号（设备独有）
    // 按钮类
    extern QString ED_DI_BtnStart;          // 启动按钮
    extern QString ED_DI_BtnStop;           // 停止按钮
    extern QString ED_DI_BtnReset;          // 复位按钮
    extern QString ED_DI_SwitchAutoMode;    // 自动模式旋钮
    extern QString ED_DI_BtnEStop1;        // 急停按钮1
    extern QString ED_DI_BtnEStop2;        // 急停按钮2

    // 气缸位置检测
    extern QString ED_DI_ClampPosCyl1_Home; // 治具定位气缸1原点
    extern QString ED_DI_ClampPosCyl1_Work; // 治具定位气缸1动点
    extern QString ED_DI_ClampPosCyl2_Home; // 治具定位气缸2原点
    extern QString ED_DI_ClampPosCyl2_Work; // 治具定位气缸2动点
    extern QString ED_DI_ClampCyl1_Home;    // 治具夹紧气缸1原点
    extern QString ED_DI_ClampCyl1_Work;    // 治具夹紧气缸1动点
    extern QString ED_DI_ClampCyl2_Home;    // 治具夹紧气缸2原点
    extern QString ED_DI_ClampCyl2_Work;    // 治具夹紧气缸2动点

    // 安全门检测
    extern QString ED_DI_ProdDetect;        // 治具产品有无检测
    extern QString ED_DI_SafDoor_Feed1;    // 来料侧安全门1
    extern QString ED_DI_SafDoor_Feed2;    // 来料侧安全门2
    extern QString ED_DI_SafDoor_Feed3;    // 来料侧安全门3
    extern QString ED_DI_SafDoor_Feed4;    // 来料侧安全门4
    extern QString ED_DI_SafDoor_Discharge1; // 出料侧安全门1
    extern QString ED_DI_SafDoor_Discharge2; // 出料侧安全门2
    extern QString ED_DI_SafDoor_Discharge3; // 出料侧安全门3
    extern QString ED_DI_SafDoor_Discharge4; // 出料侧安全门4
    extern QString ED_DI_SafDoor_NG1;      // NG皮带侧安全门1
    extern QString ED_DI_SafDoor_NG2;      // NG皮带侧安全门2
    extern QString ED_DI_SafDoor_Rear1;    // 背面安全门1
    extern QString ED_DI_SafDoor_Rear2;    // 背面安全门2

    // 报警信号
    extern QString ED_DI_AirPressureAlarm; // CDA进气压力报警

    // 皮带检测类
    extern QString ED_DI_FeedBeltMaterialDetect;      // 来料皮带产品来料检测
    extern QString ED_DI_FeedBeltArrivalDetect;       // 来料皮带产品到位检测
    extern QString ED_DI_DischargeBeltReleaseDetect;  // 出料皮带产品放料检测
    extern QString ED_DI_DischargeBeltArrivalDetect;   // 出料皮带产品到位检测
    extern QString ED_DI_NGBeltReleaseDetect_Left;    // NG皮带产品放料检测1（左）
    extern QString ED_DI_NGBeltReleaseDetect_Right;   // NG皮带产品放料检测2（右）
    extern QString ED_DI_NGBeltFullDetect;            // NG皮带产品满料检测

	extern QList<QString> allDI;
}


/**
 * DO
 */
namespace LS_DO
{
    //------------------------------------------------ 可复用DO
    extern QString DO_LightRed;    // 三色灯-红
    extern QString DO_LightGreen;  // 三色灯-绿
    extern QString DO_LightYellow; // 三色灯-黄
    extern QString DO_Buzzer;      // 三色灯-蜂鸣器
    extern QString DO_Cam2DTrigger;    // 2D相机触发
    extern QString DO_Cam3DTrigger;    // 3D相机触发
    extern QString DO_Light2DTrigger;  // 2D光源触发
    extern QString DO_StartBtnLED;     // 启动按钮指示灯
    extern QString DO_StopBtnLED;      // 停止按钮指示灯

    //------------------------------------------------ 双轨pin标机
    // 执行机构
    extern QString DO_ClampCylinderL1;  // 治具1气缸1
    extern QString DO_ClampCylinderL2;  // 治具1气缸2
    extern QString DO_ClampCylinderR1;  // 治具2气缸1
    extern QString DO_ClampCylinderR2;  // 治具2气缸2
    extern QString DO_Lock1;            // 电子锁1
    extern QString DO_Lock2;            // 电子锁2
    extern QString DO_Lock3;            // 电子锁3

    // 指示灯/信息灯
    extern QString DO_LightFlood;  // 照明灯

    // 子设备触发
    extern QString DO_PlusCam3D1;      // 3D脉冲-X
    extern QString DO_PlusCam3D2;      // 3D脉冲-Y1
    extern QString DO_PlusCam3D3;      // 3D脉冲-Y2
    extern QString DO_ReadCodeTrigger1; // 扫码枪触发1
    extern QString DO_ReadCodeTrigger2; // 扫码枪触发2

    // 指示灯
    extern QString DO_LightOK_L;  // 指示灯1-绿
    extern QString DO_LightNG_L;  // 指示灯1-红
    extern QString DO_LightOK_R;  // 指示灯2-绿
    extern QString DO_LightNG_R;  // 指示灯2-红

    //*************************************** 立昇
    extern QString LS_OUT_RedLight;        // 三色灯-红灯
    extern QString LS_OUT_YellowLight;     // 三色灯-黄灯
    extern QString LS_OUT_GreenLight;      // 三色灯-绿灯
    extern QString LS_OUT_Buzzer;          // 蜂鸣器
    extern QString LS_OUT_StartBtnLED;     // 启动按钮指示灯
    extern QString LS_OUT_StopBtnLED;      // 停止按钮指示灯
    extern QString LS_OUT_ResetBtnLED;     // 复位按钮指示灯
    extern QString LS_OUT_LightBtnLED;     // 照明按钮指示灯
    extern QString LS_OUT_ClampSuck;       // 夹爪吸气
    extern QString LS_OUT_ClampBlow;       // 夹爪吹气
    extern QString LS_OUT_Lighting;        // 照明
    extern QString LS_OUT_ClampCyl1;       // 载具夹紧气缸1
    extern QString LS_OUT_ClampCyl2;       // 载具夹紧气缸2
    extern QString LS_OUT_2DLightTrigger;  // 2D光源触发
    extern QString LS_OUT_2DCamTrigger;    // 2D相机触发
    extern QString LS_OUT_3DCamTrigger;    // 3D相机触发
    // SMEMA 进料信号（下面都是倍速链的信号）
    extern QString LS_OUT_SMEMA_UpFront_FeedReady;   // Q6.0 SMEMA 上 前机进料到位信号
    extern QString LS_OUT_SMEMA_LowFront_FeedArrived;// Q6.1 SMEMA 下 前机进料到料信号
    extern QString LS_OUT_SMEMA_UpRear_FeedReady;    // Q6.2 SMEMA 上 后机进料到位信号
    extern QString LS_OUT_SMEMA_LowRear_FeedReady;   // Q6.3 SMEMA 下 后机进料到位信号
    // 电机控制信号
    extern QString LS_OUT_UpperMotor_Run;           // Q6.4 上层电机RUN
    extern QString LS_OUT_UpperMotor_Dir;           // Q6.5 上层电机Dir
    extern QString LS_OUT_LowerMotor_Run;           // Q6.6 下层电机RUN
    extern QString LS_OUT_LowerMotor_Dir;           // Q6.7 下层电机Dir
    // 气缸控制信号
    extern QString LS_OUT_BlockCyl_Activate;        // Q7.0 阻挡气缸 0是阻挡，正常是一直阻挡着
    extern QString LS_OUT_LiftCyl_Activate;         // Q7.1 升降气缸

    //------------------------------------------------ 易鼎丰专用DO
    extern QString ED_DO_PositionCyl1;      // DO1.0 定位气缸电磁阀1
    extern QString ED_DO_PositionCyl2;      // DO1.1 定位气缸电磁阀2
    extern QString ED_DO_ClampCyl1_Valve1;  // DO1.2 夹紧气缸1电磁阀1
    extern QString ED_DO_ClampCyl1_Valve2;  // DO1.3 夹紧气缸1电磁阀2
    extern QString ED_DO_CamFrontTrigger;   // DO2.0 正面相机硬触发
    extern QString ED_DO_CamRearTrigger;    // DO2.1 反面相机硬触发
    extern QString ED_DO_LightFrontTrigger; // DO2.4 正面相机光源触发
    extern QString ED_DO_LightRearTrigger;  // DO2.5 反面相机光源触发
    extern QString ED_DO_LightPositioningTrigger;  // DO4.7 定位相机光源触发
    // 启动控制类
    extern QString ED_DO_FeedBeltStart;               // 来料皮带启动
    extern QString ED_DO_DischargeBeltStart;         // 出料皮带启动
    extern QString ED_DO_NGBeltStart;                // NG皮带启动

    // 所有 DO 信号集合
    extern QList<QString> allDO;
}

/**
* Camera. 相机名
*/
namespace LS_CameraName {
    extern QString pin2D;
    //易鼎丰上相机
    extern QString ed_Up;
    //易鼎丰下相机
    extern QString ed_Down;
    //易鼎丰定位相机
    extern QString ed_Location;
    //侧相机
    extern QString side_camera;

    extern QList<QString> allCamera;
};

// PointItemConfig 类的定义（用于配方里的每一项信息）
class PointItemConfig {
public:
    //如果左右分开存的话好像这几个参数暂时没有用，先不放出来
    //int SlnId = 0;
    int StationL = ENUMSTATION::L;
    //int SortIndex = 0;

    //点类型（2D,3D，过渡点）
    int PointType = -1;

    double TargetPX = 0.0;
    double TargetPY = 0.0;
    double TargetPZ = 0.0;
    double TargetPRZ = 0.0;
    double TargetPU = 0.0;

    // 后面轴不同专机直接用同一个变量，按顺序就行
    double TargetPAxis_1 = 0.0;
    double TargetPAxis_2 = 0.0;
    double TargetPAxis_3 = 0.0;
    double TargetPAxis_4 = 0.0;
    double TargetPAxis_5 = 0.0;
    double TargetPAxis_6 = 0.0;
    double TargetPAxis_7 = 0.0;

    //速度模式
    int SpeedModel = SpeedModeEnum::Low;
    //视觉配方名
    QString Vision = "";
    //备注
    QString Tip = "";
    //3D点云名
    QString CloudName = "";
    //插头名
    QString ProductName = "";
    //保存3D点云
    int isSave3dCloud = 0;
    //取放料动作
    int FeedAction = ENUM_FEEDACTION::MesCheck;
    //相机名称
    QString CameraName;
    //视觉方案
    int VisionConfig = ENUM_VISION::TempMatch;
    //相片名
    QString PhotoName = "";
    //型号（不同版本有可能用上）
    QString ModelName = "";
    //结果合并
    int isResultMerging = 0;
    //视觉配方名二
    QString VisionTwo = "";
    //3D扫描行数
    int scanRow3d = 2000;
    //圆测量基准值
    double CircleBenchmark = 0;
    //圆测量上偏差
    double CircleUpError = 0;
    //圆测量下偏差
    double CircleDownError = 0;
    //1mm对应的像素值
    double PixDistance = 0;
    //用于输出画图结果，这一步是否NG
    bool isNG = false;
    //曝光时间
    int ExposureTime = 0;

    //// 通讯方式，暂时没用上
    //bool ModWrite;
    //bool ModRead;
    //QString RegisterIdWrite;
    //QString RegisterIdRead;
    //QString ValueWrite;
    //QString ValueRead;

    // 构造函数
    PointItemConfig() {};
    //PointItemConfig(int slnId, bool stationL, int sortIndex, double targetPX, double targetPY, double targetPZ,
    //    double targetPRZ, double targetPU, int pointType, int speedModel, bool modWrite, bool modRead,
    //    const QString& registerIdWrite, const QString& registerIdRead, const QString& valueWrite,
    //    const QString& valueRead, const QString& vision)
    //    : SlnId(slnId), StationL(stationL), SortIndex(sortIndex), TargetPX(targetPX), TargetPY(targetPY),
    //    TargetPZ(targetPZ), TargetPRZ(targetPRZ), TargetPU(targetPU), PointType(pointType), SpeedModel(speedModel),
    //    ModWrite(modWrite), ModRead(modRead), RegisterIdWrite(registerIdWrite), RegisterIdRead(registerIdRead),
    //    ValueWrite(valueWrite), ValueRead(valueRead), Vision(vision){}
};

//结果统计
struct Run_Result
{
    //2D
    bool isNG_2D = false;
    //3D
    bool isNG_3D = false;
    //总
    bool isNG = false;
    //一维码模块
    bool isNG_OneDimensional = false;
    //二维码模块
    bool isNG_TwoDimensional = false;
    //Blob模块
    bool isNG_Blob = false;
    //圆检测
    bool isNG_Circle = false;
    //模板匹配
    bool isNG_Temple = false;
    //圆测距
    bool isNG_CircleMeasure = false;
    //型号名
    QString modelName = "";

    void clear()
    {
        isNG_2D = false;
        isNG_3D = false;
        isNG = false;
        isNG_OneDimensional = false;
        isNG_TwoDimensional = false;
        isNG_Blob = false;
        isNG_Circle = false;
        isNG_Temple = false;
        isNG_CircleMeasure = false;
        modelName = "";
    };
};

//-------------------------- 立昇
// 请求参数结构体
struct SnCheckRequest {
    QString cmd = "QUERY_RECORD";   //进站前固定值
    QString p = "UOP";
    QString station_name = "DEF_L-ASS-14_1_PIN针垂直测试";    // 工位编码 (必填)
    QString sn;              // 主条码 (必填)
    QString op;              // 用户账号 (必填)
    QString csn;             // 客户条码 (可选)
    QString tooling_no;      // RFID (可选)
    QString machine_no;      // 设备编码 (可选)
    QString first_sign;       // 首件标识 (可选)
};

// 响应结果结构体
struct SnCheckResponse {
    int code;                // 200=成功, 500=失败
    QString msg;
    QString main_sn;
    QString first_sign;
    QString rh_first_sign;
    QString IsStay;
};


// 测试结果子项
struct TestResult {
    QString name;          // 测试项名称
    QString units;          // 单位
    QString result;         // 测试结果
    QString message;        // 附加信息

    QJsonObject toJson() const {
        return {
            {"name", name},
            {"units", units},
            {"result", result},
            {"message", message}
        };
    }
};


// 过站请求参数结构体
struct SnResultRequest {
    QString cmd = "ADD_RECORD"; // 固定值
    QString station_name = "DEF_L-ASS-14_1_PIN针垂直测试";       // 工位名称
    QString sn;                     // 产品序列号
    QString op;                     // 操作员编号
    QString op_time;                // 操作时间
    QString result;                 // 最终结果
    QString start_time;             // 开始时间
    QString stop_time;              // 结束时间
    QList<TestResult> result_data;  // 测试数据集合

    QJsonObject toJson() const {
        QJsonArray dataArray;
        for (const auto& item : result_data) {
            dataArray.append(item.toJson());
        }

        return {
            {"cmd", cmd},
            {"station_name", station_name},
            {"sn", sn},
            {"op", op},
            {"op_time", op_time},
            {"result", result},
            {"start_time", start_time},
            {"stop_time", stop_time},
            {"result_data", dataArray}
        };
    }

    void clearData()
    {
        result_data.clear();
    }
};

//视觉识别模块图片
struct Vision_Message
{
    HObject img;
    //模板匹配模块信息
    vector<std::pair<PointItemConfig,ResultParamsStruct_Template>> TempMes;
    //一维码模块信息
    vector< std::pair<PointItemConfig, ResultParamsStruct_BarCode>> BarMes;
    //二维码模块信息
    vector< std::pair<PointItemConfig, ResultParamsStruct_Code2d>> QRMes;
    //Blob模块信息
    vector< std::pair<PointItemConfig, ResultParamsStruct_Blob>> BlobMes;
    //圆模块信息
    vector< std::pair<PointItemConfig, ResultParamsStruct_Circle>> CircleMes;
    bool isNG = false;
    void clear()
    {
        //if (img.IsInitialized())
        //    img.Clear();
        img.Clear();
        TempMes.clear();
        BarMes.clear();
        QRMes.clear();
        BlobMes.clear();
        CircleMes.clear();
    };
    ~Vision_Message() {
        clear();
    }
};

//机器定位结果
struct RobotResult {

    cv::Mat original;
    cv::Mat display;
    float consume;
    double x, y, a;
    long long timestamp;
};

//易鼎丰机器人读取数据
struct ED_Robot_Connect
{
    //40035(34)
    bool feedInBeltPickDone = 0;  // Bit0: 来料皮带取料完成
    bool fixturePlaceDone = 0;  // Bit1: 检测治具放料完成
    bool fixturePickDone = 0;  // Bit2: 检测治具取料完成
    bool dischargeBeltPlaceDone = 0; // Bit3: 出料皮带放料完成
    bool ngBeltLeftPlaceDone = 0;  // Bit4: NG皮带左放料完成
    bool ngBeltRightPlaceDone = 0;  // Bit5: NG皮带右放料完成
    bool haveProduct = 0;  // Bit6: 取料手有料信号
    bool haveProductB = 0;  // Bit7: B手取料手有料信号
    //40039(38)
    int errorMessage = 0;//运行信号报警码
    //40001(0) 机器人反馈值
    bool manualStatus = 0;  // Bit0: 手动状态
    bool autoStatus = 0;  // Bit1: 自动状态
    bool remoteStatus = 0;  // Bit2: 远程状态
    bool servoStatus = 0;  // Bit3: 伺服状态
    bool alarmStatus = 0;  // Bit4: 报警状态
    bool emergencyStopStatus = 0;  // Bit5: 急停状态
    bool programRunningStatus = 0;  // Bit6: 程序运行状态
    bool safePosition1Status = 0;  // Bit7: 安全位置1状态
    bool safePosition2Status = 0;  // Bit8: 安全位置2状态
    bool safePosition3Status = 0;  // Bit9: 安全位置3状态
    bool safePosition4Status = 0;  // Bit10: 安全位置4状态
    bool programLoadStatus = 0;  // Bit11: 程序加载状态
    bool servoReadyStatus = 0;  // Bit12: 伺服准备状态
    bool programReservation = 0;  // Bit13: 程序预约激活状态
    bool programResetStatus = 0;  // Bit14: 程序复位状态
    //清空运动检测信号
    void clearMotion()
    {
        feedInBeltPickDone = 0;  // Bit0: 来料皮带取料完成
        fixturePlaceDone = 0;  // Bit1: 检测治具放料完成
        fixturePickDone = 0;  // Bit2: 检测治具取料完成
        dischargeBeltPlaceDone = 0; // Bit3: 出料皮带放料完成
        ngBeltLeftPlaceDone = 0;  // Bit4: NG皮带左放料完成
        ngBeltRightPlaceDone = 0;  // Bit5: NG皮带右放料完成
        haveProduct = 0;
        haveProductB = 0;
        errorMessage = 0;
    };
    //清空机器人状态信号
    void clearState()
    {
        manualStatus = 0;  // Bit0: 手动状态
        autoStatus = 0;  // Bit1: 自动状态
        remoteStatus = 0;  // Bit2: 远程状态
        servoStatus = 0;  // Bit3: 伺服状态
        alarmStatus = 0;  // Bit4: 报警状态
        emergencyStopStatus = 0;  // Bit5: 急停状态
        programRunningStatus = 0;  // Bit6: 程序运行状态
        safePosition1Status = 0;  // Bit7: 安全位置1状态
        safePosition2Status = 0;  // Bit8: 安全位置2状态
        safePosition3Status = 0;  // Bit9: 安全位置3状态
        safePosition4Status = 0;  // Bit10: 安全位置4状态
        programLoadStatus = 0;  // Bit11: 程序加载状态
        servoReadyStatus = 0;  // Bit12: 伺服准备状态
        programReservation = 0;  // Bit13: 程序预约激活状态
        programResetStatus = 0;  // Bit14: 程序复位状态
    };
};

//轴回零标志
struct AXIS_HOME
{
    bool U = false;
    bool U1 = false;
};

//数据库内容
struct SQL_Parameter
{
    QString formulaName;
    QString snName;
};

//-------------------------- 易鼎丰MES
//进站参数
struct MesCheckInRequest_ED {
    QString sn;          // 主条码 (必填)
    QString workPlaceCode = "移远视觉检查";   // 测试站点 (必填)
    QString laneId = "1";        // 轨道 默认1
    QString checkAllRef = "true";   // 是否检查小板 默认true
    QString productSn;
};

//返回参数
struct MesCheckInResponse_ED {
    bool state = true;         // 接口状态: true-合格允许测试, false-不合格不允许测试
    QString description; // 异常描述: state=true时为空, state=false时为错误提示
};

// 易鼎丰MES过站请求结构体
struct MesCheckInFullRequest_ED {
    QString WorkPlaceCode = "移远视觉检查";
    QString WorkOrderCode = "视觉检查机测试工单";
    QString User;
    QString Sn;
    QString Result;
    QString ProductCode;
    QString OtherCode1;
    QString OtherCode2;
    QString OtherCode3;
    QString OtherCode4;
    QString Result_2D;
    QString Result_3D;
    QString Screw1Inspection;
    QString Screw2Inspection;
    QString Screw3Inspection;
    QString Screw4Inspection;
    QString Result_LabelCharacters;
    QString Result_SnapFit;
    QString WaterproofBreathableInspection;
    QString SealantInspection;
    QString Result_CircularDistance1;
    QString Result_CircularDistance2;
    QString Result_CircularDistance3;
    QString Result_CircularDistance4;
    QString PictureUrl;
};

