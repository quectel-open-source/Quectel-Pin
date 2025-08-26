#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QSqlDatabase>

class ProductDatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit ProductDatabaseManager(QObject* parent = nullptr);
    bool initializeDatabase();
    QStringList getTableNames();
    bool addTable(QString date);

    QStringList getUserTableNames();
    bool clearTableData(const QString& tableName);
    QSqlDatabase getDB()
    {
        return m_db;
    };
    //获取数据库表是否存在
    bool tableExists(const QString& tableName);
private:
    QSqlDatabase m_db;

};
