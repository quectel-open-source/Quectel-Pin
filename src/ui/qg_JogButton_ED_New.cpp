#include "qg_JogButton_ED_New.h"
#include "rs_motion.h"
#include "LTDMC.h"
//立昇专用了变成，易鼎丰7个轴
qg_JogButton_ED_New::qg_JogButton_ED_New(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_plimitTimer = new QTimer(this);
    m_nlimitTimer = new QTimer(this);
    connect(m_plimitTimer, &QTimer::timeout, this, &qg_JogButton_ED_New::checkPLimit);
    connect(m_nlimitTimer, &QTimer::timeout, this, &qg_JogButton_ED_New::checkNLimit);
}

qg_JogButton_ED_New::~qg_JogButton_ED_New()
{}

//正向点动
void qg_JogButton_ED_New::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // 创建定时器
    m_plimitTimer->start(50);
    //点动软限位
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {

        double vel = ui.lineEdit_JogSpeed->text().toDouble();
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            // 设置速度参数
            auto rtn = dmc_set_profile_unit(
                (ushort)LSM->m_Axis[axisenum].card,
                (ushort)LSM->m_Axis[axisenum].axisNum,
                LSM->m_Axis[axisenum].minSpeed,  // 起始速度（替换为配置中的 minSpeed）
                vel,  // 运行速度
                LSM->m_Axis[axisenum].accTime,               // 加速时间（替换为配置中的 accTime）
                LSM->m_Axis[axisenum].decTime,               // 减速时间（替换为配置中的 decTime）
                LSM->m_Axis[axisenum].minSpeed   // 停止速度（替换为配置中的 minSpeed）
            );
            if (rtn != 0)
            {
                machineLog->write("设置速度参数 dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
            }
            LSM->AxisVmove(axisenum, true);
        }
        else
        {
            LSM->setSpeed(axisenum, vel);
            LSM->AxisVmove(axisenum, true);
            //LSM->WriteAxisRunVel(axisenum, vel);
        }

    }
}

//反向点动
void qg_JogButton_ED_New::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // 创建定时器
    m_nlimitTimer->start(50);
    //点动软限位
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            // 设置速度参数
            auto rtn = dmc_set_profile_unit(
                (ushort)LSM->m_Axis[axisenum].card,
                (ushort)LSM->m_Axis[axisenum].axisNum,
                LSM->m_Axis[axisenum].minSpeed,  // 起始速度（替换为配置中的 minSpeed）
                vel,  // 运行速度（替换为配置中的 maxSpeed）
                LSM->m_Axis[axisenum].accTime,               // 加速时间（替换为配置中的 accTime）
                LSM->m_Axis[axisenum].decTime,               // 减速时间（替换为配置中的 decTime）
                LSM->m_Axis[axisenum].minSpeed   // 停止速度（替换为配置中的 minSpeed）
            );
            if (rtn != 0)
            {
                machineLog->write("设置速度参数 dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
            }
            LSM->AxisVmove(axisenum, false);
            //LSM->WriteAxisRunVel(axisenum, vel);
        }
        else
        {
            LSM->setSpeed(axisenum, vel);
            LSM->AxisVmove(axisenum, false);
            //LSM->WriteAxisRunVel(axisenum, -vel);
        }
    }

}

//************************************ Y点动 ********************************
//Y轴反向点动
void qg_JogButton_ED_New::on_pushButton_YUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y);
}

//Y轴正向点动
void qg_JogButton_ED_New::on_pushButton_YDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y);
}

//Y停止点动
void qg_JogButton_ED_New::on_pushButton_YUp_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Y停止点动
void qg_JogButton_ED_New::on_pushButton_YDown_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ X点动 ********************************
//X轴反向点动
void qg_JogButton_ED_New::on_pushButton_XLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X轴正向点动
void qg_JogButton_ED_New::on_pushButton_XRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//X停止点动
void qg_JogButton_ED_New::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();

}

//X停止点动
void qg_JogButton_ED_New::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ Z1点动 ********************************
//Z1轴反向点动
void qg_JogButton_ED_New::on_pushButton_Z1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z1);

}

//Z1轴正向点动
void qg_JogButton_ED_New::on_pushButton_Z1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z1);
}

//Z1停止点动
void qg_JogButton_ED_New::on_pushButton_Z1Up_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    if (m_plimitTimer->isActive())
    {
        m_plimitTimer->stop();
    }
    if (m_nlimitTimer->isActive())
    {
        m_nlimitTimer->stop();
    }

}

//Z1停止点动
void qg_JogButton_ED_New::on_pushButton_Z1Down_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ Z2点动 ********************************
//Z2轴反向点动
void qg_JogButton_ED_New::on_pushButton_Z2Up_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z2);
}

//Z2轴正向点动
void qg_JogButton_ED_New::on_pushButton_Z2Down_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z2);
}

//Z2停止点动
void qg_JogButton_ED_New::on_pushButton_Z2Up_released()
{
    LSM->AxisStop(LS_AxisName::Z2);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Z2停止点动
void qg_JogButton_ED_New::on_pushButton_Z2Down_released()
{
    LSM->AxisStop(LS_AxisName::Z2);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ U点动 ********************************
//U轴反向点动
void qg_JogButton_ED_New::on_pushButton_ULeft_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U))
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    SwitchAxisPStart(LS_AxisName::U);
}

//U轴正向点动
void qg_JogButton_ED_New::on_pushButton_URight_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U))
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    SwitchAxisNStart(LS_AxisName::U);
}

//U停止点动
void qg_JogButton_ED_New::on_pushButton_ULeft_released()
{
    LSM->AxisStop(LS_AxisName::U);
    if (m_plimitTimer->isActive())
    {
        m_plimitTimer->stop();
    }
    if (m_nlimitTimer->isActive())
    {
        m_nlimitTimer->stop();
    }
}

//U停止点动
void qg_JogButton_ED_New::on_pushButton_URight_released()
{
    LSM->AxisStop(LS_AxisName::U);
    if (m_plimitTimer->isActive())
    {
        m_plimitTimer->stop();
    }
    if (m_nlimitTimer->isActive())
    {
        m_nlimitTimer->stop();
    }
}

void qg_JogButton_ED_New::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
}

//************************************ Z点动 ********************************
//Z轴反向点动
void qg_JogButton_ED_New::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z轴正向点动
void qg_JogButton_ED_New::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Z停止点动
void qg_JogButton_ED_New::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Z停止点动
void qg_JogButton_ED_New::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ U1点动 ********************************
//U1轴反向点动
void qg_JogButton_ED_New::on_pushButton_U1Left_pressed()
{
    if (!(LSM->m_isHomed.load()|| LSM->m_axisHomeState.U1))
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1轴正向点动
void qg_JogButton_ED_New::on_pushButton_U1Right_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U1))
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1停止点动
void qg_JogButton_ED_New::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//U1停止点动
void qg_JogButton_ED_New::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//正软限位监控
void qg_JogButton_ED_New::checkPLimit()
{
    if (LSM->m_Axis[m_runningAxis].position >= LSM->m_Axis[m_runningAxis].maxTravel)
    {
        //触发停止
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " 轴超正软限位", Normal);
        m_plimitTimer->stop();
    }
    return;
}

//负软限位监控
void qg_JogButton_ED_New::checkNLimit()
{
    if (LSM->m_Axis[m_runningAxis].position <= LSM->m_Axis[m_runningAxis].minTravel)
    {
        //触发停止
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " 轴超负软限位", Normal);
        m_nlimitTimer->stop();
    }
    return;
}