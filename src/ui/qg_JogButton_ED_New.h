#pragma once

#include <QWidget>
#include "ui_qg_JogButton_ED_New.h"

class qg_JogButton_ED_New : public QWidget
{
    Q_OBJECT
public:
    QTimer* m_plimitTimer = nullptr;
    QTimer* m_nlimitTimer = nullptr;
    QString m_runningAxis = "";
public:
    qg_JogButton_ED_New(QWidget* parent = nullptr);
    ~qg_JogButton_ED_New();

    //轴正向运动
    void SwitchAxisPStart(QString axisenum);
    //轴反向运动
    void SwitchAxisNStart(QString axisenum);

private:
    Ui::qg_JogButton_ED_NewClass ui;

private slots:
    void on_pushButton_YUp_pressed();
    void on_pushButton_YDown_pressed();
    void on_pushButton_YUp_released();
    void on_pushButton_YDown_released();

    void on_pushButton_XLeft_pressed();
    void on_pushButton_XRight_pressed();
    void on_pushButton_XLeft_released();
    void on_pushButton_XRight_released();

    void on_pushButton_Z1Up_pressed();
    void on_pushButton_Z1Down_pressed();
    void on_pushButton_Z1Up_released();
    void on_pushButton_Z1Down_released();

    void on_pushButton_Z2Up_pressed();
    void on_pushButton_Z2Down_pressed();
    void on_pushButton_Z2Up_released();
    void on_pushButton_Z2Down_released();

    void on_pushButton_ULeft_pressed();
    void on_pushButton_URight_pressed();
    void on_pushButton_ULeft_released();
    void on_pushButton_URight_released();

    void on_pushButton_ZUp_pressed();
    void on_pushButton_ZDown_pressed();
    void on_pushButton_ZUp_released();
    void on_pushButton_ZDown_released();

    void on_pushButton_U1Left_pressed();
    void on_pushButton_U1Right_pressed();
    void on_pushButton_U1Left_released();
    void on_pushButton_U1Right_released();
public slots:
    void setEnabled(bool enabled);
    //软限位监控
    void checkPLimit();
    void checkNLimit();
};