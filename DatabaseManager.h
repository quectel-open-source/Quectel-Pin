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
    const int TARGET_DB_VERSION = 1; // 当前目标版本

    QStringList getUserTableNames();
    bool clearTableData(const QString& tableName);
    QSqlDatabase getDB()
    {
        return m_db;
    };
    //动态创建表格
    bool addTable(QString date);
    //获取数据库表是否存在
    bool tableExists(const QString& tableName);
    //获取产品数量统计表
    QStringList getProductTableNames();
    bool deleteTable(const QString& tableName);
    bool deleteTables(const QStringList& tables);
public:
    //时间数组
    std::vector<QString> timeName;


private:
    QSqlDatabase m_db;

};
