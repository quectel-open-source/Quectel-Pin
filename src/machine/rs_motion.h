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
		//��Ϊ���������ܺ����漰���߳�
		return motion;
	};

    void destroyInstance();

	//������ʼ��
	void init();
	//���ƿ���ʼ��
	bool motionInit();
	//��������
	void saveMotionConfig();
	//������ȡ
	void loadMotionConfig();
	//canIO��ʼ��
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
    //��������
    int AxisHome(QString axis);
    //��������(����)
    int AxisHomeAsync(QString axis);
    int GoHome(int CardId, int AxisId);
    void AxisClearPosition(int CardId, int AxisId);
    bool IsRuning(int CardId, int AxisId);
    void AxisStopAll();
    //speed ����1000�汾�忨,�����汾ͨ��setSpeedд���������
    int VMove(int CardId, int AxisId, bool pdir = true, double speed = 0);
    //speed ����1000�汾�忨,�����汾ͨ��setSpeedд���������
    int AxisVmove(QString axis, bool postive, double speed = 0);
    void WriteAxisRunVel(QString axis, double runvel);

    double ReadPosition(int CardId, int AxisId);
    double ReadAxisP(QString axis);
    void ReadAllAxisP();
    void WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed);
    void WriteAxisSpeedModel(QString axis, SpeedModeEnum model);
    void ResetSetAxisParm(QString axis);
    bool ReadServOn(QString axis);
    //�����ٶ�
    int setSpeed(QString axisName, double speed);
    // ��ȡ���Ƿ񱨾�״̬
    bool isAxisErc(QString axis);
    // ��ȡ�ᱨ���˿ڵ�ƽ
    bool isAxisErc(int CardId, int AxisId);
    // ����ᱨ��״̬
    void AxisClear(QString axis);
    void AxisClear(int CardId, int AxisId);
    // ���������˶�
    int PMove_abs(int CardId, int AxisId, double value);
    // ���������˶����ᣩ (speed ����1000�汾�忨)
    int PMove_abs(QString axis, double value,double speed = 0);
    // ��������˶�
    void PMove_rel(int CardId, int AxisId, double value);
    // ��������˶����ᣩ
    void PMove_rel(QString axis, double value);
    // �ᶨ�������˶�������ִ�У����ᵽλ�󷵻� (speed ����1000�汾�忨)
    int AxisPmoveAsync(QString axis, double value, double speed = 0);
    // �ȴ��Ḵλ����ʼλ�ú󷵻�(����)
    int AxisInitPAsync(QString axis);
    // �Ḵλ����ʼλ��
    int AxisInitP(QString axis);
    // ��λ����
    int TaskReset();
    int TaskReset_LS();
    int TaskReset_ED();
    int TaskReset_BT();
    int TaskReset_JMPin();
    int TaskReset_JMHan();
    // ��λ��λָʾ��
    void ResetStationLight(bool isstationL = true);
    // �������״̬
    int TaskGoWating(ENUMSTATION stations);
    bool isInitP(QString axis);
    bool isRuning(QString axis);
    //�Ƿ񵽴�ԭ��
    bool isHomeP(QString axis);
    // ���������䷽
    void saveFormulaToFile();
    // ���������䷽
    void loadFormulaFromFile();
    // ���嶯����������
    int runAction(QList<PointItemConfig> listProcess);
    //ǰ������ 1. 3d��λִ��ǰ�������ӳ� ,��������������
    int ItemPreProcess(PointItemConfig item);
    QString DetectPulsAxis(PointItemConfig item);
    //�ж����˶��Ƿ�����
    bool isMovePostive(QString axis, double target);
    // ItemPart �ᶯ��
    int ItemMovingProcess(PointItemConfig item);
    // �ᶨ�������˶� (speed ����1000�汾�忨)
    int AxisPmove(QString axis, double value,double speed = 0);
    // ItemPart ��������
    //  1. 3d��λִ�к󱣳֣�ȡ��
    //  2. 2d��λִ�к󣬵�λ���� �ӳ� ���ֵƹ�
    int ItemLastProcess(PointItemConfig item);
    // 2D�����������
    int catPhoto2D(std::string cameraName, std::shared_ptr<CMvCamera>& camera,int exposureTime = 0);
    // ��ȡ��ɵ��ļ�·��
    QString getOldestFileInDirectory(const QString& directoryPath);
    // ����ļ���
    bool deleteAllFilesInDirectory(const QString& directoryPath);
    // pin���չʾ
    int pinShowResult(Run_Result& reslut);
    //3D�豸��ʼ��
    void init3dCamera();
    // ���� 3D ���Ƶ��߼�����
    void process3Didentify(pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPoint, PointItemConfig item);
    // 3D����ں�
    void summary3dResult();
    // ���� 2D ʶ����߼�����
    void process2Didentify(HObject img, PointItemConfig item);
    // �������
    void destroyCamera();
    // ˫��pin�������
    int pin2MotionAction(PointItemConfig item);
    // ���Npin�������
    int pinLsAction(PointItemConfig item);
    // �׶���pin�������
    int pinEdAction(PointItemConfig item);
    // ����pin�������
    int pinJmAction(PointItemConfig item);
    // ���������������
    int hanJmAction(PointItemConfig item);
    // ��ʼ���ᣬIO��
    void initAxisAndIO();
    // ���������Ƽ����ϼ��
    int feedingAction();
    // ���������Ƽ����ϼ��
    int unloadingAction();
    // �²㱶��������
    int downFeedingAction();
    // һά��ʶ��
    int barCodeIdentify(PointItemConfig item);
    // ��ά��ʶ��
    int QrCodeIdentify(PointItemConfig item);
    // ģ��ƥ��
    int templateIdentify(PointItemConfig item);
    // Blob����
    int blobIdentify(PointItemConfig item);
    // Բ���
    int circleIdentify(PointItemConfig item);
    // Բ���
    int circleMeasure(PointItemConfig item);
    //��DO
    void closeDO();
    //��ɫ��ָʾ�� 0���̵ƣ�1���Ƶƣ�2�����
    void setThreeColorLight(int color,bool isClose = false);
    //��ťָʾ�� 0��������1����λ��2��ֹͣ��3����������4������
    void setButtonLight(int type,int val);
    //���ռ��SN
    SnCheckResponse check_sn_process(const QString& url, const SnCheckRequest& req);
    //��վ���
    SnCheckResponse post_result_process(const QString& url, const SnResultRequest& req);
    //�������״η��ϵ��ؾߺ���
    int runRobotCheck();
    //���������Ϻ���
    int runRobotUnLoading();
    //ʵʱ��ػ�����Ƥ��ȡ��
    int runRobotLoad_Belt();
    //�����˱������
    int robotCheck(int err);
    //���������ϵ�Ƥ������
    int runRobotUnLoad_Belt(bool isNG);
    //�����˷��ϵ��ξ�
    int runRobotLoad_Tool();
    //����������
    int runRobotClearHand();
    //������ʼ���������
    void initAllVariable();
    // ��ʼ��CSV�ṹ����
    void initCsvPara();
    // ͨ���豸���MES����
    void checkMesFunction();
public:
    //����汾
    int m_version{ ENUM_VERSION::TwoPin };
    //�忨�汾
    int m_motionVersion{ ENUM_MOTIONVERSION::LS_Pulse };
    //�忨״̬
    bool m_cardInitStatus{ false };
    //��������
    int m_triggerMotionTpyes{ 0 };

    //�������б�־λ �������Ļ����������Ҳ���������У�
    bool m_isRunOK = false;
    //���б�־λ
    std::atomic<bool> m_isStart = false;
    std::atomic<bool> m_isLoop = false;
    //�����־λ
    std::atomic<bool> m_isHomming = false;
    //������ɱ�־λ
    std::atomic<bool> m_isHomed = false;
    //��ͣ��־λ
    bool m_isPause = false;
    //ֹͣ��־λ(ֹͣ����������������Ҳ��������)
    std::atomic<bool> m_isRunStop = false;
    //ȡ�����ж�����־λ ����ͣ��ֹͣ��
    std::atomic<bool> m_isEmergency = false;
    //ʹ�ܱ�־λ
    bool isServeOn = false;
    //�������б�־λ
    std::atomic<bool> m_isOneStep = false;
    //�����
    //std::shared_ptr<CMvCamera> m_pcMyCamera = nullptr;
    //std::unique_ptr<Camera3D> m_pcMyCamera_3D = nullptr;
    std::shared_ptr<VisionTools> m_pcMyCamera_3D = nullptr;

    //�������Ӧ�����ʵ���ֵ�
    std::unordered_map<QString, std::shared_ptr<CMvCamera>> m_cameraMap;

    //�䷽��Ϣ
    struct Formula_Config
    {
        //int id = -1;
        //QString name = "";
        QList<PointItemConfig> ListProcess_L;
        QList<PointItemConfig> ListProcess_R;
    };
    //�䷽�б�
    std::unordered_map<QString,Formula_Config> m_Formula;
    //��Ҫ���еĲ����б�
    QList<PointItemConfig> m_runListProcess_L;
    QList<PointItemConfig> m_runListProcess_R;
    //���е��Ӿ��䷽˳���б�
    QList<QString> m_visionFormula_2D;
    QList<QString> m_visionFormula_3D;
    //ִ�в�������
    int m_showStepIndex_L = 0;
    int m_showStepIndex_R = 0;
    //pin 2D�����
    QList<std::pair<ResultParamsStruct_Pin, PointItemConfig>> m_pinResult2D;
    //pin 3D�����
    QList<std::pair<ResultParamsStruct_Pin3DMeasure, PointItemConfig>> m_pinResult3D;
    // ������������3D����(�������ظ���������ȴ���ݲ�ͬ)
    std::unordered_map< QString,pcl::PointCloud<pcl::PointXYZ>::Ptr> m_cloud_ori_map;
    // ����2D��Ƭ
    std::unordered_map< QString, HObject> m_photo_map;
    //����� QString����λ��������std::string MVS�����
    std::unordered_map<QString, std::string> m_cameraName;
    //һά������
    QList<std::pair<ResultParamsStruct_BarCode, PointItemConfig>> m_barCodeResult;
    //��ά������
    QList<std::pair<ResultParamsStruct_Code2d, PointItemConfig>> m_qrCodeResult;
    //ģ��ƥ������
    QList<std::pair<ResultParamsStruct_Template, PointItemConfig>> m_TemplateResult;
    //blob�����
    QList<std::pair<ResultParamsStruct_Blob, PointItemConfig>> m_BolbResult;
    //Բ�����
    QList<std::pair<ResultParamsStruct_Circle, PointItemConfig>> m_CircleResult;
    //mes ��վǰ���
    SnCheckRequest m_snReq;
    //mes ��վ��Ϣ
    SnResultRequest m_snResult;
    //ɨ���ȡ����Ϣ
    QString ls_tcpMess = "";
    QMutex m_networkMutex;
    //��Ҫ�Ͳ������ͼƬ
    std::unordered_map<QString, Vision_Message> m_resultMergingImg;
    bool inDI = false;
    bool outDI = false;
    bool notMes = false;
    bool notQR = false;
    bool m_notDoor = false;
    bool m_notRobot = false;
    QString testSN = "";
    QString m_forMulaName = "";
    //�׶�������˶�ȡ�ı����ṹ��
    ED_Robot_Connect m_RobotModbus;
    //���н��
    Run_Result m_runResult;
    //���̿�ʼʱ��
    QString m_runStartTime;
    //������ȡ����
    std::mutex m_robotLoadingMutex;  // ������
    std::atomic<bool> m_robotLoadingRunning{ false };  // ԭ��״̬��־����ѡ��
    //��������־
    AXIS_HOME m_axisHomeState;
    //�����˷��ϻ�ȡ����
    std::atomic<bool> m_isRobotLoading = false;
    //���ͼ·��
    QString m_resultImgPath;
    //ԭͼ·��
    QString m_originalImgPath;
    //������Ƥ�����ϱ�����־λ
    std::atomic<bool> m_isLoadErr = false;
    //��ɫָʾ����˸
    QTimer m_redLightFlashing;
    //�׶��� mes ��վǰ��鷵����Ϣ
    MesCheckInResponse_ED m_mesResponse_ED;
    //���󵼳�������(�׶������)
    std::unordered_map<QString, QString> m_ResultToCsv;
    //�����ı�ͷ˳��
    std::vector<QString> m_csvOrder;
    //����������
    rs_FunctionTool m_functionTool;
    //SN��
    QString m_requestSn;
    //�Ӿ������
    std::mutex m_visionMutex;
    //����ԭͼ
    bool m_saveImageEnabled;
    //�ش󱨴����
    QString m_errCode;
    //�䷽�ͻ����˷�����Ӧ(�׶������)
    std::unordered_map<QString, int> m_formulaToRobot;

	//����������
    struct Axis_Config
    {
        // --- ������������Ҫ����Ͷ�ȡ�� ---
        // ����
        int card = 0;
        // ���
        int axisNum = -1;
        // ��ʼλ
        double initPos = 0.0;
        // ��С�г�
        double minTravel = 0.0;
        // ����г�
        double maxTravel = 0.0;
        // ���㷽��
        int homeDir = 0;
        // ���嵱��
        double unit = 0.0;
        // ��С��
        double minSpeed = 0.0;
        // ����
        double midSpeed = 0.0;
        // �����
        double maxSpeed = 0.0;
        // ����ʱ��
        double accTime = 0.0;
        // ����ʱ��
        double decTime = 0.0;
        // S ʱ��
        double sTime = 0.0;
        // ��ͣ IO
        int stopIO = -1;

        // --- ���������������Ҫ����Ͷ�ȡ������֮ǰ�˿��������ҳ��û�У��Ȳ��ܣ� ---
        // ������ (0: ������, 1: ������, 2: Ԥ��, 3: ������, 4: ����+EZ)
        int axisType = 0;
        // �˶���ʽ (0: ����˶�, 1: �����˶�)
        int moveModel = 0;
        // ֹͣ�ٶ�
        double stopVel = 0.0;

        // ����ģʽ
        unsigned short homeModel = SpeedModeEnum::Low;
        // �����ٶ�
        double homeVel = 0.0;
        // ���������ٶ�
        double homeCreepVel = 0.0;

        // ԭ���ź�ͨ��
        int orgChannel = -1;
        // ԭ���ź���Ч��ƽ (0: �͵�ƽ, 1: �ߵ�ƽ)
        int orgValidLevel = 1;

        // �Ƿ񼤻�����Ӳ��λ
        bool isActiveHardEl = true;

        // ����Ӳ��λͨ��
        int elpHardChannel = -1;
        // ����Ӳ��λ��Ч��ƽ (0: �͵�ƽ, 1: �ߵ�ƽ)
        int elpValidLevel = 0;
        // ��������λ�趨ֵ
        //int elpSoftValue = std::numeric_limits<int>::max();
        int elpSoftValue = INT_MAX;

        // ����Ӳ��λͨ��
        int elnHardChannel = -1;
        // ����Ӳ��λ��Ч��ƽ (0: �͵�ƽ, 1: �ߵ�ƽ)
        int elnValidLevel = 1;
        // ��������λ�趨ֵ
        //int elnSoftValue = std::numeric_limits<int>::min();
        int elnSoftValue = INT_MIN;

        // �ŷ���ͣͨ����Ч��ƽ (0: �͵�ƽ, 1: �ߵ�ƽ)
        int emgValidLevel = 0;

        // --- �������������������ʱʹ�ã�����Ҫ����Ͷ�ȡ�� ---
        // ��ǰ��λ��
        double position = 0;
        // ����ģʽ�趨ֵ (0-3: ����+����, 4-7: ˫����)
        int plusModel = 0;

        // --- ˵�� ---
        // ���������У�δ��ȷ��Ҫ����Ͷ�ȡ�ģ������ڡ�����������ʱʹ�á���ע����
    };


	//�����
	std::unordered_map<QString, Axis_Config> m_Axis;

	//DI��������
	struct DI_Config
	{
		//����
		int card = 0;
		// CAN �ڵ�
		int can = -1;
		//ͨ��
		int channel = 0;
		//״̬
		int state = 0;
	};

	//DI���ܶ�Ӧ�İ忨����
	std::unordered_map<QString, DI_Config> m_DI;

	//DO��������
	struct DO_Config
	{
		//����
		int card = 0;
		// CAN �ڵ�
		int can = -1;
		//ͨ��
		int channel = 0;
        //��Ч��ƽ
        int level = 0;
		//״̬
		int state = 1;
	};

	//DI���ܶ�Ӧ�İ忨����
	std::unordered_map<QString, DO_Config> m_DO;

    struct DeviceMoveit_Config
    {
        //------------------------------------ 2D
        // 2D����ʱ��
        int BuffetTime2D = 500;
        // 2D�ӳٴ���ʱ��
        int DelayTriggerTime2D = 20;
        // 2D��Դ����ʱ��/3D���屣��ʱ��
        int HoldTime2D = 20;
        //------------------------------------ 3D
        // 3D����ʱ��
        int BuffetTime3D = 500;
        // 3D�ӳٴ���ʱ��
        int DelayTriggerTime3D = 20;
        // 3D��Դ����ʱ��/3D���屣��ʱ��
        int HoldTime3D = 20;
        int ScanRow = 2000;
        //------------------------------------ ȡ��
        //������ʱ
        int suctionTime = 500;
        //------------------------------------ ����
        //������ʱ
        int unLoadTime = 500;
        //------------------------------------ ͨѶ
        //mes url
        QString mesUrl = "";
        //ModbusTcp ��ַ
        QString modbusTcpIp = "";
        //mes csv
        QString mesCSV = "";
        //mes ngPicPath
        QString ngPicPath = "";
    };
    //��������
    DeviceMoveit_Config m_Paramer;

    //// ���ܶ�Ӧ�����
    //struct Camera_Name
    //{
    //    std::string pin2D = "";
    //    //�׶����������
    //    std::string ed_Up = "";
    //    //�׶����������
    //    std::string ed_Down = "";
    //};

    //Camera_Name m_cameraName;

    // �䷽��������
    struct ProductParameter
    {
        //����������
        int ProductCount = 0;
        //NG����
        int NGCount = 0;
        //OK����
        int OKCount = 0;
        //ֱͨ��
        double FirstPassRate = 0;
        //�û���
        QString UserAccout = "";
        //����
        QString WorkOrderCode = "";
    };

    ProductParameter m_productPara;

signals:
    void showPin_signal(ResultParamsStruct_Pin result, PointItemConfig config);    //����2Dpin��
    void showPin2Dand3D_signal(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D��3Dpin��
    void showPin3D_signal(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //����3Dpin��
    void updateStepIndex_signal();
    //ˢ����ʾ�������
    void upDataProductPara_signal(Run_Result runResult);
    //�Ӿ����ܵĴ������ʾ��ͼ
    //void showVision_signal(HObject img,bool isNG);
    void showVision_signal(Vision_Message message);
    //���ͼƬչʾ
    void clearWindow_signal();
    //������ʾͼƬ
    void showImage_signal(QImage img);
    //дmodbusֵ regAddr:��ַ�� bitPos:λ��ַ�� type = 0 : λ��= 1 ��ֵ��= 2��������
    void writeModbus_signal(int regAddr, int bitPos, float value, int type);
    //��modbusֵ regAddr:��ַ�� bitPos:λ��ַ��type = 0 : λ��= 1 ��ֵ��= 2��������
    void readModbus_signal(int regAddr, int bitPos, int type);
    // ��ʾԲ����ͼƬ
    void showCircleMeasure_signal(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config,double dis,bool isNG);
    // �����Ӿ�ģ����ʾ
    void showAllVision_signal(std::unordered_map<QString, Vision_Message> message);
    // ����״̬ state 0:������1:���У�2:ֹͣ
    void showWorkState_signal(int state);
    void upDateQgraphicsView_signal();
    //ֱ����ʾ����NG��ͼƬ
    void showNG_signal(HObject photo, PointItemConfig config,bool isNG = false);

public slots:
    void redLightFlashing_slot();
};

