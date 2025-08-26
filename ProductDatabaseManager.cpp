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

//��ȡ���б��
QStringList ProductDatabaseManager::getTableNames() {
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // ��ȡ�����û�����
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
            "`ʱ��` TEXT NOT NULL, "
            "`������` DOUBLE, "
            "`OK` DOUBLE,"
            "`���� OK` DOUBLE,"
            "`2D NG` DOUBLE, "
            "`3D NG` DOUBLE, "
            "`NG��` TEXT NOT NULL"
            ")");
        if (!rtn)
        {
            qDebug() << "pro Create departments failed";
            return false;
        }

        //QSqlQuery query;
        //query.prepare("INSERT INTO " + date + "("
        //    "`ʱ��`,"
        //    "`������`,"
        //    "`OK`,"
        //    "`���� OK`,"
        //    "`2D NG`,"
        //    "`3D NG`"
        //    ") VALUES (?, ?, ?, ?, ?)");
        //if (!query.exec()) {
        //    machineLog->write(QString("�������ݼ����ʧ��: %1").arg(query.lastError().text()), Normal);
        //    //m_dbManager.rollbackTransaction(); // �ع�����
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

//��ȡ��Ҫ��ʾ�������û���
QStringList ProductDatabaseManager::getUserTableNames()
{
    QStringList tables;
    if (m_db.isOpen()) {
        tables = m_db.tables(QSql::Tables); // ��ȡ�����û�����
    }

    // �ų�ϵͳ��Ͱ汾��
    QStringList excludeTables = { "sqlite_sequence", "sqlite_master" };
    QStringList filteredTables;
    for (const QString& table : tables) {
        if (!excludeTables.contains(table, Qt::CaseInsensitive)) {
            filteredTables.append(table);
        }
    }
    return filteredTables;
}

//����������
bool ProductDatabaseManager::clearTableData(const QString& tableName) {
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

//��ȡ���ݿ���Ƿ����
bool ProductDatabaseManager::tableExists(const QString& tableName) {
    // ��ȡ���ݿ����б�񣨺�ϵͳ��
    QStringList tables = m_db.tables(QSql::Tables);

    // ����SQLite�Ĵ�Сд��������
    return tables.contains(tableName, Qt::CaseInsensitive);
}