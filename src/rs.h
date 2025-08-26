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
    * ActionFlag. ������־
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
    * IOStateEnum. IO��ƽ
    */
    enum IOStateEnum
    {
        Low = 0,
        High
    };

};

/**
* SpeedModeEnum. �ٶ�ģʽ
*/
enum SpeedModeEnum
{
    // ����ģʽ
    Low = 0,
    // ����ģʽ
    Mid,
    // ����ģʽ
    High
};

/**
* ENUMSTATION. ��λ
*/
enum ENUMSTATION
{
    L = 0,
    R,
    Both
};

/**
* ENUMPOINTTYPE. �䷽������
*/
enum ENUMPOINTTYPE
{
    //���ɵ�
    PointNone = 0,
    //2D pin��ʶ���
    Point2D,
    //3Dɨ���
    Point3D,
    //3Dʶ��
    Point3D_identify,
    //ȡ����
    Feeding,
    //����
    TakePhoto,
    //�Ӿ�ģ��
    Visual_Identity,
    //Բ���
    Circle_Measure,
};

/**
* ENUM_VERSION. ����汾
*/
enum ENUM_VERSION
{
    //˫��pin���
    TwoPin = 0,
    //�׶���
    EdPin,
    //���N
    LsPin,
    //��̩
    BtPin,
    //����Pin
    JmPin,
    //����Han
    JmHan
};


/**
* ENUM_FEEDACTION. ȡ���϶���
*/
enum ENUM_FEEDACTION
{
    // ɨ��Mesȷ��
    MesCheck = 0,
    // ץ��_������
    Suction,
    // ����_������
    Unloading,
    //ץ��_�ؾ�
    Suction_2,
    // ����_�ؾ�
    Unloading_2,
};

/**
* ENUM_VISION. �Ӿ�ģ��
*/
enum ENUM_VISION
{
    // ģ��ƥ��
    TempMatch = 0,
    //һά��ģ��
    OneDimensional,
    //��ά��ģ��
    TwoDimensional,
    //Blobģ��
    BlobModel,
    //Բ���ģ��
    CircleModel
};

/**
* ENUM_MOTIONVERSION. �忨�汾
*/
enum ENUM_MOTIONVERSION
{
    //�������忨
    LS_Pulse = 0,
    //�������߿�
    LS_EtherCat,
    //����1000ϵ�����忨
    LS_Pulse_1000,
};

/**
* ENUM_ETHERCAT_STATE. ��״̬
*/
enum ENUM_ETHERCAT_STATE
{
    //δ����״̬
    NOT_READY_SWITCH_ON = 0,
    //������ֹ״̬
    SWITCH_ON_DISABLE,
    //׼������״̬
    READY_TO_SWITCH_ON,
    //����״̬
    SWITCH_ON,
    //����ʹ��״̬
    OP_ENABLE,
    //ֹͣ״̬
    QUICK_STOP,
    //���󴥷�״̬
    FAULT_ACTIVE,
    //����״̬
    FAULT
};

/**
* ENUM_USER. �û�
*/
enum ENUM_USER
{
    //��ͨ�û�
    Normal_User = 0,
    Admin_User
};

/**
* Axis. ��������
*/
namespace LS_AxisName {
    //*************************************** �����ᣬ���ٵ�������Ŀ������
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

    //*************************************** ���Nר������
    extern QString LS_X;
    extern QString LS_Y1;   //�ؾ��ƶ���
    extern QString LS_Y2;   //����ƶ���
    extern QString LS_U;    //3D�����ת��
    extern QString LS_Z;

	extern QList<QString> allAxis;
};


/**
* DI
*/
namespace LS_DI
{
    //*************************************** ˫��pin���
    // ��ť�ź�
    extern QString IN_BtnSetUpL;  // ��������ť
    extern QString IN_BtnSetUpR;  // ��������ť
    extern QString IN_BtnInit;    // ��ʼ����ť
    extern QString IN_BtnStop;    // ��ͣ��ť
    extern QString IN_BtnPause;   // ��ͣ��ť
    extern QString IN_BtnReset;   // ��λ��ť

    // �����ź�
    extern QString IN_OUT_ClampCylinderL1InitP;  // �ξ�1����1ԭ��
    extern QString IN_OUT_ClampCylinderL1EndP;  // �ξ�1����1����
    extern QString IN_OUT_ClampCylinderL2InitP; // �ξ�1����2ԭ��
    extern QString IN_OUT_ClampCylinderL2EndP; // �ξ�1����2����
    extern QString IN_OUT_ClampCylinderR1InitP; // �ξ�2����1ԭ��
    extern QString IN_OUT_ClampCylinderR1EndP; // �ξ�2����1����
    extern QString IN_OUT_ClampCylinderR2InitP; // �ξ�2����2ԭ��
    extern QString IN_OUT_ClampCylinderR2EndP; // �ξ�2����2����

    // ��դ����ȫ�������ź�
    extern QString IN_SafGrate1;  // ��ȫ��դ1
    extern QString IN_SafLock1;   // ��ȫ��1
    extern QString IN_SafLock2;   // ��ȫ��2
    extern QString IN_SafLock3;   // ��ȫ��3
    extern QString IN_Door1;      // �Ž�1
    extern QString IN_Door2;      // �Ž�2
    extern QString IN_Door3;      // �Ž�3
    extern QString IN_Door4;      // �Ž�4
    extern QString IN_Door5;      // �Ž�5
    extern QString IN_Door6;      // �Ž�6

    // �����ź�
    extern QString IN_LPrdOK;     // �ξ�1�����ź�
    extern QString IN_RPrdOK;     // �ξ�2�����ź�
    extern QString IN_NG1;        // NG1
    extern QString IN_NG2;        // NG2
    extern QString IN_NG3;        // NG3

    //*************************************** ���N
    // ��ť�ź�
    extern QString LS_IN_BtnSetUp;       // ������ť
    extern QString LS_IN_BtnStop;        // ֹͣ��ť
    extern QString LS_IN_BtnReset;        // ��λ��ť
    extern QString LS_IN_BtnEmergency;    // ��ͣ��ť
    extern QString LS_IN_BtnLight;        // ������ť
    extern QString LS_IN_MainAirSupply;      // ��̨����Դ

    // �ؾ߼н��ź�
    extern QString LS_IN_ClampCyl1_Home;     // �ؾ߼н�����1ԭ��
    extern QString LS_IN_ClampCyl1_Work;     // �ؾ߼н�����1����
    extern QString LS_IN_ClampCyl2_Home;     // �ؾ߼н�����2ԭ��
    extern QString LS_IN_ClampCyl2_Work;     // �ؾ߼н�����2����

    // ����ź�
    extern QString LS_IN_VacuumDetect;       // ��צ��ռ��
    extern QString LS_IN_MaterialDetect;     // �ؾ����ϼ��

    // �Ž��ź�
    extern QString LS_IN_GateFront1;       // ǰ1�Ž�����
    extern QString LS_IN_GateFront2;       // ǰ2�Ž�����
    extern QString LS_IN_GateFront3;       // ǰ3�Ž�����
    extern QString LS_IN_GateFront4;       // ǰ4�Ž�����
    extern QString LS_IN_GateRear1;        // ��1�Ž�����
    extern QString LS_IN_GateRear2;        // ��2�Ž�����
    extern QString LS_IN_GateRear3;        // ��3�Ž�����
    extern QString LS_IN_GateRear4;        // ��4�Ž�����

    // SMEMA & ���������ź�(����������)
    extern QString LS_IN_SMEMA_UpFront_AllowOut; // I8.0 SMEMA �� ǰ���������
    extern QString LS_IN_SMEMA_LowFront_AllowOut;// I8.1 SMEMA �� ǰ���������
    extern QString LS_IN_SMEMA_UpRear_AllowOut; // I8.2 SMEMA �� ����������
    extern QString LS_IN_SMEMA_LowRear_AllowOut; // I8.3 SMEMA �� ����������
    extern QString LS_IN_UpperMaterialDetect;    // I8.4 �ϲ㵽�ϸ�Ӧ
    extern QString LS_IN_UpperOutputDetect;      // I8.5 �ϲ���ϸ�Ӧ
    extern QString LS_IN_LowerOutputDetect;      // I8.6 �²���ϸ�Ӧ
    extern QString LS_IN_BlockCyl1_Work;        // I8.7 �赲����1����
    extern QString LS_IN_BlockCyl1_Home;        // I9.0 �赲����1ԭ�� 1���赲״̬
    extern QString LS_IN_BlockCyl2_Work;        // I9.1 �赲����2����
    extern QString LS_IN_BlockCyl2_Home;        // I9.2 �赲����2ԭ��
    extern QString LS_IN_LiftCyl_Work;          // I9.3 ��������
    extern QString LS_IN_LiftCyl_Home;          // I9.4 ����ԭ��
    extern QString LS_IN_LowerMaterialDetect;          // I9.5 �²����ϸ�Ӧ
    extern QString LS_IN_UpperCheckProduct;     //�ؾ������ϼ��

    //------------------------------------------------ �׶���ר��DI�źţ��豸���У�
    // ��ť��
    extern QString ED_DI_BtnStart;          // ������ť
    extern QString ED_DI_BtnStop;           // ֹͣ��ť
    extern QString ED_DI_BtnReset;          // ��λ��ť
    extern QString ED_DI_SwitchAutoMode;    // �Զ�ģʽ��ť
    extern QString ED_DI_BtnEStop1;        // ��ͣ��ť1
    extern QString ED_DI_BtnEStop2;        // ��ͣ��ť2

    // ����λ�ü��
    extern QString ED_DI_ClampPosCyl1_Home; // �ξ߶�λ����1ԭ��
    extern QString ED_DI_ClampPosCyl1_Work; // �ξ߶�λ����1����
    extern QString ED_DI_ClampPosCyl2_Home; // �ξ߶�λ����2ԭ��
    extern QString ED_DI_ClampPosCyl2_Work; // �ξ߶�λ����2����
    extern QString ED_DI_ClampCyl1_Home;    // �ξ߼н�����1ԭ��
    extern QString ED_DI_ClampCyl1_Work;    // �ξ߼н�����1����
    extern QString ED_DI_ClampCyl2_Home;    // �ξ߼н�����2ԭ��
    extern QString ED_DI_ClampCyl2_Work;    // �ξ߼н�����2����

    // ��ȫ�ż��
    extern QString ED_DI_ProdDetect;        // �ξ߲�Ʒ���޼��
    extern QString ED_DI_SafDoor_Feed1;    // ���ϲలȫ��1
    extern QString ED_DI_SafDoor_Feed2;    // ���ϲలȫ��2
    extern QString ED_DI_SafDoor_Feed3;    // ���ϲలȫ��3
    extern QString ED_DI_SafDoor_Feed4;    // ���ϲలȫ��4
    extern QString ED_DI_SafDoor_Discharge1; // ���ϲలȫ��1
    extern QString ED_DI_SafDoor_Discharge2; // ���ϲలȫ��2
    extern QString ED_DI_SafDoor_Discharge3; // ���ϲలȫ��3
    extern QString ED_DI_SafDoor_Discharge4; // ���ϲలȫ��4
    extern QString ED_DI_SafDoor_NG1;      // NGƤ���లȫ��1
    extern QString ED_DI_SafDoor_NG2;      // NGƤ���లȫ��2
    extern QString ED_DI_SafDoor_Rear1;    // ���氲ȫ��1
    extern QString ED_DI_SafDoor_Rear2;    // ���氲ȫ��2

    // �����ź�
    extern QString ED_DI_AirPressureAlarm; // CDA����ѹ������

    // Ƥ�������
    extern QString ED_DI_FeedBeltMaterialDetect;      // ����Ƥ����Ʒ���ϼ��
    extern QString ED_DI_FeedBeltArrivalDetect;       // ����Ƥ����Ʒ��λ���
    extern QString ED_DI_DischargeBeltReleaseDetect;  // ����Ƥ����Ʒ���ϼ��
    extern QString ED_DI_DischargeBeltArrivalDetect;   // ����Ƥ����Ʒ��λ���
    extern QString ED_DI_NGBeltReleaseDetect_Left;    // NGƤ����Ʒ���ϼ��1����
    extern QString ED_DI_NGBeltReleaseDetect_Right;   // NGƤ����Ʒ���ϼ��2���ң�
    extern QString ED_DI_NGBeltFullDetect;            // NGƤ����Ʒ���ϼ��

	extern QList<QString> allDI;
}


/**
 * DO
 */
namespace LS_DO
{
    //------------------------------------------------ �ɸ���DO
    extern QString DO_LightRed;    // ��ɫ��-��
    extern QString DO_LightGreen;  // ��ɫ��-��
    extern QString DO_LightYellow; // ��ɫ��-��
    extern QString DO_Buzzer;      // ��ɫ��-������
    extern QString DO_Cam2DTrigger;    // 2D�������
    extern QString DO_Cam3DTrigger;    // 3D�������
    extern QString DO_Light2DTrigger;  // 2D��Դ����
    extern QString DO_StartBtnLED;     // ������ťָʾ��
    extern QString DO_StopBtnLED;      // ֹͣ��ťָʾ��

    //------------------------------------------------ ˫��pin���
    // ִ�л���
    extern QString DO_ClampCylinderL1;  // �ξ�1����1
    extern QString DO_ClampCylinderL2;  // �ξ�1����2
    extern QString DO_ClampCylinderR1;  // �ξ�2����1
    extern QString DO_ClampCylinderR2;  // �ξ�2����2
    extern QString DO_Lock1;            // ������1
    extern QString DO_Lock2;            // ������2
    extern QString DO_Lock3;            // ������3

    // ָʾ��/��Ϣ��
    extern QString DO_LightFlood;  // ������

    // ���豸����
    extern QString DO_PlusCam3D1;      // 3D����-X
    extern QString DO_PlusCam3D2;      // 3D����-Y1
    extern QString DO_PlusCam3D3;      // 3D����-Y2
    extern QString DO_ReadCodeTrigger1; // ɨ��ǹ����1
    extern QString DO_ReadCodeTrigger2; // ɨ��ǹ����2

    // ָʾ��
    extern QString DO_LightOK_L;  // ָʾ��1-��
    extern QString DO_LightNG_L;  // ָʾ��1-��
    extern QString DO_LightOK_R;  // ָʾ��2-��
    extern QString DO_LightNG_R;  // ָʾ��2-��

    //*************************************** ���N
    extern QString LS_OUT_RedLight;        // ��ɫ��-���
    extern QString LS_OUT_YellowLight;     // ��ɫ��-�Ƶ�
    extern QString LS_OUT_GreenLight;      // ��ɫ��-�̵�
    extern QString LS_OUT_Buzzer;          // ������
    extern QString LS_OUT_StartBtnLED;     // ������ťָʾ��
    extern QString LS_OUT_StopBtnLED;      // ֹͣ��ťָʾ��
    extern QString LS_OUT_ResetBtnLED;     // ��λ��ťָʾ��
    extern QString LS_OUT_LightBtnLED;     // ������ťָʾ��
    extern QString LS_OUT_ClampSuck;       // ��צ����
    extern QString LS_OUT_ClampBlow;       // ��צ����
    extern QString LS_OUT_Lighting;        // ����
    extern QString LS_OUT_ClampCyl1;       // �ؾ߼н�����1
    extern QString LS_OUT_ClampCyl2;       // �ؾ߼н�����2
    extern QString LS_OUT_2DLightTrigger;  // 2D��Դ����
    extern QString LS_OUT_2DCamTrigger;    // 2D�������
    extern QString LS_OUT_3DCamTrigger;    // 3D�������
    // SMEMA �����źţ����涼�Ǳ��������źţ�
    extern QString LS_OUT_SMEMA_UpFront_FeedReady;   // Q6.0 SMEMA �� ǰ�����ϵ�λ�ź�
    extern QString LS_OUT_SMEMA_LowFront_FeedArrived;// Q6.1 SMEMA �� ǰ�����ϵ����ź�
    extern QString LS_OUT_SMEMA_UpRear_FeedReady;    // Q6.2 SMEMA �� ������ϵ�λ�ź�
    extern QString LS_OUT_SMEMA_LowRear_FeedReady;   // Q6.3 SMEMA �� ������ϵ�λ�ź�
    // ��������ź�
    extern QString LS_OUT_UpperMotor_Run;           // Q6.4 �ϲ���RUN
    extern QString LS_OUT_UpperMotor_Dir;           // Q6.5 �ϲ���Dir
    extern QString LS_OUT_LowerMotor_Run;           // Q6.6 �²���RUN
    extern QString LS_OUT_LowerMotor_Dir;           // Q6.7 �²���Dir
    // ���׿����ź�
    extern QString LS_OUT_BlockCyl_Activate;        // Q7.0 �赲���� 0���赲��������һֱ�赲��
    extern QString LS_OUT_LiftCyl_Activate;         // Q7.1 ��������

    //------------------------------------------------ �׶���ר��DO
    extern QString ED_DO_PositionCyl1;      // DO1.0 ��λ���׵�ŷ�1
    extern QString ED_DO_PositionCyl2;      // DO1.1 ��λ���׵�ŷ�2
    extern QString ED_DO_ClampCyl1_Valve1;  // DO1.2 �н�����1��ŷ�1
    extern QString ED_DO_ClampCyl1_Valve2;  // DO1.3 �н�����1��ŷ�2
    extern QString ED_DO_CamFrontTrigger;   // DO2.0 �������Ӳ����
    extern QString ED_DO_CamRearTrigger;    // DO2.1 �������Ӳ����
    extern QString ED_DO_LightFrontTrigger; // DO2.4 ���������Դ����
    extern QString ED_DO_LightRearTrigger;  // DO2.5 ���������Դ����
    extern QString ED_DO_LightPositioningTrigger;  // DO4.7 ��λ�����Դ����
    // ����������
    extern QString ED_DO_FeedBeltStart;               // ����Ƥ������
    extern QString ED_DO_DischargeBeltStart;         // ����Ƥ������
    extern QString ED_DO_NGBeltStart;                // NGƤ������

    // ���� DO �źż���
    extern QList<QString> allDO;
}

/**
* Camera. �����
*/
namespace LS_CameraName {
    extern QString pin2D;
    //�׶��������
    extern QString ed_Up;
    //�׶��������
    extern QString ed_Down;
    //�׶��ᶨλ���
    extern QString ed_Location;
    //�����
    extern QString side_camera;

    extern QList<QString> allCamera;
};

// PointItemConfig ��Ķ��壨�����䷽���ÿһ����Ϣ��
class PointItemConfig {
public:
    //������ҷֿ���Ļ������⼸��������ʱû���ã��Ȳ��ų���
    //int SlnId = 0;
    int StationL = ENUMSTATION::L;
    //int SortIndex = 0;

    //�����ͣ�2D,3D�����ɵ㣩
    int PointType = -1;

    double TargetPX = 0.0;
    double TargetPY = 0.0;
    double TargetPZ = 0.0;
    double TargetPRZ = 0.0;
    double TargetPU = 0.0;

    // �����᲻ͬר��ֱ����ͬһ����������˳�����
    double TargetPAxis_1 = 0.0;
    double TargetPAxis_2 = 0.0;
    double TargetPAxis_3 = 0.0;
    double TargetPAxis_4 = 0.0;
    double TargetPAxis_5 = 0.0;
    double TargetPAxis_6 = 0.0;
    double TargetPAxis_7 = 0.0;

    //�ٶ�ģʽ
    int SpeedModel = SpeedModeEnum::Low;
    //�Ӿ��䷽��
    QString Vision = "";
    //��ע
    QString Tip = "";
    //3D������
    QString CloudName = "";
    //��ͷ��
    QString ProductName = "";
    //����3D����
    int isSave3dCloud = 0;
    //ȡ���϶���
    int FeedAction = ENUM_FEEDACTION::MesCheck;
    //�������
    QString CameraName;
    //�Ӿ�����
    int VisionConfig = ENUM_VISION::TempMatch;
    //��Ƭ��
    QString PhotoName = "";
    //�ͺţ���ͬ�汾�п������ϣ�
    QString ModelName = "";
    //����ϲ�
    int isResultMerging = 0;
    //�Ӿ��䷽����
    QString VisionTwo = "";
    //3Dɨ������
    int scanRow3d = 2000;
    //Բ������׼ֵ
    double CircleBenchmark = 0;
    //Բ������ƫ��
    double CircleUpError = 0;
    //Բ������ƫ��
    double CircleDownError = 0;
    //1mm��Ӧ������ֵ
    double PixDistance = 0;
    //���������ͼ�������һ���Ƿ�NG
    bool isNG = false;
    //�ع�ʱ��
    int ExposureTime = 0;

    //// ͨѶ��ʽ����ʱû����
    //bool ModWrite;
    //bool ModRead;
    //QString RegisterIdWrite;
    //QString RegisterIdRead;
    //QString ValueWrite;
    //QString ValueRead;

    // ���캯��
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

//���ͳ��
struct Run_Result
{
    //2D
    bool isNG_2D = false;
    //3D
    bool isNG_3D = false;
    //��
    bool isNG = false;
    //һά��ģ��
    bool isNG_OneDimensional = false;
    //��ά��ģ��
    bool isNG_TwoDimensional = false;
    //Blobģ��
    bool isNG_Blob = false;
    //Բ���
    bool isNG_Circle = false;
    //ģ��ƥ��
    bool isNG_Temple = false;
    //Բ���
    bool isNG_CircleMeasure = false;
    //�ͺ���
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

//-------------------------- ���N
// ��������ṹ��
struct SnCheckRequest {
    QString cmd = "QUERY_RECORD";   //��վǰ�̶�ֵ
    QString p = "UOP";
    QString station_name = "DEF_L-ASS-14_1_PIN�봹ֱ����";    // ��λ���� (����)
    QString sn;              // ������ (����)
    QString op;              // �û��˺� (����)
    QString csn;             // �ͻ����� (��ѡ)
    QString tooling_no;      // RFID (��ѡ)
    QString machine_no;      // �豸���� (��ѡ)
    QString first_sign;       // �׼���ʶ (��ѡ)
};

// ��Ӧ����ṹ��
struct SnCheckResponse {
    int code;                // 200=�ɹ�, 500=ʧ��
    QString msg;
    QString main_sn;
    QString first_sign;
    QString rh_first_sign;
    QString IsStay;
};


// ���Խ������
struct TestResult {
    QString name;          // ����������
    QString units;          // ��λ
    QString result;         // ���Խ��
    QString message;        // ������Ϣ

    QJsonObject toJson() const {
        return {
            {"name", name},
            {"units", units},
            {"result", result},
            {"message", message}
        };
    }
};


// ��վ��������ṹ��
struct SnResultRequest {
    QString cmd = "ADD_RECORD"; // �̶�ֵ
    QString station_name = "DEF_L-ASS-14_1_PIN�봹ֱ����";       // ��λ����
    QString sn;                     // ��Ʒ���к�
    QString op;                     // ����Ա���
    QString op_time;                // ����ʱ��
    QString result;                 // ���ս��
    QString start_time;             // ��ʼʱ��
    QString stop_time;              // ����ʱ��
    QList<TestResult> result_data;  // �������ݼ���

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

//�Ӿ�ʶ��ģ��ͼƬ
struct Vision_Message
{
    HObject img;
    //ģ��ƥ��ģ����Ϣ
    vector<std::pair<PointItemConfig,ResultParamsStruct_Template>> TempMes;
    //һά��ģ����Ϣ
    vector< std::pair<PointItemConfig, ResultParamsStruct_BarCode>> BarMes;
    //��ά��ģ����Ϣ
    vector< std::pair<PointItemConfig, ResultParamsStruct_Code2d>> QRMes;
    //Blobģ����Ϣ
    vector< std::pair<PointItemConfig, ResultParamsStruct_Blob>> BlobMes;
    //Բģ����Ϣ
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

//������λ���
struct RobotResult {

    cv::Mat original;
    cv::Mat display;
    float consume;
    double x, y, a;
    long long timestamp;
};

//�׶�������˶�ȡ����
struct ED_Robot_Connect
{
    //40035(34)
    bool feedInBeltPickDone = 0;  // Bit0: ����Ƥ��ȡ�����
    bool fixturePlaceDone = 0;  // Bit1: ����ξ߷������
    bool fixturePickDone = 0;  // Bit2: ����ξ�ȡ�����
    bool dischargeBeltPlaceDone = 0; // Bit3: ����Ƥ���������
    bool ngBeltLeftPlaceDone = 0;  // Bit4: NGƤ����������
    bool ngBeltRightPlaceDone = 0;  // Bit5: NGƤ���ҷ������
    bool haveProduct = 0;  // Bit6: ȡ���������ź�
    bool haveProductB = 0;  // Bit7: B��ȡ���������ź�
    //40039(38)
    int errorMessage = 0;//�����źű�����
    //40001(0) �����˷���ֵ
    bool manualStatus = 0;  // Bit0: �ֶ�״̬
    bool autoStatus = 0;  // Bit1: �Զ�״̬
    bool remoteStatus = 0;  // Bit2: Զ��״̬
    bool servoStatus = 0;  // Bit3: �ŷ�״̬
    bool alarmStatus = 0;  // Bit4: ����״̬
    bool emergencyStopStatus = 0;  // Bit5: ��ͣ״̬
    bool programRunningStatus = 0;  // Bit6: ��������״̬
    bool safePosition1Status = 0;  // Bit7: ��ȫλ��1״̬
    bool safePosition2Status = 0;  // Bit8: ��ȫλ��2״̬
    bool safePosition3Status = 0;  // Bit9: ��ȫλ��3״̬
    bool safePosition4Status = 0;  // Bit10: ��ȫλ��4״̬
    bool programLoadStatus = 0;  // Bit11: �������״̬
    bool servoReadyStatus = 0;  // Bit12: �ŷ�׼��״̬
    bool programReservation = 0;  // Bit13: ����ԤԼ����״̬
    bool programResetStatus = 0;  // Bit14: ����λ״̬
    //����˶�����ź�
    void clearMotion()
    {
        feedInBeltPickDone = 0;  // Bit0: ����Ƥ��ȡ�����
        fixturePlaceDone = 0;  // Bit1: ����ξ߷������
        fixturePickDone = 0;  // Bit2: ����ξ�ȡ�����
        dischargeBeltPlaceDone = 0; // Bit3: ����Ƥ���������
        ngBeltLeftPlaceDone = 0;  // Bit4: NGƤ����������
        ngBeltRightPlaceDone = 0;  // Bit5: NGƤ���ҷ������
        haveProduct = 0;
        haveProductB = 0;
        errorMessage = 0;
    };
    //��ջ�����״̬�ź�
    void clearState()
    {
        manualStatus = 0;  // Bit0: �ֶ�״̬
        autoStatus = 0;  // Bit1: �Զ�״̬
        remoteStatus = 0;  // Bit2: Զ��״̬
        servoStatus = 0;  // Bit3: �ŷ�״̬
        alarmStatus = 0;  // Bit4: ����״̬
        emergencyStopStatus = 0;  // Bit5: ��ͣ״̬
        programRunningStatus = 0;  // Bit6: ��������״̬
        safePosition1Status = 0;  // Bit7: ��ȫλ��1״̬
        safePosition2Status = 0;  // Bit8: ��ȫλ��2״̬
        safePosition3Status = 0;  // Bit9: ��ȫλ��3״̬
        safePosition4Status = 0;  // Bit10: ��ȫλ��4״̬
        programLoadStatus = 0;  // Bit11: �������״̬
        servoReadyStatus = 0;  // Bit12: �ŷ�׼��״̬
        programReservation = 0;  // Bit13: ����ԤԼ����״̬
        programResetStatus = 0;  // Bit14: ����λ״̬
    };
};

//������־
struct AXIS_HOME
{
    bool U = false;
    bool U1 = false;
};

//���ݿ�����
struct SQL_Parameter
{
    QString formulaName;
    QString snName;
};

//-------------------------- �׶���MES
//��վ����
struct MesCheckInRequest_ED {
    QString sn;          // ������ (����)
    QString workPlaceCode = "��Զ�Ӿ����";   // ����վ�� (����)
    QString laneId = "1";        // ��� Ĭ��1
    QString checkAllRef = "true";   // �Ƿ���С�� Ĭ��true
    QString productSn;
};

//���ز���
struct MesCheckInResponse_ED {
    bool state = true;         // �ӿ�״̬: true-�ϸ��������, false-���ϸ��������
    QString description; // �쳣����: state=trueʱΪ��, state=falseʱΪ������ʾ
};

// �׶���MES��վ����ṹ��
struct MesCheckInFullRequest_ED {
    QString WorkPlaceCode = "��Զ�Ӿ����";
    QString WorkOrderCode = "�Ӿ��������Թ���";
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

