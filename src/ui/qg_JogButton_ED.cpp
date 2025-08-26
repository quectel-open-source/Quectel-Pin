#include "qg_JogButton_ED.h"
#include "rs_motion.h"
#include "LTDMC.h"
//立昇专用了变成，易鼎丰7个轴
qg_JogButton_ED::qg_JogButton_ED(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    switch (LSM->m_version)
    {
    case ENUM_VERSION::LsPin:
        //立昇标机，都是5轴，共用一个键盘
        ui.pushButton_YUp->setToolTip("Z上");
        ui.pushButton_YDown->setToolTip("Z下");
        ui.pushButton_Z1Up->setToolTip("Y1正");
        ui.pushButton_Z2Up->setToolTip("Y2负");
        ui.pushButton_Z1Down->setToolTip("Y1负");
        ui.pushButton_Z2Down->setToolTip("Y2正");
        break;
    default:
        break;
    }
}

qg_JogButton_ED::~qg_JogButton_ED()
{}

//正向点动
void qg_JogButton_ED::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //点动软限位
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {

        double vel = ui.lineEdit_JogSpeed->text().toDouble();
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            //// 设置速度参数
            //auto rtn = dmc_set_profile_unit(
            //    (ushort)LSM->m_Axis[axisenum].card,
            //    (ushort)LSM->m_Axis[axisenum].axisNum,
            //    LSM->m_Axis[axisenum].minSpeed,  // 起始速度（替换为配置中的 minSpeed）
            //    vel,  // 运行速度
            //    LSM->m_Axis[axisenum].accTime,               // 加速时间（替换为配置中的 accTime）
            //    LSM->m_Axis[axisenum].decTime,               // 减速时间（替换为配置中的 decTime）
            //    LSM->m_Axis[axisenum].minSpeed   // 停止速度（替换为配置中的 minSpeed）
            //);
            //if (rtn != 0)
            //{
            //    machineLog->write("设置速度参数 dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
            //}
            LSM->setSpeed(axisenum, vel);
            LSM->AxisVmove(axisenum, true);
        }
        else
        {
            LSM->AxisVmove(axisenum, true);
            LSM->WriteAxisRunVel(axisenum, vel);
        }
       
    }
}

//反向点动
void qg_JogButton_ED::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //点动软限位
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {


        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            //// 设置速度参数
            //auto rtn = dmc_set_profile_unit(
            //    (ushort)LSM->m_Axis[axisenum].card,
            //    (ushort)LSM->m_Axis[axisenum].axisNum,
            //    LSM->m_Axis[axisenum].minSpeed,  // 起始速度（替换为配置中的 minSpeed）
            //    vel,  // 运行速度（替换为配置中的 maxSpeed）
            //    LSM->m_Axis[axisenum].accTime,               // 加速时间（替换为配置中的 accTime）
            //    LSM->m_Axis[axisenum].decTime,               // 减速时间（替换为配置中的 decTime）
            //    LSM->m_Axis[axisenum].minSpeed   // 停止速度（替换为配置中的 minSpeed）
            //);
            //if (rtn != 0)
            //{
            //    machineLog->write("设置速度参数 dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
            //}
            LSM->setSpeed(axisenum, vel);
            LSM->AxisVmove(axisenum, false);
            //LSM->WriteAxisRunVel(axisenum, vel);
        }
        else
        {
            LSM->AxisVmove(axisenum, false);
            LSM->WriteAxisRunVel(axisenum, -vel);
        }
    }

}

//************************************ Y点动 ********************************
//Y轴反向点动
void qg_JogButton_ED::on_pushButton_YUp_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisNStart(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
}

//Y轴正向点动
void qg_JogButton_ED::on_pushButton_YDown_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisPStart(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }

}

//Y停止点动
void qg_JogButton_ED::on_pushButton_YUp_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
    
}

//Y停止点动
void qg_JogButton_ED::on_pushButton_YDown_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
}

//************************************ X点动 ********************************
//X轴反向点动
void qg_JogButton_ED::on_pushButton_XLeft_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        qDebug() << "bbbbbbbbbb";
        SwitchAxisNStart(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//X轴正向点动
void qg_JogButton_ED::on_pushButton_XRight_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        qDebug() << "cccccccccc";
        SwitchAxisPStart(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//X停止点动
void qg_JogButton_ED::on_pushButton_XLeft_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_X);
        qDebug() << "aaaaaaaaaaa";
        break;
    default:
        break;
    }

}

//X停止点动
void qg_JogButton_ED::on_pushButton_XRight_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//************************************ Z1点动 ********************************
//Z1轴反向点动
void qg_JogButton_ED::on_pushButton_Z1Up_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisPStart(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }

}

//Z1轴正向点动
void qg_JogButton_ED::on_pushButton_Z1Down_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisNStart(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//Z1停止点动
void qg_JogButton_ED::on_pushButton_Z1Up_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//Z1停止点动
void qg_JogButton_ED::on_pushButton_Z1Down_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//************************************ Z2点动 ********************************
//Z2轴反向点动
void qg_JogButton_ED::on_pushButton_Z2Up_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisNStart(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2轴正向点动
void qg_JogButton_ED::on_pushButton_Z2Down_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisPStart(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2停止点动
void qg_JogButton_ED::on_pushButton_Z2Up_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2停止点动
void qg_JogButton_ED::on_pushButton_Z2Down_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//************************************ U点动 ********************************
//U轴反向点动
void qg_JogButton_ED::on_pushButton_ULeft_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisNStart(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//U轴正向点动
void qg_JogButton_ED::on_pushButton_URight_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        SwitchAxisPStart(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//U停止点动
void qg_JogButton_ED::on_pushButton_ULeft_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//U停止点动
void qg_JogButton_ED::on_pushButton_URight_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //立昇标机
        LSM->AxisStop(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

void qg_JogButton_ED::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);  
}