#include "qg_Formula.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qmessagebox.h>
#include <QButtonGroup>

qg_Formula::qg_Formula(QList<PointItemConfig> allconfig, ENUMSTATION station, int editRow,QDialog *parent)
	: QDialog(parent),m_station(station), m_editRow(editRow),m_allConfig(allconfig)
{
	ui.setupUi(this);

    initVersion();
    initConnect();
    
    // Ĭ�Ϲ��ɵ�
    ui.radioButton_None->setChecked(true);
}

qg_Formula::qg_Formula(QList<PointItemConfig> allconfig, PointItemConfig config, int editRow, QDialog* parent)
    :QDialog(parent), m_editRow(editRow), m_allConfig(allconfig), m_config(config)
{
    ui.setupUi(this);
    initVersion();
    initConnect();
    // ����Ϊ��ǰ��Ľ���
    m_station = ENUMSTATION(config.StationL);
    ui.comboBox_SpeedMode->setCurrentIndex(config.SpeedModel);
    switch (config.PointType)
    {
    case ENUMPOINTTYPE::PointNone:
        ui.radioButton_None->setChecked(true);
        break;
    case ENUMPOINTTYPE::Point2D:
        //2Dʶ��
        ui.radioButton_2D->setChecked(true);
        ui.comboBox_Formula_2D->setCurrentText(config.Vision);
        //�����ͷ��
        if (!m_config.ProductName.isEmpty())
        {
            ui.comboBox_ProductName_2D->setCurrentText(m_config.ProductName);
        }
        //��ǰ���
        if (!m_config.CameraName.isEmpty())
        {
            ui.comboBox_cameraName_2->setCurrentText(m_config.CameraName);
        }
        //�ع�ʱ��
        ui.lineEdit_ExposureTime->setText(QString::number(config.ExposureTime));
        break;
    case ENUMPOINTTYPE::Point3D:
        //3Dɨ��
        ui.radioButton_3D->setChecked(true);
        if (!m_config.CloudName.isEmpty())
        {
            ui.comboBox_3dCloud->setCurrentText(m_config.CloudName);
        }
        if (m_config.isSave3dCloud == 1)
        {
            ui.checkBox_save3dCloud->setChecked(true);
        }
        ui.lineEdit_3dScanRow->setText(QString::number(config.scanRow3d));
        ui.lineEdit_ExposureTime_3D->setText(QString::number(config.ExposureTime));
        break;
    case ENUMPOINTTYPE::Point3D_identify:
        //3Dʶ��
        ui.radioButton_3Didentify->setChecked(true);
        //���������
        if (!m_config.CloudName.isEmpty())
        {
            ui.comboBox_3dCloud_2->setCurrentText(m_config.CloudName);
        }
        //�����ͷ��
        if (!m_config.ProductName.isEmpty())
        {
            ui.comboBox_ProductName_3D->setCurrentText(m_config.ProductName);
        }
        //�����䷽��
        if (!m_config.Vision.isEmpty())
        {
            ui.comboBox_Formula_3D->setCurrentText(m_config.Vision);
        }
        break;
    case ENUMPOINTTYPE::Feeding:
        //��ǰȡ����ģʽ
        ui.radioButton_Feeding->setChecked(true);
        ui.comboBox_FeedingAction->setCurrentIndex(m_config.FeedAction);
        break;
    case ENUMPOINTTYPE::TakePhoto:
        //����
        ui.radioButton_takePhoto->setChecked(true);
        //��ǰ���
        if (!m_config.CameraName.isEmpty())
        {
            ui.comboBox_cameraName->setCurrentText(m_config.CameraName);
        }
        //��ǰ��Ƭ
        if (!m_config.PhotoName.isEmpty())
        {
            ui.comboBox_photoName->setCurrentText(m_config.PhotoName);
        }
        //�ع�ʱ��
        ui.lineEdit_Exposure_2->setText(QString::number(config.ExposureTime));
        break;
    case ENUMPOINTTYPE::Visual_Identity:
        //�Ӿ��䷽
        ui.radioButton_Visual_Identity->setChecked(true);
        //��ǰ��Ƭ
        if (!m_config.PhotoName.isEmpty())
        {
            ui.comboBox_photoName_2->setCurrentText(m_config.PhotoName);
        }
        //��ǰ�Ӿ�ģ��
        ui.comboBox_Visual_Identity->setCurrentIndex(m_config.VisionConfig);
        //��ǰ�䷽��
        if (!m_config.Vision.isEmpty())
        {
            ui.comboBox_Formula_TempMatch->setCurrentText(m_config.Vision);
        }
        //�Ƿ�ϲ�ͼ��
        ui.checkBox_ResultMerging->setChecked(m_config.isResultMerging);
        break;
    case ENUMPOINTTYPE::Circle_Measure:
        //Բ���
        ui.radioButton_CircleMeasure->setChecked(true);
        //��ǰ��Ƭ
        if (!m_config.PhotoName.isEmpty())
        {
            ui.comboBox_photoName_3->setCurrentText(m_config.PhotoName);
        }
        //��ǰ�䷽��
        if (!m_config.Vision.isEmpty())
        {
            ui.comboBox_Formula_CircleOne->setCurrentText(m_config.Vision);
        }
        if (!m_config.VisionTwo.isEmpty())
        {
            ui.comboBox_Formula_CircleTwo->setCurrentText(m_config.VisionTwo);
        }
        //Բ����׼ֵ
        ui.lineEdit_Benchmark->setText(QString::number(m_config.CircleBenchmark));
        ui.lineEdit_UpError->setText(QString::number(m_config.CircleUpError));
        ui.lineEdit_DownError->setText(QString::number(m_config.CircleDownError));
        ui.lineEdit_PixDistance->setText(QString::number(m_config.PixDistance));
        
        break;
    default:
        break;
    }

}

qg_Formula::~qg_Formula()
{}

void qg_Formula::initConnect()
{
    modeButtonGroup = new QButtonGroup(this);
    modeButtonGroup->addButton(ui.radioButton_2D, ENUMPOINTTYPE::Point2D);
    modeButtonGroup->addButton(ui.radioButton_3D, ENUMPOINTTYPE::Point3D);
    modeButtonGroup->addButton(ui.radioButton_None, ENUMPOINTTYPE::PointNone);
    modeButtonGroup->addButton(ui.radioButton_3Didentify, ENUMPOINTTYPE::Point3D_identify);
    modeButtonGroup->addButton(ui.radioButton_Feeding, ENUMPOINTTYPE::Feeding);
    modeButtonGroup->addButton(ui.radioButton_takePhoto, ENUMPOINTTYPE::TakePhoto);
    modeButtonGroup->addButton(ui.radioButton_Visual_Identity, ENUMPOINTTYPE::Visual_Identity);
    modeButtonGroup->addButton(ui.radioButton_CircleMeasure, ENUMPOINTTYPE::Circle_Measure);
    
    //switch (LSM->m_version)
    //{
    //case ENUM_VERSION::EdPin:
    //    
    //    modeButtonGroup->addButton(ui.radioButton_takePhoto, ENUMPOINTTYPE::TakePhoto);
    //    modeButtonGroup->addButton(ui.radioButton_Visual_Identity, ENUMPOINTTYPE::Visual_Identity);
    //    break;
    //default:
    //    break;
    //}
    connect(modeButtonGroup, &QButtonGroup::idToggled,this, &qg_Formula::handleModeChange);
}


//OK�����Ϣ�����ȥ
void qg_Formula::on_pushButton_OK_clicked()
{
    PointItemConfig config;
    if (m_editMode)
    {
        //�༭ģʽ
        config = m_config;
    }

    // ��ȡ��ǰѡ�е�ID
    int currentModeId = modeButtonGroup->checkedId();

    // ����δѡ������������ϲ��ᴥ������Ĭ��������"None"���ף�
    if (currentModeId == -1) {
        QMessageBox::warning(this, "Error", "����ѡ��ģʽ��");
        return;
    }

    switch (currentModeId)
    {
    case ENUMPOINTTYPE::PointNone:
        // ���ȵ�
        config.PointType = ENUMPOINTTYPE::PointNone;
        break;
    case ENUMPOINTTYPE::Point2D:
        //2D pin��ʶ���
#if LOCAL_TEST == 0
         //0611����
        if (ui.comboBox_cameraName_2->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ�����",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
#endif
        config.PointType = ENUMPOINTTYPE::Point2D;
        config.Vision = ui.comboBox_Formula_2D->currentText();
        config.ProductName = ui.comboBox_ProductName_2D->currentText();
        config.CameraName = ui.comboBox_cameraName_2->currentText();
        config.ExposureTime = ui.lineEdit_ExposureTime->text().toInt();
        break;
    case ENUMPOINTTYPE::Point3D:
        //3Dɨ���
        config.PointType = ENUMPOINTTYPE::Point3D;
        config.CloudName = ui.comboBox_3dCloud->currentText();
        config.isSave3dCloud = ui.checkBox_save3dCloud->isChecked();
        config.scanRow3d = ui.lineEdit_3dScanRow->text().toInt();
        config.ExposureTime = ui.lineEdit_ExposureTime_3D->text().toInt();
        break;
    case ENUMPOINTTYPE::Point3D_identify:
        //3Dʶ���
        config.PointType = ENUMPOINTTYPE::Point3D_identify;
        config.Vision = ui.comboBox_Formula_3D->currentText();
        config.CloudName = ui.comboBox_3dCloud_2->currentText();
        config.ProductName = ui.comboBox_ProductName_3D->currentText();
        break;
    case ENUMPOINTTYPE::Feeding:
        //ȡ����
        config.PointType = ENUMPOINTTYPE::Feeding;
        config.FeedAction = ui.comboBox_FeedingAction->currentIndex();
        break;
    case ENUMPOINTTYPE::TakePhoto:
        //����
        if (ui.comboBox_cameraName->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ�����",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
        if (ui.comboBox_photoName->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ����Ƭ",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
        config.PointType = ENUMPOINTTYPE::TakePhoto;
        config.CameraName = ui.comboBox_cameraName->currentText();
        config.PhotoName = ui.comboBox_photoName->currentText();
        config.ExposureTime = ui.lineEdit_Exposure_2->text().toInt();
        break;
    case ENUMPOINTTYPE::Visual_Identity:
        //�Ӿ�ģ��
        if (ui.comboBox_photoName_2->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ����Ƭ",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
        config.PointType = ENUMPOINTTYPE::Visual_Identity;
        config.PhotoName = ui.comboBox_photoName_2->currentText();
        config.VisionConfig = ui.comboBox_Visual_Identity->currentIndex();
        config.Vision = ui.comboBox_Formula_TempMatch->currentText();
        config.isResultMerging = ui.checkBox_ResultMerging->isChecked();
        break;
    case ENUMPOINTTYPE::Circle_Measure:
        //Բ���
        if (ui.comboBox_Formula_CircleOne->currentText().isEmpty() || ui.comboBox_Formula_CircleTwo->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ���䷽",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
        if (ui.comboBox_photoName_3->currentText().isEmpty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "����", "δѡ����Ƭ",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            return;
        }
        config.PointType = ENUMPOINTTYPE::Circle_Measure;
        //�䷽
        config.Vision = ui.comboBox_Formula_CircleOne->currentText();
        config.VisionTwo = ui.comboBox_Formula_CircleTwo->currentText();
        //Բ������
        config.CircleBenchmark = ui.lineEdit_Benchmark->text().toDouble();
        config.CircleUpError = ui.lineEdit_UpError->text().toDouble();
        config.CircleDownError = ui.lineEdit_DownError->text().toDouble();
        config.PixDistance = ui.lineEdit_PixDistance->text().toDouble();
        config.PhotoName = ui.comboBox_photoName_3->currentText();
        
        break;
    default:
        break;
    }

    int speedMode = ui.comboBox_SpeedMode->currentIndex();
    config.SpeedModel = speedMode;
    auto a = LSM->m_Axis[LS_AxisName::LS_Z].position;
    if (m_editMode)
    {
        //�༭ģʽ
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "ȷ��", "�Ƿ���ĵ�λΪ��ǰ��ͷλ�ã�",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // ��λ
            switch (LSM->m_version)
            {
            case ENUM_VERSION::TwoPin:
                config.TargetPX = LSM->m_Axis[LS_AxisName::X].position;
                if (m_station == ENUMSTATION::L)
                {
                    config.TargetPY = LSM->m_Axis[LS_AxisName::Y1].position;
                    config.TargetPU = LSM->m_Axis[LS_AxisName::U1].position;
                }
                else if (m_station == ENUMSTATION::R)
                {
                    config.StationL = ENUMSTATION::R;
                    config.TargetPY = LSM->m_Axis[LS_AxisName::Y2].position;
                    config.TargetPU = LSM->m_Axis[LS_AxisName::U2].position;
                }
                config.TargetPZ = LSM->m_Axis[LS_AxisName::Z].position;
                config.TargetPRZ = LSM->m_Axis[LS_AxisName::RZ].position;
                break;
            case ENUM_VERSION::EdPin:
                //�׶���
                config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
                config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y].position;
                config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::U].position;
                config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U1].position;
                config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::Z].position;
                config.TargetPAxis_6 = LSM->m_Axis[LS_AxisName::Z1].position;
                config.TargetPAxis_7 = LSM->m_Axis[LS_AxisName::Z2].position;
                break;
            case ENUM_VERSION::LsPin:
                //���N
                config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::LS_X].position;
                config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::LS_Y1].position;
                config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::LS_Y2].position;
                config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::LS_U].position;
                config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::LS_Z].position;
                break;
            case ENUM_VERSION::BtPin:
                //��̩
                config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
                config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y1].position;
                config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y2].position;
                break;
            case ENUM_VERSION::JmPin:
                //����Pin
                config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
                config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::X1].position;
                config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y].position;
                config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U].position;
                config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::U1].position;
                config.TargetPAxis_6 = LSM->m_Axis[LS_AxisName::Z].position;
                config.TargetPAxis_7 = LSM->m_Axis[LS_AxisName::Z1].position;
                break;
            case ENUM_VERSION::JmHan:
                //����Han
                config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
                config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y].position;
                config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y1].position;
                config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U].position;
                config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::Z].position;
                break;
            default:
                break;
            }
        }

        //�����༭���ź�
        emit signalEditFormula(m_editRow, config);
    }
    else
    {
        // ��λ
        switch (LSM->m_version)
        {
        case ENUM_VERSION::TwoPin:
            config.TargetPX = LSM->m_Axis[LS_AxisName::X].position;
            if (m_station == ENUMSTATION::L)
            {
                config.TargetPY = LSM->m_Axis[LS_AxisName::Y1].position;
                config.TargetPU = LSM->m_Axis[LS_AxisName::U1].position;
            }
            else if (m_station == ENUMSTATION::R)
            {
                config.StationL = ENUMSTATION::R;
                config.TargetPY = LSM->m_Axis[LS_AxisName::Y2].position;
                config.TargetPU = LSM->m_Axis[LS_AxisName::U2].position;
            }
            config.TargetPZ = LSM->m_Axis[LS_AxisName::Z].position;
            config.TargetPRZ = LSM->m_Axis[LS_AxisName::RZ].position;
            break;
        case ENUM_VERSION::EdPin:
            //�׶���
            config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
            config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y].position;
            config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::U].position;
            config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U1].position;
            config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::Z].position;
            config.TargetPAxis_6 = LSM->m_Axis[LS_AxisName::Z1].position;
            config.TargetPAxis_7 = LSM->m_Axis[LS_AxisName::Z2].position;
            break;
        case ENUM_VERSION::LsPin:
            //���N
            config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::LS_X].position;
            config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::LS_Y1].position;
            config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::LS_Y2].position;
            config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::LS_U].position;
            config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::LS_Z].position;
            break;
        case ENUM_VERSION::BtPin:
            //��̩
            config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
            config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y1].position;
            config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y2].position;
            break;
        case ENUM_VERSION::JmPin:
            //����
            config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
            config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::X1].position;
            config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y].position;
            config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U].position;
            config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::U1].position;
            config.TargetPAxis_6 = LSM->m_Axis[LS_AxisName::Z].position;
            config.TargetPAxis_7 = LSM->m_Axis[LS_AxisName::Z1].position;
            break;
        case ENUM_VERSION::JmHan:
            //����Han
            config.TargetPAxis_1 = LSM->m_Axis[LS_AxisName::X].position;
            config.TargetPAxis_2 = LSM->m_Axis[LS_AxisName::Y].position;
            config.TargetPAxis_3 = LSM->m_Axis[LS_AxisName::Y1].position;
            config.TargetPAxis_4 = LSM->m_Axis[LS_AxisName::U].position;
            config.TargetPAxis_5 = LSM->m_Axis[LS_AxisName::Z].position;
            break;
        default:
            break;
        }

        if (m_editRow != -1)
        {            //����������ź�
            emit signalEditFormula(m_editRow, config);
        }
        else
        {
            //�����½����ź�
            emit signalFormula(config);
        }
    }

    close();
}

//��ͬģʽ�л�
void qg_Formula::handleModeChange(int id, bool isChecked)
{
    if (!isChecked)
        return;
    ui.stackedWidget->show();
    switch (id)
    {
    case ENUMPOINTTYPE::PointNone:
        //���ɵ�
        ui.stackedWidget->hide();
        break;
    case ENUMPOINTTYPE::Point2D:
        //2D pin��ʶ���
        ui.stackedWidget->setCurrentIndex(0);
        initRadioButton_2D();
        break;
    case ENUMPOINTTYPE::Point3D:
        //3Dɨ���
        ui.stackedWidget->setCurrentIndex(1);
        initRadioButton_3D();
        break;
    case ENUMPOINTTYPE::Point3D_identify:
        //3Dʶ���
        ui.stackedWidget->setCurrentIndex(2);
        initRadioButton_3Didentify();
        break;
    case ENUMPOINTTYPE::Feeding:
        //ȡ����
        ui.stackedWidget->setCurrentIndex(3);
        initRadioButton_Feeding();
        break;
    case ENUMPOINTTYPE::TakePhoto:
        //����
        ui.stackedWidget->setCurrentIndex(4);
        initRadioButton_TakePhoto();
        break;
    case ENUMPOINTTYPE::Visual_Identity:
        //�Ӿ�ģ��
        ui.stackedWidget->setCurrentIndex(5);
        initRadioButton_Visual_Identity();
        break;  
    case ENUMPOINTTYPE::Circle_Measure:
        //Բ���
        ui.stackedWidget->setCurrentIndex(6);
        initRadioButton_CircleMeasure();
        break;

    default:
        break;
    }
}

void qg_Formula::initRadioButton_2D()
{
    // ��ȡ��ǰ exe Ŀ¼·��
    QString exeDir = QCoreApplication::applicationDirPath();

    // ƴ�ӵ� 2D_formulation �ļ���·��
    QDir directory(exeDir + "/2D_formulation");

    // ����ļ����Ƿ����
    if (!directory.exists()) {
        QMessageBox::warning(this, tr("Warning"), tr("Directory does not exist."));
        return;
    }

    // ��ȡ��Ŀ¼������ .xml �ļ�
    QStringList filters;
    filters << "*.xml";  // ֻ���� XML �ļ�
    directory.setNameFilters(filters);

    // ��ȡ�ļ��б�
    QFileInfoList files = directory.entryInfoList(QDir::Files);

    // �������� XML �ļ�������ļ����� comboBox_Formula ��
    for (const QFileInfo& file : files) {
        QString fileName = file.fileName();  // ��ȡ�ļ�����������·��
        fileName.chop(4);  // ȥ�� .xml ��׺
        ui.comboBox_Formula_2D->addItem(fileName);  // ���ļ�����ӵ� comboBox_Formula ��
    }

    // �������в�ͷ��
    for (auto config : m_allConfig)
    {
        if (!config.ProductName.isEmpty())
        {
            int index = ui.comboBox_ProductName_2D->findText(config.ProductName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_ProductName_2D->addItem(config.ProductName);
        }
    }

    for (const auto& camera : LSM->m_cameraName)
    {
        if (!camera.first.isEmpty())
        {
            int index = ui.comboBox_cameraName_2->findText(camera.first, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_cameraName_2->addItem(camera.first);
        }
        //ui.comboBox_cameraName->addItem(camera.first);
    }
}

void qg_Formula::initRadioButton_3D()
{
    // ���������
    // �������е�����
    for (auto config : m_allConfig)
    {
        if (!config.CloudName.isEmpty())
        {
            int index = ui.comboBox_3dCloud->findText(config.CloudName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_3dCloud->addItem(config.CloudName);
        }
    }
}

//3Dʶ���
void qg_Formula::initRadioButton_3Didentify()
{
    // ��ȡ��ǰ exe Ŀ¼·��
    QString exeDir = QCoreApplication::applicationDirPath();

    // ƴ�ӵ� 2D_formulation �ļ���·��
    QDir directory(exeDir + "/3D_formulation");

    // ����ļ����Ƿ����
    if (!directory.exists()) {
        //QMessageBox::warning(this, tr("Warning"), tr("Directory does not exist."));
        return;
    }

    // ��ȡ��Ŀ¼������ .xml �ļ�
    QStringList filters;
    filters << "*.xml";  // ֻ���� XML �ļ�
    directory.setNameFilters(filters);

    // ��ȡ�ļ��б�
    QFileInfoList files = directory.entryInfoList(QDir::Files);

    // �������� XML �ļ�������ļ����� comboBox_Formula ��
    for (const QFileInfo& file : files) {
        QString fileName = file.fileName();
        fileName.chop(4);  // ȥ�� .xml ��׺
        ui.comboBox_Formula_3D->addItem(fileName);
    }

    // �������е�����
    for (auto config : m_allConfig)
    {
        if (!config.CloudName.isEmpty())
        {
            int index = ui.comboBox_3dCloud_2->findText(config.CloudName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_3dCloud_2->addItem(config.CloudName);
        }
    }

    // �������в�ͷ��
    for (auto config : m_allConfig)
    {
        if (!config.ProductName.isEmpty())
        {
            int index = ui.comboBox_ProductName_3D->findText(config.ProductName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_ProductName_3D->addItem(config.ProductName);
        }
    }
}

void qg_Formula::initRadioButton_Feeding()
{
}

//�汾����
void qg_Formula::initVersion()
{
#ifndef DEBUG_LHC
    ////�����Ӿ�ѡ��
    //ui.radioButton_Visual_Identity->hide();
    //ui.radioButton_takePhoto->hide();
    //ui.radioButton_Feeding->hide();
    //ui.radioButton_CircleMeasure->hide();
    //switch (LSM->m_version)
    //{
    //case ENUM_VERSION::LsPin:
    //    //ȡ����
    //    ui.radioButton_Feeding->show();
    //    break;
    //case ENUM_VERSION::EdPin:
    //    ui.radioButton_Feeding->show();
    //    ui.radioButton_Visual_Identity->show();
    //    ui.radioButton_takePhoto->show();
    //    ui.radioButton_CircleMeasure->show();
    //    break;
    //case ENUM_VERSION::BtPin:
    //    //����3D
    //    ui.radioButton_3D->hide();
    //    ui.radioButton_3Didentify->hide();
    //    break;
    //default:
    //    break;
    //}
#endif // !DEBUG_LHC
}

//����
void qg_Formula::initRadioButton_TakePhoto()
{
    //���������е����
#ifdef DEBUG_LHC
    ui.comboBox_cameraName->addItem(LS_CameraName::pin2D);
#endif // DEBUG_LHC

    for (const auto& camera : LSM->m_cameraName)
    {
        if (!camera.first.isEmpty())
        {
            int index = ui.comboBox_cameraName->findText(camera.first, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_cameraName->addItem(camera.first);
        }
        //ui.comboBox_cameraName->addItem(camera.first);
    }
    //������������Ƭ
    for (auto config : m_allConfig)
    {
        if (!config.PhotoName.isEmpty())
        {
            int index = ui.comboBox_photoName->findText(config.PhotoName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_photoName->addItem(config.PhotoName);
        }
    }
}

//�Ӿ�ģ��
void qg_Formula::initRadioButton_Visual_Identity()
{
    // ��ȡ��ǰ exe Ŀ¼·��
    QString exeDir = QCoreApplication::applicationDirPath();
    QString dirPath = "";
    switch (ui.comboBox_Visual_Identity->currentIndex())
    {
    case ENUM_VISION::TempMatch:
        dirPath = "/Template_formulation";
        break;
    case ENUM_VISION::OneDimensional:
        dirPath = "/BarCode_formulation";
        break;
    case ENUM_VISION::TwoDimensional:
        dirPath = "/QRCode_formulation";
        break;
    case ENUM_VISION::BlobModel:
        dirPath = "/Blob_formulation";
        break;
    case ENUM_VISION::CircleModel:
        dirPath = "/Circle_formulation";
        break;
    default:
        break;
    }

    // ƴ�ӵ� Vision_formulation �ļ���·��
    QDir directory(exeDir + "/Vision_formulation" + dirPath);

    // ����ļ����Ƿ����
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {
            QMessageBox::critical(this, tr("����"), tr("�޷�����Vision_formulation�ļ��У�"));
            return;
        }
    }

    // ��ȡ��Ŀ¼������ .xml �ļ�
    QStringList filters;
    filters << "*.xml";  // ֻ���� XML �ļ�
    directory.setNameFilters(filters);

    // ��ȡ�ļ��б�
    QFileInfoList files = directory.entryInfoList(QDir::Files);

    // �������� XML �ļ�������ļ����� comboBox_Formula ��
    for (const QFileInfo& file : files) {
        QString fileName = file.fileName();
        fileName.chop(4);  // ȥ�� .xml ��׺
        ui.comboBox_Formula_TempMatch->addItem(fileName);
    }

    // ����������Ƭ��
    for (auto config : m_allConfig)
    {
        if (!config.PhotoName.isEmpty())
        {
            int index = ui.comboBox_photoName_2->findText(config.PhotoName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_photoName_2->addItem(config.PhotoName);
        }
    }
    connect(ui.comboBox_Visual_Identity, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_Visual_Identity_currentIndexChanged(int)));
    //connect(ui.comboBox_Visual_Identity, &QComboBox::currentIndexChanged, this,&qg_Formula::on_comboBox_Visual_Identity_currentIndexChanged);
}

//�����Ӿ�ģ���Ӧ�䷽
void qg_Formula::on_comboBox_Visual_Identity_currentIndexChanged(int idx)
{
    // ��ȡ��ǰ exe Ŀ¼·��
    QString exeDir = QCoreApplication::applicationDirPath();
    QString dirPath = "";
    switch (idx)
    {
    case ENUM_VISION::TempMatch:
        dirPath = "/Template_formulation";
        break;
    case ENUM_VISION::OneDimensional:
        dirPath = "/BarCode_formulation";
        break;
    case ENUM_VISION::TwoDimensional:
        dirPath = "/QRCode_formulation";
        break;
    case ENUM_VISION::BlobModel:
        dirPath = "/Blob_formulation";
        break;
    case ENUM_VISION::CircleModel:
        dirPath = "/Circle_formulation";
        break;
    default:
        break;
    }

    // ƴ�ӵ� Vision_formulation �ļ���·��
    QDir directory(exeDir + "/Vision_formulation" + dirPath);

    // ����ļ����Ƿ����
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {
            QMessageBox::critical(this, tr("����"), tr("�޷�����Vision_formulation�ļ��У�"));
            return;
        }
    }

    // ��ȡ��Ŀ¼������ .xml �ļ�
    QStringList filters;
    filters << "*.xml";  // ֻ���� XML �ļ�
    directory.setNameFilters(filters);

    // ��ȡ�ļ��б�
    QFileInfoList files = directory.entryInfoList(QDir::Files);
    ui.comboBox_Formula_TempMatch->clear();
    // �������� XML �ļ�������ļ����� comboBox_Formula ��
    for (const QFileInfo& file : files) {
        QString fileName = file.fileName();
        fileName.chop(4);  // ȥ�� .xml ��׺
        ui.comboBox_Formula_TempMatch->addItem(fileName);
    }
}

//Բ���
void qg_Formula::initRadioButton_CircleMeasure()
{
    // ��ȡ��ǰ exe Ŀ¼·��
    QString exeDir = QCoreApplication::applicationDirPath();

    // ƴ�ӵ� Vision_formulation �ļ���·��
    QDir directory(exeDir + "/Vision_formulation/Circle_formulation");

    // ����ļ����Ƿ����
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {
            QMessageBox::critical(this, tr("����"), tr("�޷�����Circle_formulation�ļ��У�"));
            return;
        }
    }

    // ��ȡ��Ŀ¼������ .xml �ļ�
    QStringList filters;
    filters << "*.xml";  // ֻ���� XML �ļ�
    directory.setNameFilters(filters);

    // ��ȡ�ļ��б�
    QFileInfoList files = directory.entryInfoList(QDir::Files);

    // �������� XML �ļ�������ļ����� comboBox_Formula ��

    for (const QFileInfo& file : files) {
        QString fileName = file.fileName();
        fileName.chop(4);  // ȥ�� .xml ��׺
        int index = ui.comboBox_Formula_CircleOne->findText(fileName, Qt::MatchExactly);
            if (index == -1)
        ui.comboBox_Formula_CircleOne->addItem(fileName);

        index = ui.comboBox_Formula_CircleTwo->findText(fileName, Qt::MatchExactly);
        if (index == -1)
            ui.comboBox_Formula_CircleTwo->addItem(fileName);
    }

    //������������Ƭ
    for (auto config : m_allConfig)
    {
        if (!config.PhotoName.isEmpty())
        {
            int index = ui.comboBox_photoName_3->findText(config.PhotoName, Qt::MatchExactly);
            if (index == -1)
                ui.comboBox_photoName_3->addItem(config.PhotoName);
        }
    }
}