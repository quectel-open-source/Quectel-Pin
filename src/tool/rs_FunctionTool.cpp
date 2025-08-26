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

//����csv��ָ��·��
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
    // д��UTF-8 BOM��ʶ
    stream << QString::fromUtf8("\xEF\xBB\xBF");

    // ��һ��д��ʱ�����ͷ - ʹ�� fieldOrder ˳��
    if (!append || file.size() == 0) {
        QStringList headers;
        for (const auto& header : fieldOrder) {
            QString escapedHeader = header;
            // ת�������ַ�
            if (escapedHeader.contains('"') || escapedHeader.contains(',') || escapedHeader.contains('\n')) {
                escapedHeader = "\"" + escapedHeader.replace("\"", "\"\"") + "\"";
            }
            headers << escapedHeader;
        }
        stream << headers.join(',') << "\n";
    }

    // д�������� - ʹ�� fieldOrder ˳��
    QStringList rowValues;
    for (const auto& key : fieldOrder) {
        QString value = "";

        auto it = dataMap.find(key);
        if (it != dataMap.end()) {
            value = it->second;
        }

        // ֵת�崦��
        if (value.contains('"') || value.contains(',') || value.contains('\n')) {
            value = "\"" + value.replace("\"", "\"\"") + "\"";
        }
        rowValues << value;
    }
    stream << rowValues.join(',') << "\n";

    file.close();
}

////����csv��ָ��·��
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
//        machineLog->write("CSV����ʧ��-�޷����ļ�:" + actualPath, Normal);
//        return;
//    }
//
//    QTextStream stream(&file);
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    stream.setCodec("UTF-8");  // Qt5ʹ��setCodec
//    stream.setGenerateByteOrderMark(true);  // �Զ�����BOM
//#else
//    stream.setEncoding(QStringConverter::Utf8);  // Qt6ʹ��setEncoding
//#endif
//
//    // ��һ��д��ʱ�����ͷ
//    if (!append || file.size() == 0) {
//        QStringList headers;
//        for (const auto& header : fieldOrder) {
//            QString formattedHeader = header;
//
//            // FIX: �޸�replace���ô���
//            if (formattedHeader.contains('"') || formattedHeader.contains(',') || formattedHeader.contains('\n')) {
//                // ʹ����ȷ��replace����
//                QString replacedHeader = formattedHeader;
//                replacedHeader.replace("\"", "\"\"");
//                formattedHeader = '"' + replacedHeader + '"';
//            }
//            headers << formattedHeader;
//        }
//        stream << headers.join(',') << "\n";
//    }
//
//    // д��������
//    QList<QStringList> rowGroups; // �洢��������
//    QStringList currentGroup;
//    int groupSize = 7; // ÿ��7���ֶ�
//
//    for (size_t i = 0; i < fieldOrder.size(); ++i) {
//        const auto& key = fieldOrder[i];
//        QString value = dataMap.count(key) ? dataMap.at(key) : "";
//
//        // ÿ7���ֶη�һ��
//        if (currentGroup.size() >= groupSize) {
//            rowGroups << currentGroup;
//            currentGroup.clear();
//        }
//
//        // FIX: �޸�replace���ô���
//        if (value.contains('"') || value.contains(',') || value.contains('\n')) {
//            // ʹ����ȷ��replace����
//            QString replacedValue = value;
//            replacedValue.replace("\"", "\"\"");
//            value = '"' + replacedValue + '"';
//        }
//
//        currentGroup << value;
//    }
//
//    // ������һ��
//    if (!currentGroup.isEmpty()) {
//        rowGroups << currentGroup;
//    }
//
//    // д��������ݣ�ÿ��һ���飩
//    for (const QStringList& group : rowGroups) {
//        stream << group.join(',') << "\r\n"; // ʹ��Windows���з�ȷ��������
//    }
//
//    // ȷ������д�����
//    stream.flush();
//    file.flush();
//    file.close();
//
//    // ����ļ��Ƿ�ɹ�д��
//    QFileInfo info(actualPath);
//    if (info.size() == 0) {
//        machineLog->write("CSV����ʧ�ܣ��ļ�Ϊ�գ�" + actualPath, Normal);
//    }
//    else {
//        machineLog->write("CSV����ɹ���" + actualPath, Normal);
//    }
//}

Q_GLOBAL_STATIC(QNetworkAccessManager, globalManager)
//�׶���mes��վ����
MesCheckInResponse_ED rs_FunctionTool::mes_check_in_ED(const QString& baseUrl, const MesCheckInRequest_ED& req) {
    QNetworkRequest netRequest;

    // ��������URL
    QUrl requestUrl(baseUrl + "/api/product/checkRoute");
    if (!requestUrl.isValid()) {
        return { false, "��Ч��URL��ʽ: " + baseUrl };
    }

    // ================== ������ѯ���� ==================
    QUrlQuery query;
    // �����ֶ�
    query.addQueryItem("sn", req.sn);                        // ������
    query.addQueryItem("workPlaceCode", req.workPlaceCode);  // ����վ��
    query.addQueryItem("productSn", req.productSn);          // ��Ʒ��

    // ��ѡ�ֶ�
    if (!req.laneId.isEmpty()) query.addQueryItem("laneId", req.laneId);            // ���
    if (!req.checkAllRef.isEmpty()) query.addQueryItem("checkAllRef", req.checkAllRef); // �Ƿ���С��

    // ����ѯ�������ӵ�URL
    requestUrl.setQuery(query);
    netRequest.setUrl(requestUrl);

    // ================== ����GET���� ==================
    QNetworkReply* reply = globalManager->get(netRequest);

    // ================== ��ʱ���� ==================
    QEventLoop loop;
    QTimer timeoutTimer;
    const int timeoutMs = 5000;  // 5�볬ʱ

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("ED_MES����ʱ: " + requestUrl.toString(), Mes);
        if (reply->isRunning()) reply->abort();
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== ��Ӧ���� ==================
    MesCheckInResponse_ED resp;
    bool isTimeout = !timeoutTimer.isActive();

    if (isTimeout) {
        return { false, "����ʱ��5������Ӧ��" };
    }

    if (reply->error() != QNetworkReply::NoError) {
        return { false, "�������: " + reply->errorString() };
    }

    // ����JSON��Ӧ
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    reply->deleteLater();

    if (parseError.error != QJsonParseError::NoError) {
        return { false, "JSON��������: " + parseError.errorString() };
    }

    // ��ȡ��Ӧ�ֶ�
    QJsonObject resObj = doc.object();
    resp.state = resObj.value("state").toBool(false);
    resp.description = resObj.value("description").toString();

    // ���state=false����������Ϣ�����Ĭ�ϴ�����ʾ
    if (!resp.state && resp.description.isEmpty()) {
        resp.description = "У��ʧ�ܣ�����ϸ������Ϣ";
    }

    // ��¼��Ӧ��־
    QString logMsg = QString("ED_MES��Ӧ - SN: %1, ״̬: %2, ����: %3")
        .arg(req.sn)
        .arg(resp.state ? "�ϸ�" : "���ϸ�")
        .arg(resp.description);
    machineLog->write(logMsg, Mes);

    return resp;
}

//����halconԭʼͼƬ�浽�ļ���
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

    pathName.replace('*', '@');         // �滻����*Ϊ@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + pathName, Normal);
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

//�Ӿ����ܵĴ������ʾ��ͼ
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
        //����ͼƬ
        //saveHalconJPG(img, photoPath);
        //����ͼƬ���ͺ��ļ���
        QString picPath = "";

        //HTuple hv_Width, hv_Height;
        //GetImageSize(img, &hv_Width, &hv_Height);

        // ��ȡ���ڴ�С
        int width = 600;
        int height = 500;
        // �� ���� pinLabel ����
        pinLabel label;

        //������ͼ��ϲ������ˣ�������Ϊ��ʾģ�� Erik 2025/3/21
        // ��ʾͼ��
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

// �滻 Halcon �� WriteImage
void rs_FunctionTool::saveQImage(const QImage& image, const QString& filePath,
    const QString& format, int quality)
{
    if (image.isNull()) {
        qWarning() << "Cannot save null image";
        return;
    }

    // ���Ŀ¼�Ƿ����
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir.path();
            return;
        }
    }

    // ����ͼ��
    if (!image.save(filePath, format.toUtf8().constData(), quality)) {
        machineLog->write("����ʧ��" + filePath, Normal);
    }
}

//����QImage���ͼƬ�浽�ļ���
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

    pathName.replace('*', '@');         // �滻����*Ϊ@
    QDir photoDir(pathName);


    if (!photoDir.exists()) {
        if (!photoDir.mkpath(".")) {
            machineLog->write("�޷���������洢�ļ��У�:" + pathName, Normal);
            return -1;
        }
    }

    QString finalPath = pathName + "/" + fileName + ".jpg";

    try
    {
        //����ͼ
        saveQImage(img, finalPath, "JPG", 80);
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}


// MES��վ����
MesCheckInResponse_ED rs_FunctionTool::mes_full_check_in(const QString& baseUrl,const MesCheckInFullRequest_ED& req)
{
    QNetworkRequest netRequest;

    // ��������URL
    QUrl requestUrl(baseUrl + "/api/Inspection/visionInspection");
    if (!requestUrl.isValid()) {
        return { false, "��Ч��URL��ʽ: " + baseUrl };
    }
    netRequest.setUrl(requestUrl);

    // ��������ͷ
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ================== ����JSON���� ==================
    QJsonObject requestJson;

    // ��������ֶ�
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

    // ================== ����POST���� ==================
    QByteArray jsonData = QJsonDocument(requestJson).toJson();
    QNetworkReply* reply = globalManager->post(netRequest, jsonData);

    // ================== ��ʱ���� ==================
    QEventLoop loop;
    QTimer timeoutTimer;
    const int timeoutMs = 5000;  // 5�볬ʱ

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        machineLog->write("MES��վ����ʱ: " + requestUrl.toString(), Normal);
        if (reply->isRunning()) reply->abort();
        loop.quit();
        });

    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    loop.exec();

    // ================== ��Ӧ���� ==================
    MesCheckInResponse_ED resp;
    bool isTimeout = !timeoutTimer.isActive();

    if (isTimeout) {
        return { false, "����ʱ��5������Ӧ��" };
    }

    if (reply->error() != QNetworkReply::NoError) {
        return { false, "�������: " + reply->errorString() };
    }

    // ����JSON��Ӧ
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    reply->deleteLater();

    if (parseError.error != QJsonParseError::NoError) {
        return { false, "JSON��������: " + parseError.errorString() };
    }

    // ��ȡ��Ӧ�ֶ�
    QJsonObject resObj = doc.object();
    resp.state = resObj.value("state").toBool(false);
    resp.description = resObj.value("description").toString();

    // ���state=false����������Ϣ�����Ĭ�ϴ�����ʾ
    if (!resp.state && resp.description.isEmpty()) {
        resp.description = "��վʧ�ܣ�����ϸ������Ϣ";
    }

    // ��¼��Ӧ��־
    QString logMsg = QString("MES��վ��Ӧ - SN: %1, ״̬: %2, ����: %3")
        .arg(req.Sn)
        .arg(resp.state ? "�ɹ�" : "ʧ��")
        .arg(resp.description);
    machineLog->write(logMsg, Normal);

    return resp;
}

MesCheckInFullRequest_ED rs_FunctionTool::mapToMesRequest(const std::unordered_map<QString, QString>& resultMap) {
    MesCheckInFullRequest_ED req;

    // ʹ�ø���������ȡֵ���ṩĬ��ֵ
    auto getValue = [&](const QString& key, const QString& defaultValue = "") {
        auto it = resultMap.find(key);
        return (it != resultMap.end()) ? it->second : defaultValue;
    };

    // ����ӳ���ϵ
    //req.WorkPlaceCode = getValue("name");
    req.WorkOrderCode = getValue("����");
    req.User = getValue("������");
    req.Sn = getValue("SN");
    req.Result = getValue("���ս��");
    req.ProductCode = getValue("��Ʒ��");

    req.OtherCode1 = getValue("������1");
    req.OtherCode2 = getValue("������2");
    req.OtherCode3 = getValue("������3");
    req.OtherCode4 = getValue("������4");
    req.Result_2D = getValue("2D���");
    req.Result_3D = getValue("3D���");
    req.Screw1Inspection = getValue("��˿���1");
    req.Screw2Inspection = getValue("��˿���2");
    req.Screw3Inspection = getValue("��˿���3");
    req.Screw4Inspection = getValue("��˿���4");
    req.Result_LabelCharacters = getValue("��ǩ�ַ����");
    req.Result_SnapFit = getValue("���ۼ��");
    req.WaterproofBreathableInspection = getValue("��ˮ͸��Ĥ���");
    req.SealantInspection = getValue("�ܷ⽺���");
    req.Result_CircularDistance1 = getValue("Բ�����1");
    req.Result_CircularDistance2 = getValue("Բ�����2");
    req.Result_CircularDistance3 = getValue("Բ�����3");
    req.Result_CircularDistance4 = getValue("Բ�����4");
    req.PictureUrl = getValue("ͼƬ·��");

    return req;
}

// ���������Ϣ����ʱû���ϣ����豸��Ϣ����һ���ˣ�
void rs_FunctionTool::saveCameraMess(std::unordered_map<QString, std::string>& cameraName)
{
    JsonConfigManager config("camera_data.json");
    // д�������JSON
    for (const auto& camera : cameraName)
    {
        config.writeParam(camera.first, QString::fromStdString(camera.second));
    }
}

// ��ȡ�����Ϣ����ʱû���ϣ����豸��Ϣ����һ���ˣ�
void rs_FunctionTool::readCameraMess(std::unordered_map<QString, std::string>& cameraName)
{
    JsonConfigManager config("camera_data.json");

    // ��JSON��ȡ����ֵ����Ĭ��ֵ��
    for (const auto& name : LS_CameraName::allCamera)
    {
        QString cameraID = config.readString(name, "");
        if (!cameraID.isEmpty())
        {
            cameraName[name] = cameraID.toStdString();
        }
    }
}