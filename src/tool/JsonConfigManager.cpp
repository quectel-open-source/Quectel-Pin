#include "JsonConfigManager.h"

JsonConfigManager::JsonConfigManager(const QString& filename, QObject* parent)
    : QObject(parent),
    m_filePath(
        QCoreApplication::applicationDirPath() + "/" +
        (filename.endsWith(".json") ? filename : filename + ".json")
    )
{
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
}

// д��ӿ�ʵ��
bool JsonConfigManager::writeParam(const QString& key, const QString& value)
{
    return writeJson(key, value);
}

bool JsonConfigManager::writeParam(const QString& key, int value)
{
    return writeJson(key, value);
}

bool JsonConfigManager::writeParam(const QString& key, double value)
{
    return writeJson(key, value);
}

// ��ȡ�ӿ�ʵ��
QString JsonConfigManager::readString(const QString& key, const QString& defaultValue) const
{
    return readJson(key, defaultValue).toString();
}

int JsonConfigManager::readInt(const QString& key, int defaultValue) const
{
    return readJson(key, defaultValue).toInt();
}

double JsonConfigManager::readDouble(const QString& key, double defaultValue) const
{
    return readJson(key, defaultValue).toDouble();
}

// ˽�з���ʵ��
template<typename T>
bool JsonConfigManager::writeJson(const QString& key, const T& value)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open config file:" << file.errorString();
        return false;
    }

    // ��ȡ��������
    QJsonObject jsonObj;
    if (file.size() > 0) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        jsonObj = doc.object();
    }

    // ����ֵ
    jsonObj[key] = QJsonValue(value);

    // д���ļ�
    file.resize(0);
    file.write(QJsonDocument(jsonObj).toJson());
    file.close();
    return true;
}

QVariant JsonConfigManager::readJson(const QString& key, const QVariant& defaultValue) const
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Config file not found, using default value";
        return defaultValue;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObj = doc.object();

    if (!jsonObj.contains(key)) {
        qWarning() << "Key" << key << "not found in config";
        return defaultValue;
    }

    return jsonObj[key].toVariant();
}


QJsonObject JsonConfigManager::readAllJsonObject() const
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Config file not found:" << m_filePath;
        return QJsonObject();
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString()
            << "at offset" << parseError.offset;
        return QJsonObject();
    }

    if (!doc.isObject()) {
        qWarning() << "JSON root is not an object";
        return QJsonObject();
    }

    return doc.object();
}

// ͨ�ö�ȡ������ģ�壩
template<typename ValueType>
bool JsonConfigManager::readAllToMap(std::unordered_map<QString, ValueType>& result,
    std::function<ValueType(const QJsonValue&)> converter) const
{
    result.clear();
    QJsonObject jsonObj = readAllJsonObject();

    if (jsonObj.isEmpty()) {
        return false;
    }

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        result[it.key()] = converter(it.value());
    }

    return !result.empty();
}

bool JsonConfigManager::clearAll()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Failed to clear config file:" << file.errorString();
        return false;
    }

    // д���JSON����
    QJsonObject emptyObj;
    file.write(QJsonDocument(emptyObj).toJson());
    file.close();
    return true;
}

// ��ʽʵ����,�����м�ֵ��֧��int��
template bool JsonConfigManager::readAllToMap<int>(
    std::unordered_map<QString, int>&,
    std::function<int(const QJsonValue&)>
    ) const;

// ��ʽʵ����ģ��
template bool JsonConfigManager::writeJson<QString>(const QString&, const QString&);
template bool JsonConfigManager::writeJson<int>(const QString&, const int&);
template bool JsonConfigManager::writeJson<double>(const QString&, const double&);