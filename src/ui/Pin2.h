#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Pin2.h"
#include "PinWindow.h"
#include "pin3DMeasureWindow.h"
#include <QThread>
#include "Thread.h"
#include "rs_motion.h"
#include "RowData.h"
#include "QtcpClass.h"
#include "ShowCpuMemory.h"

class Pin2 : public QMainWindow
{
    Q_OBJECT

public:
    Pin2(QWidget *parent = nullptr);
    ~Pin2();

    PinWindow* pinWindow = nullptr;
    Pin3DMeasureWindow* pin3DMeasureWindow = nullptr;
    //扫码枪tcp通讯
    QtTcpClient m_clientQRCode;
    //易鼎丰机器人modbus tcp
    ModbusTcpClient modbusRobot;
private:
    //主运动线程，将connect函数换成标志位来判断执行
    Thread* motionObject{ nullptr };
    QThread* motionThread{ nullptr };
    //全局信号监控
    Thread* globalObject{ nullptr };
    QThread* globalThread{ nullptr };
    //立昇倍速链线程
    Thread* doubleSpeedObject{ nullptr };
    QThread* doubleSpeedThread{ nullptr };
    //易鼎丰皮带取料线程
    Thread* robotLoadEDObject{ nullptr };
    QThread* robotLoadEDThread{ nullptr };
    //易鼎丰皮带下料线程
    Thread* robotUnLoadEDObject{ nullptr };
    QThread* robotUnLoadEDThread{ nullptr };
    //结果处理及显示线程
    Thread* showResultObject{ nullptr };
    QThread* showResultThread{ nullptr };
    // 应用程序主窗口指针
    static Pin2* appWindow;
    //用户模式 0:普通用户；1：管理员；
    int m_user{ ENUM_USER::Normal_User };
    //鼠标位置
    QPoint m_dragPosition;
    //结果显示用
    int currentRow = 0;    // 当前行索引
    int currentCol = 0;    // 当前列索引
    int oldCurrentRow = 0;    // 之前的行索引
    const int maxColumns = 3; // 每行最多3个视图
    QGridLayout* gridLayout = nullptr; // 网格布局指针
    //配方方案的上一次索引
    int m_comboBox_Project_2_lastIndex = 0;
    QTimer* adminTimer; // 管理员倒计时定时器
    //CPU内存显示
    ShowCpuMemory* monitor = nullptr;
public:
    // 取得应用程序窗口
    static Pin2* getAppWindow() {
        return appWindow;
    }
    //创建PinWindow
    void createPinWindow();
    //创建Pin3DMeasureWindow
    void createPin3DMeasureWindow();
    //样式初始化
    void initStyle();
    void setupJogAxisLayout();
    ////轴正向运动
    //void SwitchAxisPStart(QString axisenum);
    ////轴反向运动
    //void SwitchAxisNStart(QString axisenum);
    //配方初始化
    void initFormula();
    //加行
    void addNewRow(const PointItemConfig config);
    // 交换表格中两行的数据 mode = 0是上移，=1 是下移
    void swapTableRows(QTableWidget* tableWidget, int row, int mode);
    void insertFormulaRow(QTableWidget* tableWidget, const PointItemConfig& config, int station);
    //写入csv
    int writeCsv(const QString configPath, const QList<std::pair<ResultParamsStruct_Pin, QString>> resultParamsList);
    // 保存一个流程配方
    void saveOneFormulaToFile(QString formulaName);
    QString findOrCreateSaveFile();
    //保存halcon图片
    int saveHalconJPG(HObject img,QString& filePath);
    //根据版本更新UI
    void updataUiFromVersion();
    //清空当前产品生产数据
    void clearProductMess();
    //保存生产数据
    void saveProductMess();
    //读取生产数据
    void readProductMess();
    //获取当前用户
    int getUser()
    {
        return m_user;
    }
    //设置当前用户
    void setUser(int tp)
    {
        m_user = tp;
    }
    //保存halcon图片存到文件夹
    int saveHalconJPGToFile(HObject img, QString pathName,QString fileName,bool isResultPic,QString& picPath);
    //图片显示到视图
    void showInGraphView(const QImage& image);
    //清除视图
    void clearAllViews();
    //扫码枪信息读取
    void slot_RevMsg(QString name, QString mess);
    //用户初始化
    void initUser();
    //保存halcon图片存到文件夹（用于立昇MES）
    int saveHalconJPGToFile_LS(HObject img, QString machineName, PointItemConfig item, bool isResultPic, QString& picPath);
    //清除机器人指令
    void clearRobotAuto();
    //保存halcon原始图片存到文件夹
    int saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName);
    //保存halcon结果图片存到文件夹
    int saveResultPhotoToFile(HObject img, QString pathName, QString fileName);
    //保存Qimage结果图片存到文件夹
    int saveResultPhotoToFile(QImage img, QString pathName, QString fileName);
public slots:
    void on_pushButton_AddFormula_clicked();
    void on_pushButton_init_clicked();
    void on_pushButton_Homing_clicked();
    void on_pushButton_JogStop_clicked();
    void updateLabelValue();
    void on_pushButton_Reset_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_AxisReset_clicked();
    void on_pushButton_AxisOn_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_AddProject_clicked();
    void on_comboBox_Project_2_currentIndexChanged(int index);
    void slotFormula(const PointItemConfig& config);
    void on_pushButton_Save_clicked();
    void onUpMoveButtonClicked();
    void onDownMoveButtonClicked();
    void onDeleteButtonClicked();
    //void onEditButtonClicked();
    void onEditButtonClicked(int row);

    void slotEditFormula(const int& row, const PointItemConfig& config);
    void on_pushButton_InsertFormula_clicked();
    void slotInsertFormula(const int& row, const PointItemConfig& config);
    void on_pushButton_OneStep_clicked();

    void onCameraSettingTabChanged(int index);
    void onTabWidgetTabChanged(int index);
    void updateStepIndex_slot();
    void on_LeftRowSelected(const QModelIndex& current, const QModelIndex& previous);
    void on_pushButton_UpMove_clicked();
    void on_pushButton_DownMove_clicked();
    void on_pushButton_DeleteRow_clicked();
    void on_pushButton_Run_clicked();
    void logUpdate_slot(QString mes, int type);
    void on_pushButton_clearLog_clicked();

    void showPin_Slot(ResultParamsStruct_Pin result, PointItemConfig config);
    void changeStationUI_slot();
    void on_RightRowSelected(const QModelIndex& current, const QModelIndex& previous);
    void on_tabWidget_FormulaChanged(int index);
    void clearLabelsInScrollArea();
    void on_pushButton_ResultClear_clicked();
    void showPin2Dand3D_Slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);
    void on_pushButton_Cancel_clicked();
    /*
    * 修改单元格时，配方UI表格参数更新到数据(左工位)

    代码里对表格操作的话要屏蔽掉这个信号，即ui->tableWidget_Formula_Left->blockSignals(true)，否则会非常频繁触发此函数
    */
    void onLeftCellDoubleClicked(int row, int column);
    void onRightCellDoubleClicked(int row, int column);
    // 展示3D识别结果图
    void showPin3D_Slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);
    void on_pushButton_RunStop_clicked();
    // 设置控制模块状态
    void setControlEnable_slot(bool state);
    //复制并新建配方
    void on_pushButton_CopyNew_clicked();
    //传输当天NG率
    void showNG_Slot(double value);
    //版本切换
    void on_pushButton_ChangeVersion_clicked();
    //版本写入
    void saveVersionToFile_Pin(int version);
    //刷新显示结果数据
    void upDataProductPara_slot(Run_Result runResult);
    //接受用户信息
    void setUser_slot(int mes,int time);
    //用户切换
    void on_pushButton_ChangeUser_clicked();
    //循环运行
    void on_checkBox_Loop_clicked(bool checked);
    //关闭软件
    void on_pushButton_Close_clicked();
    //最小化
    void on_pushButton_Min_clicked();
    //最大化
    void on_pushButton_Max_clicked();
    //用户账号
    void onUserAccountChanged();
    ////视觉功能的纯结果显示视图
    //void showVision_slot(HObject img, bool isNG);
    //视觉功能的纯结果显示视图
    void showVision_slot(Vision_Message message);
    //数据库窗口弹出
    void on_pushButton_sql_clicked();
    //清除数据统计
    void on_pushButton_clearData_clicked();
    //清除视图
    void clearAllViews_slot();
    //单纯显示图片
    void showImage_slot(QImage img);
    //写modbus值 type = 0 : 位；= 1 ：值；= 2：浮点数
    void writeModbus_slot(int regAddr, int bitPos, float value, int type);
    //读modbus值 type = 0 : 位；= 1 ：值；= 2：浮点数
    void readModbus_slot(int regAddr, int bitPos, int type);
    // 显示圆测距的图片
    void showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    //触发机器人下料
    void robotUnload_slot(bool isNG);
    //动作线程报警弹窗
    void showConfirmationDialog(const QString& message);

    //和图片显示线程的交互
    void showthread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config);    //单独2Dpin针
    void showthread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D和3Dpin针
    void showthread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //单独3Dpin针
    //视觉功能的纯结果显示视图
    void showthread_showVision_slot(Vision_Message message);
    // 显示圆测距的图片
    void showthread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    // 显示所有视觉模块图片
    void showthread_showAllVision_slot(std::unordered_map<QString, Vision_Message> message);
    void errUpdate_slot(QString mes);
    // 配方运行完成CT时间
    void showCTtime_slot(QString time);
    // 工作状态 state 0:待机；1:运行；2:停止
    void showWorkState_slot(int state);
    // 清除重大报警
    void on_pushButton_ErrorClear_clicked();
    void upDateQgraphicsView_slot();
    //管理员计时定时器
    void switchToNormalUser();
    // 易鼎丰机器人和配方的对应关系页面
    void on_pushButton_RobotProject_clicked();
    void on_checkBox_SaveImg_toggled(bool checked);
    //直接显示报错NG的图片(触发线程)
    void  showthread_showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //直接显示报错NG的图片
    void showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //单纯显示图片
    void showthread_showImage_slot(QImage img);
    //工单变化
    void onWorkOrderCodeChanged();
protected:
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject* obj, QEvent* evt) override;

    //无用
    //void mouseReleaseEvent(QMouseEvent* event) override;
private:
    Ui::Pin2Class ui;

signals:
    //点动页面状态
    void jogButtonEnable_signal(bool enable);
    void userDecision(bool confirmed); // 用户选择结果信号
};
