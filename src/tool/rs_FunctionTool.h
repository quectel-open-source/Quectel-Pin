#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <unordered_map>
#include "rs.h"


class rs_FunctionTool : public QObject
{
    Q_OBJECT
public:
    rs_FunctionTool();
    ~rs_FunctionTool();
//------------------------------ ���ߺ��� ----------------------------------
public:
    //����csv��ָ��·��
    void saveCsvInThread(
        const QString& filePath,
        const std::unordered_map<QString, QString>& dataMap,
        const std::vector<QString>& fieldOrder,  //˳���б����
        bool append = false  // �Ƿ�׷��ģʽ
    );

    //�׶���mes��վ����
    MesCheckInResponse_ED mes_check_in_ED(const QString& baseUrl, const MesCheckInRequest_ED& req);
    //����halconԭʼͼƬ�浽�ļ���
    int saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName);
    QImage showVision_slot(Vision_Message message);
    void saveQImage(const QImage& image, const QString& filePath,
        const QString& format = "JPG", int quality = 80);

    //����QImage���ͼƬ�浽�ļ���
    int saveResultPhotoToFile(QImage img, QString pathName, QString fileName);
    //�׶���mes��վ����
    MesCheckInResponse_ED mes_full_check_in(const QString& baseUrl, const MesCheckInFullRequest_ED& req);
    //csv������ת����mes�ṹ��
    MesCheckInFullRequest_ED mapToMesRequest(const std::unordered_map<QString, QString>& resultMap);
    //�����������
    void saveCameraMess(std::unordered_map<QString, std::string>& cameraName);
    //��ȡ�������
    void readCameraMess(std::unordered_map<QString, std::string>& cameraName);
};

