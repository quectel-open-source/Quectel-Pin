#include "qg_JogButton_JMPin.h"
#include "rs_motion.h"

qg_JogButton_JMPin::qg_JogButton_JMPin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(&m_plimitTimer, &QTimer::timeout, this, &qg_JogButton_JMPin::checkPLimit);
	connect(&m_nlimitTimer, &QTimer::timeout, this, &qg_JogButton_JMPin::checkNLimit);
}

qg_JogButton_JMPin::~qg_JogButton_JMPin()
{}


//正向点动
void qg_JogButton_JMPin::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // 创建定时器
    m_plimitTimer.start(50);
    //点动软限位
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, true);
    }
}

//反向点动
void qg_JogButton_JMPin::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // 创建定时器
    m_nlimitTimer.start(50);
    //点动软限位
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, false);
    }

}

//************************************ Y点动 ********************************
//Y轴反向点动
void qg_JogButton_JMPin::on_pushButton_YUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y);
}

//Y轴正向点动
void qg_JogButton_JMPin::on_pushButton_YDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y);
}

//Y停止点动
void qg_JogButton_JMPin::on_pushButton_YUp_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Y停止点动
void qg_JogButton_JMPin::on_pushButton_YDown_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ X点动 ********************************
//X轴反向点动
void qg_JogButton_JMPin::on_pushButton_XLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X轴正向点动
void qg_JogButton_JMPin::on_pushButton_XRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//X停止点动
void qg_JogButton_JMPin::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();

}

//X停止点动
void qg_JogButton_JMPin::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ X1点动 ********************************
//X1轴反向点动
void qg_JogButton_JMPin::on_pushButton_X1Left_pressed()
{
    SwitchAxisNStart(LS_AxisName::X1);
}

//X1轴正向点动
void qg_JogButton_JMPin::on_pushButton_X1Right_pressed()
{
    SwitchAxisPStart(LS_AxisName::X1);
}

//X1停止点动
void qg_JogButton_JMPin::on_pushButton_X1Left_released()
{
    LSM->AxisStop(LS_AxisName::X1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();

}

//X1停止点动
void qg_JogButton_JMPin::on_pushButton_X1Right_released()
{
    LSM->AxisStop(LS_AxisName::X1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}


//************************************ Z1点动 ********************************
//Z1轴反向点动
void qg_JogButton_JMPin::on_pushButton_Z1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z1);

}

//Z1轴正向点动
void qg_JogButton_JMPin::on_pushButton_Z1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z1);
}

//Z1停止点动
void qg_JogButton_JMPin::on_pushButton_Z1Up_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    if (m_plimitTimer.isActive())
    {
        m_plimitTimer.stop();
    }
    if (m_nlimitTimer.isActive())
    {
        m_nlimitTimer.stop();
    }

}

//Z1停止点动
void qg_JogButton_JMPin::on_pushButton_Z1Down_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}


//************************************ U点动 ********************************
//U轴反向点动
void qg_JogButton_JMPin::on_pushButton_ULeft_pressed()
{
    SwitchAxisPStart(LS_AxisName::U);
}

//U轴正向点动
void qg_JogButton_JMPin::on_pushButton_URight_pressed()
{
    SwitchAxisNStart(LS_AxisName::U);
}

//U停止点动
void qg_JogButton_JMPin::on_pushButton_ULeft_released()
{
    LSM->AxisStop(LS_AxisName::U);
    if (m_plimitTimer.isActive())
    {
        m_plimitTimer.stop();
    }
    if (m_nlimitTimer.isActive())
    {
        m_nlimitTimer.stop();
    }
}

//U停止点动
void qg_JogButton_JMPin::on_pushButton_URight_released()
{
    LSM->AxisStop(LS_AxisName::U);
    if (m_plimitTimer.isActive())
    {
        m_plimitTimer.stop();
    }
    if (m_nlimitTimer.isActive())
    {
        m_nlimitTimer.stop();
    }
}

//************************************ Z点动 ********************************
//Z轴反向点动
void qg_JogButton_JMPin::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z轴正向点动
void qg_JogButton_JMPin::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Z停止点动
void qg_JogButton_JMPin::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Z停止点动
void qg_JogButton_JMPin::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ U1点动 ********************************
//U1轴反向点动
void qg_JogButton_JMPin::on_pushButton_U1Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1轴正向点动
void qg_JogButton_JMPin::on_pushButton_U1Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1停止点动
void qg_JogButton_JMPin::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//U1停止点动
void qg_JogButton_JMPin::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//正软限位监控
void qg_JogButton_JMPin::checkPLimit()
{
    if (LSM->m_Axis[m_runningAxis].position >= LSM->m_Axis[m_runningAxis].maxTravel)
    {
        //触发停止
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " 轴超正软限位", Normal);
        m_plimitTimer.stop();
    }
    return;
}

//负软限位监控
void qg_JogButton_JMPin::checkNLimit()
{
    if (LSM->m_Axis[m_runningAxis].position <= LSM->m_Axis[m_runningAxis].minTravel)
    {
        //触发停止
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " 轴超负软限位", Normal);
        m_nlimitTimer.stop();
    }
    return;
}
