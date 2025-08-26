/************************************************************************/
/* ��C++�ӿ�Ϊ�������Գ��ú������ж��η�װ�������û�ʹ��                */
/************************************************************************/

#ifndef _MV_CAMERA_H_
#define _MV_CAMERA_H_

#include "MvCameraControl.h"
#include <string.h>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include "Thread.h"
#include <QObject>

#ifndef MV_NULL
#define MV_NULL    0
#endif

#pragma execution_character_set("utf-8")

using namespace HalconCpp;

class CMvCamera : public QObject
{
    Q_OBJECT
public:
    CMvCamera();
    ~CMvCamera();

    // ch:��ȡSDK�汾�� | en:Get SDK Version
    static int GetSDKVersion();

    // ch:ö���豸 | en:Enumerate Device
    static int EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList);

    // ch:�ж��豸�Ƿ�ɴ� | en:Is the device accessible
    static bool IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode);

    // ch:���豸 | en:Open Device
    int Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

    // ch:�ر��豸 | en:Close Device
    int Close();

    // ch:�ж�����Ƿ�������״̬ | en:Is The Device Connected
    bool IsDeviceConnected();

    // ch:ע��ͼ�����ݻص� | en:Register Image Data CallBack
    int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

    // ch:����ץͼ | en:Start Grabbing
    int StartGrabbing();

    // ch:ֹͣץͼ | en:Stop Grabbing
    int StopGrabbing();

    // ch:������ȡһ֡ͼ������ | en:Get one frame initiatively
    int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);

    // ch:�ͷ�ͼ�񻺴� | en:Free image buffer
    int FreeImageBuffer(MV_FRAME_OUT* pFrame);

    // ch:��ʾһ֡ͼ�� | en:Display one frame image
    int DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo);

    // ch:����SDK�ڲ�ͼ�񻺴�ڵ���� | en:Set the number of the internal image cache nodes in SDK
    int SetImageNodeNum(unsigned int nNum);

    // ch:��ȡ�豸��Ϣ | en:Get device information
    int GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo);

    // ch:��ȡGEV�����ͳ����Ϣ | en:Get detect info of GEV camera
    int GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect);

    // ch:��ȡU3V�����ͳ����Ϣ | en:Get detect info of U3V camera
    int GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect);

    // ch:��ȡ������Int�Ͳ������� Width��Height����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue);
    int SetIntValue(IN const char* strKey, IN int64_t nValue);

    // ch:��ȡ������Enum�Ͳ������� PixelFormat����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int SetEnumValueByString(IN const char* strKey, IN const char* sValue);

    // ch:��ȡ������Float�Ͳ������� ExposureTime��Gain����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue);
    int SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:��ȡ������Bool�Ͳ������� ReverseX����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetBoolValue(IN const char* strKey, OUT bool *pbValue);
    int SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:��ȡ������String�Ͳ������� DeviceUserID����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�UserSetSave
    // en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue);
    int SetStringValue(IN const char* strKey, IN const char * strValue);

    // ch:ִ��һ��Command������� UserSetSave����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int CommandExecute(IN const char* strKey);

    // ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
    int GetOptimalPacketSize(unsigned int* pOptimalPacketSize);

    // ch:ע����Ϣ�쳣�ص� | en:Register Message Exception CallBack
    int RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser);

    // ch:ע�ᵥ���¼��ص� | en:Register Event CallBack
    int RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser);

    // ch:ǿ��IP | en:Force IP
    int ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay);

    // ch:����IP��ʽ | en:IP configuration method
    int SetIpConfig(unsigned int nType);

    // ch:�������紫��ģʽ | en:Set Net Transfer Mode
    int SetNetTransMode(unsigned int nType);

    // ch:���ظ�ʽת�� | en:Pixel format conversion
    int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam);

    // ch:����ͼƬ | en:save image
    int SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);

    // ch:����ͼƬΪ�ļ� | en:Save the image as a file
    int SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstParam);

    // �ж�ͨ��
    bool isMono(MvGvspPixelType enType);

    //תHObject
    HObject convertToHalconImage(unsigned char* pData, int width, int height, bool isMono);

    int GetExposureTime(double& time);               // ch:�����ع�ʱ�� | en:Set Exposure Time
    int SetExposureTime(double time);
    int GetGain(double& gain);                       // ch:�������� | en:Set Gain
    int SetGain(double gain);

    //*********** �����˶��߼�
    //���豸
    int OpenDevices_Action(std::string camera);
    //��ʼ�ɼ�
    int StartGrabbing_Action();
    // ����
    int TakePhoto_Action();
    //���ҿɱ����ļ�·��
    QString findOrCreateFile();
    //����ͼƬ
    int SaveImage_Action();
    QImage convertToImage(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, bool isMono);
    //ֹͣ�ɼ�
    int StopGrabbing_Action();
    // ch:����֡�� | en:Set Frame Rate
    int GetFrameRate(double& rate);
    int SetFrameRate(double rate);
    int CloseDevice_Action();
    //����ͼƬ����·����
    int SaveImage_Action(const QString filepath);

public:
    CRITICAL_SECTION        m_hSaveImageMux;
    unsigned char* m_pSaveImageBuf = nullptr; //��Ƭ������
    unsigned int            m_nSaveImageBufSize;
    MV_FRAME_OUT_INFO_EX    m_stImageInfo; //��Ƭ��Ϣ
    BOOL                    m_bOpenDevice = false;                        // ch:�Ƿ���豸 | en:Whether to open device
    BOOL                    m_bStartGrabbing = false;                     // ch:�Ƿ�ʼץͼ | en:Whether to start grabbing
    //�˶��������µ���Ƭ
    HObject m_outImg;

private:

    void*               m_hDevHandle;

    MV_CC_DEVICE_INFO_LIST  m_stDevList;


};

#endif//_MV_CAMERA_H_
