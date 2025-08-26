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
    //ɨ��ǹtcpͨѶ
    QtTcpClient m_clientQRCode;
    //�׶��������modbus tcp
    ModbusTcpClient modbusRobot;
private:
    //���˶��̣߳���connect�������ɱ�־λ���ж�ִ��
    Thread* motionObject{ nullptr };
    QThread* motionThread{ nullptr };
    //ȫ���źż��
    Thread* globalObject{ nullptr };
    QThread* globalThread{ nullptr };
    //���N�������߳�
    Thread* doubleSpeedObject{ nullptr };
    QThread* doubleSpeedThread{ nullptr };
    //�׶���Ƥ��ȡ���߳�
    Thread* robotLoadEDObject{ nullptr };
    QThread* robotLoadEDThread{ nullptr };
    //�׶���Ƥ�������߳�
    Thread* robotUnLoadEDObject{ nullptr };
    QThread* robotUnLoadEDThread{ nullptr };
    //���������ʾ�߳�
    Thread* showResultObject{ nullptr };
    QThread* showResultThread{ nullptr };
    // Ӧ�ó���������ָ��
    static Pin2* appWindow;
    //�û�ģʽ 0:��ͨ�û���1������Ա��
    int m_user{ ENUM_USER::Normal_User };
    //���λ��
    QPoint m_dragPosition;
    //�����ʾ��
    int currentRow = 0;    // ��ǰ������
    int currentCol = 0;    // ��ǰ������
    int oldCurrentRow = 0;    // ֮ǰ��������
    const int maxColumns = 3; // ÿ�����3����ͼ
    QGridLayout* gridLayout = nullptr; // ���񲼾�ָ��
    //�䷽��������һ������
    int m_comboBox_Project_2_lastIndex = 0;
    QTimer* adminTimer; // ����Ա����ʱ��ʱ��
    //CPU�ڴ���ʾ
    ShowCpuMemory* monitor = nullptr;
public:
    // ȡ��Ӧ�ó��򴰿�
    static Pin2* getAppWindow() {
        return appWindow;
    }
    //����PinWindow
    void createPinWindow();
    //����Pin3DMeasureWindow
    void createPin3DMeasureWindow();
    //��ʽ��ʼ��
    void initStyle();
    void setupJogAxisLayout();
    ////�������˶�
    //void SwitchAxisPStart(QString axisenum);
    ////�ᷴ���˶�
    //void SwitchAxisNStart(QString axisenum);
    //�䷽��ʼ��
    void initFormula();
    //����
    void addNewRow(const PointItemConfig config);
    // ������������е����� mode = 0�����ƣ�=1 ������
    void swapTableRows(QTableWidget* tableWidget, int row, int mode);
    void insertFormulaRow(QTableWidget* tableWidget, const PointItemConfig& config, int station);
    //д��csv
    int writeCsv(const QString configPath, const QList<std::pair<ResultParamsStruct_Pin, QString>> resultParamsList);
    // ����һ�������䷽
    void saveOneFormulaToFile(QString formulaName);
    QString findOrCreateSaveFile();
    //����halconͼƬ
    int saveHalconJPG(HObject img,QString& filePath);
    //���ݰ汾����UI
    void updataUiFromVersion();
    //��յ�ǰ��Ʒ��������
    void clearProductMess();
    //������������
    void saveProductMess();
    //��ȡ��������
    void readProductMess();
    //��ȡ��ǰ�û�
    int getUser()
    {
        return m_user;
    }
    //���õ�ǰ�û�
    void setUser(int tp)
    {
        m_user = tp;
    }
    //����halconͼƬ�浽�ļ���
    int saveHalconJPGToFile(HObject img, QString pathName,QString fileName,bool isResultPic,QString& picPath);
    //ͼƬ��ʾ����ͼ
    void showInGraphView(const QImage& image);
    //�����ͼ
    void clearAllViews();
    //ɨ��ǹ��Ϣ��ȡ
    void slot_RevMsg(QString name, QString mess);
    //�û���ʼ��
    void initUser();
    //����halconͼƬ�浽�ļ��У��������NMES��
    int saveHalconJPGToFile_LS(HObject img, QString machineName, PointItemConfig item, bool isResultPic, QString& picPath);
    //���������ָ��
    void clearRobotAuto();
    //����halconԭʼͼƬ�浽�ļ���
    int saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName);
    //����halcon���ͼƬ�浽�ļ���
    int saveResultPhotoToFile(HObject img, QString pathName, QString fileName);
    //����Qimage���ͼƬ�浽�ļ���
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
    * �޸ĵ�Ԫ��ʱ���䷽UI���������µ�����(��λ)

    ������Ա������Ļ�Ҫ���ε�����źţ���ui->tableWidget_Formula_Left->blockSignals(true)�������ǳ�Ƶ�������˺���
    */
    void onLeftCellDoubleClicked(int row, int column);
    void onRightCellDoubleClicked(int row, int column);
    // չʾ3Dʶ����ͼ
    void showPin3D_Slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);
    void on_pushButton_RunStop_clicked();
    // ���ÿ���ģ��״̬
    void setControlEnable_slot(bool state);
    //���Ʋ��½��䷽
    void on_pushButton_CopyNew_clicked();
    //���䵱��NG��
    void showNG_Slot(double value);
    //�汾�л�
    void on_pushButton_ChangeVersion_clicked();
    //�汾д��
    void saveVersionToFile_Pin(int version);
    //ˢ����ʾ�������
    void upDataProductPara_slot(Run_Result runResult);
    //�����û���Ϣ
    void setUser_slot(int mes,int time);
    //�û��л�
    void on_pushButton_ChangeUser_clicked();
    //ѭ������
    void on_checkBox_Loop_clicked(bool checked);
    //�ر����
    void on_pushButton_Close_clicked();
    //��С��
    void on_pushButton_Min_clicked();
    //���
    void on_pushButton_Max_clicked();
    //�û��˺�
    void onUserAccountChanged();
    ////�Ӿ����ܵĴ������ʾ��ͼ
    //void showVision_slot(HObject img, bool isNG);
    //�Ӿ����ܵĴ������ʾ��ͼ
    void showVision_slot(Vision_Message message);
    //���ݿⴰ�ڵ���
    void on_pushButton_sql_clicked();
    //�������ͳ��
    void on_pushButton_clearData_clicked();
    //�����ͼ
    void clearAllViews_slot();
    //������ʾͼƬ
    void showImage_slot(QImage img);
    //дmodbusֵ type = 0 : λ��= 1 ��ֵ��= 2��������
    void writeModbus_slot(int regAddr, int bitPos, float value, int type);
    //��modbusֵ type = 0 : λ��= 1 ��ֵ��= 2��������
    void readModbus_slot(int regAddr, int bitPos, int type);
    // ��ʾԲ����ͼƬ
    void showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    //��������������
    void robotUnload_slot(bool isNG);
    //�����̱߳�������
    void showConfirmationDialog(const QString& message);

    //��ͼƬ��ʾ�̵߳Ľ���
    void showthread_showPin_slot(ResultParamsStruct_Pin result, PointItemConfig config);    //����2Dpin��
    void showthread_showPin2Dand3D_slot(ResultParamsStruct_Pin result2D, PointItemConfig config2D, ResultParamsStruct_Pin3DMeasure result3D, PointItemConfig config3D);    //2D��3Dpin��
    void showthread_showPin3D_slot(ResultParamsStruct_Pin3DMeasure result, PointItemConfig config);    //����3Dpin��
    //�Ӿ����ܵĴ������ʾ��ͼ
    void showthread_showVision_slot(Vision_Message message);
    // ��ʾԲ����ͼƬ
    void showthread_showCircleMeasure_slot(ResultParamsStruct_Circle cir1, ResultParamsStruct_Circle cir2, PointItemConfig config, double dis, bool isNG);
    // ��ʾ�����Ӿ�ģ��ͼƬ
    void showthread_showAllVision_slot(std::unordered_map<QString, Vision_Message> message);
    void errUpdate_slot(QString mes);
    // �䷽�������CTʱ��
    void showCTtime_slot(QString time);
    // ����״̬ state 0:������1:���У�2:ֹͣ
    void showWorkState_slot(int state);
    // ����ش󱨾�
    void on_pushButton_ErrorClear_clicked();
    void upDateQgraphicsView_slot();
    //����Ա��ʱ��ʱ��
    void switchToNormalUser();
    // �׶�������˺��䷽�Ķ�Ӧ��ϵҳ��
    void on_pushButton_RobotProject_clicked();
    void on_checkBox_SaveImg_toggled(bool checked);
    //ֱ����ʾ����NG��ͼƬ(�����߳�)
    void  showthread_showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //ֱ����ʾ����NG��ͼƬ
    void showNG_slot(HObject photo, PointItemConfig config, bool isNG = false);
    //������ʾͼƬ
    void showthread_showImage_slot(QImage img);
    //�����仯
    void onWorkOrderCodeChanged();
protected:
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject* obj, QEvent* evt) override;

    //����
    //void mouseReleaseEvent(QMouseEvent* event) override;
private:
    Ui::Pin2Class ui;

signals:
    //�㶯ҳ��״̬
    void jogButtonEnable_signal(bool enable);
    void userDecision(bool confirmed); // �û�ѡ�����ź�
};
