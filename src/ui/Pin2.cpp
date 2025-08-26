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
//���д1�����þɵ�ui��ʽ
#define OLD_UI 0

Pin2::Pin2(QWidget *parent)
    : QMainWindow(parent)
{
    appWindow = this;
    ui.setupUi(this);

    ui.scrollArea_ShowPin->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.scrollArea_ShowPin->setWidgetResizable(false);

    // �����ޱ߿����
    setWindowFlags(Qt::FramelessWindowHint);
    showMaximized();

    //״̬��
    if (!statusBar()) {
        setStatusBar(new QStatusBar(this));
    }
    this->installEventFilter(this);
    //cpu״̬����ʾ
    monitor = new ShowCpuMemory(this);
    // ����״̬����ǩ
    QLabel* statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);
    QFont font("Microsoft YaHei", 10, QFont::Normal);
    statusLabel->setFont(font);
    // ���ñ�ǩ���������
    monitor->SetLab(statusLabel);
    monitor->startMonitoring(1000); // 1��ˢ��һ��

    //���ݰ汾����UI
    updataUiFromVersion();

#ifndef DEBUG_LHC
    //������ʽ��
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


    //���ƶ���ѭ������
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

    //ȫ��IO�źţ�λ�õ�ѭ������
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
        //���N�������߳�
        doubleSpeedObject = new Thread();
        doubleSpeedThread = new QThread();
        doubleSpeedObject->moveToThread(doubleSpeedThread);
    }
    else if (LSM->m_version == EdPin)
    {
        //�׶���Ƥ��ȡ���߳�
        robotLoadEDObject = new Thread();
        robotLoadEDThread = new QThread();
        robotLoadEDObject->moveToThread(robotLoadEDThread);

        //�׶���Ƥ�������߳�
        robotUnLoadEDObject = new Thread();
        robotUnLoadEDThread = new QThread();
        robotUnLoadEDObject->moveToThread(robotUnLoadEDThread);
    }
    //���������ʾ�߳�
    showResultObject = new Thread();
    showResultThread = new QThread();
    showResultObject->moveToThread(showResultThread);
    if (!showResultThread->isRunning())
    {
        showResultThread->start();
        connect(showResultObject, &Thread::thread_showPin_signal, this, &Pin2::showPin_Slot, Qt::BlockingQueuedConnection); //ֻ�����߳���ʹ�ã���Ȼ��������
        connect(showResultObject, &Thread::thread_showPin2Dand3D_signal, this, &Pin2::showPin2Dand3D_Slot, Qt::BlockingQueuedConnection);//ֻ�����߳���ʹ�ã���Ȼ��������
        connect(showResultObject, &Thread::thread_showPin3D_signal, this, &Pin2::showPin3D_Slot, Qt::BlockingQueuedConnection);//ֻ�����߳���ʹ�ã���Ȼ��������
        //connect(showResultObject, &Thread::thread_showVision_signal, this, &Pin2::showVision_slot, Qt::BlockingQueuedConnection); //ֻ�����߳���ʹ�ã���Ȼ��������
        connect(showResultObject, &Thread::thread_showCircleMeasure_signal, this, &Pin2::showCircleMeasure_slot, Qt::BlockingQueuedConnection);
        connect(showResultObject, &Thread::thread_showNG_signal, this, &Pin2::showNG_slot, Qt::BlockingQueuedConnection);
        //connect(showResultObject, &Thread::thread_showImage_signal, this, &Pin2::showImage_slot, Qt::BlockingQueuedConnection); 
    }

#if OLD_UI
    ui.pushButton_UpMove->hide();
    ui.pushButton_DownMove->hide();
    ui.pushButton_DeleteRow->hide();
#endif
    //��־��
    connect(machineLog, &QC_Log::logUpdate_signal, this, &Pin2::logUpdate_slot);


    //setupJogAxisLayout();
    // 250321 ��ʹ�ö�̬��ӵİ취�ˣ�ҳ��о��е�����
    //2Dʶ��
    //createPinWindow();
    //3Dʶ��
    //createPin3DMeasureWindow();
    //��ʽ��ʼ��
    initStyle();
    //�䷽������ʼ��
    initFormula();
    //��ȡ��������
    readProductMess();
    //�û���ʼ��
    initUser();
    //�����û��л���ʱ��
    adminTimer = new QTimer(this);
    adminTimer->setSingleShot(true); // ���δ���
    connect(adminTimer, &QTimer::timeout, this, &Pin2::switchToNormalUser);

    // ������ʱ��
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Pin2::updateLabelValue);
    timer->start(300);


    if (LSM->m_version == LsPin)
    {
        //������ɨ��ǹ��tcpͨѶ
        if (m_clientQRCode.connectToServer("DemoClient", "192.168.3.100", 2001)) {
            qDebug() << "�����ӵ�������";
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
    connect(ui.tab_Sql, &qg_QSLiteWidget::showNG_Signal, this, &Pin2::showNG_Slot);//ˢ��NG��
    qRegisterMetaType<Run_Result>("Run_Result");
    connect(LSM, &rs_motion::upDataProductPara_signal, this, &Pin2::upDataProductPara_slot);//ˢ�½����ʾ
    connect(ui.lineEdit_UserAccount, &QLineEdit::textChanged, this, &Pin2::onUserAccountChanged);//�û��˺�
    connect(LSM, &rs_motion::clearWindow_signal, this, &Pin2::clearAllViews_slot);
    connect(LSM, &rs_motion::writeModbus_signal, this, &Pin2::writeModbus_slot, Qt::BlockingQueuedConnection);
    connect(LSM, &rs_motion::readModbus_signal, this, &Pin2::readModbus_slot, Qt::BlockingQueuedConnection);
    connect(machineLog, &QC_Log::errUpdate_signal, this, &Pin2::errUpdate_slot);
    connect(LSM, &rs_motion::upDateQgraphicsView_signal, this, &Pin2::upDateQgraphicsView_slot);
    connect(ui.lineEdit_WorkOrderCode, &QLineEdit::textChanged, this, &Pin2::onWorkOrderCodeChanged);//����
    

    //connect(LSM, &rs_motion::showPin_signal, this, &Pin2::showPin_Slot, Qt::BlockingQueuedConnection); //ֻ�����߳���ʹ�ã���Ȼ��������
    //connect(LSM, &rs_motion::showPin2Dand3D_signal, this, &Pin2::showPin2Dand3D_Slot, Qt::BlockingQueuedConnection);//ֻ�����߳���ʹ�ã���Ȼ��������
    //connect(LSM, &rs_motion::showPin3D_signal, this, &Pin2::showPin3D_Slot, Qt::BlockingQueuedConnection);//ֻ�����߳���ʹ�ã���Ȼ��������
    connect(LSM, &rs_motion::showVision_signal, this, &Pin2::showVision_slot, Qt::BlockingQueuedConnection); //ֻ�����߳���ʹ�ã���Ȼ��������
    //connect(LSM, &rs_motion::showCircleMeasure_signal, this, &Pin2::showCircleMeasure_slot, Qt::BlockingQueuedConnection);
    connect(LSM, &rs_motion::showImage_signal, this, &Pin2::showImage_slot, Qt::BlockingQueuedConnection); //ֻ�����߳���ʹ�ã���Ȼ��������

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
    //�ŷ���ʼ��
    LSM->m_triggerMotionTpyes = RS2::InitMotion;
    //�׶����ʼ�����ӻ�����ͨѶ
    if (LSM->m_version == EdPin)
    {
        //modbusRobot.connectToDevice("192.168.1.12");
        auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
        if(!rtn)
            machineLog->write("ModbusTcp����ʧ��...", Normal);
    }
   
    if (LSM->m_motionVersion == LS_Pulse_1000)
    {
        //1000�İ忨�ӿ�û�зų������ŷ��Ľӿڣ����Բ���ʾ
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

    //���N�������߳�
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

//����Pin��2Dʶ��ҳ��
void Pin2::createPinWindow() {
    HObject image;
    int processID = 1;
    int modubleID = 2;
    std::string modubleName = "Module1";
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        // ����ˢ������
    };
    std::string getConfigPath = "";

    // ���� PinWindow ʵ����ʹ�õڶ������캯��
    pinWindow = new PinWindow(image, processID, modubleID, modubleName, refreshConfig, getConfigPath);

    // ���� QScrollArea������Ϊ ui.tab_2DVisionWidget ���ӿؼ�
    QScrollArea* scrollArea = new QScrollArea(ui.tab_2DVisionWidget);

    // ���� scrollArea ������
    scrollArea->setWidgetResizable(true);  // ���ݴ�С�ı�ʱ��scrollArea ���Զ�������С
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // ���ô�ֱ������
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // ����ˮƽ������

    // ����һ�� QWidget ���ڳ��� pinWindow
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    // �� pinWindow ��ӵ� contentWidget �Ĳ�����
    contentLayout->addWidget(pinWindow);

    // �� contentWidget ����Ϊ QScrollArea ������
    scrollArea->setWidget(contentWidget);

    // ��ȡ tab_2DVisionWidget �Ĳ��ֲ��� scrollArea ��ӽ�ȥ
    QVBoxLayout* tabLayout = new QVBoxLayout(ui.tab_2DVisionWidget);
    tabLayout->addWidget(scrollArea);

    // ���� tab_2DVisionWidget �Ĳ���
    ui.tab_2DVisionWidget->setLayout(tabLayout);


}


//����Pin��3Dʶ��ҳ��
void Pin2::createPin3DMeasureWindow()
{
    // ����������Щ����
    pcl::PointCloud<pcl::PointXYZ>::Ptr image3D; // 3D��������
    int processID = 1;
    int modubleID = 2;
    std::string modubleName = "Module1";
    std::function<void(int, int, std::string)> refreshConfig = [](int processID, int modubleID, std::string modubleName) {
        // ����ˢ������
    };
    std::string getConfigPath = "";

    // ���� Pin3DMeasureWindow ʵ����ʹ�ù��캯��
    pin3DMeasureWindow = new Pin3DMeasureWindow(image3D, processID, modubleID, modubleName, refreshConfig, getConfigPath);

    // ���� QScrollArea������Ϊ ui.tab_3DVisionWidget ���ӿؼ�
    QScrollArea* scrollArea = new QScrollArea(ui.tab_3DVisionWidget);

    // ���� scrollArea ������
    scrollArea->setWidgetResizable(true);  // ���ݴ�С�ı�ʱ��scrollArea ���Զ�������С
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // ���ô�ֱ������
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // ����ˮƽ������

    // ����һ�� QWidget ���ڳ��� pin3DMeasureWindow
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);

    // �� pin3DMeasureWindow ��ӵ� contentWidget �Ĳ�����
    contentLayout->addWidget(pin3DMeasureWindow);

    // �� contentWidget ����Ϊ QScrollArea ������
    scrollArea->setWidget(contentWidget);

    // ��ȡ tab_3DVisionWidget �Ĳ��ֲ��� scrollArea ��ӽ�ȥ
    QVBoxLayout* tabLayout = new QVBoxLayout(ui.tab_3DVisionWidget);
    tabLayout->addWidget(scrollArea);

    // ���� tab_3DVisionWidget �Ĳ���
    ui.tab_3DVisionWidget->setLayout(tabLayout);

}

//��ʽ��ʼ��
void Pin2::initStyle()
{
    ui.label_WorkState->setStyleSheet("color: yellow; font-weight: bold;");
    ui.label_WorkState->setText("������");
    if (ui.checkBox_Loop->isChecked())
    {
        LSM->m_isLoop.store(true);
        //��ֹ��������
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

    // ���� QFont ����
    // QFont font("Arial", 12);
    // Ӧ��������ʽ�� QTabWidget �ı�ǩ
    //ui.tabWidget->setFont(font);
    
    // ���������� QStandardItemModel
    //QStandardItemModel* model = new QStandardItemModel(this);
    //model->setColumnCount(9);  // ��������

#if OLD_UI
    ui.tableWidget_Formula_Left->setColumnCount(9);  // ��������
#else

#endif // OLD_UI


    // ���ñ�ͷ��ǩ
    QStringList headers;
    switch (LSM->m_version)
    {
    case ENUM_VERSION::TwoPin:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // ��������
        //˫��pin��
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Z (mm)")
            << tr("RZ (��)")
            << tr("U1 (mm)")
            << tr("Remarks")
            << tr("Edit")
#if OLD_UI
            << tr("Actions")
#endif // OLD_UI
            ;
        break;
    case ENUM_VERSION::EdPin:
        ui.tableWidget_Formula_Left->setColumnCount(10);  // ��������
        //�׶���ר��
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y (mm)")
            << tr("U (��)")
            << tr("U1(�ξ�) (��)")
            << tr("Z (mm)")
            << tr("Z1(����) (mm)")
            << tr("Z2(����) (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::LsPin:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // ��������
        //���Nר��
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Y2 (mm)")
            << tr("U (��)")
            << tr("Z (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::BtPin:
        ui.tableWidget_Formula_Left->setColumnCount(6);  // ��������
        //���Nר��
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y1 (mm)")
            << tr("Y2 (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::JmPin:
        ui.tableWidget_Formula_Left->setColumnCount(10);  // ��������
        //����Pin��
        headers << tr("Action")
            << tr("X (mm)")
            << tr("X1(�����) (mm)")
            << tr("Y (mm)")
            << tr("U (��)")
            << tr("U1(�ξ�) (��)")
            << tr("Z (mm)")
            << tr("Z1(�����) (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    case ENUM_VERSION::JmHan:
        ui.tableWidget_Formula_Left->setColumnCount(8);  // ��������
        //���������
        headers << tr("Action")
            << tr("X (mm)")
            << tr("Y (mm)")
            << tr("Y1 (mm)")
            << tr("U (��)")
            << tr("Z (mm)")
            << tr("Remarks")
            << tr("Edit")
            ;
        break;
    default:
        break;
    }
  

    // ���ñ�ͷ
    ui.tableWidget_Formula_Left->setHorizontalHeaderLabels(headers);
    // ���ñ�ͷ����Ӧ��С
    ui.tableWidget_Formula_Left->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidget_Formula_Left->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
    ui.tableWidget_Formula_Left->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������
    //// ���ñ༭
    //for (int col = 0; col <= 6; ++col) {
    //    for (int row = 0; row < ui.tableWidget_Formula_Left->rowCount(); ++row) {
    //        ui.tableWidget_Formula_Left->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    //    }
    //}
    // ���õ�һ�б༭
    for (int row = 0; row < ui.tableWidget_Formula_Left->rowCount(); ++row) {
        ui.tableWidget_Formula_Left->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    // ����˫�����Ĳۺ���
    connect(ui.tableWidget_Formula_Left, &QTableWidget::cellChanged, this, &Pin2::onLeftCellDoubleClicked);

    if (LSM->m_version == ENUM_VERSION::TwoPin)
    {
        //˫��pin��
        //�ҹ�λ���
#if OLD_UI
        ui.tableWidget_Formula_Right->setColumnCount(9);  // ��������
#else
        ui.tableWidget_Formula_Right->setColumnCount(8);  // ��������
#endif // OLD_UI
    // ���ñ�ͷ��ǩ
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

        // ���ñ�ͷ
        ui.tableWidget_Formula_Right->setHorizontalHeaderLabels(headersRight);
        //// ��ģ�����ø� QTableView
        //ui.tableView_Formula_Right->setModel(modelRight);
        // ���ñ�ͷ����Ӧ��С
        ui.tableWidget_Formula_Right->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui.tableWidget_Formula_Right->setSelectionMode(QAbstractItemView::SingleSelection);  // ����ѡ��
        ui.tableWidget_Formula_Right->setSelectionBehavior(QAbstractItemView::SelectRows);  // ѡ������
        //// ���ñ༭
        //for (int col = 0; col <= 6; ++col) {
        //    for (int row = 0; row < ui.tableWidget_Formula_Right->rowCount(); ++row) {
        //        ui.tableWidget_Formula_Right->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //    }
        //}
        // ���õ�һ�б༭
        for (int row = 0; row < ui.tableWidget_Formula_Right->rowCount(); ++row) {
            ui.tableWidget_Formula_Right->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        // ����˫�����Ĳۺ���
        connect(ui.tableWidget_Formula_Right, &QTableWidget::cellChanged, this, &Pin2::onRightCellDoubleClicked);
    }


    //����ͼ��ʼ��
    ui.widget_showNG->addCurve("NG", Qt::red);
    ui.widget_showNG->addCurve("OK", Qt::green);
    // ����ʱ�䴰��Ϊ600��
    ui.widget_showNG->setTimeWindow(600);
    // ����Y�᷶Χ
    ui.widget_showNG->setYRange(0, 120);

    // ��ʼ�Զ�����
    ui.widget_showNG->startAutoUpdate();
}

//�½�����
void Pin2::on_pushButton_AddFormula_clicked()
{
    // ��ȡ��ǰ��λ����/�ң�
    ENUMSTATION station = ENUMSTATION::L;
    if(ui.tabWidget_Formula->currentIndex() == 1)
        station = ENUMSTATION::R;

    //����ǰ����
    auto name = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
    {
        if (station == ENUMSTATION::L)
        {
            //�ĳɻ�ȡ�䷽�Ĳ���
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_L,station);
            connect(&dlg, &qg_Formula::signalFormula, this, &Pin2::slotFormula);
            dlg.exec();
        }
        else if (station == ENUMSTATION::R)
        {
            //�ĳɻ�ȡ�䷽�Ĳ���
            qg_Formula dlg(LSM->m_Formula[name].ListProcess_R, station);
            connect(&dlg, &qg_Formula::signalFormula, this, &Pin2::slotFormula);
            dlg.exec();
        }
    }
}

//��ʼ��
void Pin2::on_pushButton_init_clicked()
{
    // ��ʼ���������
    LSM->m_triggerMotionTpyes = RS2::InitMotion;
}


//�������
void Pin2::on_pushButton_Homing_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���������У���ȴ����н�����ֹͣ���ٻ��㣡"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���ڸ�λ�У���ȴ����н�����ֹͣ���ٻ��㣡"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("����"), tr("�忨δ��ʼ���ɹ���������ɳ�ʼ�����ٻ��㣡"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("����"), tr("δ����������������������"));
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

//�㶯ֹͣ
void Pin2::on_pushButton_JogStop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        //����״̬��ֹͣ������
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
            //ûͣ������һֱ����ֹͣ
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("��������ͣ��ʱ��" + QString::number(ERROR_TIME) + " �����δ��ȡ����ͣ�ɹ�", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "����", "��������ͣ��ʱ,��ֱ���ļ�ͣ����", QMessageBox::Yes, this);

                // ������ʽ������Ŵ�3������ť�ߴ�����
                QString styleSheet =
                    "QMessageBox { font-size: 24px; } "  // Ĭ������Ŵ�3��������ԭΪ8px��
                    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // ��������ߴ�
                    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // ��ť�Ŵ�

                msgBox.setStyleSheet(styleSheet);

                // ��ʾ����
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 1, 0);
            //Sleep(5);
        }
        //ͣ�����ˣ����к�ֹͣ״̬��0
        clearRobotAuto();
    }
}

//����ֹͣ
void Pin2::on_pushButton_RunStop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        LSM->closeDO();
        //����״̬��ֹͣ������
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
            //ûͣ������һֱ����ֹͣ
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("��������ͣ��ʱ��" + QString::number(ERROR_TIME) + " �����δ��ȡ����ͣ�ɹ�", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "����", "��������ͣ��ʱ,��ֱ���ļ�ͣ����" ,QMessageBox::Yes, this);

                //// ������ʽ������Ŵ�3������ť�ߴ�����
                //QString styleSheet =
                //    "QMessageBox { font-size: 24px; } "  // Ĭ������Ŵ�3��������ԭΪ8px��
                //    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // ��������ߴ�
                //    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // ��ť�Ŵ�

                //msgBox.setStyleSheet(styleSheet);

                // ��ʾ����
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            //Sleep(5);
        }
        //ͣ�����ˣ����к�ֹͣ״̬��0
        clearRobotAuto();
    }

}

void Pin2::setupJogAxisLayout()
{
    // ��ȡ tab_JogAxis �Ĳ��֣����û�в����򴴽�һ���µ�
    QGridLayout* layout = qobject_cast<QGridLayout*>(ui.tab_JogAxis->layout());
    if (!layout) {
        layout = new QGridLayout(ui.tab_JogAxis);
        ui.tab_JogAxis->setLayout(layout);
    }

    // ���ò��ֵ���/�м��
    layout->setHorizontalSpacing(6);
    layout->setVerticalSpacing(6);

    // ���ò��ֵ����ݱ߾�
    layout->setContentsMargins(10, 10, 10, 10);

    // ���ò��ֵ����б�����Stretch��
    layout->setRowStretch(0, 1); // ��һ�б���
    layout->setRowStretch(1, 1); // �ڶ��б���
    layout->setRowStretch(2, 4); // �����б���

    //layout->setColumnStretch(0, 1); // ��һ�б���
    //layout->setColumnStretch(1, 1); // �ڶ��б���
    //layout->setColumnStretch(2, 1); // �����б���

    // ȷ�����ָ���
    layout->update();
}


// �ۺ��������� label_7 ��ֵ
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
        // ƴ�Ӹ������λ���ַ���
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1��Y1:%2��Y2:%3��Z:%4��RZ:%5��U1:%6��U2:%7��")
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
        axesPositions = QString("X:%1��Y:%2��U:%3��U1(�ξ�):%4��Z:%5��Z1(����):%6��Z2(����):%7��")
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
        axesPositions = QString("X:%1��Y1:%2��Y2:%3��U:%4��Z:%5��")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_X ].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y2].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_U ].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Z ].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::BtPin:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1��Y1:%2��Y2:%3��")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::LS_Y2].position * 100) / 100, 'f', 2));
        break;
    case ENUM_VERSION::JmPin:
        axisName = LS_AxisName::X;
        axesPositions = QString("X:%1��X1(�����):%2��Y:%3��U:%4��<br>U1(�ξ�):%5��Z:%6��Z1(�����):%7��")
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
        axesPositions = QString("X:%1��Y:%2��Y1:%3��U:%4��Z:%5��")
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::X].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Y1].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::U].position * 100) / 100, 'f', 2))
            .arg(QString::number(std::round(LSM->m_Axis[LS_AxisName::Z].position * 100) / 100, 'f', 2));
        break;
    default:
        break;
    }

    // ���� label_7 ���ı�
    ui.label_7->setText(axesPositions);

    // ˳�����һ���ŷ�ʹ��״̬
    if (LSM->ReadServOn(axisName))
    {
        LSM->isServeOn = true;
        ui.pushButton_AxisOn->setText("ʹ�ܹ�");
    }
    else
    {
        LSM->isServeOn = false;
        ui.pushButton_AxisOn->setText("ʹ�ܿ�");
    }
}

//������λ
void Pin2::on_pushButton_Reset_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���������У���ȴ����н�����ֹͣ���ٸ�λ��"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���ڸ�λ�У���ȴ����н�����ֹͣ���ٸ�λ��"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("����"), tr("�忨δ��ʼ���ɹ���������ɳ�ʼ�����ٸ�λ��"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("����"), tr("δ����������������������"));
        return;
    }
    ui.lineEdit_RunSteps->setText(QString::number(0));
    LSM->m_triggerMotionTpyes = RS2::ActionFlag::ResetAll;
}

//ֹͣ
void Pin2::on_pushButton_Stop_clicked()
{
    LSM->AxisStopAll();
    LSM->m_isEmergency.store(true);
    LSM->setButtonLight(2, 1);
    if (LSM->m_version == EdPin)
    {
        //����״̬��ֹͣ������
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
            //ûͣ������һֱ����ֹͣ
            if (materialTimer.elapsed() > ERROR_TIME) {
                machineLog->write("��������ͣ��ʱ��" + QString::number(ERROR_TIME) + " �����δ��ȡ����ͣ�ɹ�", Normal);
                QMessageBox msgBox(QMessageBox::Warning, "����", "��������ͣ��ʱ,��ֱ���ļ�ͣ����", QMessageBox::Yes, this);

                // ������ʽ������Ŵ�3������ť�ߴ�����
                QString styleSheet =
                    "QMessageBox { font-size: 24px; } "  // Ĭ������Ŵ�3��������ԭΪ8px��
                    "QMessageBox QLabel { min-width: 600px; min-height: 200px; } " // ��������ߴ�
                    "QMessageBox QPushButton { width: 200px; height: 60px; font-size: 24px; }"; // ��ť�Ŵ�

                msgBox.setStyleSheet(styleSheet);

                // ��ʾ����
                msgBox.exec();
                break;
            }
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            //Sleep(5);
        }
        //ͣ�����ˣ����к�ֹͣ״̬��0
        clearRobotAuto();
    }
}

//�Ḵλ
void Pin2::on_pushButton_AxisReset_clicked()
{
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���������У���ȴ����н�����ֹͣ���ٸ�λ��"));
        return;
    }
    if (LSM->m_isHomming.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���ڸ�λ�У���ȴ����н�����ֹͣ���ٸ�λ��"));
        return;
    }
    if (!LSM->m_cardInitStatus)
    {
        QMessageBox::warning(this, tr("����"), tr("�忨δ��ʼ���ɹ���������ɳ�ʼ�����ٸ�λ��"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("����"), tr("δ����������������������"));
        return;
    }
    ui.lineEdit_RunSteps->setText(QString::number(0));
    LSM->m_triggerMotionTpyes = RS2::ActionFlag::ResetAll;
}

//��ʹ��
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

//�ᱨ�����
void Pin2::on_pushButton_Clear_clicked()
{
    if (LSM->m_version == LsPin)
    {
        //ȷ�ϵ���
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "��λȷ��",
            "�������п��ƿ��ȸ�λ���Ƿ������",
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        //�������ȵ���
        QProgressDialog progressDialog("���߿���λ������...", "ȡ��", 0, 15, this);
        progressDialog.setWindowTitle("��λ����");
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setCancelButton(nullptr); // ����ȡ����ť
        progressDialog.setMinimumDuration(0);     // ������ʾ
        progressDialog.setValue(0);

        machineLog->write("������������߿������λ�����У�Լ15S������", Normal);

        // ִ�и�λ����
        QCoreApplication::processEvents();

        // ��λ��һ�׶β���
        dmc_soft_reset(LSM->m_Axis[LS_AxisName::LS_X].card);
        dmc_board_close();
        progressDialog.setValue(1);

        // �ȴ�ѭ�������ȸ���
        QElapsedTimer timer;
        timer.start();
        for (int i = 1; i <= 15; ++i) {
            // �������ȴ�������UI��Ӧ��
            while (timer.elapsed() < i * 1000) {
                QCoreApplication::processEvents();
                QThread::msleep(50);
            }

            // ���½��ȣ�����1������ɣ�
            progressDialog.setValue(i + 1);

            // ÿ5�������־
            if (i % 5 == 0) {
                machineLog->write(QString("��λ��...�ѵȴ�%1��").arg(i), Normal);
            }
        }

        // ��ɺ�������
        LSM->motionInit();
        progressDialog.setValue(16);

        //// ״ָ̬ʾ�Ʋ���
        //if (LSM->m_DO[LS_DO::LS_OUT_YellowLight].state == 0) {
        //    LSM->setDO(LS_DO::LS_OUT_RedLight, 0);
        //    LSM->setDO(LS_DO::LS_OUT_YellowLight, 1);
        //    LSM->setDO(LS_DO::LS_OUT_GreenLight, 0);
        //}

        // �ᱨ�����
        QFutureSynchronizer<void> sync;
        for (auto axis : LS_AxisName::allAxis) {
            sync.addFuture(QtConcurrent::run([=]() {
                LSM->AxisClear(axis);
                }));
        }
        sync.waitForFinished();

        LSM->m_isEmergency.store(false);

        machineLog->write("���߿������λ���,��ȷ������״̬", Normal);
        QMessageBox::information(this, "���", "��λ������ȫ�����");
    }
    else if (LSM->m_version == EdPin)
    {
        //��������˱���
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

//�½��䷽����
void Pin2::on_pushButton_AddProject_clicked()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("�����ļ���"), tr("�������ļ�����"), QLineEdit::Normal, "", &ok);

    if (!ok || fileName.isEmpty()) {
        return;
    }

    // ��ȡexeĿ¼�µ�Formula_Config�ļ���·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // ���Formula_Config�ļ��в����ڣ��򴴽�
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::critical(this, tr("����"), tr("�޷������ļ��У�"));
            return;
        }
    }

    // �����ļ�������·��
    QString filePath = dir.filePath(fileName + ".json");

    // �ж��ļ��Ƿ��Ѵ���
    if (QFile::exists(filePath)) {
        // �ļ��Ѵ��ڣ�����ȷ�Ͽ�
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("�ļ��Ѵ���"), tr("�ļ��Ѵ��ڣ��Ƿ񸲸ǣ�"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    // ������д�� JSON �ļ�
    QJsonObject jsonObject;

    QJsonDocument jsonDoc(jsonObject);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "����", "�޷����ļ�����д�룡");
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();

    //�浽�䷽������
    LSM->m_Formula[fileName];
    ui.comboBox_Project_2->addItem(fileName);
    // ���õ�ǰѡ��Ϊ�½����ļ���
    ui.comboBox_Project_2->setCurrentText(fileName);
    ui.label_ProjectName->clear();
    ui.label_ProjectName->setText(fileName);
    // ����䷽��
    ui.tableWidget_Formula_Left->clearContents();
    ui.tableWidget_Formula_Right->clearContents();
    // ��¼��ǰ�䷽��
    LSM->m_forMulaName = fileName;
}

//�䷽�����л�
void Pin2::on_comboBox_Project_2_currentIndexChanged(int index)
{
    if (LSM->m_version == EdPin)
    {
        if (m_comboBox_Project_2_lastIndex != index)
        {
            QString text = ui.comboBox_Project_2->itemText(index); // ��ȡ��ѡ����ı�
            QString proName = "��ȷ���Ƿ��л�Ϊ�䷽��[ " + text + " ] ������ξ��Ƿ�ƥ��";
            QMessageBox::StandardButton res = QMessageBox::question(nullptr, "��ʾ", proName,
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes);
            if (res == QMessageBox::No)
            {
                // �����źŷ�ֹ�ݹ鴥��
                ui.comboBox_Project_2->blockSignals(true);
                // �ָ�֮ǰ��ѡ��
                ui.comboBox_Project_2->setCurrentIndex(m_comboBox_Project_2_lastIndex);
                ui.comboBox_Project_2->blockSignals(false);
                return;
            }
            // �û�ѡ��Yes������lastIndex
            m_comboBox_Project_2_lastIndex = index;
        }
    }

    // ��ȡ��ǰѡ�����Ŀ
    QString selectedItem = ui.comboBox_Project_2->currentText();
    ui.label_ProjectName->clear();
    ui.label_ProjectName->setText(selectedItem);
    LSM->m_forMulaName = selectedItem;
    clearProductMess();

    // ����䷽���ڣ����±��
    auto it = LSM->m_Formula.find(selectedItem);
    if (it != LSM->m_Formula.end()) {
        // ��������Ҳ�ı��
        ui.tableWidget_Formula_Left->clearContents();
        ui.tableWidget_Formula_Right->clearContents();

        // ���������
        ui.tableWidget_Formula_Left->setRowCount(0);  // ���������
        rs_motion::Formula_Config selectedFormula = it->second;
        for (auto config : selectedFormula.ListProcess_L)
        {
            //��λ
            addNewRow(config);
        }

        // �����Ҳ���
        ui.tableWidget_Formula_Right->setRowCount(0);  // ���������
        for (auto config : selectedFormula.ListProcess_R)
        {
            //�ҹ�λ
            addNewRow(config);
        }
    }

   
}

//��Ӳ���Ĳۺ���
void Pin2::slotFormula(const PointItemConfig& config)
{
    addNewRow(config);

    //����ǰ����
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

//�䷽��ʼ��
void Pin2::initFormula()
{
    // ��ȡexeĿ¼�µ�Formula_Config�ļ���·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    if (!dir.exists()) {
        return;
    }

    ui.comboBox_Project_2->clear();
    // ��ȡ�ļ����������ļ������������ļ��У�
    dir.setFilter(QDir::Files);  // ֻ��ȡ�ļ������������ļ���
    //dir.setSorting(QDir::Time | QDir::Reversed);  // ������ʱ�������µ��ļ�����ǰ��
    dir.setSorting(QDir::Time);  // ������ʱ�������µ��ļ�����ǰ��

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        QString name = fileInfo.baseName();
        ui.comboBox_Project_2->addItem(name);
    }


    if (LSM->m_version == EdPin)
    {
        JsonConfigManager config("RobotToFormula.json");

        // �������ӳ��
        LSM->m_formulaToRobot.clear();

        // ��ȡ����JSON����
        QJsonObject jsonObj = config.readAllJsonObject();

        // ������ʱӳ�䣺�����˷��� -> �䷽�б�
        std::unordered_map<int, QStringList> robotToFormulas;

        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            QString robotStr = it.key();

            bool ok;
            int robotNumber = robotStr.toInt(&ok);
            if (!ok || robotNumber < 1 || robotNumber > 9) continue;

            // ���ֵ�Ƿ�Ϊ�ַ���
            if (!it.value().isString()) continue;

            QString formulaStr = it.value().toString();

            // �ָ��䷽�ַ���
            QStringList formulas = formulaStr.split(QRegularExpression("[;��]"), Qt::SkipEmptyParts);

            // ������ʱӳ��
            for (const QString& formula : formulas) {
                QString trimmedFormula = formula.trimmed();
                if (!trimmedFormula.isEmpty()) {
                    robotToFormulas[robotNumber].append(trimmedFormula);
                }
            }
        }

        // ����ȫ��ӳ��
        for (const auto& pair : robotToFormulas) {
            int robotNumber = pair.first;
            const QStringList& formulas = pair.second;

            for (const QString& formula : formulas) {
                LSM->m_formulaToRobot[formula] = robotNumber;
            }
        }

        // ����Ƿ�ɹ���ȡ
        if (LSM->m_formulaToRobot.empty()) {
            machineLog->write("��ȡ�����˷������䷽��Ӧ��ϵʧ�ܣ������¼�鲢���ã���", Normal);
            machineLog->write("��ȡ�����˷������䷽��Ӧ��ϵʧ�ܣ������¼�鲢���ã���", Err);
            return;
        }
    }
}

// �������
void Pin2::addNewRow(const PointItemConfig config) {

    if (config.StationL == ENUMSTATION::L)
    {
        // �����ź�
        ui.tableWidget_Formula_Left->blockSignals(true);
        //��λ
        // ��ȡ��ǰ������
        int row = ui.tableWidget_Formula_Left->rowCount();
        ui.tableWidget_Formula_Left->insertRow(row);

        // ���� RowData ���󣬷�װ���ݺͰ�ť
        //RowData rowData(config, row);

        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("���ɵ�");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin��ʶ���");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3Dɨ���");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3Dʶ��");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("ȡ����");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("����");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("�Ӿ�ģ��");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("Բ���");
            break;
        default:
            break;
        }

        // ֱ���ڵ�Ԫ���д�����ť�������ⲿ����
        QPushButton* editButton = new QPushButton("�༭", ui.tableWidget_Formula_Left);
        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {  // ��ʽ���� editButton
            // ��̬��ȡʵ���к�
            QPoint buttonPos = editButton->mapToParent(QPoint(0, 0));  // ת�����굽���ؼ�
            int actualRow = ui.tableWidget_Formula_Left->indexAt(buttonPos).row();
            onEditButtonClicked(actualRow);
            });
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // �������ݵ�������Ӧ��Ԫ��
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            ui.tableWidget_Formula_Left->setItem(row, 6, new QTableWidgetItem(config.Tip));
            // ����ťǶ�뵽���ĵ�7��
            ui.tableWidget_Formula_Left->setCellWidget(row, 7, editButton);
            //// ���༭��ť��ӵ�����"�༭"��
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
            // ���༭��ť��ӵ�����"�༭"��
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
            // ���༭��ť��ӵ�����"�༭"��
            ui.tableWidget_Formula_Left->setCellWidget(row, 7, editButton);
            break;
        case ENUM_VERSION::BtPin:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(config.Tip));
            // ���༭��ť��ӵ�����"�༭"��
            ui.tableWidget_Formula_Left->setCellWidget(row, 5, editButton);
            break;
        default:
            break;
        }


        //// ���ñ༭
        //for (int col = 0; col < 6; ++col) {
        //    ui.tableWidget_Formula_Left->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //}
        // ���õ�һ�б༭
        ui.tableWidget_Formula_Left->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);

#if OLD_UI
        // ����һ�� QWidget ������������ť
        QWidget* operationWidget = new QWidget();
        QHBoxLayout* operationLayout = new QHBoxLayout();
        operationLayout->setContentsMargins(0, 0, 0, 0); 

        operationLayout->addWidget(rowData.btn_upMove);
        operationLayout->addWidget(rowData.btn_downMove);
        operationLayout->addWidget(rowData.btnRemove);

        operationWidget->setLayout(operationLayout);
        // ��������ť������ӵ�"����"��
        ui.tableWidget_Formula_Left->setCellWidget(row, 8, operationWidget); 

        connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
        connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
        connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif
        // �ָ��ź�
        ui.tableWidget_Formula_Left->blockSignals(false);
    }
    else if (config.StationL == ENUMSTATION::R && LSM->m_version == ENUM_VERSION::TwoPin)
    {
        // �����ź�
        ui.tableWidget_Formula_Right->blockSignals(true);
        //�ҹ�λ
        // ��ȡ��ǰ������
        int row = ui.tableWidget_Formula_Right->rowCount();
        ui.tableWidget_Formula_Right->insertRow(row);  // ��������

        // ���� RowData ���󣬷�װ���ݺͰ�ť
        //RowData rowData(config, row);

        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("���ɵ�");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin��ʶ���");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3Dɨ���");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3Dʶ��");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("ȡ����");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("����");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("�Ӿ�ģ��");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("Բ���");
            break;
        default:
            break;
        }
        // ֱ���ڵ�Ԫ���д�����ť�������ⲿ����
        QPushButton* editButton = new QPushButton("�༭", ui.tableWidget_Formula_Right);
        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {  // ��ʽ���� editButton
            // ��̬��ȡʵ���к�
            QPoint buttonPos = editButton->mapToParent(QPoint(0, 0));  // ת�����굽���ؼ�
            int actualRow = ui.tableWidget_Formula_Right->indexAt(buttonPos).row();
            onEditButtonClicked(actualRow);
            });
        // �������ݵ�������Ӧ��Ԫ��
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // �������ݵ�������Ӧ��Ԫ��
            ui.tableWidget_Formula_Right->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Right->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPX)));
            ui.tableWidget_Formula_Right->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPY)));
            ui.tableWidget_Formula_Right->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPZ)));
            ui.tableWidget_Formula_Right->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPRZ)));
            ui.tableWidget_Formula_Right->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPU)));
            ui.tableWidget_Formula_Right->setItem(row, 6, new QTableWidgetItem(config.Tip));
            // ���༭��ť��ӵ�����"�༭"��
            ui.tableWidget_Formula_Right->setCellWidget(row, 7, editButton);
            //ui.tableWidget_Formula_Right->setCellWidget(row, 7, rowData.editButton);
            break;
        default:
            break;
        }

        //// ���ñ༭
        //for (int col = 0; col < 6; ++col) {
        //    ui.tableWidget_Formula_Right->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //}
        // ���õ�һ�б༭
        ui.tableWidget_Formula_Right->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);
        
#if OLD_UI
        // ����һ�� QWidget ������������ť
        QWidget* operationWidget = new QWidget();
        QHBoxLayout* operationLayout = new QHBoxLayout();
        operationLayout->setContentsMargins(0, 0, 0, 0);

        operationLayout->addWidget(rowData.btn_upMove);
        operationLayout->addWidget(rowData.btn_downMove);
        operationLayout->addWidget(rowData.btnRemove);

        operationWidget->setLayout(operationLayout);
        // ��������ť������ӵ�"����"��
        ui.tableWidget_Formula_Right->setCellWidget(row, 8, operationWidget);

        connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
        connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
        connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif
        // �ָ��ź�
        ui.tableWidget_Formula_Right->blockSignals(false);
    }
  
   
}

//���淽��
void Pin2::on_pushButton_Save_clicked()
{
    //LSM->saveFormulaToFile();
    //ֻ���浱ǰ����
    saveOneFormulaToFile(ui.comboBox_Project_2->currentText());
}

void Pin2::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        // ȡ����ѡ��
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
//        // ȡ����ѡ��
//        ui.tableWidget_Formula_Left->clearSelection();
//        ui.tableWidget_Formula_Right->clearSelection();
//    }
//    if (event->button() == Qt::LeftButton) {
//        //// ��¼�϶�ǰ��״̬
//        //m_isMaximizedBeforeDrag = isMaximized();
//        //m_restoreGeometry = geometry();
//
//        //// ��������״̬����ԭ���ڲ�������С
//        //if (m_isMaximizedBeforeDrag) {
//        //    showNormal(); // �Ȼ�ԭ����
//
//        //    // �����´���λ�ã���굱ǰλ��Ϊ����
//        //    QSize normalSize = m_restoreGeometry.size();
//        //    QPoint mouseGlobalPos = event->globalPos();
//        //    QRect newGeometry(
//        //        mouseGlobalPos.x() - normalSize.width() / 2,
//        //        mouseGlobalPos.y() - 20,  // ���������߾�
//        //        normalSize.width(),
//        //        normalSize.height()
//        //    );
//
//        //    // ȷ�����ڲ�������Ļ
//        //    QScreen *screen = QApplication::screenAt(mouseGlobalPos);
//        //    if (screen) {
//        //        QRect screenGeometry = screen->availableGeometry();
//        //        newGeometry = newGeometry.intersected(screenGeometry);
//        //    }
//
//        //    setGeometry(newGeometry);
//        //}
//
//        // ��¼��ǰ���λ�ã�����ڴ��ڣ�
//        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
//        event->accept();
//    }
//
//    QWidget::mousePressEvent(event);
//}

//ɾ��ĳ�в���
void Pin2::onDeleteButtonClicked()
{
    // ��ȡ����İ�ť
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }

    // ��ȡ�к�
    int row = button->property("row").toInt();

    if (ui.tabWidget_Formula->currentIndex() == 0) 
    {
        // ���λ
        ui.tableWidget_Formula_Left->removeRow(row);

        // ɾ�� m_Formula �� ListProcess_L ��Ӧ��Ԫ��
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ɾ�� ListProcess_L �ж�Ӧ������Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.removeAt(row);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) 
    {
        // �ҵ�λ
        ui.tableWidget_Formula_Right->removeRow(row);

        // ɾ�� m_Formula �� ListProcess_R ��Ӧ��Ԫ��
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ɾ�� ListProcess_R �ж�Ӧ������Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.removeAt(row);
            }
        }
    }
}

//��������
void Pin2::onUpMoveButtonClicked() {

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int row = button->property("row").toInt();

        // �ж��Ƿ��ǵ�һ�У���һ�в�������
        if (row <= 0) {
            return;
        }

        if (ui.tabWidget_Formula->currentIndex() == 0) {
            // ���λ
            // ��������е���������
            swapTableRows(ui.tableWidget_Formula_Left, row, 0);

            // ��ȡ�䷽����
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // ���� ListProcess_L �ж�Ӧ��Ԫ��
                if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                    selectedFormula.ListProcess_L.swapItemsAt(row, row - 1);
                }
            }
        }
        else if (ui.tabWidget_Formula->currentIndex() == 1) {
            // �ҵ�λ
            // ��������е���������
            swapTableRows(ui.tableWidget_Formula_Right, row, 0);

            // ��ȡ�䷽����
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // ���� ListProcess_R �ж�Ӧ��Ԫ��
                if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                    selectedFormula.ListProcess_R.swapItemsAt(row, row - 1);
                }
            }
        }
    }
}

// ��������
void Pin2::onDownMoveButtonClicked() {

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int row = button->property("row").toInt();

        if (ui.tabWidget_Formula->currentIndex() == 0) {

            // ���λ
            // �ж��Ƿ������һ�У����һ�в�������
            int rowCount = ui.tableWidget_Formula_Left->rowCount();
            if (row >= rowCount - 1) {
                return;
            }
            // ��������е���������
            swapTableRows(ui.tableWidget_Formula_Left, row, 1);

            // ��ȡ�䷽����
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // ���� ListProcess_L �ж�Ӧ��Ԫ��
                if (row >= 0 && row < selectedFormula.ListProcess_L.size() - 1) {
                    selectedFormula.ListProcess_L.swapItemsAt(row, row + 1);
                }
            }
        }
        else if (ui.tabWidget_Formula->currentIndex() == 1) {
            // �ҵ�λ
            // �ж��Ƿ������һ�У����һ�в�������
            int rowCount = ui.tableWidget_Formula_Right->rowCount();
            if (row >= rowCount - 1) {
                return;
            }
            // ��������е���������
            swapTableRows(ui.tableWidget_Formula_Right, row, 1);

            // ��ȡ�䷽����
            QString formulaName = ui.comboBox_Project_2->currentText();
            if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
                rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
                // ���� ListProcess_R �ж�Ӧ��Ԫ��
                if (row >= 0 && row < selectedFormula.ListProcess_R.size() - 1) {
                    selectedFormula.ListProcess_R.swapItemsAt(row, row + 1);
                }
            }
        }
    }
}

// ������������е����� mode = 0�����ƣ�=1 ������
void Pin2::swapTableRows(QTableWidget* tableWidget, int row,int mode)
{
    // �����ź�
    ui.tableWidget_Formula_Left->blockSignals(true);
    ui.tableWidget_Formula_Right->blockSignals(true);
    int colCount = tableWidget->columnCount();  // ��ȡ����
    for (int col = 0; col < colCount; ++col) {
        if (mode == 0)
        {
            // ��ȡ��ǰ�к���һ�еĵ�Ԫ������
            QTableWidgetItem* item1 = tableWidget->takeItem(row, col);
            QTableWidgetItem* item2 = tableWidget->takeItem(row - 1, col);

            // �������е�����
            tableWidget->setItem(row, col, item2);
            tableWidget->setItem(row - 1, col, item1);
        }
        else if (mode == 1)
        {
            // ��ȡ��ǰ�к���һ�еĵ�Ԫ������
            QTableWidgetItem* item1 = tableWidget->takeItem(row, col);
            QTableWidgetItem* item2 = tableWidget->takeItem(row + 1, col);

            // �������е�����
            tableWidget->setItem(row, col, item2);
            tableWidget->setItem(row + 1, col, item1);
        }
    }
    // �ָ��ź�
    ui.tableWidget_Formula_Left->blockSignals(false);
    ui.tableWidget_Formula_Right->blockSignals(false);
}

//�༭
void Pin2::onEditButtonClicked(int row)
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        //int row = button->property("row").toInt();
        QString fileName = ui.comboBox_Project_2->currentText();
        if (ui.tabWidget_Formula->currentIndex() == 0) {
            if (LSM->m_Formula.count(fileName))
            {
                //�Ѿ���������䷽
                if (LSM->m_Formula[fileName].ListProcess_L.size() > row)
                {
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, LSM->m_Formula[fileName].ListProcess_L[row], row);
                    dlg.setEditMode(true);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.exec();
                }
                else
                {
                    //���λ
                    ENUMSTATION station = ENUMSTATION::L;
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, station, row);
                    dlg.setEditMode(true);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.exec();
                }
            }
            else
            {
                //���λ
                ENUMSTATION station = ENUMSTATION::L;
                qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_L, station, row);
                dlg.setEditMode(true);
                connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                dlg.exec();
            }


        }
        else if (ui.tabWidget_Formula->currentIndex() == 1)
        {
            ////�ҵ�λ
            //ENUMSTATION station = ENUMSTATION::R;
            //qg_Formula dlg(station, row);

            //connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
            //dlg.exec();

            if (LSM->m_Formula.count(fileName))
            {
                //�Ѿ���������䷽
                if (LSM->m_Formula[fileName].ListProcess_R.size() > row)
                {
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, LSM->m_Formula[fileName].ListProcess_R[row], row);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.setEditMode(true);
                    dlg.exec();
                }
                else
                {
                    //�ҵ�λ
                    ENUMSTATION station = ENUMSTATION::R;
                    qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, station, row);
                    connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                    dlg.setEditMode(true);
                    dlg.exec();
                }
            }
            else
            {
                //�ҵ�λ
                ENUMSTATION station = ENUMSTATION::R;
                qg_Formula dlg(LSM->m_Formula[fileName].ListProcess_R, station, row);
                dlg.setEditMode(true);
                connect(&dlg, &qg_Formula::signalEditFormula, this, &Pin2::slotEditFormula);
                dlg.exec();
            }
        }



    }
}

// �༭����Ĳۺ���
void Pin2::slotEditFormula(const int& row, const PointItemConfig& config)
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // �����ź�
        ui.tableWidget_Formula_Left->blockSignals(true);
        // ���λ
        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("���ɵ�");
            break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin��ʶ���");
            break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3Dɨ���");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3Dʶ��");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("ȡ����");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("����");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("�Ӿ�ģ��");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("Բ���");
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
        case ENUM_VERSION::LsPin:   //  5��
        case ENUM_VERSION::JmHan:
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            ui.tableWidget_Formula_Left->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
            ui.tableWidget_Formula_Left->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
            break;
        case ENUM_VERSION::BtPin:   //  3��
            ui.tableWidget_Formula_Left->setItem(row, 0, new QTableWidgetItem(action));
            ui.tableWidget_Formula_Left->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
            ui.tableWidget_Formula_Left->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
            ui.tableWidget_Formula_Left->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
            break;
        default:
            break;
        }

        // �ָ��ź�
        ui.tableWidget_Formula_Left->blockSignals(false);
    }
    else if(ui.tabWidget_Formula->currentIndex() == 1 && LSM->m_version == ENUM_VERSION::TwoPin) {
        // �����ź�
        ui.tableWidget_Formula_Right->blockSignals(true);
        // �ҵ�λ
        QString action = "";
        switch (config.PointType)
        {
        case ENUMPOINTTYPE::PointNone:
            action = tr("���ɵ�");
                break;
        case ENUMPOINTTYPE::Point2D:
            action = tr("2D pin��ʶ���");
                break;
        case ENUMPOINTTYPE::Point3D:
            action = tr("3Dɨ���");
            break;
        case ENUMPOINTTYPE::Point3D_identify:
            action = tr("3Dʶ��");
            break;
        case ENUMPOINTTYPE::Feeding:
            action = tr("ȡ����");
            break;
        case ENUMPOINTTYPE::TakePhoto:
            action = tr("����");
            break;
        case ENUMPOINTTYPE::Visual_Identity:
            action = tr("�Ӿ�ģ��");
            break;
        case ENUMPOINTTYPE::Circle_Measure:
            action = tr("Բ���");
            break;
        default:
            break;
        }

        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            // �������ݵ�������Ӧ��Ԫ��
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
       // �ָ��ź�
       ui.tableWidget_Formula_Right->blockSignals(false);
    }



    //����ǰ����
    auto name = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(name) != LSM->m_Formula.end())
    {
        //�Ĳ�����Ϣ
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

//�½�����������֮ǰ
void Pin2::on_pushButton_InsertFormula_clicked()
{
    auto name = ui.comboBox_Project_2->currentText();
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        //���λ
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
        //�ҵ�λ
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

//���벽���еĲۺ���
void Pin2::slotInsertFormula(const int& row, const PointItemConfig& config)
{
    if (config.StationL == ENUMSTATION::L) {
        // ��λ����
        insertFormulaRow(ui.tableWidget_Formula_Left, config, ENUMSTATION::L);
    }
    else if (config.StationL == ENUMSTATION::R) {
        // �ҹ�λ����
        insertFormulaRow(ui.tableWidget_Formula_Right, config, ENUMSTATION::R);
    }
}


// ��������
void Pin2::insertFormulaRow(QTableWidget* tableWidget, const PointItemConfig& config, int station) {
    int row = tableWidget->currentRow();
    if (row < 0)
        return;
    //����
    tableWidget->insertRow(row);

    // ���� RowData ���󣬷�װ���ݺͰ�ť
    //RowData rowData(config, row);

    // ���õ�Ԫ�������
    QString action = "";
    switch (config.PointType) {
    case ENUMPOINTTYPE::PointNone:
        action = tr("���ɵ�");
        break;
    case ENUMPOINTTYPE::Point2D:
        action = tr("2D pin��ʶ���");
        break;
    case ENUMPOINTTYPE::Point3D:
        action = tr("3Dɨ���");
        break;
    case ENUMPOINTTYPE::Point3D_identify:
        action = tr("3Dʶ��");
        break;
    case ENUMPOINTTYPE::Feeding:
        action = tr("ȡ����");
        break;
    case ENUMPOINTTYPE::TakePhoto:
        action = tr("����");
        break;
    case ENUMPOINTTYPE::Visual_Identity:
        action = tr("�Ӿ�ģ��");
        break;
    case ENUMPOINTTYPE::Circle_Measure:
        action = tr("Բ���");
        break;
    default:
        break;
    }

    // ֱ���ڵ�Ԫ���д�����ť�������ⲿ����
    QPushButton* editButton = new QPushButton("�༭", tableWidget);
    // �����źţ���ʽ���� editButton �� tableWidget��
    connect(editButton, &QPushButton::clicked, this, [this, editButton, tableWidget]() {
        // ͨ����ťֱ�ӻ�ȡ�����кţ���������ת����
        QTableWidgetItem* item = tableWidget->item(tableWidget->indexAt(editButton->pos()).row(), 0);
        if (item) {
            int actualRow = item->row();
            onEditButtonClicked(actualRow);
        }
        });
    // ���ñ��Ԫ�������
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
        // ���༭��ť��ӵ�����"�༭"��
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
		// ���༭��ť��ӵ�����"�༭"��
		tableWidget->setCellWidget(row, 9, editButton);
        break;
    case ENUM_VERSION::LsPin:   //  ����5��
    case ENUM_VERSION::JmHan:
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(config.TargetPAxis_4)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(config.TargetPAxis_5)));
        tableWidget->setItem(row, 6, new QTableWidgetItem(config.Tip));
        // ���༭��ť��ӵ�����"�༭"��
        //tableWidget->setCellWidget(row, 7, rowData.editButton);
        tableWidget->setCellWidget(row, 7, editButton);
        break;
    case ENUM_VERSION::BtPin:   //  3��
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(config.TargetPAxis_1)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(config.TargetPAxis_2)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(config.TargetPAxis_3)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(config.Tip));
        tableWidget->setCellWidget(row, 5, editButton);
        break;
    default:
        break;
    }


    //// ���ñ༭����
    //for (int col = 0; col < 6; ++col) {
    //    tableWidget->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    //}
    // ���õ�һ�б༭
    tableWidget->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    //connect(rowData.editButton, &QPushButton::clicked, this, &Pin2::onEditButtonClicked);

#if OLD_UI
    // ����һ�� QWidget ������������ť
    QWidget* operationWidget = new QWidget();
    QHBoxLayout* operationLayout = new QHBoxLayout();
    operationLayout->setContentsMargins(0, 0, 0, 0);

    operationLayout->addWidget(rowData.btn_upMove);
    operationLayout->addWidget(rowData.btn_downMove);
    operationLayout->addWidget(rowData.btnRemove);

    operationWidget->setLayout(operationLayout);

    // ��������ť������ӵ�"����"��
    tableWidget->setCellWidget(row, 8, operationWidget);

    connect(rowData.btn_upMove, &QPushButton::clicked, this, &Pin2::onUpMoveButtonClicked);
    connect(rowData.btn_downMove, &QPushButton::clicked, this, &Pin2::onDownMoveButtonClicked);
    connect(rowData.btnRemove, &QPushButton::clicked, this, &Pin2::onDeleteButtonClicked);
#endif

    QString formulaName = ui.comboBox_Project_2->currentText();
    if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
        rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];

        if (station == ENUMSTATION::L) {
            // ����λ����ָ������λ��
            selectedFormula.ListProcess_L.insert(row, config);
        }
        else if (station == ENUMSTATION::R) {
            // ���ҹ�λ����ָ������λ��
            selectedFormula.ListProcess_R.insert(row, config);
        }
    }

}

//��������
void Pin2::on_pushButton_OneStep_clicked()
{
#ifndef DEBUG_LHC

#if LOCAL_TEST == 0
    //0611����
    if (!LSM->isServeOn)
    {
        QMessageBox::warning(this, tr("����"), tr("��ʹ�ܺ�ִ�У�"));
        return;
    }
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���������У����н�����ֹͣ������"));
        return;
    }
    if (!LSM->m_isHomed.load())
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("����"), tr("δ����������������������"));
        return;
    }
#endif

#endif
    //��������ʾ�ķ���
    //clearLabelsInScrollArea();
    // ������
    QString projectName = ui.comboBox_Project_2->currentText();
    //�׶����ʼ�����ӻ�����ͨѶ
    if (LSM->m_version == EdPin)
    {
        if (LSM->m_productPara.UserAccout.isEmpty())
        {
            QMessageBox::warning(this, tr("����"), tr("�û��˺ţ������ˣ�δ��д��������д��"));
            return;
        }
        if (LSM->m_formulaToRobot.empty() || !LSM->m_formulaToRobot.count(projectName))
        {
            QMessageBox::warning(this, tr("����"), tr("�����˷������䷽��Ӧ��ϵΪ�գ�������д��"));
            return;
        }
        
        auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
        if (!rtn)
        {
            machineLog->write("������ModbusTcp����ʧ��...", Normal);
            machineLog->write("������ModbusTcp����ʧ��...", Err);
            return;
        }
    }
    clearAllViews();
    //�ж�����λ�����ҹ�λ
    bool isLeft = false;
    int ProcessListCount = ui.tableWidget_Formula_Right->rowCount();
    if (ui.tabWidget_Formula->currentIndex() == 0)
    {
        // ��λ
        isLeft = true;
        ProcessListCount = ui.tableWidget_Formula_Left->rowCount();
    }

    // �ж��Ƿ񳬳�������
    int ShowStepIndex = ui.lineEdit_RunSteps->text().toInt() - 1;
    if (ShowStepIndex < 0)
        ShowStepIndex = 0;
    if (ShowStepIndex >= ProcessListCount)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "��ʾ", "����ִ�н������Ƿ�ӵ�һ����ʼ",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (reply == QMessageBox::No)
            return;

        // �ӵ�һ����ʼ
        ShowStepIndex = 0;
    }

    // ȷ���Ƿ����е�ǰ����
    QMessageBox::StandardButton res;
    res = QMessageBox::question(nullptr, "��ʾ", "�Ƿ����е�ǰ����",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (res != QMessageBox::Yes)
        return;

    LSM->m_isOneStep.store(true);
    LSM->m_isRobotLoading.store(false);
    // �ѵ�ǰ��д�����еĹ�λ��
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
            QMessageBox::critical(this, "����", "���ȹر�������л�ҳ�棡");

            // ǿ���л���ԭ���� tab����ֹҳ���л�
            ui.tabWidget_CameraSetting->setCurrentIndex(0);
        }

        //if (cameraWindow->m_pcMyCamera_3D)
        //{
        //    QMessageBox::critical(this, "����", "����ֹͣ3D�ɼ����л�ҳ�棡");

        //    // ǿ���л���ԭ���� tab����ֹҳ���л�
        //    ui.tabWidget_CameraSetting->setCurrentIndex(0);
        //}
    }
}

//���²�������ui
void Pin2::updateStepIndex_slot()
{
    if (ui.tabWidget_Formula->currentIndex() == 0 && LSM->m_showStepIndex_L < ui.tableWidget_Formula_Left->rowCount())
    {
        //ѡ����һ��
        ui.tableWidget_Formula_Left->selectRow(LSM->m_showStepIndex_L);
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1 && LSM->m_showStepIndex_R < ui.tableWidget_Formula_Right->rowCount())
    {
        //ѡ����һ��
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
            QMessageBox::critical(this, "����", "���ȹر�������л�ҳ�棡");

            // ǿ���л���ԭ���� tab����ֹҳ���л�
            ui.tabWidget->setCurrentIndex(2);
        }

        //if (cameraWindow->m_pcMyCamera_3D)
        //{
        //    QMessageBox::critical(this, "����", "����ֹͣ3D�ɼ����л�ҳ�棡");

        //    // ǿ���л���ԭ���� tab����ֹҳ���л�
        //    ui.tabWidget->setCurrentIndex(2);
        //}
    }
}

//ѡ����λ�ı����в���ֵ
void Pin2::on_LeftRowSelected(const QModelIndex& current, const QModelIndex& previous)
{
    int selectedRow = current.row();
    ui.lineEdit_RunSteps->setText(QString::number(selectedRow + 1));
}

//����
void Pin2::on_pushButton_UpMove_clicked()
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // ���λ
        // �ж��Ƿ��ǵ�һ�У���һ�в�������
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        if (row <= 0) {
            return;
        }
        // ��������е���������
        swapTableRows(ui.tableWidget_Formula_Left, row, 0);
        ui.tableWidget_Formula_Left->selectRow(row - 1);

        // ��ȡ�䷽����
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ���� ListProcess_L �ж�Ӧ��Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.swapItemsAt(row, row - 1);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) {
        // �ҵ�λ
        // �ж��Ƿ��ǵ�һ�У���һ�в�������
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        if (row <= 0) {
            return;
        }
        // ��������е���������
        swapTableRows(ui.tableWidget_Formula_Right, row, 0);
        ui.tableWidget_Formula_Right->selectRow(row - 1);

        // ��ȡ�䷽����
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ���� ListProcess_R �ж�Ӧ��Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.swapItemsAt(row, row - 1);
            }
        }
    }
}

//����
void Pin2::on_pushButton_DownMove_clicked()
{
    if (ui.tabWidget_Formula->currentIndex() == 0) {
        // ���λ
        // �ж��Ƿ��ǵ�һ�У���һ�в�������
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        int rowCount = ui.tableWidget_Formula_Left->rowCount();
        if (row >= rowCount - 1) {
            return;
        }
        // ��������е���������
        swapTableRows(ui.tableWidget_Formula_Left, row, 1);
        ui.tableWidget_Formula_Left->selectRow(row + 1);

        // ��ȡ�䷽����
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ���� ListProcess_L �ж�Ӧ��Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.swapItemsAt(row, row + 1);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1) {
        // �ҵ�λ
        // �ж��Ƿ��ǵ�һ�У���һ�в�������
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        int rowCount = ui.tableWidget_Formula_Right->rowCount();
        if (row >= rowCount - 1) {
            return;
        }
        // ��������е���������
        swapTableRows(ui.tableWidget_Formula_Right, row, 1);
        ui.tableWidget_Formula_Right->selectRow(row + 1);

        // ��ȡ�䷽����
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ���� ListProcess_R �ж�Ӧ��Ԫ��
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
        // ���λ
        int row = ui.tableWidget_Formula_Left->currentIndex().row();
        ui.tableWidget_Formula_Left->removeRow(row);

        // ɾ�� m_Formula �� ListProcess_L ��Ӧ��Ԫ��
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ɾ�� ListProcess_L �ж�Ӧ������Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_L.size()) {
                selectedFormula.ListProcess_L.removeAt(row);
            }
        }
    }
    else if (ui.tabWidget_Formula->currentIndex() == 1)
    {
        // �ҵ�λ
        int row = ui.tableWidget_Formula_Right->currentIndex().row();
        ui.tableWidget_Formula_Right->removeRow(row);

        // ɾ�� m_Formula �� ListProcess_R ��Ӧ��Ԫ��
        QString formulaName = ui.comboBox_Project_2->currentText();
        if (LSM->m_Formula.find(formulaName) != LSM->m_Formula.end()) {
            rs_motion::Formula_Config& selectedFormula = LSM->m_Formula[formulaName];
            // ɾ�� ListProcess_R �ж�Ӧ������Ԫ��
            if (row >= 0 && row < selectedFormula.ListProcess_R.size()) {
                selectedFormula.ListProcess_R.removeAt(row);
            }
        }
    }
}

//ֱ������
void Pin2::on_pushButton_Run_clicked()
{
#ifndef DEBUG_LHC
    ////0611����
#if LOCAL_TEST == 0
    if (LSM->m_version == LsPin)
    {
        //�²㱶�����˶�
        if (!doubleSpeedThread->isRunning())
        {
            doubleSpeedThread->start();
        }
        QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
    }
    if (!LSM->isServeOn)
    {
        QMessageBox::warning(this, tr("����"), tr("��ʹ�ܺ�ִ�У�"));
        return;
    }
    if (LSM->m_isStart.load())
    {
        QMessageBox::warning(this, tr("����"), tr("���������У����н�����ֹͣ������"));
        return;
    }
    if (!LSM->m_isHomed.load())
    {
        QMessageBox::warning(this, tr("����"), tr("δ��λ��ԭ�㣬���ȸ�λ��"));
        return;
    }
    if (!ui.label_11->text().isEmpty())
    {
        QMessageBox::warning(this, tr("����"), tr("δ����������������������"));
        return;
    }
#endif

#endif
    //��������ʾ�ķ���
    //clearLabelsInScrollArea();
    clearAllViews();
    // ������
    QString projectName = ui.comboBox_Project_2->currentText();
    //�׶����ʼ�����ӻ�����ͨѶ
    if (LSM->m_version == EdPin )
    {
        if (LSM->m_productPara.UserAccout.isEmpty())
        {
            QMessageBox::warning(this, tr("����"), tr("�û��˺ţ������ˣ�δ��д��������д��"));
            return;
        }
        if (LSM->m_formulaToRobot.empty() || !LSM->m_formulaToRobot.count(projectName))
        {
            QMessageBox::warning(this, tr("����"), tr("�����˷������䷽��Ӧ��ϵΪ�գ�������д��"));
            return;
        }
        if (!LSM->m_notRobot)
        {
            auto rtn = MODBUS_TCP.open("192.168.1.12", 502);
            if (!rtn)
            {
                machineLog->write("������ModbusTcp����ʧ��...", Normal);
                machineLog->write("������ModbusTcp����ʧ��...", Err);
                return;
            }
            //����������ź�
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
        // ȷ���Ƿ����е�ǰ����
        QMessageBox::StandardButton res;
        res = QMessageBox::question(nullptr, "��ʾ", "�Ƿ����е�ǰ����",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (res != QMessageBox::Yes)
            return;

        //����λ����
        //�ж�����λ�����ҹ�λ
        bool isLeft = false;
        int ProcessListCount = ui.tableWidget_Formula_Right->rowCount();
        if (ui.tabWidget_Formula->currentIndex() == 0)
        {
            // ��λ
            isLeft = true;
            ProcessListCount = ui.tableWidget_Formula_Left->rowCount();
        }
        // �ж��Ƿ񳬳�������
        int ShowStepIndex = ui.lineEdit_RunSteps->text().toInt() - 1;
        if (ShowStepIndex < 0)
            ShowStepIndex = 0;
        if (ShowStepIndex >= ProcessListCount)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, "��ʾ", "����ִ�н������Ƿ�ӵ�һ����ʼ",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

            if (reply == QMessageBox::No)
                return;

            // �ӵ�һ����ʼ
            ShowStepIndex = 0;
        }

        if (ShowStepIndex != 0 && ShowStepIndex != 1)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, "��ʾ", "��ǰ���費Ϊ 1 ���Ƿ�ӵ�һ����ʼ����",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

            if (reply == QMessageBox::Yes)
            {
                // �ӵ�һ����ʼ
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
            //�ȶ��������Զ�״̬
            auto autoStatus = false;
            MODBUS_TCP.readIntCoil(0, 1, autoStatus);
            if (!autoStatus)
            {
                QMessageBox::warning(this, tr("����"), tr("�����˷��Զ�״̬�����������Զ�״̬��"));
                return;
            }
            //�ȶ������˱���״̬
            auto alarmStatus = false;
            MODBUS_TCP.readIntCoil(0, 4, alarmStatus);
            if (alarmStatus)
            {
                QMessageBox::warning(this, tr("����"), tr("�����˱������������������λ��"));
                return;
            }
            
            //����ֹͣ
            MODBUS_TCP.writeIntCoil(100, 2, 0);
            Sleep(300);
            MODBUS_TCP.writeIntCoil(100, 2, 1);
            Sleep(300);
            MODBUS_TCP.writeIntCoil(100, 2, 0);

            //��ʹ��
            MODBUS_TCP.writeIntCoil(100, 13, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 13, 1);
            Sleep(1500);

            //����ʹ��
            MODBUS_TCP.writeIntCoil(100, 12, 0);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 12, 1);
            //��ȡ�ŷ�ʹ��״̬
            auto servoStatus = false;
            MODBUS_TCP.readIntCoil(0, 3, servoStatus);
            materialTimer.start();
            while (!servoStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("�����˶�ȡ�ŷ�ʹ��״̬��ʱ��" + QString::number(ERROR_TIME) + " �����δ��ȡ���ŷ�ʹ�ܳɹ�", Normal);
                    MODBUS_TCP.writeIntCoil(100, 0, 0);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 3, servoStatus);
                Sleep(5);
            }

            ////����Ŀ������
            //int num = LSM->m_forMulaName.toInt();
            //if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
            //    && num != 5 && num != 6 && num != 7 &&
            //    num != 8 && num != 9)
            //{
            //    machineLog->write("�䷽�޶�Ӧ�ξߣ���", Normal);
            //    QMessageBox msgBox(QMessageBox::Warning, "����", "�䷽�޶�Ӧ�ξߣ���ȷ���䷽����", QMessageBox::Yes, this);
            //    // ��ʾ����
            //    msgBox.exec();
            //    return;
            //}

            //����Ŀ������
            machineLog->write("LSM->m_forMulaName = " + LSM->m_forMulaName, Normal);
            if (LSM->m_formulaToRobot.count(LSM->m_forMulaName))
            {
                int num = LSM->m_formulaToRobot[LSM->m_forMulaName];
                if (num != 0 && num != 1 && num != 2 && num != 3 && num != 4
                    && num != 5 && num != 6 && num != 7 &&
                    num != 8 && num != 9)
                {
                    machineLog->write("�䷽�޶�Ӧ�ξ߻����˷�������", Normal);
                    QMessageBox msgBox(QMessageBox::Warning, "����", "�䷽�޶�Ӧ�ξߣ���ȷ���䷽����", QMessageBox::Yes, this);
                    // ��ʾ����
                    msgBox.exec();
                    return;
                }
                //д������
                MODBUS_TCP.writeRegisterInt(103, num);
            }
            else
            {
                machineLog->write("δ�ҵ���Ӧ�䷽�Ļ����˷����������Ӧ��ϵ����", Normal);
                machineLog->write("δ�ҵ���Ӧ�䷽�Ļ����˷����������Ӧ��ϵ����", Err);
                return;
            }

            //���س���
            MODBUS_TCP.writeIntCoil(100, 4, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 4, 0);
            //��ȡ�������״̬
            bool programLoadStatus = false;
            MODBUS_TCP.readIntCoil(0, 11, programLoadStatus);
            materialTimer.start();
            while (!programLoadStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("�����˳������״̬��ʱ��" + QString::number(ERROR_TIME) + " �����δ���سɹ�", Normal);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 11, programLoadStatus);
                Sleep(5);
            }
            //�ӵ�һ�п�ʼ����
            MODBUS_TCP.writeIntCoil(100, 5, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 5, 0);
            //����
            MODBUS_TCP.writeIntCoil(100, 1, 1);
            Sleep(500);
            MODBUS_TCP.writeIntCoil(100, 1, 0);
            //��Զ��״̬
            bool programRunningStatus = false;
            MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
            materialTimer.start();
            while (!programRunningStatus)
            {
                if (LSM->m_isEmergency.load())
                    break;
                if (materialTimer.elapsed() > ERROR_TIME) {
                    machineLog->write("�����˳�������״̬��ʱ��" + QString::number(ERROR_TIME) + " �����δ�ɹ�����", Normal);
                    //����
                    MODBUS_TCP.writeIntCoil(100, 1, 0);
                    return;
                }
                MODBUS_TCP.readIntCoil(0, 6, programRunningStatus);
                Sleep(5);
            }

        }

        // ����Ҫ���е����в���д�����еĹ�λ��
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
                //�²㱶�����˶�
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
                //�²㱶�����˶�
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
        //˫��λ����
        //����λ
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
            //�²㱶�����˶�
            if (!doubleSpeedThread->isRunning())
            {
                doubleSpeedThread->start();
                QMetaObject::invokeMethod(doubleSpeedObject, &Thread::doubleSpeedDownAction, Qt::QueuedConnection);
            }
        }
    }
  
    //��¼����ʱ��
    QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    LSM->m_snResult.op_time = timestamp;
}

//��־���µ��ؼ�
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

//��־ˢ��
void Pin2::on_pushButton_clearLog_clicked()
{  
    ui.textEdit_Log->clear();
    ui.textEdit_Log2->clear();
}

// չʾ2Dʶ����
void Pin2::showPin_Slot(ResultParamsStruct_Pin result , PointItemConfig config)
{
    QString filePath = "";
    HObject originaImg;
    HObject resultImg;
    machineLog->write("2D����������...", Normal);
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

        //�����������ݼ�
        auto sql = qg_QSLiteWidget::getSqlWindow();
        QString photoPath;
        //����ͼƬ
        //saveHalconJPG(img, photoPath);
        //����ͼƬ���ͺ��ļ���
        if (config.ModelName.isEmpty())
        {
            //���û���ͺţ�д�䷽������
            config.ModelName = ui.label_ProjectName->text();
        }
        if (ui.checkBox_SaveImg->isChecked())
        {
            //saveHalconJPGToFile(img, config.ModelName, config.ProductName, false, picPath);
            originaImg = img.Clone();
        }
        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);
        // �� ���� pinLabel ����
        pinLabel label;

        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        label.setResult(result);
        // ��ʾͼ��
        label.setImage(img);
        label.allPinImage(result.ho_Region_AllPin);
        label.showImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //����ʾ
        showInGraphView(showQImg);
        resultImg = showHjImg;

        //���ݿ�
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
        //��ӽ����2D���ݿ�
        sql->addNew2DResult(result, para);
        //mes���
        TestResult mes;
        mes.name = config.ProductName;
        mes.result = result.hv_Ret == 1 ? "PASS" : "FAIL";
        ////�ع�
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
        //������
        //saveHalconJPGToFile(resultImg, config.ModelName, config.ProductName, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.ProductName);
        //m_resultImgPath = filePath;
        //�������NmesͼƬ
        QString machinename = ui.comboBox_Project_2->currentText();
        if (LSM->m_version == LsPin && resultImg.IsInitialized() && (machinename == "A66" || machinename == "A66-T"))
        {
            saveHalconJPGToFile_LS(resultImg, machinename, config, true, filePath);
        }

        //����NGͼƬ
        bool isNG = result.hv_Ret == 0;
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("ͼƬ����ʧ��", Normal);
    }
}

// չʾ2D��3Dʶ����
void Pin2::showPin2Dand3D_Slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)
{
    QString filePath = "";
    HObject originaImg;
    HObject resultImg;
    machineLog->write("2Dand3D����������...", Normal);
    try {
        if (!result2D.ho_Image.IsInitialized())
            return;
        HObject img = result2D.ho_Image;
        if (result2D.ho_Region_AllPin.Key() == nullptr)
        {
            return;
        }

        //�����������ݼ�
        auto sql = qg_QSLiteWidget::getSqlWindow();
        QString photoPath;
        //����ͼƬ
        //saveHalconJPG(img, photoPath);
        //����ͼƬ���ͺ��ļ���
        if (config2D.ModelName.isEmpty())
        {
            //���û���ͺţ�д�䷽������
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
        //��ӽ����2D���ݿ�
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
        //��ӽ����3D���ݿ�
        sql->addNew3DResult(result3D, para);

        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);

        //�� ���� pinLabel ����
        pinLabel label;

        label.setResult(result2D, result3D);
        // ��ʾͼ��
        label.setImage(img);
        label.showImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //����ʾ
        showInGraphView(showQImg);
        resultImg = showHjImg;
        ////������
        //saveHalconJPGToFile(showHjImg, config2D.ModelName, config2D.ProductName, true, photoPath);
        //m_resultImgPath = photoPath;
        ////�������NmesͼƬ
        //QString machinename = ui.comboBox_Project_2->currentText();
        //if (LSM->m_version == LsPin && (machinename == "A66" || machinename == "A66-T"))
        //{
        //    saveHalconJPGToFile_LS(showHjImg, machinename, config2D, true, photoPath);
        //}

        //mes���
        TestResult mes;
        mes.name = config2D.ProductName;
        bool tp = (result2D.hv_Ret == 1 && result3D.isOK == true);
        mes.result = tp ? "PASS" : "FAIL";
        ////�ع�
        //mes.result = "PASS";
        LSM->m_snResult.result_data.push_back(mes);
        //// ��ȡ��������Ĳ���
        //QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();

        //if (layout == nullptr) {
        //    layout = new QHBoxLayout();
        //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
        //}
        //// ���ÿؼ�֮��ļ��
        //layout->setSpacing(20);
        //// �� pinLabel ��ӵ�������
        //layout->addWidget(label);
        //// �����ڵ����ݳ���ʱ�Զ�����
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
        //������
        //saveHalconJPGToFile(resultImg, config2D.ModelName, config2D.ProductName, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config2D.ProductName);
        //m_resultImgPath = filePath;
        //�������NmesͼƬ
        QString machinename = ui.comboBox_Project_2->currentText();
        if (LSM->m_version == LsPin && resultImg.IsInitialized() && (machinename == "A66" || machinename == "A66-T"))
        {
            saveHalconJPGToFile_LS(resultImg, machinename, config2D, true, filePath);
        }
        //����NGͼƬ
        bool isNG = (result2D.hv_Ret == 0 || result3D.isOK == false);
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config2D.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("ͼƬ����ʧ��", Normal);
    }
}

//�л���λ���
void Pin2::changeStationUI_slot()
{
    //�е��ҹ�λ
    ui.tabWidget_Formula->setCurrentIndex(1);
}

//ѡ���ҹ�λ�ı����в���ֵ
void Pin2::on_RightRowSelected(const QModelIndex& current, const QModelIndex& previous)
{
    int selectedRow = current.row();
    ui.lineEdit_RunSteps->setText(QString::number(selectedRow + 1));
}

//��λ��ǩ�л�ʱ�����踴ԭ
void Pin2::on_tabWidget_FormulaChanged(int index) {
    ui.lineEdit_RunSteps->setText("0");
    // ȡ����ѡ��
    ui.tableWidget_Formula_Left->clearSelection();
    ui.tableWidget_Formula_Right->clearSelection();
}

//���pinʶ����
void Pin2::clearLabelsInScrollArea()
{
    // ��ȡ����
    QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();
    if (layout != nullptr) {
        // ���������е����пؼ�
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            // ɾ���ؼ�
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();  // �ӳ�ɾ��
            }
            // ɾ��������
            delete item;
        }
    }
    //if (layout == nullptr) {
    //    layout = new QHBoxLayout();
    //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
    //}

    LSM->m_pinResult2D.clear();
}

//������
void Pin2::on_pushButton_ResultClear_clicked()
{
    //clearLabelsInScrollArea();
    clearAllViews();
}


//д��csv
int Pin2::writeCsv(const QString configPath, const QList<std::pair<ResultParamsStruct_Pin, QString>> resultParamsList)
{
    try
    {
        // ����Ŀ¼������ԭ�߼���
        QFileInfo fileInfo(configPath);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists() && !dir.mkpath(".")) {
            std::cerr << "�޷�����Ŀ¼: " << dir.path().toStdString() << std::endl;
            return 1;
        }

        string path = configPath.toUtf8().data();

        // ��׷��ģʽ���ļ�������������򴴽�
        std::ofstream csvFile(path, std::ios::app);
        if (!csvFile.is_open()) {
            std::cerr << "�޷����ļ��Խ���д��" << std::endl;
            return 1;
        }

        // ����ļ��Ƿ�Ϊ�գ��´������ļ���
        bool writeHeader = false;
        {
            // ������ģʽ���ļ�������Ƿ�Ϊ��
            std::ifstream checkFile(path);
            writeHeader = checkFile.peek() == std::ifstream::traits_type::eof();
        }

        // �����Ҫд�������
        if (writeHeader) {
            // ��������˳���䷽�� -> ��� -> ������
            csvFile << "�䷽��,���,X,Y,DistanceX_Standard-Pixel,DistanceY_Standard-Pixel,"
                << "OffsetX-Pixel,OffsetY-Pixel,DistanceX_Standard-MM,DistanceY_Standard-MM,"
                << "OffsetX-MM,OffsetY-MM\n";
        }

        // ����ÿ���䷽
        for (const auto& oneResult : resultParamsList) {
            const auto& resultParams = oneResult.first;
            const QString& recipeName = oneResult.second; // ��ȡ�䷽��

            // ��¼NG����
            unordered_set<int> ngIdxSet(resultParams.ngIdx.begin(), resultParams.ngIdx.end());

            // �������ݵ�
            for (int i = 0; i < resultParams.hv_Row_Array.TupleLength().I(); ++i) {
                // ���������У��䷽�� -> ��� -> ����
                std::vector<std::string> rowData = {
                    recipeName.toStdString(),                   // �䷽������������ͬ��
                    ngIdxSet.count(i) ? "NG" : "OK",            // ���
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

                // д��CSV��
                csvFile << boost::algorithm::join(rowData, ",") << "\n";
            }
        }

        csvFile.close();
        std::cout << "CSV�ļ��ѳɹ�д��" << std::endl;
        return 0;
    }
    catch (...)
    {
        return 1;
    }
}

//ɾ������
void Pin2::on_pushButton_Cancel_clicked()
{
    // ��ȡexeĿ¼�µ�Formula_Config�ļ���·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // ���Formula_Config�ļ��в����ڣ��򴴽�
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QString fileName = ui.comboBox_Project_2->currentText();
    // �����ļ�������·��
    QString filePath = dir.filePath(fileName + ".json");

    // �ж��ļ��Ƿ��Ѵ���
    if (QFile::exists(filePath)) {
        // �ļ��Ѵ��ڣ�����ȷ�Ͽ�
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("ɾ��"), tr("�Ƿ�ɾ���˷�����"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
        //ɾ��
        QFile::remove(filePath);
    }
    // ����䷽��ͷ��������¼���
    ui.tableWidget_Formula_Left->clearContents();
    ui.tableWidget_Formula_Right->clearContents();
    ui.comboBox_Project_2->clear();
    // ��ȡ�ļ����������ļ������������ļ��У�
    dir.setFilter(QDir::Files);  // ֻ��ȡ�ļ������������ļ���
    dir.setSorting(QDir::Time | QDir::Reversed);  // ������ʱ�������µ��ļ�����ǰ��

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileList) {
        QString name = fileInfo.baseName();
        ui.comboBox_Project_2->addItem(name);
    }
}

/*
* �޸ĵ�Ԫ��ʱ���䷽UI���������µ�����(��λ)

������Ա������Ļ�Ҫ���ε�����źţ���ui->tableWidget_Formula_Left->blockSignals(true)�������ǳ�Ƶ�������˺���
*/
void Pin2::onLeftCellDoubleClicked(int row, int column)
{
    // ��ȡ��˫���ĵ�Ԫ�������
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
* �޸ĵ�Ԫ��ʱ���䷽UI���������µ�����(��λ)

������Ա������Ļ�Ҫ���ε�����źţ���ui->tableWidget_Formula_Left->blockSignals(true)�������ǳ�Ƶ�������˺���
*/
void Pin2::onRightCellDoubleClicked(int row, int column)
{
    // ��ȡ��˫���ĵ�Ԫ�������
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

// ����һ�������䷽
void Pin2::saveOneFormulaToFile(QString formulaName)
{
    // ��ȡFormula_ConfigĿ¼·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (LSM->m_Formula.count(formulaName))
    {
        rs_motion::Formula_Config formulaConfig = LSM->m_Formula[formulaName];  // ��ȡ�䷽����

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
                //�׶��������
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
                //���N�����
                pointJson["TargetPAxis_1"] = point.TargetPAxis_1;
                pointJson["TargetPAxis_2"] = point.TargetPAxis_2;
                pointJson["TargetPAxis_3"] = point.TargetPAxis_3;
                pointJson["TargetPAxis_4"] = point.TargetPAxis_4;
                pointJson["TargetPAxis_5"] = point.TargetPAxis_5;
                break;
            case ENUM_VERSION::BtPin:
                //��̩�����
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
       
        // �� QJsonObject ת��Ϊ QJsonDocument
        QJsonDocument doc(formulaJson);

        // ���� JSON ���ݵ��ļ�
        QString filePath = dir.filePath(formulaName + ".json");
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }
    }
}

// չʾ3Dʶ����ͼ
void Pin2::showPin3D_Slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)
{
    machineLog->write("3D����������...", Normal);
#ifdef DEBUG_LHC
    //�������ݼ�
    auto sql = qg_QSLiteWidget::getSqlWindow();
    sql->addNew3DResult(result, config.Vision);
#endif // DEBUG_LHC

    try {
        if (result.depthImg.empty())
            return;

        //�����������ݼ�
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

        // �� ���� pinLabel ����
        pinLabel label;

        HObject img = label.HMatToHObject(result.depthImg);
        if (!img.IsInitialized())
            return;
        HTuple hv_Width, hv_Height;
        GetImageSize(img, &hv_Width, &hv_Height);

        label.setResult(result);
        // ��ʾͼ��
        label.setImage(img);
        label.show3DImage();
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        //����ʾ
        showInGraphView(showQImg);

        //// ��ȡ��������Ĳ���
        //QLayout* layout = ui.scrollAreaWidgetContents_ShowPin->layout();

        //if (layout == nullptr) {
        //    layout = new QHBoxLayout();
        //    ui.scrollAreaWidgetContents_ShowPin->setLayout(layout);
        //}
        //// ���ÿؼ�֮��ļ��
        //layout->setSpacing(20);
        //// �� pinLabel ��ӵ�������
        //layout->addWidget(label);
        //// �����ڵ����ݳ���ʱ�Զ�����
        //ui.scrollArea_ShowPin->setWidgetResizable(true);
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showPin3D_Slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
    }
}

// ���ÿ���ģ��״̬
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
            //�������
            ui.pushButton_AxisOn->setEnabled(true);
            ui.pushButton_AxisReset->setEnabled(true);
            ui.comboBox_AxisNum->setEnabled(true);
            ui.pushButton_Homing->setEnabled(true);
            ui.pushButton_RobotProject->setEnabled(true);
            //�����û��ɲ����ؼ����Է����ֿؼ�Ȩ�ޱ��ų�
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
            //�������
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

//���ҿɱ����ļ�·��
QString Pin2::findOrCreateSaveFile()
{
    QString exePath = QCoreApplication::applicationDirPath();
    QDir photoDir(exePath + "/2D_Photo");

    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            return QString();
        }
    }

    // �� 1.jpg ��ʼ����
    int fileIndex = 1;
    QString fileName;
    do {
        fileName = photoDir.filePath(QString::number(fileIndex) + ".jpg");
        fileIndex++;
    } while (QFile::exists(fileName));  // ����ļ��Ѵ��ڣ��������������

    // �������յ��ļ�·��
    return fileName;
}

//����halconͼƬ
int Pin2::saveHalconJPG(HObject img, QString& filePath)
{
    if (!img.IsInitialized())
        return -1;

    auto name = findOrCreateSaveFile();
    WriteImage(img, "jpeg 60", 0, name.toUtf8().data());
    filePath = name;
    return 0;
}

//���Ʋ��½��䷽
void Pin2::on_pushButton_CopyNew_clicked()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("�����ļ���"), tr("�������ļ�����"), QLineEdit::Normal, "", &ok);

    if (!ok || fileName.isEmpty()) {
        return;
    }

    // ��ȡexeĿ¼�µ�Formula_Config�ļ���·��
    QDir dir(qApp->applicationDirPath() + "/Formula_Config");

    // ���Formula_Config�ļ��в����ڣ��򴴽�
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::critical(this, tr("����"), tr("�޷������ļ��У�"));
            return;
        }
    }

    // �����ļ�������·��
    QString filePath = dir.filePath(fileName + ".json");

    // �ж��ļ��Ƿ��Ѵ���
    bool isReply = false;
    if (QFile::exists(filePath)) {
        // �ļ��Ѵ��ڣ�����ȷ�Ͽ�
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("�ļ��Ѵ���"), tr("�ļ��Ѵ��ڣ��Ƿ񸲸ǣ�"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
        isReply = true;
    }

    // ������д�� JSON �ļ�
    QJsonObject jsonObject;

    QJsonDocument jsonDoc(jsonObject);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "����", "�޷����ļ�����д�룡");
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();

    // ��ȡ��ǰѡ�����Ŀ
    QString selectedItem = ui.comboBox_Project_2->currentText();

    // ����䷽���ڣ����±��
    auto it = LSM->m_Formula.find(selectedItem);
    if (it != LSM->m_Formula.end()) {
        //�浽�䷽������
        LSM->m_Formula[fileName] = LSM->m_Formula[selectedItem];
        if (!isReply)
        {
            ui.comboBox_Project_2->addItem(fileName);
        }
        // ���õ�ǰѡ��Ϊ�½����ļ���
        ui.comboBox_Project_2->setCurrentText(fileName);
        //����
        saveOneFormulaToFile(fileName);
    }
}

//���䵱��NG��
void Pin2::showNG_Slot(double value)
{
    ui.label_6->clear();
    ui.label_6->setText("��ǰNG�ʣ�"+QString::number(value)+"%");
    //ui.widget_showNG->setNgValue(value);
    ui.widget_showNG->addDataPoint("NG", value);
    ui.widget_showNG->addDataPoint("OK", 100 - value);
}

//���ݰ汾����UI
void Pin2::updataUiFromVersion()
{
    ui.tabWidget_CameraSetting->setTabVisible(3, false);  // ����һά�룻
    ui.tabWidget_CameraSetting->setTabVisible(4, false);  // ���ض�ά�룻
    ui.tabWidget_CameraSetting->setTabVisible(5, false);  // ����ģ��ƥ�䣻
    ui.tabWidget_CameraSetting->setTabVisible(6, false);  // ����Blob��
    ui.tabWidget_CameraSetting->setTabVisible(7, false);  // ����Բ��⣻
    ui.tabWidget_CameraSetting->setTabVisible(8, false);  // ���ػ�����λ��
    //�����˺��䷽��Ӧҳ��
    ui.pushButton_RobotProject->hide();
    //�����ã���ʱ����
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
        //�׶���
#if LOCAL_TEST == 0
        QPixmap pix2(":/icons/�׶���.png");
#else
        QPixmap pix2(":/icons/jiangzhun.png");
#endif
        ui.label_9->setPixmap(pix2);
        ui.tabWidget_Formula->setTabText(0, "��λ");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // �����ҹ�λ
        ui.label->clear();
        ui.label->setText("3D�Ӿ�����");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0  Y:0  U:0  U1(�ξ�):0  Z:0  Z1(����):0  Z2(����):0");
        //ui.tabWidget_Jog->removeTab(1);  // �Ƴ�IOҳ
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("U1");
        ui.comboBox_AxisNum->addItem("Z");
        ui.comboBox_AxisNum->addItem("Z1");
        ui.comboBox_AxisNum->addItem("Z2");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // һά�룻
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // ��ά�룻
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // ģ��ƥ�䣻
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob��
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // Բ��⣻
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // ������λ��
        //�����˺��䷽��Ӧҳ��
        ui.pushButton_RobotProject->show();
        break;
    }
    case ENUM_VERSION::LsPin:
    {
        //���N
        ui.tabWidget_Formula->setTabText(0, "��λ");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // �����ҹ�λ
        ui.label->clear();
        ui.label->setText("���NPin���");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0  Y1:0  Y2:0  U:0  Z:0");
        //ui.tabWidget_Jog->removeTab(1);  // �Ƴ�IOҳ
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
        //��̩
        ui.tabWidget_Formula->setTabText(0, "��λ");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // �����ҹ�λ
        ui.label->clear();
        ui.label->setText("��̩Pin���");
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
        //����pin
        ui.tabWidget_Formula->setTabText(0, "��λ");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // �����ҹ�λ
        ui.label->clear();
        ui.label->setText("����Pin��");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setTextFormat(Qt::RichText);
        ui.label_7->setText("X:0��X1(�����):0��Y:0��U:0��<br>U1(�ξ�):0��Z:0��Z1(�����):0��");
        //ui.tabWidget_Jog->removeTab(1);  // �Ƴ�IOҳ
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("X1");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("U1");
        ui.comboBox_AxisNum->addItem("Z");
        ui.comboBox_AxisNum->addItem("Z1");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // һά�룻
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // ��ά�룻
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // ģ��ƥ�䣻
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob��
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // Բ��⣻
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // ������λ��
        break;
    }
    case ENUM_VERSION::JmHan:
    {
        //��������
        ui.tabWidget_Formula->setTabText(0, "��λ");
        ui.tabWidget_Formula->setCurrentIndex(0);
        ui.tabWidget_Formula->setTabVisible(1, false);  // �����ҹ�λ
        ui.label->clear();
        ui.label->setText("�����������");
        ui.radioButton_BothRun->hide();
        ui.label_7->clear();
        ui.label_7->setText("X:0��Y:0��Y1:0��U:0��Z:0��");
        ui.comboBox_AxisNum->clear();
        ui.comboBox_AxisNum->addItem("X");
        ui.comboBox_AxisNum->addItem("Y");
        ui.comboBox_AxisNum->addItem("Y1");
        ui.comboBox_AxisNum->addItem("U");
        ui.comboBox_AxisNum->addItem("Z");
        ui.tabWidget_CameraSetting->setTabVisible(3, true);  // һά�룻
        ui.tabWidget_CameraSetting->setTabVisible(4, true);  // ��ά�룻
        ui.tabWidget_CameraSetting->setTabVisible(5, true);  // ģ��ƥ�䣻
        ui.tabWidget_CameraSetting->setTabVisible(6, true);  // Blob��
        ui.tabWidget_CameraSetting->setTabVisible(7, true);  // Բ��⣻
        ui.tabWidget_CameraSetting->setTabVisible(8, true);  // ������λ��
        break;
    }
    default:
        break;
    }

#if LOCAL_TEST == 1
    ui.label->setText("���԰����");
#endif

    if (LSM->m_version == ENUM_VERSION::LsPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //�����׶���(�����N)��Ӧ���µĿؼ�
        qg_JogButton_ED* newJogWidget = new qg_JogButton_ED(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
        //bool isConnected = connect(
        //    this, &Pin2::jogButtonEnable_signal,
        //    newJogWidget, &qg_JogButton_ED::setEnabled
        //);
        //if (!isConnected) {
        //    qDebug() << "���� qg_JogButton_ED ʧ��!";
        //}
        //connect(motionObject, &Thread::setControlEnable_signal, newJogWidget, &qg_JogButton_ED::setEnabled);
    }
    else if (LSM->m_version == ENUM_VERSION::EdPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //�����׶����Ӧ���µĿؼ�
        qg_JogButton_ED_New* newJogWidget = new qg_JogButton_ED_New(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::BtPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //�����׶����Ӧ���µĿؼ�
        qg_JogButton_BT* newJogWidget = new qg_JogButton_BT(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //��������Pin��Ӧ���µĿؼ�
        qg_JogButton_JMPin* newJogWidget = new qg_JogButton_JMPin(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmHan)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_FormulaJog;
        //�������������Ӧ���µĿؼ�
        qg_JogButton_JMHan* newJogWidget = new qg_JogButton_JMHan(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else
    {
        connect(motionObject, &Thread::setControlEnable_signal, ui.widget_FormulaJog, &qg_JogButton::setEnabled);
    }
}

//�汾д��
void Pin2::saveVersionToFile_Pin(int version)
{
    // ��ȡ�ļ�·��
    QString filePath(qApp->applicationDirPath() + "/AppVersion.json");

    // ���� JSON �������� version
    QJsonObject jsonObj;
    jsonObj["version"] = version;

    // �� QJsonObject ת��Ϊ QJsonDocument
    QJsonDocument doc(jsonObj);

    // ���� JSON ���ݵ��ļ�
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "�ɹ�����汾��: " << version;
    }
    else {
        qDebug() << "�޷������ļ�: " << filePath;
    }
}

//�汾�л�
void Pin2::on_pushButton_ChangeVersion_clicked()
{
#ifndef DEBUG_LHC
    //������֤�߼�
    bool isPasswordOK = false;
    while (!isPasswordOK) { // ѭ��ֱ��������ȷ
        // ������������Ի���
        QInputDialog pwdDialog;
        pwdDialog.setWindowTitle("������֤");
        pwdDialog.setLabelText("������������룺");
        pwdDialog.setTextEchoMode(QLineEdit::Password); // ������������

        pwdDialog.setOkButtonText("ȷ��");
        pwdDialog.setCancelButtonText("ȡ��");

        // ִ�жԻ���
        if (pwdDialog.exec() == QDialog::Accepted) {
            // ��֤���루��������"888888"��
            if (pwdDialog.textValue() == "888888") {
                isPasswordOK = true;
            }
            else {
                // ���������ʾ
                QMessageBox::critical(nullptr, "����", "������������ԣ�");
            }
        }
        else {
            // �û�ȡ������ֱ���˳�
            return;
        }
    }
#endif // !DEBUG_LHC
    // �����汾�Ի���
    qg_VersionDialog verDialog(LSM->m_version);
    int result = verDialog.exec();

    if (result == QDialog::Accepted) {
        int tp = verDialog.getVersion();
        if (LSM->m_version != tp)
        {
            LSM->m_version = verDialog.getVersion();
            //д�뵱ǰ�汾
            saveVersionToFile_Pin(LSM->m_version);
        }
        //ȷ���忨�汾
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
        QMessageBox::information(this, tr("��ʾ"), tr("������رգ������´����"),
            QMessageBox::Yes);
        qApp->exit();
    }
    else {
        return;
    }
}

//��յ�ǰ��Ʒ��������
void Pin2::clearProductMess()
{
    LSM->m_productPara.FirstPassRate = 100;
    LSM->m_productPara.NGCount = 0;
    LSM->m_productPara.OKCount = 0;
    LSM->m_productPara.ProductCount = 0;
    ui.label_FirstPassRate->setText("ֱͨ�ʣ�100%");
    ui.label_NGCount->setText("NG������0");
    ui.label_ProductCount->setText("������������0");
    ui.label_OKCount->setText("OK������0");
    //NG��
    ui.label_6->setText("NG�ʣ�" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
}

//������������
void Pin2::saveProductMess()
{
    JsonConfigManager config("product_data.json");
    // д�������JSON
    config.writeParam("FirstPassRate", LSM->m_productPara.FirstPassRate);
    config.writeParam("NGCount", LSM->m_productPara.NGCount);
    config.writeParam("OKCount", LSM->m_productPara.OKCount);
    config.writeParam("ProductCount", LSM->m_productPara.ProductCount);
    config.writeParam("UserAccout", LSM->m_productPara.UserAccout);
    config.writeParam("WorkOrderCode", LSM->m_productPara.WorkOrderCode);
    
}

//��ȡ��������
void Pin2::readProductMess()
{
    JsonConfigManager config("product_data.json");

    // ��JSON��ȡ����ֵ����Ĭ��ֵ��
    LSM->m_productPara.FirstPassRate = config.readDouble("FirstPassRate", 100.0);
    LSM->m_productPara.NGCount = config.readInt("NGCount", 0);
    LSM->m_productPara.OKCount = config.readInt("OKCount", 0);
    LSM->m_productPara.ProductCount = config.readInt("ProductCount", 0);
    LSM->m_productPara.UserAccout = config.readString("UserAccout", "");
    LSM->m_productPara.WorkOrderCode = config.readString("WorkOrderCode", "");
    
    ui.lineEdit_UserAccount->setText(LSM->m_productPara.UserAccout);
    ui.lineEdit_WorkOrderCode->setText(LSM->m_productPara.WorkOrderCode);
    //�״ζ���ҳ����
    ui.label_FirstPassRate->setText("ֱͨ�ʣ�" + QString::number(LSM->m_productPara.FirstPassRate) + "%");
    ui.label_NGCount->setText("NG������" + QString::number(LSM->m_productPara.NGCount));
    ui.label_ProductCount->setText("������������" + QString::number(LSM->m_productPara.ProductCount));
    ui.label_OKCount->setText("OK������" + QString::number(LSM->m_productPara.OKCount));
    //NG��
    ui.label_6->setText("NG�ʣ�" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
    showNG_Slot(100 - LSM->m_productPara.FirstPassRate);
}

//ˢ����ʾ�������
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
    //�����������ݼ�
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
        // �������뵽��λС��
        double rate = static_cast<double>(LSM->m_productPara.OKCount) / LSM->m_productPara.ProductCount * 100.0;
        LSM->m_productPara.FirstPassRate = std::round(rate * 100.0) / 100.0;
    }
    else {
        // ����������
        LSM->m_productPara.FirstPassRate = 0.0;
    }
    ui.label_FirstPassRate->setText("ֱͨ�ʣ�" + QString::number(LSM->m_productPara.FirstPassRate) + "%");
    ui.label_NGCount->setText("NG������" + QString::number(LSM->m_productPara.NGCount));
    ui.label_ProductCount->setText("������������" + QString::number(LSM->m_productPara.ProductCount));
    ui.label_OKCount->setText("OK������" + QString::number(LSM->m_productPara.OKCount));
    //NG��
    ui.label_6->setText("NG�ʣ�" + QString::number(100 - LSM->m_productPara.FirstPassRate) + "%");
    showNG_Slot(100 - LSM->m_productPara.FirstPassRate);
}

//�����û���Ϣ
void Pin2::setUser_slot(int mes, int time)
{
    setUser(mes);
    initUser();
    if (mes == Admin_User)
    {
        //����Ա������30���ӵ���ʱ����ʱ���Զ��л�����ͨ�û�
        adminTimer->start(time * 1000);
        //adminTimer->start(20000);
    }
    else
    {
        adminTimer->stop();
    }
}

//����Ա����ʱ�л�
void Pin2::switchToNormalUser() {
    setUser(Normal_User);
    initUser();
}

//�û��л�
void Pin2::on_pushButton_ChangeUser_clicked()
{
    qg_userChange dlg;
    connect(&dlg, &qg_userChange::setUser_signal, this, &Pin2::setUser_slot);
    dlg.exec();
}

//����halconͼƬ�浽�ļ���
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

    pathName.replace('*', '@');         // �滻����*Ϊ@
    if (isResultPic)
    {
        //����ͼ
        filePath = "/images/Result/";
    }
    QString absPath = exePath + filePath + pathName + "/"+ time;
    QDir photoDir(absPath);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + absPath, Normal);
            return -1;
        }
    }

    QString name;
    //// �� 1.jpg ��ʼ����
    //int fileIndex = 1;
    //do {
    //    name = photoDir.filePath(QString::number(fileIndex) + ".jpg");
    //    fileIndex++;
    //} while (QFile::exists(name));  // ����ļ��Ѵ��ڣ��������������

    //name = exePath + filePath + pathName + fileName+ "_" + time;
    name = fileName;
    absPath = LSM->m_resultImgPath;
    QString finalPath = "";
    //picPath = absPath;
    if (isResultPic)
    {
        absPath = LSM->m_resultImgPath;
        finalPath = absPath + "/" + name;
        //����ͼ
        WriteImage(img, "jpg 80", 0, finalPath.toUtf8().data());
    }
    else
    {
        //absPath = exePath + filePath + pathName + "/" + time;
        //HObject tmp = img.Clone();
        ////��ԭͼ
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

//ѭ������
void Pin2::on_checkBox_Loop_clicked(bool checked)
{
    if (checked)
    {
        LSM->m_isLoop.store(true);
        //��ֹ��������
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

//�ر����
void Pin2::on_pushButton_Close_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ���˳�", "ȷ��Ҫ�رճ�����",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        this->close();
    }
}

//��С��
void Pin2::on_pushButton_Min_clicked()
{
    this->showMinimized();
}

//���
void Pin2::on_pushButton_Max_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();  // �����ǰ�����״̬����ԭ����
    }
    else {
        this->showMaximized(); // ��󻯴���
    }
}

//void Pin2::mouseMoveEvent(QMouseEvent* event)
//{
//    if (event->buttons() & Qt::LeftButton) {
//        const QPoint delta = event->globalPos() - m_dragPosition;
//        const int moveThreshold = 200; // �϶���ֵ(����)
//        if (delta.manhattanLength() < moveThreshold)
//        {
//            return;
//        }
//
//        //m_isMaximizedBeforeDrag = isMaximized();
//        bool m_isMaximizedBeforeDrag = isMaximized();    // �϶�ǰ�Ƿ����
//        QRect m_restoreGeometry = geometry();
//
//        // ��������״̬����ԭ���ڲ�������С
//        if (m_isMaximizedBeforeDrag) {
//            showNormal(); // �Ȼ�ԭ����
//
//            // �����´���λ�ã���굱ǰλ��Ϊ����
//            QSize normalSize = m_restoreGeometry.size();
//            QPoint mouseGlobalPos = event->globalPos();
//            QRect newGeometry(
//                mouseGlobalPos.x() - normalSize.width() / 2,
//                mouseGlobalPos.y() - 20,  // ���������߾�
//                normalSize.width(),
//                normalSize.height()
//            );
//
//            // ȷ�����ڲ�������Ļ
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


//ͼƬ��ʾ����ͼ
void Pin2::showInGraphView(const QImage& image)
{
    QGraphicsViews* graphicsView = new QGraphicsViews(ui.scrollAreaWidgetContents_ShowPin);
    graphicsView->DispImage(image);

    // ��ʼ������
    if (!gridLayout) {
        // ɾ���ɲ��֣�����У�
        QLayout* oldLayout = ui.scrollAreaWidgetContents_ShowPin->layout();
        if (oldLayout) {
            delete oldLayout;
        }

        // ���������񲼾�
        gridLayout = new QGridLayout(ui.scrollAreaWidgetContents_ShowPin);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);

        // �����п���ȷ���
        for (int c = 0; c < maxColumns; c++) {
            gridLayout->setColumnStretch(c, 1);
        }

        ui.scrollAreaWidgetContents_ShowPin->setLayout(gridLayout);
    }

    // �����ͼ����ǰ����λ��
    gridLayout->addWidget(graphicsView, currentRow, currentCol);

    ui.scrollAreaWidgetContents_ShowPin->updateGeometry();
    ui.scrollAreaWidgetContents_ShowPin->update();
    Sleep(10);
    //ˢ��һ������Ӧ
    for (int i = 0; i < gridLayout->count(); ++i) {
        QLayoutItem* item = gridLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        // ǿ����ͼ���¼���ߴ�
        item->widget()->updateGeometry();
        item->widget()->adjustSize();

        // ֱ�ӵ���GetFitȷ������ˢ��
        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
            view->GetFit();
        }
    }

    // ����ˢ�µ�ǰ��ͼ
    graphicsView->updateGeometry();
    graphicsView->adjustSize();
    graphicsView->GetFit();
    // =============================

    // ˢ�²��ֺ͸��ؼ�
    ui.scrollAreaWidgetContents_ShowPin->adjustSize();
    ui.scrollArea_ShowPin->viewport()->update();

    //if (oldCurrentRow != currentRow)
    //{
    //    //�кŸ����ˣ�ˢ��һ������Ӧ
    //    for (int i = 0; i < gridLayout->count(); ++i) {
    //        QLayoutItem* item = gridLayout->itemAt(i);
    //        if (!item || !item->widget()) continue;

    //        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
    //            //������ͼ����Ӧˢ��һ��
    //            view->GetFit();
    //        }
    //    }
    //    oldCurrentRow = currentRow;
    //}

    // ������������
    currentCol++;
    if (currentCol >= maxColumns) {
        currentCol = 0;
        currentRow++;
    }

    // ȷ�����������Զ�����
    ui.scrollArea_ShowPin->setWidgetResizable(true);
}

//������ͼ����Ӧ
void Pin2::upDateQgraphicsView_slot()
{
    ui.scrollAreaWidgetContents_ShowPin->updateGeometry();
    ui.scrollAreaWidgetContents_ShowPin->update();
    Sleep(10);
    //ˢ��һ������Ӧ
    for (int i = 0; i < gridLayout->count(); ++i) {
        QLayoutItem* item = gridLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        // ǿ����ͼ���¼���ߴ�
        item->widget()->updateGeometry();
        item->widget()->adjustSize();

        // ֱ�ӵ���GetFitȷ������ˢ��
        if (QGraphicsViews* view = qobject_cast<QGraphicsViews*>(item->widget())) {
            view->GetFit();
        }
    }

    // ˢ�²��ֺ͸��ؼ�
    ui.scrollAreaWidgetContents_ShowPin->adjustSize();
    ui.scrollArea_ShowPin->viewport()->update();
}

//�����ͼ
void Pin2::clearAllViews()
{
    if (gridLayout == nullptr)
        return;
    // ������������
    currentRow = 0;
    currentCol = 0;
    oldCurrentRow = 0;
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

//�����ͼ
void Pin2::clearAllViews_slot()
{
    clearAllViews();
}

//ɨ��ǹ��Ϣ��ȡ
void Pin2::slot_RevMsg(QString name, QString mess) {
    //LSM->ls_tcpMess = mess;
    LSM->ls_tcpMess = mess.trimmed(); // ȥ��������β�հף������ո�\t��\r��\n��
    machineLog->write("ɨ��ǹ��Ϣ��" + mess, Normal);
    return;
}

//�û���ʼ��
void Pin2::initUser()
{
    auto user = getUser();
    switch (user)
    {
    case ENUM_USER::Normal_User:
        ui.label_User->setText("��ͨ�û�");
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
        //�������
        ui.pushButton_AxisOn->setEnabled(false);
        ui.comboBox_AxisNum->setEnabled(false);
        ui.pushButton_Homing->setEnabled(false);
        //�汾�л�
        ui.pushButton_ChangeVersion->setEnabled(false);
        ui.checkBox_Loop->setEnabled(false);
        //���ڵ��Ե�Ȩ��
        ui.tab_AxisState->setUiState(false);
        ui.pushButton_RobotProject->setEnabled(false);
        break;
    case  ENUM_USER::Admin_User:
        ui.label_User->setText("����Ա");
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
        //�������
        ui.pushButton_AxisOn->setEnabled(true);
        ui.comboBox_AxisNum->setEnabled(true);
        ui.pushButton_Homing->setEnabled(true);
        //�汾�л�
        ui.pushButton_ChangeVersion->setEnabled(true);
        ui.checkBox_Loop->setEnabled(true);
        //���ڵ��Ե�Ȩ��
        ui.tab_AxisState->setUiState(true);
        ui.pushButton_RobotProject->setEnabled(true);
        break;
    default:
        break;
    }
}

//�û��˺�
void Pin2::onUserAccountChanged()
{
    LSM->m_productPara.UserAccout = ui.lineEdit_UserAccount->text();
}

//����halconͼƬ�浽�ļ��У��������NMES��
int Pin2::saveHalconJPGToFile_LS(HObject img, QString machineName, PointItemConfig item, bool isResultPic, QString& picPath)
{
    //��·����
    QString exePath = QCoreApplication::applicationDirPath();
    QString mainPath = exePath + "/LS_Mes_Images/";
    // ��ȡ��ǰ���ڣ���-��-�գ�
    QString dateStr = QDate::currentDate().toString("yyyyMMdd");
    QString fileFolder = "";
    //�ļ��и�ʽ��������-��Ʒ��-����
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

    //ʱ�䣨Сʱ-���ӣ�
    QString time = QTime::currentTime().toString("hh-mm");
    //QString filePath = "/images/Original/";
    //if (isResultPic)
    //{
    //    //����ͼ
    //    filePath = "/images/Result/";
    //}
    QString absPath = fileFolder + "/" + time;
    QDir photoDir(absPath);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + absPath, Normal);
        }
    }

    QString modifiedModelName = item.ModelName;  // ����ԭʼ�ַ���
    modifiedModelName.replace('*', '@');         // �滻����*Ϊ@

    QString name = modifiedModelName + "-" + time + "-" + machineName + "-" + item.ProductName;
    QString finalPath = absPath + "/" + name;
    WriteImage(img, "jpg 80", 0, finalPath.toUtf8().data());
    return 0;
}

//�Ӿ����ܵĴ������ʾ��ͼ
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
        //����ͼƬ
        //saveHalconJPG(img, photoPath);
        //����ͼƬ���ͺ��ļ���
        QString picPath = "";

        //HTuple hv_Width, hv_Height;
        //GetImageSize(img, &hv_Width, &hv_Height);

        // �� ���� pinLabel ����
        pinLabel label;

        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        // ��ʾͼ��
        label.setImage(message.img);

        //------------
        label.showVisionImage(message);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);

        ////------------
        //����ʾ
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

        ////���ݿ�
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
            //���û���ͺţ�д�䷽������
            config.ModelName = ui.label_ProjectName->text();
        }
        //����ԭͼֱ���Ƶ�����ģ��
       /* if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.Vision);
        }*/
        //������
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);
        saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        //m_resultImgPath = filePath;

        //����NGͼƬ
        if (config.isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.Vision);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("ͼƬ����ʧ��", Normal);
    }
    message.clear();
}

//���ݿⴰ�ڵ���
void Pin2::on_pushButton_sql_clicked()
{
    // ��ȡQSQLiteWidgetʵ��������objectNameΪ"sqlWidget"
    qg_QSLiteWidget* sqlWidget = findChild<qg_QSLiteWidget*>("tab_Sql");
    if (!sqlWidget) return;

    // ����ԭ�������Ͳ�����Ϣ
    QWidget* originalParent = sqlWidget->parentWidget();
    QLayout* originalLayout = originalParent->layout();
    int originalFontSize = sqlWidget->font().pointSize();

    // ��ԭ�������Ƴ�����
    if (originalLayout) {
        originalLayout->removeWidget(sqlWidget);
    }

    // �����Ի������ò���
    QDialog* dialog = new QDialog(this);
    //������
    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowMaximizeButtonHint);
    //���ô�����϶���С
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

    // �Ի���ر�ʱ�ָ�����
    connect(dialog, &QDialog::finished, this, [=](int result) {
        Q_UNUSED(result);
        // �������������û�ԭ������
        sqlWidget->setParent(originalParent);
        // �ָ�����
        QList<QWidget*> originalWidgets = originalParent->findChildren<QWidget*>();
        for (QWidget* w : originalWidgets) {
            QFont font = w->font();
            font.setPointSize(originalFontSize);
            w->setFont(font);
        }
        // ������ӵ�ԭ����
        if (originalLayout) {
            originalLayout->addWidget(sqlWidget);
        }
        dialog->deleteLater(); // ����Ի���
        });

    dialog->show(); // ģ̬��ʾ�Ի���
}

//�����Ʒ����ͳ��
void Pin2::on_pushButton_clearData_clicked()
{
    clearProductMess();
}

//������ʾͼƬ
void Pin2::showImage_slot(QImage img)
{
    showInGraphView(img);
}

//дmodbusֵ type = 0 : λ��= 1 ����ֵ��= 2��������
void Pin2::writeModbus_slot(int regAddr, int bitPos, float value, int type)
{
    bool res = 0;
    if (type == 0)
    {
        //λд��
        bool tp = value == 0 ? false : true;
        res = modbusRobot.writeBit(regAddr, bitPos, tp);
    }
    else if (type == 1)
    {
        //��ֵд��
        res = modbusRobot.writeSingleRegister(regAddr, quint16(value));
    }
    else if (type == 2)
    {
        //������д��
        res = modbusRobot.writeFloatRegister(regAddr, value);
    }
    if (!res)
    {
        LSM->m_isEmergency.store(true);
        machineLog->write("modbus д���ַ: "+QString::number(regAddr)+ "��������ͨ��", Normal);
        machineLog->write("modbus д���ַ: " + QString::number(regAddr) + "��������ͨ��", Err);
        return;
    }
}

//��modbusֵ type = 0 : λ��= 1 ����ֵ��= 2��������
void Pin2::readModbus_slot(int regAddr, int bitPos, int type)
{
    if (type == 0)
    {
        //λ��ȡ
        bool bit = false;
        bool rtn = modbusRobot.readBit(regAddr, bitPos, bit);
        if (!rtn)
        {
            machineLog->write("modbus ��ȡ��ַ: " + QString::number(regAddr) + "��������ͨ��", Normal);
            machineLog->write("modbus ��ȡ��ַ: " + QString::number(regAddr) + "��������ͨ��", Err);
            //LSM->m_isEmergency.store(true);
            return;
        }
        if (regAddr == 34)
        {
            //���˶�ʱ���ź�״̬
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
            //���˶�ʱ���ź�״̬
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
        //��ֵд��
        quint16 value = 0;
        modbusRobot.readHoldingRegister(regAddr, value);
        if (regAddr == 38)
        {
            //�������źű�����
            LSM->m_RobotModbus.errorMessage = value;
        }
    }
    else if (type == 2)
    {
        //��������ȡ
        float value = 0;
        modbusRobot.readFloatRegister(regAddr, value);
    }
}

// ��ʾԲ����ͼƬ
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

        // �� ���� pinLabel ����
        pinLabel label;

        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        // ��ʾͼ��
        label.setImage(ho_img);
        label.showCircleMeasure(cir1, cir2,config,dis,isNG);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);
        resultImg = showHjImg;
        //cv::namedWindow("image", cv::WINDOW_NORMAL);
        //cv::resizeWindow("image", 800, 800);
        //cv::imshow("image", showMatImg);
        //����ʾ
        showInGraphView(showQImg);
        ////����ԭͼ
        //if (ui.checkBox_SaveImg->isChecked())
        //{
        //    saveHalconJPGToFile(ho_img, config.ModelName, config.ProductName, false, picPath);
        //}
        ////������
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
            //���û���ͺţ�д�䷽������
            config.ModelName = ui.label_ProjectName->text();
        }

        if (ui.checkBox_SaveImg->isChecked() && originaImg.IsInitialized())
        {
            saveOriginalPhotoToFile(originaImg, LSM->m_originalImgPath, config.Vision);
        }
        //������
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);

        if (!config.Tip.isEmpty())
        {
            //���ȱ�ע��
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Tip);
        }
        else
        {
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        }
        //m_resultImgPath = filePath;

        //����NGͼƬ
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("ͼƬ����ʧ��", Normal);
    }
}

//��������������
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

//�����̱߳�������
void Pin2::showConfirmationDialog(const QString& message)
{
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this,
        "����",
        message,
        QMessageBox::Yes
    );

   emit userDecision(true); // �����û�ѡ��
}

//���������ָ��
void Pin2::clearRobotAuto()
{
    MODBUS_TCP.writeIntCoil(100, 1, 0);
    MODBUS_TCP.writeIntCoil(100, 4, 0);
    MODBUS_TCP.writeIntCoil(100, 5, 0);
    MODBUS_TCP.writeIntCoil(100, 2, 0);
}

//��ͼƬ��ʾ�̵߳Ľ���
void Pin2::showthread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config)    //����2Dpin��
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showPin_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin, result),
        Q_ARG(PointItemConfig, config)
    );
}
void Pin2::showthread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D)    //2D��3Dpin��
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showPin2Dand3D_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin, result2D),
        Q_ARG(PointItemConfig, config2D),
        Q_ARG(ResultParamsStruct_Pin3DMeasure, result3D),
        Q_ARG(PointItemConfig, config3D)
    );
}

//����3Dpin��
void Pin2::showthread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config)    
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showPin3D_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Pin3DMeasure, result),
        Q_ARG(PointItemConfig, config)
    );
}

//�Ӿ����ܵĴ������ʾ��ͼ
void Pin2::showthread_showVision_slot(Vision_Message message)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showVision_slot",
        Qt::QueuedConnection,
        Q_ARG(Vision_Message, message)
    );
    message.clear();
}
// ��ʾԲ����ͼƬ
void Pin2::showthread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();

    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showCircleMeasure_slot",
        Qt::QueuedConnection,
        Q_ARG(ResultParamsStruct_Circle, cir1),
        Q_ARG(ResultParamsStruct_Circle, cir2),
        Q_ARG(PointItemConfig, config),
        Q_ARG(double, dis),
        Q_ARG(bool, isNG)
    );
}

//��ʾ�����Ӿ�ģ��ͼƬ
void Pin2::showthread_showAllVision_slot(std::unordered_map<QString, Vision_Message> message)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    for (auto messageTemp : message)
    {
        // ʹ�� Q_ARG ���ݲ���
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

// �䷽�������CTʱ��
void Pin2::showCTtime_slot(QString time)
{
    //����
    //ui.label_12->setText("�䷽����CTʱ�䣺" + time + " ms");
    //ui.label_13->setText("�䷽����CTʱ�䣺" + time + " ms");
    //��
    time = QString::number(time.toDouble() / 1000.0);
    ui.label_12->setText("�䷽����CTʱ�䣺" + time + " s");
    ui.label_13->setText("�䷽����CTʱ�䣺" + time + " s");
}

//����halconԭʼͼƬ�浽�ļ���
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
        //û��·���Ļ����÷�������ʱ��
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // �滻����*Ϊ@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName;

    try
    {
        Mat showMatImg = ImageTypeConvert::HObject2Mat(img);
        QString tpPath = finalPath + ".bmp";
        cv::imwrite(tpPath.toStdString(), showMatImg);
        machineLog->write("ԭͼ����·����" + finalPath, Normal);
    }
    catch (const std::exception&)
    {
        machineLog->write("ԭͼ����ʧ�ܣ�" + pathName, Normal);
        return -1;
    }

    return 0;
}


//����halcon���ͼƬ�浽�ļ���
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
        //û��·���Ļ����÷�������ʱ��
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // �滻����*Ϊ@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName;

    try
    {
        //����ͼ
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

//����QImage���ͼƬ�浽�ļ���
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
        //û��·���Ļ����÷�������ʱ��
        pathName = exePath + filePath + ui.label_ProjectName->text() + "/" + time;
    }

    pathName.replace('*', '@');         // �滻����*Ϊ@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName + ".jpg";

    try
    {
        //����ͼ
        rs_FunctionTool tp;
        tp.saveQImage(img, finalPath, "JPG", 80);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}

// ����״̬ state 0:������1:���У�2:ֹͣ
void Pin2::showWorkState_slot(int state)
{
    switch (state)
    {
    case 0:
        ui.label_WorkState->setStyleSheet("color: yellow; font-weight: bold;");
        ui.label_WorkState->setText("������");
        break;
    case 1:
        ui.label_WorkState->setStyleSheet(
            "color: #00FF00;"             // �޺��̣�������
            "font-weight: bold;"
        );
        ui.label_WorkState->setText("������");
        break;
    case 2:
        ui.label_WorkState->setStyleSheet("color: red; font-weight: bold;");
        ui.label_WorkState->setText("ֹͣ��");
        break;
    default:
        break;
    }
}

// ����ش󱨾�
void Pin2::on_pushButton_ErrorClear_clicked()
{
    ui.label_11->clearAll();
    LSM->m_errCode.clear();
}

// �׶�������˺��䷽�Ķ�Ӧ��ϵҳ��
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

//��ʾ�����Ӿ�ģ��ͼƬ
void Pin2::showthread_showNG_slot(HObject photo, PointItemConfig config, bool isNG)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // ʹ�� Q_ARG ���ݲ���
    QMetaObject::invokeMethod(showResultObject, "thread_showNG_slot",
        Qt::QueuedConnection,
        Q_ARG(HObject, photo),
        Q_ARG(PointItemConfig, config),
        Q_ARG(bool, isNG)
    );
}

//ֱ����ʾ����NG��ͼƬ
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

        // �� ���� pinLabel ����
        pinLabel label;
        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        // ��ʾͼ��
        label.setImage(ho_img);
        label.showNGimage(photo, config, isNG);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);
        resultImg = showHjImg;

        //����ʾ
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
        //������
        //saveHalconJPGToFile(resultImg, config.ModelName, config.Vision, true, filePath);

        if (!config.Tip.isEmpty())
        {
            //���ȱ�ע��
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Tip);
        }
        else
        {
            saveResultPhotoToFile(resultImg, LSM->m_resultImgPath, config.Vision);
        }
        //m_resultImgPath = filePath;

        //����NGͼƬ
        if (isNG && !LSM->m_Paramer.ngPicPath.isEmpty())
        {
            QString ngPath = LSM->m_Paramer.ngPicPath + "/" + LSM->m_ResultToCsv["SN"] + "/" + LSM->m_runStartTime;
            saveResultPhotoToFile(resultImg, ngPath, config.ProductName);
        }
    }
    catch (const std::exception&)
    {
        machineLog->write("ͼƬ����ʧ��", Normal);
    }
}

//��ʾ�����Ӿ�ģ��ͼƬ
void Pin2::showthread_showImage_slot(QImage img)
{
    if (!showResultThread->isRunning()) {
        showResultThread->start();
    }
    // ʹ�� Q_ARG ���ݲ���
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

//�����仯
void Pin2::onWorkOrderCodeChanged()
{
    LSM->m_productPara.WorkOrderCode = ui.lineEdit_WorkOrderCode->text();
}