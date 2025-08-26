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
        //换点动控件
        QWidget* oldJogWidget = ui.widget_JogButton;
        //创建立昇对应的新的控件
        qg_JogButton_ED* newJogWidget = new qg_JogButton_ED(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::EdPin)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_JogButton;
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
        QWidget* oldJogWidget = ui.widget_JogButton;
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
        QWidget* oldJogWidget = ui.widget_JogButton;
        //创建易鼎丰对应的新的控件
        qg_JogButton_JMPin* newJogWidget = new qg_JogButton_JMPin(this);
        //替换布局中的控件
        QLayout* layout = oldJogWidget->parentWidget()->layout();
        layout->replaceWidget(oldJogWidget, newJogWidget);
        delete oldJogWidget;
    }
    else if (LSM->m_version == ENUM_VERSION::JmHan)
    {
        //换点动控件
        QWidget* oldJogWidget = ui.widget_JogButton;
        //创建金脉焊缝对应的新的控件
        qg_JogButton_JMHan* newJogWidget = new qg_JogButton_JMHan(this);
        //替换布局中的控件
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

//查找设备
void qg_2dCameraSetting::on_pushButton_FindDevices_clicked()
{
    QString strMsg;

    ui.comboBox_CameraID->clear();  // 清空 ComboBox 内容
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举子网内所有设备
    int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if (MV_OK != nRet)
    {
        return;
    }

    // 遍历所有设备并显示
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
            QMessageBox::critical(this, "Error", tr("枚举的未知设备"));
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
        QMessageBox::information(this, tr("无设备"), tr("没有找到设备"));
        return;
    }

    ui.comboBox_CameraID->setCurrentIndex(0);
}

// ch:按下打开设备按钮：打开设备 | en:Click Open button: Open Device
void qg_2dCameraSetting::on_pushButton_OpenDevices_clicked()
{
    // 如果设备已经打开或设备实例存在，直接返回
    if (m_bOpenDevice || m_pcMyCamera != nullptr)
    {
        return;
    }

    // 更新数据（如果需要）
    // (在Qt中，如果使用了QLineEdit、QComboBox等控件，可以通过模型/视图方式进行更新)
    // 比如 QComboBox 的当前选中项的获取方式为 currentIndex()

    int nIndex = ui.comboBox_CameraID->currentIndex();  // 设备选择的索引
    if (nIndex < 0 || nIndex >= MV_MAX_DEVICE_NUM)
    {
        // 错误提示：请选择设备
        QMessageBox::warning(this, tr("错误"), tr("请选择设备"));
        return;
    }

    // 如果设备信息为空，显示错误信息
    if (m_stDevList.pDeviceInfo[nIndex] == nullptr)
    {
        QMessageBox::warning(this, tr("错误"), tr("设备不存在"));
        return;
    }

    // 创建设备实例
    m_pcMyCamera = std::make_unique<CMvCamera>();
    if (m_pcMyCamera == nullptr)
    {
        return;
    }

    // 打开设备
    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
    if (nRet != MV_OK)
    {
        m_pcMyCamera.reset();
        QMessageBox::critical(this, tr("错误"), tr("打开设备失败"));
        return;
    }

    // 只对GigE设备有效，获取网络的最佳包大小
    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        unsigned int nPacketSize = 0;
        nRet = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK)
        {
            nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                QMessageBox::warning(this, tr("警告"), tr("设置数据包大小失败！"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("警告"), tr("获取数据包大小失败！"));
        }
    }

    // 标记设备已打开
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

//关闭设备按钮
void qg_2dCameraSetting::on_pushButton_CloseDevices_clicked()
{
    CloseDevice();
}

//关闭设备
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

//开始采集
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
        QMessageBox::warning(this, tr("警告"), tr("开始采集失败！"));
        return;
    }


    //开启采集线程
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

//视图刷新
void qg_2dCameraSetting::slotFlushed()
{
    if (m_pcMyCamera == nullptr || m_pcMyCamera->m_pSaveImageBuf == nullptr)
        return;

    bool isMono = m_pcMyCamera->isMono(m_pcMyCamera->m_stImageInfo.enPixelType);

    QImage image = m_pcMyCamera->convertToImage(m_pcMyCamera->m_pSaveImageBuf, &m_pcMyCamera->m_stImageInfo, isMono);

    if (image.isNull())
    {
        machineLog->write("采集图像失败,停止采集", Normal);
        QMessageBox::warning(this, tr("警告"), tr("采集图像失败,停止采集！"));
        on_pushButton_StopGrabbing_clicked();
        return;
    }


    ui.lbl_Window->DispImage(image);

    //QPixmap pixmap = QPixmap::fromImage(image);
    //// 获取 lbl_Window 的可用尺寸
    //QSize labelSize = ui.lbl_Window->maximumSize();

    //// 如果 maximumSize 未设置，使用当前实际大小
    //if (labelSize.width() == QWIDGETSIZE_MAX || labelSize.height() == QWIDGETSIZE_MAX) {
    //    labelSize = ui.lbl_Window->size();
    //}

    //// 缩放图片（保持宽高比）
    //QPixmap scaledPixmap = pixmap.scaled(
    //    labelSize,
    //    Qt::KeepAspectRatio,
    //    Qt::SmoothTransformation
    //);
    //// 将 QPixmap 显示到 QLabel 中
    //ui.lbl_Window->setPixmap(pixmap);

    //**********************************

    //// 将 pData 转换为 Halcon 图像
    //HObject halconImage = m_pcMyCamera->convertToHalconImage(m_pcMyCamera->m_pSaveImageBuf, m_pcMyCamera->m_stImageInfo.nWidth, m_pcMyCamera->m_stImageInfo.nHeight, isMono);
    //
    //// 假设 hHalconID 是 Halcon 窗口的 ID，m_label 是 QLabel 对象
    //HObject m_currentObj;
    //ui.lbl_Window->setHalconWnd(halconImage, m_currentObj, true, m_hHalconID, ui.label_3);

    //// 设置窗口显示区域（调整为与 QLabel 大小一致）
    //SetWindowExtents(m_hHalconID, 0, 0, ui.lbl_Window->width(), ui.lbl_Window->height());

    //// 设置图像的显示区域
    ////SetPart(m_hHalconID, 0, 0, halconImage.Height() - 1, m_currentImg.Width() - 1);

    //// 在窗口中显示图像
    //AttachBackgroundToWindow(halconImage, m_hHalconID);
    //ui.lbl_Window->update();

}


//停止采集
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

//相机参数读取
void qg_2dCameraSetting::on_pushButton_ReadParamer_clicked()
{
    if (m_pcMyCamera == nullptr || FALSE == m_bOpenDevice)
        return;
    //曝光
    double exposureTime = 0;
    int nRet = m_pcMyCamera->GetExposureTime(exposureTime);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_Exposure->setText(QString::number(exposureTime));
    //增益
    double gain = 0;
    nRet = m_pcMyCamera->GetGain(gain);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_Gain->setText(QString::number(gain));
    //帧率
    double frameRate = 0;
    nRet = m_pcMyCamera->GetFrameRate(frameRate);
    if (MV_OK != nRet)
    {
        return;
    }
    ui.lineEdit_FrameRate->setText(QString::number(frameRate));
}

//相机参数写入
void qg_2dCameraSetting::on_pushButton_WriteParamer_clicked()
{
    if (m_pcMyCamera == nullptr || FALSE == m_bOpenDevice)
        return;
    if (m_bStartGrabbing)
    {
        QMessageBox::warning(this, tr("警告"), tr("请先停止采集！"));
        return;
    }

    //曝光
    m_pcMyCamera->SetExposureTime(ui.lineEdit_Exposure->text().toDouble());
    //增益
    m_pcMyCamera->SetGain(ui.lineEdit_Gain->text().toDouble());
    //帧率
    m_pcMyCamera->SetFrameRate(ui.lineEdit_FrameRate->text().toDouble());
}

//3D相机开始采集
void qg_2dCameraSetting::on_pushButton_StartGrabbing_3D_clicked()
{
    machineLog->write("- - - ->>>>>>>>开始执行，初始化中......", Normal);
    ui.pushButton_StartGrabbing_3D->setEnabled(false);
    if (m_pcMyCamera_3D == nullptr)
    {
        // 创建设备实例
        m_pcMyCamera_3D = std::make_unique<Camera3D>(false);
        if (m_pcMyCamera_3D == nullptr)
        {
            return;
        }
        connect(m_pcMyCamera_3D.get(), &Camera3D::signalFlushed_3D, this, &qg_2dCameraSetting::slotFlushed_3D);
    }
    m_pcMyCamera_3D->run();
}

//3d相机停止采集
void qg_2dCameraSetting::on_pushButton_StopGrabbing_3D_clicked()
{
    if (m_pcMyCamera_3D)
    {
        m_pcMyCamera_3D->StopSubThread(true);
        m_pcMyCamera_3D.reset();
    }
    ui.pushButton_StartGrabbing_3D->setEnabled(true);
}

//保存2D图片
void qg_2dCameraSetting::on_pushButton_SaveImg_clicked()
{
    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        tr("Save File"),
        "",
        tr("BMP Files (*.bmp)")
    );

    // 如果用户没有选择文件，或者点击了取消
    if (filePath.isEmpty()) {
        return;
    }

    // 确保文件扩展名是 .bmp
    if (!filePath.endsWith(".bmp", Qt::CaseInsensitive)) {
        filePath += ".bmp";
    }
    if(m_pcMyCamera)
        m_pcMyCamera->SaveImage_Action(filePath);
}


//显示3D图片
void qg_2dCameraSetting::slotFlushed_3D()
{
    if (m_pcMyCamera_3D == nullptr)
        return;

    QImage image = m_pcMyCamera_3D->matToQImage(m_pcMyCamera_3D->showImg);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);
    //ui.lbl_Window->setPixmap(pixmap);
}

//确定当前相机为2D相机
void qg_2dCameraSetting::on_pushButton_sure2D_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "是否确定此相机为2D识别工位？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }
    LSM->m_cameraName[LS_CameraName::pin2D] = m_cameraName;
    LSM->saveMotionConfig();
}

//确定当前相机为上相机
void qg_2dCameraSetting::on_pushButton_sureED_Up_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "是否确定此相机为上识别工位？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Up = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Up] = m_cameraName;
    LSM->saveMotionConfig();
}

//确定当前相机为下相机
void qg_2dCameraSetting::on_pushButton_sureED_Down_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "是否确定此相机为下识别工位？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Down = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Down] = m_cameraName;
    LSM->saveMotionConfig();
}

//确认当前相机为定位相机
void qg_2dCameraSetting::on_pushButton_sureED_Location_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "是否确定此相机为定位工位？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (reply == QMessageBox::No) {
        return;
    }

    //LSM->m_cameraName.ed_Down = m_cameraName;
    LSM->m_cameraName[LS_CameraName::ed_Location] = m_cameraName;
    LSM->saveMotionConfig();
}

//确定当前相机为侧相机
void qg_2dCameraSetting::on_pushButton_sideCamera_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "是否确定此相机为侧识别工位？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    LSM->m_cameraName[LS_CameraName::side_camera] = m_cameraName;
    LSM->saveMotionConfig();
}