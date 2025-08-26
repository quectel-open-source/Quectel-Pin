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
//------------------------------ 工具函数 ----------------------------------
public:
    //导出csv到指定路径
    void saveCsvInThread(
        const QString& filePath,
        const std::unordered_map<QString, QString>& dataMap,
        const std::vector<QString>& fieldOrder,  //顺序列表参数
        bool append = false  // 是否追加模式
    );

    //易鼎丰mes进站请求
    MesCheckInResponse_ED mes_check_in_ED(const QString& baseUrl, const MesCheckInRequest_ED& req);
    //保存halcon原始图片存到文件夹
    int saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName);
    QImage showVision_slot(Vision_Message message);
    void saveQImage(const QImage& image, const QString& filePath,
        const QString& format = "JPG", int quality = 80);

    //保存QImage结果图片存到文件夹
    int saveResultPhotoToFile(QImage img, QString pathName, QString fileName);
    //易鼎丰mes过站请求
    MesCheckInResponse_ED mes_full_check_in(const QString& baseUrl, const MesCheckInFullRequest_ED& req);
    //csv的内容转换到mes结构体
    MesCheckInFullRequest_ED mapToMesRequest(const std::unordered_map<QString, QString>& resultMap);
    //保存相机参数
    void saveCameraMess(std::unordered_map<QString, std::string>& cameraName);
    //读取相机参数
    void readCameraMess(std::unordered_map<QString, std::string>& cameraName);
};

