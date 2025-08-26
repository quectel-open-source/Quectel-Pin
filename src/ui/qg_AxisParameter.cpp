#include "qg_AxisParameter.h"
#include <QtGui/qstandarditemmodel.h>
#include "rs_motion.h"
#include "StatusDelegate.h"
#include "SwitchDelegate.h"
#include <qdebug.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qboxlayout.h>
#include <QTimer>

qg_AxisParameter::qg_AxisParameter(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
  
    initUI();
    initAxisStyle();
    switch (LSM->m_motionVersion)
    {
    case ENUM_MOTIONVERSION::LS_Pulse:
    case ENUM_MOTIONVERSION::LS_Pulse_1000:
        initDIStyle();
        initDOStyle();
        break;
    case ENUM_MOTIONVERSION::LS_EtherCat:
        initDIStyle_EtherCAT();
        initDOStyle_EtherCAT();
        break;
    default:
        break;
    }

 
    initParamerStyle();
    m_timerDI = new QTimer(this);
    connect(m_timerDI, &QTimer::timeout, this, &qg_AxisParameter::onTimerDITimeout);
    m_timerDO = new QTimer(this);
    connect(m_timerDO, &QTimer::timeout, this, &qg_AxisParameter::onTimerDOTimeout);
}

qg_AxisParameter::~qg_AxisParameter()
{}

//����������ʼ��
void qg_AxisParameter::initAxisStyle()
{
    // ���������� QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(15);  // ��������

    // ���ñ�ͷ��ǩ
    QStringList headers;
    headers << tr("Axis Name")
        << tr("Card Number")
        << tr("Axis Number")
        << tr("Initial Position (mm)")
        << tr("Minimum Travel (mm)")
        << tr("Maximum Travel (mm)")
        << tr("Homing Direction")
        << tr("Pulse Equivalent")
        << tr("Minimum Speed (mm/s)")
        << tr("Medium Speed (mm/s)")
        << tr("Maximum Speed (mm/s)")
        << tr("Acceleration Time (s)")
        << tr("Deceleration Time (s)")
        << tr("S Time (s)")
        << tr("Emergency Stop IO");


    // ���ñ�ͷ
    model->setHorizontalHeaderLabels(headers);

    for (const QString& axisName : LS_AxisName::allAxis) {
        // ʹ�� find ��������Ƿ������ m_Axis ��
        auto it = LSM->m_Axis.find(axisName);
        if (it == LSM->m_Axis.end()) {
            continue; // ���δ�ҵ���������ǰ��
        }

        // ��ȡ��ǰ�������
        const rs_motion::Axis_Config& axisConfig = it->second;

        // ����һ������
        QList<QStandardItem*> rowItems;

        // 1. ����
        QStandardItem* axisItem = new QStandardItem(axisName);
        axisItem->setEditable(false); // ����Ϊ���ɱ༭
        rowItems.append(axisItem);

        // 2. ����
        rowItems.append(new QStandardItem(QString::number(axisConfig.card)));

        // 3. ���
        rowItems.append(new QStandardItem(QString::number(axisConfig.axisNum)));

        // 4. ��ʼλ
        rowItems.append(new QStandardItem(QString::number(axisConfig.initPos)));

        // 5. ��С�г�
        rowItems.append(new QStandardItem(QString::number(axisConfig.minTravel)));

        // 6. ����г�
        rowItems.append(new QStandardItem(QString::number(axisConfig.maxTravel)));

        // 7. ���㷽��
        rowItems.append(new QStandardItem(QString::number(axisConfig.homeDir)));

        // 8. ���嵱��
        rowItems.append(new QStandardItem(QString::number(axisConfig.unit)));

        // 9. ��С��
        rowItems.append(new QStandardItem(QString::number(axisConfig.minSpeed)));

        // 10. ����
        rowItems.append(new QStandardItem(QString::number(axisConfig.midSpeed)));

        // 11. �����
        rowItems.append(new QStandardItem(QString::number(axisConfig.maxSpeed)));

        // 12. ����ʱ��
        rowItems.append(new QStandardItem(QString::number(axisConfig.accTime)));

        // 13. ����ʱ��
        rowItems.append(new QStandardItem(QString::number(axisConfig.decTime)));

        // 14. Sʱ��
        rowItems.append(new QStandardItem(QString::number(axisConfig.sTime)));

        // 15. ��ͣIO
        rowItems.append(new QStandardItem(QString::number(axisConfig.stopIO)));

        // ���һ�����ݵ�ģ��
        model->appendRow(rowItems);
    }


    // ��ģ�����ø� QTableView
    ui.tableView_AxisParameter->setModel(model);

    //// ���ñ�ͷ�������ݵ������
    //ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // ���ñ�ͷ����Ӧ��С
    ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_AxisParameter->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableView_AxisParameter->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������
    // �� dataChanged �źŵ��ۺ���
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::onTableDataChanged);
}

 //DI ҳ���ʼ��
void qg_AxisParameter::initDIStyle()
{
    // ���������� QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(5);  // ��������

    // ���ñ�ͷ��ǩ
    QStringList headers;
    headers << tr("����")
        << tr("����")
        << tr("CAN�ڵ�")
        << tr("ͨ��")
        << tr("״̬");


    // ���ñ�ͷ
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DI::allDI)
    {
        QList<QStandardItem*> rowItems;

        // ��ȡ DI ������
        rs_motion::DI_Config diConfig;
        if (LSM->m_DI.find(name) != LSM->m_DI.end()) {
            diConfig = LSM->m_DI[name];  // ��������ã�ʹ�����õ�ֵ
        }

        // 1. DI ����
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // ���Ʋ��ɱ༭
        rowItems.append(nameItem);

        // 2. ����
        QStandardItem* cardItem = new QStandardItem(QString::number(diConfig.card));
        rowItems.append(cardItem);

        // 3. CAN�ڵ�
        QStandardItem* canItem = new QStandardItem(QString::number(diConfig.can));
        rowItems.append(canItem);

        // 4. ͨ��
        QStandardItem* channelItem = new QStandardItem(QString::number(diConfig.channel));
        rowItems.append(channelItem);

        // 5. ״̬
        QStandardItem* stateItem = new QStandardItem(QString::number(diConfig.state));
        rowItems.append(stateItem);

        // ���һ�е����ģ��
        model->appendRow(rowItems);
    }

    // ��ģ�����ø� QTableView
    ui.tableView_DIConfig->setModel(model);

    //// ���ñ�ͷ�������ݵ������
    //ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // ���ñ�ͷ����Ӧ��С
    ui.tableView_DIConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ������ͷ
    ui.tableView_DIConfig->verticalHeader()->setVisible(false);
    ui.tableView_DIConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableView_DIConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������
    // �����Զ���ĵ���ʾ��״̬��
    StatusDelegate* statusDelegate = new StatusDelegate(this);
    ui.tableView_DIConfig->setItemDelegateForColumn(4, statusDelegate);
    // �� dataChanged �źŵ��ۺ���
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DIConfig_DataChanged);
}

// DO ҳ���ʼ��
void qg_AxisParameter::initDOStyle()
{
    // ���������� QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(5);  // ��������

    // ���ñ�ͷ��ǩ
    QStringList headers;
    headers << tr("����")
        << tr("����")
        << tr("CAN�ڵ�")
        << tr("ͨ��")
        << tr("��Ч��ƽ")
        << tr("״̬");

    // ���ñ�ͷ
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // ��ȡ DO ������
        rs_motion::DO_Config doConfig;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            doConfig = LSM->m_DO[name];  // ��������ã�ʹ�����õ�ֵ
        }

        // 1. DO ����
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // ���Ʋ��ɱ༭
        rowItems.append(nameItem);

        // 2. ����
        QStandardItem* cardItem = new QStandardItem(QString::number(doConfig.card));
        rowItems.append(cardItem);

        // 3. CAN�ڵ�
        QStandardItem* canItem = new QStandardItem(QString::number(doConfig.can));
        rowItems.append(canItem);

        // 4. ͨ��
        QStandardItem* channelItem = new QStandardItem(QString::number(doConfig.channel));
        rowItems.append(channelItem);

        // 5. ��Ч��ƽ
        QStandardItem* levelItem = new QStandardItem(QString::number(doConfig.level));
        rowItems.append(levelItem);

        // 6. ״̬��ִ�У�
        QStandardItem* stateItem = new QStandardItem(QString::number(0));
        rowItems.append(stateItem);

        // ���һ�е����ģ��
        model->appendRow(rowItems);
    }

    // ��ģ�����ø� QTableView
    ui.tableView_DOConfig->setModel(model);

    // ���ñ�ͷ����Ӧ��С
    ui.tableView_DOConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ������ͷ
    ui.tableView_DOConfig->verticalHeader()->setVisible(false);
    ui.tableView_DOConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableView_DOConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������

    // �����Զ���ί�и�״̬��
    SwitchDelegate* switchDelegate = new SwitchDelegate(this);
    ui.tableView_DOConfig->setItemDelegateForColumn(5, switchDelegate);

    // �� dataChanged �źŵ��ۺ���
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DOConfig_DataChanged);
}

void qg_AxisParameter::onTableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // ��ȡ�޸ĵ��к���
    int row = topLeft.row();
    int column = topLeft.column();

    // ��ȡ�޸ĺ��ֵ
    QVariant newValue = topLeft.data(Qt::EditRole);

    // ��ȡ����
    QString axisName = ui.tableView_AxisParameter->model()->index(row, 0).data().toString();

    // ��������Ƿ������������
    auto it = LSM->m_Axis.find(axisName);
    if (it == LSM->m_Axis.end()) {
        return; // ��������������� m_Axis �У�ֱ�ӷ���
    }

    // ��ȡ��ǰ�����������
    rs_motion::Axis_Config& axisConfig = it->second;

    // �����кŸ��¶�Ӧ��������
    switch (column) {
    case 1:  // ����
        axisConfig.card = newValue.toInt();
        break;
    case 2:  // ���
        axisConfig.axisNum = newValue.toInt();
        break;
    case 3:  // ��ʼλ
        axisConfig.initPos = newValue.toDouble();
        break;
    case 4:  // ��С�г�
        axisConfig.minTravel = newValue.toDouble();
        break;
    case 5:  // ����г�
        axisConfig.maxTravel = newValue.toDouble();
        break;
    case 6:  // ���㷽��
        axisConfig.homeDir = newValue.toInt();
        break;
    case 7:  // ���嵱��
        axisConfig.unit = newValue.toDouble();
        break;
    case 8:  // ��С��
        axisConfig.minSpeed = newValue.toDouble();
        break;
    case 9:  // ����
        axisConfig.midSpeed = newValue.toDouble();
        break;
    case 10:  // �����
        axisConfig.maxSpeed = newValue.toDouble();
        break;
    case 11:  // ����ʱ��
        axisConfig.accTime = newValue.toDouble();
        break;
    case 12:  // ����ʱ��
        axisConfig.decTime = newValue.toDouble();
        break;
    case 13:  // Sʱ��
        axisConfig.sTime = newValue.toDouble();
        break;
    case 14:  // ��ͣIO
        axisConfig.stopIO = newValue.toInt();
        break;
    default:
        break;
    }
}

void qg_AxisParameter::on_tableView_DIConfig_DataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // ��ȡ�޸ĵ��к���
    int row = topLeft.row();
    int column = topLeft.column();

    // ��ȡ�޸ĺ��ֵ
    QVariant newValue = topLeft.data(Qt::EditRole);

    // ��ȡDI��
    QString axisName = ui.tableView_DIConfig->model()->index(row, 0).data().toString();

    // ��������Ƿ������������
    auto it = LSM->m_DI.find(axisName);
    if (it == LSM->m_DI.end()) {
        return; // ��������������� m_Axis �У�ֱ�ӷ���
    }

    // ��ȡ��ǰ�����������
    rs_motion::DI_Config& axisConfig = it->second;

    switch (LSM->m_motionVersion)
    {
    case ENUM_MOTIONVERSION::LS_Pulse:
    case ENUM_MOTIONVERSION::LS_Pulse_1000:
        // �����кŸ��¶�Ӧ��������
        switch (column) {
        case 1:  // ����
            axisConfig.card = newValue.toInt();
            break;
        case 2:  // CAN�ڵ�
            axisConfig.can = newValue.toInt();
            break;
        case 3:  // ͨ��
            axisConfig.channel = newValue.toInt();
            break;
        case 4:  // ״̬
            axisConfig.state = newValue.toInt();
            // ����޸ĵ���״̬�У�ˢ��״̬����ʾ
            ui.tableView_DIConfig->viewport()->update();

            break;
        default:
            break;
        }
        break;
    case ENUM_MOTIONVERSION::LS_EtherCat:
        // �����кŸ��¶�Ӧ��������
        switch (column) {
        case 1:  // ����
            axisConfig.card = newValue.toInt();
            break;
        case 2:  // ͨ��
            axisConfig.channel = newValue.toInt();
            break;
        case 3:  // ״̬
            axisConfig.state = newValue.toInt();
            // ����޸ĵ���״̬�У�ˢ��״̬����ʾ
            ui.tableView_DIConfig->viewport()->update();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
   
}

void qg_AxisParameter::on_tableView_DOConfig_DataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    int row = topLeft.row();
    int column = topLeft.column();

    QString doName = ui.tableView_DOConfig->model()->data(ui.tableView_DOConfig->model()->index(row, 0)).toString();

    if (LSM->m_DO.find(doName) != LSM->m_DO.end()) {
        rs_motion::DO_Config& doConfig = LSM->m_DO[doName];
        QVariant newValue = topLeft.data(Qt::EditRole);

        switch (LSM->m_motionVersion)
        {
        case ENUM_MOTIONVERSION::LS_Pulse:
        case ENUM_MOTIONVERSION::LS_Pulse_1000:
            // �����кŸ��¶�Ӧ��������
            switch (column) {
            case 1:
                doConfig.card = newValue.toInt();
                break;
            case 2:
                doConfig.can = newValue.toInt();
                break;
            case 3:
                doConfig.channel = newValue.toInt();
                break;
            case 4:
                doConfig.level = newValue.toInt();
                break;
            case 5:
                LSM->setDO(doName, newValue.toInt());
                break;
            default:
                break;
            }
            break;
        case ENUM_MOTIONVERSION::LS_EtherCat:
            // �����кŸ��¶�Ӧ��������
            switch (column) {
            case 1:
                doConfig.card = newValue.toInt();
                break;
            case 2:
                doConfig.channel = newValue.toInt();
                break;
            case 3:
                doConfig.level = newValue.toInt();
                break;
            case 4:
                LSM->setDO(doName, newValue.toInt());
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

//DI ui����
void qg_AxisParameter::updateDIConfigUI()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_DIConfig->model());

    if (!model) return;

    const int rowCount = model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QStandardItem* item = model->item(row, 0);
        if (item) {
           auto diName = item->text();
           // ��ȡ DI ������
           rs_motion::DI_Config config;
           if (LSM->m_DI.find(diName) != LSM->m_DI.end()) {
               config = LSM->m_DI[diName];
           }

           switch (LSM->m_motionVersion)
           {
           case ENUM_MOTIONVERSION::LS_Pulse:
           case ENUM_MOTIONVERSION::LS_Pulse_1000:
           {
               QModelIndex stateIndex = model->index(row, 4);  // ״̬��
               model->setData(stateIndex, config.state);
               break;
           }
           case ENUM_MOTIONVERSION::LS_EtherCat:
           {
               QModelIndex stateIndex = model->index(row, 3);  // ״̬��
               model->setData(stateIndex, config.state);
               break;
           }
           default:
               break;
           }
        }
    }
}

//DO ui����
void qg_AxisParameter::updateDOConfigUI()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_DOConfig->model());

    if (!model) return;

    // ���� m_DO ��ϣ������ģ������
    int row = 0;
    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // ��ȡ DO ������
        rs_motion::DO_Config config;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            config = LSM->m_DO[name];  // ��������ã�ʹ�����õ�ֵ
        }

        QModelIndex stateIndex;  // ״̬��
        switch (LSM->m_motionVersion)
        {
        case ENUM_MOTIONVERSION::LS_Pulse:
        case ENUM_MOTIONVERSION::LS_Pulse_1000:
        {
            stateIndex = model->index(row, 5);  // ״̬��
            break;
        }
        case ENUM_MOTIONVERSION::LS_EtherCat:
        {
            stateIndex = model->index(row, 4);  // ״̬��
            break;
        }
        default:
            break;
        }


        //if (config.level == 0)
        //{
        //    //�͵�ƽ��Ч
        //    if (config.state == 0)
        //        model->setData(stateIndex, 1);
        //    else
        //        model->setData(stateIndex, 0);
        //}
        //else
        //{
        //    //�ߵ�ƽ��Ч
        //    if (config.state == 1)
        //        model->setData(stateIndex, 1);
        //    else
        //        model->setData(stateIndex, 0);
        //}

        //ֱ����״̬Ϊ��׼
        if (config.state == 1)
            model->setData(stateIndex, 1);
        else
            model->setData(stateIndex, 0);
        row++;
    }
}

//ˢ��DO
void qg_AxisParameter::on_pushButton_upDate_clicked()
{
    LSM->getAllDO();
    updateDOConfigUI();
}

//������������忨
void qg_AxisParameter::on_pushButton_Download_clicked()
{
    for (auto axisName : LS_AxisName::allAxis)
    {
        rs_motion::Axis_Config config = LSM->m_Axis[axisName];
        LSM->WriteAxisParm(axisName);
    }
}

//DIʵʱ��ذ�ť
void qg_AxisParameter::on_pushButton_DiReading_clicked()
{
    m_isDiReading = !m_isDiReading;
    // ����״̬������ֹͣ��ʱ��
    if (m_isDiReading) {
        m_timerDI->start(500);
        //qDebug() << "Timer started.";
        ui.pushButton_DiReading->setText(QString::fromUtf8("��ؿ�"));
    }
    else {
        m_timerDI->stop();
        //qDebug() << "Timer stopped.";
        ui.pushButton_DiReading->setText(QString::fromUtf8("��ع�"));
    }
}

// DI��ʱ���
void qg_AxisParameter::onTimerDITimeout()
{
    updateDIConfigUI();
}

// DO��ʱ���
void qg_AxisParameter::onTimerDOTimeout()
{
    LSM->getAllDO();
    updateDOConfigUI();
}

//DOʵʱ��ذ�ť
void qg_AxisParameter::on_pushButton_DOReading_clicked()
{
    m_isDoReading = !m_isDoReading;
    // ����״̬������ֹͣ��ʱ��
    if (m_isDoReading) {
        m_timerDO->start(500);
        //qDebug() << "Timer started.";
        ui.pushButton_DOReading->setText(QString::fromUtf8("��ؿ�"));
    }
    else {
        m_timerDO->stop();
        //qDebug() << "Timer stopped.";
        ui.pushButton_DOReading->setText(QString::fromUtf8("��ع�"));
    }
}

void qg_AxisParameter::on_pushButton_ParaSave_clicked()
{
    //2d
    LSM->m_Paramer.BuffetTime2D = ui.lineEdit_2Daction_1->text().toInt();
    LSM->m_Paramer.DelayTriggerTime2D = ui.lineEdit_2Daction_2->text().toInt();
    LSM->m_Paramer.HoldTime2D = ui.lineEdit_2Daction_3->text().toInt();
    //3d
    LSM->m_Paramer.BuffetTime3D = ui.lineEdit_3Daction_1->text().toInt();
    LSM->m_Paramer.DelayTriggerTime3D = ui.lineEdit_3Daction_2->text().toInt();
    LSM->m_Paramer.HoldTime3D = ui.lineEdit_3Daction_3->text().toInt();
    LSM->m_Paramer.ScanRow = ui.lineEdit_3Dscan->text().toInt();
    //����
    LSM->m_Paramer.suctionTime = ui.lineEdit_feedTime->text().toInt();
    //����
    LSM->m_Paramer.unLoadTime = ui.lineEdit_unLoadingTime->text().toInt();
    //mes url
    LSM->m_Paramer.mesUrl = ui.lineEdit_mesUrl->text();
    //Modbus TCP��IP
    LSM->m_Paramer.modbusTcpIp = ui.lineEdit_modbusIP->text();
    //mes CSV
    LSM->m_Paramer.mesCSV = ui.lineEdit_mesCSV->text();
    //mes NGͼƬ����·��
    LSM->m_Paramer.ngPicPath = ui.lineEdit_ngPicPath->text();

    //�������в���
    LSM->saveMotionConfig();
}

//����ҳ���ʼ��
void qg_AxisParameter::initParamerStyle()
{
    // 2D
    ui.lineEdit_2Daction_1->setText(QString::number(LSM->m_Paramer.BuffetTime2D));
    ui.lineEdit_2Daction_2->setText(QString::number(LSM->m_Paramer.DelayTriggerTime2D));
    ui.lineEdit_2Daction_3->setText(QString::number(LSM->m_Paramer.HoldTime2D));
    // 3D
    ui.lineEdit_3Daction_1->setText(QString::number(LSM->m_Paramer.BuffetTime3D));
    ui.lineEdit_3Daction_2->setText(QString::number(LSM->m_Paramer.DelayTriggerTime3D));
    ui.lineEdit_3Daction_3->setText(QString::number(LSM->m_Paramer.HoldTime3D));
    ui.lineEdit_3Dscan->setText(QString::number(LSM->m_Paramer.ScanRow));
    //����
    ui.lineEdit_feedTime->setText(QString::number(LSM->m_Paramer.suctionTime));
    //����
    ui.lineEdit_unLoadingTime->setText(QString::number(LSM->m_Paramer.unLoadTime));
    //mes url
    ui.lineEdit_mesUrl->setText(LSM->m_Paramer.mesUrl);
    //Modbus TCP��IP
    ui.lineEdit_modbusIP->setText(LSM->m_Paramer.modbusTcpIp);
    //mes CSV
    ui.lineEdit_mesCSV->setText(LSM->m_Paramer.mesCSV);
    //mes NGͼƬ����·��
    ui.lineEdit_ngPicPath->setText(LSM->m_Paramer.ngPicPath);
}


//ҳ��ui��ʼ��
void qg_AxisParameter::initUI()
{
    ui.pushButton_upDate->hide();
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
    case ENUM_VERSION::EdPin:
        ui.groupBox_Feeding->hide();
        break;
    case BtPin:
        ui.groupBox_3DParamer->hide();
        ui.groupBox_Feeding->hide();
        break;
    default:
        break;
    }
}

//����DI
void qg_AxisParameter::initDIStyle_EtherCAT()
{
    // ���������� QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(4);  // ��������

    // ���ñ�ͷ��ǩ
    QStringList headers;
    headers << tr("����")
        << tr("����")
        << tr("ͨ��")
        << tr("״̬");


    // ���ñ�ͷ
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DI::allDI)
    {
        QList<QStandardItem*> rowItems;

        // ��ȡ DI ������
        rs_motion::DI_Config diConfig;
        if (LSM->m_DI.find(name) != LSM->m_DI.end()) {
            diConfig = LSM->m_DI[name];  // ��������ã�ʹ�����õ�ֵ
        }

        // 1. DI ����
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // ���Ʋ��ɱ༭
        rowItems.append(nameItem);

        // 2. ����
        QStandardItem* cardItem = new QStandardItem(QString::number(diConfig.card));
        rowItems.append(cardItem);

        // 3. ͨ��
        QStandardItem* channelItem = new QStandardItem(QString::number(diConfig.channel));
        rowItems.append(channelItem);

        // 4. ״̬
        QStandardItem* stateItem = new QStandardItem(QString::number(diConfig.state));
        rowItems.append(stateItem);

        // ���һ�е����ģ��
        model->appendRow(rowItems);
    }

    // ��ģ�����ø� QTableView
    ui.tableView_DIConfig->setModel(model);

    // ���ñ�ͷ����Ӧ��С
    ui.tableView_DIConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ������ͷ
    ui.tableView_DIConfig->verticalHeader()->setVisible(false);
    ui.tableView_DIConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableView_DIConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������
    // �����Զ���ĵ���ʾ��״̬��
    StatusDelegate* statusDelegate = new StatusDelegate(this);
    ui.tableView_DIConfig->setItemDelegateForColumn(3, statusDelegate);
    // �� dataChanged �źŵ��ۺ���
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DIConfig_DataChanged);
}

//����DO
void qg_AxisParameter::initDOStyle_EtherCAT()
{
    // ���������� QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(4);  // ��������

    // ���ñ�ͷ��ǩ
    QStringList headers;
    headers << tr("����")
        << tr("����")
        << tr("ͨ��")
        << tr("��Ч��ƽ")
        << tr("״̬");

    // ���ñ�ͷ
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // ��ȡ DO ������
        rs_motion::DO_Config doConfig;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            doConfig = LSM->m_DO[name];  // ��������ã�ʹ�����õ�ֵ
        }

        // 1. DO ����
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // ���Ʋ��ɱ༭
        rowItems.append(nameItem);

        // 2. ����
        QStandardItem* cardItem = new QStandardItem(QString::number(doConfig.card));
        rowItems.append(cardItem);

        // 3. ͨ��
        QStandardItem* channelItem = new QStandardItem(QString::number(doConfig.channel));
        rowItems.append(channelItem);

        // 4. ��Ч��ƽ
        QStandardItem* levelItem = new QStandardItem(QString::number(doConfig.level));
        rowItems.append(levelItem);

        // 5. ״̬��ִ�У�
        QStandardItem* stateItem = new QStandardItem(QString::number(0));
        rowItems.append(stateItem);

        // ���һ�е����ģ��
        model->appendRow(rowItems);
    }

    // ��ģ�����ø� QTableView
    ui.tableView_DOConfig->setModel(model);

    // ���ñ�ͷ����Ӧ��С
    ui.tableView_DOConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ������ͷ
    ui.tableView_DOConfig->verticalHeader()->setVisible(false);
    ui.tableView_DOConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableView_DOConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������

    // �����Զ���ί�и�״̬��
    SwitchDelegate* switchDelegate = new SwitchDelegate(this);
    ui.tableView_DOConfig->setItemDelegateForColumn(4, switchDelegate);

    // �� dataChanged �źŵ��ۺ���
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DOConfig_DataChanged);
}