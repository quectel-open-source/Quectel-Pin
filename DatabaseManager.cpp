#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError> 
#include "QC_Log.h"

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("my_database.db");

    // 时间分先后
    // 生成从 08:00 到 24:00 的时间段
    for (int hour = 8; hour < 24; ++hour) {
        QString start = QString("%1:00 - %2:00").arg(hour, 2, 10, QChar('0')).arg(hour + 1, 2, 10, QChar('0'));
        timeName.push_back(start);
    }
    // 生成从 00:00 到 08:00 的时间段
    for (int hour = 0; hour < 8; ++hour) {
        QString start = QString("%1:00 - %2:00").arg(hour, 2, 10, QChar('0')).arg(hour + 1, 2, 10, QChar('0'));
        timeName.push_back(start);
    }
}

bool DatabaseManager::initializeDatabase() {
    if (!m_db.open()) {
        qDebug() << "Database Error:" << m_db.lastError().text();
        return false;
    }

    // 创建版本表（如果不存在）
    if (!getTableNames().contains("db_version")) {
        if (!createVersionTable() || !setDbVersion(1)) {
            return false;
        }
    }

    // 执行版本升级
    const int currentVersion = getCurrentDbVersion();
    if (currentVersion <= TARGET_DB_VERSION) {
        if (!upgradeToVersion(TARGET_DB_VERSION)) {
            return false;
        }
    }

    return true;
}

//获取所有表格
QStringList DatabaseManager::getTableNames() {
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // 获取所有用户表名
    }
    return tables;
}


// 新增版本控制相关方法
bool DatabaseManager::createVersionTable() {
    QSqlQuery query;
    return query.exec("CREATE TABLE db_version ("
        "version INTEGER PRIMARY KEY)");
}

int DatabaseManager::getCurrentDbVersion() {
    QSqlQuery query("SELECT version FROM db_version LIMIT 1");
    int rtn = 0;
    if (query.next())
    {
        rtn = query.value(0).toInt();
    }
    return rtn;
}

bool DatabaseManager::setDbVersion(int version) {
    QSqlQuery query;
    // 使用 INSERT OR REPLACE 确保首次插入
    query.prepare("INSERT OR REPLACE INTO db_version (version) VALUES (?)");
    query.addBindValue(version);
    return query.exec();
}

// 核心升级方法
bool DatabaseManager::upgradeToVersion(int targetVersion) {
    m_db.transaction();
    try {
        for (int v = getCurrentDbVersion(); v <= targetVersion; ++v) {
            if (v == 1)
            {
                // 初始版本（已存在）
                // 产品数据
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `产品数据` ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL, "
                    "`结果` TEXT NOT NULL, "
                    "`2D结果` TEXT NOT NULL, "
                    "`3D结果` TEXT NOT NULL, "
                    "`SN型号` TEXT NOT NULL, "
                    "`创建时间` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`更新时间` DATETIME,"
                    "`图片路径` TEXT NOT NULL, "
                    "`复判` TEXT NOT NULL "
                    ")").exec()) {
                    qDebug() << "product Create departments failed";
                    return false;
                }
                //2D识别表格
                //QSqlQuery table2D;
                //bool rtn = table2D.exec("CREATE TABLE IF NOT EXISTS Pin2D_Result ("
                //    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                //    "name TEXT NOT NULL, "
                //    "`SN型号` TEXT NOT NULL, "
                //    "X DOUBLE,"
                //    "Y DOUBLE, "
                //    "`相对基准的X方向距离 mm` DOUBLE, "
                //    "`相对基准的Y方向距离 mm` DOUBLE,"
                //    "`相对基准的X方向偏差值 mm` DOUBLE,"
                //    "`相对基准的Y方向偏差值 mm` DOUBLE, "
                //    "`结果` TEXT NOT NULL,"
                //    "`创建时间` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                //    "`更新时间` DATETIME"
                //    ")");
                //if (!rtn)
                //{
                //    qDebug() << "2d Create departments failed";
                //    return false;
                //}
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS Pin2D_Result ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "`SN型号` TEXT NOT NULL, "
                    "X DOUBLE,"
                    "Y DOUBLE, "
                    "`相对基准的X方向距离 mm` DOUBLE, "
                    "`相对基准的Y方向距离 mm` DOUBLE,"
                    "`相对基准的X方向偏差值 mm` DOUBLE,"
                    "`相对基准的Y方向偏差值 mm` DOUBLE, "
                    "`结果` TEXT NOT NULL,"
                    "`创建时间` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`更新时间` DATETIME"
                    ")").exec()) {
                    qDebug() << "2d Create departments failed";
                    return false;
                }
                // 3D识别表格
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `Pin3D_Result` ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL, "
                    "`SN型号` TEXT NOT NULL, "
                    "`高度` DOUBLE,"
                    "`结果` TEXT NOT NULL, "
                    "`创建时间` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`更新时间` DATETIME"
                    ")").exec()) {
                    qDebug() << "3d Create departments failed";
                    return false;
                }

            }
            else if (v == 2)
            {
                // V2 版本 加上3D识别数据
                //// 给employees表添加email字段
                //if (!QSqlQuery("ALTER TABLE employees ADD COLUMN email TEXT").exec()) {
                //    throw std::runtime_error("Add email column failed");
                //}
                // 创建新表
           /*     if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `Pin3D_Result` ("
                    "id INTEGER PRIMARY KEY,"
                    "name TEXT UNIQUE NOT NULL, "
                    "`高度` DOUBLE,"
                    "`结果` TEXT NOT NULL, "
                    "`创建时间` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`更新时间` DATETIME"
                    ")").exec()) {
                    qDebug() << "3d Create departments failed";
                }*/
            }
            if (!setDbVersion(v)) {
                qDebug() << "Update version failed";
            }
        }
        return m_db.commit();
    }
    catch (const std::exception& e) {
        m_db.rollback();
        qDebug() << "Upgrade failed:" << e.what();
        return false;
    }
}

//获取需要显示出来的用户表
QStringList DatabaseManager::getUserTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // 获取所有用户表名
    }

    //// 排除系统表和版本表
    //QStringList excludeTables = { "sqlite_sequence", "sqlite_master", "db_version" };
    //QStringList filteredTables;
    //for (const QString& table : tables) {
    //    if (!excludeTables.contains(table, Qt::CaseInsensitive)) {
    //        filteredTables.append(table);
    //    }
    //}
    //只取要的表
    QStringList icludeTables = {"Pin2D_Result","Pin3D_Result","产品数据"};
    QStringList filteredTables;
    for (const QString& table : tables) {
        if (icludeTables.contains(table, Qt::CaseInsensitive)) {
            filteredTables.append(table);
        }
    }
    return filteredTables;
}

//清理表格内容
bool DatabaseManager::clearTableData(const QString& tableName) {
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

bool DatabaseManager::addTable(QString date) {
    m_db.transaction();
    try {
        QSqlQuery temp;
        //"id INTEGER PRIMARY KEY AUTOINCREMENT, "
        bool rtn = temp.exec("CREATE TABLE IF NOT EXISTS " + date + " ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "`时间` TEXT NOT NULL, "
            "`总数量` INT, "
            "`OK` INT,"
            "`NG` INT,"
            "`2D NG` INT, "
            "`3D NG` INT, "
            "`NG率` TEXT NOT NULL"
            ")");
        if (!rtn)
        {
            qDebug() << "pro Create departments failed";
            return false;
        }

        for (int i = 0; i <= 24; i++)
        {
            QSqlQuery query;
            query.prepare("INSERT INTO " + date + "("
                "`时间`,"
                "`总数量`,"
                "`OK`,"
                "`NG`,"
                "`2D NG`,"
                "`3D NG`,"
                "`NG率`"
                ") VALUES (?, ?, ?, ?, ?, ?, ?)");

            if (i == 0)
            {
                // 全天数据
                query.addBindValue("全天 | 8:00 - 8:00");
            }
            else
            {
                query.addBindValue(timeName[i - 1]);
            }

            query.addBindValue(0);
            query.addBindValue(0);
            query.addBindValue(0);
            query.addBindValue(0);
            query.addBindValue(0);
            query.addBindValue("0.00%");

            if (!query.exec()) {
                machineLog->write(QString("产量数据集添加失败: %1").arg(query.lastError().text()), Normal);
                //m_dbManager.rollbackTransaction(); // 回滚事务
                return false;
            }
        }
      

        return m_db.commit();
    }
    catch (const std::exception& e) {
        m_db.rollback();
        qDebug() << "pro Upgrade failed:" << e.what();
        return false;
    }
}

//获取数据库表是否存在
bool DatabaseManager::tableExists(const QString& tableName) {
    // 获取数据库所有表格（含系统表）
    QStringList tables = m_db.tables(QSql::Tables);

    // 处理SQLite的大小写敏感问题
    return tables.contains(tableName, Qt::CaseInsensitive);
}

//获取产品数量统计表
QStringList DatabaseManager::getProductTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // 获取所有用户表名
    }

    // 排除系统表和版本表
    QStringList excludeTables = { "sqlite_sequence", "sqlite_master", "db_version","Pin2D_Result","Pin3D_Result","产品数据" };
    QStringList filteredTables;
    for (const QString& table : tables) {
        if (!excludeTables.contains(table, Qt::CaseInsensitive)) {
            filteredTables.append(table);
        }
    }

    return filteredTables;
}

bool DatabaseManager::deleteTable(const QString& tableName)
{
    QSqlQuery query;
    return query.exec(QString("DROP TABLE IF EXISTS `%1`").arg(tableName));
}

bool DatabaseManager::deleteTables(const QStringList& tables)
{
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    foreach(const QString & table, tables) {
        QSqlQuery query;
        if (!query.exec(QString("DROP TABLE IF EXISTS `%1`").arg(table))) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}