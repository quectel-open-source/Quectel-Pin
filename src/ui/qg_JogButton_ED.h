#pragma once

#include <QWidget>
#include "ui_qg_JogButton_ED.h"

class qg_JogButton_ED : public QWidget
{
	Q_OBJECT

public:
    qg_JogButton_ED(QWidget *parent = nullptr);
	~qg_JogButton_ED();

	//�������˶�
	void SwitchAxisPStart(QString axisenum);
	//�ᷴ���˶�
	void SwitchAxisNStart(QString axisenum);

private:
	Ui::qg_JogButton_EDClass ui;

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
public slots:
    void setEnabled(bool enabled);
};
