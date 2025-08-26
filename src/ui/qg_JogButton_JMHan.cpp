#include "qg_JogButton_JMHan.h"
#include "rs_motion.h"

qg_JogButton_JMHan::qg_JogButton_JMHan(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

qg_JogButton_JMHan::~qg_JogButton_JMHan()
{}


//����㶯
void qg_JogButton_JMHan::SwitchAxisPStart(QString axisenum)
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
void qg_JogButton_JMHan::SwitchAxisNStart(QString axisenum)
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
void qg_JogButton_JMHan::on_pushButton_YUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y);
}

//Y������㶯
void qg_JogButton_JMHan::on_pushButton_YDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y);
}

//Yֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_YUp_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Yֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_YDown_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//************************************ X�㶯 ********************************
//X�ᷴ��㶯
void qg_JogButton_JMHan::on_pushButton_XLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X������㶯
void qg_JogButton_JMHan::on_pushButton_XRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//Xֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();

}

//Xֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}


//************************************ U�㶯 ********************************
//U�ᷴ��㶯
void qg_JogButton_JMHan::on_pushButton_ULeft_pressed()
{
    SwitchAxisPStart(LS_AxisName::U);
}

//U������㶯
void qg_JogButton_JMHan::on_pushButton_URight_pressed()
{
    SwitchAxisNStart(LS_AxisName::U);
}

//Uֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_ULeft_released()
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
void qg_JogButton_JMHan::on_pushButton_URight_released()
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
void qg_JogButton_JMHan::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z������㶯
void qg_JogButton_JMHan::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Zֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//Zֹͣ�㶯
void qg_JogButton_JMHan::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer.stop();
    m_nlimitTimer.stop();
}

//������λ���
void qg_JogButton_JMHan::checkPLimit()
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
void qg_JogButton_JMHan::checkNLimit()
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
