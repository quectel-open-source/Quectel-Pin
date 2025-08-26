#include "qg_QSLiteWidget.h"
#include <QMessageBox>
#include <QtWidgets/qcombobox.h>
#include <QSqlQuery>
#include <unordered_set>
#include "qc_log.h"
#include <QSqlError>
#include <qdebug.h>
#include <QtWidgets/qfiledialog.h>
#include <QSqlRecord>
#include "ImageTypeConvert.h"
#include "QGraphicsViews.h"
#include "qg_showResultPicDlg.h"
#include "rs_motion.h"

qg_QSLiteWidget* qg_QSLiteWidget::sqlWindow = nullptr;
qg_QSLiteWidget::qg_QSLiteWidget(QWidget* parent)
    : ui(new Ui::qg_QSLiteWidgetClass) {
    ui->setupUi(this);
    sqlWindow = this;
    // 初始化数据库
    if (!m_dbManager.initializeDatabase()) {
        QMessageBox::critical(this, "Error", "无法初始化数据库!");
        return;
    }

    if (LSM->m_version != EdPin)
    {
        ui->pushButton_ngBeltMove->hide();
    }

    //初始化产量表
    initProductTable();
    initDateTimeEdits();

    // 取消表头显示
    //ui->tableView_Sql->verticalHeader()->setVisible(false);
    // 设置整行选中模式
    ui->tableView_Sql->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_Sql->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 刷新表格列表
    refreshTableList();

    ui->tableView_Product->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_Product->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 连接信号
    connect(ui->comboBoxTables, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &qg_QSLiteWidget::onTableSelected);
    connect(ui->deleteButton, &QPushButton::clicked, this, &qg_QSLiteWidget::onDeleteButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &qg_QSLiteWidget::refreshTableView);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateChanged, [=](const QDate& date) {
        // 如果开始日期晚于结束日期，自动调整结束日期
        if (date > ui->endDateTimeEdit->date()) {
            ui->endDateTimeEdit->setDate(date);
        }
        });

    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateChanged, [=](const QDate& date) {
        // 如果结束日期早于开始日期，自动调整开始日期
        if (date < ui->startDateTimeEdit->date()) {
            ui->startDateTimeEdit->setDate(date);
        }
        });

    //显示第一个表格
    onTableSelected(0);

    connect(&m_stopNGTimer, &QTimer::timeout, this, &qg_QSLiteWidget::stopNGBelt);
    connect(ui->tableView_Sql, &QTableView::doubleClicked,this, &qg_QSLiteWidget::onTableViewDoubleClicked);
}

qg_QSLiteWidget::~qg_QSLiteWidget()
{
    if (m_model) {
        m_model->deleteLater();
        m_model = nullptr;
    }

    if (m_ProductModel) 
        m_ProductModel->deleteLater();

    if(ui)
        delete ui;

}

void qg_QSLiteWidget::onTableSelected(int index) {
    QString tableName = ui->comboBoxTables->itemText(index);
    if (tableName.isEmpty()) return;

    if (tableName == "产品数据")
    {
        ui->groupBox_CheckNG->setEnabled(true);
    }
    else
    {
        //展示复判功能
        ui->groupBox_CheckNG->setEnabled(false);
    }

    // 释放旧模型
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    // 创建新模型
    m_model = new QSqlTableModel(this);
    m_model->setTable(tableName);
    // ==== 新增排序代码 =====
   // 根据表名设置不同的排序字段
    int sortColumn = -1;

    if (tableName == "产品数据") {
        sortColumn = m_model->fieldIndex("创建时间");
    }
    else if (tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
        sortColumn = m_model->fieldIndex("创建时间");  // 或使用"更新时间"
    }
    else {
        sortColumn = 0; // 通用表用首列（通常是ID）
    }

    if (sortColumn >= 0) {
        m_model->setSort(sortColumn, Qt::DescendingOrder);
    }
    // ==== 结束排序代码 =====
    m_model->select();
    ui->tableView_Sql->setModel(m_model);
    m_currentTable = tableName;

    // 调整列宽
    ui->tableView_Sql->resizeColumnsToContents();
    ui->tableView_Sql->hideColumn(0); // 隐藏ID列
#ifndef DEBUG_LHC
    //不可选择编辑
    ui->tableView_Sql->setEditTriggers(QAbstractItemView::NoEditTriggers);
#endif // !DEBUG_LHC

}

//清空
void qg_QSLiteWidget::on_clearButton_clicked() {
    //QSqlQuery query;
    //query.prepare("INSERT INTO employees (name, age, department) VALUES (?, ?, ?)");
    //query.addBindValue(ui->nameLineEdit->text());
    //query.addBindValue(ui->ageSpinBox->value());
    //query.addBindValue(ui->departmentLineEdit->text());

    //if (!query.exec()) {
    //    QMessageBox::warning(this, "Error", "Failed to add record!");
    //}
    //refreshTableView();

    QString currentTable = ui->comboBoxTables->currentText();

    if (QMessageBox::question(this, "确认",
        QString("确定要清空表格 [%1] 的所有数据吗？").arg(currentTable))
        == QMessageBox::Yes)
    {
        //先把图片全部清掉
        if (currentTable == "产品数据")
        {
            clearImageFolder();
        }
        if (m_dbManager.clearTableData(currentTable)) {
            refreshTableView();
            QMessageBox::information(this, "成功", "数据已清空");
        }
    }
}

//删除某行
void qg_QSLiteWidget::onDeleteButtonClicked() {

    if (!m_model)
        return;
    QString tableName = ui->comboBoxTables->currentText();
    if (tableName == "产品数据")
    {
        QModelIndex index = ui->tableView_Sql->currentIndex();
        // 获取选中行
        int selectedRow = index.row();
        if (selectedRow >= 0)
        {
            // 获取"图片路径"列数据
            int imagePathCol = m_model->fieldIndex("图片路径");
            if (imagePathCol == -1) {
                machineLog->write("错误：未找到列 '图片路径'！", Normal);
            }
            QString imagePath = m_model->data(m_model->index(selectedRow, imagePathCol)).toString();
            if (!imagePath.isNull()) {
                clearSelectImageFolder(imagePath);
            }
        }
    }


    QModelIndex index = ui->tableView_Sql->currentIndex();
    if (index.isValid()) {
        m_model->removeRow(index.row());
        m_model->submitAll();
    }
}

//刷新表格
void qg_QSLiteWidget::refreshTableView() {
    //if (m_model) {
    //    // 先保存当前选择的表格
    //    QString tableName = m_currentTable;

    //    // 完全重新创建模型
    //    delete m_model;
    //    m_model = new QSqlTableModel(this);
    //    m_model->setTable(tableName);

    //    // 重新应用排序设置
    //    if (tableName == "产品数据") {
    //        m_model->setSort(m_model->fieldIndex("创建时间"), Qt::DescendingOrder);
    //    }
    //    else if (tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
    //        m_model->setSort(m_model->fieldIndex("创建时间"), Qt::DescendingOrder);
    //    }

    //    m_model->select();
    //    ui->tableView_Sql->setModel(m_model);

    //    // 通用表在视图级别排序
    //    if (!(tableName == "产品数据" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result")) {
    //        ui->tableView_Sql->sortByColumn(0, Qt::DescendingOrder);
    //    }
    //}
    //ui->tableView_Sql->resizeColumnsToContents();
    //refreshTableList(); // 刷新表格列表

    if (!m_model) return;

    try {
        // 保存当前选择（可选）
        QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
        // 设置排序
        const QString tableName = m_currentTable;
        //m_model->setTable("Pin2D_Result");
        //m_model->setTable(m_currentTable);
        const int dateColumn = m_model->fieldIndex("创建时间");

        if (tableName == "产品数据" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
            if (dateColumn != -1) { // 确保列存在
                m_model->setSort(dateColumn, Qt::DescendingOrder);
            }
        }

        //ui->tableView_Sql->setModel(m_model);

        //// 检查模型是否设置了表，若没有则设置
        //if (m_model->tableName().isEmpty()) {
        //    m_model->setTable("`" + m_currentTable + "`");
        //}

        // 刷新数据
        if (!m_model->select()) {
            machineLog->write("数据集刷新失败" + m_model->lastError().text(), Normal);
            throw std::runtime_error(m_model->lastError().text().toStdString());
        }
        //m_model->select();


        // 通用表排序
        if (!(tableName == "产品数据" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result")) {
            ui->tableView_Sql->sortByColumn(0, Qt::DescendingOrder);
        }

        // 恢复选择（可选）
        ui->tableView_Sql->setCurrentIndex(currentIndex);

        // 调整列宽
        ui->tableView_Sql->resizeColumnsToContents();
    }
    catch (const std::exception& e) {
        qCritical() << "刷新表格时出错：" << e.what();
        //QMessageBox::critical(this, "错误", QString("刷新表格失败：%1").arg(e.what()));
    }
}

//刷新表格列表
void qg_QSLiteWidget::refreshTableList() {
    ui->comboBoxTables->clear();
    ui->comboBoxTables->addItems(m_dbManager.getUserTableNames());
}

//加入2D识别后的结果
void qg_QSLiteWidget::addNew2DResult(ResultParamsStruct_Pin result, SQL_Parameter para)
{
    try
    {
        bool hasTab = false;
        QStringList allMes = m_dbManager.getUserTableNames();
        for (auto tab : allMes)
        {
            if (tab == "Pin2D_Result")
            {
                hasTab = true;
                break;
            }
        }
        if (!hasTab)
        {
            machineLog->write("没有找到2D数据集，添加结果失败", Normal);
            return;
        }

        // 记录NG索引
        unordered_set<int> ngIdxSet(result.ngIdx.begin(), result.ngIdx.end());
        // 获取当前时间
        //QDateTime currentTime = QDateTime::currentDateTime().toUTC();
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");
        // 遍历数据点
        for (int i = 0; i < result.hv_Row_Array.TupleLength().I(); ++i) {
            QSqlQuery query;
            query.prepare("INSERT INTO Pin2D_Result ("
                "name,"
                "`SN型号`,"
                "X,"
                "Y, "
                "`相对基准的X方向距离 mm`, "
                "`相对基准的Y方向距离 mm`,"
                "`相对基准的X方向偏差值 mm`,"
                "`相对基准的Y方向偏差值 mm`,"
                "`结果`,"
                "`创建时间`,"
                "`更新时间`"
                ")VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?)");


            // 结果
            //auto id = m_model->columnCount();
            //query.addBindValue(id);
            query.addBindValue(para.formulaName);
            //QString tpSN = "`" + para.snName + "`";
            query.addBindValue(para.snName);
            query.addBindValue(result.hv_Column_Array[i].D());
            query.addBindValue(result.hv_Row_Array[i].D());
            query.addBindValue(result.hv_DisX_MM_Array[i].D());
            query.addBindValue(result.hv_DisY_MM_Array[i].D());
            query.addBindValue(result.hv_OffsetX_MM_Array[i].D());
            query.addBindValue(result.hv_OffsetY_MM_Array[i].D());
            query.addBindValue(ngIdxSet.count(i) ? "NG" : "OK");
            query.addBindValue(timeStr); // create_time
            query.addBindValue(timeStr); // update_time

            if (!query.exec()) {
                //QMessageBox::warning(this, "Error", "Failed to add record!");
                machineLog->write("2D数据集添加失败", Normal);
                return;
            }
        }
        m_currentTable = "Pin2D_Result";
        refreshTableView();
    }
    catch (...)
    {
        machineLog->write("2D数据集获取数据失败", Normal);
        return;
    }
}

//时间查询控件初始化
void qg_QSLiteWidget::initDateTimeEdits()
{
    ui->startDateTimeEdit->setCalendarPopup(true);  // 日历弹出
    ui->endDateTimeEdit->setCalendarPopup(true);  // 日历弹出
    // 开始时间默认1天前
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    // 结束时间默认一天后
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    ui->endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    // 时间范围限制
    QDateTime minTime = QDateTime::currentDateTime().addYears(-1);
    QDateTime maxTime = QDateTime::currentDateTime().addYears(1);
    ui->startDateTimeEdit->setMinimumDateTime(minTime);
    ui->startDateTimeEdit->setMaximumDateTime(maxTime);
    ui->endDateTimeEdit->setMinimumDateTime(minTime);
    ui->endDateTimeEdit->setMaximumDateTime(maxTime);
}

// 查询按钮点击
void qg_QSLiteWidget::on_pushButton_find_clicked() {
    // 获取时间范围
    QDateTime start = ui->startDateTimeEdit->dateTime();
    QDateTime end = ui->endDateTimeEdit->dateTime();

    // 校验时间有效性
    if (start > end) {
        QMessageBox::warning(this, "提示", "结束时间不能早于开始时间");
        return;
    }

    // 生成过滤条件
    QString filter = siftTabel();

    // 设置模型过滤器
    //m_model->setTable("Pin2D_Result");
    m_model->setFilter(filter);
    m_model->select();
    // 检查模型错误
    if (!m_model->select()) {
        qDebug() << "查询错误:" << m_model->lastError().text();
    }

    // 显示数据
    ui->tableView_Sql->setModel(m_model);
    //ui->tableView_Sql->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 调整列宽
    ui->tableView_Sql->resizeColumnsToContents();

    // 优化显示
    ui->tableView_Sql->hideColumn(0); // 隐藏ID列
    ui->tableView_Sql->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableView_Sql->setSortingEnabled(true);
}

//添加3Dpin针结果
void qg_QSLiteWidget::addNew3DResult(ResultParamsStruct_Pin3DMeasure result, SQL_Parameter para) {
    try {

        //检查表是否存在
        QStringList tables = m_dbManager.getUserTableNames();
        if (!tables.contains("Pin3D_Result")) {
            machineLog->write("没有找到3D数据集，添加结果失败", Normal);
            return;
        }

        //获取当前时间
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");

        ////开启事务（可选）
        //m_dbManager.beginTransaction();

        //插入数据
        for (int i = 0; i < result.PinFlag.size(); ++i) {
            QSqlQuery query;
            query.prepare("INSERT INTO Pin3D_Result ("
                "name,"
                "`SN型号`, "
                "`高度`,"
                "`结果`,"
                "`创建时间`,"
                "`更新时间`"
                ") VALUES (?, ?, ?, ?, ?, ?)");

            query.addBindValue(para.formulaName);
            query.addBindValue(para.snName);
            query.addBindValue(result.PinHeight[i]);

            QString resultStr = (result.PinHeight[i] == -2) ? "NG" : (result.PinFlag[i] ? "OK" : "NG");
            query.addBindValue(resultStr);
            query.addBindValue(timeStr);
            query.addBindValue(timeStr);

            if (!query.exec()) {
                machineLog->write(QString("3D数据集添加失败: %1").arg(query.lastError().text()), Normal);
                //m_dbManager.rollbackTransaction(); // 回滚事务
                return;
            }
        }

        ////提交事务（可选）
        //m_dbManager.commitTransaction();
        m_currentTable = "Pin3D_Result";
        refreshTableView();
    }
    catch (...) {
        machineLog->write("3D数据集操作异常", Normal);
    }
}

//数据库内容CSV导出
void qg_QSLiteWidget::on_pushButton_ExportCSV_clicked()
{
    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        tr("Save File"),
        "",
        tr("CSV Files (*.csv)")
    );

    // 如果用户没有选择文件，或者点击了取消
    if (filePath.isEmpty()) {
        return;
    }

    // 确保文件扩展名是 .csv
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }


    QString tableName = ui->comboBoxTables->currentText();
    QString queryStr = "SELECT * FROM `" + tableName + "`";

    if (ui->radioButton_Export->isChecked())
    {
        // 导出筛选的表格信息
        // 获取当前的过滤条件
        QString filter = m_model->filter();

        // 如果有过滤条件，生成对应的查询语句
        if (!filter.isEmpty()) {
            queryStr += " WHERE " + filter;  // 添加筛选条件
        }
    }

    QSqlQuery query(queryStr);
    // 查询整个表的数据
    if (!query.exec()) {
        machineLog->write("查询失败: " + query.lastError().text(), Normal);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        machineLog->write("无法打开文件: " + file.errorString(), Normal);
        return;
    }

    QTextStream out(&file);

    // 写入表头（从1开始，跳过id）
    QSqlRecord record = query.record();
    for (int i = 1; i < record.count(); ++i) {
        out << record.fieldName(i);  // 获取字段名称
        if (i != record.count() - 1)
            out << ",";
    }
    out << "\n";

    // 写入数据
    while (query.next()) {
        for (int i = 1; i < record.count(); ++i) {
            out << query.value(i).toString();  // 获取字段值
            if (i != record.count() - 1)
                out << ",";
        }
        out << "\n";
    }

    file.close();
    qDebug() << "数据已成功导出为 CSV 文件";
}

// 条件筛选
QString qg_QSLiteWidget::siftTabel()
{
    // 获取时间范围
    QDateTime start = ui->startDateTimeEdit->dateTime();
    QDateTime end = ui->endDateTimeEdit->dateTime();
    // 生成过滤条件
    QString filter = QString("`创建时间` >= '%1' AND `创建时间` <= '%2'")
        .arg(start.toString("yyyy-MM-dd HH:mm:ss"))
        .arg(end.toString("yyyy-MM-dd HH:mm:ss"));

    // 是否设置了NG筛选
    if (ui->checkBox_checkNG->isChecked())
    {
        filter += " AND `结果` LIKE '%NG%'";
    }
    // 是否设置了OK筛选
    if (ui->checkBox_checkOK->isChecked())
    {
        filter += " AND `结果` LIKE '%OK%'";
    }
    //是否设置了按名字筛选
    QString name = ui->lineEdit_name->text();
    if (!name.isEmpty()) {
        // 使用 arg() 自动处理转义，避免 SQL 注入
        filter += QString(" AND name LIKE '%%1%'").arg(name);
    }
    //是否设置了按sn型号筛选
    QString nameSN = ui->lineEdit_sn->text();
    if (!nameSN.isEmpty()) {
        // 使用 arg() 自动处理转义，避免 SQL 注入
        filter += QString(" AND `SN型号` LIKE '%%1%'").arg(nameSN);
    }
    
    return filter;
}

//// 产量结果输出
//void qg_QSLiteWidget::addProductResult(Run_Result runResult)
//{
//     // 获取当前日期
//    QDate currentDate = QDate::currentDate();
//    QString dateTime = currentDate.toString("yyyy-MM-dd");
//    QString date = "`" + dateTime + "`";
//    if (!m_dbManager.tableExists(dateTime))
//    {
//        //没有这个表，创建
//        m_dbManager.addTable(date);
//        // 添加一行数据
//        int row = ui->tableWidget_Date->rowCount();
//        ui->tableWidget_Date->insertRow(row);
//        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(dateTime));
//    }
//
//    //计算数据
//    QDateTime currentTime = QDateTime::currentDateTime();
//    int currentHour = currentTime.toString("hh").toInt();  // 获取当前小时（24小时制）
//    QString timeSlot = m_dbManager.timeName[currentHour - 8];  // 当前时间段字符串
//
//    QSqlQuery query;
//    //读取
//    int totalQuantity =0;  // `总数量`
//    int ok = 0;             // `OK`
//    int recheckOk = 0;      // `复判 OK`
//    int ng = 0;      // `NG`
//    int ng2D = 0;           // `2D NG`
//    int ng3D = 0;           // `3D NG`
//    QString ngRate = "0.00%";      // `NG率`
//
//    query.prepare("SELECT `总数量`, `OK`, `复判 OK`, `NG`, `2D NG`, `3D NG`, `NG率` "
//        "FROM " + date + " WHERE `时间` = ?");
//    query.addBindValue(timeSlot);  // 绑定时间段条件
//
//    // 执行查询并检查是否成功
//    if (!query.exec()) {
//        qDebug() << "Error executing query: " << query.lastError().text();
//        return;
//    }
//    else {
//        qDebug() << "Query executed successfully.";
//
//        // 获取查询结果
//        if (query.next()) {  // 如果查询到数据
//            totalQuantity = query.value(0).toInt();  // `总数量`
//            ok = query.value(1).toInt();             // `OK`
//            recheckOk = query.value(2).toInt();      // `复判 OK`
//            ng2D = query.value(3).toInt();           // `NG`
//            ng2D = query.value(4).toInt();           // `2D NG`
//            ng3D = query.value(5).toInt();           // `3D NG`
//            //QString ngRate = query.value(5).toString();  // `NG率`
//
//            // 计算刷新数据
//            totalQuantity += 1;
//            if (runResult.isNG_2D)
//                ng2D += 1;
//            if (runResult.isNG_3D)
//                ng3D += 1;
//            if (!runResult.isNG)
//                ok += 1;
//            else
//                ng += 1;
//            if (totalQuantity != 0)
//            {
//                //计算NG率
//                double tempRate = ng / double(totalQuantity) * 100;
//                ngRate = QString::number(tempRate) + "%";
//                emit showNG_Signal(tempRate);
//            }
//        }
//        else {
//            qDebug() << "No data found for the time slot:" << timeSlot;
//            return;
//        }
//    }
//
//
//    //写入
//    // 假设要更新的数据如下,测试：
//    // 使用 UPDATE 语句更新数据
//    query.prepare("UPDATE " + date + " SET "
//        "`总数量` = ?, `OK` = ?,`NG` = ?, `2D NG` = ?, `3D NG` = ?, `NG率` = ? "
//        "WHERE `时间` = ?");  // 使用 `WHERE` 来指定特定的时间段
//
//    // 绑定参数
//    query.addBindValue(totalQuantity);        // `总数量`
//    query.addBindValue(ok);         // `OK`
//    query.addBindValue(ng);         // `NG`
//    query.addBindValue(ng2D);         // `2D NG`
//    query.addBindValue(ng3D);         // `3D NG`
//    query.addBindValue(ngRate);  // `NG率`
//    query.addBindValue(timeSlot);  // `时间` 字段的条件，用来找到特定时间段对应的行
//
//    // 执行查询并检查是否成功
//    if (!query.exec()) {
//        qDebug() << "Error updating database: " << query.lastError().text();
//    }
//    else {
//        qDebug() << "Database updated successfully.";
//    }
//
//    //----------------------------------------------- 全天数据添加
//    QSqlQuery queryAllDay;
//    queryAllDay.prepare("SELECT `总数量`, `OK`, `复判 OK`, `NG`, `2D NG`, `3D NG`, `NG率` "
//        "FROM " + date + " WHERE `时间` = ?");
//    queryAllDay.addBindValue("全天 | 8:00 - 8:00");
//    // 执行查询并检查是否成功
//    if (!queryAllDay.exec()) {
//        qDebug() << "allday Error executing query: " << queryAllDay.lastError().text();
//        return;
//    }
//    else {
//        qDebug() << "allday Query executed successfully.";
//
//        // 获取查询结果
//        if (queryAllDay.next()) {  // 如果查询到数据
//            totalQuantity = queryAllDay.value(0).toInt();  // `总数量`
//            ok = queryAllDay.value(1).toInt();             // `OK`
//            recheckOk = queryAllDay.value(2).toInt();      // `复判 OK`
//            ng2D = queryAllDay.value(3).toInt();           // `NG`
//            ng2D = queryAllDay.value(4).toInt();           // `2D NG`
//            ng3D = queryAllDay.value(5).toInt();           // `3D NG`
//            //QString ngRate = queryAllDay.value(5).toString();  // `NG率`
//
//            // 计算刷新数据
//            totalQuantity += 1;
//            if (runResult.isNG_2D)
//                ng2D += 1;
//            if (runResult.isNG_3D)
//                ng3D += 1;
//            if (!runResult.isNG)
//                ok += 1;
//            else
//                ng += 1;
//            if (totalQuantity != 0)
//            {
//                //计算NG率
//                double tempRate = ng / double(totalQuantity) * 100;
//                ngRate = QString::number(tempRate) + "%";
//                emit showNG_Signal(tempRate);
//            }
//        }
//        else {
//            qDebug() << "allday No data found for the time slot:" << timeSlot;
//            return;
//        }
//    }
//
//
//    //写入
//    // 假设要更新的数据如下,测试：
//    // 使用 UPDATE 语句更新数据
//    queryAllDay.prepare("UPDATE " + date + " SET "
//        "`总数量` = ?, `OK` = ?,`NG` = ?, `2D NG` = ?, `3D NG` = ?, `NG率` = ? "
//        "WHERE `时间` = ?");  // 使用 `WHERE` 来指定特定的时间段
//
//    // 绑定参数
//    queryAllDay.addBindValue(totalQuantity);        // `总数量`
//    queryAllDay.addBindValue(ok);         // `OK`
//    queryAllDay.addBindValue(ng);         // `NG`
//    queryAllDay.addBindValue(ng2D);         // `2D NG`
//    queryAllDay.addBindValue(ng3D);         // `3D NG`
//    queryAllDay.addBindValue(ngRate);  // `NG率`
//    queryAllDay.addBindValue(timeSlot);  // `时间` 字段的条件，用来找到特定时间段对应的行
//
//    // 执行查询并检查是否成功
//    if (!queryAllDay.exec()) {
//        qDebug() << "allday Error updating database: " << queryAllDay.lastError().text();
//    }
//    else {
//        qDebug() << "allday Database updated successfully.";
//    }
//
//    // 释放旧模型
//    if (m_ProductModel) {
//        delete m_ProductModel;
//        m_ProductModel = nullptr;
//    }
//
//    // 创建新模型
//    m_ProductModel = new QSqlTableModel(this);
//    m_ProductModel->setTable(dateTime);
//    m_ProductModel->select();
//    ui->tableView_Product->setModel(m_ProductModel);
//    m_currentProductTable = dateTime;
//
//    // 调整列宽
//    ui->tableView_Product->resizeColumnsToContents();
//    ui->tableView_Product->hideColumn(0); // 隐藏ID列
//    ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//#ifndef DEBUG_LHC
//    //不可选择编辑
//    ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
//#endif // !DEBUG_LHC
//}

// 更新函数
void qg_QSLiteWidget::updateTimeSlot(const QString& table, const QString& timeSlot, Run_Result result) {
    QSqlQuery query;

    // 查询原有数据
    query.prepare("SELECT `总数量`,`OK`,`NG`,`2D NG`,`3D NG` FROM " + table + " WHERE `时间`=?");
    query.addBindValue(timeSlot);
    if (!query.exec() || !query.next()) {
        throw std::runtime_error("查询失败");
    }

    // 读取并更新数据
    int total = query.value(0).toInt() + 1;
    int ok = query.value(1).toInt() + (!result.isNG ? 1 : 0);
    int ng = query.value(2).toInt() + (result.isNG ? 1 : 0);
    int ng2d = query.value(3).toInt() + (result.isNG_2D ? 1 : 0);
    int ng3d = query.value(4).toInt() + (result.isNG_3D ? 1 : 0);
    double ngRate = (total > 0) ? (ng * 100.0 / total) : 0.0;

    // 执行更新
    query.prepare("UPDATE " + table + " SET `总数量`=?,`OK`=?,`NG`=?,`2D NG`=?,`3D NG`=?,`NG率`=? WHERE `时间`=?");
    query.addBindValue(total);
    query.addBindValue(ok);
    query.addBindValue(ng);
    query.addBindValue(ng2d);
    query.addBindValue(ng3d);
    query.addBindValue(QString::number(ngRate, 'f', 2) + "%");
    query.addBindValue(timeSlot);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    //刷新NG率
    //emit showNG_Signal(ng * 100 / double(total));

    return;
}


// 产量结果输出
void qg_QSLiteWidget::addProductResult(Run_Result runResult)
{
    // 获取当前日期
    QDate currentDate = QDate::currentDate();
    QString dateTime = currentDate.toString("yyyy-MM-dd");
    QString date = "`" + dateTime + "`";
    if (!m_dbManager.tableExists(dateTime))
    {
        //没有这个表，创建
        m_dbManager.addTable(date);
        // 添加一行数据
        int row = ui->tableWidget_Date->rowCount();
        ui->tableWidget_Date->insertRow(row);
        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(dateTime));
    }

    //计算数据
    QDateTime currentTime = QDateTime::currentDateTime();
    int currentHour = currentTime.toString("hh").toInt();  // 获取当前小时（24小时制）
    QString timeSlot = "8:00 - 9:00";
    if (currentHour - 8 >= 0 && currentHour - 8 < m_dbManager.timeName.size())
    {
        timeSlot = m_dbManager.timeName[currentHour - 8];  // 当前时间段字符串
    }
    else if (currentHour < 8 && currentHour + 16 >= 0 && currentHour + 16 < m_dbManager.timeName.size())
    {
        timeSlot = m_dbManager.timeName[currentHour + 16];  // 当前时间段字符串
    }

    //// 使用事务
    //m_dbManager.db().transaction();

    try {
        // 更新当前时段
        updateTimeSlot(date, timeSlot, runResult);

        // 更新全天时段
        updateTimeSlot(date, "全天 | 8:00 - 8:00", runResult);

       // m_dbManager.db().commit();
    }
    catch (const std::exception& e) {
       // m_dbManager.db().rollback();
        qDebug() << "数据库操作失败:" << e.what();
    }

    // 释放旧模型
    if (m_ProductModel) {
        delete m_ProductModel;
        m_ProductModel = nullptr;
    }

    // 创建新模型
    m_ProductModel = new QSqlTableModel(this);
    m_ProductModel->setTable(dateTime);
    m_ProductModel->select();
    ui->tableView_Product->setModel(m_ProductModel);
    m_currentProductTable = dateTime;

    // 调整列宽
    ui->tableView_Product->resizeColumnsToContents();
    ui->tableView_Product->hideColumn(0); // 隐藏ID列
    ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#ifndef DEBUG_LHC
    //不可选择编辑
    ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
#endif // !DEBUG_LHC


    return;
}

// 初始化产量统计
void qg_QSLiteWidget::initProductTable()
{
    // 日期的表头设置
    ui->tableWidget_Date->setColumnCount(1);
    ui->tableWidget_Date->setHorizontalHeaderLabels({ "日期" });
    ui->tableWidget_Date->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置整行选中模式
    ui->tableWidget_Date->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Date->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto allNames = m_dbManager.getProductTableNames();
    for (auto name : allNames)
    {
        // 添加一行数据
        int row = ui->tableWidget_Date->rowCount();
        ui->tableWidget_Date->insertRow(row);
        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(name));
    }
    connect(ui->tableWidget_Date->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, &qg_QSLiteWidget::onTableDateRowClicked);
}

//void qg_QSLiteWidget::onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous)
//{
//    // 获取选中的行的日期
//    QString selectedDate = ui->tableWidget_Date->item(current.row(), 0)->text();
//
//    // 设置 m_currentProductTable 为所选日期，便于后续查询
//    m_currentProductTable = selectedDate;
//
//    if (m_dbManager.tableExists(selectedDate)) {
//        // 释放旧的模型
//        if (m_ProductModel) {
//            delete m_ProductModel;
//            m_ProductModel = nullptr;
//        }
//
//        // 创建新模型
//        m_ProductModel = new QSqlTableModel(this);
//        m_ProductModel->setTable(selectedDate);  // 设置表格为选中的日期
//        m_ProductModel->select();  // 查询该表格中的所有数据
//
//        // 绑定模型到 tableView_Product
//        ui->tableView_Product->setModel(m_ProductModel);
//
//        // 调整列宽
//        ui->tableView_Product->resizeColumnsToContents();
//        ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//        ui->tableView_Product->hideColumn(0); // 隐藏ID列
//
//#ifndef DEBUG_LHC
//        // 禁止编辑
//        ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
//#endif // !DEBUG_LHC
//    }
//    else {
//        qDebug() << "Table for the selected date does not exist!";
//    }
//}

void qg_QSLiteWidget::onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous)
{
    // 新增边界检查 -------------------------
    if (!current.isValid() ||
        current.row() >= ui->tableWidget_Date->rowCount() ||
        current.row() < 0)
    {
        qDebug() << "无效的行选择";
        return;
    }

    QTableWidgetItem* dateItem = ui->tableWidget_Date->item(current.row(), 0);
    if (!dateItem) { // 防止空指针
        qWarning() << "日期项不存在，行号:" << current.row();
        return;
    }
    //-------------------------------------

    QString selectedDate = dateItem->text();
    m_currentProductTable = selectedDate;

    if (m_dbManager.tableExists(selectedDate)) {
        // 释放旧的模型
        if (m_ProductModel) {
            delete m_ProductModel;
            m_ProductModel = nullptr;
        }

        // 创建新模型
        m_ProductModel = new QSqlTableModel(this);
        m_ProductModel->setTable(selectedDate);
        if (!m_ProductModel->select()) { // 增加查询失败处理
            qCritical() << "数据加载失败:" << m_ProductModel->lastError().text();
            delete m_ProductModel;
            m_ProductModel = nullptr;
            return;
        }

        ui->tableView_Product->setModel(m_ProductModel);
        ui->tableView_Product->resizeColumnsToContents();
        ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_Product->hideColumn(0);

#ifndef DEBUG_LHC
        ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
#endif
    }
    else {
        qDebug() << "表不存在:" << selectedDate;
    }
}

// 产品结果输出
void qg_QSLiteWidget::addOneProductResult(Run_Result res, QString productName, QString filePath)
{
   
    //检查表是否存在
    QStringList tables = m_dbManager.getUserTableNames();
    if (!tables.contains("产品数据")) {
        machineLog->write("没有找到产品数据集，添加结果失败", Normal);
        return;
    }
    //获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");

    QSqlQuery query;
    query.prepare("INSERT INTO `产品数据` ("
        "name, `SN型号`,`结果`,`2D结果`,`3D结果`, `创建时间`, `更新时间`, `图片路径`, `复判`"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(productName);
    //if (res.modelName.isEmpty())
    //{
    //    query.addBindValue(productName);
    //}
    //else
    //{
    //    query.addBindValue(res.modelName);
    //}
    if (!LSM->m_requestSn.isEmpty())
        query.addBindValue(LSM->m_requestSn);
    else
        query.addBindValue("none");
    QString resultStr = res.isNG ? "NG" : "OK";
    query.addBindValue(resultStr);
    resultStr = res.isNG_2D ? "NG" : "OK";
    query.addBindValue(resultStr);
    resultStr = res.isNG_3D ? "NG" : "OK";
    query.addBindValue(resultStr);
    query.addBindValue(timeStr);
    query.addBindValue(timeStr);
    if (filePath.isEmpty())
    {
        machineLog->write("没有图片保存路径", Normal);
        query.addBindValue("");
    }
    else
    {
        query.addBindValue(filePath);
    }
    query.addBindValue("No");

    if (!query.exec()) {
        machineLog->write(QString("产品数据集添加失败: %1").arg(query.lastError().text()), Normal);
        //m_dbManager.rollbackTransaction(); // 回滚事务
        return;
    }
    //m_currentTable = "产品数据";
    refreshTableView();


    return;
}

//打开图片
void qg_QSLiteWidget::on_pushButton_openPhoto_clicked()
{
    QModelIndex index = ui->tableView_Sql->currentIndex();
    // 获取选中行
    int selectedRow = index.row();
    if (selectedRow < 0) return;

    if (!m_model)
        return;
    // 获取"图片路径"列数据
    int imagePathCol = m_model->fieldIndex("图片路径");
    if (imagePathCol == -1) {
        machineLog->write("错误：未找到列 '图片路径'！", Normal);
        return;
    }
    QString imagePath = m_model->data(m_model->index(selectedRow, imagePathCol)).toString();
    if (!imagePath.isNull()) {
        //cv::Mat img = cv::imread(imagePath.toUtf8().data());
        //if (img.empty())
        //    return;
        ////cv::namedWindow("image", cv::WINDOW_NORMAL);
        ////cv::resizeWindow("image", 800, 800);
        ////cv::imshow("image", img);

        //QImage showQImg = ImageTypeConvert::Mat2QImage(img);
        //QGraphicsViews* graphicsView = new QGraphicsViews();
        //graphicsView->DispImage(showQImg);

        //显示图片
        qg_showResultPicDlg dlg;
        dlg.loadAndShowBmpImages(imagePath);
        dlg.exec();
    }

}

void qg_QSLiteWidget::clearImageFolder() {
    // 获取目标文件夹路径
    QString folderPath = QCoreApplication::applicationDirPath() + "/images";
    QDir dir(folderPath);

    // 检查文件夹是否存在
    if (!dir.exists()) {
        qDebug() << "文件夹不存在：" << folderPath;
        return;
    }

    // 遍历并删除所有内容（保留文件夹本身）
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isFile()) {
            // 删除文件
            if (!QFile::remove(path)) {
                qDebug() << "删除文件失败：" << path;
            }
        }
        else if (entry.isDir()) {
            // 递归删除子目录
            QDir subDir(path);
            if (!subDir.removeRecursively()) {
                qDebug() << "删除目录失败：" << path;
            }
        }
    }

    qDebug() << "文件夹已清空：" << folderPath;
}

//删除
void qg_QSLiteWidget::on_pushButton_DeleteDate_clicked()
{
    // 获取当前选中的行
    int currentRow = ui->tableWidget_Date->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先在日期列表中选择要删除的行");
        return;
    }

    // 获取日期名称
    QString date = ui->tableWidget_Date->item(currentRow, 0)->text();
    if (date.isEmpty()) return;

    // 确认对话框
    if (QMessageBox::question(this, "确认删除",
        QString("确定要永久删除 [%1] 的所有数据吗？\n此操作不可恢复！").arg(date),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }

    // 执行删除操作
    QSqlDatabase::database().transaction(); // 开启事务

    try {
        // 1. 删除数据库表
        QSqlQuery dropQuery;
        if (!dropQuery.exec(QString("DROP TABLE `%1`").arg(date))) {
            throw std::runtime_error(dropQuery.lastError().text().toStdString());
        }

        // 2. 从日期列表中移除行
        ui->tableWidget_Date->removeRow(currentRow);

        // 3. 如果当前展示的是被删表，清空产品视图
        if (m_currentProductTable == date) {
            if (m_ProductModel) {
                m_ProductModel->clear();
                ui->tableView_Product->setModel(nullptr);
                m_currentProductTable.clear();
            }
        }

        QSqlDatabase::database().commit(); // 提交事务
        QMessageBox::information(this, "成功", "日期数据已删除");
    }
    catch (const std::exception& e) {
        QSqlDatabase::database().rollback(); // 回滚事务
        QMessageBox::critical(this, "错误",
            QString("删除失败:\n%1").arg(QString::fromStdString(e.what())));
    }
}

//清空
void qg_QSLiteWidget::on_pushButton_ClearDate_clicked()
{
    // 确认对话框
    if (QMessageBox::question(this, "操作",
        "确定要永久清空所有日期数据吗？\n此操作将删除所有历史记录且不可恢复！",
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }

    // 获取所有日期表名
    QStringList dateTables;
    for (int i = 0; i < ui->tableWidget_Date->rowCount(); ++i) {
        QString tableName = ui->tableWidget_Date->item(i, 0)->text();
        dateTables.append(tableName);
    }

    QSqlDatabase::database().transaction(); // 开启事务

    try {
        // 1. 删除所有数据库表
        foreach(const QString & table, dateTables) {
            if (!m_dbManager.deleteTable(table)) {
                throw std::runtime_error(
                    QString("删除表 %1 失败: %2")
                    .arg(table)
                    .arg(QSqlDatabase::database().lastError().text())
                    .toStdString()
                );
            }
        }

        // 2. 清空界面数据
        ui->tableWidget_Date->setRowCount(0); // 清空日期列表
        if (m_ProductModel) {
            m_ProductModel->clear();
            delete m_ProductModel;
            m_ProductModel = nullptr;
        }
        ui->tableView_Product->setModel(nullptr); // 清空产品视图

        QSqlDatabase::database().commit(); // 提交事务
        QMessageBox::information(this, "成功", "所有历史数据已清空");
    }
    catch (const std::exception& e) {
        QSqlDatabase::database().rollback(); // 回滚事务
        QMessageBox::critical(this, "错误",
            QString("清空失败:\n%1").arg(QString::fromStdString(e.what())));
        refreshTableList(); // 失败后刷新列表恢复显示
    }
}

//清空指定文件夹
void qg_QSLiteWidget::clearSelectImageFolder(QString path)
{
    QDir dir(path);

    // 检查文件夹是否存在
    if (!dir.exists()) {
        qDebug() << "文件夹不存在：" << path;
        return;
    }

    // 遍历并删除所有内容（保留文件夹本身）
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isFile()) {
            // 删除文件
            if (!QFile::remove(path)) {
                qDebug() << "删除文件失败：" << path;
            }
        }
        else if (entry.isDir()) {
            // 递归删除子目录
            QDir subDir(path);
            if (!subDir.removeRecursively()) {
                qDebug() << "删除目录失败：" << path;
            }
        }
    }
}


// 复判OK
void qg_QSLiteWidget::on_pushButton_checkOK_clicked() 
{
    // 确保当前显示的是产品数据表
    QString currentTable = ui->comboBoxTables->currentText();
    if (currentTable != "产品数据") {
        QMessageBox::warning(this, "提示", "请在产品数据表中进行操作");
        return;
    }

    // 获取当前选中行
    QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要复判的产品记录");
        return;
    }
    int selectedRow = currentIndex.row();

    // 获取列索引
    int resultCol = m_model->fieldIndex("结果");
    int result2DCol = m_model->fieldIndex("2D结果");
    int result3DCol = m_model->fieldIndex("3D结果");
    int recheckCol = m_model->fieldIndex("复判");

    // 检查列索引有效性
    if (resultCol < 0 || result2DCol < 0 || result3DCol < 0 || recheckCol < 0) {
        QMessageBox::critical(this, "错误", "数据库列信息不匹配");
        return;
    }

    //// 确认复判操作
    //if (QMessageBox::question(this, "确认",
    //    "确定要将此产品记录复判为OK吗？",
    //    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    //    return;
    //}

    // 更新模型数据
    m_model->setData(m_model->index(selectedRow, resultCol), "OK");
    m_model->setData(m_model->index(selectedRow, result2DCol), "OK");
    m_model->setData(m_model->index(selectedRow, result3DCol), "OK");
    m_model->setData(m_model->index(selectedRow, recheckCol), "Yes");

    //// 提交修改到数据库
    //if (!m_model->submitAll()) {
    //    QMessageBox::critical(this, "错误", "数据库更新失败:\n" + m_model->lastError().text());
    //    m_model->revertAll();
    //}
    //else {
    //    QMessageBox::information(this, "成功", "复判操作成功完成");
    //    // 可选：刷新视图显示
    //    m_model->select();
    //}
    return;
}

//复判NG
void qg_QSLiteWidget::on_pushButton_checkNG_clicked()
{
    // 确保当前显示的是产品数据表
    QString currentTable = ui->comboBoxTables->currentText();
    if (currentTable != "产品数据") {
        QMessageBox::warning(this, "提示", "请在产品数据表中进行操作");
        return;
    }

    // 获取当前选中行
    QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要复判的产品记录");
        return;
    }
    int selectedRow = currentIndex.row();

    // 获取列索引
    int resultCol = m_model->fieldIndex("结果");
    int result2DCol = m_model->fieldIndex("2D结果");
    int result3DCol = m_model->fieldIndex("3D结果");
    int recheckCol = m_model->fieldIndex("复判");

    // 检查列索引有效性
    if (resultCol < 0 || result2DCol < 0 || result3DCol < 0 || recheckCol < 0) {
        QMessageBox::critical(this, "错误", "数据库列信息不匹配");
        return;
    }

    //// 确认复判操作
    //if (QMessageBox::question(this, "确认",
    //    "确定要将此产品记录复判为OK吗？",
    //    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    //    return;
    //}

    // 更新模型数据
    m_model->setData(m_model->index(selectedRow, resultCol), "NG");
    m_model->setData(m_model->index(selectedRow, result2DCol), "NG");
    m_model->setData(m_model->index(selectedRow, result3DCol), "NG");
    m_model->setData(m_model->index(selectedRow, recheckCol), "Yes");

    //// 提交修改到数据库
    //if (!m_model->submitAll()) {
    //    QMessageBox::critical(this, "错误", "数据库更新失败:\n" + m_model->lastError().text());
    //    m_model->revertAll();
    //}
    //else {
    //    QMessageBox::information(this, "成功", "复判操作成功完成");
    //    // 可选：刷新视图显示
    //    m_model->select();
    //}
    return;
}

//NG皮带运动
void qg_QSLiteWidget::on_pushButton_ngBeltMove_pressed()
{
    LSM->setDO(LS_DO::ED_DO_NGBeltStart, 1);
    m_stopNGTimer.start(50);
}

//NG皮带停止运动
void qg_QSLiteWidget::on_pushButton_ngBeltMove_released()
{
    LSM->setDO(LS_DO::ED_DO_NGBeltStart, 0);
    m_stopNGTimer.stop();
}

//定时器检测停止NG皮带
void qg_QSLiteWidget::stopNGBelt()
{
    if (LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect))
    {
        //满料，停止皮带
        LSM->setDO(LS_DO::ED_DO_NGBeltStart, 0);
    }
}

//导出当日数据
void qg_QSLiteWidget::on_pushButton_ExportData_clicked()
{
    // 检查是否有选中的日期表
    int selectedRow = ui->tableWidget_Date->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "提示", "请先在日期列表中选择要导出的数据表");
        return;
    }

    // 获取选中的日期表名
    QTableWidgetItem* dateItem = ui->tableWidget_Date->item(selectedRow, 0);
    if (!dateItem) {
        qWarning() << "错误：日期项不存在";
        return;
    }
    QString tableName = dateItem->text();

    // 使用表名作为默认文件名
    QString defaultFileName = tableName + ".csv";

    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("导出CSV文件"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv)")
    );

    // 检查用户是否取消了操作
    if (filePath.isEmpty()) {
        return;
    }

    // 确保文件扩展名是.csv
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    try {
        // 查询整个表的数据（跳过ID列）
        QString queryStr = "SELECT * FROM `" + tableName + "`";
        QSqlQuery query;
        if (!query.exec(queryStr)) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        QSqlRecord record = query.record();

        // 写入表头（从1开始，跳过ID列）
        for (int col = 1; col < record.count(); ++col) {
            out << "\"" << record.fieldName(col) << "\"";
            if (col < record.count() - 1) {
                out << ",";
            }
        }
        out << "\n";

        // 写入数据行
        while (query.next()) {
            for (int col = 1; col < record.count(); ++col) {
                QString value = query.value(col).toString();

                // 处理包含逗号或引号的值
                if (value.contains(',') || value.contains('"')) {
                    value.replace("\"", "\"\"");
                    out << "\"" << value << "\"";
                }
                else {
                    out << value;
                }

                if (col < record.count() - 1) {
                    out << ",";
                }
            }
            out << "\n";
        }

        file.close();
        //QMessageBox::information(this, "成功", "数据已成功导出至:\n" + filePath);
    }
    catch (const std::exception& e) {
        file.close();
        QFile::remove(filePath);
        QMessageBox::critical(this, "错误", "导出失败: " + QString::fromStdString(e.what()));
    }
}

//清除所有已保存原图
void qg_QSLiteWidget::on_pushButton_ClearOriginal_clicked()
{
    if (QMessageBox::question(this, "确认",
        "确定要清空原图吗？")
        == QMessageBox::Yes)
    {
        QString folderPath = QCoreApplication::applicationDirPath() + "/images/Original";
        clearSelectImageFolder(folderPath);
    }
}

//双击打开图片
void qg_QSLiteWidget::onTableViewDoubleClicked()
{
    if (ui->comboBoxTables->currentText() != "产品数据") return;
    on_pushButton_openPhoto_clicked();
}