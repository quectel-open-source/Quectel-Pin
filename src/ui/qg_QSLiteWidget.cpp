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
    // ��ʼ�����ݿ�
    if (!m_dbManager.initializeDatabase()) {
        QMessageBox::critical(this, "Error", "�޷���ʼ�����ݿ�!");
        return;
    }

    if (LSM->m_version != EdPin)
    {
        ui->pushButton_ngBeltMove->hide();
    }

    //��ʼ��������
    initProductTable();
    initDateTimeEdits();

    // ȡ����ͷ��ʾ
    //ui->tableView_Sql->verticalHeader()->setVisible(false);
    // ��������ѡ��ģʽ
    ui->tableView_Sql->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_Sql->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ˢ�±���б�
    refreshTableList();

    ui->tableView_Product->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_Product->setSelectionBehavior(QAbstractItemView::SelectRows);

    // �����ź�
    connect(ui->comboBoxTables, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &qg_QSLiteWidget::onTableSelected);
    connect(ui->deleteButton, &QPushButton::clicked, this, &qg_QSLiteWidget::onDeleteButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &qg_QSLiteWidget::refreshTableView);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateChanged, [=](const QDate& date) {
        // �����ʼ�������ڽ������ڣ��Զ�������������
        if (date > ui->endDateTimeEdit->date()) {
            ui->endDateTimeEdit->setDate(date);
        }
        });

    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateChanged, [=](const QDate& date) {
        // ��������������ڿ�ʼ���ڣ��Զ�������ʼ����
        if (date < ui->startDateTimeEdit->date()) {
            ui->startDateTimeEdit->setDate(date);
        }
        });

    //��ʾ��һ�����
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

    if (tableName == "��Ʒ����")
    {
        ui->groupBox_CheckNG->setEnabled(true);
    }
    else
    {
        //չʾ���й���
        ui->groupBox_CheckNG->setEnabled(false);
    }

    // �ͷž�ģ��
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    // ������ģ��
    m_model = new QSqlTableModel(this);
    m_model->setTable(tableName);
    // ==== ����������� =====
   // ���ݱ������ò�ͬ�������ֶ�
    int sortColumn = -1;

    if (tableName == "��Ʒ����") {
        sortColumn = m_model->fieldIndex("����ʱ��");
    }
    else if (tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
        sortColumn = m_model->fieldIndex("����ʱ��");  // ��ʹ��"����ʱ��"
    }
    else {
        sortColumn = 0; // ͨ�ñ������У�ͨ����ID��
    }

    if (sortColumn >= 0) {
        m_model->setSort(sortColumn, Qt::DescendingOrder);
    }
    // ==== ����������� =====
    m_model->select();
    ui->tableView_Sql->setModel(m_model);
    m_currentTable = tableName;

    // �����п�
    ui->tableView_Sql->resizeColumnsToContents();
    ui->tableView_Sql->hideColumn(0); // ����ID��
#ifndef DEBUG_LHC
    //����ѡ��༭
    ui->tableView_Sql->setEditTriggers(QAbstractItemView::NoEditTriggers);
#endif // !DEBUG_LHC

}

//���
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

    if (QMessageBox::question(this, "ȷ��",
        QString("ȷ��Ҫ��ձ�� [%1] ������������").arg(currentTable))
        == QMessageBox::Yes)
    {
        //�Ȱ�ͼƬȫ�����
        if (currentTable == "��Ʒ����")
        {
            clearImageFolder();
        }
        if (m_dbManager.clearTableData(currentTable)) {
            refreshTableView();
            QMessageBox::information(this, "�ɹ�", "���������");
        }
    }
}

//ɾ��ĳ��
void qg_QSLiteWidget::onDeleteButtonClicked() {

    if (!m_model)
        return;
    QString tableName = ui->comboBoxTables->currentText();
    if (tableName == "��Ʒ����")
    {
        QModelIndex index = ui->tableView_Sql->currentIndex();
        // ��ȡѡ����
        int selectedRow = index.row();
        if (selectedRow >= 0)
        {
            // ��ȡ"ͼƬ·��"������
            int imagePathCol = m_model->fieldIndex("ͼƬ·��");
            if (imagePathCol == -1) {
                machineLog->write("����δ�ҵ��� 'ͼƬ·��'��", Normal);
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

//ˢ�±��
void qg_QSLiteWidget::refreshTableView() {
    //if (m_model) {
    //    // �ȱ��浱ǰѡ��ı��
    //    QString tableName = m_currentTable;

    //    // ��ȫ���´���ģ��
    //    delete m_model;
    //    m_model = new QSqlTableModel(this);
    //    m_model->setTable(tableName);

    //    // ����Ӧ����������
    //    if (tableName == "��Ʒ����") {
    //        m_model->setSort(m_model->fieldIndex("����ʱ��"), Qt::DescendingOrder);
    //    }
    //    else if (tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
    //        m_model->setSort(m_model->fieldIndex("����ʱ��"), Qt::DescendingOrder);
    //    }

    //    m_model->select();
    //    ui->tableView_Sql->setModel(m_model);

    //    // ͨ�ñ�����ͼ��������
    //    if (!(tableName == "��Ʒ����" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result")) {
    //        ui->tableView_Sql->sortByColumn(0, Qt::DescendingOrder);
    //    }
    //}
    //ui->tableView_Sql->resizeColumnsToContents();
    //refreshTableList(); // ˢ�±���б�

    if (!m_model) return;

    try {
        // ���浱ǰѡ�񣨿�ѡ��
        QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
        // ��������
        const QString tableName = m_currentTable;
        //m_model->setTable("Pin2D_Result");
        //m_model->setTable(m_currentTable);
        const int dateColumn = m_model->fieldIndex("����ʱ��");

        if (tableName == "��Ʒ����" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result") {
            if (dateColumn != -1) { // ȷ���д���
                m_model->setSort(dateColumn, Qt::DescendingOrder);
            }
        }

        //ui->tableView_Sql->setModel(m_model);

        //// ���ģ���Ƿ������˱���û��������
        //if (m_model->tableName().isEmpty()) {
        //    m_model->setTable("`" + m_currentTable + "`");
        //}

        // ˢ������
        if (!m_model->select()) {
            machineLog->write("���ݼ�ˢ��ʧ��" + m_model->lastError().text(), Normal);
            throw std::runtime_error(m_model->lastError().text().toStdString());
        }
        //m_model->select();


        // ͨ�ñ�����
        if (!(tableName == "��Ʒ����" || tableName == "Pin2D_Result" || tableName == "Pin3D_Result")) {
            ui->tableView_Sql->sortByColumn(0, Qt::DescendingOrder);
        }

        // �ָ�ѡ�񣨿�ѡ��
        ui->tableView_Sql->setCurrentIndex(currentIndex);

        // �����п�
        ui->tableView_Sql->resizeColumnsToContents();
    }
    catch (const std::exception& e) {
        qCritical() << "ˢ�±��ʱ����" << e.what();
        //QMessageBox::critical(this, "����", QString("ˢ�±��ʧ�ܣ�%1").arg(e.what()));
    }
}

//ˢ�±���б�
void qg_QSLiteWidget::refreshTableList() {
    ui->comboBoxTables->clear();
    ui->comboBoxTables->addItems(m_dbManager.getUserTableNames());
}

//����2Dʶ���Ľ��
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
            machineLog->write("û���ҵ�2D���ݼ�����ӽ��ʧ��", Normal);
            return;
        }

        // ��¼NG����
        unordered_set<int> ngIdxSet(result.ngIdx.begin(), result.ngIdx.end());
        // ��ȡ��ǰʱ��
        //QDateTime currentTime = QDateTime::currentDateTime().toUTC();
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");
        // �������ݵ�
        for (int i = 0; i < result.hv_Row_Array.TupleLength().I(); ++i) {
            QSqlQuery query;
            query.prepare("INSERT INTO Pin2D_Result ("
                "name,"
                "`SN�ͺ�`,"
                "X,"
                "Y, "
                "`��Ի�׼��X������� mm`, "
                "`��Ի�׼��Y������� mm`,"
                "`��Ի�׼��X����ƫ��ֵ mm`,"
                "`��Ի�׼��Y����ƫ��ֵ mm`,"
                "`���`,"
                "`����ʱ��`,"
                "`����ʱ��`"
                ")VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ?)");


            // ���
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
                machineLog->write("2D���ݼ����ʧ��", Normal);
                return;
            }
        }
        m_currentTable = "Pin2D_Result";
        refreshTableView();
    }
    catch (...)
    {
        machineLog->write("2D���ݼ���ȡ����ʧ��", Normal);
        return;
    }
}

//ʱ���ѯ�ؼ���ʼ��
void qg_QSLiteWidget::initDateTimeEdits()
{
    ui->startDateTimeEdit->setCalendarPopup(true);  // ��������
    ui->endDateTimeEdit->setCalendarPopup(true);  // ��������
    // ��ʼʱ��Ĭ��1��ǰ
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    // ����ʱ��Ĭ��һ���
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    ui->endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    // ʱ�䷶Χ����
    QDateTime minTime = QDateTime::currentDateTime().addYears(-1);
    QDateTime maxTime = QDateTime::currentDateTime().addYears(1);
    ui->startDateTimeEdit->setMinimumDateTime(minTime);
    ui->startDateTimeEdit->setMaximumDateTime(maxTime);
    ui->endDateTimeEdit->setMinimumDateTime(minTime);
    ui->endDateTimeEdit->setMaximumDateTime(maxTime);
}

// ��ѯ��ť���
void qg_QSLiteWidget::on_pushButton_find_clicked() {
    // ��ȡʱ�䷶Χ
    QDateTime start = ui->startDateTimeEdit->dateTime();
    QDateTime end = ui->endDateTimeEdit->dateTime();

    // У��ʱ����Ч��
    if (start > end) {
        QMessageBox::warning(this, "��ʾ", "����ʱ�䲻�����ڿ�ʼʱ��");
        return;
    }

    // ���ɹ�������
    QString filter = siftTabel();

    // ����ģ�͹�����
    //m_model->setTable("Pin2D_Result");
    m_model->setFilter(filter);
    m_model->select();
    // ���ģ�ʹ���
    if (!m_model->select()) {
        qDebug() << "��ѯ����:" << m_model->lastError().text();
    }

    // ��ʾ����
    ui->tableView_Sql->setModel(m_model);
    //ui->tableView_Sql->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // �����п�
    ui->tableView_Sql->resizeColumnsToContents();

    // �Ż���ʾ
    ui->tableView_Sql->hideColumn(0); // ����ID��
    ui->tableView_Sql->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableView_Sql->setSortingEnabled(true);
}

//���3Dpin����
void qg_QSLiteWidget::addNew3DResult(ResultParamsStruct_Pin3DMeasure result, SQL_Parameter para) {
    try {

        //�����Ƿ����
        QStringList tables = m_dbManager.getUserTableNames();
        if (!tables.contains("Pin3D_Result")) {
            machineLog->write("û���ҵ�3D���ݼ�����ӽ��ʧ��", Normal);
            return;
        }

        //��ȡ��ǰʱ��
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");

        ////�������񣨿�ѡ��
        //m_dbManager.beginTransaction();

        //��������
        for (int i = 0; i < result.PinFlag.size(); ++i) {
            QSqlQuery query;
            query.prepare("INSERT INTO Pin3D_Result ("
                "name,"
                "`SN�ͺ�`, "
                "`�߶�`,"
                "`���`,"
                "`����ʱ��`,"
                "`����ʱ��`"
                ") VALUES (?, ?, ?, ?, ?, ?)");

            query.addBindValue(para.formulaName);
            query.addBindValue(para.snName);
            query.addBindValue(result.PinHeight[i]);

            QString resultStr = (result.PinHeight[i] == -2) ? "NG" : (result.PinFlag[i] ? "OK" : "NG");
            query.addBindValue(resultStr);
            query.addBindValue(timeStr);
            query.addBindValue(timeStr);

            if (!query.exec()) {
                machineLog->write(QString("3D���ݼ����ʧ��: %1").arg(query.lastError().text()), Normal);
                //m_dbManager.rollbackTransaction(); // �ع�����
                return;
            }
        }

        ////�ύ���񣨿�ѡ��
        //m_dbManager.commitTransaction();
        m_currentTable = "Pin3D_Result";
        refreshTableView();
    }
    catch (...) {
        machineLog->write("3D���ݼ������쳣", Normal);
    }
}

//���ݿ�����CSV����
void qg_QSLiteWidget::on_pushButton_ExportCSV_clicked()
{
    // ���ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        tr("Save File"),
        "",
        tr("CSV Files (*.csv)")
    );

    // ����û�û��ѡ���ļ������ߵ����ȡ��
    if (filePath.isEmpty()) {
        return;
    }

    // ȷ���ļ���չ���� .csv
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }


    QString tableName = ui->comboBoxTables->currentText();
    QString queryStr = "SELECT * FROM `" + tableName + "`";

    if (ui->radioButton_Export->isChecked())
    {
        // ����ɸѡ�ı����Ϣ
        // ��ȡ��ǰ�Ĺ�������
        QString filter = m_model->filter();

        // ����й������������ɶ�Ӧ�Ĳ�ѯ���
        if (!filter.isEmpty()) {
            queryStr += " WHERE " + filter;  // ���ɸѡ����
        }
    }

    QSqlQuery query(queryStr);
    // ��ѯ�����������
    if (!query.exec()) {
        machineLog->write("��ѯʧ��: " + query.lastError().text(), Normal);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        machineLog->write("�޷����ļ�: " + file.errorString(), Normal);
        return;
    }

    QTextStream out(&file);

    // д���ͷ����1��ʼ������id��
    QSqlRecord record = query.record();
    for (int i = 1; i < record.count(); ++i) {
        out << record.fieldName(i);  // ��ȡ�ֶ�����
        if (i != record.count() - 1)
            out << ",";
    }
    out << "\n";

    // д������
    while (query.next()) {
        for (int i = 1; i < record.count(); ++i) {
            out << query.value(i).toString();  // ��ȡ�ֶ�ֵ
            if (i != record.count() - 1)
                out << ",";
        }
        out << "\n";
    }

    file.close();
    qDebug() << "�����ѳɹ�����Ϊ CSV �ļ�";
}

// ����ɸѡ
QString qg_QSLiteWidget::siftTabel()
{
    // ��ȡʱ�䷶Χ
    QDateTime start = ui->startDateTimeEdit->dateTime();
    QDateTime end = ui->endDateTimeEdit->dateTime();
    // ���ɹ�������
    QString filter = QString("`����ʱ��` >= '%1' AND `����ʱ��` <= '%2'")
        .arg(start.toString("yyyy-MM-dd HH:mm:ss"))
        .arg(end.toString("yyyy-MM-dd HH:mm:ss"));

    // �Ƿ�������NGɸѡ
    if (ui->checkBox_checkNG->isChecked())
    {
        filter += " AND `���` LIKE '%NG%'";
    }
    // �Ƿ�������OKɸѡ
    if (ui->checkBox_checkOK->isChecked())
    {
        filter += " AND `���` LIKE '%OK%'";
    }
    //�Ƿ������˰�����ɸѡ
    QString name = ui->lineEdit_name->text();
    if (!name.isEmpty()) {
        // ʹ�� arg() �Զ�����ת�壬���� SQL ע��
        filter += QString(" AND name LIKE '%%1%'").arg(name);
    }
    //�Ƿ������˰�sn�ͺ�ɸѡ
    QString nameSN = ui->lineEdit_sn->text();
    if (!nameSN.isEmpty()) {
        // ʹ�� arg() �Զ�����ת�壬���� SQL ע��
        filter += QString(" AND `SN�ͺ�` LIKE '%%1%'").arg(nameSN);
    }
    
    return filter;
}

//// ����������
//void qg_QSLiteWidget::addProductResult(Run_Result runResult)
//{
//     // ��ȡ��ǰ����
//    QDate currentDate = QDate::currentDate();
//    QString dateTime = currentDate.toString("yyyy-MM-dd");
//    QString date = "`" + dateTime + "`";
//    if (!m_dbManager.tableExists(dateTime))
//    {
//        //û�����������
//        m_dbManager.addTable(date);
//        // ���һ������
//        int row = ui->tableWidget_Date->rowCount();
//        ui->tableWidget_Date->insertRow(row);
//        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(dateTime));
//    }
//
//    //��������
//    QDateTime currentTime = QDateTime::currentDateTime();
//    int currentHour = currentTime.toString("hh").toInt();  // ��ȡ��ǰСʱ��24Сʱ�ƣ�
//    QString timeSlot = m_dbManager.timeName[currentHour - 8];  // ��ǰʱ����ַ���
//
//    QSqlQuery query;
//    //��ȡ
//    int totalQuantity =0;  // `������`
//    int ok = 0;             // `OK`
//    int recheckOk = 0;      // `���� OK`
//    int ng = 0;      // `NG`
//    int ng2D = 0;           // `2D NG`
//    int ng3D = 0;           // `3D NG`
//    QString ngRate = "0.00%";      // `NG��`
//
//    query.prepare("SELECT `������`, `OK`, `���� OK`, `NG`, `2D NG`, `3D NG`, `NG��` "
//        "FROM " + date + " WHERE `ʱ��` = ?");
//    query.addBindValue(timeSlot);  // ��ʱ�������
//
//    // ִ�в�ѯ������Ƿ�ɹ�
//    if (!query.exec()) {
//        qDebug() << "Error executing query: " << query.lastError().text();
//        return;
//    }
//    else {
//        qDebug() << "Query executed successfully.";
//
//        // ��ȡ��ѯ���
//        if (query.next()) {  // �����ѯ������
//            totalQuantity = query.value(0).toInt();  // `������`
//            ok = query.value(1).toInt();             // `OK`
//            recheckOk = query.value(2).toInt();      // `���� OK`
//            ng2D = query.value(3).toInt();           // `NG`
//            ng2D = query.value(4).toInt();           // `2D NG`
//            ng3D = query.value(5).toInt();           // `3D NG`
//            //QString ngRate = query.value(5).toString();  // `NG��`
//
//            // ����ˢ������
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
//                //����NG��
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
//    //д��
//    // ����Ҫ���µ���������,���ԣ�
//    // ʹ�� UPDATE ����������
//    query.prepare("UPDATE " + date + " SET "
//        "`������` = ?, `OK` = ?,`NG` = ?, `2D NG` = ?, `3D NG` = ?, `NG��` = ? "
//        "WHERE `ʱ��` = ?");  // ʹ�� `WHERE` ��ָ���ض���ʱ���
//
//    // �󶨲���
//    query.addBindValue(totalQuantity);        // `������`
//    query.addBindValue(ok);         // `OK`
//    query.addBindValue(ng);         // `NG`
//    query.addBindValue(ng2D);         // `2D NG`
//    query.addBindValue(ng3D);         // `3D NG`
//    query.addBindValue(ngRate);  // `NG��`
//    query.addBindValue(timeSlot);  // `ʱ��` �ֶε������������ҵ��ض�ʱ��ζ�Ӧ����
//
//    // ִ�в�ѯ������Ƿ�ɹ�
//    if (!query.exec()) {
//        qDebug() << "Error updating database: " << query.lastError().text();
//    }
//    else {
//        qDebug() << "Database updated successfully.";
//    }
//
//    //----------------------------------------------- ȫ���������
//    QSqlQuery queryAllDay;
//    queryAllDay.prepare("SELECT `������`, `OK`, `���� OK`, `NG`, `2D NG`, `3D NG`, `NG��` "
//        "FROM " + date + " WHERE `ʱ��` = ?");
//    queryAllDay.addBindValue("ȫ�� | 8:00 - 8:00");
//    // ִ�в�ѯ������Ƿ�ɹ�
//    if (!queryAllDay.exec()) {
//        qDebug() << "allday Error executing query: " << queryAllDay.lastError().text();
//        return;
//    }
//    else {
//        qDebug() << "allday Query executed successfully.";
//
//        // ��ȡ��ѯ���
//        if (queryAllDay.next()) {  // �����ѯ������
//            totalQuantity = queryAllDay.value(0).toInt();  // `������`
//            ok = queryAllDay.value(1).toInt();             // `OK`
//            recheckOk = queryAllDay.value(2).toInt();      // `���� OK`
//            ng2D = queryAllDay.value(3).toInt();           // `NG`
//            ng2D = queryAllDay.value(4).toInt();           // `2D NG`
//            ng3D = queryAllDay.value(5).toInt();           // `3D NG`
//            //QString ngRate = queryAllDay.value(5).toString();  // `NG��`
//
//            // ����ˢ������
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
//                //����NG��
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
//    //д��
//    // ����Ҫ���µ���������,���ԣ�
//    // ʹ�� UPDATE ����������
//    queryAllDay.prepare("UPDATE " + date + " SET "
//        "`������` = ?, `OK` = ?,`NG` = ?, `2D NG` = ?, `3D NG` = ?, `NG��` = ? "
//        "WHERE `ʱ��` = ?");  // ʹ�� `WHERE` ��ָ���ض���ʱ���
//
//    // �󶨲���
//    queryAllDay.addBindValue(totalQuantity);        // `������`
//    queryAllDay.addBindValue(ok);         // `OK`
//    queryAllDay.addBindValue(ng);         // `NG`
//    queryAllDay.addBindValue(ng2D);         // `2D NG`
//    queryAllDay.addBindValue(ng3D);         // `3D NG`
//    queryAllDay.addBindValue(ngRate);  // `NG��`
//    queryAllDay.addBindValue(timeSlot);  // `ʱ��` �ֶε������������ҵ��ض�ʱ��ζ�Ӧ����
//
//    // ִ�в�ѯ������Ƿ�ɹ�
//    if (!queryAllDay.exec()) {
//        qDebug() << "allday Error updating database: " << queryAllDay.lastError().text();
//    }
//    else {
//        qDebug() << "allday Database updated successfully.";
//    }
//
//    // �ͷž�ģ��
//    if (m_ProductModel) {
//        delete m_ProductModel;
//        m_ProductModel = nullptr;
//    }
//
//    // ������ģ��
//    m_ProductModel = new QSqlTableModel(this);
//    m_ProductModel->setTable(dateTime);
//    m_ProductModel->select();
//    ui->tableView_Product->setModel(m_ProductModel);
//    m_currentProductTable = dateTime;
//
//    // �����п�
//    ui->tableView_Product->resizeColumnsToContents();
//    ui->tableView_Product->hideColumn(0); // ����ID��
//    ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//#ifndef DEBUG_LHC
//    //����ѡ��༭
//    ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
//#endif // !DEBUG_LHC
//}

// ���º���
void qg_QSLiteWidget::updateTimeSlot(const QString& table, const QString& timeSlot, Run_Result result) {
    QSqlQuery query;

    // ��ѯԭ������
    query.prepare("SELECT `������`,`OK`,`NG`,`2D NG`,`3D NG` FROM " + table + " WHERE `ʱ��`=?");
    query.addBindValue(timeSlot);
    if (!query.exec() || !query.next()) {
        throw std::runtime_error("��ѯʧ��");
    }

    // ��ȡ����������
    int total = query.value(0).toInt() + 1;
    int ok = query.value(1).toInt() + (!result.isNG ? 1 : 0);
    int ng = query.value(2).toInt() + (result.isNG ? 1 : 0);
    int ng2d = query.value(3).toInt() + (result.isNG_2D ? 1 : 0);
    int ng3d = query.value(4).toInt() + (result.isNG_3D ? 1 : 0);
    double ngRate = (total > 0) ? (ng * 100.0 / total) : 0.0;

    // ִ�и���
    query.prepare("UPDATE " + table + " SET `������`=?,`OK`=?,`NG`=?,`2D NG`=?,`3D NG`=?,`NG��`=? WHERE `ʱ��`=?");
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

    //ˢ��NG��
    //emit showNG_Signal(ng * 100 / double(total));

    return;
}


// ����������
void qg_QSLiteWidget::addProductResult(Run_Result runResult)
{
    // ��ȡ��ǰ����
    QDate currentDate = QDate::currentDate();
    QString dateTime = currentDate.toString("yyyy-MM-dd");
    QString date = "`" + dateTime + "`";
    if (!m_dbManager.tableExists(dateTime))
    {
        //û�����������
        m_dbManager.addTable(date);
        // ���һ������
        int row = ui->tableWidget_Date->rowCount();
        ui->tableWidget_Date->insertRow(row);
        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(dateTime));
    }

    //��������
    QDateTime currentTime = QDateTime::currentDateTime();
    int currentHour = currentTime.toString("hh").toInt();  // ��ȡ��ǰСʱ��24Сʱ�ƣ�
    QString timeSlot = "8:00 - 9:00";
    if (currentHour - 8 >= 0 && currentHour - 8 < m_dbManager.timeName.size())
    {
        timeSlot = m_dbManager.timeName[currentHour - 8];  // ��ǰʱ����ַ���
    }
    else if (currentHour < 8 && currentHour + 16 >= 0 && currentHour + 16 < m_dbManager.timeName.size())
    {
        timeSlot = m_dbManager.timeName[currentHour + 16];  // ��ǰʱ����ַ���
    }

    //// ʹ������
    //m_dbManager.db().transaction();

    try {
        // ���µ�ǰʱ��
        updateTimeSlot(date, timeSlot, runResult);

        // ����ȫ��ʱ��
        updateTimeSlot(date, "ȫ�� | 8:00 - 8:00", runResult);

       // m_dbManager.db().commit();
    }
    catch (const std::exception& e) {
       // m_dbManager.db().rollback();
        qDebug() << "���ݿ����ʧ��:" << e.what();
    }

    // �ͷž�ģ��
    if (m_ProductModel) {
        delete m_ProductModel;
        m_ProductModel = nullptr;
    }

    // ������ģ��
    m_ProductModel = new QSqlTableModel(this);
    m_ProductModel->setTable(dateTime);
    m_ProductModel->select();
    ui->tableView_Product->setModel(m_ProductModel);
    m_currentProductTable = dateTime;

    // �����п�
    ui->tableView_Product->resizeColumnsToContents();
    ui->tableView_Product->hideColumn(0); // ����ID��
    ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#ifndef DEBUG_LHC
    //����ѡ��༭
    ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
#endif // !DEBUG_LHC


    return;
}

// ��ʼ������ͳ��
void qg_QSLiteWidget::initProductTable()
{
    // ���ڵı�ͷ����
    ui->tableWidget_Date->setColumnCount(1);
    ui->tableWidget_Date->setHorizontalHeaderLabels({ "����" });
    ui->tableWidget_Date->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ��������ѡ��ģʽ
    ui->tableWidget_Date->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Date->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto allNames = m_dbManager.getProductTableNames();
    for (auto name : allNames)
    {
        // ���һ������
        int row = ui->tableWidget_Date->rowCount();
        ui->tableWidget_Date->insertRow(row);
        ui->tableWidget_Date->setItem(row, 0, new QTableWidgetItem(name));
    }
    connect(ui->tableWidget_Date->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, &qg_QSLiteWidget::onTableDateRowClicked);
}

//void qg_QSLiteWidget::onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous)
//{
//    // ��ȡѡ�е��е�����
//    QString selectedDate = ui->tableWidget_Date->item(current.row(), 0)->text();
//
//    // ���� m_currentProductTable Ϊ��ѡ���ڣ����ں�����ѯ
//    m_currentProductTable = selectedDate;
//
//    if (m_dbManager.tableExists(selectedDate)) {
//        // �ͷžɵ�ģ��
//        if (m_ProductModel) {
//            delete m_ProductModel;
//            m_ProductModel = nullptr;
//        }
//
//        // ������ģ��
//        m_ProductModel = new QSqlTableModel(this);
//        m_ProductModel->setTable(selectedDate);  // ���ñ��Ϊѡ�е�����
//        m_ProductModel->select();  // ��ѯ�ñ���е���������
//
//        // ��ģ�͵� tableView_Product
//        ui->tableView_Product->setModel(m_ProductModel);
//
//        // �����п�
//        ui->tableView_Product->resizeColumnsToContents();
//        ui->tableView_Product->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//        ui->tableView_Product->hideColumn(0); // ����ID��
//
//#ifndef DEBUG_LHC
//        // ��ֹ�༭
//        ui->tableView_Product->setEditTriggers(QAbstractItemView::NoEditTriggers);
//#endif // !DEBUG_LHC
//    }
//    else {
//        qDebug() << "Table for the selected date does not exist!";
//    }
//}

void qg_QSLiteWidget::onTableDateRowClicked(const QModelIndex& current, const QModelIndex& previous)
{
    // �����߽��� -------------------------
    if (!current.isValid() ||
        current.row() >= ui->tableWidget_Date->rowCount() ||
        current.row() < 0)
    {
        qDebug() << "��Ч����ѡ��";
        return;
    }

    QTableWidgetItem* dateItem = ui->tableWidget_Date->item(current.row(), 0);
    if (!dateItem) { // ��ֹ��ָ��
        qWarning() << "��������ڣ��к�:" << current.row();
        return;
    }
    //-------------------------------------

    QString selectedDate = dateItem->text();
    m_currentProductTable = selectedDate;

    if (m_dbManager.tableExists(selectedDate)) {
        // �ͷžɵ�ģ��
        if (m_ProductModel) {
            delete m_ProductModel;
            m_ProductModel = nullptr;
        }

        // ������ģ��
        m_ProductModel = new QSqlTableModel(this);
        m_ProductModel->setTable(selectedDate);
        if (!m_ProductModel->select()) { // ���Ӳ�ѯʧ�ܴ���
            qCritical() << "���ݼ���ʧ��:" << m_ProductModel->lastError().text();
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
        qDebug() << "������:" << selectedDate;
    }
}

// ��Ʒ������
void qg_QSLiteWidget::addOneProductResult(Run_Result res, QString productName, QString filePath)
{
   
    //�����Ƿ����
    QStringList tables = m_dbManager.getUserTableNames();
    if (!tables.contains("��Ʒ����")) {
        machineLog->write("û���ҵ���Ʒ���ݼ�����ӽ��ʧ��", Normal);
        return;
    }
    //��ȡ��ǰʱ��
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");

    QSqlQuery query;
    query.prepare("INSERT INTO `��Ʒ����` ("
        "name, `SN�ͺ�`,`���`,`2D���`,`3D���`, `����ʱ��`, `����ʱ��`, `ͼƬ·��`, `����`"
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
        machineLog->write("û��ͼƬ����·��", Normal);
        query.addBindValue("");
    }
    else
    {
        query.addBindValue(filePath);
    }
    query.addBindValue("No");

    if (!query.exec()) {
        machineLog->write(QString("��Ʒ���ݼ����ʧ��: %1").arg(query.lastError().text()), Normal);
        //m_dbManager.rollbackTransaction(); // �ع�����
        return;
    }
    //m_currentTable = "��Ʒ����";
    refreshTableView();


    return;
}

//��ͼƬ
void qg_QSLiteWidget::on_pushButton_openPhoto_clicked()
{
    QModelIndex index = ui->tableView_Sql->currentIndex();
    // ��ȡѡ����
    int selectedRow = index.row();
    if (selectedRow < 0) return;

    if (!m_model)
        return;
    // ��ȡ"ͼƬ·��"������
    int imagePathCol = m_model->fieldIndex("ͼƬ·��");
    if (imagePathCol == -1) {
        machineLog->write("����δ�ҵ��� 'ͼƬ·��'��", Normal);
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

        //��ʾͼƬ
        qg_showResultPicDlg dlg;
        dlg.loadAndShowBmpImages(imagePath);
        dlg.exec();
    }

}

void qg_QSLiteWidget::clearImageFolder() {
    // ��ȡĿ���ļ���·��
    QString folderPath = QCoreApplication::applicationDirPath() + "/images";
    QDir dir(folderPath);

    // ����ļ����Ƿ����
    if (!dir.exists()) {
        qDebug() << "�ļ��в����ڣ�" << folderPath;
        return;
    }

    // ������ɾ���������ݣ������ļ��б���
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isFile()) {
            // ɾ���ļ�
            if (!QFile::remove(path)) {
                qDebug() << "ɾ���ļ�ʧ�ܣ�" << path;
            }
        }
        else if (entry.isDir()) {
            // �ݹ�ɾ����Ŀ¼
            QDir subDir(path);
            if (!subDir.removeRecursively()) {
                qDebug() << "ɾ��Ŀ¼ʧ�ܣ�" << path;
            }
        }
    }

    qDebug() << "�ļ�������գ�" << folderPath;
}

//ɾ��
void qg_QSLiteWidget::on_pushButton_DeleteDate_clicked()
{
    // ��ȡ��ǰѡ�е���
    int currentRow = ui->tableWidget_Date->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "��ʾ", "�����������б���ѡ��Ҫɾ������");
        return;
    }

    // ��ȡ��������
    QString date = ui->tableWidget_Date->item(currentRow, 0)->text();
    if (date.isEmpty()) return;

    // ȷ�϶Ի���
    if (QMessageBox::question(this, "ȷ��ɾ��",
        QString("ȷ��Ҫ����ɾ�� [%1] ������������\n�˲������ɻָ���").arg(date),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }

    // ִ��ɾ������
    QSqlDatabase::database().transaction(); // ��������

    try {
        // 1. ɾ�����ݿ��
        QSqlQuery dropQuery;
        if (!dropQuery.exec(QString("DROP TABLE `%1`").arg(date))) {
            throw std::runtime_error(dropQuery.lastError().text().toStdString());
        }

        // 2. �������б����Ƴ���
        ui->tableWidget_Date->removeRow(currentRow);

        // 3. �����ǰչʾ���Ǳ�ɾ����ղ�Ʒ��ͼ
        if (m_currentProductTable == date) {
            if (m_ProductModel) {
                m_ProductModel->clear();
                ui->tableView_Product->setModel(nullptr);
                m_currentProductTable.clear();
            }
        }

        QSqlDatabase::database().commit(); // �ύ����
        QMessageBox::information(this, "�ɹ�", "����������ɾ��");
    }
    catch (const std::exception& e) {
        QSqlDatabase::database().rollback(); // �ع�����
        QMessageBox::critical(this, "����",
            QString("ɾ��ʧ��:\n%1").arg(QString::fromStdString(e.what())));
    }
}

//���
void qg_QSLiteWidget::on_pushButton_ClearDate_clicked()
{
    // ȷ�϶Ի���
    if (QMessageBox::question(this, "����",
        "ȷ��Ҫ���������������������\n�˲�����ɾ��������ʷ��¼�Ҳ��ɻָ���",
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }

    // ��ȡ�������ڱ���
    QStringList dateTables;
    for (int i = 0; i < ui->tableWidget_Date->rowCount(); ++i) {
        QString tableName = ui->tableWidget_Date->item(i, 0)->text();
        dateTables.append(tableName);
    }

    QSqlDatabase::database().transaction(); // ��������

    try {
        // 1. ɾ���������ݿ��
        foreach(const QString & table, dateTables) {
            if (!m_dbManager.deleteTable(table)) {
                throw std::runtime_error(
                    QString("ɾ���� %1 ʧ��: %2")
                    .arg(table)
                    .arg(QSqlDatabase::database().lastError().text())
                    .toStdString()
                );
            }
        }

        // 2. ��ս�������
        ui->tableWidget_Date->setRowCount(0); // ��������б�
        if (m_ProductModel) {
            m_ProductModel->clear();
            delete m_ProductModel;
            m_ProductModel = nullptr;
        }
        ui->tableView_Product->setModel(nullptr); // ��ղ�Ʒ��ͼ

        QSqlDatabase::database().commit(); // �ύ����
        QMessageBox::information(this, "�ɹ�", "������ʷ���������");
    }
    catch (const std::exception& e) {
        QSqlDatabase::database().rollback(); // �ع�����
        QMessageBox::critical(this, "����",
            QString("���ʧ��:\n%1").arg(QString::fromStdString(e.what())));
        refreshTableList(); // ʧ�ܺ�ˢ���б�ָ���ʾ
    }
}

//���ָ���ļ���
void qg_QSLiteWidget::clearSelectImageFolder(QString path)
{
    QDir dir(path);

    // ����ļ����Ƿ����
    if (!dir.exists()) {
        qDebug() << "�ļ��в����ڣ�" << path;
        return;
    }

    // ������ɾ���������ݣ������ļ��б���
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isFile()) {
            // ɾ���ļ�
            if (!QFile::remove(path)) {
                qDebug() << "ɾ���ļ�ʧ�ܣ�" << path;
            }
        }
        else if (entry.isDir()) {
            // �ݹ�ɾ����Ŀ¼
            QDir subDir(path);
            if (!subDir.removeRecursively()) {
                qDebug() << "ɾ��Ŀ¼ʧ�ܣ�" << path;
            }
        }
    }
}


// ����OK
void qg_QSLiteWidget::on_pushButton_checkOK_clicked() 
{
    // ȷ����ǰ��ʾ���ǲ�Ʒ���ݱ�
    QString currentTable = ui->comboBoxTables->currentText();
    if (currentTable != "��Ʒ����") {
        QMessageBox::warning(this, "��ʾ", "���ڲ�Ʒ���ݱ��н��в���");
        return;
    }

    // ��ȡ��ǰѡ����
    QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "��ʾ", "��ѡ��Ҫ���еĲ�Ʒ��¼");
        return;
    }
    int selectedRow = currentIndex.row();

    // ��ȡ������
    int resultCol = m_model->fieldIndex("���");
    int result2DCol = m_model->fieldIndex("2D���");
    int result3DCol = m_model->fieldIndex("3D���");
    int recheckCol = m_model->fieldIndex("����");

    // �����������Ч��
    if (resultCol < 0 || result2DCol < 0 || result3DCol < 0 || recheckCol < 0) {
        QMessageBox::critical(this, "����", "���ݿ�����Ϣ��ƥ��");
        return;
    }

    //// ȷ�ϸ��в���
    //if (QMessageBox::question(this, "ȷ��",
    //    "ȷ��Ҫ���˲�Ʒ��¼����ΪOK��",
    //    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    //    return;
    //}

    // ����ģ������
    m_model->setData(m_model->index(selectedRow, resultCol), "OK");
    m_model->setData(m_model->index(selectedRow, result2DCol), "OK");
    m_model->setData(m_model->index(selectedRow, result3DCol), "OK");
    m_model->setData(m_model->index(selectedRow, recheckCol), "Yes");

    //// �ύ�޸ĵ����ݿ�
    //if (!m_model->submitAll()) {
    //    QMessageBox::critical(this, "����", "���ݿ����ʧ��:\n" + m_model->lastError().text());
    //    m_model->revertAll();
    //}
    //else {
    //    QMessageBox::information(this, "�ɹ�", "���в����ɹ����");
    //    // ��ѡ��ˢ����ͼ��ʾ
    //    m_model->select();
    //}
    return;
}

//����NG
void qg_QSLiteWidget::on_pushButton_checkNG_clicked()
{
    // ȷ����ǰ��ʾ���ǲ�Ʒ���ݱ�
    QString currentTable = ui->comboBoxTables->currentText();
    if (currentTable != "��Ʒ����") {
        QMessageBox::warning(this, "��ʾ", "���ڲ�Ʒ���ݱ��н��в���");
        return;
    }

    // ��ȡ��ǰѡ����
    QModelIndex currentIndex = ui->tableView_Sql->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "��ʾ", "��ѡ��Ҫ���еĲ�Ʒ��¼");
        return;
    }
    int selectedRow = currentIndex.row();

    // ��ȡ������
    int resultCol = m_model->fieldIndex("���");
    int result2DCol = m_model->fieldIndex("2D���");
    int result3DCol = m_model->fieldIndex("3D���");
    int recheckCol = m_model->fieldIndex("����");

    // �����������Ч��
    if (resultCol < 0 || result2DCol < 0 || result3DCol < 0 || recheckCol < 0) {
        QMessageBox::critical(this, "����", "���ݿ�����Ϣ��ƥ��");
        return;
    }

    //// ȷ�ϸ��в���
    //if (QMessageBox::question(this, "ȷ��",
    //    "ȷ��Ҫ���˲�Ʒ��¼����ΪOK��",
    //    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    //    return;
    //}

    // ����ģ������
    m_model->setData(m_model->index(selectedRow, resultCol), "NG");
    m_model->setData(m_model->index(selectedRow, result2DCol), "NG");
    m_model->setData(m_model->index(selectedRow, result3DCol), "NG");
    m_model->setData(m_model->index(selectedRow, recheckCol), "Yes");

    //// �ύ�޸ĵ����ݿ�
    //if (!m_model->submitAll()) {
    //    QMessageBox::critical(this, "����", "���ݿ����ʧ��:\n" + m_model->lastError().text());
    //    m_model->revertAll();
    //}
    //else {
    //    QMessageBox::information(this, "�ɹ�", "���в����ɹ����");
    //    // ��ѡ��ˢ����ͼ��ʾ
    //    m_model->select();
    //}
    return;
}

//NGƤ���˶�
void qg_QSLiteWidget::on_pushButton_ngBeltMove_pressed()
{
    LSM->setDO(LS_DO::ED_DO_NGBeltStart, 1);
    m_stopNGTimer.start(50);
}

//NGƤ��ֹͣ�˶�
void qg_QSLiteWidget::on_pushButton_ngBeltMove_released()
{
    LSM->setDO(LS_DO::ED_DO_NGBeltStart, 0);
    m_stopNGTimer.stop();
}

//��ʱ�����ֹͣNGƤ��
void qg_QSLiteWidget::stopNGBelt()
{
    if (LSM->getDI(LS_DI::ED_DI_NGBeltFullDetect))
    {
        //���ϣ�ֹͣƤ��
        LSM->setDO(LS_DO::ED_DO_NGBeltStart, 0);
    }
}

//������������
void qg_QSLiteWidget::on_pushButton_ExportData_clicked()
{
    // ����Ƿ���ѡ�е����ڱ�
    int selectedRow = ui->tableWidget_Date->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "��ʾ", "�����������б���ѡ��Ҫ���������ݱ�");
        return;
    }

    // ��ȡѡ�е����ڱ���
    QTableWidgetItem* dateItem = ui->tableWidget_Date->item(selectedRow, 0);
    if (!dateItem) {
        qWarning() << "�������������";
        return;
    }
    QString tableName = dateItem->text();

    // ʹ�ñ�����ΪĬ���ļ���
    QString defaultFileName = tableName + ".csv";

    // ���ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("����CSV�ļ�"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv)")
    );

    // ����û��Ƿ�ȡ���˲���
    if (filePath.isEmpty()) {
        return;
    }

    // ȷ���ļ���չ����.csv
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "����", "�޷������ļ�: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    try {
        // ��ѯ����������ݣ�����ID�У�
        QString queryStr = "SELECT * FROM `" + tableName + "`";
        QSqlQuery query;
        if (!query.exec(queryStr)) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        QSqlRecord record = query.record();

        // д���ͷ����1��ʼ������ID�У�
        for (int col = 1; col < record.count(); ++col) {
            out << "\"" << record.fieldName(col) << "\"";
            if (col < record.count() - 1) {
                out << ",";
            }
        }
        out << "\n";

        // д��������
        while (query.next()) {
            for (int col = 1; col < record.count(); ++col) {
                QString value = query.value(col).toString();

                // ����������Ż����ŵ�ֵ
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
        //QMessageBox::information(this, "�ɹ�", "�����ѳɹ�������:\n" + filePath);
    }
    catch (const std::exception& e) {
        file.close();
        QFile::remove(filePath);
        QMessageBox::critical(this, "����", "����ʧ��: " + QString::fromStdString(e.what()));
    }
}

//��������ѱ���ԭͼ
void qg_QSLiteWidget::on_pushButton_ClearOriginal_clicked()
{
    if (QMessageBox::question(this, "ȷ��",
        "ȷ��Ҫ���ԭͼ��")
        == QMessageBox::Yes)
    {
        QString folderPath = QCoreApplication::applicationDirPath() + "/images/Original";
        clearSelectImageFolder(folderPath);
    }
}

//˫����ͼƬ
void qg_QSLiteWidget::onTableViewDoubleClicked()
{
    if (ui->comboBoxTables->currentText() != "��Ʒ����") return;
    on_pushButton_openPhoto_clicked();
}