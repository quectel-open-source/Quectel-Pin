#include "qg_2dCameraSetting.h"
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qfiledialog.h>
#include "qg_JogButton_ED.h"
#include "qg_JogButton_ED_New.h"
#include "qc_log.h"
#include "qg_JogButton_BT.h"
#include "qg_JogButton_JMPin.h"
#include "qg_JogButton_JMHan.h"

#pragma execution_character_set("utf-8")
//#include "Pin2.h"
qg_2dCameraSetting* qg_2dCameraSetting::cameraWindow = NULL;

qg_2dCameraSetting::qg_2dCameraSetting(QWidget *parent)
	: QDialog(parent)
{
    cameraWindow = this;
	ui.setupUi(this);

    //ui.lbl_Window->setScaledContents(true);
    if (LSM->m_version == ENUM_VERSION::LsPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_JogButton;
        //�������N��Ӧ���µĿؼ�
        qg_JogButton_ED* newJogWidget = new qg_JogButton_ED(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::EdPin)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_JogButton;
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
        QWidget* oldJogWidget = ui.widget_JogButton;
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
        QWidget* oldJogWidget = ui.widget_JogButton;
        //�����׶����Ӧ���µĿؼ�
        qg_JogButton_JMPin* newJogWidget = new qg_JogButton_JMPin(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmHan)
    {
        //���㶯�ؼ�
        QWidget* oldJogWidget = ui.widget_JogButton;
        //�������������Ӧ���µĿؼ�
        qg_JogButton_JMHan* newJogWidget = new qg_JogButton_JMHan(this);
        //�滻�����еĿؼ�
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    
    ui.pushButton_sureED_Up->hide();
    ui.pushButton_sureED_Down->hide();
    ui.pushButton_sureED_Location->hide();
    ui.pushButton_sideCamera->hide();
    switch (LSM->m_version)
    {
    case EdPin:
        ui.pushButton_sureED_Up->show();
        ui.pushButton_sureED_Down->show();
        ui.pushButton_sureED_Location->show();
        break;
    case JmPin:
        ui.pushButton_sideCamera->show();
        break;
    default:
        break;
    }


    ui.groupBox_CameraTest3D->hide();
}

qg_2dCameraSetting::~qg_2dCameraSetting()
{
    CloseDevice();
}

//�����豸
void qg_2dCameraSetting::on_pushButton_FindDevices_clicked()
{
    QString strMsg;

    ui.comboBox_CameraID->clear();  // ��� ComboBox ����
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // ö�������������豸
    int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if (MV_OK != nRet)
    {
        return;
    }

    // ���������豸����ʾ
    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (NULL == pDeviceInfo)
        {
            continue;
        }

        wchar_t* pUserName = NULL;
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            int nIp1 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            int nIp2 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            int nIp3 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            int nIp4 = (pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
            if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                //m_cameraName = (char*)pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256] = { 0 };
                sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
                    pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
                    pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                //m_cameraName = (char*)pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }

            strMsg = QString("[%1]GigE: %2 (%3.%4.%5.%6)").arg(i).arg(QString::fromWCharArray(pUserName))
                .arg(nIp1).arg(nIp2).arg(nIp3).arg(nIp4);
        }
        else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
        {
            if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256] = { 0 };
                sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
                    pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
                    pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }

            strMsg = QString("[%1]UsbV3: %2").arg(i).arg(QString::fromWCharArray(pUserName));
        }
        else
        {
            QMessageBox::critical(this, "Error", tr("ö�ٵ�δ֪�豸"));
            continue;
        }

        ui.comboBox_CameraID->addItem(strMsg);

        if (pUserName)
        {
            delete[] pUserName;
            pUserName = NULL;
        }
    }

    if (m_stDevList.nDeviceNum == 0)
    {
        QMessageBox::information(this, tr("���豸"), tr("û���ҵ��豸"));
        return;
    }

    ui.comboBox_CameraID->setCurrentIndex(0);
}

// ch:���´��豸��ť�����豸 | en:Click Open button: Open Device
void qg_2dCameraSetting::on_pushButton_OpenDevices_clicked()
{
    // ����豸�Ѿ��򿪻��豸ʵ�����ڣ�ֱ�ӷ���
    if (m_bOpenDevice || m_pcMyCamera != nullptr)
    {
        return;
    }

    // �������ݣ������Ҫ��
    // (��Qt�У����ʹ����QLineEdit��QComboBox�ȿؼ�������ͨ��ģ��/��ͼ��ʽ���и���)
    // ���� QComboBox �ĵ�ǰѡ����Ļ�ȡ��ʽΪ currentIndex()

    int nIndex = ui.comboBox_CameraID->currentIndex();  // �豸ѡ�������
    if (nIndex < 0 || nIndex >= MV_MAX_DEVICE_NUM)
    {
        // ������ʾ����ѡ���豸
        QMessageBox::warning(this, tr("����"), tr("��ѡ���豸"));
        return;
    }

    // ����豸��ϢΪ�գ���ʾ������Ϣ
    if (m_stDevList.pDeviceInfo[nIndex] == nullptr)
    {
        QMessageBox::warning(this, tr("����"), tr("�豸������"));
        return;
    }

    // �����豸ʵ��
    m_pcMyCamera = std::make_unique<CMvCamera>();
    if (m_pcMyCamera == nullptr)
    {
        return;
    }

    // ���豸
    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
    if (nRet != MV_OK)
    {
        m_pcMyCamera.reset();
        QMessageBox::critical(this, tr("����"), tr("���豸ʧ��"));
        return;
    }

    // ֻ��GigE�豸��Ч����ȡ�������Ѱ���С
    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        unsigned int nPacketSize = 0;
        nRet = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK)
        {
            nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                QMessageBox::warning(this, tr("����"), tr("�������ݰ���Сʧ�ܣ�"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("����"), tr("��ȡ���ݰ���Сʧ�ܣ�"));
        }
    }

    // ����豸�Ѵ�
    m_bOpenDevice = true;

    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        m_cameraName = (char*)m_stDevList.pDeviceInfo[nIndex]->SpecialInfo.stGigEInfo.chSerialNumber;
    }
    else if(m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_USB_DEVICE)
    {
        m_cameraName = (char*)m_stDevList.pDeviceInfo[nIndex]->SpecialInfo.stUsb3VInfo.chSerialNumber;
    }
    machineLog->write("m_cameraName = " + QString::fromStdString(m_cameraName), Normal);
}

//�ر��豸��ť
void qg_2dCameraSetting::on_pushButton_CloseDevices_clicked()
{
    CloseDevice();
}

//�ر��豸
int qg_2dCameraSetting::CloseDevice()
{
    if (m_hGrabObject)
    {
        m_hGrabObject->bExit = true;
        m_hGrabObject->deleteLater();
        m_hGrabObject = nullptr;
    }

    if (m_hGrabThread)
    {
        m_hGrabThread->quit();
        m_hGrabThread->wait();
        m_hGrabThread->deleteLater();
        m_hGrabThread = nullptr;
    }
    if (m_pcMyCamera)
    {
        if (m_pcMyCamera->m_pSaveImageBuf)
        {
            free(m_pcMyCamera->m_pSaveImageBuf);
            m_pcMyCamera->m_pSaveImageBuf = NULL;
        }
        m_pcMyCamera->m_nSaveImageBufSize = 0;

        m_pcMyCamera->Close();
        m_pcMyCamera.reset();
    }

    m_bStartGrabbing = FALSE;
    m_bOpenDevice = FALSE;

    return MV_OK;
}

//��ʼ�ɼ�
void qg_2dCameraSetting::on_pushButton_StartGrabbing_clicked()
{
    if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing || nullptr == m_pcMyCamera)
    {
        return;
    }

    int nRet = m_pcMyCamera->StartGrabbing();
    if (MV_OK != nRet)
    {
        m_hGrabObject->bExit = true;
        QMessageBox::warning(this, tr("����"), tr("��ʼ�ɼ�ʧ�ܣ�"));
        return;
    }


    //�����ɼ��߳�
    if (m_hGrabObject == nullptr)
    {
        m_hGrabObject = new Thread();
    }
    if (m_hGrabThread == nullptr)
    {
        m_hGrabThread = new QThread();
        m_hGrabObject->moveToThread(m_hGrabThread);
    }
    if (!m_hGrabThread->isRunning())
    {
        m_hGrabObject->bExit = false;
        m_hGrabThread->start();
        QMetaObject::invokeMethod(m_hGrabObject, &Thread::cameraGrabbing, Qt::QueuedConnection);
        connect(m_hGrabObject, &Thread::signalFlushed, this, &qg_2dCameraSetting::slotFlushed);
    }


    m_bStartGrabbing = TRUE;
}

//��ͼˢ��
void qg_2dCameraSetting::slotFlushed()
{
    if (m_pcMyCamera == nullptr || m_pcMyCamera->m_pSaveImageBuf == nullptr)
        return;

    bool isMono = m_pcMyCamera->isMono(m_pcMyCamera->m_stImageInfo.enPixelType);

    QImage image = m_pcMyCamera->convertToImage(m_pcMyCamera->m_pSaveImageBuf, &m_pcMyCamera->m_stImageInfo, isMono);

    if (image.isNull())
    {
        machineLog->write("�ɼ�ͼ��ʧ��,ֹͣ�ɼ�", Normal);
        QMessageBox::warning(this, tr("����"), tr("�ɼ�ͼ��ʧ��,ֹͣ�ɼ���"));
        on_pushButton_StopGrabbing_clicked();
        return;
    }


    ui.lbl_Window->DispImage(image);

    //QPixmap pixmap = QPixmap::fromImage(image);
    //// ��ȡ lbl_Window �Ŀ��óߴ�
    //QSize labelSize = ui.lbl_Window->maximumSize();

    //// ��� maximumSize δ���ã�ʹ�õ�ǰʵ�ʴ�С
    //if (labelSize.width() == QWIDGETSIZE_MAX || labelSize.height() == QWIDGETSIZE_MAX) {
    //    labelSize = ui.lbl_Window->size();
    //}

    //// ����ͼƬ�����ֿ�߱ȣ�
    //QPixmap scaledPixmap = pixmap.scaled(
    //    labelSize,
    //    Qt::KeepAspectRatio,
    //    Qt::SmoothTransformation
    //);
    //// �� QPixmap ��ʾ�� QLabel ��
    //ui.lbl_Window->setPixmap(pixmap);

    //**********************************

    //// �� pData ת��Ϊ Halcon ͼ��
    //HObject halconImage = m_pcMyCamera->convertToHalconImage(m_pcMyCamera->m_pSaveImageBuf, m_pcMyCamera->m_stImageInfo.nWidth, m_pcMyCamera->m_stImageInfo.nHeight, isMono);
    //
    //// ���� hHalconID �� Halcon ���ڵ� ID��m_label �� QLabel ����
    //HObject m_currentObj;
    //ui.lbl_Window->setHalconWnd(halconImage, m_currentObj, true, m_hHalconID, ui.label_3);

    //// ���ô�����ʾ���򣨵���Ϊ�� QLabel ��Сһ�£�
    //SetWindowExtents(m_hHalconID, 0, 0, ui.lbl_Window->width(), ui.lbl_Window->height());

    //// ����ͼ�����ʾ����
    ////SetPart(m_hHalconID, 0, 0, halconImage.Height() - 1, m_currentImg.Width() - 1);

    //// �ڴ�������ʾͼ��
    //AttachBackgroundToWindow(halconImage, m_hHalconID);
    //ui.lbl_Window->update();

}


//ֹͣ�ɼ�
void qg_2dCameraSetting::on_pushButton_StopGrabbing_clicked()
{
    if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing || nullptr == m_pcMyCamera)
    {
        return;
    }

    if (m_hGrabObject)
    {
        m_hGrabObject->bExit = true;
    }

    if (m_hGrabThread)
    {
        m_hGrabThread->quit();
        m_hGrabThread->wait();
    }

    int nRet = m_pcMyCamera->StopGrabbing();
    if (MV_OK != nRet)
    {
        return;
    }
    m_bStartGrabbing = FALSE;
}

//���������ȡ
void qg_2dCameraSetting::on_pushButton_ReadParamer_clicked()
{
    if (m_pcMyCamera == nullptr || FALSE == m_bOpenDevice)
        return;
    //�ع�
    double exposureTime = 0;
    int nRet = m_pcMyCamera->GetExposureTime(exposureTime);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_Exposure->setText(QString::number(exposureTime));
    //����
    double gain = 0;
    nRet = m_pcMyCamera->GetGain(gain);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_Gain->setText(QString::number(gain));
    //֡��
    double frameRate = 0;
    nRet = m_pcMyCamera->GetFrameRate(frameRate);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_FrameRate->setText(QString::number(frameRate));
}

//�������д��
void qg_2dCameraSetting::on_pushButton_WriteParamer_clicked()
{
    if (m_pcMyCamera == nullptr || FALSE == m_bOpenDevice)
        return;
    if (m_bStartGrabbing)
    {
        QMessageBox::warning(this, tr("����"), tr("����ֹͣ�ɼ���"));
        return;
    }

    //�ع�
    m_pcMyCamera->SetExposureTime(ui.lineEdit_Exposure->text().toDouble());
    //����
    m_pcMyCamera->SetGain(ui.lineEdit_Gain->text().toDouble());
    //֡��
    m_pcMyCamera->SetFrameRate(ui.lineEdit_FrameRate->text().toDouble());
}

//3D�����ʼ�ɼ�
void qg_2dCameraSetting::on_pushButton_StartGrabbing_3D_clicked()
{
    machineLog->write("- - - ->>>>>>>>��ʼִ�У���ʼ����......", Normal);
    ui.pushButton_StartGrabbing_3D->setEnabled(false);
    if (m_pcMyCamera_3D == nullptr)
    {
        // �����豸ʵ��
        m_pcMyCamera_3D = std::make_unique<Camera3D>(false);
        if (m_pcMyCamera_3D == nullptr)
        {
            return;
        }
        connect(m_pcMyCamera_3D.get(), &Camera3D::signalFlushed_3D, this, &qg_2dCameraSetting::slotFlushed_3D);
    }
    m_pcMyCamera_3D->run();
}

//3d���ֹͣ�ɼ�
void qg_2dCameraSetting::on_pushButton_StopGrabbing_3D_clicked()
{
    if (m_pcMyCamera_3D)
    {
        m_pcMyCamera_3D->StopSubThread(true);
        m_pcMyCamera_3D.reset();
    }
    ui.pushButton_StartGrabbing_3D->setEnabled(true);
}

//����2DͼƬ
void qg_2dCameraSetting::on_pushButton_SaveImg_clicked()
{
    // ���ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        tr("Save File"),
        "",
        tr("BMP Files (*.bmp)")
    );

    // ����û�û��ѡ���ļ������ߵ����ȡ��
    if (filePath.isEmpty()) {
        return;
    }

    // ȷ���ļ���չ���� .bmp
    if (!filePath.endsWith(".bmp", Qt::CaseInsensitive)) {
        filePath += ".bmp";
    }
    if(m_pcMyCamera)
        m_pcMyCamera->SaveImage_Action(filePath);
}


//��ʾ3DͼƬ
void qg_2dCameraSetting::slotFlushed_3D()
{
    if (m_pcMyCamera_3D == nullptr)
        return;

    QImage image = m_pcMyCamera_3D->matToQImage(m_pcMyCamera_3D->showImg);

    // �� QImage ת��Ϊ QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);
    //ui.lbl_Window->setPixmap(pixmap);
}

//ȷ����ǰ���Ϊ2D���
void qg_2dCameraSetting::on_pushButton_sure2D_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "�Ƿ�ȷ�������Ϊ2Dʶ��λ��",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }
    LSM->m_cameraName[LS_CameraName::pin2D] = m_cameraName;
    LSM->saveMotionConfig();
}

//ȷ����ǰ���Ϊ�����
void qg_2dCameraSetting::on_pushButton_sureED_Up_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "�Ƿ�ȷ�������Ϊ��ʶ��λ��",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Up = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Up] = m_cameraName;
    LSM->saveMotionConfig();
}

//ȷ����ǰ���Ϊ�����
void qg_2dCameraSetting::on_pushButton_sureED_Down_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "�Ƿ�ȷ�������Ϊ��ʶ��λ��",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Down = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Down] = m_cameraName;
    LSM->saveMotionConfig();
}

//ȷ�ϵ�ǰ���Ϊ��λ���
void qg_2dCameraSetting::on_pushButton_sureED_Location_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "�Ƿ�ȷ�������Ϊ��λ��λ��",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Down = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Location] = m_cameraName;
    LSM->saveMotionConfig();
}

//ȷ����ǰ���Ϊ�����
void qg_2dCameraSetting::on_pushButton_sideCamera_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "�Ƿ�ȷ�������Ϊ��ʶ��λ��",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    LSM->m_cameraName[LS_CameraName::side_camera] = m_cameraName;
    LSM->saveMotionConfig();
}