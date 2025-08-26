#pragma once

#include <QWidget>
#include "ui_qg_JogButton.h"

class qg_JogButton : public QWidget
{
	Q_OBJECT

public:
	qg_JogButton(QWidget *parent = nullptr);
	~qg_JogButton();

    //轴正向运动
    void SwitchAxisPStart(QString axisenum);
    //轴反向运动
    void SwitchAxisNStart(QString axisenum);

private:
	Ui::qg_JogButtonClass ui;

private slots:
    void on_pushButton_Y1Up_pressed();
    void on_pushButton_Y1Down_pressed();
    void on_pushButton_Y1Up_released();
    void on_pushButton_Y1Down_released();

    void on_pushButton_XLeft_pressed();
    void on_pushButton_XRight_pressed();
    void on_pushButton_XLeft_released();
    void on_pushButton_XRight_released();

    void on_pushButton_Y2Up_pressed();
    void on_pushButton_Y2Down_pressed();
    void on_pushButton_Y2Up_released();
    void on_pushButton_Y2Down_released();

    void on_pushButton_ZUp_pressed();
    void on_pushButton_ZDown_pressed();
    void on_pushButton_ZUp_released();
    void on_pushButton_ZDown_released();

    void on_pushButton_RZLeft_pressed();
    void on_pushButton_RZRight_pressed();
    void on_pushButton_RZLeft_released();
    void on_pushButton_RZRight_released();

    void on_pushButton_U1Left_pressed();
    void on_pushButton_U1Right_pressed();
    void on_pushButton_U1Left_released();
    void on_pushButton_U1Right_released();

    void on_pushButton_U2Left_pressed();
    void on_pushButton_U2Right_pressed();
    void on_pushButton_U2Left_released();
    void on_pushButton_U2Right_released();

public slots:
    void setEnabled(bool enabled);
};
