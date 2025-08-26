#include "qg_JogButton_ED_New.h"
#include "rs_motion.h"
#include "LTDMC.h"
//���Nר���˱�ɣ��׶���7����
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

//����㶯
void qg_JogButton_ED_New::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // ������ʱ��
    m_plimitTimer->start(50);
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {

        double vel = ui.lineEdit_JogSpeed->text().toDouble();
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            // �����ٶȲ���
            auto rtn = dmc_set_profile_unit(
                (ushort)LSM->m_Axis[axisenum].card,
                (ushort)LSM->m_Axis[axisenum].axisNum,
                LSM->m_Axis[axisenum].minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
                vel,  // �����ٶ�
                LSM->m_Axis[axisenum].accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
                LSM->m_Axis[axisenum].decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
                LSM->m_Axis[axisenum].minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
            );
            if (rtn != 0)
            {
                machineLog->write("�����ٶȲ��� dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
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

//����㶯
void qg_JogButton_ED_New::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    m_runningAxis = axisenum;
    // ������ʱ��
    m_nlimitTimer->start(50);
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            // �����ٶȲ���
            auto rtn = dmc_set_profile_unit(
                (ushort)LSM->m_Axis[axisenum].card,
                (ushort)LSM->m_Axis[axisenum].axisNum,
                LSM->m_Axis[axisenum].minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
                vel,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
                LSM->m_Axis[axisenum].accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
                LSM->m_Axis[axisenum].decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
                LSM->m_Axis[axisenum].minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
            );
            if (rtn != 0)
            {
                machineLog->write("�����ٶȲ��� dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
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

//************************************ Y�㶯 ********************************
//Y�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_YUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y);
}

//Y������㶯
void qg_JogButton_ED_New::on_pushButton_YDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y);
}

//Yֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_YUp_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Yֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_YDown_released()
{
    LSM->AxisStop(LS_AxisName::Y);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ X�㶯 ********************************
//X�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_XLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X������㶯
void qg_JogButton_ED_New::on_pushButton_XRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//Xֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();

}

//Xֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ Z1�㶯 ********************************
//Z1�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_Z1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z1);

}

//Z1������㶯
void qg_JogButton_ED_New::on_pushButton_Z1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z1);
}

//Z1ֹͣ�㶯
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

//Z1ֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_Z1Down_released()
{
    LSM->AxisStop(LS_AxisName::Z1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ Z2�㶯 ********************************
//Z2�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_Z2Up_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z2);
}

//Z2������㶯
void qg_JogButton_ED_New::on_pushButton_Z2Down_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z2);
}

//Z2ֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_Z2Up_released()
{
    LSM->AxisStop(LS_AxisName::Z2);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Z2ֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_Z2Down_released()
{
    LSM->AxisStop(LS_AxisName::Z2);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ U�㶯 ********************************
//U�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_ULeft_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U))
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    SwitchAxisPStart(LS_AxisName::U);
}

//U������㶯
void qg_JogButton_ED_New::on_pushButton_URight_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U))
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    SwitchAxisNStart(LS_AxisName::U);
}

//Uֹͣ�㶯
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

//Uֹͣ�㶯
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

//************************************ Z�㶯 ********************************
//Z�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z������㶯
void qg_JogButton_ED_New::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Zֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//Zֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//************************************ U1�㶯 ********************************
//U1�ᷴ��㶯
void qg_JogButton_ED_New::on_pushButton_U1Left_pressed()
{
    if (!(LSM->m_isHomed.load()|| LSM->m_axisHomeState.U1))
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1������㶯
void qg_JogButton_ED_New::on_pushButton_U1Right_pressed()
{
    if (!(LSM->m_isHomed.load() || LSM->m_axisHomeState.U1))
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1ֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//U1ֹͣ�㶯
void qg_JogButton_ED_New::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
    m_plimitTimer->stop();
    m_nlimitTimer->stop();
}

//������λ���
void qg_JogButton_ED_New::checkPLimit()
{
    if (LSM->m_Axis[m_runningAxis].position >= LSM->m_Axis[m_runningAxis].maxTravel)
    {
        //����ֹͣ
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " �ᳬ������λ", Normal);
        m_plimitTimer->stop();
    }
    return;
}

//������λ���
void qg_JogButton_ED_New::checkNLimit()
{
    if (LSM->m_Axis[m_runningAxis].position <= LSM->m_Axis[m_runningAxis].minTravel)
    {
        //����ֹͣ
        LSM->AxisStop(m_runningAxis);
        machineLog->write(m_runningAxis + " �ᳬ������λ", Normal);
        m_nlimitTimer->stop();
    }
    return;
}