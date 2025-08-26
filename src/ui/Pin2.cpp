#include "Pin2.h"
#include <QScrollArea>
#include "qg_Formula.h"
#include "rs_motion.h"
#include <QTimer>
#include "pinLabel.h"
#include <unordered_set>
#include "qg_QSLiteWidget.h"
#include "qg_JogButton_ED.h"
#include "qg_VersionDialog.h"
#include "qg_userChange.h"
#include "QGraphicsViews.h"
#include "ImageTypeConvert.h"
#include "JsonConfigManager.h"
#include "LTDMC.h"
#include "qg_TestUI.h"
#include "qg_JogButton_ED_New.h"
#include "qg_MyMessageBox.h"
#include "qg_JogButton_BT.h"
#include "ModbusManager.h"
#include "qg_RobotProject_ED.h"
#include "qg_JogButton_JMPin.h"
#include "qg_JogButton_JMHan.h"

Pin2* Pin2::appWindow = NULL;
//如果写1就是用旧的ui方式
#define OLD_UI 0

Pin2::Pin2(QWidget *parent)
    : QMainWindow(parent)
{
    appWindow = this;
    ui.setupUi(this);

    ui.scrollArea_ShowPin->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.scrollArea_ShowPin->setWidgetResizable(false);

    // 适配无边框最大化
    setWindowFlags(Qt::FramelessWindowHint);
    showMaximized();

    //状态栏
    if (!statusBar()) {
        setStatusBar(new QStatusBar(this));
    }
    this->installEventFilter(this);
    //cpu状态等显示
    monitor = new ShowCpuMemory(this);
    // 创建状态栏标签
    QLabel* statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);
    QFont font("Microsoft YaHei", 10, QFont::Normal);
    statusLabel->setFont(font);
    // 设置标签并启动监控
    monitor->SetLab(statusLabel);
    monitor->startMonitoring(1000); // 1秒刷新一次

    //根据版本更新UI
    updataUiFromVersion();

#ifndef DEBUG_LHC
    //加载样式表
    QString exePath = QCoreApplication::applicationDirPath();
    QString stylePath = exePath + "/qss/darkstyle.qss";
    stylePath = "qss/darkstyle.qss";
    //str = "D:/test_project/pin_project/Pin2/qss/lightblue.qss";

    QFile file(stylePath);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
#endif // DEBUG_LHC


    //控制动作循环监听
    motionObject = new Thread();
    motionThread = new QThread();
    motionObject->moveToThread(motionThread);
    if (!motionThread->isRunning())
    {
        motionThread->start();
        QMetaObject::invokeMethod(motionObject, &Thread::triggerMotionTpyes, Qt::QueuedConnection);
        connect(motionObject, &Thread::changeStationUI_signal, this, &Pin2::changeStationUI_slot);
        connect(motionObject, &Thread::setControlEnable_signal, this, &Pin2::setControlEnable_slot);
        connect(motionObject, &Thread::clearWindow_signal, this, &Pin2::clearAllViews_slot);
        connect(motionObject, &Thread::confirmationRequested,
            this, &Pin2::showConfirmationDialog, Qt::BlockingQueuedConnection);
        connect(motionObject, &Thread::robotUnload_signal, this, &Pin2::robotUnload_slot);
        connect(motionObject, &Thread::showCTtime_signal, this, &Pin2::showCTtime_slot);
        
    }

    //全局IO信号，位置等循环监听
    globalObject = new Thread();
    globalThread = new QThread();
    globalObject->moveToThread(globalThread);
    if (!globalThread->isRunning())
    {
        globalThread->start();
        QMetaObject::invokeMethod(globalObject, &Thread::globalTpyes, Qt::QueuedConnection);
        connect(globalObject, &Thread::di_runAction_signal, this, &Pin2::on_pushButton_Run_clicked);
        connect(globalObject, &Thread::di_stopAction_signal, this, &Pin2::on_pushButton_RunStop_clicked);
        connect(globalObject, &Thread::di_resetAction_signal, this, &Pin2::on_pushButton_Reset_clicked);
        
    }
    if (LSM->m_version == LsPin)
    {
        //立昇倍速链线程
        doubleSpeedObject = new Thread();
        doubleSpeedThread = new QThread();
        doubleSpeedObject->moveToThread(doubleSpeedThread);
    }
    else if (LSM->m_version == EdPin)
    {
        //易鼎丰皮带取料线程
        robotLoadEDObject = new Thread();
        robotLoadEDThread = new QThread();
        robotLoadEDObject->moveToThread(robotLoadEDThread);

        //易鼎丰皮带下料线程
        robotUnLoadEDObject = new Thread();
        robotUnLoadEDThread = new QThread();
        robotUnLoadEDObject->moveToThread(robotUnLoadEDThread);
    }
    //结果处理及显示线程
    showResultObject = new Thread();
    showResultThread = new QThread();
    showResultObject->moveToThread(showResultThread);
    if (!showResultThread->isRunning())
    {
        showResultThread->start();
        connect(showResultObject, &Thread::thread_showPin_signal, this, &Pin2::showPin_Slot, Qt::BlockingQueuedConnection); //只能在线程中使用，不然可能死锁
        connect(showResultObject, &Thread::thread_showPin2Dand3D_signal, this, &Pin2::showPin2Dand3D_Slot, Qt::BlockingQueuedConnection);//只能在线程中使用，不然可能死锁
        connect(showResultObject, &Thread::thread_showPin3D_signal, this, &Pin2::showPin3D_Slot, Qt::BlockingQueuedConnection);//只能在线程中使用，不然可能死锁
        //connect(showResultObject, &Thread::thread_showVision_signal, this, &Pin2::showVision_slot, Qt::BlockingQueuedConnection); //只能在线程中使用，不然可能死锁
        connect(showResultObject, &Thread::thread_showCircleMeasure_signal, this, &Pin2::showCircleMeasure_slot, Qt::BlockingQueuedConnection);
        connect(showResultObject, &Thread::thread_showNG_signal, this, &Pin2::showNG_slot, Qt::BlockingQueuedConnection);
        //connect(showResultObject, &Thread::thread_showImage_signal, this, &Pin2::showImage_slot, Qt::BlockingQueuedConnection); 
    }

#if OLD_UI
    ui.pushButton_UpMove->hide();
    ui.pushButton_DownMove->hide();
    ui.pushButton_DeleteRow->hide();
#endif
    //日志绑定
    connect(machineLog, &QC_Log::logUpdate_signal, this, &Pin2::logUpdate_slot);


    //setupJogAxisLayout();
    // 250321 不使用动态添加的办法了，页面感觉有点问题
    //2D识别
    //createPinWindow();
    //3D识别
    //createPin3DMeasureWindow();
    //样式初始化
    initStyle();
    //配方方案初始化
    initFormula();
    //读取生产数据
    readProductMess();
    //用户初始化
    initUser();
    //创建用户切换定时器
    adminTimer = new QTimer(this);
    adminTimer->setSingleShot(true); // 单次触发
    connect(adminTimer, &QTimer::timeout, this, &Pin2::switchToNormalUser);

    // 创建定时器
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Pin2::updateLabelValue);
    timer->start(300);


    if (LSM->m_version == LsPin)
    {
        //建立和扫码枪的tcp通讯
        if (m_clientQRCode.connectToServer("DemoClient", "192.168.3.100", 2001)) {
            qDebug() << "已连接到服务器";
        }
        connect(&m_clientQRCode, &QtTcpClient::sig_RevMsg, this, &Pin2::slot_RevMsg);
    }

    qRegisterMetaType<ResultParamsStruct_Pin>("ResultParamsStruct_Pin");
    qRegisterMetaType<ResultParamsStruct_Pin3DMeasure>("ResultParamsStruct_Pin3DMeasure");
    qRegisterMetaType<PointItemConfig>("PointItemConfig");
    qRegisterMetaType<ResultParamsStruct_Circle>("ResultParamsStruct_Circle");
    qRegisterMetaType<Vision_Message>("Vision_Message");
    qRegisterMetaType<std::unordered_map<QString, Vision_Message>>("std::unordered_map<QString, Vision_Message>");

    qRegisterMetaType<ResultParamsStruct_Template>("ResultParamsStruct_Template");
    qRegisterMetaType<ResultParamsStruct_BarCode>("ResultParamsStruct_BarCode");
    qRegisterMetaType<ResultParamsStruct_Code2d>("ResultParamsStruct_Code2d");
    qRegisterMetaType<ResultParamsStruct_Blob>("ResultParamsStruct_Blob");
    
    
    //connect(ui.comboBox_Project_2, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_Project_2_currentIndexChanged(int)));
    connect(ui.pushButton_Save, &QPushButton::clicked, this, &Pin2::on_pushButton_Save_clicked);
    connect(ui.tabWidget_CameraSetting, &QTabWidget::currentChanged, this, &Pin2::onCameraSettingTabChanged);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &Pin2::onTabWidgetTabChanged);
    connect(LSM, &rs_motion::updateStepIndex_signal, this, &Pin2::updateStepIndex_slot);
    connect(ui.tableWidget_Formula_Left->selectionModel(), &QItemSelectionModel::currentChanged,this, &Pin2::on_LeftRowSelected);
    connect(ui.tableWidget_Formula_Right->selectionModel(), &QItemSelectionModel::currentChanged, this, &Pin2::on_RightRowSelected);
    connect(ui.tabWidget_Formula, &QTabWidget::currentChanged, this, &Pin2::on_tabWidget_FormulaChanged);
    connect(ui.tab_Sql, &qg_QSLiteWidget::showNG_Signal, this, &Pin2::showNG_Slot);//刷新NG率
    qRegisterMetaType<Run_Result>("Run_Result");
    connect(LSM, &rs_motion::upDataProductPara_signal, this, &Pin2::upDataProductPara_slot);//刷新结果显示
    connect(ui.lineEdit_UserAccount, &QLineEdit::textChanged, this, &Pin2::onUserAccountChanged);//用户账号
    connect(LSM, &rs_motion::clearWindow_signal, this, &Pin2::clearAllViews_slot);
    connect(LSM, &rs_motion::writeModbus_signal, this, &Pin2::writeModbus_slot, Qt::BlockingQueuedConnection);
    connect(LSM, &rs_motion::readModbus_signal, this, &Pin2::readModbus_slot, Qt::BlockingQueuedConnection);
    connect(machineLog, &QC_Log::errUpdate_signal, this, &Pin2::errUpdate_slot);
    connect(LSM, &rs_motion::upDateQgraphicsView_signal, this, &Pin2::upDateQgraphicsView_slot);
    connect(ui.lineEdit_WorkOrderCode, &QLineEdit::textChanged, this, &Pin2::onWorkOrderCodeChanged);//工单
    

    //connect(LSM, &rs_motion::showPin_signal, this, &Pin2::showPin_Slot, Qt::BlockingQueuedConnection); //只能在线程中使用，不然可能死锁
    //connect(LSM, &rs_motion::showPin2Dand3D_signal, this, &Pin2::showPin2Dand3D_Slot, Qt::BlockingQueuedConnection);//只能在线程中使用，不然可能死锁
    //connect(LSM, &rs_motion::showPin3D_signal, this, &Pin2::showPin3D_Slot, Qt::BlockingQueuedConnection);//只能在线程中使用，不然可能死锁
    connect(LSM, &rs_motion::showVision_signal, this, &Pin2::showVision_slot, Qt::BlockingQueuedConnection); //只能在线程中使用，不然可能死锁
    //connect(LSM, &rs_motion::showCircleMeasure_signal, this, &Pin2::showCircleMeasure_slot, Qt::BlockingQueuedConnection);
    connect(LSM, &rs_motion::showImage_signal, this, &Pin2::showImage_slot, Qt::BlockingQueuedConnection); //只能在线程中使用，不然可能死锁

    connect(LSM, &rs_motion::showPin_signal, this, &Pin2::showthread_showPin_slot);
    connect(LSM, &rs_motion::showPin2Dand3D_signal, this, &Pin2::showthread_showPin2Dand3D_slot);
    connect(LSM, &rs_motion::showPin3D_signal, this, &Pin2::showthread_showPin3D_slot);
    //connect(LSM, &rs_motion::showVision_signal, this, &Pin2::showthread_showVision_slot);
    connect(LSM, &rs_motion::showCircleMeasure_signal, this, &Pin2::showthread_showCircleMeasure_slot);
    connect(LSM, &rs_motion::showAllVision_signal, this, &Pin2::showthread_showAllVision_slot);
    connect(LSM, &rs_motion::showWorkState_signal, this, &Pin2::showWorkState_slot);
    connect(LSM, &rs_motion::showNG_signal, this, &Pin2::showthread_showNG_slot);
    //connect(LSM, &rs_motion::showImage_signal, this, &Pin2::showthread_showImage_slot);
    
    
#ifdef DEBUG_LHC
    connect(ui.tab_2DVisionWidget, &PinWindow::showPinDeubg_signal, this, &Pin2::showPin_Slot);
#endif // DEBUG_LHC
    //伺服初始化
    LSM->m_triggerMotionTpyes = RS2::InitMotion;
    //易鼎丰初始化连接机器人通讯
    if (LSM->m_version == EdPin)
    {
        //modbusRobot.connectToDevice("192.168.1.12");
        auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
        if(!rtn)
            machineLog->write("ModbusTcp连接失败...", Normal);
    }
   
    if (LSM->m_motionVersion == LS_Pulse_1000)
    {
        //1000的板卡接口没有放出开关伺服的接口，所以不显示
        ui.pushButton_AxisOn->hide();
    }

}

Pin2::~Pin2()
{
    on_pushButton_RunStop_clicked();
    saveProductMess();
    monitor->stopMonitoring();
    if (motionObject)
    {
        motionObject->bExit = true;
        motionObject->deleteLater();
        motionObject = nullptr;
    }

    if (motionThread)
    {
        motionThread->quit();
        motionThread->wait();
        motionThread->deleteLater();
        motionThread = nullptr;
    }

    if (globalObject)
    {
        globalObject->bExit = true;
        globalObject->deleteLater();
        globalObject = nullptr;
    }

    if (globalThread)
    {
        globalThread->quit();
        globalThread->wait();
        globalThread->deleteLater();
        globalThread = nullptr;
    }

    //立昇倍速链线程
    if (doubleSpeedObject)
    {
        doubleSpeedObject->bExit = true;
        doubleSpeedObject->deleteLater();
        doubleSpeedObject = nullptr;
    }

    if (doubleSpeedThread)
    {
        doubleSpeedThread->quit();
        doubleSpeedThread->wait();
        doubleSpeedThread->deleteLater();
        doubleSpeedThread = nullptr;
    }

    if (robotLoadEDObject)
    {
        robotLoadEDObject->bExit = true;
        robotLoadEDObject->deleteLater();
        robotLoadEDObject = nullptr;
    }

    if (robotLoadEDThread)
    {
        robotLoadEDThread->quit();
        robotLoadEDThread->wait();
        robotLoadEDThread->deleteLater();
        robotLoadEDThread = nullptr;
    }

    if (robotUnLoadEDObject)
    {
        robotUnLoadEDObject->bExit = true;
        robotUnLoadEDObject->deleteLater();
        robotUnLoadEDObject = nullptr;
    }

    if (robotUnLoadEDThread)
    {
        robotUnLoadEDThread->quit();
        robotUnLoadEDThread->wait();
        robotUnLoadEDThread->deleteLater();
        robotUnLoadEDThread = nullptr;
    }

    if (showResultObject)
    {
        showResultObject->bExit = true;
        showResultObject->deleteLater();
        showResultObject = nullptr;
    }

    if (showResultThread)
    {
        showResultThread->quit();
        showResultThread->wait();
        showResultThread->deleteLater();
        showResultThread = nullptr;
    }

    if (pinWindow)
    {
        delete pinWindow;
    }

    if (pin3DMeasureWindow)
        delete pin3DMeasureWindow;
}

//创建Pin针2D识别页面
void Pin2::createPinWindow() {
    HObject image;
    int processID = 1;
    int modubleID = 2;
    std::string modubleName = "Module1";
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        // 处理刷新配置
    };
    std::string getConfigPath = "";

    // 创建 PinWindow 实例，使用第二个构造函数
    pinWindow = new PinWindow(image, processID, modubleID, modubleName, refreshConfig, getConfigPath);

    // 创建 QScrollArea，设置为 ui.tab_2DVisionWidget 的子控件
    QScrollArea* scrollArea = new QScrollArea(ui.tab_2DVisionWidget);

    // 设置 scrollArea 的属性
    scrollArea->setWidgetResizable(true);  // 内容大小改变时，scrollArea 会自动调整大小
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 启用垂直滚动条
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条

    // 创建一个 QWidget 用于承载 pinWindow
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    // 将 pinWindow 添加到 contentWidget 的布局中
    contentLayout->addWidget(pinWindow);

    // 将 contentWidget 设置为 QScrollArea 的内容
    scrollArea->setWidget(contentWidget);

    // 获取 tab_2DVisionWidget 的布局并将 scrollArea 添加进去
    QVBoxLayout* tabLayout = new QVBoxLayout(ui.tab_2DVisionWidget);
    tabLayout->addWidget(scrollArea);

    // 设置 tab_2DVisionWidget 的布局
    ui.tab_2DVisionWidget->setLayout(tabLayout);


}


//创建Pin针3D识别页面
void Pin2::createPin3DMeasureWindow()
{
    // 假设已有这些参数
    pcl::PointCloud<pcl::PointXYZ>::Ptr image3D; // 3D点云数据
    int processID = 1;
    int modubleID = 2;
    std::string modubleName = "Module1";
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        // 处理刷新配置
    };
    std::string getConfigPath = "";

    // 创建 Pin3DMeasureWindow 实例，使用构造函数
    pin3DMeasureWindow = new Pin3DMeasureWindow(image3D, processID, modubleID, modubleName, refreshConfig, getConfigPath);

    // 创建 QScrollArea，设置为 ui.tab_3DVisionWidget 的子控件
    QScrollArea* scrollArea = new QScrollArea(ui.tab_3DVisionWidget);

    // 设置 scrollArea 的属性
    scrollArea->setWidgetResizable(true);  // 内容大小改变时，scrollArea 会自动调整大小
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 启用垂直滚动条
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条

    // 创建一个 QWidget 用于承载 pin3DMeasureWindow
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    // 将 pin3DMeasureWindow 添加到 contentWidget 的布局中
    contentLayout->addWidget(pin3DMeasureWindow);

    // 将 contentWidget 设置为 QScrollArea 的内容
    scrollArea->setWidget(contentWidget);

    // 获取 tab_3DVisionWidget 的布局并将 scrollArea 添加进去
    QVBoxLayout* tabLayout = new QVBoxLayout(ui.tab_3DVisionWidget);
    tabLayout->addWidget(scrollArea);

    // 设置 tab_3DVisionWidget 的布局
    ui.tab_3DVisionWidget->setLayout(tabLayout);

}

//样式初始化
void Pin2::initStyle()
{
    ui.label_WorkState->setStyleSheet("color: yellow; font-weight: bold;");
    ui.label_WorkState->setText("待机中");
    if (ui.checkBox_Loop->isChecked())
    {
        LSM->m_isLoop.store(true);
        //禁止单步运行
        ui.pushButton_OneStep->setEnabled(false);
    }
    else
    {
        LSM->m_isLoop.store(false);
        if (!LSM->m_isStart.load())
        {
            ui.pushButton_OneStep->setEnabled(true);
        }
    }

    // 创建 QFont 对象
    // QFont font("Arial", 12);
    // 应用字体样式到 QTabWidget 的标签
    //ui.tabWidget->setFont(font);
    
    // 创建并配置 QStandardItemModel
    //QStandardItemModel* model = new QStandardItemModel(this);
    //model->setColumnCount(9);  // 设置列数

#if OLD_UI
    ui.tableWidget_Formula_Left->setColumnCount(9);  // 设置列数
#else

#endif // OLD_UI


    // 设置表头标签
    QStringList headers;
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // 设置列数
        //双轨pin针
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Z (mm)")
            << tr("RZ (°)")
            << tr("U1 (mm)")
            << tr("Remarks")
            << tr("Edit")
#if OLD_UI
            << tr("Actions")
#endif // OLD_UI
            ;
        break;
    case ENUM_VERSION::EdPin:
        ui.tableWidget_Formula_Left->setColumnCount(10);  // 设置列数
        //易鼎丰专机
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y (mm)")
            << tr("U (°)")
            << tr("U1(治具) (°)")
            << tr("Z (mm)")
            << tr("Z1(正面) (mm)")
            << tr("Z2(反面) (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::LsPin:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // 设置列数
        //立昇专机
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Y2 (mm)")
            << tr("U (°)")
            << tr("Z (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::BtPin:
        ui.tableWidget_Formula_Left->setColumnCount(6);  // 设置列数
        //立昇专机
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Y2 (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::JmPin:
        ui.tableWidget_Formula_Left->setColumnCount(10);  // 设置列数
        //金脉Pin机
        headers << tr("Action")
            << tr("X (mm)")
            << tr("X1(侧相机) (mm)")
            << tr("Y (mm)")
            << tr("U (°)")
            << tr("U1(治具) (°)")
            << tr("Z (mm)")
            << tr("Z1(侧相机) (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::JmHan:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // 设置列数
        //金脉焊缝机
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y (mm)")
            << tr("Y1 (mm)")
            << tr("U (°)")
            << tr("Z (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    default:
        break;
    }
  

    // 设置表头
    ui.tableWidget_Formula_Left->setHorizontalHeaderLabels(headers);
    // 设置表头自适应大小
    ui.tableWidget_Formula_Left->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidget_Formula_Left->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
    ui.tableWidget_Formula_Left->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
    //// 禁用编辑
    //for (int col = 0; col <= 6; ++col) {
    //    for (int row = 0; row < ui.tableWidget_Formula_Left->rowCount(); ++row) {
    //        ui.tableWidget_Formula_Left->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    //    }
    //}
    // 禁用第一列编辑
    for (int row = 0; row < ui.tableWidget_Formula_Left->rowCount(); ++row) {
        ui.tableWidget_Formula_Left->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    // 连接双击更改槽函数
    connect(ui.tableWidget_Formula_Left, &QTableWidget::cellChanged, this, &Pin2::onLeftCellDoubleClicked);

    if (LSM->m_version == ENUM_VERSION::TwoPin)
    {
        //双轨pin针
        //右工位表格
#if OLD_UI
        ui.tableWidget_Formula_Right->setColumnCount(9);  // 设置列数
#else
        ui.tableWidget_Formula_Right->setColumnCount(8);  // 设置列数
#endif // OLD_UI
    // 设置表头标签
        QStringList headersRight;
        headersRight << tr("Action")
            << tr("X (mm)")
            << tr("Y2 (mm)")
            << tr("Z (mm)")
            << tr("RZ ()")
            << tr("U2 (mm)")
            << tr("Remarks")
            << tr("Edit")
#if OLD_UI
            << tr("Actions")
#endif // OLD_UI
            ;

        // 设置表头
        ui.tableWidget_Formula_Right->setHorizontalHeaderLabels(headersRight);
        //// 将模型设置给 QTableView
        //ui.tableView_Formula_Right->setModel(modelRight);
        // 设置表头自适应大小
        ui.tableWidget_Formula_Right->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui.tableWidget_Formula_Right->setSelectionMode(QAbstractItemView::SingleSelection);  // 单行选择
        ui.tableWidget_Formula_Right->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
        //// 禁用编辑
        //for (int col = 0; col <= 6; ++col) {
        //    for (int row = 0; row < ui.tableWidget_Formula_Right->rowCount(); ++row) {
        //        ui.tableWidget_Formula_Right->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //    }
        //}
        // 禁用第一列编辑
        for (int row = 0; row < ui.tableWidget_Formula_Right->rowCount(); ++row) {
            ui.tableWidget_Formula_Right->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        // 连接双击更改槽函数
        connect(ui.tableWidget_Formula_Right, &QTableWidget::cellChanged, this, &Pin2::onRightCellDoubleClicked);
    }


    //折线图初始化
    ui.widget_showNG->addCurve("NG", Qt::red);
    ui.widget_showNG->addCurve("OK", Qt::green);
    // 设置时间窗口为600秒
    ui.widget_showNG->setTimeWindow(600);
    // 设置Y轴范围
    ui.widget_showNG->setYRange(0, 120);

    // 开始自动更新
    ui.widget_showNG->startAutoUpdate();
}

//新建步骤
void Pin2::on_pushButton_AddFormula_clicked()
{
    // 获取当前工位（左/右）
    ENUMSTATION station = ENUMSTATION::L;
    if(ui.tabWidget_Formula->currentIndex() == 1)
        station = ENUMSTATION::R;

    //读当前方案
    auto name = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
    {
        if (station == ENUMSTATION::L)
        {
            //改成获取配方的参数
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_L,station);
            connect(&dlg, &qg_Formula::signalFormula, this, &Pin2::slotFormula);
            dlg.exec();
        }
        else if (station == ENUMSTATION::R)
        {
            //改成获取配方的参数
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_R, station);
            connect(&dlg, &qg_Formula::signalFormula, this, &Pin2::slotFormula);
            dlg.exec();
        }
    }
}

//初始化
void Pin2::on_pushButton_init_clicked()
{
    // 初始化请求参数
    LSM->m_triggerMotionTpyes = RS2::InitMotion;
}


//单轴回零
void Pin2::on_pushButton_Homing_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在运行中，请等待运行结束或停止后再回零！"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在复位中，请等待运行结束或停止后再回零！"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("警告"), tr("板卡未初始化成功，请先完成初始化后再回零！"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("未清除报警，请先清除报警！"));
        return;
    }
    QString currentAxis = ui.comboBox_AxisNum->currentText();

    if (LSM->m_version == LsPin)
    {
        if (currentAxis == LS_AxisName::LS_X)
        {
            LSM->m_triggerMotionTpyes = RS2::X_Homing;
        }
        else if (currentAxis == LS_AxisName::LS_Y1)
        {
            LSM->m_triggerMotionTpyes = RS2::Y1_Homing;
        }
        else if (currentAxis == LS_AxisName::LS_Y2)
        {
            LSM->m_triggerMotionTpyes = RS2::Y2_Homing;
        }
        else if (currentAxis == LS_AxisName::LS_Z)
        {
            LSM->m_triggerMotionTpyes = RS2::Z_Homing;
        }
        else if (currentAxis == LS_AxisName::LS_U)
        {
            LSM->m_triggerMotionTpyes = RS2::U1_Homing;
        }
    }
    else
    {
        if (currentAxis == LS_AxisName::X)
        {
            LSM->m_triggerMotionTpyes = RS2::X_Homing;
        }
        else if (currentAxis == LS_AxisName::Y)
        {
            LSM->m_triggerMotionTpyes = RS2::Y_Homing;
        }
        else if (currentAxis == LS_AxisName::Y1)
        {
            LSM->m_triggerMotionTpyes = RS2::Y1_Homing;
        }
        else if (currentAxis == LS_AxisName::Y2)
        {
            LSM->m_triggerMotionTpyes = RS2::Y2_Homing;
        }
        else if (currentAxis == LS_AxisName::Z)
        {
            LSM->m_triggerMotionTpyes = RS2::Z_Homing;
        }
        else if (currentAxis == LS_AxisName::Z1)
        {
            LSM->m_triggerMotionTpyes = RS2::Z1_Homing;
        }
        else if (currentAxis == LS_AxisName::Z2)
        {
            LSM->m_triggerMotionTpyes = RS2::Z2_Homing;
        }
        else if (currentAxis == LS_AxisName::RZ)
        {
            LSM->m_triggerMotionTpyes = RS2::RZ_Homing;
        }
        else if (currentAxis == LS_AxisName::U)
        {
            LSM->m_triggerMotionTpyes = RS2::U_Homing;
        }
        else if (currentAxis == LS_AxisName::U1)
        {
            LSM->m_triggerMotionTpyes = RS2::U1_Homing;
        }
        else if (currentAxis == LS_AxisName::U2)
        {
            LSM->m_triggerMotionTpyes = RS2::U2_Homing;
        }
    }
}

//点动停止
void Pin2::on_pushButton_JogStop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        //运行状态，停止机器人
        MODBUS_TCP.writeIntCoil(100, 2, 0);
        Sleep(500);
        MODBUS_TCP.writeIntCoil(100, 2, 1);
        bool programRunningStatus = false;
        MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
        LSM->m_RobotModbus.remoteStatus = 0;
        QElapsedTimer materialTimer;
        materialTimer.start();
        while (programRunningStatus)
        {
            //没停下来，一直发送停止
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("机器人暂停超时：" + QString::number(ERROR_TIME) + " 毫秒后还未读取到暂停成功", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "警告", "机器人暂停超时,请直接拍急停！！", QMessageBox::Yes, this);

                // 设置样式表：字体放大3倍，按钮尺寸适配
                QString styleSheet =
                    "QMessageBox { font-size: 24px; } "  // 默认字体放大3倍（假设原为8px）
                    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // 内容区域尺寸
                    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // 按钮放大

                msgBox.setStyleSheet(styleSheet);

                // 显示弹窗
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 1, 0);
            //Sleep(5);
        }
        //停下来了，运行和停止状态清0
        clearRobotAuto();
    }
}

//运行停止
void Pin2::on_pushButton_RunStop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        LSM->closeDO();
        //运行状态，停止机器人
        MODBUS_TCP.writeIntCoil(100, 2, 0);
        Sleep(500);
        MODBUS_TCP.writeIntCoil(100, 2, 1);
        LSM->m_RobotModbus.remoteStatus = 0;
        bool programRunningStatus = false;
        MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
        QElapsedTimer materialTimer;
        materialTimer.start();
        while (programRunningStatus)
        {
            //没停下来，一直发送停止
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("机器人暂停超时：" + QString::number(ERROR_TIME) + " 毫秒后还未读取到暂停成功", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "警告", "机器人暂停超时,请直接拍急停！！" ,QMessageBox::Yes, this);

                //// 设置样式表：字体放大3倍，按钮尺寸适配
                //QString styleSheet =
                //    "QMessageBox { font-size: 24px; } "  // 默认字体放大3倍（假设原为8px）
                //    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // 内容区域尺寸
                //    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // 按钮放大

                //msgBox.setStyleSheet(styleSheet);

                // 显示弹窗
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            //Sleep(5);
        }
        //停下来了，运行和停止状态清0
        clearRobotAuto();
    }

}

void Pin2::setupJogAxisLayout()
{
    // 获取 tab_JogAxis 的布局，如果没有布局则创建一个新的
    QGridLayout* layout = qobject_cast<QGridLayout*>(ui.tab_JogAxis->layout());
    if (!layout) {
        layout = new QGridLayout(ui.tab_JogAxis);
        ui.tab_JogAxis->setLayout(layout);
    }

    // 设置布局的行/列间距
    layout->setHorizontalSpacing(6);
    layout->setVerticalSpacing(6);

    // 设置布局的内容边距
    layout->setContentsMargins(10, 10, 10, 10);

    // 设置布局的行列比例（Stretch）
    layout->setRowStretch(0, 1); // 第一行比例
    layout->setRowStretch(1, 1); // 第二行比例
    layout->setRowStretch(2, 4); // 第三行比例

    //layout->setColumnStretch(0, 1); // 第一列比例
    //layout->setColumnStretch(1, 1); // 第二列比例
    //layout->setColumnStretch(2, 1); // 第三列比例

    // 确保布局更新
    layout->update();
}


// 槽函数：更新 label_7 的值
void Pin2::updateLabelValue()
{
#ifdef DEBUG_LHC
    if (!LSM->m_cardInitStatus)
        return;
#endif // DEBUG_LHC
    QString axesPositions;
    QString axisName;
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        // 拼接各个轴的位置字符串
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1；Y1:%2；Y2:%3；Z:%4；RZ:%5；U1:%6；U2:%7；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y2].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::RZ].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U2].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::EdPin:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1；Y:%2；U:%3；U1(治具):%4；Z:%5；Z1(正面):%6；Z2(反面):%7；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z2].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::LsPin:
        axisName = LS_AxisName::LS_X;
        axesPositions = QString("X:%1；Y1:%2；Y2:%3；U:%4；Z:%5；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_X ].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y2].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_U ].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Z ].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::BtPin:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1；Y1:%2；Y2:%3；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y2].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::JmPin:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1；X1(侧相机):%2；Y:%3；U:%4；<br>U1(治具):%5；Z:%6；Z1(侧相机):%7；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z1].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::JmHan:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1；Y:%2；Y1:%3；U:%4；Z:%5；")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z].position * 100) / 100, 'f', 2));
        break;
    default:
        break;
    }

    // 更新 label_7 的文本
    ui.label_7->setText(axesPositions);

    // 顺便更新一下伺服使能状态
    if (LSM->ReadServOn(axisName))
    {
        LSM->isServeOn = true;
        ui.pushButton_AxisOn->setText("使能关");
    }
    else
    {
        LSM->isServeOn = false;
        ui.pushButton_AxisOn->setText("使能开");
    }
}

//触发复位
void Pin2::on_pushButton_Reset_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在运行中，请等待运行结束或停止后再复位！"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在复位中，请等待运行结束或停止后再复位！"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("警告"), tr("板卡未初始化成功，请先完成初始化后再复位！"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("未清除报警，请先清除报警！"));
        return;
    }
    ui.lineEdit_RunSteps->setText(QString::number(0));
    LSM->m_triggerMotionTpyes = RS2::ActionFlag::ResetAll;
}

//停止
void Pin2::on_pushButton_Stop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        //运行状态，停止机器人
        MODBUS_TCP.writeIntCoil(100, 2, 0);
        Sleep(500);
        MODBUS_TCP.writeIntCoil(100, 2, 1);
        LSM->m_RobotModbus.remoteStatus = 0;
        bool programRunningStatus = false;
        MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
        QElapsedTimer materialTimer;
        materialTimer.start();
        while (programRunningStatus)
        {
            //没停下来，一直发送停止
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("机器人暂停超时：" + QString::number(ERROR_TIME) + " 毫秒后还未读取到暂停成功", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "警告", "机器人暂停超时,请直接拍急停！！", QMessageBox::Yes, this);

                // 设置样式表：字体放大3倍，按钮尺寸适配
                QString styleSheet =
                    "QMessageBox { font-size: 24px; } "  // 默认字体放大3倍（假设原为8px）
                    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // 内容区域尺寸
                    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // 按钮放大

                msgBox.setStyleSheet(styleSheet);

                // 显示弹窗
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            //Sleep(5);
        }
        //停下来了，运行和停止状态清0
        clearRobotAuto();
    }
}

//轴复位
void Pin2::on_pushButton_AxisReset_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在运行中，请等待运行结束或停止后再复位！"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在复位中，请等待运行结束或停止后再复位！"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("警告"), tr("板卡未初始化成功，请先完成初始化后再复位！"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("未清除报警，请先清除报警！"));
        return;
    }
    ui.lineEdit_RunSteps->setText(QString::number(0));
    LSM->m_triggerMotionTpyes = RS2::ActionFlag::ResetAll;
}

//轴使能
void Pin2::on_pushButton_AxisOn_clicked()
{
    if (LSM->isServeOn)
    {
        for (auto axis : LS_AxisName::allAxis)
        {
            LSM->WriteAxisServeOn(axis, false);
        }
    }
    else if (!LSM->isServeOn)
    {
        for (auto axis : LS_AxisName::allAxis)
        {
            LSM->WriteAxisServeOn(axis, true);
        }
    }
}

//轴报错清除
void Pin2::on_pushButton_Clear_clicked()
{
    if (LSM->m_version == LsPin)
    {
        //确认弹窗
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "复位确认",
            "即将进行控制卡热复位，是否继续？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        //创建进度弹窗
        QProgressDialog progressDialog("总线卡复位进行中...", "取消", 0, 15, this);
        progressDialog.setWindowTitle("复位进度");
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setCancelButton(nullptr); // 禁用取消按钮
        progressDialog.setMinimumDuration(0);     // 立即显示
        progressDialog.setValue(0);

        machineLog->write("请勿操作，总线卡软件复位进行中（约15S）……", Normal);

        // 执行复位操作
        QCoreApplication::processEvents();

        // 复位第一阶段操作
        dmc_soft_reset(LSM->m_Axis[LS_AxisName::LS_X].card);
        dmc_board_close();
        progressDialog.setValue(1);

        // 等待循环带进度更新
        QElapsedTimer timer;
        timer.start();
        for (int i = 1; i <= 15; ++i) {
            // 非阻塞等待（保持UI响应）
            while (timer.elapsed() < i * 1000) {
                QCoreApplication::processEvents();
                QThread::msleep(50);
            }

            // 更新进度（保留1秒已完成）
            progressDialog.setValue(i + 1);

            // 每5秒更新日志
            if (i % 5 == 0) {
                machineLog->write(QString("复位中...已等待%1秒").arg(i), Normal);
            }
        }

        // 完成后续操作
        LSM->motionInit();
        progressDialog.setValue(16);

        //// 状态指示灯操作
        //if (LSM->m_DO[LS_DO::LS_OUT_YellowLight].state == 0) {
        //    LSM->setDO(LS_DO::LS_OUT_RedLight, 0);
        //    LSM->setDO(LS_DO::LS_OUT_YellowLight, 1);
        //    LSM->setDO(LS_DO::LS_OUT_GreenLight, 0);
        //}

        // 轴报错清除
        QFutureSynchronizer<void> sync;
        for (auto axis : LS_AxisName::allAxis) {
            sync.addFuture(QtConcurrent::run([=]() {
                LSM->AxisClear(axis);
                }));
        }
        sync.waitForFinished();

        LSM->m_isEmergency.store(false);

        machineLog->write("总线卡软件复位完成,请确认总线状态", Normal);
        QMessageBox::information(this, "完成", "复位操作已全部完成");
    }
    else if (LSM->m_version == EdPin)
    {
        //清除机器人报错
        MODBUS_TCP.writeIntCoil(100, 3, 0);
        Sleep(500);
        MODBUS_TCP.writeIntCoil(100, 3, 1);
        for (auto axis : LS_AxisName::allAxis)
        {
            LSM->AxisClear(axis);
        }
        LSM->m_isEmergency.store(false);
    }
    else
    {
        for (auto axis : LS_AxisName::allAxis)
        {
            LSM->AxisClear(axis);
        }
        LSM->m_isEmergency.store(false);
    }
}

//新建配方方案
void Pin2::on_pushButton_AddProject_clicked()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("输入文件名"), tr("请输入文件名："), QLineEdit::Normal, "", &ok);

    if (!ok || fileName.isEmpty()) {
        return;
    }

    // 获取exe目录下的Formula_Config文件夹路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // 如果Formula_Config文件夹不存在，则创建
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::critical(this, tr("错误"), tr("无法创建文件夹！"));
            return;
        }
    }

    // 构造文件的完整路径
    QString filePath = dir.filePath(fileName + ".json");

    // 判断文件是否已存在
    if (QFile::exists(filePath)) {
        // 文件已存在，弹出确认框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("文件已存在"), tr("文件已存在，是否覆盖？"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    // 创建并写入 JSON 文件
    QJsonObject jsonObject;

    QJsonDocument jsonDoc(jsonObject);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入！");
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();

    //存到配方方案里
    LSM->m_Formula[fileName];
    ui.comboBox_Project_2->addItem(fileName);
    // 设置当前选项为新建的文件名
    ui.comboBox_Project_2->setCurrentText(fileName);
    ui.label_ProjectName->clear();
    ui.label_ProjectName->setText(fileName);
    // 清空配方表
    ui.tableWidget_Formula_Left->clearContents();
    ui.tableWidget_Formula_Right->clearContents();
    // 记录当前配方名
    LSM->m_forMulaName = fileName;
}

//配方方案切换
void Pin2::on_comboBox_Project_2_currentIndexChanged(int index)
{
    if (LSM->m_version == EdPin)
    {
        if (m_comboBox_Project_2_lastIndex != index)
        {
            QString text = ui.comboBox_Project_2->itemText(index); // 获取新选项的文本
            QString proName = "请确认是否切换为配方：[ " + text + " ] 并检查治具是否匹配";
            QMessageBox::StandardButton res = QMessageBox::question(nullptr, "提示", proName,
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes);
            if (res == QMessageBox::No)
            {
                // 阻塞信号防止递归触发
                ui.comboBox_Project_2->blockSignals(true);
                // 恢复之前的选项
                ui.comboBox_Project_2->setCurrentIndex(m_comboBox_Project_2_lastIndex);
                ui.comboBox_Project_2->blockSignals(false);
                return;
            }
            // 用户选择Yes，更新lastIndex
            m_comboBox_Project_2_lastIndex = index;
        }
    }

    // 获取当前选择的项目
    QString selectedItem = ui.comboBox_Project_2->currentText();
    ui.label_ProjectName->clear();
    ui.label_ProjectName->setText(selectedItem);
    LSM->m_forMulaName = selectedItem;
    clearProductMess();

    // 如果配方存在，更新表格
    auto it = LSM->m_Formula.find(selectedItem);
    if (it != LSM->m_Formula.end()) {
        // 清空左侧和右侧的表格
        ui.tableWidget_Formula_Left->clearContents();
        ui.tableWidget_Formula_Right->clearContents();

        // 更新左侧表格
        ui.tableWidget_Formula_Left->setRowCount(0);  // 清除现有行
        rs_motion::Formula_Config selectedFormula = it->second;
        for (auto config : selectedFormula.ListProcess_L)
        {
            //左工位
            addNewRow(config);
        }

        // 更新右侧表格
        ui.tableWidget_Formula_Right->setRowCount(0);  // 清除现有行
        for (auto config : selectedFormula.ListProcess_R)
        {
            //右工位
            addNewRow(config);
        }
    }

   
}

//添加步骤的槽函数
void Pin2::slotFormula(const PointItemConfig& config)
{
    addNewRow(config);

    //读当前方案
    auto name = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
    {
        if (config.StationL == ENUMSTATION::L)
        {
            LSM->m_Formula[name].ListProcess_L.push_back(config);
        }
        else if (config.StationL == ENUMSTATION::R)
        {
            LSM->m_Formula[name].ListProcess_R.push_back(config);
        }
    }
}

//配方初始化
void Pin2::initFormula()
{
    // 获取exe目录下的Formula_Config文件夹路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    if (!dir.exists()) {
        return;
    }

    ui.comboBox_Project_2->clear();
    // 获取文件夹中所有文件（不包括子文件夹）
    dir.setFilter(QDir::Files);  // 只获取文件，不包括子文件夹
    //dir.setSorting(QDir::Time | QDir::Reversed);  // 按创建时间排序，新的文件排在前面
    dir.setSorting(QDir::Time);  // 按创建时间排序，新的文件排在前面

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        QString name = fileInfo.baseName();
        ui.comboBox_Project_2->addItem(name);
    }


    if (LSM->m_version == EdPin)
    {
        JsonConfigManager config("RobotToFormula.json");

        // 清空现有映射
        LSM->m_formulaToRobot.clear();

        // 读取整个JSON对象
        QJsonObject jsonObj = config.readAllJsonObject();

        // 创建临时映射：机器人方案 -> 配方列表
        std::unordered_map<int, QStringList> robotToFormulas;

        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            QString robotStr = it.key();

            bool ok;
            int robotNumber = robotStr.toInt(&ok);
            if (!ok || robotNumber < 1 || robotNumber > 9) continue;

            // 检查值是否为字符串
            if (!it.value().isString()) continue;

            QString formulaStr = it.value().toString();

            // 分割配方字符串
            QStringList formulas = formulaStr.split(QRegularExpression("[;；]"), Qt::SkipEmptyParts);

            // 更新临时映射
            for (const QString& formula : formulas) {
                QString trimmedFormula = formula.trimmed();
                if (!trimmedFormula.isEmpty()) {
                    robotToFormulas[robotNumber].append(trimmedFormula);
                }
            }
        }

        // 更新全局映射
        for (const auto& pair : robotToFormulas) {
            int robotNumber = pair.first;
            const QStringList& formulas = pair.second;

            for (const QString& formula : formulas) {
                LSM->m_formulaToRobot[formula] = robotNumber;
            }
        }

        // 检查是否成功读取
        if (LSM->m_formulaToRobot.empty()) {
            machineLog->write("读取机器人方案和配方对应关系失败，请重新检查并配置！！", Normal);
            machineLog->write("读取机器人方案和配方对应关系失败，请重新检查并配置！！", Err);
            return;
        }
    }
}

// 添加新行
void Pin2::addNewRow(const PointItemConfig config) {

    if (config.StationL == ENUMSTATION::L)
    {
        // 阻塞信号
        ui.tableWidget_Formula_Left->blockSignals(true);
        //左工位
        // 获取当前的行数
        int row = ui.tableWidget_Formula_Left->rowCount();
        ui.tableWidget_Formula_Left->insertRow(row);

        // 创建 RowData 对象，封装数据和按钮
        //RowData rowData(config, row);

        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("过渡点");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin针识别点");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3D扫描点");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3D识别");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("取放料");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("拍照");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("视觉模块");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("圆测距");
            break;
        default:
            break;
        }

        // 直接在单元格中创建按钮（无需外部对象）
        QPushButton* editButton = new QPushButton("编辑", ui.tableWidget_Formula_Left);
        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {  // 显式捕获 editButton
            // 动态获取实际行号
            QPoint buttonPos = editButton->mapToParent(QPoint(0, 0));  // 转换坐标到父控件
            int actualRow = ui.tableWidget_Formula_Left->indexAt(buttonPos).row();
            onEditButtonClicked(actualRow);
            });
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // 设置数据到表格的相应单元格
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            ui.tableWidget_Formula_Left->setItem(row, 6, new QTableWidgetItem(config.Tip));
            // 将按钮嵌入到表格的第7列
            ui.tableWidget_Formula_Left->setCellWidget(row, 7, editButton);
            //// 将编辑按钮添加到表格的"编辑"列
            //ui.tableWidget_Formula_Left->setCellWidget(row, 7, rowData.editButton);
            break;
        case ENUM_VERSION::EdPin:
        case ENUM_VERSION::JmPin:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
            ui.tableWidget_Formula_Left->setItem(row, 6, new QTableWidgetItem(QString::number(config.TargetPAxis_6)));
            ui.tableWidget_Formula_Left->setItem(row, 7, new QTableWidgetItem(QString::number(config.TargetPAxis_7)));
            ui.tableWidget_Formula_Left->setItem(row, 8, new QTableWidgetItem(config.Tip));
            // 将编辑按钮添加到表格的"编辑"列
            ui.tableWidget_Formula_Left->setCellWidget(row, 9, editButton);
            break;
        case ENUM_VERSION::LsPin:
        case ENUM_VERSION::JmHan:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
            ui.tableWidget_Formula_Left->setItem(row, 6, new QTableWidgetItem(config.Tip));
            // 将编辑按钮添加到表格的"编辑"列
            ui.tableWidget_Formula_Left->setCellWidget(row, 7, editButton);
            break;
        case ENUM_VERSION::BtPin:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(config.Tip));
            // 将编辑按钮添加到表格的"编辑"列
            ui.tableWidget_Formula_Left->setCellWidget(row, 5, editButton);
            break;
        default:
            break;
        }


        //// 禁用编辑
        //for (int col = 0; col < 6; ++col) {
        //    ui.tableWidget_Formula_Left->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //}
        // 禁用第一列编辑
        ui.tableWidget_Formula_Left->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);

#if OLD_UI
        // 创建一个 QWidget 来包含操作按钮
        QWidget* operationWidget = new QWidget();
        QHBoxLayout* operationLayout = new QHBoxLayout();
        operationLayout->setContentsMargins(0, 0, 0, 0); 

        operationLayout->addWidget(rowData.btn_upMove);
        operationLayout->addWidget(rowData.btn_downMove);
        operationLayout->addWidget(rowData.btnRemove);

        operationWidget->setLayout(operationLayout);
        // 将操作按钮布局添加到"操作"列
        ui.tableWidget_Formula_Left->setCellWidget(row, 8, operationWidget); 

        connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
        connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
        connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif
        // 恢复信号
        ui.tableWidget_Formula_Left->blockSignals(false);
    }
    else if (config.StationL == ENUMSTATION::R && LSM->m_version == ENUM_VERSION::TwoPin)
    {
        // 阻塞信号
        ui.tableWidget_Formula_Right->blockSignals(true);
        //右工位
        // 获取当前的行数
        int row = ui.tableWidget_Formula_Right->rowCount();
        ui.tableWidget_Formula_Right->insertRow(row);  // 插入新行

        // 创建 RowData 对象，封装数据和按钮
        //RowData rowData(config, row);

        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("过渡点");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin针识别点");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3D扫描点");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3D识别");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("取放料");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("拍照");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("视觉模块");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("圆测距");
            break;
        default:
            break;
        }
        // 直接在单元格中创建按钮（无需外部对象）
        QPushButton* editButton = new QPushButton("编辑", ui.tableWidget_Formula_Right);
        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {  // 显式捕获 editButton
            // 动态获取实际行号
            QPoint buttonPos = editButton->mapToParent(QPoint(0, 0));  // 转换坐标到父控件
            int actualRow = ui.tableWidget_Formula_Right->indexAt(buttonPos).row();
            onEditButtonClicked(actualRow);
            });
        // 设置数据到表格的相应单元格
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // 设置数据到表格的相应单元格
            ui.tableWidget_Formula_Right->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Right->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Right->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Right->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Right->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Right->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            ui.tableWidget_Formula_Right->setItem(row, 6, new QTableWidgetItem(config.Tip));
            // 将编辑按钮添加到表格的"编辑"列
            ui.tableWidget_Formula_Right->setCellWidget(row, 7, editButton);
            //ui.tableWidget_Formula_Right->setCellWidget(row, 7, rowData.editButton);
            break;
        default:
            break;
        }

        //// 禁用编辑
        //for (int col = 0; col < 6; ++col) {
        //    ui.tableWidget_Formula_Right->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //}
        // 禁用第一列编辑
        ui.tableWidget_Formula_Right->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);
        
#if OLD_UI
        // 创建一个 QWidget 来包含操作按钮
        QWidget* operationWidget = new QWidget();
        QHBoxLayout* operationLayout = new QHBoxLayout();
        operationLayout->setContentsMargins(0, 0, 0, 0);

        operationLayout->addWidget(rowData.btn_upMove);
        operationLayout->addWidget(rowData.btn_downMove);
        operationLayout->addWidget(rowData.btnRemove);

        operationWidget->setLayout(operationLayout);
        // 将操作按钮布局添加到"操作"列
        ui.tableWidget_Formula_Right->setCellWidget(row, 8, operationWidget);

        connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
        connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
        connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif
        // 恢复信号
        ui.tableWidget_Formula_Right->blockSignals(false);
    }
  
   
}

//保存方案
void Pin2::on_pushButton_Save_clicked()
{
    //LSM->saveFormulaToFile();
    //只保存当前方案
    saveOneFormulaToFile(ui.comboBox_Project_2->currentText());
}

void Pin2::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        // 取消行选择
        ui.tableWidget_Formula_Left->clearSelection();
        ui.tableWidget_Formula_Right->clearSelection();
    }

    QWidget::keyPressEvent(event);
}
//
//void Pin2::mousePressEvent(QMouseEvent* event)
//{
//    if (!ui.tableWidget_Formula_Left->rect().contains(ui.tableWidget_Formula_Left->mapFromGlobal(event->globalPos())) &&
//        !ui.tableWidget_Formula_Right->rect().contains(ui.tableWidget_Formula_Right->mapFromGlobal(event->globalPos())))
//    {
//        // 取消行选择
//        ui.tableWidget_Formula_Left->clearSelection();
//        ui.tableWidget_Formula_Right->clearSelection();
//    }
//    if (event->button() == Qt::LeftButton) {
//        //// 记录拖动前的状态
//        //m_isMaximizedBeforeDrag = isMaximized();
//        //m_restoreGeometry = geometry();
//
//        //// 如果是最大化状态，则还原窗口并调整大小
//        //if (m_isMaximizedBeforeDrag) {
//        //    showNormal(); // 先还原窗口
//
//        //    // 计算新窗口位置：鼠标当前位置为中心
//        //    QSize normalSize = m_restoreGeometry.size();
//        //    QPoint mouseGlobalPos = event->globalPos();
//        //    QRect newGeometry(
//        //        mouseGlobalPos.x() - normalSize.width() / 2,
//        //        mouseGlobalPos.y() - 20,  // 保留顶部边距
//        //        normalSize.width(),
//        //        normalSize.height()
//        //    );
//
//        //    // 确保窗口不超出屏幕
//        //    QScreen *screen = QApplication::screenAt(mouseGlobalPos);
//        //    if (screen) {
//        //        QRect screenGeometry = screen->availableGeometry();
//        //        newGeometry = newGeometry.intersected(screenGeometry);
//        //    }
//
//        //    setGeometry(newGeometry);
//        //}
//
//        // 记录当前鼠标位置（相对于窗口）
//        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
//        event->accept();
//    }
//
//    QWidget::mousePressEvent(event);
//}

//删除某行步骤
void Pin2::onDeleteButtonClicked()
{
    // 获取点击的按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }

    // 获取行号
    int row = button->property("row").toInt();

    if (ui.tabWidget_Formula->currentIndex() == 0) 
    {
        // 左点位
        ui.tableWidget_Formula_Left->removeRow(row);

        // 删除 m_Formula 中 ListProcess_L 对应的元素
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 删除 ListProcess_L 中对应索引的元素
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.removeAt(row);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) 
    {
        // 右点位
        ui.tableWidget_Formula_Right->removeRow(row);

        // 删除 m_Formula 中 ListProcess_R 对应的元素
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 删除 ListProcess_R 中对应索引的元素
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.removeAt(row);
            }
        }
    }
}

//步骤上移
void Pin2::onUpMoveButtonClicked() {

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int row = button->property("row").toInt();

        // 判断是否是第一行，第一行不能上移
        if (row <= 0) {
            return;
        }

        if (ui.tabWidget_Formula->currentIndex() == 0) {
            // 左点位
            // 交换表格中的两行数据
            swapTableRows(ui.tableWidget_Formula_Left, row, 0);

            // 获取配方名称
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // 交换 ListProcess_L 中对应的元素
                if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                    selectedFormula.ListProcess_L.swapItemsAt(row, row - 1);
                }
            }
        }
        else if (ui.tabWidget_Formula->currentIndex() == 1) {
            // 右点位
            // 交换表格中的两行数据
            swapTableRows(ui.tableWidget_Formula_Right, row, 0);

            // 获取配方名称
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // 交换 ListProcess_R 中对应的元素
                if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                    selectedFormula.ListProcess_R.swapItemsAt(row, row - 1);
                }
            }
        }
    }
}

// 步骤下移
void Pin2::onDownMoveButtonClicked() {

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int row = button->property("row").toInt();

        if (ui.tabWidget_Formula->currentIndex() == 0) {

            // 左点位
            // 判断是否是最后一行，最后一行不能下移
            int rowCount = ui.tableWidget_Formula_Left->rowCount();
            if (row >= rowCount - 1) {
                return;
            }
            // 交换表格中的两行数据
            swapTableRows(ui.tableWidget_Formula_Left, row, 1);

            // 获取配方名称
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // 交换 ListProcess_L 中对应的元素
                if (row >= 0 && row < selectedFormula.ListProcess_L.size() - 1) {
                    selectedFormula.ListProcess_L.swapItemsAt(row, row + 1);
                }
            }
        }
        else if (ui.tabWidget_Formula->currentIndex() == 1) {
            // 右点位
            // 判断是否是最后一行，最后一行不能下移
            int rowCount = ui.tableWidget_Formula_Right->rowCount();
            if (row >= rowCount - 1) {
                return;
            }
            // 交换表格中的两行数据
            swapTableRows(ui.tableWidget_Formula_Right, row, 1);

            // 获取配方名称
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // 交换 ListProcess_R 中对应的元素
                if (row >= 0 && row < selectedFormula.ListProcess_R.size() - 1) {
                    selectedFormula.ListProcess_R.swapItemsAt(row, row + 1);
                }
            }
        }
    }
}

// 交换表格中两行的数据 mode = 0是上移，=1 是下移
void Pin2::swapTableRows(QTableWidget* tableWidget, int row,int mode)
{
    // 阻塞信号
    ui.tableWidget_Formula_Left->blockSignals(true);
    ui.tableWidget_Formula_Right->blockSignals(true);
    int colCount = tableWidget->columnCount();  // 获取列数
    for (int col = 0; col < colCount; ++col) {
        if (mode == 0)
        {
            // 获取当前行和上一行的单元格内容
            QTableWidgetItem* item1 = tableWidget->takeItem(row, col);
            QTableWidgetItem* item2 = tableWidget->takeItem(row - 1, col);

            // 交换两行的数据
            tableWidget->setItem(row, col, item2);
            tableWidget->setItem(row - 1, col, item1);
        }
        else if (mode == 1)
        {
            // 获取当前行和下一行的单元格内容
            QTableWidgetItem* item1 = tableWidget->takeItem(row, col);
            QTableWidgetItem* item2 = tableWidget->takeItem(row + 1, col);

            // 交换两行的数据
            tableWidget->setItem(row, col, item2);
            tableWidget->setItem(row + 1, col, item1);
        }
    }
    // 恢复信号
    ui.tableWidget_Formula_Left->blockSignals(false);
    ui.tableWidget_Formula_Right->blockSignals(false);
}

//编辑
void Pin2::onEditButtonClicked(int row)
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        //int row = button->property("row").toInt();
        QString fileName = ui.comboBox_Project_2->currentText();
        if (ui.tabWidget_Formula->currentIndex() == 0) {
            if (LSM->m_Formula.count(fileName))
            {
                //已经保存过的配方
                if (LSM->m_Formula[fileName].ListProcess_L.size() > row)
                {
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, LSM->m_Formula[fileName].ListProcess_L[row], row);
                    dlg.setEditMode(true);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.exec();
                }
                else
                {
                    //左点位
                    ENUMSTATION station = ENUMSTATION::L;
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, station, row);
                    dlg.setEditMode(true);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.exec();
                }
            }
            else
            {
                //左点位
                ENUMSTATION station = ENUMSTATION::L;
                qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, station, row);
                dlg.setEditMode(true);
                connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                dlg.exec();
            }


        }
        else if (ui.tabWidget_Formula->currentIndex() == 1)
        {
            ////右点位
            //ENUMSTATION station = ENUMSTATION::R;
            //qg_Formula dlg(station, row);

            //connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
            //dlg.exec();

            if (LSM->m_Formula.count(fileName))
            {
                //已经保存过的配方
                if (LSM->m_Formula[fileName].ListProcess_R.size() > row)
                {
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, LSM->m_Formula[fileName].ListProcess_R[row], row);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.setEditMode(true);
                    dlg.exec();
                }
                else
                {
                    //右点位
                    ENUMSTATION station = ENUMSTATION::R;
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, station, row);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.setEditMode(true);
                    dlg.exec();
                }
            }
            else
            {
                //右点位
                ENUMSTATION station = ENUMSTATION::R;
                qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, station, row);
                dlg.setEditMode(true);
                connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                dlg.exec();
            }
        }



    }
}

// 编辑步骤的槽函数
void Pin2::slotEditFormula(const int& row, const PointItemConfig& config)
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // 阻塞信号
        ui.tableWidget_Formula_Left->blockSignals(true);
        // 左点位
        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("过渡点");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin针识别点");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3D扫描点");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3D识别");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("取放料");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("拍照");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("视觉模块");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("圆测距");
            break;
        default:
            break;
        }

        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            break;
        case ENUM_VERSION::EdPin:
        case ENUM_VERSION::JmPin:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
            ui.tableWidget_Formula_Left->setItem(row, 6, new QTableWidgetItem(QString::number(config.TargetPAxis_6)));
            ui.tableWidget_Formula_Left->setItem(row, 7, new QTableWidgetItem(QString::number(config.TargetPAxis_7)));
            break;
        case ENUM_VERSION::LsPin:   //  5轴
        case ENUM_VERSION::JmHan:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
            break;
        case ENUM_VERSION::BtPin:   //  3轴
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            break;
        default:
            break;
        }

        // 恢复信号
        ui.tableWidget_Formula_Left->blockSignals(false);
    }
    else if(ui.tabWidget_Formula->currentIndex() == 1 && LSM->m_version == ENUM_VERSION::TwoPin) {
        // 阻塞信号
        ui.tableWidget_Formula_Right->blockSignals(true);
        // 右点位
        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("过渡点");
                break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin针识别点");
                break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3D扫描点");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3D识别");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("取放料");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("拍照");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("视觉模块");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("圆测距");
            break;
        default:
            break;
        }

        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // 设置数据到表格的相应单元格
            ui.tableWidget_Formula_Right->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Right->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Right->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Right->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Right->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Right->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            break;
        case ENUM_VERSION::EdPin:
            break;
        default:
            break;
        }
       // 恢复信号
       ui.tableWidget_Formula_Right->blockSignals(false);
    }



    //读当前方案
    auto name = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
    {
        //改步骤信息
        if (config.StationL == ENUMSTATION::L)
        {
            LSM->m_Formula[name].ListProcess_L[row] = config;
        }
        else if (config.StationL == ENUMSTATION::R)
        {
            LSM->m_Formula[name].ListProcess_R[row] = config;
        }
    }
}

//新建步骤插入此行之前
void Pin2::on_pushButton_InsertFormula_clicked()
{
    auto name = ui.comboBox_Project_2->currentText();
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        //左点位
        int row = ui.tableWidget_Formula_Left->currentRow();
        if (row < 0)
            return;
        ENUMSTATION station = ENUMSTATION::L;
        if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
        {
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_L, station, row);
            connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotInsertFormula);
            dlg.exec();
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1)
    {
        //右点位
        int row = ui.tableWidget_Formula_Right->currentRow();
        if (row < 0)
            return;
        ENUMSTATION station = ENUMSTATION::R;
        if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
        {
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_R, station, row);

            connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotInsertFormula);
            dlg.exec();
        }
    }
}

//插入步骤行的槽函数
void Pin2::slotInsertFormula(const int& row, const PointItemConfig& config)
{
    if (config.StationL == ENUMSTATION::L) {
        // 左工位插入
        insertFormulaRow(ui.tableWidget_Formula_Left, config, ENUMSTATION::L);
    }
    else if (config.StationL == ENUMSTATION::R) {
        // 右工位插入
        insertFormulaRow(ui.tableWidget_Formula_Right, config, ENUMSTATION::R);
    }
}


// 插入新行
void Pin2::insertFormulaRow(QTableWidget* tableWidget, const PointItemConfig& config, int station) {
    int row = tableWidget->currentRow();
    if (row < 0)
        return;
    //插入
    tableWidget->insertRow(row);

    // 创建 RowData 对象，封装数据和按钮
    //RowData rowData(config, row);

    // 设置单元格的内容
    QString action = "";
    switch (config.PointType) {
    case ENUMPOINTTYPE::PointNone:
        action = tr("过渡点");
        break;
    case ENUMPOINTTYPE::Point2D:
        action = tr("2D pin针识别点");
        break;
    case ENUMPOINTTYPE::Point3D:
        action = tr("3D扫描点");
        break;
    case ENUMPOINTTYPE::Point3D_identify:
        action = tr("3D识别");
        break;
    case ENUMPOINTTYPE::Feeding:
        action = tr("取放料");
        break;
    case ENUMPOINTTYPE::TakePhoto:
        action = tr("拍照");
        break;
    case ENUMPOINTTYPE::Visual_Identity:
        action = tr("视觉模块");
        break;
    case ENUMPOINTTYPE::Circle_Measure:
        action = tr("圆测距");
        break;
    default:
        break;
    }

    // 直接在单元格中创建按钮（无需外部对象）
    QPushButton* editButton = new QPushButton("编辑", tableWidget);
    // 连接信号（显式捕获 editButton 和 tableWidget）
    connect(editButton, &QPushButton::clicked, this, [this, editButton, tableWidget]() {
        // 通过按钮直接获取所在行号（无需坐标转换）
        QTableWidgetItem* item = tableWidget->item(tableWidget->indexAt(editButton->pos()).row(), 0);
        if (item) {
            int actualRow = item->row();
            onEditButtonClicked(actualRow);
        }
        });
    // 设置表格单元格的数据
    tableWidget->setItem(row, 0, new QTableWidgetItem(action));
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
        tableWidget->setItem(row, 6, new QTableWidgetItem(config.Tip));
        // 将编辑按钮添加到表格的"编辑"列
        //tableWidget->setCellWidget(row, 7, rowData.editButton);
        tableWidget->setCellWidget(row, 7, editButton);
        break;
    case ENUM_VERSION::EdPin:
    case ENUM_VERSION::JmPin:
		tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
		tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
		tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
		tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
		tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
		tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(config.TargetPAxis_6)));
		tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(config.TargetPAxis_7)));
		tableWidget->setItem(row, 8, new QTableWidgetItem(config.Tip));
		// 将编辑按钮添加到表格的"编辑"列
		tableWidget->setCellWidget(row, 9, editButton);
        break;
    case ENUM_VERSION::LsPin:   //  都是5轴
    case ENUM_VERSION::JmHan:
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
        tableWidget->setItem(row, 6, new QTableWidgetItem(config.Tip));
        // 将编辑按钮添加到表格的"编辑"列
        //tableWidget->setCellWidget(row, 7, rowData.editButton);
        tableWidget->setCellWidget(row, 7, editButton);
        break;
    case ENUM_VERSION::BtPin:   //  3轴
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(config.Tip));
        tableWidget->setCellWidget(row, 5, editButton);
        break;
    default:
        break;
    }


    //// 禁用编辑功能
    //for (int col = 0; col < 6; ++col) {
    //    tableWidget->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    //}
    // 禁用第一列编辑
    tableWidget->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);

#if OLD_UI
    // 创建一个 QWidget 来包含操作按钮
    QWidget* operationWidget = new QWidget();
    QHBoxLayout* operationLayout = new QHBoxLayout();
    operationLayout->setContentsMargins(0, 0, 0, 0);

    operationLayout->addWidget(rowData.btn_upMove);
    operationLayout->addWidget(rowData.btn_downMove);
    operationLayout->addWidget(rowData.btnRemove);

    operationWidget->setLayout(operationLayout);

    // 将操作按钮布局添加到"操作"列
    tableWidget->setCellWidget(row, 8, operationWidget);

    connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
    connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
    connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif

    QString formulaName = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
        rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];

        if (station == ENUMSTATION::L) {
            // 在左工位插入指定索引位置
            selectedFormula.ListProcess_L.insert(row, config);
        }
        else if (station == ENUMSTATION::R) {
            // 在右工位插入指定索引位置
            selectedFormula.ListProcess_R.insert(row, config);
        }
    }

}

//单步运行
void Pin2::on_pushButton_OneStep_clicked()
{
#ifndef DEBUG_LHC

#if LOCAL_TEST == 0
    //0611测试
    if (!LSM->isServeOn)
    {
        QMessageBox::warning(this, tr("警告"), tr("请使能后执行！"));
        return;
    }
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在运行中，运行结束或停止后点击！"));
        return;
    }
    if (!LSM->m_isHomed.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("未清除报警，请先清除报警！"));
        return;
    }
#endif

#endif
    //清理已显示的方案
    //clearLabelsInScrollArea();
    // 方案名
    QString projectName = ui.comboBox_Project_2->currentText();
    //易鼎丰初始化连接机器人通讯
    if (LSM->m_version == EdPin)
    {
        if (LSM->m_productPara.UserAccout.isEmpty())
        {
            QMessageBox::warning(this, tr("警告"), tr("用户账号（生产人）未填写，请先填写！"));
            return;
        }
        if (LSM->m_formulaToRobot.empty() || !LSM->m_formulaToRobot.count(projectName))
        {
            QMessageBox::warning(this, tr("警告"), tr("机器人方案与配方对应关系为空，请先填写！"));
            return;
        }
        
        auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
        if (!rtn)
        {
            machineLog->write("机器人ModbusTcp连接失败...", Normal);
            machineLog->write("机器人ModbusTcp连接失败...", Err);
            return;
        }
    }
    clearAllViews();
    //判断是左工位还是右工位
    bool isLeft = false;
    int ProcessListCount = ui.tableWidget_Formula_Right->rowCount();
    if (ui.tabWidget_Formula->currentIndex() == 0)
    {
        // 左工位
        isLeft = true;
        ProcessListCount = ui.tableWidget_Formula_Left->rowCount();
    }

    // 判断是否超出步骤数
    int ShowStepIndex = ui.lineEdit_RunSteps->text().toInt() - 1;
    if (ShowStepIndex < 0)
        ShowStepIndex = 0;
    if (ShowStepIndex >= ProcessListCount)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "提示", "动作执行结束，是否从第一步开始",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (reply == QMessageBox::No)
            return;

        // 从第一步开始
        ShowStepIndex = 0;
    }

    // 确认是否运行当前动作
    QMessageBox::StandardButton res;
    res = QMessageBox::question(nullptr, "提示", "是否运行当前动作",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (res != QMessageBox::Yes)
        return;

    LSM->m_isOneStep.store(true);
    LSM->m_isRobotLoading.store(false);
    // 把当前项写到运行的工位上
    if (isLeft)
    {
        LSM->m_showStepIndex_L = ShowStepIndex;
        LSM->m_runListProcess_L.clear();
        LSM->m_runListProcess_L.push_back(LSM->m_Formula[projectName].ListProcess_L[ShowStepIndex]);
        switch (LSM->m_version)
        {
        case TwoPin:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::Left_Run;
            break;
        case EdPin:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::ED_Run;
            break;
        case LsPin:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::LS_Run;
            break;
        case BtPin:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::BT_Run;
            break;
        case JmPin:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::JmPin_Run;
            break;
        case JmHan:
            LSM->m_triggerMotionTpyes = RS2::ActionFlag::JmHan_Run;
            break;
        default:
            break;
        }
    }
    else
    {
        LSM->m_showStepIndex_R = ShowStepIndex;
        LSM->m_runListProcess_R.clear();
        LSM->m_runListProcess_R.push_back(LSM->m_Formula[projectName].ListProcess_R[ShowStepIndex]);
        LSM->m_triggerMotionTpyes = RS2::ActionFlag::Right_Run;
    }
}

void Pin2::onCameraSettingTabChanged(int index)
{
    if (index != 0)
    {
        if (LSM->m_isStart.load())
            return;
        auto cameraWindow = qg_2dCameraSetting::getCameraWindow();
        if (cameraWindow == NULL)
            return;
        if (cameraWindow->m_pcMyCamera)
        {
            QMessageBox::critical(this, "错误", "请先关闭相机再切换页面！");

            // 强制切换回原来的 tab，阻止页面切换
            ui.tabWidget_CameraSetting->setCurrentIndex(0);
        }

        //if (cameraWindow->m_pcMyCamera_3D)
        //{
        //    QMessageBox::critical(this, "错误", "请先停止3D采集再切换页面！");

        //    // 强制切换回原来的 tab，阻止页面切换
        //    ui.tabWidget_CameraSetting->setCurrentIndex(0);
        //}
    }
}

//更新步骤索引ui
void Pin2::updateStepIndex_slot()
{
    if (ui.tabWidget_Formula->currentIndex() == 0 && LSM->m_showStepIndex_L < ui.tableWidget_Formula_Left->rowCount())
    {
        //选择下一行
        ui.tableWidget_Formula_Left->selectRow(LSM->m_showStepIndex_L);
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1 && LSM->m_showStepIndex_R < ui.tableWidget_Formula_Right->rowCount())
    {
        //选择下一行
        ui.tableWidget_Formula_Right->selectRow(LSM->m_showStepIndex_R);
    }
}

void Pin2::onTabWidgetTabChanged(int index)
{
    if (index != 2)
    {
        if (LSM->m_isStart.load())
            return;
        auto cameraWindow = qg_2dCameraSetting::getCameraWindow();
        if (cameraWindow == NULL)
            return;
        if (cameraWindow->m_pcMyCamera)
        {
            QMessageBox::critical(this, "错误", "请先关闭相机再切换页面！");

            // 强制切换回原来的 tab，阻止页面切换
            ui.tabWidget->setCurrentIndex(2);
        }

        //if (cameraWindow->m_pcMyCamera_3D)
        //{
        //    QMessageBox::critical(this, "错误", "请先停止3D采集再切换页面！");

        //    // 强制切换回原来的 tab，阻止页面切换
        //    ui.tabWidget->setCurrentIndex(2);
        //}
    }
}

//选中左工位改变运行步骤值
void Pin2::on_LeftRowSelected(const QModelIndex& current, const QModelIndex& previous)
{
    int selectedRow = current.row();
    ui.lineEdit_RunSteps->setText(QString::number(selectedRow + 1));
}

//上移
void Pin2::on_pushButton_UpMove_clicked()
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // 左点位
        // 判断是否是第一行，第一行不能上移
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        if (row <= 0) {
            return;
        }
        // 交换表格中的两行数据
        swapTableRows(ui.tableWidget_Formula_Left, row, 0);
        ui.tableWidget_Formula_Left->selectRow(row - 1);

        // 获取配方名称
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 交换 ListProcess_L 中对应的元素
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.swapItemsAt(row, row - 1);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) {
        // 右点位
        // 判断是否是第一行，第一行不能上移
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        if (row <= 0) {
            return;
        }
        // 交换表格中的两行数据
        swapTableRows(ui.tableWidget_Formula_Right, row, 0);
        ui.tableWidget_Formula_Right->selectRow(row - 1);

        // 获取配方名称
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 交换 ListProcess_R 中对应的元素
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.swapItemsAt(row, row - 1);
            }
        }
    }
}

//下移
void Pin2::on_pushButton_DownMove_clicked()
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // 左点位
        // 判断是否是第一行，第一行不能上移
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        int rowCount = ui.tableWidget_Formula_Left->rowCount();
        if (row >= rowCount - 1) {
            return;
        }
        // 交换表格中的两行数据
        swapTableRows(ui.tableWidget_Formula_Left, row, 1);
        ui.tableWidget_Formula_Left->selectRow(row + 1);

        // 获取配方名称
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 交换 ListProcess_L 中对应的元素
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.swapItemsAt(row, row + 1);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) {
        // 右点位
        // 判断是否是第一行，第一行不能上移
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        int rowCount = ui.tableWidget_Formula_Right->rowCount();
        if (row >= rowCount - 1) {
            return;
        }
        // 交换表格中的两行数据
        swapTableRows(ui.tableWidget_Formula_Right, row, 1);
        ui.tableWidget_Formula_Right->selectRow(row + 1);

        // 获取配方名称
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 交换 ListProcess_R 中对应的元素
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.swapItemsAt(row, row + 1);
            }
        }
    }
}

void Pin2::on_pushButton_DeleteRow_clicked()
{
    if (ui.tabWidget_Formula->currentIndex() == 0)
    {
        // 左点位
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        ui.tableWidget_Formula_Left->removeRow(row);

        // 删除 m_Formula 中 ListProcess_L 对应的元素
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 删除 ListProcess_L 中对应索引的元素
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.removeAt(row);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1)
    {
        // 右点位
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        ui.tableWidget_Formula_Right->removeRow(row);

        // 删除 m_Formula 中 ListProcess_R 对应的元素
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // 删除 ListProcess_R 中对应索引的元素
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.removeAt(row);
            }
        }
    }
}

//直接运行
void Pin2::on_pushButton_Run_clicked()
{
#ifndef DEBUG_LHC
    ////0611测试
#if LOCAL_TEST == 0
    if (LSM->m_version == LsPin)
    {
        //下层倍速链运动
        if (!doubleSpeedThread->isRunning())
        {
            doubleSpeedThread->start();
        }
        QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
    }
    if (!LSM->isServeOn)
    {
        QMessageBox::warning(this, tr("警告"), tr("请使能后执行！"));
        return;
    }
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("正在运行中，运行结束或停止后点击！"));
        return;
    }
    if (!LSM->m_isHomed.load())
    {
        QMessageBox::warning(this, tr("警告"), tr("未复位回原点，请先复位！"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("未清除报警，请先清除报警！"));
        return;
    }
#endif

#endif
    //清理已显示的方案
    //clearLabelsInScrollArea();
    clearAllViews();
    // 方案名
    QString projectName = ui.comboBox_Project_2->currentText();
    //易鼎丰初始化连接机器人通讯
    if (LSM->m_version == EdPin )
    {
        if (LSM->m_productPara.UserAccout.isEmpty())
        {
            QMessageBox::warning(this, tr("警告"), tr("用户账号（生产人）未填写，请先填写！"));
            return;
        }
        if (LSM->m_formulaToRobot.empty() || !LSM->m_formulaToRobot.count(projectName))
        {
            QMessageBox::warning(this, tr("警告"), tr("机器人方案与配方对应关系为空，请先填写！"));
            return;
        }
        if (!LSM->m_notRobot)
        {
            auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
            if (!rtn)
            {
                machineLog->write("机器人ModbusTcp连接失败...", Normal);
                machineLog->write("机器人ModbusTcp连接失败...", Err);
                return;
            }
            //清除机器人信号
            MODBUS_TCP.writeIntCoil(134, 0, 0);
            MODBUS_TCP.writeIntCoil(134, 1, 0);
            MODBUS_TCP.writeIntCoil(134, 2, 0);
            MODBUS_TCP.writeIntCoil(134, 3, 0);
            MODBUS_TCP.writeIntCoil(134, 4, 0);
            MODBUS_TCP.writeIntCoil(134, 5, 0);
        }
    }

    if (!ui.radioButton_BothRun->isChecked())
    {
        // 确认是否运行当前动作
        QMessageBox::StandardButton res;
        res = QMessageBox::question(nullptr, "提示", "是否运行当前动作",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (res != QMessageBox::Yes)
            return;

        //单工位运行
        //判断是左工位还是右工位
        bool isLeft = false;
        int ProcessListCount = ui.tableWidget_Formula_Right->rowCount();
        if (ui.tabWidget_Formula->currentIndex() == 0)
        {
            // 左工位
            isLeft = true;
            ProcessListCount = ui.tableWidget_Formula_Left->rowCount();
        }
        // 判断是否超出步骤数
        int ShowStepIndex = ui.lineEdit_RunSteps->text().toInt() - 1;
        if (ShowStepIndex < 0)
            ShowStepIndex = 0;
        if (ShowStepIndex >= ProcessListCount)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, "提示", "动作执行结束，是否从第一步开始",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

            if (reply == QMessageBox::No)
                return;

            // 从第一步开始
            ShowStepIndex = 0;
        }

        if (ShowStepIndex != 0 && ShowStepIndex != 1)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, "提示", "当前步骤不为 1 ，是否从第一步开始运行",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

            if (reply == QMessageBox::Yes)
            {
                // 从第一步开始
                ShowStepIndex = 0;
                ui.lineEdit_RunSteps->setText("1");
            }
        }

        
        LSM->m_isOneStep.store(false);
        LSM->m_isRobotLoading.store(false);
        if (LSM->m_version == EdPin && !LSM->m_notRobot)
        {
            clearRobotAuto();

            QElapsedTimer materialTimer;
            LSM->m_RobotModbus.clearState();
            //先读机器人自动状态
            auto autoStatus = false;
            MODBUS_TCP.readIntCoil(0, 1, autoStatus);
            if (!autoStatus)
            {
                QMessageBox::warning(this, tr("警告"), tr("机器人非自动状态，请先设置自动状态！"));
                return;
            }
            //先读机器人报警状态
            auto alarmStatus = false;
            MODBUS_TCP.readIntCoil(0, 4, alarmStatus);
            if (alarmStatus)
            {
                QMessageBox::warning(this, tr("警告"), tr("机器人报警，请先清除报警或复位！"));
                return;
            }
            
            //触发停止
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(300);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            Sleep(300);
            MODBUS_TCP.writeIntCoil(100, 2, 0);

            //掉使能
            MODBUS_TCP.writeIntCoil(100, 13, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 13, 1);
            Sleep(1500);

            //发送使能
            MODBUS_TCP.writeIntCoil(100, 12, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 12, 1);
            //读取伺服使能状态
            auto servoStatus = false;
            MODBUS_TCP.readIntCoil(0, 3, servoStatus);
            materialTimer.start();
            while (!servoStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("机器人读取伺服使能状态超时：" + QString::number(ERROR_TIME) + " 毫秒后还未读取到伺服使能成功", Normal);
                    MODBUS_TCP.writeIntCoil(100, 0, 0);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 3, servoStatus);
                Sleep(5);
            }

            ////加载目标程序号
            //int num = LSM->m_forMulaName.toInt();
            //if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
            //    && num != 5 && num != 6 && num != 7 &&
            //    num != 8 && num != 9)
            //{
            //    machineLog->write("配方无对应治具！！", Normal);
            //    QMessageBox msgBox(QMessageBox::Warning, "警告", "配方无对应治具，请确认配方！！", QMessageBox::Yes, this);
            //    // 显示弹窗
            //    msgBox.exec();
            //    return;
            //}

            //加载目标程序号
            machineLog->write("LSM->m_forMulaName = " + LSM->m_forMulaName, Normal);
            if (LSM->m_formulaToRobot.count(LSM->m_forMulaName))
            {
                int num = LSM->m_formulaToRobot[LSM->m_forMulaName];
                if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
                    && num != 5 && num != 6 && num != 7 &&
                    num != 8 && num != 9)
                {
                    machineLog->write("配方无对应治具机器人方案！！", Normal);
                    QMessageBox msgBox(QMessageBox::Warning, "警告", "配方无对应治具，请确认配方！！", QMessageBox::Yes, this);
                    // 显示弹窗
                    msgBox.exec();
                    return;
                }
                //写入程序号
                MODBUS_TCP.writeRegisterInt(103, num);
            }
            else
            {
                machineLog->write("未找到对应配方的机器人方案，请检查对应关系！！", Normal);
                machineLog->write("未找到对应配方的机器人方案，请检查对应关系！！", Err);
                return;
            }

            //加载程序
            MODBUS_TCP.writeIntCoil(100, 4, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 4, 0);
            //读取程序加载状态
            bool programLoadStatus = false;
            MODBUS_TCP.readIntCoil(0, 11, programLoadStatus);
            materialTimer.start();
            while (!programLoadStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("机器人程序加载状态超时：" + QString::number(ERROR_TIME) + " 毫秒后还未加载成功", Normal);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 11, programLoadStatus);
                Sleep(5);
            }
            //从第一行开始运行
            MODBUS_TCP.writeIntCoil(100, 5, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 5, 0);
            //运行
            MODBUS_TCP.writeIntCoil(100, 1, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 1, 0);
            //读远程状态
            bool programRunningStatus = false;
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            materialTimer.start();
            while (!programRunningStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("机器人程序运行状态超时：" + QString::number(ERROR_TIME) + " 毫秒后还未成功运行", Normal);
                    //运行
                    MODBUS_TCP.writeIntCoil(100, 1, 0);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
                Sleep(5);
            }

        }

        // 把需要运行的所有步骤写到运行的工位上
        if (isLeft)
        {
            LSM->m_showStepIndex_L = ShowStepIndex;
            LSM->m_runListProcess_L.clear();
            for (int i = ShowStepIndex; i < LSM->m_Formula[projectName].ListProcess_L.size(); i++)
            {
                LSM->m_runListProcess_L.push_back(LSM->m_Formula[projectName].ListProcess_L[i]);
            }

            switch (LSM->m_version)
            {
            case TwoPin:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::Left_Run;
                break;
            case EdPin:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::ED_Run;
                break;
            case LsPin:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::LS_Run;
                break;
            case BtPin:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::BT_Run;
                break;
            case JmPin:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::JmPin_Run;
                break;
            case JmHan:
                LSM->m_triggerMotionTpyes = RS2::ActionFlag::JmHan_Run;
                break;
            default:
                break;
            }
            if (LSM->m_version == LsPin)
            {
                //下层倍速链运动
                if (!doubleSpeedThread->isRunning())
                {
                    doubleSpeedThread->start();
                    QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
                }
            }
            else if (LSM->m_version == EdPin)
            {
                if (!robotLoadEDThread->isRunning())
                {
                    robotLoadEDThread->start();
                    QMetaObject::invokeMethod(robotLoadEDObject, &Thread::robotLoadingAction, Qt::QueuedConnection);
                }
            }
        }
        else
        {
            LSM->m_showStepIndex_R = ShowStepIndex;
            LSM->m_runListProcess_R.clear();
            for (int i = ShowStepIndex; i < LSM->m_Formula[projectName].ListProcess_R.size(); i++)
            {
                LSM->m_runListProcess_R.push_back(LSM->m_Formula[projectName].ListProcess_R[i]);
            }

            LSM->m_triggerMotionTpyes = RS2::ActionFlag::Right_Run;
            if (LSM->m_version == LsPin)
            {
                //下层倍速链运动
                if (!doubleSpeedThread->isRunning())
                {
                    doubleSpeedThread->start();
                    QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
                }
            }
        }
    }
    else
    {
        //双工位运行
        //先左工位
        ui.tabWidget_Formula->setCurrentIndex(0);
        LSM->m_showStepIndex_L = 0;
        LSM->m_runListProcess_L.clear();
        for (int i = 0; i < LSM->m_Formula[projectName].ListProcess_L.size(); i++)
        {
            LSM->m_runListProcess_L.push_back(LSM->m_Formula[projectName].ListProcess_L[i]);
        }
        LSM->m_showStepIndex_R = 0;
        LSM->m_runListProcess_R.clear();
        for (int i = 0; i < LSM->m_Formula[projectName].ListProcess_R.size(); i++)
        {
            LSM->m_runListProcess_R.push_back(LSM->m_Formula[projectName].ListProcess_R[i]);
        }
        LSM->m_triggerMotionTpyes = RS2::ActionFlag::Both_Run;
        if (LSM->m_version == LsPin)
        {
            //下层倍速链运动
            if (!doubleSpeedThread->isRunning())
            {
                doubleSpeedThread->start();
                QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
            }
        }
    }
  
    //记录操作时间
    QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    LSM->m_snResult.op_time = timestamp;
}

//日志更新到控件
void Pin2::logUpdate_slot(QString mes, int type)
{
    if (type == Normal)
    {
        ui.textEdit_Log->append(mes);
        ui.textEdit_Log2->append(mes);
    }
    else if (type == Mes)
    {
        ui.textEdit_mesLog->append(mes);
    }
}

//日志刷新
void Pin2::on_pushButton_clearLog_clicked()
{  
    ui.textEdit_Log->clear();
    ui.textEdit_Log2->clear();
}

// 展示2D识别结果
void Pin2::showPin_Slot(ResultParamsStruct_Pin result , PointItemConfig config)
{
    QString filePath = "";
    HObject originaImg;
    HObject resultImg;
    machineLog->write("2D正在输出结果...", Normal);
    try {
        //if (!result.ho_Image.IsInitialized())
        //    return;
        if (result.ho_Image.Key() == nullptr)
        {
            return;
        }
        HObject img = result.ho_Image;
        if (result.ho_Region_AllPin.Key() == nullptr)
        {
            return;
        }

        //输出结果到数据集
        auto sql = qg_QSLiteWidget::getSqlWindow();
        QString photoPath;
        //保存图片
        //saveHalconJPG(img, photoPath);
        //保存图片到型号文件夹
        if (config.ModelName.isEmpty())
        {
            //如果没有型号，写配方方案名
            config.ModelName = ui.label_ProjectName->text();
        }
        if (ui.checkBox_SaveImg->isChecked())
        {
            //saveHalconJPGToFile(img, config.ModelName, config.ProductName, false, picPath);
            originaImg = img.Clone();
        }
        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);
        // 新 创建 pinLabel 对象
        pinLabel label;

        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        label.setResult(result);
        // 显示图像
        label.setImage(img);
        label.allPinImage(result.ho_Region_AllPin);
        label.showImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //新显示
        showInGraphView(showQImg);
        resultImg = showHjImg;

        //数据库
        SQL_Parameter para;
        para.formulaName = config.Vision;
        if (!LSM->testSN.isEmpty())
        {
            para.snName = LSM->testSN;
        }
        else if (!LSM->m_snReq.sn.isEmpty())
        {
            para.snName = LSM->m_snReq.sn;
        }
        if (para.snName.isEmpty())
        {
            para.snName = "none";
        }
        //添加结果到2D数据库
        sql->addNew2DResult(result, para);
        //mes结果
        TestResult mes;
        mes.name = config.ProductName;
        mes.result = result.hv_Ret == 1 ? "PASS" : "FAIL";
        ////必过
        //mes.result = result.hv_Ret == "PASS";
        LSM->m_snResult.result_data.push_back(mes);
 
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showPin_Slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }

    try
    {
        if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            //saveHalconJPGToFile(originaImg, config.ModelName, config.ProductName, false, filePath);
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.ProductName);
        }
        //保存结果
        //saveHalconJPGToFile(resultImg, config.ModelName, config.ProductName, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.ProductName);
        //m_resultImgPath = filePath;
        //保存立昇mes图片
        QString machinename = ui.comboBox_Project_2->currentText();
        if (LSM->m_version == LsPin && resultImg.IsInitialized() && (machinename == "A66" || machinename == "A66-T"))
        {
            saveHalconJPGToFile_LS(resultImg, machinename, config, true, filePath);
        }

        //保存NG图片
        bool isNG = result.hv_Ret == 0;
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("图片保存失败", Normal);
    }
}

// 展示2D和3D识别结果
void Pin2::showPin2Dand3D_Slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)
{
    QString filePath = "";
    HObject originaImg;
    HObject resultImg;
    machineLog->write("2Dand3D正在输出结果...", Normal);
    try {
        if (!result2D.ho_Image.IsInitialized())
            return;
        HObject img = result2D.ho_Image;
        if (result2D.ho_Region_AllPin.Key() == nullptr)
        {
            return;
        }

        //输出结果到数据集
        auto sql = qg_QSLiteWidget::getSqlWindow();
        QString photoPath;
        //保存图片
        //saveHalconJPG(img, photoPath);
        //保存图片到型号文件夹
        if (config2D.ModelName.isEmpty())
        {
            //如果没有型号，写配方方案名
            config2D.ModelName = ui.label_ProjectName->text();
        }
        if (ui.checkBox_SaveImg->isChecked())
        {
            //saveHalconJPGToFile(img, config2D.ModelName, config2D.ProductName, false, photoPath);
            originaImg = img;
        }
        SQL_Parameter para;
        para.formulaName = config2D.Vision;
        if (!LSM->testSN.isEmpty())
        {
            para.snName = LSM->testSN;
        }
        else if (!LSM->m_snReq.sn.isEmpty())
        {
            para.snName = LSM->m_snReq.sn;
        }
        if (para.snName.isEmpty())
        {
            para.snName = "none";
        }
        //添加结果到2D数据库
        sql->addNew2DResult(result2D, para);
        para.formulaName = config3D.Vision;
        if (!LSM->testSN.isEmpty())
        {
            para.snName = LSM->testSN;
        }
        else if (!LSM->m_snReq.sn.isEmpty())
        {
            para.snName = LSM->m_snReq.sn;
        }
        if (para.snName.isEmpty())
        {
            para.snName = "none";
        }
        //添加结果到3D数据库
        sql->addNew3DResult(result3D, para);

        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);

        //新 创建 pinLabel 对象
        pinLabel label;

        label.setResult(result2D, result3D);
        // 显示图像
        label.setImage(img);
        label.showImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //新显示
        showInGraphView(showQImg);
        resultImg = showHjImg;
        ////保存结果
        //saveHalconJPGToFile(showHjImg, config2D.ModelName, config2D.ProductName, true, photoPath);
        //m_resultImgPath = photoPath;
        ////保存立昇mes图片
        //QString machinename = ui.comboBox_Project_2->currentText();
        //if (LSM->m_version == LsPin && (machinename == "A66" || machinename == "A66-T"))
        //{
        //    saveHalconJPGToFile_LS(showHjImg, machinename, config2D, true, photoPath);
        //}

        //mes结果
        TestResult mes;
        mes.name = config2D.ProductName;
        bool tp = (result2D.hv_Ret == 1 && result3D.isOK == true);
        mes.result = tp ? "PASS" : "FAIL";
        ////必过
        //mes.result = "PASS";
        LSM->m_snResult.result_data.push_back(mes);
        //// 获取滚动区域的布局
        //QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();

        //if (layout == nullptr) {
        //    layout = new QHBoxLayout();
        //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
        //}
        //// 设置控件之间的间隔
        //layout->setSpacing(20);
        //// 将 pinLabel 添加到布局中
        //layout->addWidget(label);
        //// 容器内的内容超出时自动调整
        //ui.scrollArea_ShowPin->setWidgetResizable(true);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showPin_Slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }

    try
    {
        if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            //saveHalconJPGToFile(originaImg, config2D.ModelName, config2D.ProductName, false, filePath);
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config2D.ProductName);
        }
        //保存结果
        //saveHalconJPGToFile(resultImg, config2D.ModelName, config2D.ProductName, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config2D.ProductName);
        //m_resultImgPath = filePath;
        //保存立昇mes图片
        QString machinename = ui.comboBox_Project_2->currentText();
        if (LSM->m_version == LsPin && resultImg.IsInitialized() && (machinename == "A66" || machinename == "A66-T"))
        {
            saveHalconJPGToFile_LS(resultImg, machinename, config2D, true, filePath);
        }
        //保存NG图片
        bool isNG = (result2D.hv_Ret == 0 || result3D.isOK == false);
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config2D.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("图片保存失败", Normal);
    }
}

//切换工位表格
void Pin2::changeStationUI_slot()
{
    //切到右工位
    ui.tabWidget_Formula->setCurrentIndex(1);
}

//选中右工位改变运行步骤值
void Pin2::on_RightRowSelected(const QModelIndex& current, const QModelIndex& previous)
{
    int selectedRow = current.row();
    ui.lineEdit_RunSteps->setText(QString::number(selectedRow + 1));
}

//工位标签切换时，步骤复原
void Pin2::on_tabWidget_FormulaChanged(int index) {
    ui.lineEdit_RunSteps->setText("0");
    // 取消行选择
    ui.tableWidget_Formula_Left->clearSelection();
    ui.tableWidget_Formula_Right->clearSelection();
}

//清除pin识别结果
void Pin2::clearLabelsInScrollArea()
{
    // 获取布局
    QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();
    if (layout != nullptr) {
        // 遍历布局中的所有控件
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            // 删除控件
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();  // 延迟删除
            }
            // 删除布局项
            delete item;
        }
    }
    //if (layout == nullptr) {
    //    layout = new QHBoxLayout();
    //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
    //}

    LSM->m_pinResult2D.clear();
}

//清除结果
void Pin2::on_pushButton_ResultClear_clicked()
{
    //clearLabelsInScrollArea();
    clearAllViews();
}


//写入csv
int Pin2::writeCsv(const QString configPath, const QList<std::pair<ResultParamsStruct_Pin, QString>> resultParamsList)
{
    try
    {
        // 创建目录（保持原逻辑）
        QFileInfo fileInfo(configPath);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists() && !dir.mkpath(".")) {
            std::cerr << "无法创建目录: " << dir.path().toStdString() << std::endl;
            return 1;
        }

        string path = configPath.toUtf8().data();

        // 以追加模式打开文件，如果不存在则创建
        std::ofstream csvFile(path, std::ios::app);
        if (!csvFile.is_open()) {
            std::cerr << "无法打开文件以进行写入" << std::endl;
            return 1;
        }

        // 检查文件是否为空（新创建的文件）
        bool writeHeader = false;
        {
            // 以输入模式打开文件，检查是否为空
            std::ifstream checkFile(path);
            writeHeader = checkFile.peek() == std::ifstream::traits_type::eof();
        }

        // 如果需要写入标题行
        if (writeHeader) {
            // 调整标题顺序：配方名 -> 结果 -> 其他列
            csvFile << "配方名,结果,X,Y,DistanceX_Standard-Pixel,DistanceY_Standard-Pixel,"
                << "OffsetX-Pixel,OffsetY-Pixel,DistanceX_Standard-MM,DistanceY_Standard-MM,"
                << "OffsetX-MM,OffsetY-MM\n";
        }

        // 遍历每个配方
        for (const auto& oneResult : resultParamsList) {
            const auto& resultParams = oneResult.first;
            const QString& recipeName = oneResult.second; // 提取配方名

            // 记录NG索引
            unordered_set<int> ngIdxSet(resultParams.ngIdx.begin(), resultParams.ngIdx.end());

            // 遍历数据点
            for (int i = 0; i < resultParams.hv_Row_Array.TupleLength().I(); ++i) {
                // 构建数据行：配方名 -> 结果 -> 数据
                std::vector<std::string> rowData = {
                    recipeName.toStdString(),                   // 配方名（所有行相同）
                    ngIdxSet.count(i) ? "NG" : "OK",            // 结果
                    std::to_string(resultParams.hv_Column_Array[i].D()),
                    std::to_string(resultParams.hv_Row_Array[i].D()),
                    std::to_string(resultParams.hv_DisX_Pixel_Array[i].D()),
                    std::to_string(resultParams.hv_DisY_Pixel_Array[i].D()),
                    std::to_string(resultParams.hv_OffsetX_Pixel_Array[i].D()),
                    std::to_string(resultParams.hv_OffsetY_Pixel_Array[i].D()),
                    std::to_string(resultParams.hv_DisX_MM_Array[i].D()),
                    std::to_string(resultParams.hv_DisY_MM_Array[i].D()),
                    std::to_string(resultParams.hv_OffsetX_MM_Array[i].D()),
                    std::to_string(resultParams.hv_OffsetY_MM_Array[i].D())
                };

                // 写入CSV行
                csvFile << boost::algorithm::join(rowData, ",") << "\n";
            }
        }

        csvFile.close();
        std::cout << "CSV文件已成功写入" << std::endl;
        return 0;
    }
    catch (...)
    {
        return 1;
    }
}

//删除方案
void Pin2::on_pushButton_Cancel_clicked()
{
    // 获取exe目录下的Formula_Config文件夹路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // 如果Formula_Config文件夹不存在，则创建
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QString fileName = ui.comboBox_Project_2->currentText();
    // 构造文件的完整路径
    QString filePath = dir.filePath(fileName + ".json");

    // 判断文件是否已存在
    if (QFile::exists(filePath)) {
        // 文件已存在，弹出确认框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("删除"), tr("是否删除此方案？"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
        //删除
        QFile::remove(filePath);
    }
    // 清空配方表和方案，重新加载
    ui.tableWidget_Formula_Left->clearContents();
    ui.tableWidget_Formula_Right->clearContents();
    ui.comboBox_Project_2->clear();
    // 获取文件夹中所有文件（不包括子文件夹）
    dir.setFilter(QDir::Files);  // 只获取文件，不包括子文件夹
    dir.setSorting(QDir::Time | QDir::Reversed);  // 按创建时间排序，新的文件排在前面

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        QString name = fileInfo.baseName();
        ui.comboBox_Project_2->addItem(name);
    }
}

/*
* 修改单元格时，配方UI表格参数更新到数据(左工位)

代码里对表格操作的话要屏蔽掉这个信号，即ui->tableWidget_Formula_Left->blockSignals(true)，否则会非常频繁触发此函数
*/
void Pin2::onLeftCellDoubleClicked(int row, int column)
{
    // 获取被双击的单元格的内容
    QTableWidgetItem* item = ui.tableWidget_Formula_Left->item(row, column);
    if (item) {
        QString cellText = item->text();

        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            if (row >= LSM->m_Formula[formulaName].ListProcess_L.size())
                return;
            switch (LSM->m_version)
            {
            case ENUM_VERSION::TwoPin:
                switch (column)
                {
                case 1:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPX = cellText.toDouble();
                    break;
                case 2:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPY = cellText.toDouble();
                    break;
                case 3:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPZ = cellText.toDouble();
                    break;
                case 4:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPRZ = cellText.toDouble();
                    break;
                case 5:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPU = cellText.toDouble();
                    break;
                case 6:
                    LSM->m_Formula[formulaName].ListProcess_L[row].Tip = cellText;
                    break;
                default:
                    break;
                }
                break;
            case ENUM_VERSION::LsPin:
            case ENUM_VERSION::JmHan:
                switch (column)
                {
                case 1:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_1 = cellText.toDouble();
                    break;
                case 2:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_2 = cellText.toDouble();
                    break;
                case 3:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_3 = cellText.toDouble();
                    break;
                case 4:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_4 = cellText.toDouble();
                    break;
                case 5:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_5 = cellText.toDouble();
                    break;
                case 6:
                    LSM->m_Formula[formulaName].ListProcess_L[row].Tip = cellText;
                    break;
                default:
                    break;
                }
                break;
            case ENUM_VERSION::EdPin:
            case ENUM_VERSION::JmPin:
                switch (column)
                {
                case 1:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_1 = cellText.toDouble();
                    break;
                case 2:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_2 = cellText.toDouble();
                    break;
                case 3:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_3 = cellText.toDouble();
                    break;
                case 4:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_4 = cellText.toDouble();
                    break;
                case 5:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_5 = cellText.toDouble();
                    break;
                case 6:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_6 = cellText.toDouble();
                    break;
                case 7:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_7 = cellText.toDouble();
                    break;
                case 8:
                    LSM->m_Formula[formulaName].ListProcess_L[row].Tip = cellText;
                    break;
                default:
                    break;
                }
                break;
            case ENUM_VERSION::BtPin:
                switch (column)
                {
                case 1:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_1 = cellText.toDouble();
                    break;
                case 2:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_2 = cellText.toDouble();
                    break;
                case 3:
                    LSM->m_Formula[formulaName].ListProcess_L[row].TargetPAxis_3 = cellText.toDouble();
                    break;
                case 4:
                    LSM->m_Formula[formulaName].ListProcess_L[row].Tip = cellText;
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
}

/*
* 修改单元格时，配方UI表格参数更新到数据(左工位)

代码里对表格操作的话要屏蔽掉这个信号，即ui->tableWidget_Formula_Left->blockSignals(true)，否则会非常频繁触发此函数
*/
void Pin2::onRightCellDoubleClicked(int row, int column)
{
    // 获取被双击的单元格的内容
    QTableWidgetItem* item = ui.tableWidget_Formula_Right->item(row, column);
    if (item) {
        QString cellText = item->text();

        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            if (row >= LSM->m_Formula[formulaName].ListProcess_R.size())
                return;
            switch (LSM->m_version)
            {
            case ENUM_VERSION::TwoPin:
                switch (column)
                {
                case 1:
                    LSM->m_Formula[formulaName].ListProcess_R[row].TargetPX = cellText.toDouble();
                    break;
                case 2:
                    LSM->m_Formula[formulaName].ListProcess_R[row].TargetPY = cellText.toDouble();
                    break;
                case 3:
                    LSM->m_Formula[formulaName].ListProcess_R[row].TargetPZ = cellText.toDouble();
                    break;
                case 4:
                    LSM->m_Formula[formulaName].ListProcess_R[row].TargetPRZ = cellText.toDouble();
                    break;
                case 5:
                    LSM->m_Formula[formulaName].ListProcess_R[row].TargetPU = cellText.toDouble();
                    break;
                case 6:
                    LSM->m_Formula[formulaName].ListProcess_R[row].Tip = cellText;
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
}

// 保存一个流程配方
void Pin2::saveOneFormulaToFile(QString formulaName)
{
    // 获取Formula_Config目录路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (LSM->m_Formula.count(formulaName))
    {
        rs_motion::Formula_Config formulaConfig = LSM->m_Formula[formulaName];  // 获取配方数据

        QJsonObject formulaJson;
        //ListProcess_L
        QJsonArray listProcessLArray;
        for (const auto& point : formulaConfig.ListProcess_L) {
            QJsonObject pointJson;
            pointJson["StationL"] = point.StationL;
            pointJson["PointType"] = point.PointType;
            switch (LSM->m_version)
            {
            case ENUM_VERSION::TwoPin:
                pointJson["TargetPX"] = point.TargetPX;
                pointJson["TargetPY"] = point.TargetPY;
                pointJson["TargetPZ"] = point.TargetPZ;
                pointJson["TargetPRZ"] = point.TargetPRZ;
                pointJson["TargetPU"] = point.TargetPU;
                break;
            case ENUM_VERSION::EdPin:
            case ENUM_VERSION::JmPin:
                //易鼎丰轴参数
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                pointJson["TargetPAxis_6"] = point.TargetPAxis_6;
                pointJson["TargetPAxis_7"] = point.TargetPAxis_7;
                break;
            case ENUM_VERSION::LsPin: 
            case ENUM_VERSION::JmHan:
                //立昇轴参数
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                break;
            case ENUM_VERSION::BtPin:
                //博泰轴参数
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                break;
            default:
                break;
            }
            pointJson["SpeedModel"] = point.SpeedModel;
            pointJson["Vision"] = point.Vision;
            pointJson["Tip"] = point.Tip;
            pointJson["CloudName"] = point.CloudName;
            pointJson["ProductName"] = point.ProductName;
            pointJson["FeedAction"] = point.FeedAction;
            pointJson["CameraName"] = point.CameraName;
            pointJson["PhotoName"] = point.PhotoName;
            pointJson["VisionConfig"] = point.VisionConfig;
            pointJson["isResultMerging"] = point.isResultMerging;
            pointJson["scanRow3d"] = point.scanRow3d;
            pointJson["CircleBenchmark"] = point.CircleBenchmark;
            pointJson["CircleUpError"] = point.CircleUpError;
            pointJson["CircleDownError"] = point.CircleDownError;
            pointJson["VisionTwo"] = point.VisionTwo;
            pointJson["PixDistance"] = point.PixDistance;
            pointJson["ExposureTime"] = point.ExposureTime;
            

            listProcessLArray.append(pointJson);
        }
        formulaJson["ListProcess_L"] = listProcessLArray;

        if (LSM->m_version == ENUM_VERSION::TwoPin)
        {
            //ListProcess_R
            QJsonArray listProcessRArray;
            for (const auto& point : formulaConfig.ListProcess_R) {
                QJsonObject pointJson;
                pointJson["StationL"] = point.StationL;
                pointJson["PointType"] = point.PointType;
                pointJson["TargetPX"] = point.TargetPX;
                pointJson["TargetPY"] = point.TargetPY;
                pointJson["TargetPZ"] = point.TargetPZ;
                pointJson["TargetPRZ"] = point.TargetPRZ;
                pointJson["TargetPU"] = point.TargetPU;
                pointJson["SpeedModel"] = point.SpeedModel;
                pointJson["Vision"] = point.Vision;
                pointJson["Tip"] = point.Tip;
                pointJson["CloudName"] = point.CloudName;
                pointJson["ProductName"] = point.ProductName;
                pointJson["FeedAction"] = point.FeedAction;
                pointJson["CameraName"] = point.CameraName;
                pointJson["PhotoName"] = point.PhotoName;
                pointJson["VisionConfig"] = point.VisionConfig;
                pointJson["isResultMerging"] = point.isResultMerging;
                pointJson["scanRow3d"] = point.scanRow3d;
                pointJson["CircleBenchmark"] = point.CircleBenchmark;
                pointJson["CircleUpError"] = point.CircleUpError;
                pointJson["CircleDownError"] = point.CircleDownError;
                pointJson["VisionTwo"] = point.VisionTwo;
                pointJson["PixDistance"] = point.PixDistance;
                pointJson["ExposureTime"] = point.ExposureTime;

                listProcessRArray.append(pointJson);
            }
            formulaJson["ListProcess_R"] = listProcessRArray;
        }
       
        // 将 QJsonObject 转换为 QJsonDocument
        QJsonDocument doc(formulaJson);

        // 保存 JSON 数据到文件
        QString filePath = dir.filePath(formulaName + ".json");
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }
    }
}

// 展示3D识别结果图
void Pin2::showPin3D_Slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)
{
    machineLog->write("3D正在输出结果...", Normal);
#ifdef DEBUG_LHC
    //测试数据集
    auto sql = qg_QSLiteWidget::getSqlWindow();
    sql->addNew3DResult(result, config.Vision);
#endif // DEBUG_LHC

    try {
        if (result.depthImg.empty())
            return;

        //输出结果到数据集
        auto sql = qg_QSLiteWidget::getSqlWindow();
        SQL_Parameter para;
        para.formulaName = config.Vision;
        if (!LSM->testSN.isEmpty())
        {
            para.snName = LSM->testSN;
        }
        else if (!LSM->m_snReq.sn.isEmpty())
        {
            para.snName = LSM->m_snReq.sn;
        }
        if (para.snName.isEmpty())
        {
            para.snName = "none";
        }
        sql->addNew3DResult(result, para);

        // 新 创建 pinLabel 对象
        pinLabel label;

        HObject img = label.HMatToHObject(result.depthImg);
        if (!img.IsInitialized())
            return;
        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);

        label.setResult(result);
        // 显示图像
        label.setImage(img);
        label.show3DImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //新显示
        showInGraphView(showQImg);

        //// 获取滚动区域的布局
        //QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();

        //if (layout == nullptr) {
        //    layout = new QHBoxLayout();
        //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
        //}
        //// 设置控件之间的间隔
        //layout->setSpacing(20);
        //// 将 pinLabel 添加到布局中
        //layout->addWidget(label);
        //// 容器内的内容超出时自动调整
        //ui.scrollArea_ShowPin->setWidgetResizable(true);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showPin3D_Slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

// 设置控制模块状态
void Pin2::setControlEnable_slot(bool state)
{
    //emit jogButtonEnable_signal(state);
    try
    {
        if (state)
        {
            ui.comboBox_Project_2->setEnabled(true);
            ui.pushButton_AddProject->setEnabled(true);
            ui.pushButton_Save->setEnabled(true);
            ui.pushButton_Cancel->setEnabled(true);
            ui.lineEdit_RunSteps->setEnabled(true);
            ui.radioButton_BothRun->setEnabled(true);
            ui.pushButton_Run->setEnabled(true);
            ui.pushButton_OneStep->setEnabled(true);
            ui.pushButton_AddFormula->setEnabled(true);
            ui.pushButton_InsertFormula->setEnabled(true);
            ui.pushButton_UpMove->setEnabled(true);
            ui.pushButton_DownMove->setEnabled(true);
            ui.pushButton_DeleteRow->setEnabled(true);
            ui.tabWidget_Formula->setEnabled(true);
            //ui.groupBox_2->setEnabled(true);
            ui.pushButton_init->setEnabled(true);
            ui.pushButton_Reset->setEnabled(true);
            ui.pushButton_CopyNew->setEnabled(true);
            //ui.checkBox_Loop->setEnabled(true);
            //轴控制栏
            ui.pushButton_AxisOn->setEnabled(true);
            ui.pushButton_AxisReset->setEnabled(true);
            ui.comboBox_AxisNum->setEnabled(true);
            ui.pushButton_Homing->setEnabled(true);
            ui.pushButton_RobotProject->setEnabled(true);
            //更新用户可操作控件，以防部分控件权限被放出
            initUser();
        }
        else
        {
            ui.comboBox_Project_2->setEnabled(false);
            ui.pushButton_AddProject->setEnabled(false);
            ui.pushButton_Save->setEnabled(false);
            ui.pushButton_Cancel->setEnabled(false);
            ui.lineEdit_RunSteps->setEnabled(false);
            ui.radioButton_BothRun->setEnabled(false);
            ui.pushButton_Run->setEnabled(false);
            ui.pushButton_OneStep->setEnabled(false);
            ui.pushButton_AddFormula->setEnabled(false);
            ui.pushButton_InsertFormula->setEnabled(false);
            ui.pushButton_UpMove->setEnabled(false);
            ui.pushButton_DownMove->setEnabled(false);
            ui.pushButton_DeleteRow->setEnabled(false);
            ui.tabWidget_Formula->setEnabled(false);
            //ui.groupBox_2->setEnabled(false);
            ui.pushButton_init->setEnabled(false);
            ui.pushButton_Reset->setEnabled(false);
            ui.pushButton_CopyNew->setEnabled(false);
            //ui.checkBox_Loop->setEnabled(false);
            //轴控制栏
            ui.pushButton_AxisOn->setEnabled(false);
            ui.pushButton_AxisReset->setEnabled(false);
            ui.comboBox_AxisNum->setEnabled(false);
            ui.pushButton_Homing->setEnabled(false);
            ui.pushButton_RobotProject->setEnabled(false);
        }
    }
    catch (const std::exception&)
    {
        return;
    }
    return;
}

//查找可保存文件路径
QString Pin2::findOrCreateSaveFile()
{
    QString exePath = QCoreApplication::applicationDirPath();
    QDir photoDir(exePath + "/2D_Photo");

    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            return QString();
        }
    }

    // 从 1.jpg 开始查找
    int fileIndex = 1;
    QString fileName;
    do {
        fileName = photoDir.filePath(QString::number(fileIndex) + ".jpg");
        fileIndex++;
    } while (QFile::exists(fileName));  // 如果文件已存在，则继续增加索引

    // 返回最终的文件路径
    return fileName;
}

//保存halcon图片
int Pin2::saveHalconJPG(HObject img, QString& filePath)
{
    if (!img.IsInitialized())
        return -1;

    auto name = findOrCreateSaveFile();
    WriteImage(img, "jpeg 60", 0, name.toUtf8().data());
    filePath = name;
    return 0;
}

//复制并新建配方
void Pin2::on_pushButton_CopyNew_clicked()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("输入文件名"), tr("请输入文件名："), QLineEdit::Normal, "", &ok);

    if (!ok || fileName.isEmpty()) {
        return;
    }

    // 获取exe目录下的Formula_Config文件夹路径
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // 如果Formula_Config文件夹不存在，则创建
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::critical(this, tr("错误"), tr("无法创建文件夹！"));
            return;
        }
    }

    // 构造文件的完整路径
    QString filePath = dir.filePath(fileName + ".json");

    // 判断文件是否已存在
    bool isReply = false;
    if (QFile::exists(filePath)) {
        // 文件已存在，弹出确认框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("文件已存在"), tr("文件已存在，是否覆盖？"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
        isReply = true;
    }

    // 创建并写入 JSON 文件
    QJsonObject jsonObject;

    QJsonDocument jsonDoc(jsonObject);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入！");
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();

    // 获取当前选择的项目
    QString selectedItem = ui.comboBox_Project_2->currentText();

    // 如果配方存在，更新表格
    auto it = LSM->m_Formula.find(selectedItem);
    if (it != LSM->m_Formula.end()) {
        //存到配方方案里
        LSM->m_Formula[fileName] = LSM->m_Formula[selectedItem];
        if (!isReply)
        {
            ui.comboBox_Project_2->addItem(fileName);
        }
        // 设置当前选项为新建的文件名
        ui.comboBox_Project_2->setCurrentText(fileName);
        //保存
        saveOneFormulaToFile(fileName);
    }
}

//传输当天NG率
void Pin2::showNG_Slot(double value)
{
    ui.label_6->clear();
    ui.label_6->setText("当前NG率："+QString::number(value)+"%");
    //ui.widget_showNG->setNgValue(value);
    ui.widget_showNG->addDataPoint("NG", value);
    ui.widget_showNG->addDataPoint("OK", 100 - value);
}

//根据版本更新UI
void Pin2::updataUiFromVersion()
{
    ui.tabWidget_CameraSetting->setTabVisible(3, false);  // 隐藏一维码；
    ui.tabWidget_CameraSetting->setTabVisible(4, false);  // 隐藏二维码；
    ui.tabWidget_CameraSetting->setTabVisible(5, false);  // 隐藏模板匹配；
    ui.tabWidget_CameraSetting->setTabVisible(6, false);  // 隐藏Blob；
    ui.tabWidget_CameraSetting->setTabVisible(7, false);  // 隐藏圆检测；
    ui.tabWidget_CameraSetting->setTabVisible(8, false);  // 隐藏机器定位；
    //机器人和配方对应页面
    ui.pushButton_RobotProject->hide();
    //不常用，暂时隐藏
    ui.pushButton_init->hide();
    ui.pushButton_Reset->hide();
    ui.pushButton_Stop->hide();
    QPixmap pix(":/icons/jiangzhun.png");
    ui.label_9->setPixmap(pix);
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
    {
        break;
    }
    case ENUM_VERSION::EdPin:
    {
        //易鼎丰
#if LOCAL_TEST == 0
        QPixmap pix2(":/icons/易鼎丰.png");
#else
        QPixmap pix2(":/icons/jiangzhun.png");
#endif
        ui.label_9->setPixmap(pix2);
        ui.tabWidget_Formula->setTabText(0, "工位");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // 隐藏右工位
        ui.label->clear();
        ui.label->setText("3D视觉检查机");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0  Y:0  U:0  U1(治具):0  Z:0  Z1(正面):0  Z2(反面):0");
        //ui.tabWidget_Jog->removeTab(1);  // 移除IO页
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("U1");
        ui.comboBox_AxisNum->addItem("Z");
        ui.comboBox_AxisNum->addItem("Z1");
        ui.comboBox_AxisNum->addItem("Z2");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // 一维码；
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // 二维码；
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // 模板匹配；
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob；
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // 圆检测；
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // 机器定位；
        //机器人和配方对应页面
        ui.pushButton_RobotProject->show();
        break;
    }
    case ENUM_VERSION::LsPin:
    {
        //立昇
        ui.tabWidget_Formula->setTabText(0, "工位");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // 隐藏右工位
        ui.label->clear();
        ui.label->setText("立昇Pin标机");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0  Y1:0  Y2:0  U:0  Z:0");
        //ui.tabWidget_Jog->removeTab(1);  // 移除IO页
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y1");
        ui.comboBox_AxisNum->addItem("Y2");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("Z");
        break;
    }
    case ENUM_VERSION::BtPin:
    {
        //博泰
        ui.tabWidget_Formula->setTabText(0, "工位");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // 隐藏右工位
        ui.label->clear();
        ui.label->setText("博泰Pin标机");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0  Y1:0  Y2:0");
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y1");
        ui.comboBox_AxisNum->addItem("Y2");
        break;
    }
    case ENUM_VERSION::JmPin:
    {
        //金脉pin
        ui.tabWidget_Formula->setTabText(0, "工位");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // 隐藏右工位
        ui.label->clear();
        ui.label->setText("金脉Pin机");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setTextFormat(Qt::RichText);
        ui.label_7->setText("X:0；X1(侧相机):0；Y:0；U:0；<br>U1(治具):0；Z:0；Z1(侧相机):0；");
        //ui.tabWidget_Jog->removeTab(1);  // 移除IO页
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("X1");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("U1");
        ui.comboBox_AxisNum->addItem("Z");
        ui.comboBox_AxisNum->addItem("Z1");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // 一维码；
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // 二维码；
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // 模板匹配；
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob；
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // 圆检测；
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // 机器定位；
        break;
    }
    case ENUM_VERSION::JmHan:
    {
        //金脉焊缝
        ui.tabWidget_Formula->setTabText(0, "工位");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // 隐藏右工位
        ui.label->clear();
        ui.label->setText("金脉焊缝检测机");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0；Y:0；Y1:0；U:0；Z:0；");
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("Y1");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("Z");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // 一维码；
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // 二维码；
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // 模板匹配；
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob；
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // 圆检测；
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // 机器定位；
        break;
    }
    default:
        break;
    }

#if LOCAL_TEST == 1
    ui.label->setText("测试版软件");
#endif

    if (LSM->m_version == ENUM_VERSION::LsPin)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //创建易鼎丰(或立昇)对应的新的控件
        qg_JogButton_ED* newJogWidget = new qg_JogButton_ED(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
        //bool isConnected = connect(
        //    this, &Pin2::jogButtonEnable_signal,
        //    newJogWidget, &qg_JogButton_ED::setEnabled
        //);
        //if (!isConnected) {
        //    qDebug() << "连接 qg_JogButton_ED 失败!";
        //}
        //connect(motionObject, &Thread::setControlEnable_signal, newJogWidget, &qg_JogButton_ED::setEnabled);
    }
    else if (LSM->m_version == ENUM_VERSION::EdPin)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //创建易鼎丰对应的新的控件
        qg_JogButton_ED_New* newJogWidget = new qg_JogButton_ED_New(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::BtPin)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //创建易鼎丰对应的新的控件
        qg_JogButton_BT* newJogWidget = new qg_JogButton_BT(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmPin)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //创建金脉Pin对应的新的控件
        qg_JogButton_JMPin* newJogWidget = new qg_JogButton_JMPin(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmHan)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //创建金脉焊缝对应的新的控件
        qg_JogButton_JMHan* newJogWidget = new qg_JogButton_JMHan(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else
    {
        connect(motionObject, &Thread::setControlEnable_signal, ui.widget_FormulaJog, &qg_JogButton::setEnabled);
    }
}

//版本写入
void Pin2::saveVersionToFile_Pin(int version)
{
    // 获取文件路径
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    // 创建 JSON 对象并设置 version
    QJsonObject jsonObj;
    jsonObj["version"] = version;

    // 将 QJsonObject 转换为 QJsonDocument
    QJsonDocument doc(jsonObj);

    // 保存 JSON 数据到文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "成功保存版本号: " << version;
    }
    else {
        qDebug() << "无法保存文件: " << filePath;
    }
}

//版本切换
void Pin2::on_pushButton_ChangeVersion_clicked()
{
#ifndef DEBUG_LHC
    //密码验证逻辑
    bool isPasswordOK = false;
    while (!isPasswordOK) { // 循环直到密码正确
        // 创建密码输入对话框
        QInputDialog pwdDialog;
        pwdDialog.setWindowTitle("密码验证");
        pwdDialog.setLabelText("请输入访问密码：");
        pwdDialog.setTextEchoMode(QLineEdit::Password); // 设置密码掩码

        pwdDialog.setOkButtonText("确定");
        pwdDialog.setCancelButtonText("取消");

        // 执行对话框
        if (pwdDialog.exec() == QDialog::Accepted) {
            // 验证密码（暂用密码"888888"）
            if (pwdDialog.textValue() == "888888") {
                isPasswordOK = true;
            }
            else {
                // 密码错误提示
                QMessageBox::critical(nullptr, "错误", "密码错误，请重试！");
            }
        }
        else {
            // 用户取消输入直接退出
            return;
        }
    }
#endif // !DEBUG_LHC
    // 创建版本对话框
    qg_VersionDialog verDialog(LSM->m_version);
    int result = verDialog.exec();

    if (result == QDialog::Accepted) {
        int tp = verDialog.getVersion();
        if (LSM->m_version != tp)
        {
            LSM->m_version = verDialog.getVersion();
            //写入当前版本
            saveVersionToFile_Pin(LSM->m_version);
        }
        //确定板卡版本
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
        case ENUM_VERSION::EdPin:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_Pulse;
            break;
        case ENUM_VERSION::LsPin:
        case ENUM_VERSION::JmPin:
        case ENUM_VERSION::JmHan:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_EtherCat;
            break;
        case ENUM_VERSION::BtPin:
            LSM->m_motionVersion = ENUM_MOTIONVERSION::LS_Pulse_1000;
            break;
        default:
            break;
        }
        QMessageBox::information(this, tr("提示"), tr("软件将关闭，请重新打开软件"),
            QMessageBox::Yes);
        qApp->exit();
    }
    else {
        return;
    }
}

//清空当前产品生产数据
void Pin2::clearProductMess()
{
    LSM->m_productPara.FirstPassRate = 100;
    LSM->m_productPara.NGCount = 0;
    LSM->m_productPara.OKCount = 0;
    LSM->m_productPara.ProductCount = 0;
    ui.label_FirstPassRate->setText("直通率：100%");
    ui.label_NGCount->setText("NG数量：0");
    ui.label_ProductCount->setText("已生产数量：0");
    ui.label_OKCount->setText("OK数量：0");
    //NG率
    ui.label_6->setText("NG率：" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
}

//保存生产数据
void Pin2::saveProductMess()
{
    JsonConfigManager config("product_data.json");
    // 写入参数到JSON
    config.writeParam("FirstPassRate", LSM->m_productPara.FirstPassRate);
    config.writeParam("NGCount", LSM->m_productPara.NGCount);
    config.writeParam("OKCount", LSM->m_productPara.OKCount);
    config.writeParam("ProductCount", LSM->m_productPara.ProductCount);
    config.writeParam("UserAccout", LSM->m_productPara.UserAccout);
    config.writeParam("WorkOrderCode", LSM->m_productPara.WorkOrderCode);
    
}

//读取生产数据
void Pin2::readProductMess()
{
    JsonConfigManager config("product_data.json");

    // 从JSON读取并赋值（带默认值）
    LSM->m_productPara.FirstPassRate = config.readDouble("FirstPassRate", 100.0);
    LSM->m_productPara.NGCount = config.readInt("NGCount", 0);
    LSM->m_productPara.OKCount = config.readInt("OKCount", 0);
    LSM->m_productPara.ProductCount = config.readInt("ProductCount", 0);
    LSM->m_productPara.UserAccout = config.readString("UserAccout", "");
    LSM->m_productPara.WorkOrderCode = config.readString("WorkOrderCode", "");
    
    ui.lineEdit_UserAccount->setText(LSM->m_productPara.UserAccout);
    ui.lineEdit_WorkOrderCode->setText(LSM->m_productPara.WorkOrderCode);
    //首次读到页面上
    ui.label_FirstPassRate->setText("直通率：" + QString::number(LSM->m_productPara.FirstPassRate) + "%");
    ui.label_NGCount->setText("NG数量：" + QString::number(LSM->m_productPara.NGCount));
    ui.label_ProductCount->setText("已生产数量：" + QString::number(LSM->m_productPara.ProductCount));
    ui.label_OKCount->setText("OK数量：" + QString::number(LSM->m_productPara.OKCount));
    //NG率
    ui.label_6->setText("NG率：" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
    showNG_Slot(100 - LSM->m_productPara.FirstPassRate);
}

//刷新显示结果数据
void Pin2::upDataProductPara_slot(Run_Result runResult)
{
    //QElapsedTimer materialTimer;
    //materialTimer.start();
    //while (m_resultImgPath.isEmpty())
    //{
    //    if (materialTimer.elapsed() > ERROR_TIME)
    //    {
    //        break;
    //    }
    //}
    //输出结果到数据集
    auto sql = qg_QSLiteWidget::getSqlWindow();
    sql->addProductResult(runResult);
    sql->addOneProductResult(runResult, ui.label_ProjectName->text(), LSM->m_resultImgPath);
    LSM->m_productPara.ProductCount += 1;
    if (runResult.isNG)
    {
        LSM->m_productPara.NGCount += 1;
        ui.label_ResultPic->setPixmap(QPixmap(":/icons/ng.png"));
    }
    else
    {
        LSM->m_productPara.OKCount += 1;
        ui.label_ResultPic->setPixmap(QPixmap(":/icons/ok.png"));
    }

    if (LSM->m_productPara.ProductCount > 0) {
        // 四舍五入到两位小数
        double rate = static_cast<double>(LSM->m_productPara.OKCount) / LSM->m_productPara.ProductCount * 100.0;
        LSM->m_productPara.FirstPassRate = std::round(rate * 100.0) / 100.0;
    }
    else {
        // 处理除零情况
        LSM->m_productPara.FirstPassRate = 0.0;
    }
    ui.label_FirstPassRate->setText("直通率：" + QString::number(LSM->m_productPara.FirstPassRate) + "%");
    ui.label_NGCount->setText("NG数量：" + QString::number(LSM->m_productPara.NGCount));
    ui.label_ProductCount->setText("已生产数量：" + QString::number(LSM->m_productPara.ProductCount));
    ui.label_OKCount->setText("OK数量：" + QString::number(LSM->m_productPara.OKCount));
    //NG率
    ui.label_6->setText("NG率：" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
    showNG_Slot(100 - LSM->m_productPara.FirstPassRate);
}

//接受用户信息
void Pin2::setUser_slot(int mes, int time)
{
    setUser(mes);
    initUser();
    if (mes == Admin_User)
    {
        //管理员，开启30分钟倒计时，到时间自动切换回普通用户
        adminTimer->start(time * 1000);
        //adminTimer->start(20000);
    }
    else
    {
        adminTimer->stop();
    }
}

//管理员倒计时切换
void Pin2::switchToNormalUser() {
    setUser(Normal_User);
    initUser();
}

//用户切换
void Pin2::on_pushButton_ChangeUser_clicked()
{
    qg_userChange dlg;
    connect(&dlg, &qg_userChange::setUser_signal, this, &Pin2::setUser_slot);
    dlg.exec();
}

//保存halcon图片存到文件夹
int Pin2::saveHalconJPGToFile(HObject img, QString pathName, QString fileName, bool isResultPic, QString& picPath)
{
    if (img.Key() == nullptr || pathName.isEmpty() || fileName.isEmpty())
    {
        return -1;
    }
    //QString time = QTime::currentTime().toString("hh-mm-ss");
    QString time = QDate::currentDate().toString("yyyyMMdd-") + QTime::currentTime().toString("hh-mm");
    time = LSM->m_runStartTime;
    QString exePath = QCoreApplication::applicationDirPath();
    QString filePath = "/images/Original/";

    pathName.replace('*', '@');         // 替换所有*为@
    if (isResultPic)
    {
        //存结果图
        filePath = "/images/Result/";
    }
    QString absPath = exePath + filePath + pathName + "/"+ time;
    QDir photoDir(absPath);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + absPath, Normal);
            return -1;
        }
    }

    QString name;
    //// 从 1.jpg 开始查找
    //int fileIndex = 1;
    //do {
    //    name = photoDir.filePath(QString::number(fileIndex) + ".jpg");
    //    fileIndex++;
    //} while (QFile::exists(name));  // 如果文件已存在，则继续增加索引

    //name = exePath + filePath + pathName + fileName+ "_" + time;
    name = fileName;
    absPath = LSM->m_resultImgPath;
    QString finalPath = "";
    //picPath = absPath;
    if (isResultPic)
    {
        absPath = LSM->m_resultImgPath;
        finalPath = absPath + "/" + name;
        //存结果图
        WriteImage(img, "jpg 80", 0, finalPath.toUtf8().data());
    }
    else
    {
        //absPath = exePath + filePath + pathName + "/" + time;
        //HObject tmp = img.Clone();
        ////存原图
        //WriteImage(tmp, "bmp", 0, finalPath.toUtf8().data());
        //tmp.Clear();
        absPath = exePath + filePath + pathName + "/" + time;
        finalPath = absPath + "/" + name;
        try
        {
            Mat showMatImg = ImageTypeConvert::HObject2Mat(img);
            QString tpPath = finalPath + ".bmp";
            cv::imwrite(tpPath.toStdString(), showMatImg);
        }
        catch (const std::exception&)
        {
            return -1;
        }
    }
    return 0;
}

//循环运行
void Pin2::on_checkBox_Loop_clicked(bool checked)
{
    if (checked)
    {
        LSM->m_isLoop.store(true);
        //禁止单步运行
        ui.pushButton_OneStep->setEnabled(false);
    }
    else
    {
        LSM->m_isLoop.store(false);
        if (!LSM->m_isStart.load())
        {
            ui.pushButton_OneStep->setEnabled(true);
        }
    }
}

//关闭软件
void Pin2::on_pushButton_Close_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认退出", "确定要关闭程序吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        this->close();
    }
}

//最小化
void Pin2::on_pushButton_Min_clicked()
{
    this->showMinimized();
}

//最大化
void Pin2::on_pushButton_Max_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();  // 如果当前是最大化状态，则还原窗口
    }
    else {
        this->showMaximized(); // 最大化窗口
    }
}

//void Pin2::mouseMoveEvent(QMouseEvent* event)
//{
//    if (event->buttons() & Qt::LeftButton) {
//        const QPoint delta = event->globalPos() - m_dragPosition;
//        const int moveThreshold = 200; // 拖动阈值(像素)
//        if (delta.manhattanLength() < moveThreshold)
//        {
//            return;
//        }
//
//        //m_isMaximizedBeforeDrag = isMaximized();
//        bool m_isMaximizedBeforeDrag = isMaximized();    // 拖动前是否最大化
//        QRect m_restoreGeometry = geometry();
//
//        // 如果是最大化状态，则还原窗口并调整大小
//        if (m_isMaximizedBeforeDrag) {
//            showNormal(); // 先还原窗口
//
//            // 计算新窗口位置：鼠标当前位置为中心
//            QSize normalSize = m_restoreGeometry.size();
//            QPoint mouseGlobalPos = event->globalPos();
//            QRect newGeometry(
//                mouseGlobalPos.x() - normalSize.width() / 2,
//                mouseGlobalPos.y() - 20,  // 保留顶部边距
//                normalSize.width(),
//                normalSize.height()
//            );
//
//            // 确保窗口不超出屏幕
//            QScreen* screen = QApplication::screenAt(mouseGlobalPos);
//            if (screen) {
//                QRect screenGeometry = screen->availableGeometry();
//                newGeometry = newGeometry.intersected(screenGeometry);
//            }
//
//            setGeometry(newGeometry);
//        }
//
//        move(event->globalPos() - m_dragPosition);
//        event->accept();
//    }
//}


//图片显示到视图
void Pin2::showInGraphView(const QImage& image)
{
    QGraphicsViews* graphicsView = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPin);
    graphicsView->DispImage(image);

    // 初始化布局
    if (!gridLayout) {
        // 删除旧布局（如果有）
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPin->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // 创建新网格布局
        gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPin);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // 设置列宽均匀分配
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        ui.scrollAreaWidgetContents_ShowPin->setLayout(gridLayout);
    }

    // 添加视图到当前行列位置
    gridLayout->addWidget(graphicsView, currentRow, currentCol);

    ui.scrollAreaWidgetContents_ShowPin->updateGeometry();
    ui.scrollAreaWidgetContents_ShowPin->update();
    Sleep(10);
    //刷新一下自适应
    for (int i = 0; i < gridLayout->count(); ++i) {
        QLayoutItem* item = gridLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        // 强制视图重新计算尺寸
        item->widget()->updateGeometry();
        item->widget()->adjustSize();

        // 直接调用GetFit确保立即刷新
        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
            view->GetFit();
        }
    }

    // 立即刷新当前视图
    graphicsView->updateGeometry();
    graphicsView->adjustSize();
    graphicsView->GetFit();
    // =============================

    // 刷新布局和父控件
    ui.scrollAreaWidgetContents_ShowPin->adjustSize();
    ui.scrollArea_ShowPin->viewport()->update();

    //if (oldCurrentRow != currentRow)
    //{
    //    //行号更新了，刷新一下自适应
    //    for (int i = 0; i < gridLayout->count(); ++i) {
    //        QLayoutItem* item = gridLayout->itemAt(i);
    //        if (!item || !item->widget()) continue;

    //        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
    //            //其他视图自适应刷新一下
    //            view->GetFit();
    //        }
    //    }
    //    oldCurrentRow = currentRow;
    //}

    // 更新行列索引
    currentCol++;
    if (currentCol >= maxColumns) {
        currentCol = 0;
        currentRow++;
    }

    // 确保滚动区域自动调整
    ui.scrollArea_ShowPin->setWidgetResizable(true);
}

//所有视图自适应
void Pin2::upDateQgraphicsView_slot()
{
    ui.scrollAreaWidgetContents_ShowPin->updateGeometry();
    ui.scrollAreaWidgetContents_ShowPin->update();
    Sleep(10);
    //刷新一下自适应
    for (int i = 0; i < gridLayout->count(); ++i) {
        QLayoutItem* item = gridLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        // 强制视图重新计算尺寸
        item->widget()->updateGeometry();
        item->widget()->adjustSize();

        // 直接调用GetFit确保立即刷新
        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
            view->GetFit();
        }
    }

    // 刷新布局和父控件
    ui.scrollAreaWidgetContents_ShowPin->adjustSize();
    ui.scrollArea_ShowPin->viewport()->update();
}

//清除视图
void Pin2::clearAllViews()
{
    if (gridLayout == nullptr)
        return;
    // 重置行列索引
    currentRow = 0;
    currentCol = 0;
    oldCurrentRow = 0;
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

//清除视图
void Pin2::clearAllViews_slot()
{
    clearAllViews();
}

//扫码枪信息读取
void Pin2::slot_RevMsg(QString name, QString mess) {
    //LSM->ls_tcpMess = mess;
    LSM->ls_tcpMess = mess.trimmed(); // 去除所有首尾空白（包括空格、\t、\r、\n）
    machineLog->write("扫码枪信息：" + mess, Normal);
    return;
}

//用户初始化
void Pin2::initUser()
{
    auto user = getUser();
    switch (user)
    {
    case ENUM_USER::Normal_User:
        ui.label_User->setText("普通用户");
        ui.pushButton_AddProject->setEnabled(false);
        ui.pushButton_Save->setEnabled(false);
        ui.pushButton_Cancel->setEnabled(false);
        ui.lineEdit_RunSteps->setEnabled(false);
        ui.radioButton_BothRun->setEnabled(false);
        ui.pushButton_OneStep->setEnabled(false);
        ui.pushButton_AddFormula->setEnabled(false);
        ui.pushButton_InsertFormula->setEnabled(false);
        ui.pushButton_UpMove->setEnabled(false);
        ui.pushButton_DownMove->setEnabled(false);
        ui.pushButton_DeleteRow->setEnabled(false);
        ui.tabWidget_Formula->setEnabled(false);
        ui.pushButton_init->setEnabled(false);
        ui.pushButton_CopyNew->setEnabled(false);
        //轴控制栏
        ui.pushButton_AxisOn->setEnabled(false);
        ui.comboBox_AxisNum->setEnabled(false);
        ui.pushButton_Homing->setEnabled(false);
        //版本切换
        ui.pushButton_ChangeVersion->setEnabled(false);
        ui.checkBox_Loop->setEnabled(false);
        //用于调试的权限
        ui.tab_AxisState->setUiState(false);
        ui.pushButton_RobotProject->setEnabled(false);
        break;
    case  ENUM_USER::Admin_User:
        ui.label_User->setText("管理员");
        ui.pushButton_AddProject->setEnabled(true);
        ui.pushButton_Save->setEnabled(true);
        ui.pushButton_Cancel->setEnabled(true);
        ui.lineEdit_RunSteps->setEnabled(true);
        ui.radioButton_BothRun->setEnabled(true);
        ui.pushButton_OneStep->setEnabled(true);
        ui.pushButton_AddFormula->setEnabled(true);
        ui.pushButton_InsertFormula->setEnabled(true);
        ui.pushButton_UpMove->setEnabled(true);
        ui.pushButton_DownMove->setEnabled(true);
        ui.pushButton_DeleteRow->setEnabled(true);
        ui.tabWidget_Formula->setEnabled(true);
        ui.pushButton_init->setEnabled(true);
        ui.pushButton_CopyNew->setEnabled(true);
        //轴控制栏
        ui.pushButton_AxisOn->setEnabled(true);
        ui.comboBox_AxisNum->setEnabled(true);
        ui.pushButton_Homing->setEnabled(true);
        //版本切换
        ui.pushButton_ChangeVersion->setEnabled(true);
        ui.checkBox_Loop->setEnabled(true);
        //用于调试的权限
        ui.tab_AxisState->setUiState(true);
        ui.pushButton_RobotProject->setEnabled(true);
        break;
    default:
        break;
    }
}

//用户账号
void Pin2::onUserAccountChanged()
{
    LSM->m_productPara.UserAccout = ui.lineEdit_UserAccount->text();
}

//保存halcon图片存到文件夹（用于立昇MES）
int Pin2::saveHalconJPGToFile_LS(HObject img, QString machineName, PointItemConfig item, bool isResultPic, QString& picPath)
{
    //主路径：
    QString exePath = QCoreApplication::applicationDirPath();
    QString mainPath = exePath + "/LS_Mes_Images/";
    // 获取当前日期（年-月-日）
    QString dateStr = QDate::currentDate().toString("yyyyMMdd");
    QString fileFolder = "";
    //文件夹格式：机种名-样品名-日期
    if (machineName == "A66")
    {
        fileFolder = mainPath + machineName + "-" + "LS202004AH11-26" + "-" + dateStr;
    }
    else if (machineName == "A66-T")
    {
        fileFolder = mainPath + machineName + "-" + "LS202004AJ11-03" + "-" + dateStr;
    }
    else {
        return 0;
    }

    //时间（小时-分钟）
    QString time = QTime::currentTime().toString("hh-mm");
    //QString filePath = "/images/Original/";
    //if (isResultPic)
    //{
    //    //存结果图
    //    filePath = "/images/Result/";
    //}
    QString absPath = fileFolder + "/" + time;
    QDir photoDir(absPath);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + absPath, Normal);
        }
    }

    QString modifiedModelName = item.ModelName;  // 复制原始字符串
    modifiedModelName.replace('*', '@');         // 替换所有*为@

    QString name = modifiedModelName + "-" + time + "-" + machineName + "-" + item.ProductName;
    QString finalPath = absPath + "/" + name;
    WriteImage(img, "jpg 80", 0, finalPath.toUtf8().data());
    return 0;
}

//视觉功能的纯结果显示视图
void Pin2::showVision_slot(Vision_Message message)
{
    HObject originaImg;
    HObject resultImg;
    //QImage resultImg;
    PointItemConfig config;
    QString filePath = "";
    try {
        //if (!result.ho_Image.IsInitialized())
        //    return;
        if (message.img.Key() == nullptr)
        {
            return;
        }
        originaImg = message.img;
        QString photoPath;
        //保存图片
        //saveHalconJPG(img, photoPath);
        //保存图片到型号文件夹
        QString picPath = "";

        //HTuple hv_Width, hv_Height;
        //GetImageSize(img, &hv_Width, &hv_Height);

        // 新 创建 pinLabel 对象
        pinLabel label;

        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        // 显示图像
        label.setImage(message.img);

        //------------
        label.showVisionImage(message);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        ////------------
        //新显示
        showInGraphView(showQImg);
        if (!message.BarMes.empty())
        {
            config = message.BarMes[0].first;
        }
        else if (!message.TempMes.empty())
        {
            config = message.TempMes[0].first;
        }
        else if (!message.QRMes.empty())
        {
            config = message.QRMes[0].first;
        }
        else if (!message.BlobMes.empty())
        {
            config = message.BlobMes[0].first;
        }
        else if (!message.CircleMes.empty())
        {
            config = message.CircleMes[0].first;
        }

        ////------------
        resultImg = showHjImg;

        ////数据库
        //sql->addNew2DResult(result, config.Vision);

    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showVision_slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
        message.clear();
    }


    try
    {
        if (config.ModelName.isEmpty())
        {
            //如果没有型号，写配方方案名
            config.ModelName = ui.label_ProjectName->text();
        }
        //保存原图直接移到拍照模块
       /* if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.Vision);
        }*/
        //保存结果
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        //m_resultImgPath = filePath;

        //保存NG图片
        if (config.isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.Vision);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("图片保存失败", Normal);
    }
    message.clear();
}

//数据库窗口弹出
void Pin2::on_pushButton_sql_clicked()
{
    // 获取QSQLiteWidget实例，假设objectName为"sqlWidget"
    qg_QSLiteWidget* sqlWidget = findChild<qg_QSLiteWidget*>("tab_Sql");
    if (!sqlWidget) return;

    // 保存原父部件和布局信息
    QWidget* originalParent = sqlWidget->parentWidget();
    QLayout* originalLayout = originalParent->layout();
    int originalFontSize = sqlWidget->font().pointSize();

    // 从原布局中移除部件
    if (originalLayout) {
        originalLayout->removeWidget(sqlWidget);
    }

    // 创建对话框并设置布局
    QDialog* dialog = new QDialog(this);
    //添加最大化
    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowMaximizeButtonHint);
    //设置窗体可拖动大小
    dialog->setSizeGripEnabled(true);
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(sqlWidget);
    dialog->setLayout(dialogLayout);
    QList<QWidget*> widgets = dialog->findChildren<QWidget*>();
    for (QWidget* w : widgets) {
        QFont font = w->font();
        font.setPointSize(originalFontSize);
        w->setFont(font);
    }

    // 对话框关闭时恢复部件
    connect(dialog, &QDialog::finished, this, [=](int result) {
        Q_UNUSED(result);
        // 将部件重新设置回原父部件
        sqlWidget->setParent(originalParent);
        // 恢复字体
        QList<QWidget*> originalWidgets = originalParent->findChildren<QWidget*>();
        for (QWidget* w : originalWidgets) {
            QFont font = w->font();
            font.setPointSize(originalFontSize);
            w->setFont(font);
        }
        // 重新添加到原布局
        if (originalLayout) {
            originalLayout->addWidget(sqlWidget);
        }
        dialog->deleteLater(); // 清理对话框
        });

    dialog->show(); // 模态显示对话框
}

//清除产品数据统计
void Pin2::on_pushButton_clearData_clicked()
{
    clearProductMess();
}

//单纯显示图片
void Pin2::showImage_slot(QImage img)
{
    showInGraphView(img);
}

//写modbus值 type = 0 : 位；= 1 ：整值；= 2：浮点数
void Pin2::writeModbus_slot(int regAddr, int bitPos, float value, int type)
{
    bool res = 0;
    if (type == 0)
    {
        //位写入
        bool tp = value == 0 ? false : true;
        res = modbusRobot.writeBit(regAddr, bitPos, tp);
    }
    else if (type == 1)
    {
        //整值写入
        res = modbusRobot.writeSingleRegister(regAddr, quint16(value));
    }
    else if (type == 2)
    {
        //浮点数写入
        res = modbusRobot.writeFloatRegister(regAddr, value);
    }
    if (!res)
    {
        LSM->m_isEmergency.store(true);
        machineLog->write("modbus 写入地址: "+QString::number(regAddr)+ "错误，请检查通信", Normal);
        machineLog->write("modbus 写入地址: " + QString::number(regAddr) + "错误，请检查通信", Err);
        return;
    }
}

//读modbus值 type = 0 : 位；= 1 ：整值；= 2：浮点数
void Pin2::readModbus_slot(int regAddr, int bitPos, int type)
{
    if (type == 0)
    {
        //位读取
        bool bit = false;
        bool rtn = modbusRobot.readBit(regAddr, bitPos, bit);
        if (!rtn)
        {
            machineLog->write("modbus 读取地址: " + QString::number(regAddr) + "错误，请检查通信", Normal);
            machineLog->write("modbus 读取地址: " + QString::number(regAddr) + "错误，请检查通信", Err);
            //LSM->m_isEmergency.store(true);
            return;
        }
        if (regAddr == 34)
        {
            //读运动时各信号状态
            switch (bitPos)
            {
            case 0:
                LSM->m_RobotModbus.feedInBeltPickDone = bit;
                break;
            case 1:
                LSM->m_RobotModbus.fixturePlaceDone = bit;
                break;
            case 2:
                LSM->m_RobotModbus.fixturePickDone = bit;
                break;
            case 3:
                LSM->m_RobotModbus.dischargeBeltPlaceDone = bit;
                break;
            case 4:
                LSM->m_RobotModbus.ngBeltLeftPlaceDone = bit;
                break;
            case 5:
                LSM->m_RobotModbus.ngBeltRightPlaceDone = bit;
                break;
            case 6:
                LSM->m_RobotModbus.haveProduct = bit;
                break;
            case 7:
                LSM->m_RobotModbus.haveProductB = bit;
                break;
            default:
                break;
            }
        }
        if (regAddr == 0)
        {
            //读运动时各信号状态
            switch (bitPos)
            {
            case 0:
                LSM->m_RobotModbus.manualStatus = bit;
                break;
            case 1:
                LSM->m_RobotModbus.autoStatus = bit;
                break;
            case 2:
                LSM->m_RobotModbus.remoteStatus = bit;
                break;
            case 3:
                LSM->m_RobotModbus.servoStatus = bit;
                break;
            case 4:
                LSM->m_RobotModbus.alarmStatus = bit;
                break;
            case 5:
                LSM->m_RobotModbus.emergencyStopStatus = bit;
                break;
            case 6:
                LSM->m_RobotModbus.programRunningStatus = bit;
                break;
            case 7:
                LSM->m_RobotModbus.safePosition1Status = bit;
                break;
            case 8:
                LSM->m_RobotModbus.safePosition2Status = bit;
                break;
            case 9:
                LSM->m_RobotModbus.safePosition3Status = bit;
                break;
            case 10:
                LSM->m_RobotModbus.safePosition4Status = bit;
                break;
            case 11:
                LSM->m_RobotModbus.programLoadStatus = bit;
                break;
            case 12:
                LSM->m_RobotModbus.servoReadyStatus = bit;
                break;
            case 13:
                LSM->m_RobotModbus.programReservation = bit;
                break;
            case 14:
                LSM->m_RobotModbus.programResetStatus = bit;
                break;
            default:
                break;
            }
        }
    }
    else if (type == 1)
    {
        //整值写入
        quint16 value = 0;
        modbusRobot.readHoldingRegister(regAddr, value);
        if (regAddr == 38)
        {
            //读运行信号报警码
            LSM->m_RobotModbus.errorMessage = value;
        }
    }
    else if (type == 2)
    {
        //浮点数读取
        float value = 0;
        modbusRobot.readFloatRegister(regAddr, value);
    }
}

// 显示圆测距的图片
void Pin2::showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    HObject originaImg;
    HObject resultImg;
    QString filePath = "";
    try {
        QString picPath;
#if LOCAL_TEST == 1
        HObject ho_img;
        ReadImage(&ho_img, "C:\\Users\\erik.lin\\Desktop\\test\\20250808_10-33-18\\up1.bmp");
#else
        auto ho_img = LSM->m_photo_map[config.PhotoName];
#endif

        if (ho_img.Key() == nullptr)
        {
            return;
        }
        originaImg = ho_img;

        // 新 创建 pinLabel 对象
        pinLabel label;

        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        // 显示图像
        label.setImage(ho_img);
        label.showCircleMeasure(cir1, cir2,config,dis,isNG);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);
        resultImg = showHjImg;
        //cv::namedWindow("image", cv::WINDOW_NORMAL);
        //cv::resizeWindow("image", 800, 800);
        //cv::imshow("image", showMatImg);
        //新显示
        showInGraphView(showQImg);
        ////保存原图
        //if (ui.checkBox_SaveImg->isChecked())
        //{
        //    saveHalconJPGToFile(ho_img, config.ModelName, config.ProductName, false, picPath);
        //}
        ////保存结果
        //saveHalconJPGToFile(showHjImg, config.ModelName, config.ProductName, true, picPath);
        //m_resultImgPath = picPath;
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showVision_slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }

    try
    {
        if (config.ModelName.isEmpty())
        {
            //如果没有型号，写配方方案名
            config.ModelName = ui.label_ProjectName->text();
        }

        if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.Vision);
        }
        //保存结果
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);

        if (!config.Tip.isEmpty())
        {
            //优先备注名
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Tip);
        }
        else
        {
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        }
        //m_resultImgPath = filePath;

        //保存NG图片
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("图片保存失败", Normal);
    }
}

//触发机器人下料
void Pin2::robotUnload_slot(bool isNG)
{
    if (!robotUnLoadEDThread->isRunning())
    {
        robotUnLoadEDThread->start();
        QMetaObject::invokeMethod(robotUnLoadEDObject, "robotUnLoadingAction",
            Qt::QueuedConnection,
            Q_ARG(bool, isNG));
    }
}

//动作线程报警弹窗
void Pin2::showConfirmationDialog(const QString& message)
{
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this,
        "警告",
        message,
        QMessageBox::Yes
    );

   emit userDecision(true); // 发送用户选择
}

//清除机器人指令
void Pin2::clearRobotAuto()
{
    MODBUS_TCP.writeIntCoil(100, 1, 0);
    MODBUS_TCP.writeIntCoil(100, 4, 0);
    MODBUS_TCP.writeIntCoil(100, 5, 0);
    MODBUS_TCP.writeIntCoil(100, 2, 0);
}

//和图片显示线程的交互
void Pin2::showthread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config)    //单独2Dpin针
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showPin_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin, result),
        Q_ARG(PointItemConfig, config)
    );
}
void Pin2::showthread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)    //2D和3Dpin针
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showPin2Dand3D_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin, result2D),
        Q_ARG(PointItemConfig, config2D),
        Q_ARG(ResultParamsStruct_Pin3DMeasure, result3D),
        Q_ARG(PointItemConfig, config3D)
    );
}

//单独3Dpin针
void Pin2::showthread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)    
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showPin3D_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin3DMeasure, result),
        Q_ARG(PointItemConfig, config)
    );
}

//视觉功能的纯结果显示视图
void Pin2::showthread_showVision_slot(Vision_Message message)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showVision_slot",
        Qt::QueuedConnection,
        Q_ARG(Vision_Message, message)
    );
    message.clear();
}
// 显示圆测距的图片
void Pin2::showthread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showCircleMeasure_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Circle, cir1),
        Q_ARG(ResultParamsStruct_Circle, cir2),
        Q_ARG(PointItemConfig, config),
        Q_ARG(double, dis),
        Q_ARG(bool, isNG)
    );
}

//显示所有视觉模块图片
void Pin2::showthread_showAllVision_slot(std::unordered_map<QString, Vision_Message> message)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    for (auto messageTemp : message)
    {
        // 使用 Q_ARG 传递参数
        QMetaObject::invokeMethod(showResultObject, "thread_showVision_slot",
            Qt::QueuedConnection,
            Q_ARG(Vision_Message, messageTemp.second)
        );
    }
}

void Pin2::errUpdate_slot(QString mes)
{
    ui.label_11->setStyleSheet("color: red;");
    ui.label_11->setText(mes);
    if (!ui.label_11->isScrollingActive())
    {
        ui.label_11->startScrolling(100);
    }
    LSM->m_errCode = mes;
}

// 配方运行完成CT时间
void Pin2::showCTtime_slot(QString time)
{
    //毫秒
    //ui.label_12->setText("配方流程CT时间：" + time + " ms");
    //ui.label_13->setText("配方流程CT时间：" + time + " ms");
    //秒
    time = QString::number(time.toDouble() / 1000.0);
    ui.label_12->setText("配方流程CT时间：" + time + " s");
    ui.label_13->setText("配方流程CT时间：" + time + " s");
}

//保存halcon原始图片存到文件夹
int Pin2::saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName)
{
    if (img.Key() == nullptr)
    {
        return -1;
    }
    if (pathName.isEmpty())
    {
        QString time = QDate::currentDate().toString("yyyyMMdd-") + QTime::currentTime().toString("hh-mm");
        QString exePath = QCoreApplication::applicationDirPath();
        QString filePath = "/images/Original/";
        //没有路径的话，用方案名加时间
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // 替换所有*为@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName;

    try
    {
        Mat showMatImg = ImageTypeConvert::HObject2Mat(img);
        QString tpPath = finalPath + ".bmp";
        cv::imwrite(tpPath.toStdString(), showMatImg);
        machineLog->write("原图保存路径：" + finalPath, Normal);
    }
    catch (const std::exception&)
    {
        machineLog->write("原图保存失败：" + pathName, Normal);
        return -1;
    }

    return 0;
}


//保存halcon结果图片存到文件夹
int Pin2::saveResultPhotoToFile(HObject img, QString pathName, QString fileName)
{
    if (img.Key() == nullptr)
    {
        return -1;
    }
    if (pathName.isEmpty())
    {
        QString time = QDate::currentDate().toString("yyyyMMdd-") + QTime::currentTime().toString("hh-mm");
        QString exePath = QCoreApplication::applicationDirPath();
        QString filePath = "/images/Result/";
        //没有路径的话，用方案名加时间
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // 替换所有*为@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName;

    try
    {
        //存结果图
        QByteArray utf8Path = finalPath.toUtf8();
        const char* filePath = utf8Path.constData();
        WriteImage(img, "jpg 80", 0, filePath);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}

//保存QImage结果图片存到文件夹
int Pin2::saveResultPhotoToFile(QImage img, QString pathName, QString fileName)
{
    if (img.isNull())
    {
        return -1;
    }
    if (pathName.isEmpty())
    {
        QString time = QDate::currentDate().toString("yyyyMMdd-") + QTime::currentTime().toString("hh-mm");
        QString exePath = QCoreApplication::applicationDirPath();
        QString filePath = "/images/Result/";
        //没有路径的话，用方案名加时间
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // 替换所有*为@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName + ".jpg";

    try
    {
        //存结果图
        rs_FunctionTool tp;
        tp.saveQImage(img, finalPath, "JPG", 80);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}

// 工作状态 state 0:待机；1:运行；2:停止
void Pin2::showWorkState_slot(int state)
{
    switch (state)
    {
    case 0:
        ui.label_WorkState->setStyleSheet("color: yellow; font-weight: bold;");
        ui.label_WorkState->setText("待机中");
        break;
    case 1:
        ui.label_WorkState->setStyleSheet(
            "color: #00FF00;"             // 霓虹绿（更亮）
            "font-weight: bold;"
        );
        ui.label_WorkState->setText("运行中");
        break;
    case 2:
        ui.label_WorkState->setStyleSheet("color: red; font-weight: bold;");
        ui.label_WorkState->setText("停止中");
        break;
    default:
        break;
    }
}

// 清除重大报警
void Pin2::on_pushButton_ErrorClear_clicked()
{
    ui.label_11->clearAll();
    LSM->m_errCode.clear();
}

// 易鼎丰机器人和配方的对应关系页面
void Pin2::on_pushButton_RobotProject_clicked()
{
    qg_RobotProject_ED dlg;
    dlg.exec();
}

void Pin2::on_checkBox_SaveImg_toggled(bool checked)
{
    if (checked) {
        LSM->m_saveImageEnabled = true;
    }
    else {
        LSM->m_saveImageEnabled = false;
    }
}

//显示所有视觉模块图片
void Pin2::showthread_showNG_slot(HObject photo, PointItemConfig config, bool isNG)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showNG_slot",
        Qt::QueuedConnection,
        Q_ARG(HObject, photo),
        Q_ARG(PointItemConfig, config),
        Q_ARG(bool, isNG)
    );
}

//直接显示报错NG的图片
void Pin2::showNG_slot(HObject photo, PointItemConfig config, bool isNG)
{
    HObject originaImg;
    HObject resultImg;
    QString filePath = "";
    try {
        QString picPath;
        auto ho_img = photo;
        if (ho_img.Key() == nullptr)
        {
            return;
        }
        originaImg = ho_img;

        // 新 创建 pinLabel 对象
        pinLabel label;
        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        // 显示图像
        label.setImage(ho_img);
        label.showNGimage(photo, config, isNG);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);
        resultImg = showHjImg;

        //新显示
        showInGraphView(showQImg);

    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showNG_slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }

    try
    {
        if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.Vision);
        }
        //保存结果
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);

        if (!config.Tip.isEmpty())
        {
            //优先备注名
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Tip);
        }
        else
        {
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        }
        //m_resultImgPath = filePath;

        //保存NG图片
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("图片保存失败", Normal);
    }
}

//显示所有视觉模块图片
void Pin2::showthread_showImage_slot(QImage img)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // 使用 Q_ARG 传递参数
    QMetaObject::invokeMethod(showResultObject, "thread_showImage_slot",
        Qt::QueuedConnection,
        Q_ARG(QImage, img)
    );
}

bool Pin2::eventFilter(QObject* obj, QEvent* evt)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent* event = static_cast<QMouseEvent*>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - this->pos();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    }
    else if (event->type() == QEvent::MouseMove) {
        if (!this->isMaximized() && mousePressed && (event->buttons() && Qt::LeftButton)) {
            this->move(event->globalPos() - mousePoint);
            return true;
        }
        else if (this->isMaximized() && mousePressed && (event->buttons() && Qt::LeftButton))
        {
            this->showNormal();
            return true;
        }
    }

    return QWidget::eventFilter(obj, evt);
}

//工单变化
void Pin2::onWorkOrderCodeChanged()
{
    LSM->m_productPara.WorkOrderCode = ui.lineEdit_WorkOrderCode->text();
}