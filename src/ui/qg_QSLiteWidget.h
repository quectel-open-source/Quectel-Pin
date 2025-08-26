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

	//刷新表格
	void refreshTableList();

public:

	// 取得数据集窗口
	static qg_QSLiteWidget* getSqlWindow() {
		return sqlWindow;
	}
	// 外部获取
	QSqlTableModel* getSQL() {
		return m_model;
	};
	//加入2D识别后的结果
	void addNew2DResult(ResultParamsStruct_Pin result, SQL_Parameter para);
	//加入3D识别后的结果
	void addNew3DResult(ResultParamsStruct_Pin3DMeasure result, SQL_Parameter para);
	//时间查询控件初始化
	void initDateTimeEdits();
	// 条件筛选
	QString siftTabel();

	// 产量结果输出
	void addProductResult(Run_Result runResult);
	void updateTimeSlot(const QString& table, const QString& timeSlot, Run_Result result);
	// 初始化产量统计
	void initProductTable();
	// 产品结果输出
	void addOneProductResult(Run_Result res, QString productName,QString filePath);
	//清空images文件夹
	void clearImageFolder();
	//清空指定文件夹
	void clearSelectImageFolder(QString path);


private slots:
    void on_clearButton_clicked();
    void onDeleteButtonClicked();
    void refreshTableView();
	void onTableSelected(int index);    
	void on_pushButton_find_clicked();
	void on_pushButton_ExportCSV_clicked();
	void onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous);
	//打开图片
	void on_pushButton_openPhoto_clicked();
	//删除
	void on_pushButton_DeleteDate_clicked();
	//清空
	void on_pushButton_ClearDate_clicked();
	//复判OK
	void on_pushButton_checkOK_clicked();
	//复判NG
	void on_pushButton_checkNG_clicked();
	//NG皮带运动
	void on_pushButton_ngBeltMove_pressed();
	void on_pushButton_ngBeltMove_released();
	//定时器检测停止NG皮带
	void stopNGBelt();
	//导出当日数据
	void on_pushButton_ExportData_clicked();
	//清除所有已保存原图
	void on_pushButton_ClearOriginal_clicked();
	//双击打开图片
	void onTableViewDoubleClicked();
signals:
	void showNG_Signal(double value);//传输当天NG率

private:
	Ui::qg_QSLiteWidgetClass* ui;
	QSqlTableModel* m_model;
	DatabaseManager m_dbManager;
	// 应用程序主窗口指针
	static qg_QSLiteWidget* sqlWindow;

	QSqlTableModel* m_ProductModel;
	ProductDatabaseManager m_dbProductManager;
	QTimer m_stopNGTimer; // 停止NG皮带定时器
public:
	QString m_currentTable;
	QString m_currentProductTable;
};
