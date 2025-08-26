#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject* parent = nullptr);
    bool initializeDatabase();
    QStringList getTableNames();
    bool createVersionTable();
    int getCurrentDbVersion();
    bool setDbVersion(int version);
    bool upgradeToVersion(int targetVersion);
    const int TARGET_DB_VERSION = 1; // ��ǰĿ��汾

    QStringList getUserTableNames();
    bool clearTableData(const QString& tableName);
    QSqlDatabase getDB()
    {
        return m_db;
    };
    //��̬�������
    bool addTable(QString date);
    //��ȡ���ݿ���Ƿ����
    bool tableExists(const QString& tableName);
    //��ȡ��Ʒ����ͳ�Ʊ�
    QStringList getProductTableNames();
    bool deleteTable(const QString& tableName);
    bool deleteTables(const QStringList& tables);
public:
    //ʱ������
    std::vector<QString> timeName;


private:
    QSqlDatabase m_db;

};
