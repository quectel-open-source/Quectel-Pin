#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError> 
#include "QC_Log.h"

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("my_database.db");

    // ʱ����Ⱥ�
    // ���ɴ� 08:00 �� 24:00 ��ʱ���
    for (int hour = 8; hour < 24; ++hour) {
        QString start = QString("%1:00 - %2:00").arg(hour, 2, 10, QChar('0')).arg(hour + 1, 2, 10, QChar('0'));
        timeName.push_back(start);
    }
    // ���ɴ� 00:00 �� 08:00 ��ʱ���
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

    // �����汾����������ڣ�
    if (!getTableNames().contains("db_version")) {
        if (!createVersionTable() || !setDbVersion(1)) {
            return false;
        }
    }

    // ִ�а汾����
    const int currentVersion = getCurrentDbVersion();
    if (currentVersion <= TARGET_DB_VERSION) {
        if (!upgradeToVersion(TARGET_DB_VERSION)) {
            return false;
        }
    }

    return true;
}

//��ȡ���б��
QStringList DatabaseManager::getTableNames() {
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // ��ȡ�����û�����
    }
    return tables;
}


// �����汾������ط���
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
    // ʹ�� INSERT OR REPLACE ȷ���״β���
    query.prepare("INSERT OR REPLACE INTO db_version (version) VALUES (?)");
    query.addBindValue(version);
    return query.exec();
}

// ������������
bool DatabaseManager::upgradeToVersion(int targetVersion) {
    m_db.transaction();
    try {
        for (int v = getCurrentDbVersion(); v <= targetVersion; ++v) {
            if (v == 1)
            {
                // ��ʼ�汾���Ѵ��ڣ�
                // ��Ʒ����
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `��Ʒ����` ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL, "
                    "`���` TEXT NOT NULL, "
                    "`2D���` TEXT NOT NULL, "
                    "`3D���` TEXT NOT NULL, "
                    "`SN�ͺ�` TEXT NOT NULL, "
                    "`����ʱ��` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`����ʱ��` DATETIME,"
                    "`ͼƬ·��` TEXT NOT NULL, "
                    "`����` TEXT NOT NULL "
                    ")").exec()) {
                    qDebug() << "product Create departments failed";
                    return false;
                }
                //2Dʶ����
                //QSqlQuery table2D;
                //bool rtn = table2D.exec("CREATE TABLE IF NOT EXISTS Pin2D_Result ("
                //    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                //    "name TEXT NOT NULL, "
                //    "`SN�ͺ�` TEXT NOT NULL, "
                //    "X DOUBLE,"
                //    "Y DOUBLE, "
                //    "`��Ի�׼��X������� mm` DOUBLE, "
                //    "`��Ի�׼��Y������� mm` DOUBLE,"
                //    "`��Ի�׼��X����ƫ��ֵ mm` DOUBLE,"
                //    "`��Ի�׼��Y����ƫ��ֵ mm` DOUBLE, "
                //    "`���` TEXT NOT NULL,"
                //    "`����ʱ��` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                //    "`����ʱ��` DATETIME"
                //    ")");
                //if (!rtn)
                //{
                //    qDebug() << "2d Create departments failed";
                //    return false;
                //}
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS Pin2D_Result ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "`SN�ͺ�` TEXT NOT NULL, "
                    "X DOUBLE,"
                    "Y DOUBLE, "
                    "`��Ի�׼��X������� mm` DOUBLE, "
                    "`��Ի�׼��Y������� mm` DOUBLE,"
                    "`��Ի�׼��X����ƫ��ֵ mm` DOUBLE,"
                    "`��Ի�׼��Y����ƫ��ֵ mm` DOUBLE, "
                    "`���` TEXT NOT NULL,"
                    "`����ʱ��` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`����ʱ��` DATETIME"
                    ")").exec()) {
                    qDebug() << "2d Create departments failed";
                    return false;
                }
                // 3Dʶ����
                if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `Pin3D_Result` ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL, "
                    "`SN�ͺ�` TEXT NOT NULL, "
                    "`�߶�` DOUBLE,"
                    "`���` TEXT NOT NULL, "
                    "`����ʱ��` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`����ʱ��` DATETIME"
                    ")").exec()) {
                    qDebug() << "3d Create departments failed";
                    return false;
                }

            }
            else if (v == 2)
            {
                // V2 �汾 ����3Dʶ������
                //// ��employees�����email�ֶ�
                //if (!QSqlQuery("ALTER TABLE employees ADD COLUMN email TEXT").exec()) {
                //    throw std::runtime_error("Add email column failed");
                //}
                // �����±�
           /*     if (!QSqlQuery("CREATE TABLE IF NOT EXISTS `Pin3D_Result` ("
                    "id INTEGER PRIMARY KEY,"
                    "name TEXT UNIQUE NOT NULL, "
                    "`�߶�` DOUBLE,"
                    "`���` TEXT NOT NULL, "
                    "`����ʱ��` DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "`����ʱ��` DATETIME"
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

//��ȡ��Ҫ��ʾ�������û���
QStringList DatabaseManager::getUserTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // ��ȡ�����û�����
    }

    //// �ų�ϵͳ��Ͱ汾��
    //QStringList excludeTables = { "sqlite_sequence", "sqlite_master", "db_version" };
    //QStringList filteredTables;
    //for (const QString& table : tables) {
    //    if (!excludeTables.contains(table, Qt::CaseInsensitive)) {
    //        filteredTables.append(table);
    //    }
    //}
    //ֻȡҪ�ı�
    QStringList icludeTables = {"Pin2D_Result","Pin3D_Result","��Ʒ����"};
    QStringList filteredTables;
    for (const QString& table : tables) {
        if (icludeTables.contains(table, Qt::CaseInsensitive)) {
            filteredTables.append(table);
        }
    }
    return filteredTables;
}

//����������
bool DatabaseManager::clearTableData(const QString& tableName) {
    if (!m_db.isOpen()) {
        qDebug() << "Database not open";
        return false;
    }

    QSqlQuery query;
    query.prepare(QString("DELETE FROM `%1`").arg(tableName)); // ɾ����������

    if (!query.exec()) {
        qDebug() << "Clear table failed:" << query.lastError().text();
        return false;
    }

    // ������������������ѡ��
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
            "`ʱ��` TEXT NOT NULL, "
            "`������` INT, "
            "`OK` INT,"
            "`NG` INT,"
            "`2D NG` INT, "
            "`3D NG` INT, "
            "`NG��` TEXT NOT NULL"
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
                "`ʱ��`,"
                "`������`,"
                "`OK`,"
                "`NG`,"
                "`2D NG`,"
                "`3D NG`,"
                "`NG��`"
                ") VALUES (?, ?, ?, ?, ?, ?, ?)");

            if (i == 0)
            {
                // ȫ������
                query.addBindValue("ȫ�� | 8:00 - 8:00");
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
                machineLog->write(QString("�������ݼ����ʧ��: %1").arg(query.lastError().text()), Normal);
                //m_dbManager.rollbackTransaction(); // �ع�����
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

//��ȡ���ݿ���Ƿ����
bool DatabaseManager::tableExists(const QString& tableName) {
    // ��ȡ���ݿ����б�񣨺�ϵͳ��
    QStringList tables = m_db.tables(QSql::Tables);

    // ����SQLite�Ĵ�Сд��������
    return tables.contains(tableName, Qt::CaseInsensitive);
}

//��ȡ��Ʒ����ͳ�Ʊ�
QStringList DatabaseManager::getProductTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // ��ȡ�����û�����
    }

    // �ų�ϵͳ��Ͱ汾��
    QStringList excludeTables = { "sqlite_sequence", "sqlite_master", "db_version","Pin2D_Result","Pin3D_Result","��Ʒ����" };
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