#include "qg_JogButton_BT.h"
#include "rs_motion.h"

qg_JogButton_BT::qg_JogButton_BT(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

qg_JogButton_BT::~qg_JogButton_BT()
{}

//正向点动
void qg_JogButton_BT::SwitchAxisPStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //点动软限位
    if (LSM->m_Axis[axisenum].position >= LSM->m_Axis[axisenum].maxTravel)
        return;
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {

        double vel = ui.lineEdit_JogSpeed->text().toDouble();
        // 设置速度参数并定速运行
        LSM->AxisVmove(axisenum, true, vel);

    }
}

//反向点动
void qg_JogButton_BT::SwitchAxisNStart(QString axisenum)
{
    if (LSM->m_isStart || LSM->m_isHomming)
        return;
    //点动软限位
    if (LSM->m_Axis[axisenum].position <= LSM->m_Axis[axisenum].minTravel)
        return;
    double vel = ui.lineEdit_JogSpeed->text().toDouble();
    if (!LSM->IsRuning(LSM->m_Axis[axisenum].card, LSM->m_Axis[axisenum].axisNum))
    {
        // 设置速度参数并定速运行
        LSM->AxisVmove(axisenum, false, vel);
    }

}

//************************************ Y1点动 ********************************
//Y1轴反向点动
void qg_JogButton_BT::on_pushButton_Y1front_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisNStart(LS_AxisName::Y1);
        break;
    default:
        break;
    }
}

//Y1轴正向点动
void qg_JogButton_BT::on_pushButton_Y1back_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisPStart(LS_AxisName::Y1);
        break;
    default:
        break;
    }

}

//Y1停止点动
void qg_JogButton_BT::on_pushButton_Y1front_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::Y1);
        break;
    default:
        break;
    }

}

//Y1停止点动
void qg_JogButton_BT::on_pushButton_Y1back_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::Y1);
        break;
    default:
        break;
    }
}

//************************************ Y2点动 ********************************
//Y2轴反向点动
void qg_JogButton_BT::on_pushButton_Y2front_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisNStart(LS_AxisName::Y2);
        break;
    default:
        break;
    }
}

//Y2轴正向点动
void qg_JogButton_BT::on_pushButton_Y2back_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisPStart(LS_AxisName::Y2);
        break;
    default:
        break;
    }

}

//Y2停止点动
void qg_JogButton_BT::on_pushButton_Y2front_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::Y2);
        break;
    default:
        break;
    }

}

//Y2停止点动
void qg_JogButton_BT::on_pushButton_Y2back_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::Y2);
        break;
    default:
        break;
    }
}

//************************************ X点动 ********************************
//X轴反向点动
void qg_JogButton_BT::on_pushButton_XLeft_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisPStart(LS_AxisName::X);
        break;
    default:
        break;
    }
}

//X轴正向点动
void qg_JogButton_BT::on_pushButton_XRight_pressed()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        SwitchAxisNStart(LS_AxisName::X);
        break;
    default:
        break;
    }
}

//X停止点动
void qg_JogButton_BT::on_pushButton_XLeft_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    default:
        break;
    }

}

//X停止点动
void qg_JogButton_BT::on_pushButton_XRight_released()
{
    switch (LSM->m_version)
    {
    case ENUM_VERSION::BtPin:
        LSM->AxisStop(LS_AxisName::X);
        break;
    default:
        break;
    }
}