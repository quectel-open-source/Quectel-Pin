#include "ProductDatabaseManager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError> 

ProductDatabaseManager::ProductDatabaseManager(QObject* parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE", "connection2");
    m_db.setDatabaseName("my_ProductDatabase.db");
}

bool ProductDatabaseManager::initializeDatabase() {
    if (!m_db.open()) {
        qDebug() << "Database Error:" << m_db.lastError().text();
        return false;
    }

    return true;
}

//获取所有表格
QStringList ProductDatabaseManager::getTableNames() {
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // 获取所有用户表名
    }
    return tables;
}

bool ProductDatabaseManager::addTable(QString date) {
    m_db.transaction();
    try {
        QSqlQuery table2D;
        //"id INTEGER PRIMARY KEY AUTOINCREMENT, "
        bool rtn = table2D.exec("CREATE TABLE IF NOT EXISTS "+ date +" ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "`时间` TEXT NOT NULL, "
            "`总数量` DOUBLE, "
            "`OK` DOUBLE,"
            "`复判 OK` DOUBLE,"
            "`2D NG` DOUBLE, "
            "`3D NG` DOUBLE, "
            "`NG率` TEXT NOT NULL"
            ")");
        if (!rtn)
        {
            qDebug() << "pro Create departments failed";
            return false;
        }

        //QSqlQuery query;
        //query.prepare("INSERT INTO " + date + "("
        //    "`时间`,"
        //    "`总数量`,"
        //    "`OK`,"
        //    "`复判 OK`,"
        //    "`2D NG`,"
        //    "`3D NG`"
        //    ") VALUES (?, ?, ?, ?, ?)");
        //if (!query.exec()) {
        //    machineLog->write(QString("产量数据集添加失败: %1").arg(query.lastError().text()), Normal);
        //    //m_dbManager.rollbackTransaction(); // 回滚事务
        //    return;
        //}

        return m_db.commit();
    }
    catch (const std::exception& e) {
        m_db.rollback();
        qDebug() << "pro Upgrade failed:" << e.what();
        return false;
    }
}

//获取需要显示出来的用户表
QStringList ProductDatabaseManager::getUserTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // 获取所有用户表名
    }

    // 排除系统表和版本表
    QStringList excludeTables = { "sqlite_sequence", "sqlite_master" };
    QStringList filteredTables;
    for (const QString& table : tables) {
        if (!excludeTables.contains(table, Qt::CaseInsensitive)) {
            filteredTables.append(table);
        }
    }
    return filteredTables;
}

//清理表格内容
bool ProductDatabaseManager::clearTableData(const QString& tableName) {
    if (!m_db.isOpen()) {
        qDebug() << "Database not open";
        return false;
    }

    QSqlQuery query;
    query.prepare(QString("DELETE FROM `%1`").arg(tableName)); // 删除所有数据

    if (!query.exec()) {
        qDebug() << "Clear table failed:" << query.lastError().text();
        return false;
    }

    // 重置自增计数器（可选）
    query.exec(QString("DELETE FROM sqlite_sequence WHERE name='%1'").arg(tableName));

    return true;
}

//获取数据库表是否存在
bool ProductDatabaseManager::tableExists(const QString& tableName) {
    // 获取数据库所有表格（含系统表）
    QStringList tables = m_db.tables(QSql::Tables);

    // 处理SQLite的大小写敏感问题
    return tables.contains(tableName, Qt::CaseInsensitive);
}