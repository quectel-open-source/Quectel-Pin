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

// ch:获取SDK版本号 | en:Get SDK Version
int CMvCamera::GetSDKVersion()
{
    return MV_CC_GetSDKVersion();
}

// ch:枚举设备 | en:Enumerate Device
int CMvCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
    return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

// ch:判断设备是否可达 | en:Is the device accessible
bool CMvCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
    return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}

// ch:打开设备 | en:Open Device
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

    //设置心跳时间
    int HeartBeatTimeout = 3000;
    nRet = MV_CC_SetHeartBeatTimeout(m_hDevHandle, HeartBeatTimeout);

    return nRet;
}

// ch:关闭设备 | en:Close Device
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

// ch:判断相机是否处于连接状态 | en:Is The Device Connected
bool CMvCamera::IsDeviceConnected()
{
    return MV_CC_IsDeviceConnected(m_hDevHandle);
}

// ch:注册图像数据回调 | en:Register Image Data CallBack
int CMvCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterImageCallBackEx(m_hDevHandle, cbOutput, pUser);
}

// ch:开启抓图 | en:Start Grabbing
int CMvCamera::StartGrabbing()
{
    return MV_CC_StartGrabbing(m_hDevHandle);
}

// ch:停止抓图 | en:Stop Grabbing
int CMvCamera::StopGrabbing()
{
    return MV_CC_StopGrabbing(m_hDevHandle);
}

// ch:主动获取一帧图像数据 | en:Get one frame initiatively
int CMvCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
    return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

// ch:释放图像缓存 | en:Free image buffer
int CMvCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
    return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

// ch:设置显示窗口句柄 | en:Set Display Window Handle
int CMvCamera::DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo)
{
    return MV_CC_DisplayOneFrame(m_hDevHandle, pDisplayInfo);
}

// ch:设置SDK内部图像缓存节点个数 | en:Set the number of the internal image cache nodes in SDK
int CMvCamera::SetImageNodeNum(unsigned int nNum)
{
    return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

// ch:获取设备信息 | en:Get device information
int CMvCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
    return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

// ch:获取GEV相机的统计信息 | en:Get detect info of GEV camera
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

// ch:获取U3V相机的统计信息 | en:Get detect info of U3V camera
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

// ch:获取和设置Int型参数，如 Width和Height，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue)
{
    return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int CMvCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
    return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

// ch:获取和设置Enum型参数，如 PixelFormat，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
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

// ch:获取和设置Float型参数，如 ExposureTime和Gain，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue)
{
    return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int CMvCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
    return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

// ch:获取和设置Bool型参数，如 ReverseX，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetBoolValue(IN const char* strKey, OUT bool *pbValue)
{
    return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int CMvCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
    return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

// ch:获取和设置String型参数，如 DeviceUserID，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件UserSetSave
// en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue)
{
    return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int CMvCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
    return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

// ch:执行一次Command型命令，如 UserSetSave，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::CommandExecute(IN const char* strKey)
{
    return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
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

// ch:注册消息异常回调 | en:Register Message Exception CallBack
int CMvCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),void* pUser)
{
    return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

// ch:注册单个事件回调 | en:Register Event CallBack
int CMvCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
}

// ch:强制IP | en:Force IP
int CMvCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
    return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

// ch:配置IP方式 | en:IP configuration method
int CMvCamera::SetIpConfig(unsigned int nType)
{
    return MV_GIGE_SetIpConfig(m_hDevHandle, nType);
}

// ch:设置网络传输模式 | en:Set Net Transfer Mode
int CMvCamera::SetNetTransMode(unsigned int nType)
{
    return MV_GIGE_SetNetTransMode(m_hDevHandle, nType);
}

// ch:像素格式转换 | en:Pixel format conversion
int CMvCamera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam)
{
    return MV_CC_ConvertPixelType(m_hDevHandle, pstCvtParam);
}

// ch:保存图片 | en:save image
int CMvCamera::SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam)
{
    return MV_CC_SaveImageEx2(m_hDevHandle, pstParam);
}

// ch:保存图片为文件 | en:Save the image as a file
int CMvCamera::SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam)
{
    return MV_CC_SaveImageToFile(m_hDevHandle, pstSaveFileParam);
}

//判断是否单通道
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

//把海康的流数据转成 HObject
HObject CMvCamera::convertToHalconImage(unsigned char* pData, int width, int height, bool isMono)
{
    HObject hImage;
    try {
        if (isMono) {
            // 如果是灰度图像，使用 GenImage1
            GenImage1(&hImage, "byte", width, height, (Hlong)pData);
        }
        else {
            // 如果是彩色图像，使用 GenImageInterleaved
            // 使用 GenImageInterleaved 创建 RGB 图像
            GenImageInterleaved(&hImage, (Hlong)pData, "rgb", width, height, 0, "byte", width * 3, height, 0, 0, -1, 0);
        }
    }
    catch (HalconCpp::HException& e) {
        throw std::runtime_error("convertToHalconImage Halcon error: " + std::string(e.ErrorMessage()));
    }
    return hImage;
}


// ch:获取曝光时间 | en:Get Exposure Time
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

// ch:设置曝光时间 | en:Set Exposure Time
int CMvCamera::SetExposureTime(double time)
{
    // ch:调节这两个曝光模式，才能让曝光时间生效
    // en:Adjust these two exposure mode to allow exposure time effective
    int nRet = SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

    return SetFloatValue("ExposureTime", (float)time);
}

// ch:获取增益 | en:Get Gain
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

// ch:设置增益 | en:Set Gain
int CMvCamera::SetGain(double gain)
{
    // ch:设置增益前先把自动增益关闭，失败无需返回
    //en:Set Gain after Auto Gain is turned off, this failure does not need to return
    SetEnumValue("GainAuto", 0);

    return SetFloatValue("Gain", (float)gain);
}

// 打开设备
int CMvCamera::OpenDevices_Action(std::string camera)
{
    // 如果设备已经打开或设备实例存在，直接返回
    if (m_bOpenDevice)
    {
        return MV_OK;
    }

    //MV_CC_DEVICE_INFO_LIST pstDevList;
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    int ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);

    // 如果设备信息为空，显示错误信息
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
            // 打开对应相机名的设备
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
        //    // 打开对应相机名的设备
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

    //// 打开设备
    //int nRet = Open(m_stDevList.pDeviceInfo[0]);
    //if (nRet != MV_OK)
    //{
    //    return -1;
    //}

    // 只对GigE设备有效，获取网络的最佳包大小
    if (m_stDevList.pDeviceInfo[0]->nTLayerType == MV_GIGE_DEVICE)
    {
        unsigned int nPacketSize = 0;
        nRet = GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK)
        {
            nRet = SetIntValue("GevSCPSPacketSize", nPacketSize);
        }
    }

    // 标记设备已打开
    m_bOpenDevice = true;

    //设置心跳时间
    int HeartBeatTimeout = 3000;
    nRet = MV_CC_SetHeartBeatTimeout(m_hDevHandle, HeartBeatTimeout);
    //if (MV_OK != nRet)
    //{
    //    printf("Set HeartBeatTimeout fail! nRet [0x%x]\n", nRet);
    //}
    return 0;
}

//开始采集
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

// 拍照
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


        // 加锁保证线程安全
        //std::lock_guard<std::mutex> lock(m_saveMutex);
        // 或 EnterCriticalSection(&m_hSaveImageMux);

        const size_t frameLen = stImageInfo.stFrameInfo.nFrameLen;

        // 检查是否需要重新分配
        if (!m_pSaveImageBuf || frameLen != m_nSaveImageBufSize)
        {
            // 释放旧内存
            if (m_pSaveImageBuf)
            {
                free(m_pSaveImageBuf);
                m_pSaveImageBuf = nullptr;
                m_nSaveImageBufSize = 0;
            }

            // 分配新内存（建议使用calloc清零）
            m_pSaveImageBuf = static_cast<unsigned char*>(calloc(1, frameLen));
            if (!m_pSaveImageBuf)
            {
                // 错误处理
                return -1;
            }
            m_nSaveImageBufSize = frameLen;
        }

        // 内存拷贝
        memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, frameLen);
        memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));
    }
    return 0;
}


//查找可保存文件路径
QString CMvCamera::findOrCreateFile()
{
    QString exePath = QCoreApplication::applicationDirPath();
    QDir photoDir(exePath + "/2D_Photo");

    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            return QString();
        }
    }

    // 从 1.bmp 开始查找
    int fileIndex = 1;
    QString fileName;
    do {
        fileName = photoDir.filePath(QString::number(fileIndex) + ".bmp");
        fileIndex++;
    } while (QFile::exists(fileName));  // 如果文件已存在，则继续增加索引

    // 返回最终的文件路径
    return fileName;
}

//保存图片
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


// 转QImage
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

    // 根据相机像素格式选择处理方式
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
            // 使用 OpenCV 进行 Demosaic
            cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            cv::Mat rgbImage;
            cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2BGR);  // BayerGB8 -> BGR

            // 将 OpenCV 的 Mat 转换为 QImage
            image = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows,
                rgbImage.step, QImage::Format_RGB888);
            image = image.copy();  // 深拷贝避免 OpenCV 数据被释放后失效
            break;


            //cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            //cv::Mat rgbImage;

            //// 模式 2：转 BGR 再手动交换通道
            //cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2BGR);
            ////cv::Mat bayerImage(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);
            ////cv::imwrite("raw_bayer.png", bayerImage);
            ////cv::cvtColor(rgbImage, rgbImage, cv::COLOR_BGR2RGB); // 转为 Qt 需要的 RGB
            ////cv::imwrite("opencv_rgb.png", rgbImage); // 查看 OpenCV 输出的 RGB 是否正确
            //// 将 OpenCV Mat 转为 QImage
            //image = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows,
            //    rgbImage.step, QImage::Format_RGB888);
            //image = image.copy();
            //break;
        }
        default: {
            return QImage();
        }
    }

    // 验证图像是否有效
    if (image.isNull() || image.size().isEmpty()) {
        //qWarning() << "Failed to create QImage from camera data";
        return QImage();
    }

    return image;
}

//停止采集
int CMvCamera::StopGrabbing_Action()
{
    //停止采集
    int rtn = StopGrabbing();
    m_bStartGrabbing = FALSE;
    return rtn;
}

// ch:获取帧率 | en:Get Frame Rate
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

// ch:设置帧率 | en:Set Frame Rate
int CMvCamera::SetFrameRate(double rate)
{
    int nRet = SetBoolValue("AcquisitionFrameRateEnable", true);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    return SetFloatValue("AcquisitionFrameRate", (float)rate);
}

//关闭设备
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

//保存图片（带路径）
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