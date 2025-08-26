#include "qg_JogButton_ED.h"
#include "rs_motion.h"
#include "LTDMC.h"
//���Nר���˱�ɣ��׶���7����
qg_JogButton_ED::qg_JogButton_ED(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    switch (LSM->m_version)
    {
    case ENUM_VERSION::LsPin:
        //���N���������5�ᣬ����һ������
        ui.pushButton_YUp->setToolTip("Z��");
        ui.pushButton_YDown->setToolTip("Z��");
        ui.pushButton_Z1Up->setToolTip("Y1��");
        ui.pushButton_Z2Up->setToolTip("Y2��");
        ui.pushButton_Z1Down->setToolTip("Y1��");
        ui.pushButton_Z2Down->setToolTip("Y2��");
        break;
    default:
        break;
    }
}

qg_JogButton_ED::~qg_JogButton_ED()
{}

//����㶯
void qg_JogButton_ED::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {

        double vel = ui.lineEdit_JogSpeed->text().toDouble();
        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            //// �����ٶȲ���
            //auto rtn = dmc_set_profile_unit(
            //    (ushort)LSM->m_Axis[axisenum].card,
            //    (ushort)LSM->m_Axis[axisenum].axisNum,
            //    LSM->m_Axis[axisenum].minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
            //    vel,  // �����ٶ�
            //    LSM->m_Axis[axisenum].accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
            //    LSM->m_Axis[axisenum].decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
            //    LSM->m_Axis[axisenum].minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
            //);
            //if (rtn != 0)
            //{
            //    machineLog->write("�����ٶȲ��� dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
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

//����㶯
void qg_JogButton_ED::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {


        if (LSM->m_motionVersion == ENUM_MOTIONVERSION::LS_EtherCat)
        {
            //// �����ٶȲ���
            //auto rtn = dmc_set_profile_unit(
            //    (ushort)LSM->m_Axis[axisenum].card,
            //    (ushort)LSM->m_Axis[axisenum].axisNum,
            //    LSM->m_Axis[axisenum].minSpeed,  // ��ʼ�ٶȣ��滻Ϊ�����е� minSpeed��
            //    vel,  // �����ٶȣ��滻Ϊ�����е� maxSpeed��
            //    LSM->m_Axis[axisenum].accTime,               // ����ʱ�䣨�滻Ϊ�����е� accTime��
            //    LSM->m_Axis[axisenum].decTime,               // ����ʱ�䣨�滻Ϊ�����е� decTime��
            //    LSM->m_Axis[axisenum].minSpeed   // ֹͣ�ٶȣ��滻Ϊ�����е� minSpeed��
            //);
            //if (rtn != 0)
            //{
            //    machineLog->write("�����ٶȲ��� dmc_set_profile_unit rtn = " + QString::number(rtn), Machine);
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

//************************************ Y�㶯 ********************************
//Y�ᷴ��㶯
void qg_JogButton_ED::on_pushButton_YUp_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisNStart(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
}

//Y������㶯
void qg_JogButton_ED::on_pushButton_YDown_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisPStart(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }

}

//Yֹͣ�㶯
void qg_JogButton_ED::on_pushButton_YUp_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
    
}

//Yֹͣ�㶯
void qg_JogButton_ED::on_pushButton_YDown_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Y);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Z);
        break;
    default:
        break;
    }
}

//************************************ X�㶯 ********************************
//X�ᷴ��㶯
void qg_JogButton_ED::on_pushButton_XLeft_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        qDebug() << "bbbbbbbbbb";
        SwitchAxisNStart(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//X������㶯
void qg_JogButton_ED::on_pushButton_XRight_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        qDebug() << "cccccccccc";
        SwitchAxisPStart(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//Xֹͣ�㶯
void qg_JogButton_ED::on_pushButton_XLeft_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_X);
        qDebug() << "aaaaaaaaaaa";
        break;
    default:
        break;
    }

}

//Xֹͣ�㶯
void qg_JogButton_ED::on_pushButton_XRight_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_X);
        break;
    default:
        break;
    }
}

//************************************ Z1�㶯 ********************************
//Z1�ᷴ��㶯
void qg_JogButton_ED::on_pushButton_Z1Up_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisPStart(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }

}

//Z1������㶯
void qg_JogButton_ED::on_pushButton_Z1Down_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisNStart(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//Z1ֹͣ�㶯
void qg_JogButton_ED::on_pushButton_Z1Up_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//Z1ֹͣ�㶯
void qg_JogButton_ED::on_pushButton_Z1Down_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z1);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Y1);
        break;
    default:
        break;
    }
}

//************************************ Z2�㶯 ********************************
//Z2�ᷴ��㶯
void qg_JogButton_ED::on_pushButton_Z2Up_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisNStart(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2������㶯
void qg_JogButton_ED::on_pushButton_Z2Down_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisPStart(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2ֹͣ�㶯
void qg_JogButton_ED::on_pushButton_Z2Up_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//Z2ֹͣ�㶯
void qg_JogButton_ED::on_pushButton_Z2Down_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::Z2);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_Y2);
        break;
    default:
        break;
    }
}

//************************************ U�㶯 ********************************
//U�ᷴ��㶯
void qg_JogButton_ED::on_pushButton_ULeft_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisPStart(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisNStart(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//U������㶯
void qg_JogButton_ED::on_pushButton_URight_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        SwitchAxisNStart(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        SwitchAxisPStart(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//Uֹͣ�㶯
void qg_JogButton_ED::on_pushButton_ULeft_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

//Uֹͣ�㶯
void qg_JogButton_ED::on_pushButton_URight_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::EdPin:
        LSM->AxisStop(LS_AxisName::U);
        break;
    case ENUM_VERSION::LsPin:
        //���N���
        LSM->AxisStop(LS_AxisName::LS_U);
        break;
    default:
        break;
    }
}

void qg_JogButton_ED::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);  
}