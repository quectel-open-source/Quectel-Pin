#include "LS_Motion.h"
#include <Windows.h> 
#include "LTDMC.h"
#include "qc_log.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QtCore/qcoreapplication.h>

bool LS_Motion::motionInit()
{
    WORD wCardNum;      //���忨��
    WORD arrwCardList[8];   //���忨������
    DWORD arrdwCardTypeList[8];   //�����������
    bool b1 = 0;
    if (dmc_board_init() <= 0)      //���ƿ��ĳ�ʼ������
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("���ƿ������쳣���������ü�Ӳ����"));
        machineLog->write(QString::fromStdString("���ƿ������쳣���������ü�Ӳ����"), Normal);
    else
        b1 = true;
    dmc_get_CardInfList(&wCardNum, arrdwCardTypeList, arrwCardList);    //��ȡ����ʹ�õĿ����б�
    //m_wCard = arrwCardList[0];

    bool b2 = CANInit(0, 1);//��ʼ��CAN-IO
    bool b3 = CANInit(0, 2);

    if (!b2)
    {
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO��չ��1�����쳣���������ü�Ӳ����"));
        machineLog->write(QString::fromStdString("IO��չ��1�����쳣���������ü�Ӳ����"), Normal);
        //EventBus.Default.Log(new LogInfo("IO��չ��1�����쳣��", LogLevel.Error));
    }
    if (!b3)
    {
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO��չ��2�����쳣���������ü�Ӳ����"));
        machineLog->write(QString::fromStdString("IO��չ��2�����쳣���������ü�Ӳ����"), Normal);
        //EventBus.Default.Log(new LogInfo("IO��չ��2�����쳣��", LogLevel.Error));
    }

    if (b1) InitAxes();

    return b1 && b2 && b3;
}

bool LS_Motion::CANInit(int CardId, int NodeId)
{
    WORD usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    nmc_get_connect_state(usCardNum, &usCanNum, &usCanStatus);
    if (usCanStatus != 1)
        nmc_set_connect_state(usCardNum, (WORD)NodeId, 1, 0);

    nmc_get_connect_state(usCardNum, &usCanNum, &usCanStatus);

    return (usCanStatus == 1);
}


//��ȡSEVON�ź� ���ŷ�״̬
bool LS_Motion::ReadAxisServeOn(int CardId, int AxisId)
{
    return (dmc_read_sevon_pin((WORD)CardId, (WORD)AxisId) == 0);
}

//���SEVON�ź�
short LS_Motion::WriteAxisServeOn(int CardId, int AxisId, bool isEnable)
{
    return dmc_write_sevon_pin((WORD)CardId, (WORD)AxisId, isEnable ? (WORD)0 : (WORD)1);
}

void LS_Motion::WriteAxis(int CardId, int AxisId,
    double StartVel, double RunVel,
    double AccTime, double DccTime, double StopVel,
    double STime,int homeDir,int stopIO)
{
    if (AxisId == -1) return;
    WORD card = CardId;
    WORD axis = AxisId;
    dmc_set_profile(card, axis, StartVel, RunVel, AccTime, DccTime, StopVel);//�����ٶȲ���
    dmc_set_s_profile(card, axis, 0, STime);                                    //����S���ٶȲ���
    // LTDMC.dmc_set_home_pin_logic(card, axis, 0, 0);//����ԭ��͵�ƽ��Ч

    // ����ԭ����Ч��ƽ
    dmc_set_home_pin_logic(
        (ushort)card,
        (ushort)axis,
        0, // 0�͵�ƽ��Ч 1�ߵ�ƽ��Ч��д��Ϊ�͵�ƽ��Ч��
        0  // ����������д��Ϊ 0��
    );

    // ����������λ��Ч��ƽ
    dmc_set_el_mode(
        (ushort)axis,
        (ushort)axis,
        1, // �Ƿ���������Ӳ��λ��д��Ϊ����
        0, // ����Ӳ��λ��Ч��ƽ��0�͵�ƽ��Ч��д��Ϊ�͵�ƽ��Ч��
        1  // ����Ӳ��λ��Ч��ƽ��1�ߵ�ƽ��Ч��д��Ϊ�ߵ�ƽ��Ч��
    );

    // ���û�ԭģʽ
    dmc_set_homemode(
        (ushort)axis,
        (ushort)axis,
        homeDir == 1 ? (ushort)1 : (ushort)0,  // ���㷽��0���� 1�����滻Ϊ�����е� homeDir��
        0,                      // �����ٶ�ģʽ��0���� 1���٣�д��Ϊ���٣�
        1,                      // ����ģʽ��д��Ϊģʽ 1��
        2                       // ����������д��Ϊ 2��
    );

    // ���û��㵽��λ�Ƿ���
    dmc_set_home_el_return(
        (ushort)card,
        (ushort)axis,
        1 // 0������ 1���ң�д��Ϊ���÷��ң�
    );

    // ӳ����ļ�ͣ�ź�
    dmc_set_axis_io_map(
        (ushort)card,
        (ushort)axis,
        3,                // �ź����ͣ��ἱͣ�źţ�д��Ϊ 3��
        6,                // �� IO ӳ�����ͣ�ͨ������˿ڣ�д��Ϊ 6��
        (ushort)stopIO,  // ��ͣ�źŶ�Ӧ�� IO ����
        0.01             // �˲�ʱ�䣨д��Ϊ 0.01��
    );

    // ���ü�ͣģʽ
    dmc_set_emg_mode(
        (ushort)card,
        (ushort)axis,
        1, // �Ƿ�����ʹ�ü�ͣ�źţ�д��Ϊ���ã�
        RS2::IOStateEnum::High  // ��Ч��ƽ��0�͵�ƽ��Ч 1�ߵ�ƽ��Ч��д��Ϊ�ߵ�ƽ��Ч��
    );
}

//��ͣIO����
void LS_Motion::MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel)
{
    //ӳ����ļ�ͣ�ź�
    dmc_set_axis_io_map((ushort)CardId, (ushort)AxisId,
        3,//�ź����ͣ��ἱͣ�ź�
        6,//��IOӳ������:ͨ������˿�
        (ushort)IOindex,//IO����
        0.01);//�˲�ʱ��
    dmc_set_emg_mode((ushort)CardId, (ushort)AxisId,
        1,//����ʹ�ü�ͣ�ź�
        validlevel);//��Ч��ƽ
}


//�رհ忨
void LS_Motion::motionClose()
{
    ////m_isLoop = false;
    ////m_isPause = false;
    ////m_isEmergency = false;
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis); //ֹͣ������
        //WriteAxisServeOn(axis, false);   //��ʹ��
    }

    //������
    CANClose(1);
    CANClose(2);
    dmc_board_close();
}


//��ֹͣ
void LS_Motion::AxisStop(int CardId, int AxisId)
{
    dmc_stop((ushort)CardId, (ushort)AxisId, 0);// 0:����ֹͣ��1������ֹͣ
}

//�����ἱͣ
void LS_Motion::AxisEmgStop(int CardId)
{
    dmc_emg_stop((ushort)CardId);
}

//�ر�CAN��
void LS_Motion::CANClose(int CardId)
{
    ushort usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    dmc_get_can_state(usCardNum, &usCanNum, &usCanStatus);

    //usCanNum ��Χ 1-8
    for (ushort i = usCanNum; i < usCanNum + 1; i++)
    {
        nmc_set_connect_state(usCardNum, i, 0, 0);//����CANͨѶ״̬���Ͽ� 
    }
}


//��������
void LS_Motion::GoHome(int CardId, int AxisId)
{
    dmc_home_move((ushort)CardId, (ushort)AxisId);
}

//����λ�� �ͱ�����
void LS_Motion::AxisClearPosition(int CardId, int AxisId)
{
    dmc_set_position((ushort)CardId, (ushort)AxisId, 0);
    dmc_set_encoder((ushort)CardId, (ushort)AxisId, 0);
}

// ���ָ������˶�״̬������ true ��ʾ�������У�false ��ʾ��ֹͣ
bool LS_Motion::IsRuning(int CardId, int AxisId)
{
    return (dmc_check_done((ushort)CardId, (ushort)AxisId) == 0);
}

//ֹͣ������
void LS_Motion::AxisStopAll()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis);
    }
}

//�����˶� 0:������1��������
void LS_Motion::VMove(int CardId, int AxisId, bool pdir)
{
    ushort dir = 0;
    if (pdir)
        dir = 1;
    auto rtn = dmc_vmove((ushort)CardId, (ushort)AxisId, dir);
    //machineLog->write(QString::number(AxisId) + " VMove = " + QString::number(rtn), Normal);
}


// ʵʱ�������ٶ�(���߱���)
void LS_Motion::WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed)
{
    short rtn = dmc_change_speed((ushort)CardId, (ushort)AxisId, RealSpeed, 0);
}

//������λ��
double LS_Motion::ReadPosition(int CardId, int AxisId)
{
    return (dmc_get_position((ushort)CardId, (ushort)AxisId));
}


// ��ȡ�ᱨ���˿ڵ�ƽ
bool LS_Motion::isAxisErc(int CardId, int AxisId)
{
    bool res = (dmc_read_erc_pin((ushort)CardId, (ushort)AxisId) == 0);
    return res;
}


// ����ᱨ��״̬
void LS_Motion::AxisClear(int CardId, int AxisId)
{
    dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 0);
}

// ���������˶�
int LS_Motion::PMove_abs(int CardId, int AxisId, int value)
{
    int rtn = dmc_pmove((ushort)CardId, (ushort)AxisId, value, 1); //0���������ģʽ��1����������ģʽ
    return rtn;
}

// ��������˶�
void LS_Motion::PMove_rel(int CardId, int AxisId, int value)
{
    dmc_pmove((ushort)CardId, (ushort)AxisId, value, 0); //0���������ģʽ��1����������ģʽ
}
