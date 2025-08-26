#pragma once

#include <QDialog>
#include "ui_qg_Formula.h"
#include "rs.h"
#include "rs_motion.h"

class qg_Formula : public QDialog
{
	Q_OBJECT

public:
	qg_Formula(QList<PointItemConfig> allconfig, ENUMSTATION station, int editRow = -1,QDialog *parent = nullptr);
	qg_Formula(QList<PointItemConfig> allconfig, PointItemConfig config, int editRow = -1, QDialog* parent = nullptr);
	~qg_Formula();
	void initConnect();
	//2Dʶ��
	void initRadioButton_2D();
	//3Dɨ��
	void initRadioButton_3D();
	//3Dʶ��
	void initRadioButton_3Didentify();
	//ȡ����
	void initRadioButton_Feeding();
	//�汾����
	void initVersion();
	//����
	void initRadioButton_TakePhoto();
	//�Ӿ�ģ��
	void initRadioButton_Visual_Identity();
	//Բ���
	void initRadioButton_CircleMeasure();
	void setEditMode(bool tp)
	{
		m_editMode = tp;
	};

private slots:
	//void onradioButton_2DToggled(bool checked);
	//void onradioButton_3DToggled(bool checked);
	//void on_radioButton_NoneToggled(bool checked);
	void on_pushButton_OK_clicked();
	//void on_radioButton_3DidentifyToggled(bool checked);
	//void on_radioButton_FeedingToggled(bool checked);
	//��ͬģʽ�л�
	void handleModeChange(int id, bool isChecked);
	void on_comboBox_Visual_Identity_currentIndexChanged(int);
	
signals:
	void signalFormula(const PointItemConfig& config);
	void signalEditFormula(const int& row,const PointItemConfig& config);

private:
	Ui::qg_FormulaClass ui;
	ENUMSTATION m_station = ENUMSTATION::L;
	int m_editRow = -1;
	QList<PointItemConfig> m_allConfig;
	PointItemConfig m_config;
	QButtonGroup* modeButtonGroup;
	//�༭ģʽ
	bool m_editMode = false;

};
