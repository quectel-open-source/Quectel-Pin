#include "rs_FunctionTool.h"
#include <QtCore>
#include "qc_log.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "ImageTypeConvert.h"
#include "pinLabel.h"
#include "JsonConfigManager.h"

rs_FunctionTool::rs_FunctionTool()
{

}

rs_FunctionTool::~rs_FunctionTool()
{

}

//导出csv到指定路径
void rs_FunctionTool::saveCsvInThread(
    const QString& filePath,
    const std::unordered_map<QString, QString>& dataMap,
    const std::vector<QString>& fieldOrder,
    bool append
)
{
    QString actualPath = filePath;
    if (!actualPath.endsWith(".csv", Qt::CaseInsensitive)) {
        actualPath += ".csv";
    }

    QFile file(actualPath);
    QIODevice::OpenMode mode = append
        ? QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text
        : QIODevice::WriteOnly | QIODevice::Text;

    if (!file.open(mode)) {
        machineLog->write("CSV save failed - cannot open file:" + actualPath, Normal);
        return;
    }

    QTextStream stream(&file);
    // 写入UTF-8 BOM标识
    stream << QString::fromUtf8("\xEF\xBB\xBF");

    // 第一次写入时输出表头 - 使用 fieldOrder 顺序
    if (!append || file.size() == 0) {
        QStringList headers;
        for (const auto& header : fieldOrder) {
            QString escapedHeader = header;
            // 转义特殊字符
            if (escapedHeader.contains('"') || escapedHeader.contains(',') || escapedHeader.contains('\n')) {
                escapedHeader = "\"" + escapedHeader.replace("\"", "\"\"") + "\"";
            }
            headers << escapedHeader;
        }
        stream << headers.join(',') << "\n";
    }

    // 写入数据行 - 使用 fieldOrder 顺序
    QStringList rowValues;
    for (const auto& key : fieldOrder) {
        QString value = "";

        auto it = dataMap.find(key);
        if (it != dataMap.end()) {
            value = it->second;
        }

        // 值转义处理
        if (value.contains('"') || value.contains(',') || value.contains('\n')) {
            value = "\"" + value.replace("\"", "\"\"") + "\"";
        }
        rowValues << value;
    }
    stream << rowValues.join(',') << "\n";

    file.close();
}

////导出csv到指定路径
//void rs_FunctionTool::saveCsvInThread(
//    const QString& filePath,
//    const std::unordered_map<QString, QString>& dataMap,
//    const std::vector<QString>& fieldOrder,
//    bool append
//)
//{
//    QString actualPath = filePath;
//    if (!actualPath.endsWith(".csv", Qt::CaseInsensitive)) {
//        actualPath += ".csv";
//    }
//
//    QFile file(actualPath);
//    QIODevice::OpenMode mode = append
//        ? QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text
//        : QIODevice::WriteOnly | QIODevice::Text;
//
//    if (!file.open(mode)) {
//        machineLog->write("CSV保存失败-无法打开文件:" + actualPath, Normal);
//        return;
//    }
//
//    QTextStream stream(&file);
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    stream.setCodec("UTF-8");  // Qt5使用setCodec
//    stream.setGenerateByteOrderMark(true);  // 自动生成BOM
//#else
//    stream.setEncoding(QStringConverter::Utf8);  // Qt6使用setEncoding
//#endif
//
//    // 第一次写入时输出表头
//    if (!append || file.size() == 0) {
//        QStringList headers;
//        for (const auto& header : fieldOrder) {
//            QString formattedHeader = header;
//
//            // FIX: 修复replace调用错误
//            if (formattedHeader.contains('"') || formattedHeader.contains(',') || formattedHeader.contains('\n')) {
//                // 使用正确的replace重载
//                QString replacedHeader = formattedHeader;
//                replacedHeader.replace("\"", "\"\"");
//                formattedHeader = '"' + replacedHeader + '"';
//            }
//            headers << formattedHeader;
//        }
//        stream << headers.join(',') << "\n";
//    }
//
//    // 写入数据行
//    QList<QStringList> rowGroups; // 存储分组后的行
//    QStringList currentGroup;
//    int groupSize = 7; // 每组7个字段
//
//    for (size_t i = 0; i < fieldOrder.size(); ++i) {
//        const auto& key = fieldOrder[i];
//        QString value = dataMap.count(key) ? dataMap.at(key) : "";
//
//        // 每7个字段分一组
//        if (currentGroup.size() >= groupSize) {
//            rowGroups << currentGroup;
//            currentGroup.clear();
//        }
//
//        // FIX: 修复replace调用错误
//        if (value.contains('"') || value.contains(',') || value.contains('\n')) {
//            // 使用正确的replace重载
//            QString replacedValue = value;
//            replacedValue.replace("\"", "\"\"");
//            value = '"' + replacedValue + '"';
//        }
//
//        currentGroup << value;
//    }
//
//    // 添加最后一组
//    if (!currentGroup.isEmpty()) {
//        rowGroups << currentGroup;
//    }
//
//    // 写入分组数据（每行一个组）
//    for (const QStringList& group : rowGroups) {
//        stream << group.join(',') << "\r\n"; // 使用Windows换行符确保兼容性
//    }
//
//    // 确保数据写入磁盘
//    stream.flush();
//    file.flush();
//    file.close();
//
//    // 检查文件是否成功写入
//    QFileInfo info(actualPath);
//    if (info.size() == 0) {
//        machineLog->write("CSV保存失败：文件为空：" + actualPath, Normal);
//    }
//    else {
//        machineLog->write("CSV保存成功：" + actualPath, Normal);
//    }
//}

Q_GLOBAL_STATIC(QNetworkAccessManager, globalManager)
//易鼎丰mes进站请求
MesCheckInResponse_ED rs_FunctionTool::mes_check_in_ED(const QString& baseUrl, const MesCheckInRequest_ED& req) {
    QNetworkRequest netRequest;

    // 构建基础URL
    QUrl requestUrl(baseUrl + "/api/product/checkRoute");
    if (!requestUrl.isValid()) {
        return { false, "无效的URL格式: " + baseUrl };
    }

    // ================== 构建查询参数 ==================
    QUrlQuery query;
    // 必填字段
    query.addQueryItem("sn", req.sn);                        // 主条码
    query.addQueryItem("workPlaceCode", req.workPlaceCode);  // 测试站点
    query.addQueryItem("productSn", req.productSn);          // 产品码

    // 可选字段
    if (!req.laneId.isEmpty()) query.addQueryItem("laneId", req.laneId);            // 轨道
    if (!req.checkAllRef.isEmpty()) query.addQueryItem("checkAllRef", req.checkAllRef); // 是否检查小板

    // 将查询参数附加到URL
    requestUrl.setQuery(query);
    netRequest.setUrl(requestUrl);

    // ================== 发送GET请求 ==================
    QNetworkReply* reply = globalManager->get(netRequest);

    // ================== 超时控制 ==================
    QEventLoop loop;
    QTimer timeoutTimer;
    const int timeoutMs = 5000;  // 5秒超时

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("ED_MES请求超时: " + requestUrl.toString(), Mes);
        if (reply->isRunning()) reply->abort();
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== 响应处理 ==================
    MesCheckInResponse_ED resp;
    bool isTimeout = !timeoutTimer.isActive();

    if (isTimeout) {
        return { false, "请求超时（5秒无响应）" };
    }

    if (reply->error() != QNetworkReply::NoError) {
        return { false, "网络错误: " + reply->errorString() };
    }

    // 解析JSON响应
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    reply->deleteLater();

    if (parseError.error != QJsonParseError::NoError) {
        return { false, "JSON解析错误: " + parseError.errorString() };
    }

    // 提取响应字段
    QJsonObject resObj = doc.object();
    resp.state = resObj.value("state").toBool(false);
    resp.description = resObj.value("description").toString();

    // 如果state=false但无描述信息，添加默认错误提示
    if (!resp.state && resp.description.isEmpty()) {
        resp.description = "校验失败，无详细错误信息";
    }

    // 记录响应日志
    QString logMsg = QString("ED_MES响应 - SN: %1, 状态: %2, 描述: %3")
        .arg(req.sn)
        .arg(resp.state ? "合格" : "不合格")
        .arg(resp.description);
    machineLog->write(logMsg, Mes);

    return resp;
}

//保存halcon原始图片存到文件夹
int rs_FunctionTool::saveOriginalPhotoToFile(HObject img, QString pathName, QString fileName)
{
    if (img.Key() == nullptr)
    {
        return -1;
    }
    if (pathName.isEmpty())
    {
        return -1;
    }

    pathName.replace('*', '@');         // 替换所有*为@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName;

    try
    {
        Mat showMatImg = ImageTypeConvert::HObject2Mat(img);
        QString tpPath = finalPath + ".bmp";
        cv::imwrite(tpPath.toStdString(), showMatImg);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}

//视觉功能的纯结果显示视图
QImage rs_FunctionTool::showVision_slot(Vision_Message message)
{
    HObject originaImg;
    HObject resultImg;
    PointItemConfig config;
    QString filePath = "";
    try {
        //if (!result.ho_Image.IsInitialized())
        //    return;
        if (message.img.Key() == nullptr)
        {
            return QImage();
        }
        originaImg = message.img;
        QString photoPath;
        //保存图片
        //saveHalconJPG(img, photoPath);
        //保存图片到型号文件夹
        QString picPath = "";

        //HTuple hv_Width, hv_Height;
        //GetImageSize(img, &hv_Width, &hv_Height);

        // 获取窗口大小
        int width = 600;
        int height = 500;
        // 新 创建 pinLabel 对象
        pinLabel label;

        //仅用作图像合并处理了，不再作为显示模块 Erik 2025/3/21
        // 显示图像
        label.setImage(message.img);

        //------------
        label.showVisionImage(message);
        HObject showHjImg = label.getShowImg();
        Mat showMatImg = ImageTypeConvert::HObject2Mat(showHjImg);
        //cv::namedWindow("image", cv::WINDOW_NORMAL);
        //cv::resizeWindow("image", 800, 800);
        //cv::imshow("image", showMatImg);
        QImage showQImg = ImageTypeConvert::Mat2QImage(showMatImg);
        message.clear();
        return showQImg;
    }
    catch (const HalconCpp::HException& e) {
        machineLog->write("showVision_slot Error displaying Halcon image or region:" + QString(e.ErrorMessage().Text()), Normal);
        message.clear();
    }
    return QImage();
}

// 替换 Halcon 的 WriteImage
void rs_FunctionTool::saveQImage(const QImage& image, const QString& filePath,
    const QString& format, int quality)
{
    if (image.isNull()) {
        qWarning() << "Cannot save null image";
        return;
    }

    // 检查目录是否存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir.path();
            return;
        }
    }

    // 保存图像
    if (!image.save(filePath, format.toUtf8().constData(), quality)) {
        machineLog->write("保存失败" + filePath, Normal);
    }
}

//保存QImage结果图片存到文件夹
int rs_FunctionTool::saveResultPhotoToFile(QImage img, QString pathName, QString fileName)
{
    if (img.isNull())
    {
        return -1;
    }
    if (pathName.isEmpty())
    {
        return -1;
    }

    pathName.replace('*', '@');         // 替换所有*为@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("无法创建结果存储文件夹！:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName + ".jpg";

    try
    {
        //存结果图
        saveQImage(img, finalPath, "JPG", 80);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}


// MES过站函数
MesCheckInResponse_ED rs_FunctionTool::mes_full_check_in(const QString& baseUrl,const MesCheckInFullRequest_ED& req)
{
    QNetworkRequest netRequest;

    // 构建完整URL
    QUrl requestUrl(baseUrl + "/api/Inspection/visionInspection");
    if (!requestUrl.isValid()) {
        return { false, "无效的URL格式: " + baseUrl };
    }
    netRequest.setUrl(requestUrl);

    // 设置请求头
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ================== 构建JSON数据 ==================
    QJsonObject requestJson;

    // 填充所有字段
    requestJson["WorkPlaceCode"] = req.WorkPlaceCode;
    requestJson["WorkOrderCode"] = req.WorkOrderCode;
    requestJson["User"] = req.User;
    requestJson["Sn"] = req.Sn;
    requestJson["Result"] = req.Result;
    requestJson["ProductCode"] = req.ProductCode;
    requestJson["OtherCode1"] = req.OtherCode1;
    requestJson["OtherCode2"] = req.OtherCode2;
    requestJson["OtherCode3"] = req.OtherCode3;
    requestJson["OtherCode4"] = req.OtherCode4;
    requestJson["Result_2D"] = req.Result_2D;
    requestJson["Result_3D"] = req.Result_3D;
    requestJson["Screw1Inspection"] = req.Screw1Inspection;
    requestJson["Screw2Inspection"] = req.Screw2Inspection;
    requestJson["Screw3Inspection"] = req.Screw3Inspection;
    requestJson["Screw4Inspection"] = req.Screw4Inspection;
    requestJson["Result_LabelCharacters"] = req.Result_LabelCharacters;
    requestJson["Result_SnapFit"] = req.Result_SnapFit;
    requestJson["WaterproofBreathableInspection"] = req.WaterproofBreathableInspection;
    requestJson["SealantInspection"] = req.SealantInspection;
    requestJson["Result_CircularDistance1"] = req.Result_CircularDistance1;
    requestJson["Result_CircularDistance2"] = req.Result_CircularDistance2;
    requestJson["Result_CircularDistance3"] = req.Result_CircularDistance3;
    requestJson["Result_CircularDistance4"] = req.Result_CircularDistance4;
    requestJson["PictureUrl"] = req.PictureUrl;

    // ================== 发送POST请求 ==================
    QByteArray jsonData = QJsonDocument(requestJson).toJson();
    QNetworkReply* reply = globalManager->post(netRequest, jsonData);

    // ================== 超时控制 ==================
    QEventLoop loop;
    QTimer timeoutTimer;
    const int timeoutMs = 5000;  // 5秒超时

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("MES过站请求超时: " + requestUrl.toString(), Normal);
        if (reply->isRunning()) reply->abort();
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== 响应处理 ==================
    MesCheckInResponse_ED resp;
    bool isTimeout = !timeoutTimer.isActive();

    if (isTimeout) {
        return { false, "请求超时（5秒无响应）" };
    }

    if (reply->error() != QNetworkReply::NoError) {
        return { false, "网络错误: " + reply->errorString() };
    }

    // 解析JSON响应
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    reply->deleteLater();

    if (parseError.error != QJsonParseError::NoError) {
        return { false, "JSON解析错误: " + parseError.errorString() };
    }

    // 提取响应字段
    QJsonObject resObj = doc.object();
    resp.state = resObj.value("state").toBool(false);
    resp.description = resObj.value("description").toString();

    // 如果state=false但无描述信息，添加默认错误提示
    if (!resp.state && resp.description.isEmpty()) {
        resp.description = "过站失败，无详细错误信息";
    }

    // 记录响应日志
    QString logMsg = QString("MES过站响应 - SN: %1, 状态: %2, 描述: %3")
        .arg(req.Sn)
        .arg(resp.state ? "成功" : "失败")
        .arg(resp.description);
    machineLog->write(logMsg, Normal);

    return resp;
}

MesCheckInFullRequest_ED rs_FunctionTool::mapToMesRequest(const std::unordered_map<QString, QString>& resultMap) {
    MesCheckInFullRequest_ED req;

    // 使用辅助函数获取值，提供默认值
    auto getValue = [&](const QString& key, const QString& defaultValue = "") {
        auto it = resultMap.find(key);
        return (it != resultMap.end()) ? it->second : defaultValue;
    };

    // 建立映射关系
    //req.WorkPlaceCode = getValue("name");
    req.WorkOrderCode = getValue("工单");
    req.User = getValue("生产人");
    req.Sn = getValue("SN");
    req.Result = getValue("最终结果");
    req.ProductCode = getValue("产品码");

    req.OtherCode1 = getValue("其他码1");
    req.OtherCode2 = getValue("其他码2");
    req.OtherCode3 = getValue("其他码3");
    req.OtherCode4 = getValue("其他码4");
    req.Result_2D = getValue("2D结果");
    req.Result_3D = getValue("3D结果");
    req.Screw1Inspection = getValue("螺丝检测1");
    req.Screw2Inspection = getValue("螺丝检测2");
    req.Screw3Inspection = getValue("螺丝检测3");
    req.Screw4Inspection = getValue("螺丝检测4");
    req.Result_LabelCharacters = getValue("标签字符检测");
    req.Result_SnapFit = getValue("卡扣检测");
    req.WaterproofBreathableInspection = getValue("防水透气膜检测");
    req.SealantInspection = getValue("密封胶检测");
    req.Result_CircularDistance1 = getValue("圆测距检测1");
    req.Result_CircularDistance2 = getValue("圆测距检测2");
    req.Result_CircularDistance3 = getValue("圆测距检测3");
    req.Result_CircularDistance4 = getValue("圆测距检测4");
    req.PictureUrl = getValue("图片路径");

    return req;
}

// 保存相机信息（暂时没用上，和设备信息保存一起了）
void rs_FunctionTool::saveCameraMess(std::unordered_map<QString, std::string>& cameraName)
{
    JsonConfigManager config("camera_data.json");
    // 写入参数到JSON
    for (const auto& camera : cameraName)
    {
        config.writeParam(camera.first, QString::fromStdString(camera.second));
    }
}

// 读取相机信息（暂时没用上，和设备信息保存一起了）
void rs_FunctionTool::readCameraMess(std::unordered_map<QString, std::string>& cameraName)
{
    JsonConfigManager config("camera_data.json");

    // 从JSON读取并赋值（带默认值）
    for (const auto& name : LS_CameraName::allCamera)
    {
        QString cameraID = config.readString(name, "");
        if (!cameraID.isEmpty())
        {
            cameraName[name] = cameraID.toStdString();
        }
    }
}