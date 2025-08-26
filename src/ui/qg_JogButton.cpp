#include "qg_JogButton.h"
#include "rs_motion.h"

qg_JogButton::qg_JogButton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    //250717 把左工位U轴放回去
    //ui.pushButton_U1Left->hide();
    //ui.pushButton_U1Right->hide();
    ui.pushButton_U2Left->hide();
    ui.pushButton_U2Right->hide();

}

qg_JogButton::~qg_JogButton()
{}

//正向点动
void qg_JogButton::SwitchAxisPStart(QString axisenum)
{
    //点动软限位
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

//反向点动
void qg_JogButton::SwitchAxisNStart(QString axisenum)
{
    //点动软限位
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

//************************************ Y1点动 ********************************
//Y1轴反向点动
void qg_JogButton::on_pushButton_Y1Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y1);
}

//Y1轴正向点动
void qg_JogButton::on_pushButton_Y1Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y1);
}

//Y1停止点动
void qg_JogButton::on_pushButton_Y1Up_released()
{
    LSM->AxisStop(LS_AxisName::Y1);
}

//Y1停止点动
void qg_JogButton::on_pushButton_Y1Down_released()
{
    LSM->AxisStop(LS_AxisName::Y1);
}

//************************************ X点动 ********************************
//X轴反向点动
void qg_JogButton::on_pushButton_XLeft_pressed()
{
    SwitchAxisPStart(LS_AxisName::X);
}

//X轴正向点动
void qg_JogButton::on_pushButton_XRight_pressed()
{
    SwitchAxisNStart(LS_AxisName::X);
}

//X停止点动
void qg_JogButton::on_pushButton_XLeft_released()
{
    LSM->AxisStop(LS_AxisName::X);
}

//X停止点动
void qg_JogButton::on_pushButton_XRight_released()
{
    LSM->AxisStop(LS_AxisName::X);
}

//************************************ Y2点动 ********************************
//Y2轴反向点动
void qg_JogButton::on_pushButton_Y2Up_pressed()
{
    SwitchAxisNStart(LS_AxisName::Y2);
}

//Y2轴正向点动
void qg_JogButton::on_pushButton_Y2Down_pressed()
{
    SwitchAxisPStart(LS_AxisName::Y2);
}

//Y2停止点动
void qg_JogButton::on_pushButton_Y2Up_released()
{
    LSM->AxisStop(LS_AxisName::Y2);
}

//Y2停止点动
void qg_JogButton::on_pushButton_Y2Down_released()
{
    LSM->AxisStop(LS_AxisName::Y2);
}

//************************************ Z点动 ********************************
//Z轴反向点动
void qg_JogButton::on_pushButton_ZUp_pressed()
{
    SwitchAxisNStart(LS_AxisName::Z);
}

//Z轴正向点动
void qg_JogButton::on_pushButton_ZDown_pressed()
{
    SwitchAxisPStart(LS_AxisName::Z);
}

//Z停止点动
void qg_JogButton::on_pushButton_ZUp_released()
{
    LSM->AxisStop(LS_AxisName::Z);
}

//Z停止点动
void qg_JogButton::on_pushButton_ZDown_released()
{
    LSM->AxisStop(LS_AxisName::Z);
}

//************************************ RZ点动 ********************************
//RZ轴反向点动
void qg_JogButton::on_pushButton_RZLeft_pressed()
{
    SwitchAxisNStart(LS_AxisName::RZ);
}

//RZ轴正向点动
void qg_JogButton::on_pushButton_RZRight_pressed()
{
    SwitchAxisPStart(LS_AxisName::RZ);
}

//RZ停止点动
void qg_JogButton::on_pushButton_RZLeft_released()
{
    LSM->AxisStop(LS_AxisName::RZ);
}

//RZ停止点动
void qg_JogButton::on_pushButton_RZRight_released()
{
    LSM->AxisStop(LS_AxisName::RZ);
}

//************************************ U1点动 ********************************
//U1轴反向点动
void qg_JogButton::on_pushButton_U1Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U1);
}

//U1轴正向点动
void qg_JogButton::on_pushButton_U1Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U1);
}

//U1停止点动
void qg_JogButton::on_pushButton_U1Left_released()
{
    LSM->AxisStop(LS_AxisName::U1);
}

//U1停止点动
void qg_JogButton::on_pushButton_U1Right_released()
{
    LSM->AxisStop(LS_AxisName::U1);
}

//************************************ U2点动 ********************************
//U2轴反向点动
void qg_JogButton::on_pushButton_U2Left_pressed()
{
    SwitchAxisPStart(LS_AxisName::U2);
}

//U2轴正向点动
void qg_JogButton::on_pushButton_U2Right_pressed()
{
    SwitchAxisNStart(LS_AxisName::U2);
}

//U2停止点动
void qg_JogButton::on_pushButton_U2Left_released()
{
    LSM->AxisStop(LS_AxisName::U2);
}

//U2停止点动
void qg_JogButton::on_pushButton_U2Right_released()
{
    LSM->AxisStop(LS_AxisName::U2);
}

void qg_JogButton::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
}