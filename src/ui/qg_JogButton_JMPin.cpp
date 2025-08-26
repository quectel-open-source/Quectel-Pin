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


//����㶯
void qg_JogButton_JMPin::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // ������ʱ��
    m_plimitTimer.start(50);
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, true);
    }
}

//����㶯
void qg_JogButton_JMPin::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // ������ʱ��
    m_nlimitTimer.start(50);
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, false);
    }

}

//************************************ Y�㶯 ********************************
//Y�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_YUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y);
}

//Y������㶯
void qg_JogButton_JMPin::on_pushButton_YDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y);
}

//Yֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_YUp_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Yֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_YDown_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ X�㶯 ********************************
//X�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_XLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X������㶯
void qg_JogButton_JMPin::on_pushButton_XRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//Xֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();

}

//Xֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ X1�㶯 ********************************
//X1�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_X1Left_pressed()
{
    SwitchAxisNStart(LS_AxisName::X1);
}

//X1������㶯
void qg_JogButton_JMPin::on_pushButton_X1Right_pressed()
{
    SwitchAxisPStart(LS_AxisName::X1);
}

//X1ֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_X1Left_released()
{
    LSM->AxisStop(LS_AxisName::X1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();

}

//X1ֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_X1Right_released()
{
    LSM->AxisStop(LS_AxisName::X1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}


//************************************ Z1�㶯 ********************************
//Z1�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_Z1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z1);

}

//Z1������㶯
void qg_JogButton_JMPin::on_pushButton_Z1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z1);
}

//Z1ֹͣ�㶯
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

//Z1ֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_Z1Down_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}


//************************************ U�㶯 ********************************
//U�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_ULeft_pressed()
{
    SwitchAxisPStart(LS_AxisName::U);
}

//U������㶯
void qg_JogButton_JMPin::on_pushButton_URight_pressed()
{
    SwitchAxisNStart(LS_AxisName::U);
}

//Uֹͣ�㶯
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

//Uֹͣ�㶯
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

//************************************ Z�㶯 ********************************
//Z�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z������㶯
void qg_JogButton_JMPin::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Zֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Zֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ U1�㶯 ********************************
//U1�ᷴ��㶯
void qg_JogButton_JMPin::on_pushButton_U1Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1������㶯
void qg_JogButton_JMPin::on_pushButton_U1Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1ֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//U1ֹͣ�㶯
void qg_JogButton_JMPin::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//������λ���
void qg_JogButton_JMPin::checkPLimit()
{
    if (LSM->m_Axis[m_runningAxis].position >= LSM->m_Axis[m_runningAxis].maxTravel)
    {
        //����ֹͣ
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " �ᳬ������λ", Normal);
        m_plimitTimer.stop();
    }
    return;
}

//������λ���
void qg_JogButton_JMPin::checkNLimit()
{
    if (LSM->m_Axis[m_runningAxis].position <= LSM->m_Axis[m_runningAxis].minTravel)
    {
        //����ֹͣ
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " �ᳬ������λ", Normal);
        m_nlimitTimer.stop();
    }
    return;
}
