#include "Thread.h"
#include "rs_motion.h"
#include <Windows.h>
#include <LTDMC.h>
#include "lvm_sdk.h"
#include "qg_2dCameraSetting.h"
#include "ModbusManager.h"

#define BUTTON_TIME 3000


Thread::Thread(QObject* parent)
    : QObject(parent)
{

}

//�˿��߳�
void Thread::triggerMotionTpyes()
{
    int sRtn = 0;
    while (!bExit)
    {

        if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::InitMotion)
        {
            machineLog->write(QString::fromStdString("��ʼ��.."), Normal);
            //�忨��IO��ʼ��
            if (LSM->motionInit())
            {
                LSM->m_cardInitStatus = true;
                machineLog->write(QString::fromStdString("�忨��ʼ�����"), Normal);
            }
            else
            {
                LSM->m_cardInitStatus = false;
                machineLog->write(QString::fromStdString("�忨��ʼ�����󣡣�"), Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::X_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("X ����.."), Normal);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_X);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::X);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("X ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("X ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Y);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Y ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y1 ����.."), Normal);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Y1);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Y1);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y1 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Y1 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y2 ����.."), Normal);
            //sRtn = LSM->AxisHomeAsync(LS_AxisName::Y2);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Y2);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Y2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y2 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Y2 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z ����.."), Normal);
            //sRtn = LSM->AxisHomeAsync(LS_AxisName::Z);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Z);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Z);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Z ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z1 ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Z1);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z1 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Z1 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z2 ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Z2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z2 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("Z2 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::RZ_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("RZ ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::RZ);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("RZ ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("RZ ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::U);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("U ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("U ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            LSM->m_axisHomeState.U = true;
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U1 ����.."), Normal);
            //sRtn = LSM->AxisHomeAsync(LS_AxisName::U1);

            if (LSM->m_version == LsPin)
            {
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_U);
            }
            else
            {
                sRtn = LSM->AxisHomeAsync(LS_AxisName::U1);
            }
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("U1 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("U1 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            LSM->m_axisHomeState.U1 = true;
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U2 ����.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::U2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("U2 ����ɹ�"), Normal);
            else
                machineLog->write(QString::fromStdString("U2 ����ʧ�ܣ���"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::ResetAll)
        {
            //�Ѿ��ڹ����Ļ������ܽ���
            if (LSM->m_isStart.load())
                continue;
            emit setControlEnable_signal(false);
            //��ɫ�ƻƵ���
            LSM->setThreeColorLight(1);
            LSM->setButtonLight(1, 1);
            //��ʼ��3D���
            LSM->init3dCamera();
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("��λ��.."), Normal);

            switch (LSM->m_version) {
            case TwoPin:
                sRtn = LSM->TaskReset();
                break;
            case LsPin:
                sRtn = LSM->TaskReset_LS();
                break;
            case EdPin:
                sRtn = LSM->TaskReset_ED();
                break;
            case BtPin:
                sRtn = LSM->TaskReset_BT();
                break;
            case JmPin:
                sRtn = LSM->TaskReset_JMPin();
                break;
            case JmHan:
                sRtn = LSM->TaskReset_JMHan();
                break;
            default:
                break;
            }
            if (sRtn == 0)
            {
                LSM->m_isHomed.store(true);
                machineLog->write(QString::fromStdString("��λ�ɹ�"), Normal);
            }
            else
                machineLog->write(QString::fromStdString("��λʧ�� ����"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
            LSM->setButtonLight(1, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Left_Run)
        {
            //�Ѿ��ڹ����Ļ������ܽ���
            if (LSM->m_isStart.load())
                continue;
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //��λ����
             LSM->m_isStart.store(true);

             // ��λ��������
            do {
                //���ͼƬ
                //emit clearWindow_signal();
                // ������λ
                machineLog->write("��λ������..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��
            if (sRtn == 0)
                machineLog->write("��λ�������", Normal);
            else
                machineLog->write("��λ�����ж�", Normal);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Right_Run)
        {
            //�Ѿ��ڹ����Ļ������ܽ���
            if (LSM->m_isStart.load())
                continue;
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //�ҹ�λ����
             LSM->m_isStart.store(true);

            // �ҹ�λ��������
            do {
                //���ͼƬ
                //emit clearWindow_signal();
                // �����ҹ�λ
                machineLog->write("�ҹ�λ������..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_R);
                if (sRtn != 0) 
                    break;

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��
            if (sRtn == 0)
                machineLog->write("�ҹ�λ�������", Normal);
            else
                machineLog->write("�ҹ�λ�����ж�", Normal);
            LSM->m_triggerMotionTpyes = 0;
             LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Both_Run)
        {
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //����״̬
            LSM->emit showWorkState_signal(1);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //˫��λ����
            //����λ
            LSM->m_isStart.store(true);

            // ˫��λ��������
            do {
                //���ͼƬ
                //emit clearWindow_signal();
                // ������λ
                machineLog->write("��λ������..", Normal);
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                if (sRtn != 0)
                    break;

                // �л����ҹ�λ
                emit changeStationUI_signal();

                // �����ҹ�λ
                machineLog->write("�ҹ�λ������..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_R);
                // ��ȡִ��ʱ��
                qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��

            // ��β����
            LSM->m_isStart.store(false);
            LSM->m_triggerMotionTpyes = 0;
            emit setControlEnable_signal(true);
            //����״̬
            LSM->emit showWorkState_signal(0);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
            // ��¼����״̬
            machineLog->write(sRtn == 0 ? "˫��λ�������" : "�����ж�", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::LS_Run)
        {
            //�Ѿ��ڹ����Ļ������ܽ���
            if (LSM->m_isStart.load())
            {
                machineLog->write("�Ѿ�������", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //����״̬
            LSM->emit showWorkState_signal(1);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //��λ����
            LSM->m_isStart.store(true);

            // ��λ��������
            do {
                ////0611����
#if LOCAL_TEST == 0
                if (!LSM->m_isOneStep.load())
                {
                    //�ȱ���������,�������߱�����
                    sRtn = LSM->feedingAction();
                    if (sRtn != 0 && sRtn != 10086)
                    {
                        machineLog->write("���������Ͽ����ж�", Normal);
                        break;
                    }
                    if (sRtn == 10086)
                    {
                        //���ϻ�û���룬ֱ��������һվ
                        machineLog->write("���ϻ�û���룬ֱ��������һվ", Normal);
                        sRtn = LSM->unloadingAction();
                        if (sRtn != 0)
                        {
                            machineLog->write("���������Ͽ����ж�", Normal);
                            break;
                        }
                        continue;
                    }
                }
#endif
                //���ͼƬ
                //emit clearWindow_signal();
                machineLog->write("��λ������..", Normal);
                //��¼����ʱ��
                QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
                LSM->m_snResult.start_time = timestamp;
                //����
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // ��ȡִ��ʱ��
                qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
                emit showCTtime_signal(QString::number(elapsed));
                timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
                LSM->m_snResult.stop_time = timestamp;
                if (sRtn != 0)
                    break;
                if (!LSM->notMes)
                {
                    //MES ��վ��Ϣ����
                    LSM->m_snResult.sn = LSM->m_snReq.sn;
                    LSM->m_snResult.station_name = LSM->m_snReq.station_name;
                    LSM->m_snResult.op = LSM->m_snReq.op;

                    //ģ��
                    //m_snResult.op = "M001";
                    ////�ع�
                    //LSM->m_snResult.result = "PASS";
                    // ���ü��ӿ�
                    SnCheckResponse res = LSM->post_result_process(
                        "http://10.23.164.56:8001/MainWebForm.aspx",
                        LSM->m_snResult
                    );
                }
                ////0611����
#if LOCAL_TEST == 0
                if (!LSM->m_isOneStep.load())
                {
                    //�ȱ���������,�������߱�����
                    machineLog->write("������ɣ�����������", Normal);
                    sRtn = LSM->unloadingAction();
                    if (sRtn != 0)
                    {
                        machineLog->write("���������Ͽ����ж�", Normal);
                        break;
                    }
                }
#endif

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��
            if (sRtn == 0)
            {
                machineLog->write("��λ�������", Normal);
                //��ɫ�ƻƵ���
                LSM->setThreeColorLight(1);
                //����״̬
                LSM->emit showWorkState_signal(0);
            }
            else
            {
                //����״̬
                LSM->emit showWorkState_signal(2);
                machineLog->write("��λ�����ж�", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
            //�������ռ��
            for (auto& camera : LSM->m_cameraMap)
            {
                if (camera.second)
                {
                    camera.second->StopGrabbing_Action();
                    camera.second->CloseDevice_Action();
                }
            }
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::ED_Run)
        {
            //�Ѿ��ڹ����Ļ������ܽ���
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("�Ѿ�������", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("��λ������..", Normal);
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //����״̬
            LSM->emit showWorkState_signal(1);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //��λ����
            LSM->m_isStart.store(true);
            if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
            {
                //�ǵ��� ��һ�������������Ҫ���� �� !LSM->m_notRobot��ʹ�û����ˣ�
                bool haveProduct = false;
                MODBUS_TCP.readIntCoil(34, 6, haveProduct);
                if (haveProduct)
                {
                    machineLog->write("������A��ִ������", Normal);
                    //NGƤ������
                    if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        //����������
                        LSM->setButtonLight(3, 1);
                        LSM->m_redLightFlashing.start(1000);
                    }

                    while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        if (LSM->m_isEmergency)
                            break;
                        emit confirmationRequested("NGƤ�����ϣ���ȡ�ϣ���");
                        QThread::msleep(200);
                    }
                    //�رշ�����
                    LSM->setButtonLight(3, 0);
                    //�رպ����˸
                    LSM->m_redLightFlashing.stop();
                    //���̵�
                    LSM->setThreeColorLight(0);
                    sRtn = LSM->runRobotClearHand();
                    if (sRtn != 0)
                    {
                        machineLog->write("������A�����ϴ��󣡣�", Normal);
                        machineLog->write("������A�����ϴ��󣡣�", Err);
                    }
                }
                //���Bצ
                bool haveProductB = false;
                MODBUS_TCP.readIntCoil(34, 7, haveProductB);
                if (haveProductB)
                {
                    machineLog->write("������B��ִ������", Normal);
                    //NGƤ������
                    if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        //����������
                        LSM->setButtonLight(3, 1);
                        LSM->m_redLightFlashing.start(1000);
                    }

                    while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        if (LSM->m_isEmergency)
                            break;

                        emit confirmationRequested("NGƤ�����ϣ���ȡ�ϣ���");
                        QThread::msleep(200);
                    }
                    //�رշ�����
                    LSM->setButtonLight(3, 0);
                    //�رպ����˸
                    LSM->m_redLightFlashing.stop();
                    //���̵�
                    LSM->setThreeColorLight(0);
                    sRtn = LSM->runRobotClearHand();
                    if (sRtn != 0)
                    {
                        machineLog->write("������B�����ϴ��󣡣�", Normal);
                        machineLog->write("������B�����ϴ��󣡣�", Err);
                    }
                }
               
            }

            if (sRtn == 0)
            {
                // ��λ��������
                do {
                    
                    LSM->m_mesResponse_ED.state = true;
                    if (LSM->getDI(LS_DI::ED_DI_ProdDetect) == 0)
                    {
                        //�ξ������ϲ����У�û������ȡ��
                        if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
                        {
                            //�ǵ��� �ȵ�һ��Ƥ������ �� !LSM->m_notRobot��ʹ�û����ˣ�
                            sRtn = LSM->runRobotCheck();
                            if (sRtn != 0)
                            {
                                machineLog->write("�ξߵ��Ͽ����ж�", Normal);
                                break;
                            }
                        }
                        //continue;
                    }

                    if (LSM->m_mesResponse_ED.state)
                    {
                        //����
                        if (LSM->getDI(LS_DI::ED_DI_ProdDetect) == 1
                            && (LSM->getDI(LS_DI::ED_DI_ClampPosCyl1_Home) == 1 || LSM->getDI(LS_DI::ED_DI_ClampCyl1_Home) == 1 || LSM->getDI(LS_DI::ED_DI_ClampCyl2_Home) == 1))
                        {
                            LSM->setDO(LS_DO::ED_DO_PositionCyl1, 1);
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                            LSM->setDO(LS_DO::ED_DO_ClampCyl1_Valve1, 1);
                            LSM->setDO(LS_DO::ED_DO_ClampCyl1_Valve2, 1);
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        QElapsedTimer timer;
                        timer.start();
                        //��ʼ�����䷽
                        sRtn = LSM->runAction(LSM->m_runListProcess_L);
                        // ��ȡִ��ʱ��
                        qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
                        emit showCTtime_signal(QString::number(elapsed));
                        if (sRtn != 0)
                            break;

                        if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
                        {
                            //�ǵ��� ���϶��� �� !LSM->m_notRobot��ʹ�û����ˣ�
                            machineLog->write("������ɣ��ξ�ȡ��", Normal);
                            sRtn = LSM->runRobotUnLoading();
                            if (sRtn != 0)
                            {
                                machineLog->write("�ξ�ȡ�Ͽ����ж�", Normal);
                                break;
                            }

                            machineLog->write("�ξ�ȡ����ɣ��ȴ��ξ߷���", Normal);
                            sRtn = LSM->runRobotLoad_Tool();
                            //sRtn = LSM->runRobotCheck();
                            if (sRtn != 0)
                            {
                                machineLog->write("�ξ߷��Ͽ����ж�", Normal);
                                break;
                            }
                            //LSM->m_runResult.isNG = false;
                            if (!LSM->m_runResult.isNG)
                            {
                                //����Ƥ������
                                if (LSM->getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1 && LSM->getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
                                {
                                    //����������
                                    LSM->setButtonLight(3, 1);
                                    LSM->m_redLightFlashing.start(1000);
                                }
                                while (LSM->getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1 && LSM->getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
                                {
                                    if (LSM->m_isEmergency)
                                        break;
                                    emit confirmationRequested("����Ƥ�����ϣ���ȡ�ϣ���");
                                    QThread::msleep(200);
                                }
                                //�رշ�����
                                LSM->setButtonLight(3, 0);
                                //�رպ����˸
                                LSM->m_redLightFlashing.stop();
                                //���̵�
                                LSM->setThreeColorLight(0);
                                //����Ƥ������
                                emit robotUnload_signal(false);
                            }
                            else
                            {
                                //NGƤ������
                                if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                                {
                                    //����������
                                    LSM->setButtonLight(3, 1);
                                    LSM->m_redLightFlashing.start(1000);
                                }

                                while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                                {
                                    if (LSM->m_isEmergency)
                                        break;

                                    emit confirmationRequested("NGƤ�����ϣ���ȡ�ϣ���");
                                    QThread::msleep(200);
                                }
                                //�رշ�����
                                LSM->setButtonLight(3, 0);
                                //�رպ����˸
                                LSM->m_redLightFlashing.stop();
                                //���̵�
                                LSM->setThreeColorLight(0);
                                //����Ƥ������
                                emit robotUnload_signal(true);
                            }

                        }
                    }
                    else
                    {
                        machineLog->write("MES���ϼ��У�鲻�ϸ񣬲�������ԣ�����NGƤ��", Normal);
                        //NGƤ������
                        if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                        {
                            //����������
                            LSM->setButtonLight(3, 1);
                            LSM->m_redLightFlashing.start(1000);
                        }

                        while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                        {
                            if (LSM->m_isEmergency)
                                break;
                            emit confirmationRequested("NGƤ�����ϣ���ȡ�ϣ���");
                            QThread::msleep(200);
                        }
                        //�رշ�����
                        LSM->setButtonLight(3, 0);
                        //�رպ����˸
                        LSM->m_redLightFlashing.stop();
                        //���̵�
                        LSM->setThreeColorLight(0);
                        //����Ƥ������
                        emit robotUnload_signal(true);
                    }
                  
                    if (LSM->m_isEmergency)
                        break;
                    if (LSM->m_isOneStep.load())
                    {
                        //��������
                        break;
                    }
                    if (LSM->m_isLoadErr.load())
                    {
                        //Ƥ�����ϱ���
                        sRtn = -1;
                        break;
                    }
                    Sleep(10);
                } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��

                //�������ռ��
                for (auto& camera : LSM->m_cameraMap)
                {
                    if (camera.second)
                    {
                        camera.second->StopGrabbing_Action();
                        camera.second->CloseDevice_Action();
                    }
                }
            }
            if (sRtn == 0)
            {
                //��ɫ�ƻƵ���
                LSM->setThreeColorLight(1);
                //����״̬
                LSM->emit showWorkState_signal(0);
                machineLog->write("��λ�������", Normal);
            }
            else
            {   
                //����������
                for (int i = 0; i < 2; i++)
                {
                    LSM->setButtonLight(3, 1);
                    Sleep(1000);
                    LSM->setButtonLight(3, 0);
                    Sleep(500);
                }
                //��ɫ�ƺ����
                LSM->setThreeColorLight(2);
                //����״̬
                LSM->emit showWorkState_signal(2);
                machineLog->write("��λ�����ж�", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
            //����ź�
            MODBUS_TCP.writeIntCoil(134, 0, 0);
            MODBUS_TCP.writeIntCoil(134, 1, 0);
            MODBUS_TCP.writeIntCoil(134, 2, 0);
            MODBUS_TCP.writeIntCoil(134, 3, 0);
            MODBUS_TCP.writeIntCoil(134, 4, 0);
            MODBUS_TCP.writeIntCoil(134, 5, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::BT_Run)
        {
            //��̩ ����
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("�Ѿ�������", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("��λ������..", Normal);
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //����״̬
            LSM->emit showWorkState_signal(1);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            LSM->m_isStart.store(true);

            // ��������
            do {
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // ��ȡִ��ʱ��
                qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��

            if (sRtn == 0)
            {
                //��ɫ�ƻƵ���
                LSM->setThreeColorLight(1);
                //����״̬
                LSM->emit showWorkState_signal(0);
                machineLog->write("��λ�������", Normal);
            }
            else
            {
                //��ɫ�ƺ����
                LSM->setThreeColorLight(2);
                //����״̬
                LSM->emit showWorkState_signal(2);
                machineLog->write("��λ�����ж�", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
            // ��¼����״̬
            machineLog->write(sRtn == 0 ? "��λ�������" : "�����ж�", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::JmPin_Run)
        {
		    //����pin ����
		    sRtn = 0;
		    if (LSM->m_isStart.load())
		    {
		    	machineLog->write("�Ѿ�������", Normal);
		    	LSM->m_triggerMotionTpyes = 0;
		    	continue;
		    }
		    machineLog->write("��λ������..", Normal);
		    //��ɫ���̵���
		    LSM->setThreeColorLight(0);
		    //����״̬
		    LSM->emit showWorkState_signal(1);
		    //��ťָʾ�ƿ�
		    LSM->setButtonLight(0, 1);
		    emit setControlEnable_signal(false);
		    LSM->m_isStart.store(true);

		    // ��������
		    do {
		    	QElapsedTimer timer;
		    	timer.start();
		    	sRtn = LSM->runAction(LSM->m_runListProcess_L);
		    	// ��ȡִ��ʱ��
		    	qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
		    	emit showCTtime_signal(QString::number(elapsed));
		    	if (sRtn != 0)
		    		break;

		    } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��

		    if (sRtn == 0)
		    {
		    	//��ɫ�ƻƵ���
		    	LSM->setThreeColorLight(1);
		    	//����״̬
		    	LSM->emit showWorkState_signal(0);
		    	machineLog->write("��λ�������", Normal);
		    }
		    else
		    {
		    	//��ɫ�ƺ����
		    	LSM->setThreeColorLight(2);
		    	//����״̬
		    	LSM->emit showWorkState_signal(2);
		    	machineLog->write("��λ�����ж�", Normal);
		    }
		    LSM->m_triggerMotionTpyes = 0;
		    LSM->m_isStart.store(false);
		    emit setControlEnable_signal(true);
		    //��ťָʾ�ƹ�
		    LSM->setButtonLight(0, 0);
		    // ��¼����״̬
		    machineLog->write(sRtn == 0 ? "��λ�������" : "�����ж�", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::JmHan_Run)
        {
            //�������� ����
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("�Ѿ�������", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("��λ������..", Normal);
            //��ɫ���̵���
            LSM->setThreeColorLight(0);
            //����״̬
            LSM->emit showWorkState_signal(1);
            //��ťָʾ�ƿ�
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            LSM->m_isStart.store(true);

            // ��������
            do {
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // ��ȡִ��ʱ��
                qint64 elapsed = timer.elapsed();  // ����Ϊ��λ
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // ѭ������ʱ����ִ��

            if (sRtn == 0)
            {
                //��ɫ�ƻƵ���
                LSM->setThreeColorLight(1);
                //����״̬
                LSM->emit showWorkState_signal(0);
                machineLog->write("��λ�������", Normal);
            }
            else
            {
                //��ɫ�ƺ����
                LSM->setThreeColorLight(2);
                //����״̬
                LSM->emit showWorkState_signal(2);
                machineLog->write("��λ�����ж�", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //��ťָʾ�ƹ�
            LSM->setButtonLight(0, 0);
            // ��¼����״̬
            machineLog->write(sRtn == 0 ? "��λ�������" : "�����ж�", Normal);
        }

        if (LSM->m_isEmergency && LSM->m_cardInitStatus)
        {
            //machineLog->write("����ֹͣ", Normal);
            LSM->AxisStopAll();
            LSM->closeDO();
            //��ɫ�ƺ����
            LSM->setThreeColorLight(2);
            //����״̬
            LSM->emit showWorkState_signal(2);
            LSM->m_isEmergency.store(false);
            LSM->m_isStart.store(false);
            LSM->m_isRobotLoading.store(false);
            emit setControlEnable_signal(true);
        }
        Sleep(10);
    }
}

void Thread::globalTpyes()
{
    QTime di_start = QTime::currentTime();
    QTime di_stop = QTime::currentTime();
    QTime di_reset = QTime::currentTime();
    QTime di_light = QTime::currentTime();
    int lastEmg = 0;
    // ����DI�뱨����Ϣ����б�
    QList<std::pair<QString, QString>> diCheckList;
    // ��ȫ״̬��־λ
    bool safetyTriggered = false;
    // ����Ӳ���汾����Ž���
    if (LSM->m_version == TwoPin)
    {
        diCheckList.push_back({ LS_DI::IN_BtnPause,  "��ͣ��ť��������" });
        diCheckList.push_back({ LS_DI::IN_SafGrate1, "��ȫ��դ1��������" });
        diCheckList.push_back({ LS_DI::IN_SafLock1,  "��ȫ��1��������" });
        diCheckList.push_back({ LS_DI::IN_SafLock2,  "��ȫ��2��������" });
        diCheckList.push_back({ LS_DI::IN_SafLock3,  "��ȫ��3��������" });
        diCheckList.push_back({ LS_DI::IN_Door1, "�Ž�1��������" });
        diCheckList.push_back({ LS_DI::IN_Door2, "�Ž�2��������" });
        diCheckList.push_back({ LS_DI::IN_Door3, "�Ž�3��������" });
        diCheckList.push_back({ LS_DI::IN_Door4, "�Ž�4��������" });
        diCheckList.push_back({ LS_DI::IN_Door5, "�Ž�5��������" });
        diCheckList.push_back({ LS_DI::IN_Door6, "�Ž�6��������" });
    }
    else if (LSM->m_version == LsPin)
    {
        diCheckList.push_back({ LS_DI::LS_IN_BtnStop, "ֹͣ��ť��������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront1, "ǰ1�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront2, "ǰ2�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront3, "ǰ3�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront4, "ǰ4�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear1,  "��1�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear2,  "��2�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear3,  "��3�Ž���������" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear4,  "��4�Ž���������" });
    }
    else if (LSM->m_version == EdPin)
    {
        diCheckList.push_back({ LS_DI::ED_DI_BtnStop, "ֹͣ��ť��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed1, LS_DI::ED_DI_SafDoor_Feed1 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed2, LS_DI::ED_DI_SafDoor_Feed2 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed3, LS_DI::ED_DI_SafDoor_Feed3 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed4, LS_DI::ED_DI_SafDoor_Feed4 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge1, LS_DI::ED_DI_SafDoor_Discharge1 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge2, LS_DI::ED_DI_SafDoor_Discharge2 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge3, LS_DI::ED_DI_SafDoor_Discharge3 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge4, LS_DI::ED_DI_SafDoor_Discharge4 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_NG1, LS_DI::ED_DI_SafDoor_NG1 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_NG2, LS_DI::ED_DI_SafDoor_NG2 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Rear1, LS_DI::ED_DI_SafDoor_Rear1 + "��������" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Rear2, LS_DI::ED_DI_SafDoor_Rear2 + "��������" });
    }

    //ȫ���źż��
    while (!bExit)
    {

        if (LSM->m_cardInitStatus)
        {
            //���DI�ź�
            LSM->getAllDI();

            if (LSM->m_version == TwoPin)
            {
                if (LSM->m_DI[LS_DI::IN_BtnStop].state != 1 && lastEmg == 0)
                {
                    //������ͣ
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::X].card);
                    machineLog->write(QString::fromStdString("������ͣ����"), Global);
                    LSM->m_isEmergency.store(true);
                    //����������
                    LSM->setButtonLight(3, 1);

                }
                else if (LSM->m_DI[LS_DI::IN_BtnStop].state == 0 && lastEmg == 1)
                {
                    //֮ǰ�������⿪��ͣ
                    lastEmg = 0;
                    //�رշ�����
                    LSM->setButtonLight(3, 0);
                }
                //��λ
                if (LSM->m_DI[LS_DI::IN_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) < BUTTON_TIME)
                        continue;
                    di_reset = QTime::currentTime();
                    //ʵ�尴ť��λ
                    emit di_resetAction_signal();
                }
                //����
                if (LSM->m_DI[LS_DI::IN_BtnSetUpL].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) < BUTTON_TIME)
                        continue;
                    di_start = QTime::currentTime();
                    //ʵ�尴ť����
                    emit di_runAction_signal();
                }
                //ֹͣ
                if (LSM->m_DI[LS_DI::IN_BtnPause].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) < BUTTON_TIME)
                        continue;
                    di_stop = QTime::currentTime();
                    //ʵ�尴ťֹͣ
                    emit di_stopAction_signal();
                }

                //bool safetyTriggered = false;
                //for (const auto& item : diCheckList) {
                //    if (LSM->m_DI[item.first].state != 0) {
                //        machineLog->write(item.second, Normal);  
                //        safetyTriggered = true;
                //    }
                //}

                //if (safetyTriggered && !LSM->m_isEmergency.load()) {
                //    // ʱ������飨1���ڲ��ظ�������
                //    if (fabs(QTime::currentTime().msecsTo(di_stop)) >= 1000) {
                //        di_stop = QTime::currentTime();
                //        emit di_stopAction_signal();
                //    }
                //}
            }
            else if (LSM->m_version == LsPin)
            {
                //���N
                if (LSM->m_DI[LS_DI::LS_IN_BtnEmergency].state != 1  && lastEmg == 0)
                {
                    //������ͣ
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::LS_X].card);
                    machineLog->write(QString::fromStdString("������ͣ����"), Normal);
                    LSM->m_isEmergency.store(true);
                    LSM->m_isRunStop.store(true);
                    lastEmg = 1;
                    //����������
                    LSM->setButtonLight(3, 1);
                }
                else if (LSM->m_DI[LS_DI::LS_IN_BtnEmergency].state == 1 && lastEmg == 1)
                {
                    //֮ǰ�������⿪��ͣ
                    lastEmg = 0;
                    //�رշ�����
                    LSM->setButtonLight(3, 0);
                    machineLog->write(QString::fromStdString("�����ͣ����"), Normal);
                    LSM->m_isEmergency.store(false);
                    LSM->m_isRunStop.store(false);
                }
                //if (LSM->m_DI[LS_DI::LS_IN_BtnSetUp].state == 1)
                //{
                //    //�ָ�����״̬
                //    LSM->m_isRunStop.store(false);
                //    LSM->setDO(LS_DO::LS_OUT_StartBtnLED, 1);
                //    if (LSM->m_DO[LS_DO::LS_OUT_YellowLight].state == 0)
                //    {
                //        //��ɫ�ƻƵ���
                //        LSM->setDO(LS_DO::LS_OUT_RedLight, 0);
                //        LSM->setDO(LS_DO::LS_OUT_YellowLight, 1);
                //        LSM->setDO(LS_DO::LS_OUT_GreenLight, 0);
                //    }
                //}
                //����λ
                if (LSM->m_isStart.load())
                {
                    for (const auto& axis : LSM->m_Axis)
                    {
                        if (axis.second.position > axis.second.maxTravel)
                        {
                            //������ͣ
                            LSM->AxisEmgStop(axis.second.card);
                            machineLog->write(axis.first + "��������λ", Normal);
                            machineLog->write("������ͣ����", Global);
                            LSM->m_isEmergency.store(true);
                            LSM->m_isRunStop.store(true);
                        }
                        if (axis.second.position < axis.second.minTravel)
                        {
                            //������ͣ
                            LSM->AxisEmgStop(axis.second.card);
                            machineLog->write(axis.first + "��������λ", Normal);
                            machineLog->write("������ͣ����", Global);
                            LSM->m_isEmergency.store(true);
                            LSM->m_isRunStop.store(true);
                        }
                    }
                }

                if (LSM->m_DI[LS_DI::LS_IN_BtnSetUp].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) < BUTTON_TIME)
                        continue;
                    machineLog->write("������������", Normal);
                    di_start = QTime::currentTime();
                    //ʵ�尴ť����
                    emit di_runAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnStop].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) < BUTTON_TIME)
                        continue;
                    di_stop = QTime::currentTime();
                    //ʵ�尴ťֹͣ
                    machineLog->write("����ֹͣ����", Normal);
                    emit di_stopAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) < BUTTON_TIME)
                        continue;
                    di_reset = QTime::currentTime();
                    //ʵ�尴ť��λ
                    emit di_resetAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnLight].state != 0)
                {
                    if (fabs(QTime::currentTime().msecsTo(di_light)) < BUTTON_TIME)
                        continue;
                    di_light = QTime::currentTime();
                    //ʵ�尴ť����
                    auto state = LSM->getDO(LS_DO::LS_OUT_Lighting);
                    if (state == 0)
                        LSM->setButtonLight(4, 1);
                    else
                        LSM->setButtonLight(4, 0);
                }
            }
            else if (LSM->m_version == EdPin)
            {
                //�׶���
                if ((LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state != 1 || LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state != 1) && lastEmg == 0)
                {
                    //������ͣ
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::LS_X].card);
                    if (LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state != 1)
                    {
                        machineLog->write(QString::fromStdString("������ͣһ����"), Normal);
                        machineLog->write(QString::fromStdString("������ͣһ����"), Err);
                    }
                    if (LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state != 1)
                    {
                        machineLog->write(QString::fromStdString("������ͣ������"), Normal);
                        machineLog->write(QString::fromStdString("������ͣ������"), Err);
                    }
                    LSM->m_isEmergency.store(true);
                    LSM->m_isRunStop.store(true);
                    lastEmg = 1;
                    //����������
                    LSM->setButtonLight(3, 1);
                }
                else if ((LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state == 1 && LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state == 1) && lastEmg == 1)
                {
                    //֮ǰ�������⿪��ͣ
                    lastEmg = 0;
                    //�رշ�����
                    LSM->setButtonLight(3, 0);
                    machineLog->write(QString::fromStdString("�����ͣ����"), Normal);
                    LSM->m_isEmergency.store(false);
                    LSM->m_isRunStop.store(false);
                }

                if (LSM->m_DI[LS_DI::ED_DI_BtnStart].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) >= BUTTON_TIME)
                    {
                        di_start = QTime::currentTime();
                        //ʵ�尴ť����
                        emit di_runAction_signal();
                    }
                }
                if (LSM->m_DI[LS_DI::ED_DI_BtnStop].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) >= BUTTON_TIME)
                    {
                        di_stop = QTime::currentTime();
                        //ʵ�尴ťֹͣ
                        emit di_stopAction_signal();
                    }
                }
                if (LSM->m_DI[LS_DI::ED_DI_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) >= BUTTON_TIME)
                    {
                        di_reset = QTime::currentTime();
                        //ʵ�尴ť��λ
                        emit di_resetAction_signal();
                    }  
                }

                if (!LSM->m_notDoor)
                {
                    bool tempALLSave = true;
                    for (const auto& item : diCheckList) {
                        if (LSM->m_DI[item.first].state != 0) {
                            if(safetyTriggered == false)
                            {
                                //��û�������ȱ���
                                if (LSM->m_errCode.isEmpty() || LSM->m_errCode != item.second)
                                {
                                    machineLog->write(item.second, Normal);
                                    machineLog->write(item.second, Err);
                                }
                                safetyTriggered = true;
                                tempALLSave = false;
                                break;
                            }
                        }
                    }
                    if (tempALLSave)
                        safetyTriggered = false;
                    if (safetyTriggered && !LSM->m_isEmergency.load() && LSM->m_isStart.load()) {
                        // �˶�״̬ʱ����ֹͣ��ʱ������飨1���ڲ��ظ�������
                        if (fabs(QTime::currentTime().msecsTo(di_stop)) >= BUTTON_TIME) {
                            di_stop = QTime::currentTime();
                            emit di_stopAction_signal();
                        }
                    }
                }

            }

            LSM->ReadAllAxisP();
           
        }
        Sleep(50);
    }
}

void Thread::cameraGrabbing()
{
    MV_FRAME_OUT stImageInfo = { 0 };
    MV_DISPLAY_FRAME_INFO stDisplayInfo = { 0 };
    int nRet = MV_OK;
    auto cameraWindow = qg_2dCameraSetting::getCameraWindow();
    if (cameraWindow == NULL)
        return;
    while (!bExit)
    {
        nRet = cameraWindow->m_pcMyCamera->GetImageBuffer(&stImageInfo, 1000);
        if (nRet == MV_OK)
        {
            //���ڱ���ͼƬ
            //EnterCriticalSection(&cameraWindow->m_pcMyCamera->m_hSaveImageMux);
            if (nullptr == cameraWindow->m_pcMyCamera->m_pSaveImageBuf || stImageInfo.stFrameInfo.nFrameLen > cameraWindow->m_pcMyCamera->m_nSaveImageBufSize)
            {
                if (cameraWindow->m_pcMyCamera->m_pSaveImageBuf)
                {
                    free(cameraWindow->m_pcMyCamera->m_pSaveImageBuf);
                    cameraWindow->m_pcMyCamera->m_pSaveImageBuf = nullptr;
                }

                cameraWindow->m_pcMyCamera->m_pSaveImageBuf = (unsigned char*)malloc(sizeof(unsigned char) * stImageInfo.stFrameInfo.nFrameLen);
                if (cameraWindow->m_pcMyCamera->m_pSaveImageBuf == nullptr)
                {
                    //LeaveCriticalSection(&cameraWindow->m_pcMyCamera->m_hSaveImageMux);
                    return;
                }
                cameraWindow->m_pcMyCamera->m_nSaveImageBufSize = stImageInfo.stFrameInfo.nFrameLen;
            }
            memcpy(cameraWindow->m_pcMyCamera->m_pSaveImageBuf, stImageInfo.pBufAddr, stImageInfo.stFrameInfo.nFrameLen);
            memcpy(&cameraWindow->m_pcMyCamera->m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));

            //ˢ�������ͼ
            emit signalFlushed();
            //LeaveCriticalSection(&cameraWindow->m_pcMyCamera->m_hSaveImageMux);

            ////�Զ����ʽ��֧����ʾ
            //if (cameraWindow->m_pcMyCamera->RemoveCustomPixelFormats(stImageInfo.stFrameInfo.enPixelType))
            //{
            //    cameraWindow->m_pcMyCamera->m_pcMyCamera->FreeImageBuffer(&stImageInfo);
            //    continue;
            //}
            //stDisplayInfo.hWnd = m_hwndDisplay;
            //stDisplayInfo.pData = stImageInfo.pBufAddr;
            //stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
            //stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
            //stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
            //stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;
            //m_pcMyCamera->DisplayOneFrame(&stDisplayInfo);

            cameraWindow->m_pcMyCamera->FreeImageBuffer(&stImageInfo);
        }
        //else
        //{
        //    if (MV_TRIGGER_MODE_ON == m_nTriggerMode)
        //    {
        //        Sleep(5);
        //    }
        //}

        Sleep(100);
    }

    return ;
}


void Thread::cameraGrabbing_3D()
{
    int timeout = 600000;

    //QDateTime endTime = QDateTime::currentDateTime();
    //qint64 intervalTimeMS = startTime.msecsTo(endTime);
    //qDebug()<<"��������ڴ�ʱ��:" + QString::number(intervalTimeMS) + "ms";
    auto cameraWindow = qg_2dCameraSetting::getCameraWindow();
    if (cameraWindow == NULL)
        return;
    while (!bExit)
    {
        lvm_point_cloud_t pc;
        //QDateTime startTime = QDateTime::currentDateTime();
        if (cameraWindow->m_pcMyCamera_3D == nullptr || cameraWindow->m_pcMyCamera_3D->dev == nullptr)
        {
            qDebug() << "3D�ɼ�ʧ��";
            return;
        }
        cameraWindow->m_pcMyCamera_3D->StartFlag = true;
        lvm_dev_t* dev = cameraWindow->m_pcMyCamera_3D->dev;
        lvm_depth_map_t* depth_map = (lvm_depth_map_t*)lvm_grab_frame(dev, timeout);
        if (depth_map)
        {
            if (!cameraWindow->m_pcMyCamera_3D->m_isAction)
            {
                 //depth_img = cv::Mat(depth_map->head.height, depth_map->head.width, CV_16UC1, depth_map->data);
                cv::Mat src_gray_f , depth_img, depth_color_img;
                depth_img.convertTo(src_gray_f, CV_32F);
                cv::Mat img = cv::Mat::zeros(depth_img.size(), CV_32FC1);
                cv::normalize(src_gray_f, img, 1.0, 0, cv::NORM_MINMAX);
                cv::Mat result = img * 255;
                result.convertTo(img, CV_8UC1);
                cv::applyColorMap(img, cameraWindow->m_pcMyCamera_3D->showImg, 2);
                emit cameraWindow->m_pcMyCamera_3D->signalFlushed_3D();
                qDebug() << "ˢ��ͼƬ";
            }


            pc.p = (lvm_point_t*)malloc(depth_map->head.width * depth_map->head.height * sizeof(lvm_point_t));
            if (CORRECT == lvm_convert_depth_map_to_pcld(depth_map, &pc, depth_map->head.width * depth_map->head.height))
            {
                pc.head.width = depth_map->head.width;
                pc.head.height = depth_map->head.height;
                //if (CORRECT == lvm_save_pcld(dev, cameraWindow->m_pcMyCamera_3D->m_savePath.toStdString().c_str(), &pc))
                //{
                //    qDebug() << "����ͼ��ɹ�";
                //}
                pcl::PointCloud<pcl::PointXYZ>::Ptr GrabCloud;
                GrabCloud->width = pc.head.width * pc.head.height;
                GrabCloud->height = 1;
                GrabCloud->points.resize(pc.head.width * pc.head.height);
                for (int i = 0; i < GrabCloud->points.size(); i++)
                {
                    GrabCloud->points[i].x = pc.p[i].x;
                    GrabCloud->points[i].y = pc.p[i].y;
                    GrabCloud->points[i].z = pc.p[i].z;
                }
                //��ȡ����
                cameraWindow->m_pcMyCamera_3D->ori_cloud = GrabCloud;
                qDebug() << "GrabCloud size = "<< GrabCloud->size();
                free(pc.p);
                //QDateTime endTime = QDateTime::currentDateTime();
                //qint64 intervalTimeMS = startTime.msecsTo(endTime);
                //qDebug()<<"�����ͼ����ʱ��:" + QString::number(intervalTimeMS) + "ms";
                break;
            }
            else
            {
                qDebug() << "convert to depth map fail!";
                free(pc.p);
                break;
            }
        }
        else
        {
            qDebug() << "grab_frame timeout!";
            break;
        }
    }

    return;
}

//�²㱶��������
void Thread::doubleSpeedDownAction()
{
    LSM->downFeedingAction();
    // �˳��߳��¼�ѭ��
    QThread::currentThread()->quit();
}

//������Ƥ�����ϼ��
void Thread::robotLoadingAction()
{
    LSM->runRobotLoad_Belt();
    // �˳��߳��¼�ѭ��
    QThread::currentThread()->quit();
}

//������Ƥ������
void Thread::robotUnLoadingAction(bool isNG)
{
    machineLog->write("�ξ߷�����ɣ��ȴ�����", Normal);
    auto sRtn = LSM->runRobotUnLoad_Belt(isNG);
    if (sRtn != 0)
    {
        machineLog->write("���Ͽ����ж�", Normal);
    }
    // �˳��߳��¼�ѭ��
    QThread::currentThread()->quit();
}

//����2Dpin��
void Thread::thread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config)
{
    emit thread_showPin_signal(result, config);
}
//2D��3Dpin��
void Thread::thread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)
{
    emit thread_showPin2Dand3D_signal(result2D, config2D, result3D, config3D);
}
//����3Dpin��
void Thread::thread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)
{
    emit thread_showPin3D_signal(result, config);
}
//�Ӿ����ܵĴ������ʾ��ͼ
void Thread::thread_showVision_slot(Vision_Message message)
{
    emit thread_showVision_signal(message);
    message.clear();
}
// ��ʾԲ����ͼƬ
void Thread::thread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    emit thread_showCircleMeasure_signal(cir1, cir2, config, dis, isNG);
}

//ֱ����ʾ����NG��ͼƬ
void Thread::thread_showNG_slot(HObject photo, PointItemConfig config, bool isNG)
{
    emit thread_showNG_signal(photo, config, isNG);
}

//ֱ����ʾͼƬ
void Thread::thread_showImage_slot(QImage img)
{
    emit thread_showImage_signal(img);
}