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

    //���Nר������
    QString LS_X = "X";
    QString LS_Y1 = "Y1";    //�ؾ��ƶ���
    QString LS_Y2 = "Y2";    //3D����ƶ���
    QString LS_U = "U";  //�����ת��
    QString LS_Z = "Z";

    //Ĭ����˫��pin�������
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
    QString LS_DI::IN_BtnSetUpL{ "��������ť" };
    QString LS_DI::IN_BtnSetUpR{ "��������ť" };
    QString LS_DI::IN_BtnInit{ "��ʼ����ť" };
    QString LS_DI::IN_BtnStop{ "��ͣ��ť" };
    QString LS_DI::IN_BtnPause{ "��ͣ��ť" };
    QString LS_DI::IN_BtnReset{ "��λ��ť" };

    // �����ź�
    QString LS_DI::IN_OUT_ClampCylinderL1InitP{ "�ξ�1����1ԭ��" };
    QString LS_DI::IN_OUT_ClampCylinderL1EndP{ "�ξ�1����1����" };
    QString LS_DI::IN_OUT_ClampCylinderL2InitP{ "�ξ�1����2ԭ��" };
    QString LS_DI::IN_OUT_ClampCylinderL2EndP{ "�ξ�1����2����" };
    QString LS_DI::IN_OUT_ClampCylinderR1InitP{ "�ξ�2����1ԭ��" };
    QString LS_DI::IN_OUT_ClampCylinderR1EndP{ "�ξ�2����1����" };
    QString LS_DI::IN_OUT_ClampCylinderR2InitP{ "�ξ�2����2ԭ��" };
    QString LS_DI::IN_OUT_ClampCylinderR2EndP{ "�ξ�2����2����" };

    // ��դ����ȫ�������ź�
    QString LS_DI::IN_SafGrate1{ "��ȫ��դ1" };
    QString LS_DI::IN_SafLock1{ "��ȫ��1" };
    QString LS_DI::IN_SafLock2{ "��ȫ��2" };
    QString LS_DI::IN_SafLock3{ "��ȫ��3" };
    QString LS_DI::IN_Door1{ "�Ž�1" };
    QString LS_DI::IN_Door2{ "�Ž�2" };
    QString LS_DI::IN_Door3{ "�Ž�3" };
    QString LS_DI::IN_Door4{ "�Ž�4" };
    QString LS_DI::IN_Door5{ "�Ž�5" };
    QString LS_DI::IN_Door6{ "�Ž�6" };

    // �����ź�
    QString LS_DI::IN_LPrdOK{ "�ξ�1�����ź�" };
    QString LS_DI::IN_RPrdOK{ "�ξ�2�����ź�" };
    QString LS_DI::IN_NG1{ "NG1" };
    QString LS_DI::IN_NG2{ "NG2" };
    QString LS_DI::IN_NG3{ "NG3" };

    //*************************************** ���N
    // ��ť�ź�
    QString LS_IN_BtnSetUp{ "������ť" };       // DI0.0
    QString LS_IN_BtnStop{ "ֹͣ��ť" };        // DI0.1
    QString LS_IN_BtnReset{ "��λ��ť" };       // DI0.2
    QString LS_IN_BtnEmergency{ "��ͣ��ť" };   // DI0.3 ״̬Ϊ0�Ǵ���
    QString LS_IN_BtnLight{ "������ť" };       // DI0.4
    QString LS_IN_MainAirSupply{ "��̨����Դ" };   // DI0.5

    // �ؾ߼н��ź�
    QString LS_IN_ClampCyl1_Home{ "�ؾ߼н�����1ԭ��" }; // DI1.0
    QString LS_IN_ClampCyl1_Work{ "�ؾ߼н�����1����" }; // DI1.1
    QString LS_IN_ClampCyl2_Home{ "�ؾ߼н�����2ԭ��" }; // DI1.2
    QString LS_IN_ClampCyl2_Work{ "�ؾ߼н�����2����" }; // DI1.3

    // ����ź�
    QString LS_IN_VacuumDetect{ "��צ��ռ��" };   // DI2.0
    QString LS_IN_MaterialDetect{ "�ؾ����ϼ��" }; // DI2.1 ״̬Ϊ0�Ǵ���

    // �Ž��ź�
    QString LS_IN_GateFront1{ "ǰ1�Ž�����" };     // DI3.0
    QString LS_IN_GateFront2{ "ǰ2�Ž�����" };     // DI3.1
    QString LS_IN_GateFront3{ "ǰ3�Ž�����" };     // DI3.2
    QString LS_IN_GateFront4{ "ǰ4�Ž�����" };     // DI3.3
    QString LS_IN_GateRear1{ "��1�Ž�����" };      // DI3.4
    QString LS_IN_GateRear2{ "��2�Ž�����" };      // DI3.5
    QString LS_IN_GateRear3{ "��3�Ž�����" };      // DI3.6
    QString LS_IN_GateRear4{ "��4�Ž�����" };      // DI3.7

    // SMEMA & ���������ź�
    QString LS_IN_SMEMA_UpFront_AllowOut{ "SMEMA �� ǰ���������" }; // I8.0
    QString LS_IN_SMEMA_LowFront_AllowOut{ "SMEMA �� ǰ���������" };// I8.1
    QString LS_IN_SMEMA_UpRear_AllowOut{ "SMEMA �� ����������" }; // I8.2
    QString LS_IN_SMEMA_LowRear_AllowOut{ "SMEMA �� ����������" }; // I8.3
    QString LS_IN_UpperMaterialDetect{ "�ϲ����ϸ�Ӧ" };          // I8.4
    QString LS_IN_UpperOutputDetect{ "�ϲ���ϸ�Ӧ" };            // I8.5
    QString LS_IN_LowerOutputDetect{ "�²���ϸ�Ӧ" };            // I8.6
    QString LS_IN_BlockCyl1_Work{ "�赲����1����" };             // I8.7
    QString LS_IN_BlockCyl1_Home{ "�赲����1ԭ��" };             // I9.0
    QString LS_IN_BlockCyl2_Work{ "�赲����2����" };             // I9.1
    QString LS_IN_BlockCyl2_Home{ "�赲����2ԭ��" };             // I9.2
    QString LS_IN_LiftCyl_Work{ "��������" };                   // I9.3
    QString LS_IN_LiftCyl_Home{ "����ԭ��" };                   // I9.4
    QString LS_IN_LowerMaterialDetect{ "�²����ϸ�Ӧ" };                   // I9.5
    QString LS_IN_UpperCheckProduct{ "�ؾ������ϼ��" };

    //------------------------------------------------ �׶���DI��ʼ��
    // ��ť��
    QString ED_DI_BtnStart{ "������ť" };
    QString ED_DI_BtnStop{ "ֹͣ��ť" };
    QString ED_DI_BtnReset{ "��λ��ť" };
    QString ED_DI_SwitchAutoMode{ "�Զ�ģʽ��ť" };
    QString ED_DI_BtnEStop1{ "��ͣ��ť1" };
    QString ED_DI_BtnEStop2{ "��ͣ��ť2" };
    // ����λ�ü��
    QString ED_DI_ClampPosCyl1_Home{ "�ξ߶�λ����1ԭ��" };
    QString ED_DI_ClampPosCyl1_Work{ "�ξ߶�λ����1����" };
    QString ED_DI_ClampPosCyl2_Home{ "�ξ߶�λ����2ԭ��" };
    QString ED_DI_ClampPosCyl2_Work{ "�ξ߶�λ����2����" };
    QString ED_DI_ClampCyl1_Home{ "�ξ߼н�����1ԭ��" };
    QString ED_DI_ClampCyl1_Work{ "�ξ߼н�����1����" };
    QString ED_DI_ClampCyl2_Home{ "�ξ߼н�����2ԭ��" };
    QString ED_DI_ClampCyl2_Work{ "�ξ߼н�����2����" };
    // ��ȫ�ż��
    QString ED_DI_ProdDetect{ "�ξ߲�Ʒ���޼��" };
    QString ED_DI_SafDoor_Feed1{ "���ϲలȫ��1" };
    QString ED_DI_SafDoor_Feed2{ "���ϲలȫ��2" };
    QString ED_DI_SafDoor_Feed3{ "���ϲలȫ��3" };
    QString ED_DI_SafDoor_Feed4{ "���ϲలȫ��4" };
    QString ED_DI_SafDoor_Discharge1{ "���ϲలȫ��1" };
    QString ED_DI_SafDoor_Discharge2{ "���ϲలȫ��2" };
    QString ED_DI_SafDoor_Discharge3{ "���ϲలȫ��3" };
    QString ED_DI_SafDoor_Discharge4{ "���ϲలȫ��4" };
    QString ED_DI_SafDoor_NG1{ "NGƤ���లȫ��1" };
    QString ED_DI_SafDoor_NG2{ "NGƤ���లȫ��2" };
    QString ED_DI_SafDoor_Rear1{ "���氲ȫ��1" };
    QString ED_DI_SafDoor_Rear2{ "���氲ȫ��2" };
    // �����ź�
    QString ED_DI_AirPressureAlarm{ "CDA����ѹ������" };
    // Ƥ��������ʼ��
    QString ED_DI_FeedBeltMaterialDetect{ "����Ƥ����Ʒ���ϼ��" };
    QString ED_DI_FeedBeltArrivalDetect{ "����Ƥ����Ʒ��λ���" };
    QString ED_DI_DischargeBeltReleaseDetect{ "����Ƥ����Ʒ���ϼ��" };
    QString ED_DI_DischargeBeltArrivalDetect{ "����Ƥ����Ʒ��λ���" };
    QString ED_DI_NGBeltReleaseDetect_Left{ "NGƤ����Ʒ���ϼ��1����" };
    QString ED_DI_NGBeltReleaseDetect_Right{ "NGƤ����Ʒ���ϼ��2���ң�" };
    QString ED_DI_NGBeltFullDetect{ "NGƤ����Ʒ���ϼ��" };

    // ���� DI �źż���
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

    //------------------------------------------------ �ɸ���DO
    QString DO_LightRed{ "��ɫ��-��" };     
    QString DO_LightGreen{ "��ɫ��-��" };   
    QString DO_LightYellow{ "��ɫ��-��" };  
    QString DO_Buzzer{ "������" };          
    QString DO_Cam2DTrigger{ "2D�������" };
    QString DO_Cam3DTrigger{ "3D�������" };
    QString DO_Light2DTrigger{ "2D��Դ����" }; 
    QString DO_StartBtnLED{ "������ťָʾ��" };
    QString DO_StopBtnLED{ "ֹͣ��ťָʾ��" }; 

    //------------------------------------------------ ˫��pin���
    QString DO_ClampCylinderL1{ "�ξ�1����1" };  
    QString DO_ClampCylinderL2{ "�ξ�1����2" };  
    QString DO_ClampCylinderR1{ "�ξ�2����1" };  
    QString DO_ClampCylinderR2{ "�ξ�2����2" };  
    QString DO_Lock1{ "������1" };               
    QString DO_Lock2{ "������2" };               
    QString DO_Lock3{ "������3" };               
    QString DO_LightFlood{ "������" };           
    QString DO_PlusCam3D1{ "3D����-X" };         
    QString DO_PlusCam3D2{ "3D����-Y1" };        
    QString DO_PlusCam3D3{ "3D����-Y2" };        
    QString DO_ReadCodeTrigger1{ "ɨ��ǹ����1" };
    QString DO_ReadCodeTrigger2{ "ɨ��ǹ����2" };
    QString DO_LightOK_L{ "ָʾ��1-��" };        
    QString DO_LightNG_L{ "ָʾ��1-��" };        
    QString DO_LightOK_R{ "ָʾ��2-��" };        
    QString DO_LightNG_R{ "ָʾ��2-��" };        

    //*************************************** ���N
    QString LS_OUT_RedLight{ "��ɫ��-���" };       // DO0.0
    QString LS_OUT_YellowLight{ "��ɫ��-�Ƶ�" };     // DO0.1
    QString LS_OUT_GreenLight{ "��ɫ��-�̵�" };      // DO0.2
    QString LS_OUT_Buzzer{ "������" };             // DO0.3
    QString LS_OUT_StartBtnLED{ "������ťָʾ��" };   // DO0.4
    QString LS_OUT_StopBtnLED{ "ֹͣ��ťָʾ��" };    // DO0.5
    QString LS_OUT_ResetBtnLED{ "��λ��ťָʾ��" };   // DO0.6
    QString LS_OUT_LightBtnLED{ "������ťָʾ��" };   // DO0.7
    QString LS_OUT_ClampSuck{ "��צ����" };       // DO1.0
    QString LS_OUT_ClampBlow{ "��צ����" };       // DO1.1
    QString LS_OUT_Lighting{ "����" };            // DO1.2
    QString LS_OUT_ClampCyl1{ "�ؾ߼н�����1" };  // DO1.3
    QString LS_OUT_ClampCyl2{ "�ؾ߼н�����2" };  // DO1.4
    QString LS_OUT_2DLightTrigger{ "2D��Դ����" }; // DO2.0
    QString LS_OUT_2DCamTrigger{ "2D�������" };   // DO2.1
    QString LS_OUT_3DCamTrigger{ "3D�������" };   // DO2.2
    // SMEMA �����ź�
    QString LS_OUT_SMEMA_UpFront_FeedReady{ "SMEMA �� ǰ�����ϵ�λ�ź�" };    // Q6.0
    QString LS_OUT_SMEMA_LowFront_FeedArrived{ "SMEMA �� ǰ�����ϵ����ź�" }; // Q6.1
    QString LS_OUT_SMEMA_UpRear_FeedReady{ "SMEMA �� ������ϵ�λ�ź�" };     // Q6.2
    QString LS_OUT_SMEMA_LowRear_FeedReady{ "SMEMA �� ������ϵ�λ�ź�" };    // Q6.3
    // ��������ź�
    QString LS_OUT_UpperMotor_Run{ "�ϲ���RUN" };                       // Q6.4
    QString LS_OUT_UpperMotor_Dir{ "�ϲ���Dir" };                       // Q6.5
    QString LS_OUT_LowerMotor_Run{ "�²���RUN" };                       // Q6.6
    QString LS_OUT_LowerMotor_Dir{ "�²���Dir" };                       // Q6.7
    // ���׿����ź�
    QString LS_OUT_BlockCyl_Activate{ "�赲����" };                       // Q7.0
    QString LS_OUT_LiftCyl_Activate{ "��������" };                        // Q7.1

    //------------------------------------------------ �׶���ר��DO
    QString ED_DO_PositionCyl1{ "��λ���׵�ŷ�1" };      // DO1.0
    QString ED_DO_PositionCyl2{ "��λ���׵�ŷ�2" };      // DO1.1
    QString ED_DO_ClampCyl1_Valve1{ "�н�����1��ŷ�1" };  // DO1.2
    QString ED_DO_ClampCyl1_Valve2{ "�н�����1��ŷ�2" };  // DO1.3
    QString ED_DO_CamFrontTrigger{ "�������Ӳ����" };     // DO2.0
    QString ED_DO_CamRearTrigger{ "�������Ӳ����" };      // DO2.1
    QString ED_DO_LightFrontTrigger{ "���������Դ����" }; // DO2.4
    QString ED_DO_LightRearTrigger{ "���������Դ����" };  // DO2.5
    QString ED_DO_LightPositioningTrigger{ "��λ�����Դ����" };
    // �����������ʼ��
    QString ED_DO_FeedBeltStart{ "����Ƥ������" };
    QString ED_DO_DischargeBeltStart{ "����Ƥ������" };
    QString ED_DO_NGBeltStart{ "NGƤ������" };

    // ���� DO �źż���
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
* Camera. �����
*/
namespace LS_CameraName {
    QString LS_CameraName::pin2D{ "Pin��2D���" };
    //�׶��������
    QString LS_CameraName::ed_Up{ "�׶��������" };
    //�׶��������
    QString LS_CameraName::ed_Down{ "�׶��������" };
    //�׶��ᶨλ���
    QString LS_CameraName::ed_Location{ "�׶��ᶨλ���" };
    //�����
    QString LS_CameraName::side_camera{ "�����" };

    QList<QString> LS_CameraName::allCamera{
        pin2D,
        ed_Up,
        ed_Down,
        ed_Location,
        side_camera
    };
};