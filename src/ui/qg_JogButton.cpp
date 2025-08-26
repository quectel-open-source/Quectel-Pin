#include "qg_JogButton.h"
#include "rs_motion.h"

qg_JogButton::qg_JogButton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    //250717 ����λU��Ż�ȥ
    //ui.pushButton_U1Left->hide();
    //ui.pushButton_U1Right->hide();
    ui.pushButton_U2Left->hide();
    ui.pushButton_U2Right->hide();

}

qg_JogButton::~qg_JogButton()
{}

//����㶯
void qg_JogButton::SwitchAxisPStart(QString axisenum)
{
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, true);
    }
    //LSM->WriteAxisRunVel(axisenum, vel);
}

//����㶯
void qg_JogButton::SwitchAxisNStart(QString axisenum)
{
    //�㶯����λ
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        LSM->setSpeed(axisenum, vel);
        LSM->AxisVmove(axisenum, false);
    }
    //LSM->WriteAxisRunVel(axisenum, -vel);
}

//************************************ Y1�㶯 ********************************
//Y1�ᷴ��㶯
void qg_JogButton::on_pushButton_Y1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y1);
}

//Y1������㶯
void qg_JogButton::on_pushButton_Y1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y1);
}

//Y1ֹͣ�㶯
void qg_JogButton::on_pushButton_Y1Up_released()
{
    LSM->AxisStop(LS_AxisName::Y1);
}

//Y1ֹͣ�㶯
void qg_JogButton::on_pushButton_Y1Down_released()
{
    LSM->AxisStop(LS_AxisName::Y1);
}

//************************************ X�㶯 ********************************
//X�ᷴ��㶯
void qg_JogButton::on_pushButton_XLeft_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//X������㶯
void qg_JogButton::on_pushButton_XRight_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//Xֹͣ�㶯
void qg_JogButton::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
}

//Xֹͣ�㶯
void qg_JogButton::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
}

//************************************ Y2�㶯 ********************************
//Y2�ᷴ��㶯
void qg_JogButton::on_pushButton_Y2Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y2);
}

//Y2������㶯
void qg_JogButton::on_pushButton_Y2Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y2);
}

//Y2ֹͣ�㶯
void qg_JogButton::on_pushButton_Y2Up_released()
{
    LSM->AxisStop(LS_AxisName::Y2);
}

//Y2ֹͣ�㶯
void qg_JogButton::on_pushButton_Y2Down_released()
{
    LSM->AxisStop(LS_AxisName::Y2);
}

//************************************ Z�㶯 ********************************
//Z�ᷴ��㶯
void qg_JogButton::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z������㶯
void qg_JogButton::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Zֹͣ�㶯
void qg_JogButton::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
}

//Zֹͣ�㶯
void qg_JogButton::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
}

//************************************ RZ�㶯 ********************************
//RZ�ᷴ��㶯
void qg_JogButton::on_pushButton_RZLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::RZ);
}

//RZ������㶯
void qg_JogButton::on_pushButton_RZRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::RZ);
}

//RZֹͣ�㶯
void qg_JogButton::on_pushButton_RZLeft_released()
{
    LSM->AxisStop(LS_AxisName::RZ);
}

//RZֹͣ�㶯
void qg_JogButton::on_pushButton_RZRight_released()
{
    LSM->AxisStop(LS_AxisName::RZ);
}

//************************************ U1�㶯 ********************************
//U1�ᷴ��㶯
void qg_JogButton::on_pushButton_U1Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1������㶯
void qg_JogButton::on_pushButton_U1Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1ֹͣ�㶯
void qg_JogButton::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
}

//U1ֹͣ�㶯
void qg_JogButton::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
}

//************************************ U2�㶯 ********************************
//U2�ᷴ��㶯
void qg_JogButton::on_pushButton_U2Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U2);
}

//U2������㶯
void qg_JogButton::on_pushButton_U2Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U2);
}

//U2ֹͣ�㶯
void qg_JogButton::on_pushButton_U2Left_released()
{
    LSM->AxisStop(LS_AxisName::U2);
}

//U2ֹͣ�㶯
void qg_JogButton::on_pushButton_U2Right_released()
{
    LSM->AxisStop(LS_AxisName::U2);
}

void qg_JogButton::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
}