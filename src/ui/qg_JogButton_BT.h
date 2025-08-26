#pragma once

#include <QWidget>
#include "ui_qg_JogButton_BT.h"

class qg_JogButton_BT : public QWidget
{
	Q_OBJECT

public:
    qg_JogButton_BT(QWidget *parent = nullptr);
	~qg_JogButton_BT();

    //轴正向运动
    void SwitchAxisPStart(QString axisenum);
    //轴反向运动
    void SwitchAxisNStart(QString axisenum);

private:
	Ui::qg_JogButton_BTClass ui;

private slots:
    void on_pushButton_Y1front_pressed();
    void on_pushButton_Y1back_pressed();
    void on_pushButton_Y1front_released();
    void on_pushButton_Y1back_released();

    void on_pushButton_Y2front_pressed();
    void on_pushButton_Y2back_pressed();
    void on_pushButton_Y2front_released();
    void on_pushButton_Y2back_released();

    void on_pushButton_XLeft_pressed();
    void on_pushButton_XRight_pressed();
    void on_pushButton_XLeft_released();
    void on_pushButton_XRight_released();
};

