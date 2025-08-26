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

//运控线程
void Thread::triggerMotionTpyes()
{
    int sRtn = 0;
    while (!bExit)
    {

        if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::InitMotion)
        {
            machineLog->write(QString::fromStdString("初始化.."), Normal);
            //板卡，IO初始化
            if (LSM->motionInit())
            {
                LSM->m_cardInitStatus = true;
                machineLog->write(QString::fromStdString("板卡初始化完成"), Normal);
            }
            else
            {
                LSM->m_cardInitStatus = false;
                machineLog->write(QString::fromStdString("板卡初始化错误！！"), Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::X_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("X 回零.."), Normal);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_X);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::X);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("X 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("X 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Y);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Y 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y1 回零.."), Normal);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Y1);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Y1);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y1 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Y1 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Y2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Y2 回零.."), Normal);
            //sRtn = LSM->AxisHomeAsync(LS_AxisName::Y2);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Y2);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Y2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Y2 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Y2 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z 回零.."), Normal);
            //sRtn = LSM->AxisHomeAsync(LS_AxisName::Z);
            if (LSM->m_version == LsPin)
                sRtn = LSM->AxisHomeAsync(LS_AxisName::LS_Z);
            else
                sRtn = LSM->AxisHomeAsync(LS_AxisName::Z);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Z 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z1 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Z1);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z1 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Z1 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Z2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("Z2 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::Z2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("Z2 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("Z2 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::RZ_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("RZ 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::RZ);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("RZ 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("RZ 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::U);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("U 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("U 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            LSM->m_axisHomeState.U = true;
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U1_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U1 回零.."), Normal);
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
                machineLog->write(QString::fromStdString("U1 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("U1 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            LSM->m_axisHomeState.U1 = true;
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::U2_Homing)
        {
            emit setControlEnable_signal(false);
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("U2 回零.."), Normal);
            sRtn = LSM->AxisHomeAsync(LS_AxisName::U2);
            if (sRtn == 0)
                machineLog->write(QString::fromStdString("U2 回零成功"), Normal);
            else
                machineLog->write(QString::fromStdString("U2 回零失败！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::ResetAll)
        {
            //已经在工作的话，不能进入
            if (LSM->m_isStart.load())
                continue;
            emit setControlEnable_signal(false);
            //三色灯黄灯亮
            LSM->setThreeColorLight(1);
            LSM->setButtonLight(1, 1);
            //初始化3D相机
            LSM->init3dCamera();
            LSM->m_isHomming.store(true);
            machineLog->write(QString::fromStdString("复位中.."), Normal);

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
                machineLog->write(QString::fromStdString("复位成功"), Normal);
            }
            else
                machineLog->write(QString::fromStdString("复位失败 ！！"), Err);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isHomming.store(false);
            emit setControlEnable_signal(true);
            LSM->setButtonLight(1, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Left_Run)
        {
            //已经在工作的话，不能进入
            if (LSM->m_isStart.load())
                continue;
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //左工位运行
             LSM->m_isStart.store(true);

             // 左工位运行流程
            do {
                //清除图片
                //emit clearWindow_signal();
                // 运行左工位
                machineLog->write("左工位运行中..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行
            if (sRtn == 0)
                machineLog->write("左工位运行完成", Normal);
            else
                machineLog->write("左工位运行中断", Normal);
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Right_Run)
        {
            //已经在工作的话，不能进入
            if (LSM->m_isStart.load())
                continue;
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //右工位运行
             LSM->m_isStart.store(true);

            // 右工位运行流程
            do {
                //清除图片
                //emit clearWindow_signal();
                // 运行右工位
                machineLog->write("右工位运行中..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_R);
                if (sRtn != 0) 
                    break;

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行
            if (sRtn == 0)
                machineLog->write("右工位运行完成", Normal);
            else
                machineLog->write("右工位运行中断", Normal);
            LSM->m_triggerMotionTpyes = 0;
             LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::Both_Run)
        {
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //运行状态
            LSM->emit showWorkState_signal(1);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //双工位运行
            //先左工位
            LSM->m_isStart.store(true);

            // 双工位运行流程
            do {
                //清除图片
                //emit clearWindow_signal();
                // 运行左工位
                machineLog->write("左工位运行中..", Normal);
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                if (sRtn != 0)
                    break;

                // 切换到右工位
                emit changeStationUI_signal();

                // 运行右工位
                machineLog->write("右工位运行中..", Normal);
                sRtn = LSM->runAction(LSM->m_runListProcess_R);
                // 获取执行时间
                qint64 elapsed = timer.elapsed();  // 毫秒为单位
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行

            // 收尾处理
            LSM->m_isStart.store(false);
            LSM->m_triggerMotionTpyes = 0;
            emit setControlEnable_signal(true);
            //运行状态
            LSM->emit showWorkState_signal(0);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
            // 记录最终状态
            machineLog->write(sRtn == 0 ? "双工位运行完成" : "运行中断", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::LS_Run)
        {
            //已经在工作的话，不能进入
            if (LSM->m_isStart.load())
            {
                machineLog->write("已经运行中", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //运行状态
            LSM->emit showWorkState_signal(1);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //左工位运行
            LSM->m_isStart.store(true);

            // 左工位运行流程
            do {
                ////0611测试
#if LOCAL_TEST == 0
                if (!LSM->m_isOneStep.load())
                {
                    //等倍速链进料,单步不走倍速链
                    sRtn = LSM->feedingAction();
                    if (sRtn != 0 && sRtn != 10086)
                    {
                        machineLog->write("倍速链进料控制中断", Normal);
                        break;
                    }
                    if (sRtn == 10086)
                    {
                        //空料或没有码，直接流到下一站
                        machineLog->write("空料或没有码，直接流到下一站", Normal);
                        sRtn = LSM->unloadingAction();
                        if (sRtn != 0)
                        {
                            machineLog->write("倍速链出料控制中断", Normal);
                            break;
                        }
                        continue;
                    }
                }
#endif
                //清除图片
                //emit clearWindow_signal();
                machineLog->write("工位运行中..", Normal);
                //记录操作时间
                QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
                LSM->m_snResult.start_time = timestamp;
                //运行
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // 获取执行时间
                qint64 elapsed = timer.elapsed();  // 毫秒为单位
                emit showCTtime_signal(QString::number(elapsed));
                timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
                LSM->m_snResult.stop_time = timestamp;
                if (sRtn != 0)
                    break;
                if (!LSM->notMes)
                {
                    //MES 过站信息发送
                    LSM->m_snResult.sn = LSM->m_snReq.sn;
                    LSM->m_snResult.station_name = LSM->m_snReq.station_name;
                    LSM->m_snResult.op = LSM->m_snReq.op;

                    //模拟
                    //m_snResult.op = "M001";
                    ////必过
                    //LSM->m_snResult.result = "PASS";
                    // 调用检查接口
                    SnCheckResponse res = LSM->post_result_process(
                        "http://10.23.164.56:8001/MainWebForm.aspx",
                        LSM->m_snResult
                    );
                }
                ////0611测试
#if LOCAL_TEST == 0
                if (!LSM->m_isOneStep.load())
                {
                    //等倍速链出料,单步不走倍速链
                    machineLog->write("工作完成，倍速链出料", Normal);
                    sRtn = LSM->unloadingAction();
                    if (sRtn != 0)
                    {
                        machineLog->write("倍速链出料控制中断", Normal);
                        break;
                    }
                }
#endif

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行
            if (sRtn == 0)
            {
                machineLog->write("工位运行完成", Normal);
                //三色灯黄灯亮
                LSM->setThreeColorLight(1);
                //运行状态
                LSM->emit showWorkState_signal(0);
            }
            else
            {
                //运行状态
                LSM->emit showWorkState_signal(2);
                machineLog->write("工位运行中断", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
            //清理相机占用
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
            //已经在工作的话，不能进入
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("已经运行中", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("工位运行中..", Normal);
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //运行状态
            LSM->emit showWorkState_signal(1);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            //左工位运行
            LSM->m_isStart.store(true);
            if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
            {
                //非单步 第一次如果手上有料要清料 和 !LSM->m_notRobot（使用机器人）
                bool haveProduct = false;
                MODBUS_TCP.readIntCoil(34, 6, haveProduct);
                if (haveProduct)
                {
                    machineLog->write("机器人A手执行清料", Normal);
                    //NG皮带满料
                    if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        //触发蜂鸣器
                        LSM->setButtonLight(3, 1);
                        LSM->m_redLightFlashing.start(1000);
                    }

                    while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        if (LSM->m_isEmergency)
                            break;
                        emit confirmationRequested("NG皮带满料，请取料！！");
                        QThread::msleep(200);
                    }
                    //关闭蜂鸣器
                    LSM->setButtonLight(3, 0);
                    //关闭红灯闪烁
                    LSM->m_redLightFlashing.stop();
                    //打开绿灯
                    LSM->setThreeColorLight(0);
                    sRtn = LSM->runRobotClearHand();
                    if (sRtn != 0)
                    {
                        machineLog->write("机器人A手清料错误！！", Normal);
                        machineLog->write("机器人A手清料错误！！", Err);
                    }
                }
                //检测B爪
                bool haveProductB = false;
                MODBUS_TCP.readIntCoil(34, 7, haveProductB);
                if (haveProductB)
                {
                    machineLog->write("机器人B手执行清料", Normal);
                    //NG皮带满料
                    if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        //触发蜂鸣器
                        LSM->setButtonLight(3, 1);
                        LSM->m_redLightFlashing.start(1000);
                    }

                    while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                        && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                    {
                        if (LSM->m_isEmergency)
                            break;

                        emit confirmationRequested("NG皮带满料，请取料！！");
                        QThread::msleep(200);
                    }
                    //关闭蜂鸣器
                    LSM->setButtonLight(3, 0);
                    //关闭红灯闪烁
                    LSM->m_redLightFlashing.stop();
                    //打开绿灯
                    LSM->setThreeColorLight(0);
                    sRtn = LSM->runRobotClearHand();
                    if (sRtn != 0)
                    {
                        machineLog->write("机器人B手清料错误！！", Normal);
                        machineLog->write("机器人B手清料错误！！", Err);
                    }
                }
               
            }

            if (sRtn == 0)
            {
                // 工位运行流程
                do {
                    
                    LSM->m_mesResponse_ED.state = true;
                    if (LSM->getDI(LS_DI::ED_DI_ProdDetect) == 0)
                    {
                        //治具上有料才运行，没有料先取料
                        if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
                        {
                            //非单步 等第一次皮带进料 和 !LSM->m_notRobot（使用机器人）
                            sRtn = LSM->runRobotCheck();
                            if (sRtn != 0)
                            {
                                machineLog->write("治具等料控制中断", Normal);
                                break;
                            }
                        }
                        //continue;
                    }

                    if (LSM->m_mesResponse_ED.state)
                    {
                        //运行
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
                        //开始运行配方
                        sRtn = LSM->runAction(LSM->m_runListProcess_L);
                        // 获取执行时间
                        qint64 elapsed = timer.elapsed();  // 毫秒为单位
                        emit showCTtime_signal(QString::number(elapsed));
                        if (sRtn != 0)
                            break;

                        if (!LSM->m_isOneStep.load() && !LSM->m_notRobot)
                        {
                            //非单步 出料动作 或 !LSM->m_notRobot（使用机器人）
                            machineLog->write("工作完成，治具取料", Normal);
                            sRtn = LSM->runRobotUnLoading();
                            if (sRtn != 0)
                            {
                                machineLog->write("治具取料控制中断", Normal);
                                break;
                            }

                            machineLog->write("治具取料完成，等待治具放料", Normal);
                            sRtn = LSM->runRobotLoad_Tool();
                            //sRtn = LSM->runRobotCheck();
                            if (sRtn != 0)
                            {
                                machineLog->write("治具放料控制中断", Normal);
                                break;
                            }
                            //LSM->m_runResult.isNG = false;
                            if (!LSM->m_runResult.isNG)
                            {
                                //出料皮带满料
                                if (LSM->getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1 && LSM->getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
                                {
                                    //触发蜂鸣器
                                    LSM->setButtonLight(3, 1);
                                    LSM->m_redLightFlashing.start(1000);
                                }
                                while (LSM->getDI(LS_DI::ED_DI_DischargeBeltReleaseDetect) == 1 && LSM->getDI(LS_DI::ED_DI_DischargeBeltArrivalDetect) == 1)
                                {
                                    if (LSM->m_isEmergency)
                                        break;
                                    emit confirmationRequested("出料皮带满料，请取料！！");
                                    QThread::msleep(200);
                                }
                                //关闭蜂鸣器
                                LSM->setButtonLight(3, 0);
                                //关闭红灯闪烁
                                LSM->m_redLightFlashing.stop();
                                //打开绿灯
                                LSM->setThreeColorLight(0);
                                //触发皮带出料
                                emit robotUnload_signal(false);
                            }
                            else
                            {
                                //NG皮带满料
                                if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                                {
                                    //触发蜂鸣器
                                    LSM->setButtonLight(3, 1);
                                    LSM->m_redLightFlashing.start(1000);
                                }

                                while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                                    && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                                {
                                    if (LSM->m_isEmergency)
                                        break;

                                    emit confirmationRequested("NG皮带满料，请取料！！");
                                    QThread::msleep(200);
                                }
                                //关闭蜂鸣器
                                LSM->setButtonLight(3, 0);
                                //关闭红灯闪烁
                                LSM->m_redLightFlashing.stop();
                                //打开绿灯
                                LSM->setThreeColorLight(0);
                                //触发皮带出料
                                emit robotUnload_signal(true);
                            }

                        }
                    }
                    else
                    {
                        machineLog->write("MES进料检测校验不合格，不允许测试，放料NG皮带", Normal);
                        //NG皮带满料
                        if (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                        {
                            //触发蜂鸣器
                            LSM->setButtonLight(3, 1);
                            LSM->m_redLightFlashing.start(1000);
                        }

                        while (LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Left) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltReleaseDetect_Right) == 1
                            && LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect) == 1)
                        {
                            if (LSM->m_isEmergency)
                                break;
                            emit confirmationRequested("NG皮带满料，请取料！！");
                            QThread::msleep(200);
                        }
                        //关闭蜂鸣器
                        LSM->setButtonLight(3, 0);
                        //关闭红灯闪烁
                        LSM->m_redLightFlashing.stop();
                        //打开绿灯
                        LSM->setThreeColorLight(0);
                        //触发皮带出料
                        emit robotUnload_signal(true);
                    }
                  
                    if (LSM->m_isEmergency)
                        break;
                    if (LSM->m_isOneStep.load())
                    {
                        //单步运行
                        break;
                    }
                    if (LSM->m_isLoadErr.load())
                    {
                        //皮带进料报警
                        sRtn = -1;
                        break;
                    }
                    Sleep(10);
                } while (LSM->m_isLoop.load()); // 循环运行时持续执行

                //清理相机占用
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
                //三色灯黄灯亮
                LSM->setThreeColorLight(1);
                //运行状态
                LSM->emit showWorkState_signal(0);
                machineLog->write("工位运行完成", Normal);
            }
            else
            {   
                //触发蜂鸣器
                for (int i = 0; i < 2; i++)
                {
                    LSM->setButtonLight(3, 1);
                    Sleep(1000);
                    LSM->setButtonLight(3, 0);
                    Sleep(500);
                }
                //三色灯红灯亮
                LSM->setThreeColorLight(2);
                //运行状态
                LSM->emit showWorkState_signal(2);
                machineLog->write("工位运行中断", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
            //清除信号
            MODBUS_TCP.writeIntCoil(134, 0, 0);
            MODBUS_TCP.writeIntCoil(134, 1, 0);
            MODBUS_TCP.writeIntCoil(134, 2, 0);
            MODBUS_TCP.writeIntCoil(134, 3, 0);
            MODBUS_TCP.writeIntCoil(134, 4, 0);
            MODBUS_TCP.writeIntCoil(134, 5, 0);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::BT_Run)
        {
            //博泰 运行
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("已经运行中", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("工位运行中..", Normal);
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //运行状态
            LSM->emit showWorkState_signal(1);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            LSM->m_isStart.store(true);

            // 运行流程
            do {
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // 获取执行时间
                qint64 elapsed = timer.elapsed();  // 毫秒为单位
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行

            if (sRtn == 0)
            {
                //三色灯黄灯亮
                LSM->setThreeColorLight(1);
                //运行状态
                LSM->emit showWorkState_signal(0);
                machineLog->write("工位运行完成", Normal);
            }
            else
            {
                //三色灯红灯亮
                LSM->setThreeColorLight(2);
                //运行状态
                LSM->emit showWorkState_signal(2);
                machineLog->write("工位运行中断", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
            // 记录最终状态
            machineLog->write(sRtn == 0 ? "工位运行完成" : "运行中断", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::JmPin_Run)
        {
		    //金脉pin 运行
		    sRtn = 0;
		    if (LSM->m_isStart.load())
		    {
		    	machineLog->write("已经运行中", Normal);
		    	LSM->m_triggerMotionTpyes = 0;
		    	continue;
		    }
		    machineLog->write("工位运行中..", Normal);
		    //三色灯绿灯亮
		    LSM->setThreeColorLight(0);
		    //运行状态
		    LSM->emit showWorkState_signal(1);
		    //按钮指示灯开
		    LSM->setButtonLight(0, 1);
		    emit setControlEnable_signal(false);
		    LSM->m_isStart.store(true);

		    // 运行流程
		    do {
		    	QElapsedTimer timer;
		    	timer.start();
		    	sRtn = LSM->runAction(LSM->m_runListProcess_L);
		    	// 获取执行时间
		    	qint64 elapsed = timer.elapsed();  // 毫秒为单位
		    	emit showCTtime_signal(QString::number(elapsed));
		    	if (sRtn != 0)
		    		break;

		    } while (LSM->m_isLoop.load()); // 循环运行时持续执行

		    if (sRtn == 0)
		    {
		    	//三色灯黄灯亮
		    	LSM->setThreeColorLight(1);
		    	//运行状态
		    	LSM->emit showWorkState_signal(0);
		    	machineLog->write("工位运行完成", Normal);
		    }
		    else
		    {
		    	//三色灯红灯亮
		    	LSM->setThreeColorLight(2);
		    	//运行状态
		    	LSM->emit showWorkState_signal(2);
		    	machineLog->write("工位运行中断", Normal);
		    }
		    LSM->m_triggerMotionTpyes = 0;
		    LSM->m_isStart.store(false);
		    emit setControlEnable_signal(true);
		    //按钮指示灯关
		    LSM->setButtonLight(0, 0);
		    // 记录最终状态
		    machineLog->write(sRtn == 0 ? "工位运行完成" : "运行中断", Normal);
        }
        else if (LSM->m_triggerMotionTpyes == RS2::ActionFlag::JmHan_Run)
        {
            //金脉焊缝 运行
            sRtn = 0;
            if (LSM->m_isStart.load())
            {
                machineLog->write("已经运行中", Normal);
                LSM->m_triggerMotionTpyes = 0;
                continue;
            }
            machineLog->write("工位运行中..", Normal);
            //三色灯绿灯亮
            LSM->setThreeColorLight(0);
            //运行状态
            LSM->emit showWorkState_signal(1);
            //按钮指示灯开
            LSM->setButtonLight(0, 1);
            emit setControlEnable_signal(false);
            LSM->m_isStart.store(true);

            // 运行流程
            do {
                QElapsedTimer timer;
                timer.start();
                sRtn = LSM->runAction(LSM->m_runListProcess_L);
                // 获取执行时间
                qint64 elapsed = timer.elapsed();  // 毫秒为单位
                emit showCTtime_signal(QString::number(elapsed));
                if (sRtn != 0)
                    break;

            } while (LSM->m_isLoop.load()); // 循环运行时持续执行

            if (sRtn == 0)
            {
                //三色灯黄灯亮
                LSM->setThreeColorLight(1);
                //运行状态
                LSM->emit showWorkState_signal(0);
                machineLog->write("工位运行完成", Normal);
            }
            else
            {
                //三色灯红灯亮
                LSM->setThreeColorLight(2);
                //运行状态
                LSM->emit showWorkState_signal(2);
                machineLog->write("工位运行中断", Normal);
            }
            LSM->m_triggerMotionTpyes = 0;
            LSM->m_isStart.store(false);
            emit setControlEnable_signal(true);
            //按钮指示灯关
            LSM->setButtonLight(0, 0);
            // 记录最终状态
            machineLog->write(sRtn == 0 ? "工位运行完成" : "运行中断", Normal);
        }

        if (LSM->m_isEmergency && LSM->m_cardInitStatus)
        {
            //machineLog->write("触发停止", Normal);
            LSM->AxisStopAll();
            LSM->closeDO();
            //三色灯红灯亮
            LSM->setThreeColorLight(2);
            //运行状态
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
    // 声明DI与报警信息检测列表
    QList<std::pair<QString, QString>> diCheckList;
    // 安全状态标志位
    bool safetyTriggered = false;
    // 根据硬件版本添加门禁项
    if (LSM->m_version == TwoPin)
    {
        diCheckList.push_back({ LS_DI::IN_BtnPause,  "暂停按钮触发！！" });
        diCheckList.push_back({ LS_DI::IN_SafGrate1, "安全光栅1触发！！" });
        diCheckList.push_back({ LS_DI::IN_SafLock1,  "安全锁1触发！！" });
        diCheckList.push_back({ LS_DI::IN_SafLock2,  "安全锁2触发！！" });
        diCheckList.push_back({ LS_DI::IN_SafLock3,  "安全锁3触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door1, "门禁1触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door2, "门禁2触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door3, "门禁3触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door4, "门禁4触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door5, "门禁5触发！！" });
        diCheckList.push_back({ LS_DI::IN_Door6, "门禁6触发！！" });
    }
    else if (LSM->m_version == LsPin)
    {
        diCheckList.push_back({ LS_DI::LS_IN_BtnStop, "停止按钮触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront1, "前1门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront2, "前2门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront3, "前3门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateFront4, "前4门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear1,  "后1门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear2,  "后2门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear3,  "后3门禁触发！！" });
        diCheckList.push_back({ LS_DI::LS_IN_GateRear4,  "后4门禁触发！！" });
    }
    else if (LSM->m_version == EdPin)
    {
        diCheckList.push_back({ LS_DI::ED_DI_BtnStop, "停止按钮触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed1, LS_DI::ED_DI_SafDoor_Feed1 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed2, LS_DI::ED_DI_SafDoor_Feed2 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed3, LS_DI::ED_DI_SafDoor_Feed3 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Feed4, LS_DI::ED_DI_SafDoor_Feed4 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge1, LS_DI::ED_DI_SafDoor_Discharge1 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge2, LS_DI::ED_DI_SafDoor_Discharge2 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge3, LS_DI::ED_DI_SafDoor_Discharge3 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Discharge4, LS_DI::ED_DI_SafDoor_Discharge4 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_NG1, LS_DI::ED_DI_SafDoor_NG1 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_NG2, LS_DI::ED_DI_SafDoor_NG2 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Rear1, LS_DI::ED_DI_SafDoor_Rear1 + "触发！！" });
        diCheckList.push_back({ LS_DI::ED_DI_SafDoor_Rear2, LS_DI::ED_DI_SafDoor_Rear2 + "触发！！" });
    }

    //全局信号监控
    while (!bExit)
    {

        if (LSM->m_cardInitStatus)
        {
            //监控DI信号
            LSM->getAllDI();

            if (LSM->m_version == TwoPin)
            {
                if (LSM->m_DI[LS_DI::IN_BtnStop].state != 1 && lastEmg == 0)
                {
                    //触发急停
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::X].card);
                    machineLog->write(QString::fromStdString("触发急停！！"), Global);
                    LSM->m_isEmergency.store(true);
                    //触发蜂鸣器
                    LSM->setButtonLight(3, 1);

                }
                else if (LSM->m_DI[LS_DI::IN_BtnStop].state == 0 && lastEmg == 1)
                {
                    //之前按过，解开急停
                    lastEmg = 0;
                    //关闭蜂鸣器
                    LSM->setButtonLight(3, 0);
                }
                //复位
                if (LSM->m_DI[LS_DI::IN_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) < BUTTON_TIME)
                        continue;
                    di_reset = QTime::currentTime();
                    //实体按钮复位
                    emit di_resetAction_signal();
                }
                //启动
                if (LSM->m_DI[LS_DI::IN_BtnSetUpL].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) < BUTTON_TIME)
                        continue;
                    di_start = QTime::currentTime();
                    //实体按钮启动
                    emit di_runAction_signal();
                }
                //停止
                if (LSM->m_DI[LS_DI::IN_BtnPause].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) < BUTTON_TIME)
                        continue;
                    di_stop = QTime::currentTime();
                    //实体按钮停止
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
                //    // 时间间隔检查（1秒内不重复触发）
                //    if (fabs(QTime::currentTime().msecsTo(di_stop)) >= 1000) {
                //        di_stop = QTime::currentTime();
                //        emit di_stopAction_signal();
                //    }
                //}
            }
            else if (LSM->m_version == LsPin)
            {
                //立昇
                if (LSM->m_DI[LS_DI::LS_IN_BtnEmergency].state != 1  && lastEmg == 0)
                {
                    //触发急停
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::LS_X].card);
                    machineLog->write(QString::fromStdString("触发急停！！"), Normal);
                    LSM->m_isEmergency.store(true);
                    LSM->m_isRunStop.store(true);
                    lastEmg = 1;
                    //触发蜂鸣器
                    LSM->setButtonLight(3, 1);
                }
                else if (LSM->m_DI[LS_DI::LS_IN_BtnEmergency].state == 1 && lastEmg == 1)
                {
                    //之前按过，解开急停
                    lastEmg = 0;
                    //关闭蜂鸣器
                    LSM->setButtonLight(3, 0);
                    machineLog->write(QString::fromStdString("解除急停！！"), Normal);
                    LSM->m_isEmergency.store(false);
                    LSM->m_isRunStop.store(false);
                }
                //if (LSM->m_DI[LS_DI::LS_IN_BtnSetUp].state == 1)
                //{
                //    //恢复运行状态
                //    LSM->m_isRunStop.store(false);
                //    LSM->setDO(LS_DO::LS_OUT_StartBtnLED, 1);
                //    if (LSM->m_DO[LS_DO::LS_OUT_YellowLight].state == 0)
                //    {
                //        //三色灯黄灯亮
                //        LSM->setDO(LS_DO::LS_OUT_RedLight, 0);
                //        LSM->setDO(LS_DO::LS_OUT_YellowLight, 1);
                //        LSM->setDO(LS_DO::LS_OUT_GreenLight, 0);
                //    }
                //}
                //软限位
                if (LSM->m_isStart.load())
                {
                    for (const auto& axis : LSM->m_Axis)
                    {
                        if (axis.second.position > axis.second.maxTravel)
                        {
                            //触发急停
                            LSM->AxisEmgStop(axis.second.card);
                            machineLog->write(axis.first + "超正软限位", Normal);
                            machineLog->write("触发急停！！", Global);
                            LSM->m_isEmergency.store(true);
                            LSM->m_isRunStop.store(true);
                        }
                        if (axis.second.position < axis.second.minTravel)
                        {
                            //触发急停
                            LSM->AxisEmgStop(axis.second.card);
                            machineLog->write(axis.first + "超负软限位", Normal);
                            machineLog->write("触发急停！！", Global);
                            LSM->m_isEmergency.store(true);
                            LSM->m_isRunStop.store(true);
                        }
                    }
                }

                if (LSM->m_DI[LS_DI::LS_IN_BtnSetUp].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) < BUTTON_TIME)
                        continue;
                    machineLog->write("触发启动！！", Normal);
                    di_start = QTime::currentTime();
                    //实体按钮启动
                    emit di_runAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnStop].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) < BUTTON_TIME)
                        continue;
                    di_stop = QTime::currentTime();
                    //实体按钮停止
                    machineLog->write("触发停止！！", Normal);
                    emit di_stopAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) < BUTTON_TIME)
                        continue;
                    di_reset = QTime::currentTime();
                    //实体按钮复位
                    emit di_resetAction_signal();
                }
                if (LSM->m_DI[LS_DI::LS_IN_BtnLight].state != 0)
                {
                    if (fabs(QTime::currentTime().msecsTo(di_light)) < BUTTON_TIME)
                        continue;
                    di_light = QTime::currentTime();
                    //实体按钮照明
                    auto state = LSM->getDO(LS_DO::LS_OUT_Lighting);
                    if (state == 0)
                        LSM->setButtonLight(4, 1);
                    else
                        LSM->setButtonLight(4, 0);
                }
            }
            else if (LSM->m_version == EdPin)
            {
                //易鼎丰
                if ((LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state != 1 || LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state != 1) && lastEmg == 0)
                {
                    //触发急停
                    LSM->AxisEmgStop(LSM->m_Axis[LS_AxisName::LS_X].card);
                    if (LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state != 1)
                    {
                        machineLog->write(QString::fromStdString("触发急停一！！"), Normal);
                        machineLog->write(QString::fromStdString("触发急停一！！"), Err);
                    }
                    if (LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state != 1)
                    {
                        machineLog->write(QString::fromStdString("触发急停二！！"), Normal);
                        machineLog->write(QString::fromStdString("触发急停二！！"), Err);
                    }
                    LSM->m_isEmergency.store(true);
                    LSM->m_isRunStop.store(true);
                    lastEmg = 1;
                    //触发蜂鸣器
                    LSM->setButtonLight(3, 1);
                }
                else if ((LSM->m_DI[LS_DI::ED_DI_BtnEStop1].state == 1 && LSM->m_DI[LS_DI::ED_DI_BtnEStop2].state == 1) && lastEmg == 1)
                {
                    //之前按过，解开急停
                    lastEmg = 0;
                    //关闭蜂鸣器
                    LSM->setButtonLight(3, 0);
                    machineLog->write(QString::fromStdString("解除急停！！"), Normal);
                    LSM->m_isEmergency.store(false);
                    LSM->m_isRunStop.store(false);
                }

                if (LSM->m_DI[LS_DI::ED_DI_BtnStart].state != 0 && !LSM->m_isStart.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_start)) >= BUTTON_TIME)
                    {
                        di_start = QTime::currentTime();
                        //实体按钮启动
                        emit di_runAction_signal();
                    }
                }
                if (LSM->m_DI[LS_DI::ED_DI_BtnStop].state != 0 && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_stop)) >= BUTTON_TIME)
                    {
                        di_stop = QTime::currentTime();
                        //实体按钮停止
                        emit di_stopAction_signal();
                    }
                }
                if (LSM->m_DI[LS_DI::ED_DI_BtnReset].state != 0 && !LSM->m_isStart.load() && !LSM->m_isEmergency.load())
                {
                    if (fabs(QTime::currentTime().msecsTo(di_reset)) >= BUTTON_TIME)
                    {
                        di_reset = QTime::currentTime();
                        //实体按钮复位
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
                                //还没报警，先报警
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
                        // 运动状态时触发停止，时间间隔检查（1秒内不重复触发）
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
            //用于保存图片
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

            //刷新相机视图
            emit signalFlushed();
            //LeaveCriticalSection(&cameraWindow->m_pcMyCamera->m_hSaveImageMux);

            ////自定义格式不支持显示
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
    //qDebug()<<"相机申请内存时间:" + QString::number(intervalTimeMS) + "ms";
    auto cameraWindow = qg_2dCameraSetting::getCameraWindow();
    if (cameraWindow == NULL)
        return;
    while (!bExit)
    {
        lvm_point_cloud_t pc;
        //QDateTime startTime = QDateTime::currentDateTime();
        if (cameraWindow->m_pcMyCamera_3D == nullptr || cameraWindow->m_pcMyCamera_3D->dev == nullptr)
        {
            qDebug() << "3D采集失败";
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
                qDebug() << "刷新图片";
            }


            pc.p = (lvm_point_t*)malloc(depth_map->head.width * depth_map->head.height * sizeof(lvm_point_t));
            if (CORRECT == lvm_convert_depth_map_to_pcld(depth_map, &pc, depth_map->head.width * depth_map->head.height))
            {
                pc.head.width = depth_map->head.width;
                pc.head.height = depth_map->head.height;
                //if (CORRECT == lvm_save_pcld(dev, cameraWindow->m_pcMyCamera_3D->m_savePath.toStdString().c_str(), &pc))
                //{
                //    qDebug() << "保存图像成功";
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
                //获取点云
                cameraWindow->m_pcMyCamera_3D->ori_cloud = GrabCloud;
                qDebug() << "GrabCloud size = "<< GrabCloud->size();
                free(pc.p);
                //QDateTime endTime = QDateTime::currentDateTime();
                //qint64 intervalTimeMS = startTime.msecsTo(endTime);
                //qDebug()<<"相机采图运行时间:" + QString::number(intervalTimeMS) + "ms";
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

//下层倍速链动作
void Thread::doubleSpeedDownAction()
{
    LSM->downFeedingAction();
    // 退出线程事件循环
    QThread::currentThread()->quit();
}

//机器人皮带上料检测
void Thread::robotLoadingAction()
{
    LSM->runRobotLoad_Belt();
    // 退出线程事件循环
    QThread::currentThread()->quit();
}

//机器人皮带下料
void Thread::robotUnLoadingAction(bool isNG)
{
    machineLog->write("治具放料完成，等待出料", Normal);
    auto sRtn = LSM->runRobotUnLoad_Belt(isNG);
    if (sRtn != 0)
    {
        machineLog->write("出料控制中断", Normal);
    }
    // 退出线程事件循环
    QThread::currentThread()->quit();
}

//单独2Dpin针
void Thread::thread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config)
{
    emit thread_showPin_signal(result, config);
}
//2D和3Dpin针
void Thread::thread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)
{
    emit thread_showPin2Dand3D_signal(result2D, config2D, result3D, config3D);
}
//单独3Dpin针
void Thread::thread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)
{
    emit thread_showPin3D_signal(result, config);
}
//视觉功能的纯结果显示视图
void Thread::thread_showVision_slot(Vision_Message message)
{
    emit thread_showVision_signal(message);
    message.clear();
}
// 显示圆测距的图片
void Thread::thread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    emit thread_showCircleMeasure_signal(cir1, cir2, config, dis, isNG);
}

//直接显示报错NG的图片
void Thread::thread_showNG_slot(HObject photo, PointItemConfig config, bool isNG)
{
    emit thread_showNG_signal(photo, config, isNG);
}

//直接显示图片
void Thread::thread_showImage_slot(QImage img)
{
    emit thread_showImage_signal(img);
}