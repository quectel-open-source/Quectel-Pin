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

//轴参数界面初始化
void qg_AxisParameter::initAxisStyle()
{
    // 创建并配置 QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(15);  // 设置列数

    // 设置表头标签
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


    // 设置表头
    model->setHorizontalHeaderLabels(headers);

    for (const QString& axisName : LS_AxisName::allAxis) {
        // 使用 find 检查轴名是否存在于 m_Axis 中
        auto it = LSM->m_Axis.find(axisName);
        if (it == LSM->m_Axis.end()) {
            continue; // 如果未找到，跳过当前轴
        }

        // 获取当前轴的配置
        const rs_motion::Axis_Config& axisConfig = it->second;

        // 创建一行数据
        QList<QStandardItem*> rowItems;

        // 1. 轴名
        QStandardItem* axisItem = new QStandardItem(axisName);
        axisItem->setEditable(false); // 设置为不可编辑
        rowItems.append(axisItem);

        // 2. 卡号
        rowItems.append(new QStandardItem(QString::number(axisConfig.card)));

        // 3. 轴号
        rowItems.append(new QStandardItem(QString::number(axisConfig.axisNum)));

        // 4. 初始位
        rowItems.append(new QStandardItem(QString::number(axisConfig.initPos)));

        // 5. 最小行程
        rowItems.append(new QStandardItem(QString::number(axisConfig.minTravel)));

        // 6. 最大行程
        rowItems.append(new QStandardItem(QString::number(axisConfig.maxTravel)));

        // 7. 回零方向
        rowItems.append(new QStandardItem(QString::number(axisConfig.homeDir)));

        // 8. 脉冲当量
        rowItems.append(new QStandardItem(QString::number(axisConfig.unit)));

        // 9. 最小速
        rowItems.append(new QStandardItem(QString::number(axisConfig.minSpeed)));

        // 10. 中速
        rowItems.append(new QStandardItem(QString::number(axisConfig.midSpeed)));

        // 11. 最大速
        rowItems.append(new QStandardItem(QString::number(axisConfig.maxSpeed)));

        // 12. 加速时间
        rowItems.append(new QStandardItem(QString::number(axisConfig.accTime)));

        // 13. 减速时间
        rowItems.append(new QStandardItem(QString::number(axisConfig.decTime)));

        // 14. S时间
        rowItems.append(new QStandardItem(QString::number(axisConfig.sTime)));

        // 15. 急停IO
        rowItems.append(new QStandardItem(QString::number(axisConfig.stopIO)));

        // 添加一行数据到模型
        model->appendRow(rowItems);
    }


    // 将模型设置给 QTableView
    ui.tableView_AxisParameter->setModel(model);

    //// 设置表头根据内容调整宽度
    //ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 设置表头自适应大小
    ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_AxisParameter->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableView_AxisParameter->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
    // 绑定 dataChanged 信号到槽函数
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::onTableDataChanged);
}

 //DI 页面初始化
void qg_AxisParameter::initDIStyle()
{
    // 创建并配置 QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(5);  // 设置列数

    // 设置表头标签
    QStringList headers;
    headers << tr("名称")
        << tr("卡号")
        << tr("CAN节点")
        << tr("通道")
        << tr("状态");


    // 设置表头
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DI::allDI)
    {
        QList<QStandardItem*> rowItems;

        // 获取 DI 的配置
        rs_motion::DI_Config diConfig;
        if (LSM->m_DI.find(name) != LSM->m_DI.end()) {
            diConfig = LSM->m_DI[name];  // 如果已配置，使用配置的值
        }

        // 1. DI 名称
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // 名称不可编辑
        rowItems.append(nameItem);

        // 2. 卡号
        QStandardItem* cardItem = new QStandardItem(QString::number(diConfig.card));
        rowItems.append(cardItem);

        // 3. CAN节点
        QStandardItem* canItem = new QStandardItem(QString::number(diConfig.can));
        rowItems.append(canItem);

        // 4. 通道
        QStandardItem* channelItem = new QStandardItem(QString::number(diConfig.channel));
        rowItems.append(channelItem);

        // 5. 状态
        QStandardItem* stateItem = new QStandardItem(QString::number(diConfig.state));
        rowItems.append(stateItem);

        // 添加一行到表格模型
        model->appendRow(rowItems);
    }

    // 将模型设置给 QTableView
    ui.tableView_DIConfig->setModel(model);

    //// 设置表头根据内容调整宽度
    //ui.tableView_AxisParameter->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 设置表头自适应大小
    ui.tableView_DIConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 隐藏行头
    ui.tableView_DIConfig->verticalHeader()->setVisible(false);
    ui.tableView_DIConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableView_DIConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
    // 设置自定义的灯显示给状态列
    StatusDelegate* statusDelegate = new StatusDelegate(this);
    ui.tableView_DIConfig->setItemDelegateForColumn(4, statusDelegate);
    // 绑定 dataChanged 信号到槽函数
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DIConfig_DataChanged);
}

// DO 页面初始化
void qg_AxisParameter::initDOStyle()
{
    // 创建并配置 QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(5);  // 设置列数

    // 设置表头标签
    QStringList headers;
    headers << tr("名称")
        << tr("卡号")
        << tr("CAN节点")
        << tr("通道")
        << tr("有效电平")
        << tr("状态");

    // 设置表头
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // 获取 DO 的配置
        rs_motion::DO_Config doConfig;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            doConfig = LSM->m_DO[name];  // 如果已配置，使用配置的值
        }

        // 1. DO 名称
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // 名称不可编辑
        rowItems.append(nameItem);

        // 2. 卡号
        QStandardItem* cardItem = new QStandardItem(QString::number(doConfig.card));
        rowItems.append(cardItem);

        // 3. CAN节点
        QStandardItem* canItem = new QStandardItem(QString::number(doConfig.can));
        rowItems.append(canItem);

        // 4. 通道
        QStandardItem* channelItem = new QStandardItem(QString::number(doConfig.channel));
        rowItems.append(channelItem);

        // 5. 有效电平
        QStandardItem* levelItem = new QStandardItem(QString::number(doConfig.level));
        rowItems.append(levelItem);

        // 6. 状态（执行）
        QStandardItem* stateItem = new QStandardItem(QString::number(0));
        rowItems.append(stateItem);

        // 添加一行到表格模型
        model->appendRow(rowItems);
    }

    // 将模型设置给 QTableView
    ui.tableView_DOConfig->setModel(model);

    // 设置表头自适应大小
    ui.tableView_DOConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 隐藏行头
    ui.tableView_DOConfig->verticalHeader()->setVisible(false);
    ui.tableView_DOConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableView_DOConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行

    // 设置自定义委托给状态列
    SwitchDelegate* switchDelegate = new SwitchDelegate(this);
    ui.tableView_DOConfig->setItemDelegateForColumn(5, switchDelegate);

    // 绑定 dataChanged 信号到槽函数
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DOConfig_DataChanged);
}

void qg_AxisParameter::onTableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // 获取修改的行和列
    int row = topLeft.row();
    int column = topLeft.column();

    // 获取修改后的值
    QVariant newValue = topLeft.data(Qt::EditRole);

    // 获取轴名
    QString axisName = ui.tableView_AxisParameter->model()->index(row, 0).data().toString();

    // 检查轴名是否存在于配置中
    auto it = LSM->m_Axis.find(axisName);
    if (it == LSM->m_Axis.end()) {
        return; // 如果轴名不存在于 m_Axis 中，直接返回
    }

    // 获取当前轴的配置引用
    rs_motion::Axis_Config& axisConfig = it->second;

    // 根据列号更新对应的配置项
    switch (column) {
    case 1:  // 卡号
        axisConfig.card = newValue.toInt();
        break;
    case 2:  // 轴号
        axisConfig.axisNum = newValue.toInt();
        break;
    case 3:  // 初始位
        axisConfig.initPos = newValue.toDouble();
        break;
    case 4:  // 最小行程
        axisConfig.minTravel = newValue.toDouble();
        break;
    case 5:  // 最大行程
        axisConfig.maxTravel = newValue.toDouble();
        break;
    case 6:  // 回零方向
        axisConfig.homeDir = newValue.toInt();
        break;
    case 7:  // 脉冲当量
        axisConfig.unit = newValue.toDouble();
        break;
    case 8:  // 最小速
        axisConfig.minSpeed = newValue.toDouble();
        break;
    case 9:  // 中速
        axisConfig.midSpeed = newValue.toDouble();
        break;
    case 10:  // 最大速
        axisConfig.maxSpeed = newValue.toDouble();
        break;
    case 11:  // 加速时间
        axisConfig.accTime = newValue.toDouble();
        break;
    case 12:  // 减速时间
        axisConfig.decTime = newValue.toDouble();
        break;
    case 13:  // S时间
        axisConfig.sTime = newValue.toDouble();
        break;
    case 14:  // 急停IO
        axisConfig.stopIO = newValue.toInt();
        break;
    default:
        break;
    }
}

void qg_AxisParameter::on_tableView_DIConfig_DataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // 获取修改的行和列
    int row = topLeft.row();
    int column = topLeft.column();

    // 获取修改后的值
    QVariant newValue = topLeft.data(Qt::EditRole);

    // 获取DI名
    QString axisName = ui.tableView_DIConfig->model()->index(row, 0).data().toString();

    // 检查轴名是否存在于配置中
    auto it = LSM->m_DI.find(axisName);
    if (it == LSM->m_DI.end()) {
        return; // 如果轴名不存在于 m_Axis 中，直接返回
    }

    // 获取当前轴的配置引用
    rs_motion::DI_Config& axisConfig = it->second;

    switch (LSM->m_motionVersion)
    {
    case ENUM_MOTIONVERSION::LS_Pulse:
    case ENUM_MOTIONVERSION::LS_Pulse_1000:
        // 根据列号更新对应的配置项
        switch (column) {
        case 1:  // 卡号
            axisConfig.card = newValue.toInt();
            break;
        case 2:  // CAN节点
            axisConfig.can = newValue.toInt();
            break;
        case 3:  // 通道
            axisConfig.channel = newValue.toInt();
            break;
        case 4:  // 状态
            axisConfig.state = newValue.toInt();
            // 如果修改的是状态列，刷新状态列显示
            ui.tableView_DIConfig->viewport()->update();

            break;
        default:
            break;
        }
        break;
    case ENUM_MOTIONVERSION::LS_EtherCat:
        // 根据列号更新对应的配置项
        switch (column) {
        case 1:  // 卡号
            axisConfig.card = newValue.toInt();
            break;
        case 2:  // 通道
            axisConfig.channel = newValue.toInt();
            break;
        case 3:  // 状态
            axisConfig.state = newValue.toInt();
            // 如果修改的是状态列，刷新状态列显示
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
            // 根据列号更新对应的配置项
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
            // 根据列号更新对应的配置项
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

//DI ui更新
void qg_AxisParameter::updateDIConfigUI()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_DIConfig->model());

    if (!model) return;

    const int rowCount = model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QStandardItem* item = model->item(row, 0);
        if (item) {
           auto diName = item->text();
           // 获取 DI 的配置
           rs_motion::DI_Config config;
           if (LSM->m_DI.find(diName) != LSM->m_DI.end()) {
               config = LSM->m_DI[diName];
           }

           switch (LSM->m_motionVersion)
           {
           case ENUM_MOTIONVERSION::LS_Pulse:
           case ENUM_MOTIONVERSION::LS_Pulse_1000:
           {
               QModelIndex stateIndex = model->index(row, 4);  // 状态列
               model->setData(stateIndex, config.state);
               break;
           }
           case ENUM_MOTIONVERSION::LS_EtherCat:
           {
               QModelIndex stateIndex = model->index(row, 3);  // 状态列
               model->setData(stateIndex, config.state);
               break;
           }
           default:
               break;
           }
        }
    }
}

//DO ui更新
void qg_AxisParameter::updateDOConfigUI()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_DOConfig->model());

    if (!model) return;

    // 遍历 m_DO 哈希表，更新模型数据
    int row = 0;
    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // 获取 DO 的配置
        rs_motion::DO_Config config;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            config = LSM->m_DO[name];  // 如果已配置，使用配置的值
        }

        QModelIndex stateIndex;  // 状态列
        switch (LSM->m_motionVersion)
        {
        case ENUM_MOTIONVERSION::LS_Pulse:
        case ENUM_MOTIONVERSION::LS_Pulse_1000:
        {
            stateIndex = model->index(row, 5);  // 状态列
            break;
        }
        case ENUM_MOTIONVERSION::LS_EtherCat:
        {
            stateIndex = model->index(row, 4);  // 状态列
            break;
        }
        default:
            break;
        }


        //if (config.level == 0)
        //{
        //    //低电平有效
        //    if (config.state == 0)
        //        model->setData(stateIndex, 1);
        //    else
        //        model->setData(stateIndex, 0);
        //}
        //else
        //{
        //    //高电平有效
        //    if (config.state == 1)
        //        model->setData(stateIndex, 1);
        //    else
        //        model->setData(stateIndex, 0);
        //}

        //直接以状态为标准
        if (config.state == 1)
            model->setData(stateIndex, 1);
        else
            model->setData(stateIndex, 0);
        row++;
    }
}

//刷新DO
void qg_AxisParameter::on_pushButton_upDate_clicked()
{
    LSM->getAllDO();
    updateDOConfigUI();
}

//下载轴参数到板卡
void qg_AxisParameter::on_pushButton_Download_clicked()
{
    for (auto axisName : LS_AxisName::allAxis)
    {
        rs_motion::Axis_Config config = LSM->m_Axis[axisName];
        LSM->WriteAxisParm(axisName);
    }
}

//DI实时监控按钮
void qg_AxisParameter::on_pushButton_DiReading_clicked()
{
    m_isDiReading = !m_isDiReading;
    // 根据状态启动或停止定时器
    if (m_isDiReading) {
        m_timerDI->start(500);
        //qDebug() << "Timer started.";
        ui.pushButton_DiReading->setText(QString::fromUtf8("监控开"));
    }
    else {
        m_timerDI->stop();
        //qDebug() << "Timer stopped.";
        ui.pushButton_DiReading->setText(QString::fromUtf8("监控关"));
    }
}

// DI定时监控
void qg_AxisParameter::onTimerDITimeout()
{
    updateDIConfigUI();
}

// DO定时监控
void qg_AxisParameter::onTimerDOTimeout()
{
    LSM->getAllDO();
    updateDOConfigUI();
}

//DO实时监控按钮
void qg_AxisParameter::on_pushButton_DOReading_clicked()
{
    m_isDoReading = !m_isDoReading;
    // 根据状态启动或停止定时器
    if (m_isDoReading) {
        m_timerDO->start(500);
        //qDebug() << "Timer started.";
        ui.pushButton_DOReading->setText(QString::fromUtf8("监控开"));
    }
    else {
        m_timerDO->stop();
        //qDebug() << "Timer stopped.";
        ui.pushButton_DOReading->setText(QString::fromUtf8("监控关"));
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
    //吸料
    LSM->m_Paramer.suctionTime = ui.lineEdit_feedTime->text().toInt();
    //放料
    LSM->m_Paramer.unLoadTime = ui.lineEdit_unLoadingTime->text().toInt();
    //mes url
    LSM->m_Paramer.mesUrl = ui.lineEdit_mesUrl->text();
    //Modbus TCP的IP
    LSM->m_Paramer.modbusTcpIp = ui.lineEdit_modbusIP->text();
    //mes CSV
    LSM->m_Paramer.mesCSV = ui.lineEdit_mesCSV->text();
    //mes NG图片保存路径
    LSM->m_Paramer.ngPicPath = ui.lineEdit_ngPicPath->text();

    //保存所有参数
    LSM->saveMotionConfig();
}

//参数页面初始化
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
    //吸料
    ui.lineEdit_feedTime->setText(QString::number(LSM->m_Paramer.suctionTime));
    //放料
    ui.lineEdit_unLoadingTime->setText(QString::number(LSM->m_Paramer.unLoadTime));
    //mes url
    ui.lineEdit_mesUrl->setText(LSM->m_Paramer.mesUrl);
    //Modbus TCP的IP
    ui.lineEdit_modbusIP->setText(LSM->m_Paramer.modbusTcpIp);
    //mes CSV
    ui.lineEdit_mesCSV->setText(LSM->m_Paramer.mesCSV);
    //mes NG图片保存路径
    ui.lineEdit_ngPicPath->setText(LSM->m_Paramer.ngPicPath);
}


//页面ui初始化
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

//总线DI
void qg_AxisParameter::initDIStyle_EtherCAT()
{
    // 创建并配置 QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(4);  // 设置列数

    // 设置表头标签
    QStringList headers;
    headers << tr("名称")
        << tr("卡号")
        << tr("通道")
        << tr("状态");


    // 设置表头
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DI::allDI)
    {
        QList<QStandardItem*> rowItems;

        // 获取 DI 的配置
        rs_motion::DI_Config diConfig;
        if (LSM->m_DI.find(name) != LSM->m_DI.end()) {
            diConfig = LSM->m_DI[name];  // 如果已配置，使用配置的值
        }

        // 1. DI 名称
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // 名称不可编辑
        rowItems.append(nameItem);

        // 2. 卡号
        QStandardItem* cardItem = new QStandardItem(QString::number(diConfig.card));
        rowItems.append(cardItem);

        // 3. 通道
        QStandardItem* channelItem = new QStandardItem(QString::number(diConfig.channel));
        rowItems.append(channelItem);

        // 4. 状态
        QStandardItem* stateItem = new QStandardItem(QString::number(diConfig.state));
        rowItems.append(stateItem);

        // 添加一行到表格模型
        model->appendRow(rowItems);
    }

    // 将模型设置给 QTableView
    ui.tableView_DIConfig->setModel(model);

    // 设置表头自适应大小
    ui.tableView_DIConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 隐藏行头
    ui.tableView_DIConfig->verticalHeader()->setVisible(false);
    ui.tableView_DIConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableView_DIConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
    // 设置自定义的灯显示给状态列
    StatusDelegate* statusDelegate = new StatusDelegate(this);
    ui.tableView_DIConfig->setItemDelegateForColumn(3, statusDelegate);
    // 绑定 dataChanged 信号到槽函数
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DIConfig_DataChanged);
}

//总线DO
void qg_AxisParameter::initDOStyle_EtherCAT()
{
    // 创建并配置 QStandardItemModel
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(4);  // 设置列数

    // 设置表头标签
    QStringList headers;
    headers << tr("名称")
        << tr("卡号")
        << tr("通道")
        << tr("有效电平")
        << tr("状态");

    // 设置表头
    model->setHorizontalHeaderLabels(headers);

    for (const auto& name : LS_DO::allDO)
    {
        QList<QStandardItem*> rowItems;

        // 获取 DO 的配置
        rs_motion::DO_Config doConfig;
        if (LSM->m_DO.find(name) != LSM->m_DO.end()) {
            doConfig = LSM->m_DO[name];  // 如果已配置，使用配置的值
        }

        // 1. DO 名称
        QStandardItem* nameItem = new QStandardItem(name);
        nameItem->setEditable(false);  // 名称不可编辑
        rowItems.append(nameItem);

        // 2. 卡号
        QStandardItem* cardItem = new QStandardItem(QString::number(doConfig.card));
        rowItems.append(cardItem);

        // 3. 通道
        QStandardItem* channelItem = new QStandardItem(QString::number(doConfig.channel));
        rowItems.append(channelItem);

        // 4. 有效电平
        QStandardItem* levelItem = new QStandardItem(QString::number(doConfig.level));
        rowItems.append(levelItem);

        // 5. 状态（执行）
        QStandardItem* stateItem = new QStandardItem(QString::number(0));
        rowItems.append(stateItem);

        // 添加一行到表格模型
        model->appendRow(rowItems);
    }

    // 将模型设置给 QTableView
    ui.tableView_DOConfig->setModel(model);

    // 设置表头自适应大小
    ui.tableView_DOConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 隐藏行头
    ui.tableView_DOConfig->verticalHeader()->setVisible(false);
    ui.tableView_DOConfig->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableView_DOConfig->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行

    // 设置自定义委托给状态列
    SwitchDelegate* switchDelegate = new SwitchDelegate(this);
    ui.tableView_DOConfig->setItemDelegateForColumn(4, switchDelegate);

    // 绑定 dataChanged 信号到槽函数
    connect(model, &QStandardItemModel::dataChanged, this, &qg_AxisParameter::on_tableView_DOConfig_DataChanged);
}