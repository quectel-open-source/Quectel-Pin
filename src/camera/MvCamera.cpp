#include "MvCamera.h"
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <opencv2/opencv.hpp>

CMvCamera::CMvCamera()
{
    m_hDevHandle = MV_NULL;
}

CMvCamera::~CMvCamera()
{
    //if (m_hDevHandle)
    //{
    //    MV_CC_DestroyHandle(m_hDevHandle);
    //    m_hDevHandle    = MV_NULL;
    //}

    Close();
}

// ch:��ȡSDK�汾�� | en:Get SDK Version
int CMvCamera::GetSDKVersion()
{
    return MV_CC_GetSDKVersion();
}

// ch:ö���豸 | en:Enumerate Device
int CMvCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
    return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

// ch:�ж��豸�Ƿ�ɴ� | en:Is the device accessible
bool CMvCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
    return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}

// ch:���豸 | en:Open Device
int CMvCamera::Open(MV_CC_DEVICE_INFO* pstDeviceInfo)
{
    if (MV_NULL == pstDeviceInfo)
    {
        return MV_E_PARAMETER;
    }

    if (m_hDevHandle)
    {
        return MV_E_CALLORDER;
    }
    int nRet = MV_OK;

    //int  nRet = MV_CC_IsDeviceConnected(m_hDevHandle);
    //if (nRet == 1)
    //{
    //    nRet = MV_CC_CloseDevice(m_hDevHandle);
    //    if (MV_OK != nRet)
    //    {
    //        printf("ClosDevice fail! nRet [0x%x]\n", nRet);
    //    }
    //}

    nRet  = MV_CC_CreateHandle(&m_hDevHandle, pstDeviceInfo);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    nRet = MV_CC_OpenDevice(m_hDevHandle);
    if (MV_OK != nRet)
    {
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle = MV_NULL;
    }

    //��������ʱ��
    int HeartBeatTimeout = 3000;
    nRet = MV_CC_SetHeartBeatTimeout(m_hDevHandle, HeartBeatTimeout);

    return nRet;
}

// ch:�ر��豸 | en:Close Device
int CMvCamera::Close()
{
    if (MV_NULL == m_hDevHandle)
    {
        return MV_E_HANDLE;
    }

    MV_CC_CloseDevice(m_hDevHandle);

    int nRet = MV_CC_DestroyHandle(m_hDevHandle);
    m_hDevHandle = MV_NULL;

    return nRet;
}

// ch:�ж�����Ƿ�������״̬ | en:Is The Device Connected
bool CMvCamera::IsDeviceConnected()
{
    return MV_CC_IsDeviceConnected(m_hDevHandle);
}

// ch:ע��ͼ�����ݻص� | en:Register Image Data CallBack
int CMvCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterImageCallBackEx(m_hDevHandle, cbOutput, pUser);
}

// ch:����ץͼ | en:Start Grabbing
int CMvCamera::StartGrabbing()
{
    return MV_CC_StartGrabbing(m_hDevHandle);
}

// ch:ֹͣץͼ | en:Stop Grabbing
int CMvCamera::StopGrabbing()
{
    return MV_CC_StopGrabbing(m_hDevHandle);
}

// ch:������ȡһ֡ͼ������ | en:Get one frame initiatively
int CMvCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
    return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

// ch:�ͷ�ͼ�񻺴� | en:Free image buffer
int CMvCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
    return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

// ch:������ʾ���ھ�� | en:Set Display Window Handle
int CMvCamera::DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo)
{
    return MV_CC_DisplayOneFrame(m_hDevHandle, pDisplayInfo);
}

// ch:����SDK�ڲ�ͼ�񻺴�ڵ���� | en:Set the number of the internal image cache nodes in SDK
int CMvCamera::SetImageNodeNum(unsigned int nNum)
{
    return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

// ch:��ȡ�豸��Ϣ | en:Get device information
int CMvCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
    return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

// ch:��ȡGEV�����ͳ����Ϣ | en:Get detect info of GEV camera
int CMvCamera::GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect)
{
    if (MV_NULL == pMatchInfoNetDetect)
    {
        return MV_E_PARAMETER;
    }

    MV_CC_DEVICE_INFO stDevInfo = {0};
    GetDeviceInfo(&stDevInfo);
    if (stDevInfo.nTLayerType != MV_GIGE_DEVICE)
    {
        return MV_E_SUPPORT;
    }

    MV_ALL_MATCH_INFO struMatchInfo = {0};

    struMatchInfo.nType = MV_MATCH_TYPE_NET_DETECT;
    struMatchInfo.pInfo = pMatchInfoNetDetect;
    struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_NET_DETECT);
    memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_NET_DETECT));

    return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// ch:��ȡU3V�����ͳ����Ϣ | en:Get detect info of U3V camera
int CMvCamera::GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect)
{
    if (MV_NULL == pMatchInfoUSBDetect)
    {
        return MV_E_PARAMETER;
    }

    MV_CC_DEVICE_INFO stDevInfo = {0};
    GetDeviceInfo(&stDevInfo);
    if (stDevInfo.nTLayerType != MV_USB_DEVICE)
    {
        return MV_E_SUPPORT;
    }

    MV_ALL_MATCH_INFO struMatchInfo = {0};

    struMatchInfo.nType = MV_MATCH_TYPE_USB_DETECT;
    struMatchInfo.pInfo = pMatchInfoUSBDetect;
    struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_USB_DETECT);
    memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_USB_DETECT));
    
    return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// ch:��ȡ������Int�Ͳ������� Width��Height����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
// en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue)
{
    return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int CMvCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
    return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

// ch:��ȡ������Enum�Ͳ������� PixelFormat����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
// en:Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue)
{
    return MV_CC_GetEnumValue(m_hDevHandle, strKey, pEnumValue);
}

int CMvCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
    return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int CMvCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
    return MV_CC_SetEnumValueByString(m_hDevHandle, strKey, sValue);
}

// ch:��ȡ������Float�Ͳ������� ExposureTime��Gain����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
// en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue)
{
    return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int CMvCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
    return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

// ch:��ȡ������Bool�Ͳ������� ReverseX����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
// en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetBoolValue(IN const char* strKey, OUT bool *pbValue)
{
    return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int CMvCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
    return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

// ch:��ȡ������String�Ͳ������� DeviceUserID����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�UserSetSave
// en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue)
{
    return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int CMvCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
    return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

// ch:ִ��һ��Command������� UserSetSave����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
// en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::CommandExecute(IN const char* strKey)
{
    return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

// ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
int CMvCamera::GetOptimalPacketSize(unsigned int* pOptimalPacketSize)
{
    if (MV_NULL == pOptimalPacketSize)
    {
        return MV_E_PARAMETER;
    }

    int nRet = MV_CC_GetOptimalPacketSize(m_hDevHandle);
    if (nRet < MV_OK)
    {
        return nRet;
    }

    *pOptimalPacketSize = (unsigned int)nRet;

    return MV_OK;
}

// ch:ע����Ϣ�쳣�ص� | en:Register Message Exception CallBack
int CMvCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),void* pUser)
{
    return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

// ch:ע�ᵥ���¼��ص� | en:Register Event CallBack
int CMvCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
}

// ch:ǿ��IP | en:Force IP
int CMvCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
    return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

// ch:����IP��ʽ | en:IP configuration method
int CMvCamera::SetIpConfig(unsigned int nType)
{
    return MV_GIGE_SetIpConfig(m_hDevHandle, nType);
}

// ch:�������紫��ģʽ | en:Set Net Transfer Mode
int CMvCamera::SetNetTransMode(unsigned int nType)
{
    return MV_GIGE_SetNetTransMode(m_hDevHandle, nType);
}

// ch:���ظ�ʽת�� | en:Pixel format conversion
int CMvCamera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam)
{
    return MV_CC_ConvertPixelType(m_hDevHandle, pstCvtParam);
}

// ch:����ͼƬ | en:save image
int CMvCamera::SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam)
{
    return MV_CC_SaveImageEx2(m_hDevHandle, pstParam);
}

// ch:����ͼƬΪ�ļ� | en:Save the image as a file
int CMvCamera::SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam)
{
    return MV_CC_SaveImageToFile(m_hDevHandle, pstSaveFileParam);
}

//�ж��Ƿ�ͨ��
bool CMvCamera::isMono(MvGvspPixelType enType)
{
    switch (enType)
    {
    case PixelType_Gvsp_Mono8:
    case PixelType_Gvsp_Mono10:
    case PixelType_Gvsp_Mono10_Packed:
    case PixelType_Gvsp_Mono12:
    case PixelType_Gvsp_Mono12_Packed:
        return true;
    default:
        return false;
    }
}

//�Ѻ�����������ת�� HObject
HObject CMvCamera::convertToHalconImage(unsigned char* pData, int width, int height, bool isMono)
{
    HObject hImage;
    try {
        if (isMono) {
            // ����ǻҶ�ͼ��ʹ�� GenImage1
            GenImage1(&hImage, "byte", width, height, (Hlong)pData);
        }
        else {
            // ����ǲ�ɫͼ��ʹ�� GenImageInterleaved
            // ʹ�� GenImageInterleaved ���� RGB ͼ��
            GenImageInterleaved(&hImage, (Hlong)pData, "rgb", width, height, 0, "byte", width * 3, height, 0, 0, -1, 0);
        }
    }
    catch (HalconCpp::HException& e) {
        throw std::runtime_error("convertToHalconImage Halcon error: " + std::string(e.ErrorMessage()));
    }
    return hImage;
}


// ch:��ȡ�ع�ʱ�� | en:Get Exposure Time
int CMvCamera::GetExposureTime(double& time)
{
    MVCC_FLOATVALUE stFloatValue = { 0 };

    int nRet = GetFloatValue("ExposureTime", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    time = stFloatValue.fCurValue;

    return MV_OK;
}

// ch:�����ع�ʱ�� | en:Set Exposure Time
int CMvCamera::SetExposureTime(double time)
{
    // ch:�����������ع�ģʽ���������ع�ʱ����Ч
    // en:Adjust these two exposure mode to allow exposure time effective
    int nRet = SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

    return SetFloatValue("ExposureTime", (float)time);
}

// ch:��ȡ���� | en:Get Gain
int CMvCamera::GetGain(double& gain)
{
    MVCC_FLOATVALUE stFloatValue = { 0 };

    int nRet = GetFloatValue("Gain", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    gain = stFloatValue.fCurValue;

    return MV_OK;
}

// ch:�������� | en:Set Gain
int CMvCamera::SetGain(double gain)
{
    // ch:��������ǰ�Ȱ��Զ�����رգ�ʧ�����践��
    //en:Set Gain after Auto Gain is turned off, this failure does not need to return
    SetEnumValue("GainAuto", 0);

    return SetFloatValue("Gain", (float)gain);
}

// ���豸
int CMvCamera::OpenDevices_Action(std::string camera)
{
    // ����豸�Ѿ��򿪻��豸ʵ�����ڣ�ֱ�ӷ���
    if (m_bOpenDevice)
    {
        return MV_OK;
    }

    //MV_CC_DEVICE_INFO_LIST pstDevList;
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    int ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);

    // ����豸��ϢΪ�գ���ʾ������Ϣ
    if (ret == MV_OK) {
        if (m_stDevList.nDeviceNum == 0) {
            return -1;
        }
    }
    int nRet = 0;
    machineLog->write("m_stDevList.nDeviceNum = " + QString::number(m_stDevList.nDeviceNum), Normal);
    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (NULL == pDeviceInfo)
        {
            continue;
        }
        //machineLog->write("camera = " + QString::fromStdString(camera), Normal);
        //machineLog->write("chUserDefinedName = " + QString::fromStdString((const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber), Normal);
        if (strcmp((const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber, camera.data()) == 0)
        {
            // �򿪶�Ӧ��������豸
            nRet = Open(m_stDevList.pDeviceInfo[i]);
            if (nRet != MV_OK)
            {
                return -1;
            }
            break;
        }
        else
        {
            continue;
        }
        //if (camera == (char*)pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName || camera == (char*)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber)
        //{
        //    machineLog->write("camera = " + QString::fromStdString(camera), Normal);
        //    // �򿪶�Ӧ��������豸
        //    nRet = Open(m_stDevList.pDeviceInfo[i]);
        //    if (nRet != MV_OK)
        //    {
        //        return -1;
        //    }
        //    break;
        //}
        //else
        //{
        //    continue;
        //}
    }

    //// ���豸
    //int nRet = Open(m_stDevList.pDeviceInfo[0]);
    //if (nRet != MV_OK)
    //{
    //    return -1;
    //}

    // ֻ��GigE�豸��Ч����ȡ�������Ѱ���С
    if (m_stDevList.pDeviceInfo[0]->nTLayerType == MV_GIGE_DEVICE)
    {
        unsigned int nPacketSize = 0;
        nRet = GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK)
        {
            nRet = SetIntValue("GevSCPSPacketSize", nPacketSize);
        }
    }

    // ����豸�Ѵ�
    m_bOpenDevice = true;

    //��������ʱ��
    int HeartBeatTimeout = 3000;
    nRet = MV_CC_SetHeartBeatTimeout(m_hDevHandle, HeartBeatTimeout);
    //if (MV_OK != nRet)
    //{
    //    printf("Set HeartBeatTimeout fail! nRet [0x%x]\n", nRet);
    //}
    return 0;
}

//��ʼ�ɼ�
int CMvCamera::StartGrabbing_Action()
{
    if (FALSE == m_bOpenDevice)
    {
        return -1;
    }
    if (TRUE == m_bStartGrabbing)
    {
        return 0;
    }

    int nRet = StartGrabbing();
    if (MV_OK != nRet)
    {
        return -1;
    }
    m_bStartGrabbing = TRUE;
    return 0;
}

// ����
int CMvCamera::TakePhoto_Action()
{
    MV_FRAME_OUT stImageInfo = { 0 };
    int nRet = GetImageBuffer(&stImageInfo, 1000);
    if (nRet == MV_OK)
    {
        //if (nullptr == m_pSaveImageBuf || stImageInfo.stFrameInfo.nFrameLen > m_nSaveImageBufSize)
        //{
        //    if (m_pSaveImageBuf)
        //    {
        //        free(m_pSaveImageBuf);
        //        m_pSaveImageBuf = nullptr;
        //    }

        //    m_pSaveImageBuf = (unsigned char*)malloc(sizeof(unsigned char) * stImageInfo.stFrameInfo.nFrameLen);
        //    if (m_pSaveImageBuf == nullptr)
        //    {
        //        //LeaveCriticalSection(&m_hSaveImageMux);
        //        return -1;
        //    }
        //    m_nSaveImageBufSize = stImageInfo.stFrameInfo.nFrameLen;
        //}

        //memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, stImageInfo.stFrameInfo.nFrameLen);
        //memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));


        // ������֤�̰߳�ȫ
        //std::lock_guard<std::mutex> lock(m_saveMutex);
        // �� EnterCriticalSection(&m_hSaveImageMux);

        const size_t frameLen = stImageInfo.stFrameInfo.nFrameLen;

        // ����Ƿ���Ҫ���·���
        if (!m_pSaveImageBuf || frameLen != m_nSaveImageBufSize)
        {
            // �ͷž��ڴ�
            if (m_pSaveImageBuf)
            {
                free(m_pSaveImageBuf);
                m_pSaveImageBuf = nullptr;
                m_nSaveImageBufSize = 0;
            }

            // �������ڴ棨����ʹ��calloc���㣩
            m_pSaveImageBuf = static_cast<unsigned char*>(calloc(1, frameLen));
            if (!m_pSaveImageBuf)
            {
                // ������
                return -1;
            }
            m_nSaveImageBufSize = frameLen;
        }

        // �ڴ濽��
        memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, frameLen);
        memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));
    }
    return 0;
}


//���ҿɱ����ļ�·��
QString CMvCamera::findOrCreateFile()
{
    QString exePath = QCoreApplication::applicationDirPath();
    QDir photoDir(exePath + "/2D_Photo");

    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            return QString();
        }
    }

    // �� 1.bmp ��ʼ����
    int fileIndex = 1;
    QString fileName;
    do {
        fileName = photoDir.filePath(QString::number(fileIndex) + ".bmp");
        fileIndex++;
    } while (QFile::exists(fileName));  // ����ļ��Ѵ��ڣ��������������

    // �������յ��ļ�·��
    return fileName;
}

//����ͼƬ
int CMvCamera::SaveImage_Action()
{
    if (m_pSaveImageBuf == nullptr)
        return -1;

    auto name = findOrCreateFile();
    bool tp = isMono(m_stImageInfo.enPixelType);
    QImage image = convertToImage(m_pSaveImageBuf, &m_stImageInfo, tp);
    //image.save(name, "bmp", 100);
    image.save(name, "jpg", 60);
    return 0;
}


// תQImage
QImage CMvCamera::convertToImage(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, bool isMono)
{
    //QImage image;
    //if (isMono) {
    //    image = QImage(static_cast<uchar*>(pData),
    //        pFrameInfo->nWidth,
    //        pFrameInfo->nHeight,
    //        QImage::Format_Grayscale8);
    //}
    //else
    //{
    //    image = QImage(static_cast<uchar*>(pData),
    //        pFrameInfo->nWidth,
    //        pFrameInfo->nHeight,
    //        QImage::Format_RGB888);
    //}
    //return image;

    if (!pData || !pFrameInfo || pFrameInfo->nWidth <= 0 || pFrameInfo->nHeight <= 0) {
        return QImage();
    }

    QImage image;

    // ����������ظ�ʽѡ����ʽ
    switch (pFrameInfo->enPixelType) {
        case PixelType_Gvsp_Mono8: {
            image = QImage(pData, pFrameInfo->nWidth, pFrameInfo->nHeight,
                QImage::Format_Grayscale8);
            break;
        }
        case PixelType_Gvsp_RGB8_Packed: {
            image = QImage(pData, pFrameInfo->nWidth, pFrameInfo->nHeight,
                QImage::Format_RGB888);
            break;
        }
        case PixelType_Gvsp_BayerGB8: {
            // ʹ�� OpenCV ���� Demosaic
            cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            cv::Mat rgbImage;
            cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2BGR);  // BayerGB8 -> BGR

            // �� OpenCV �� Mat ת��Ϊ QImage
            image = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows,
                rgbImage.step, QImage::Format_RGB888);
            image = image.copy();  // ������� OpenCV ���ݱ��ͷź�ʧЧ
            break;


            //cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            //cv::Mat rgbImage;

            //// ģʽ 2��ת BGR ���ֶ�����ͨ��
            //cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2BGR);
            ////cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            ////cv::imwrite("raw_bayer.png", bayerImage);
            ////cv::cvtColor(rgbImage, rgbImage, cv::COLOR_BGR2RGB); // תΪ Qt ��Ҫ�� RGB
            ////cv::imwrite("opencv_rgb.png", rgbImage); // �鿴 OpenCV ����� RGB �Ƿ���ȷ
            //// �� OpenCV Mat תΪ QImage
            //image = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows,
            //    rgbImage.step, QImage::Format_RGB888);
            //image = image.copy();
            //break;
        }
        default: {
            return QImage();
        }
    }

    // ��֤ͼ���Ƿ���Ч
    if (image.isNull() || image.size().isEmpty()) {
        //qWarning() << "Failed to create QImage from camera data";
        return QImage();
    }

    return image;
}

//ֹͣ�ɼ�
int CMvCamera::StopGrabbing_Action()
{
    //ֹͣ�ɼ�
    int rtn = StopGrabbing();
    m_bStartGrabbing = FALSE;
    return rtn;
}

// ch:��ȡ֡�� | en:Get Frame Rate
int CMvCamera::GetFrameRate(double& rate)
{
    MVCC_FLOATVALUE stFloatValue = { 0 };

    int nRet = GetFloatValue("ResultingFrameRate", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    rate = stFloatValue.fCurValue;

    return MV_OK;
}

// ch:����֡�� | en:Set Frame Rate
int CMvCamera::SetFrameRate(double rate)
{
    int nRet = SetBoolValue("AcquisitionFrameRateEnable", true);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    return SetFloatValue("AcquisitionFrameRate", (float)rate);
}

//�ر��豸
int CMvCamera::CloseDevice_Action()
{
    if (m_pSaveImageBuf)
    {
        free(m_pSaveImageBuf);
        m_pSaveImageBuf = NULL;
    }
    m_nSaveImageBufSize = 0;
    
    Close();

    m_bStartGrabbing = FALSE;
    m_bOpenDevice = FALSE;

    return MV_OK;
}

//����ͼƬ����·����
int CMvCamera::SaveImage_Action(const QString filepath)
{
    if (m_pSaveImageBuf == nullptr)
        return -1;

    auto name = filepath;
    bool tp = isMono(m_stImageInfo.enPixelType);
    QImage image = convertToImage(m_pSaveImageBuf, &m_stImageInfo, tp);
    image.save(name, "bmp", 100);
    //image.save(name, "jpg", 60);
    return 0;
}