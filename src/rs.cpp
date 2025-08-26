#include"rs.h"

namespace LS_AxisName {
    QString X = "X";
    QString X1 = "X1";
    QString X2 = "X2";
    QString Y = "Y";
    QString Y1 = "Y1";
    QString Y2 = "Y2";
    QString Z = "Z";
    QString Z1 = "Z1";
    QString Z2 = "Z2";
    QString RZ = "RZ";
    QString U = "U";
    QString U1 = "U1";
    QString U2 = "U2";

    //立昇专机的轴
    QString LS_X = "X";
    QString LS_Y1 = "Y1";    //载具移动轴
    QString LS_Y2 = "Y2";    //3D相机移动轴
    QString LS_U = "U";  //相机旋转轴
    QString LS_Z = "Z";

    //默认用双轨pin标机配置
    QList<QString> allAxis{
        X,
        Y1,
        Y2,
        Z,
        RZ,
        U1,
        U2
    };
}

namespace LS_DI
{
    QString LS_DI::IN_BtnSetUpL{ "左启动按钮" };
    QString LS_DI::IN_BtnSetUpR{ "右启动按钮" };
    QString LS_DI::IN_BtnInit{ "初始化按钮" };
    QString LS_DI::IN_BtnStop{ "急停按钮" };
    QString LS_DI::IN_BtnPause{ "暂停按钮" };
    QString LS_DI::IN_BtnReset{ "复位按钮" };

    // 气缸信号
    QString LS_DI::IN_OUT_ClampCylinderL1InitP{ "治具1气缸1原点" };
    QString LS_DI::IN_OUT_ClampCylinderL1EndP{ "治具1气缸1动点" };
    QString LS_DI::IN_OUT_ClampCylinderL2InitP{ "治具1气缸2原点" };
    QString LS_DI::IN_OUT_ClampCylinderL2EndP{ "治具1气缸2动点" };
    QString LS_DI::IN_OUT_ClampCylinderR1InitP{ "治具2气缸1原点" };
    QString LS_DI::IN_OUT_ClampCylinderR1EndP{ "治具2气缸1动点" };
    QString LS_DI::IN_OUT_ClampCylinderR2InitP{ "治具2气缸2原点" };
    QString LS_DI::IN_OUT_ClampCylinderR2EndP{ "治具2气缸2动点" };

    // 光栅、安全锁、门信号
    QString LS_DI::IN_SafGrate1{ "安全光栅1" };
    QString LS_DI::IN_SafLock1{ "安全锁1" };
    QString LS_DI::IN_SafLock2{ "安全锁2" };
    QString LS_DI::IN_SafLock3{ "安全锁3" };
    QString LS_DI::IN_Door1{ "门禁1" };
    QString LS_DI::IN_Door2{ "门禁2" };
    QString LS_DI::IN_Door3{ "门禁3" };
    QString LS_DI::IN_Door4{ "门禁4" };
    QString LS_DI::IN_Door5{ "门禁5" };
    QString LS_DI::IN_Door6{ "门禁6" };

    // 其他信号
    QString LS_DI::IN_LPrdOK{ "治具1防呆信号" };
    QString LS_DI::IN_RPrdOK{ "治具2防呆信号" };
    QString LS_DI::IN_NG1{ "NG1" };
    QString LS_DI::IN_NG2{ "NG2" };
    QString LS_DI::IN_NG3{ "NG3" };

    //*************************************** 立昇
    // 按钮信号
    QString LS_IN_BtnSetUp{ "启动按钮" };       // DI0.0
    QString LS_IN_BtnStop{ "停止按钮" };        // DI0.1
    QString LS_IN_BtnReset{ "复位按钮" };       // DI0.2
    QString LS_IN_BtnEmergency{ "急停按钮" };   // DI0.3 状态为0是触发
    QString LS_IN_BtnLight{ "照明按钮" };       // DI0.4
    QString LS_IN_MainAirSupply{ "机台总气源" };   // DI0.5

    // 载具夹紧信号
    QString LS_IN_ClampCyl1_Home{ "载具夹紧气缸1原点" }; // DI1.0
    QString LS_IN_ClampCyl1_Work{ "载具夹紧气缸1动点" }; // DI1.1
    QString LS_IN_ClampCyl2_Home{ "载具夹紧气缸2原点" }; // DI1.2
    QString LS_IN_ClampCyl2_Work{ "载具夹紧气缸2动点" }; // DI1.3

    // 检测信号
    QString LS_IN_VacuumDetect{ "夹爪真空检测" };   // DI2.0
    QString LS_IN_MaterialDetect{ "载具有料检测" }; // DI2.1 状态为0是触发

    // 门禁信号
    QString LS_IN_GateFront1{ "前1门禁开关" };     // DI3.0
    QString LS_IN_GateFront2{ "前2门禁开关" };     // DI3.1
    QString LS_IN_GateFront3{ "前3门禁开关" };     // DI3.2
    QString LS_IN_GateFront4{ "前4门禁开关" };     // DI3.3
    QString LS_IN_GateRear1{ "后1门禁开关" };      // DI3.4
    QString LS_IN_GateRear2{ "后2门禁开关" };      // DI3.5
    QString LS_IN_GateRear3{ "后3门禁开关" };      // DI3.6
    QString LS_IN_GateRear4{ "后4门禁开关" };      // DI3.7

    // SMEMA & 新增气缸信号
    QString LS_IN_SMEMA_UpFront_AllowOut{ "SMEMA 上 前机允许出料" }; // I8.0
    QString LS_IN_SMEMA_LowFront_AllowOut{ "SMEMA 下 前机允许出料" };// I8.1
    QString LS_IN_SMEMA_UpRear_AllowOut{ "SMEMA 上 后机允许出料" }; // I8.2
    QString LS_IN_SMEMA_LowRear_AllowOut{ "SMEMA 下 后机允许出料" }; // I8.3
    QString LS_IN_UpperMaterialDetect{ "上层有料感应" };          // I8.4
    QString LS_IN_UpperOutputDetect{ "上层出料感应" };            // I8.5
    QString LS_IN_LowerOutputDetect{ "下层出料感应" };            // I8.6
    QString LS_IN_BlockCyl1_Work{ "阻挡气缸1动点" };             // I8.7
    QString LS_IN_BlockCyl1_Home{ "阻挡气缸1原点" };             // I9.0
    QString LS_IN_BlockCyl2_Work{ "阻挡气缸2动点" };             // I9.1
    QString LS_IN_BlockCyl2_Home{ "阻挡气缸2原点" };             // I9.2
    QString LS_IN_LiftCyl_Work{ "顶升动点" };                   // I9.3
    QString LS_IN_LiftCyl_Home{ "顶升原点" };                   // I9.4
    QString LS_IN_LowerMaterialDetect{ "下层有料感应" };                   // I9.5
    QString LS_IN_UpperCheckProduct{ "载具上有料检测" };

    //------------------------------------------------ 易鼎丰DI初始化
    // 按钮类
    QString ED_DI_BtnStart{ "启动按钮" };
    QString ED_DI_BtnStop{ "停止按钮" };
    QString ED_DI_BtnReset{ "复位按钮" };
    QString ED_DI_SwitchAutoMode{ "自动模式旋钮" };
    QString ED_DI_BtnEStop1{ "急停按钮1" };
    QString ED_DI_BtnEStop2{ "急停按钮2" };
    // 气缸位置检测
    QString ED_DI_ClampPosCyl1_Home{ "治具定位气缸1原点" };
    QString ED_DI_ClampPosCyl1_Work{ "治具定位气缸1动点" };
    QString ED_DI_ClampPosCyl2_Home{ "治具定位气缸2原点" };
    QString ED_DI_ClampPosCyl2_Work{ "治具定位气缸2动点" };
    QString ED_DI_ClampCyl1_Home{ "治具夹紧气缸1原点" };
    QString ED_DI_ClampCyl1_Work{ "治具夹紧气缸1动点" };
    QString ED_DI_ClampCyl2_Home{ "治具夹紧气缸2原点" };
    QString ED_DI_ClampCyl2_Work{ "治具夹紧气缸2动点" };
    // 安全门检测
    QString ED_DI_ProdDetect{ "治具产品有无检测" };
    QString ED_DI_SafDoor_Feed1{ "来料侧安全门1" };
    QString ED_DI_SafDoor_Feed2{ "来料侧安全门2" };
    QString ED_DI_SafDoor_Feed3{ "来料侧安全门3" };
    QString ED_DI_SafDoor_Feed4{ "来料侧安全门4" };
    QString ED_DI_SafDoor_Discharge1{ "出料侧安全门1" };
    QString ED_DI_SafDoor_Discharge2{ "出料侧安全门2" };
    QString ED_DI_SafDoor_Discharge3{ "出料侧安全门3" };
    QString ED_DI_SafDoor_Discharge4{ "出料侧安全门4" };
    QString ED_DI_SafDoor_NG1{ "NG皮带侧安全门1" };
    QString ED_DI_SafDoor_NG2{ "NG皮带侧安全门2" };
    QString ED_DI_SafDoor_Rear1{ "背面安全门1" };
    QString ED_DI_SafDoor_Rear2{ "背面安全门2" };
    // 报警信号
    QString ED_DI_AirPressureAlarm{ "CDA进气压力报警" };
    // 皮带检测类初始化
    QString ED_DI_FeedBeltMaterialDetect{ "来料皮带产品来料检测" };
    QString ED_DI_FeedBeltArrivalDetect{ "来料皮带产品到位检测" };
    QString ED_DI_DischargeBeltReleaseDetect{ "出料皮带产品放料检测" };
    QString ED_DI_DischargeBeltArrivalDetect{ "出料皮带产品到位检测" };
    QString ED_DI_NGBeltReleaseDetect_Left{ "NG皮带产品放料检测1（左）" };
    QString ED_DI_NGBeltReleaseDetect_Right{ "NG皮带产品放料检测2（右）" };
    QString ED_DI_NGBeltFullDetect{ "NG皮带产品满料检测" };

    // 所有 DI 信号集合
    QList<QString> LS_DI::allDI{
        IN_BtnSetUpL,
        IN_BtnSetUpR,
        IN_BtnInit,
        IN_BtnStop,
        IN_BtnPause,
        IN_BtnReset,
        IN_OUT_ClampCylinderL1InitP,
        IN_OUT_ClampCylinderL1EndP,
        IN_OUT_ClampCylinderL2InitP,
        IN_OUT_ClampCylinderL2EndP,
        IN_OUT_ClampCylinderR1InitP,
        IN_OUT_ClampCylinderR1EndP,
        IN_OUT_ClampCylinderR2InitP,
        IN_OUT_ClampCylinderR2EndP,
        IN_SafGrate1,
        IN_SafLock1,
        IN_SafLock2,
        IN_SafLock3,
        IN_Door1,
        IN_Door2,
        IN_Door3,
        IN_Door4,
        IN_Door5,
        IN_Door6,
        IN_LPrdOK,
        IN_RPrdOK,
        IN_NG1,
        IN_NG2,
        IN_NG3
    };
}

namespace LS_DO {

    //------------------------------------------------ 可复用DO
    QString DO_LightRed{ "三色灯-红" };     
    QString DO_LightGreen{ "三色灯-绿" };   
    QString DO_LightYellow{ "三色灯-黄" };  
    QString DO_Buzzer{ "蜂鸣器" };          
    QString DO_Cam2DTrigger{ "2D相机触发" };
    QString DO_Cam3DTrigger{ "3D相机触发" };
    QString DO_Light2DTrigger{ "2D光源触发" }; 
    QString DO_StartBtnLED{ "启动按钮指示灯" };
    QString DO_StopBtnLED{ "停止按钮指示灯" }; 

    //------------------------------------------------ 双轨pin标机
    QString DO_ClampCylinderL1{ "治具1气缸1" };  
    QString DO_ClampCylinderL2{ "治具1气缸2" };  
    QString DO_ClampCylinderR1{ "治具2气缸1" };  
    QString DO_ClampCylinderR2{ "治具2气缸2" };  
    QString DO_Lock1{ "电子锁1" };               
    QString DO_Lock2{ "电子锁2" };               
    QString DO_Lock3{ "电子锁3" };               
    QString DO_LightFlood{ "照明灯" };           
    QString DO_PlusCam3D1{ "3D脉冲-X" };         
    QString DO_PlusCam3D2{ "3D脉冲-Y1" };        
    QString DO_PlusCam3D3{ "3D脉冲-Y2" };        
    QString DO_ReadCodeTrigger1{ "扫码枪触发1" };
    QString DO_ReadCodeTrigger2{ "扫码枪触发2" };
    QString DO_LightOK_L{ "指示灯1-绿" };        
    QString DO_LightNG_L{ "指示灯1-红" };        
    QString DO_LightOK_R{ "指示灯2-绿" };        
    QString DO_LightNG_R{ "指示灯2-红" };        

    //*************************************** 立昇
    QString LS_OUT_RedLight{ "三色灯-红灯" };       // DO0.0
    QString LS_OUT_YellowLight{ "三色灯-黄灯" };     // DO0.1
    QString LS_OUT_GreenLight{ "三色灯-绿灯" };      // DO0.2
    QString LS_OUT_Buzzer{ "蜂鸣器" };             // DO0.3
    QString LS_OUT_StartBtnLED{ "启动按钮指示灯" };   // DO0.4
    QString LS_OUT_StopBtnLED{ "停止按钮指示灯" };    // DO0.5
    QString LS_OUT_ResetBtnLED{ "复位按钮指示灯" };   // DO0.6
    QString LS_OUT_LightBtnLED{ "照明按钮指示灯" };   // DO0.7
    QString LS_OUT_ClampSuck{ "夹爪吸气" };       // DO1.0
    QString LS_OUT_ClampBlow{ "夹爪吹气" };       // DO1.1
    QString LS_OUT_Lighting{ "照明" };            // DO1.2
    QString LS_OUT_ClampCyl1{ "载具夹紧气缸1" };  // DO1.3
    QString LS_OUT_ClampCyl2{ "载具夹紧气缸2" };  // DO1.4
    QString LS_OUT_2DLightTrigger{ "2D光源触发" }; // DO2.0
    QString LS_OUT_2DCamTrigger{ "2D相机触发" };   // DO2.1
    QString LS_OUT_3DCamTrigger{ "3D相机触发" };   // DO2.2
    // SMEMA 进料信号
    QString LS_OUT_SMEMA_UpFront_FeedReady{ "SMEMA 上 前机进料到位信号" };    // Q6.0
    QString LS_OUT_SMEMA_LowFront_FeedArrived{ "SMEMA 下 前机进料到料信号" }; // Q6.1
    QString LS_OUT_SMEMA_UpRear_FeedReady{ "SMEMA 上 后机进料到位信号" };     // Q6.2
    QString LS_OUT_SMEMA_LowRear_FeedReady{ "SMEMA 下 后机进料到位信号" };    // Q6.3
    // 电机控制信号
    QString LS_OUT_UpperMotor_Run{ "上层电机RUN" };                       // Q6.4
    QString LS_OUT_UpperMotor_Dir{ "上层电机Dir" };                       // Q6.5
    QString LS_OUT_LowerMotor_Run{ "下层电机RUN" };                       // Q6.6
    QString LS_OUT_LowerMotor_Dir{ "下层电机Dir" };                       // Q6.7
    // 气缸控制信号
    QString LS_OUT_BlockCyl_Activate{ "阻挡气缸" };                       // Q7.0
    QString LS_OUT_LiftCyl_Activate{ "升降气缸" };                        // Q7.1

    //------------------------------------------------ 易鼎丰专用DO
    QString ED_DO_PositionCyl1{ "定位气缸电磁阀1" };      // DO1.0
    QString ED_DO_PositionCyl2{ "定位气缸电磁阀2" };      // DO1.1
    QString ED_DO_ClampCyl1_Valve1{ "夹紧气缸1电磁阀1" };  // DO1.2
    QString ED_DO_ClampCyl1_Valve2{ "夹紧气缸1电磁阀2" };  // DO1.3
    QString ED_DO_CamFrontTrigger{ "正面相机硬触发" };     // DO2.0
    QString ED_DO_CamRearTrigger{ "反面相机硬触发" };      // DO2.1
    QString ED_DO_LightFrontTrigger{ "正面相机光源触发" }; // DO2.4
    QString ED_DO_LightRearTrigger{ "反面相机光源触发" };  // DO2.5
    QString ED_DO_LightPositioningTrigger{ "定位相机光源触发" };
    // 启动控制类初始化
    QString ED_DO_FeedBeltStart{ "来料皮带启动" };
    QString ED_DO_DischargeBeltStart{ "出料皮带启动" };
    QString ED_DO_NGBeltStart{ "NG皮带启动" };

    // 所有 DO 信号集合
    QList<QString> LS_DO::allDO{
        DO_LightRed,       
        DO_LightGreen,     
        DO_LightYellow,    
        DO_Buzzer,         
        DO_Cam2DTrigger,   
        DO_Cam3DTrigger,   
        DO_Light2DTrigger, 
        DO_StartBtnLED,    
        DO_StopBtnLED      
    };
}

/**
* Camera. 相机名
*/
namespace LS_CameraName {
    QString LS_CameraName::pin2D{ "Pin针2D相机" };
    //易鼎丰上相机
    QString LS_CameraName::ed_Up{ "易鼎丰上相机" };
    //易鼎丰下相机
    QString LS_CameraName::ed_Down{ "易鼎丰下相机" };
    //易鼎丰定位相机
    QString LS_CameraName::ed_Location{ "易鼎丰定位相机" };
    //侧相机
    QString LS_CameraName::side_camera{ "侧相机" };

    QList<QString> LS_CameraName::allCamera{
        pin2D,
        ed_Up,
        ed_Down,
        ed_Location,
        side_camera
    };
};