#ifndef JSONCONFIGMANAGER_H
#define JSONCONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <unordered_map>

class JsonConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit JsonConfigManager(const QString& filename, QObject* parent = nullptr);

    // д��ӿ�
    bool writeParam(const QString& key, const QString& value);
    bool writeParam(const QString& key, int value);
    bool writeParam(const QString& key, double value);

    // ��ȡ�ӿ�
    QString readString(const QString& key, const QString& defaultValue = "") const;
    int readInt(const QString& key, int defaultValue = 0) const;
    double readDouble(const QString& key, double defaultValue = 0.0) const;
    // ��ȡ����JSON����
    QJsonObject readAllJsonObject() const; 
    // ͨ�÷�������ȡ����JSON�� unordered_map<QString, int>
    template<typename ValueType>
    bool readAllToMap(std::unordered_map<QString, ValueType>& result,
        std::function<ValueType(const QJsonValue&)> converter) const;
    // ��������ļ�����
    bool clearAll(); 

private:
    const QString m_filePath;

    template<typename T>
    bool writeJson(const QString& key, const T& value);

    QVariant readJson(const QString& key, const QVariant& defaultValue) const;
};

#endif // JSONCONFIGMANAGER_H