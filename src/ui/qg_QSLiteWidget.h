#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include "ui_qg_QSLiteWidget.h"
#include <QSqlTableModel>
#include "DatabaseManager.h"
#include "datas.h"
#include "ProductDatabaseManager.h"
#include "rs.h"
#include <QTimer>

class qg_QSLiteWidget : public QWidget
{
	Q_OBJECT

public:
	qg_QSLiteWidget(QWidget *parent = nullptr);
	~qg_QSLiteWidget();

	//ˢ�±��
	void refreshTableList();

public:

	// ȡ�����ݼ�����
	static qg_QSLiteWidget* getSqlWindow() {
		return sqlWindow;
	}
	// �ⲿ��ȡ
	QSqlTableModel* getSQL() {
		return m_model;
	};
	//����2Dʶ���Ľ��
	void addNew2DResult(ResultParamsStruct_Pin result, SQL_Parameter para);
	//����3Dʶ���Ľ��
	void addNew3DResult(ResultParamsStruct_Pin3DMeasure result, SQL_Parameter para);
	//ʱ���ѯ�ؼ���ʼ��
	void initDateTimeEdits();
	// ����ɸѡ
	QString siftTabel();

	// ����������
	void addProductResult(Run_Result runResult);
	void updateTimeSlot(const QString& table, const QString& timeSlot, Run_Result result);
	// ��ʼ������ͳ��
	void initProductTable();
	// ��Ʒ������
	void addOneProductResult(Run_Result res, QString productName,QString filePath);
	//���images�ļ���
	void clearImageFolder();
	//���ָ���ļ���
	void clearSelectImageFolder(QString path);


private slots:
    void on_clearButton_clicked();
    void onDeleteButtonClicked();
    void refreshTableView();
	void onTableSelected(int index);    
	void on_pushButton_find_clicked();
	void on_pushButton_ExportCSV_clicked();
	void onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous);
	//��ͼƬ
	void on_pushButton_openPhoto_clicked();
	//ɾ��
	void on_pushButton_DeleteDate_clicked();
	//���
	void on_pushButton_ClearDate_clicked();
	//����OK
	void on_pushButton_checkOK_clicked();
	//����NG
	void on_pushButton_checkNG_clicked();
	//NGƤ���˶�
	void on_pushButton_ngBeltMove_pressed();
	void on_pushButton_ngBeltMove_released();
	//��ʱ�����ֹͣNGƤ��
	void stopNGBelt();
	//������������
	void on_pushButton_ExportData_clicked();
	//��������ѱ���ԭͼ
	void on_pushButton_ClearOriginal_clicked();
	//˫����ͼƬ
	void onTableViewDoubleClicked();
signals:
	void showNG_Signal(double value);//���䵱��NG��

private:
	Ui::qg_QSLiteWidgetClass* ui;
	QSqlTableModel* m_model;
	DatabaseManager m_dbManager;
	// Ӧ�ó���������ָ��
	static qg_QSLiteWidget* sqlWindow;

	QSqlTableModel* m_ProductModel;
	ProductDatabaseManager m_dbProductManager;
	QTimer m_stopNGTimer; // ֹͣNGƤ����ʱ��
public:
	QString m_currentTable;
	QString m_currentProductTable;
};
