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
    WORD wCardNum;      //定义卡数
    WORD arrwCardList[8];   //定义卡号数组
    DWORD arrdwCardTypeList[8];   //定义各卡类型
    bool b1 = 0;
    if (dmc_board_init() <= 0)      //控制卡的初始化操作
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("控制卡连接异常，请检查配置及硬件！"));
        machineLog->write(QString::fromStdString("控制卡连接异常，请检查配置及硬件！"), Normal);
    else
        b1 = true;
    dmc_get_CardInfList(&wCardNum, arrdwCardTypeList, arrwCardList);    //获取正在使用的卡号列表
    //m_wCard = arrwCardList[0];

    bool b2 = CANInit(0, 1);//初始化CAN-IO
    bool b3 = CANInit(0, 2);

    if (!b2)
    {
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO扩展卡1连接异常，请检查配置及硬件！"));
        machineLog->write(QString::fromStdString("IO扩展卡1连接异常，请检查配置及硬件！"), Normal);
        //EventBus.Default.Log(new LogInfo("IO扩展卡1连接异常！", LogLevel.Error));
    }
    if (!b3)
    {
        //QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("IO扩展卡2连接异常，请检查配置及硬件！"));
        machineLog->write(QString::fromStdString("IO扩展卡2连接异常，请检查配置及硬件！"), Normal);
        //EventBus.Default.Log(new LogInfo("IO扩展卡2连接异常！", LogLevel.Error));
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


//读取SEVON信号 读伺服状态
bool LS_Motion::ReadAxisServeOn(int CardId, int AxisId)
{
    return (dmc_read_sevon_pin((WORD)CardId, (WORD)AxisId) == 0);
}

//输出SEVON信号
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
    dmc_set_profile(card, axis, StartVel, RunVel, AccTime, DccTime, StopVel);//设置速度参数
    dmc_set_s_profile(card, axis, 0, STime);                                    //设置S段速度参数
    // LTDMC.dmc_set_home_pin_logic(card, axis, 0, 0);//设置原点低电平有效

    // 设置原点有效电平
    dmc_set_home_pin_logic(
        (ushort)card,
        (ushort)axis,
        0, // 0低电平有效 1高电平有效（写死为低电平有效）
        0  // 保留参数（写死为 0）
    );

    // 设置正负限位有效电平
    dmc_set_el_mode(
        (ushort)axis,
        (ushort)axis,
        1, // 是否允许正负硬限位（写死为允许）
        0, // 正向硬限位有效电平：0低电平有效（写死为低电平有效）
        1  // 负向硬限位有效电平：1高电平有效（写死为高电平有效）
    );

    // 设置回原模式
    dmc_set_homemode(
        (ushort)axis,
        (ushort)axis,
        homeDir == 1 ? (ushort)1 : (ushort)0,  // 回零方向：0负向 1正向（替换为配置中的 homeDir）
        0,                      // 回零速度模式：0低速 1高速（写死为低速）
        1,                      // 回零模式（写死为模式 1）
        2                       // 保留参数（写死为 2）
    );

    // 设置回零到限位是否反找
    dmc_set_home_el_return(
        (ushort)card,
        (ushort)axis,
        1 // 0不反找 1反找（写死为启用反找）
    );

    // 映射轴的急停信号
    dmc_set_axis_io_map(
        (ushort)card,
        (ushort)axis,
        3,                // 信号类型：轴急停信号（写死为 3）
        6,                // 轴 IO 映射类型：通用输入端口（写死为 6）
        (ushort)stopIO,  // 急停信号对应的 IO 索引
        0.01             // 滤波时间（写死为 0.01）
    );

    // 设置急停模式
    dmc_set_emg_mode(
        (ushort)card,
        (ushort)axis,
        1, // 是否允许使用急停信号（写死为启用）
        RS2::IOStateEnum::High  // 有效电平：0低电平有效 1高电平有效（写死为高电平有效）
    );
}

//急停IO配置
void LS_Motion::MapEmgIO(int CardId, int AxisId, int IOindex, ushort validlevel)
{
    //映射轴的急停信号
    dmc_set_axis_io_map((ushort)CardId, (ushort)AxisId,
        3,//信号类型：轴急停信号
        6,//轴IO映射类型:通用输入端口
        (ushort)IOindex,//IO索引
        0.01);//滤波时间
    dmc_set_emg_mode((ushort)CardId, (ushort)AxisId,
        1,//允许使用急停信号
        validlevel);//有效电平
}


//关闭板卡
void LS_Motion::motionClose()
{
    ////m_isLoop = false;
    ////m_isPause = false;
    ////m_isEmergency = false;
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis); //停止所有轴
        //WriteAxisServeOn(axis, false);   //下使能
    }

    //断连接
    CANClose(1);
    CANClose(2);
    dmc_board_close();
}


//轴停止
void LS_Motion::AxisStop(int CardId, int AxisId)
{
    dmc_stop((ushort)CardId, (ushort)AxisId, 0);// 0:减速停止，1：紧急停止
}

//所有轴急停
void LS_Motion::AxisEmgStop(int CardId)
{
    dmc_emg_stop((ushort)CardId);
}

//关闭CAN卡
void LS_Motion::CANClose(int CardId)
{
    ushort usCanNum = 0, usCanStatus = 0, usCardNum = CardId;
    dmc_get_can_state(usCardNum, &usCanNum, &usCanStatus);

    //usCanNum 范围 1-8
    for (ushort i = usCanNum; i < usCanNum + 1; i++)
    {
        nmc_set_connect_state(usCardNum, i, 0, 0);//设置CAN通讯状态，断开 
    }
}


//参数回零
void LS_Motion::GoHome(int CardId, int AxisId)
{
    dmc_home_move((ushort)CardId, (ushort)AxisId);
}

//清零位置 和编码器
void LS_Motion::AxisClearPosition(int CardId, int AxisId)
{
    dmc_set_position((ushort)CardId, (ushort)AxisId, 0);
    dmc_set_encoder((ushort)CardId, (ushort)AxisId, 0);
}

// 检查指定轴的运动状态，返回 true 表示正在运行，false 表示已停止
bool LS_Motion::IsRuning(int CardId, int AxisId)
{
    return (dmc_check_done((ushort)CardId, (ushort)AxisId) == 0);
}

//停止所有轴
void LS_Motion::AxisStopAll()
{
    for (auto axis : LS_AxisName::allAxis)
    {
        AxisStop(axis);
    }
}

//定速运动 0:负方向，1：正方向
void LS_Motion::VMove(int CardId, int AxisId, bool pdir)
{
    ushort dir = 0;
    if (pdir)
        dir = 1;
    auto rtn = dmc_vmove((ushort)CardId, (ushort)AxisId, dir);
    //machineLog->write(QString::number(AxisId) + " VMove = " + QString::number(rtn), Normal);
}


// 实时设置轴速度(在线变速)
void LS_Motion::WriteAxisRealSpeed(int CardId, int AxisId, double RealSpeed)
{
    short rtn = dmc_change_speed((ushort)CardId, (ushort)AxisId, RealSpeed, 0);
}

//读脉冲位置
double LS_Motion::ReadPosition(int CardId, int AxisId)
{
    return (dmc_get_position((ushort)CardId, (ushort)AxisId));
}


// 读取轴报警端口电平
bool LS_Motion::isAxisErc(int CardId, int AxisId)
{
    bool res = (dmc_read_erc_pin((ushort)CardId, (ushort)AxisId) == 0);
    return res;
}


// 清除轴报警状态
void LS_Motion::AxisClear(int CardId, int AxisId)
{
    dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    dmc_write_erc_pin((ushort)CardId, (ushort)AxisId, 0);
}

// 定长绝对运动
int LS_Motion::PMove_abs(int CardId, int AxisId, int value)
{
    int rtn = dmc_pmove((ushort)CardId, (ushort)AxisId, value, 1); //0：相对坐标模式，1：绝对坐标模式
    return rtn;
}

// 定长相对运动
void LS_Motion::PMove_rel(int CardId, int AxisId, int value)
{
    dmc_pmove((ushort)CardId, (ushort)AxisId, value, 0); //0：相对坐标模式，1：绝对坐标模式
}
