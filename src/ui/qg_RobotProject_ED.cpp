#include "qg_RobotProject_ED.h"
#include <QHeaderView>
#include "rs_motion.h"
#include "JsonConfigManager.h"
#include <QSet>
#include <QRegularExpression>

qg_RobotProject_ED::qg_RobotProject_ED(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    // ���ô��ڱ�־�������󻯰�ť
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    //��ʼ�����
    initTableWidget(ui.tableWidget);
    //��ȡ�ļ�
    readRobotProject();
}

qg_RobotProject_ED::~qg_RobotProject_ED()
{}

// ��ʼ�������
void qg_RobotProject_ED::initTableWidget(QTableWidget* tableWidget)
{
    //������ͷ
    tableWidget->verticalHeader()->setVisible(false);
    // ����������
    tableWidget->setRowCount(9);
    tableWidget->setColumnCount(2);

    // ���ñ�ͷ
    QStringList headers;
    headers << "�����˷���" << "�䷽����";
    tableWidget->setHorizontalHeaderLabels(headers);

    // ���ñ�ͷ��ʽ
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // ��һ�й̶����
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // �ڶ����Զ�����

    // ���õ�һ�п�ȣ����գ�
    tableWidget->setColumnWidth(0, 80); // �������ݵ������
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter); // ���ж���

    // ������� - �����˷�����(1-9)
    for (int row = 0; row < 9; ++row) {
        // ��һ�У������˷���(1-9)
        QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(row + 1));
        item1->setTextAlignment(Qt::AlignCenter); // ���ж���
        //���ɱ༭
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(row, 0, item1);

        // �ڶ��У��䷽(��ʼΪ��)
        QTableWidgetItem* item2 = new QTableWidgetItem("");
        item2->setTextAlignment(Qt::AlignCenter); // ���ж���
        tableWidget->setItem(row, 1, item2);
    }

    tableWidget->setAlternatingRowColors(true); // ��������ɫ
}

//����
void qg_RobotProject_ED::on_pushButton_save_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "ȷ��", "ȷ��Ҫ������",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    // �����֮ǰ����ļ�¼
    LSM->m_formulaToRobot.clear();

    // ���ڼ���ظ��䷽
    QSet<QString> allFormulas;

    // ��������ÿһ��
    for (int row = 0; row < ui.tableWidget->rowCount(); ++row)
    {
        // ��ȡ�䷽�е���
        QTableWidgetItem* formulaItem = ui.tableWidget->item(row, 1);

        if (!formulaItem) continue;

        QString formulaText = formulaItem->text().trimmed();
        if (formulaText.isEmpty()) continue;

        // ��ȡ�����˷����е���
        QTableWidgetItem* robotItem = ui.tableWidget->item(row, 0);
        if (!robotItem) continue;

        int robotNumber = robotItem->text().toInt();

        // �ָ��䷽�ַ�����֧�����ĺ�Ӣ�ķֺţ�
        QStringList formulas = formulaText.split(QRegularExpression("[;��]"), Qt::SkipEmptyParts);

        for (QString& formula : formulas) {
            formula = formula.trimmed();
            if (formula.isEmpty()) continue;

            // ����䷽�Ƿ��ظ�
            if (allFormulas.contains(formula)) {
                QMessageBox::warning(this, "����", QString("�䷽ '%1' �ڶ�������˷������ظ�����").arg(formula));
                return;
            }
            allFormulas.insert(formula);

            // ��ӵ�ӳ����
            LSM->m_formulaToRobot[formula] = robotNumber;
        }
    }

    //���浽����
    saveRobotProject();
    QMessageBox::information(this, "�ɹ�", "����ɹ���");
}

//�������������
void qg_RobotProject_ED::saveRobotProject()
{
    if (LSM->m_formulaToRobot.empty())
        return;

    JsonConfigManager config("RobotToFormula.json");

    // ����ļ�����
    if (!config.clearAll()) {
        QMessageBox::warning(this, "����", "��������ļ�ʧ�ܣ�");
        return;
    }

    // ������תӳ�䣺�����˷��� -> �䷽�б�
    std::unordered_map<int, QStringList> robotToFormulas;
    for (const auto& pair : LSM->m_formulaToRobot) {
        robotToFormulas[pair.second].append(pair.first);
    }

    // д�������JSON
    for (const auto& pair : robotToFormulas) {
        // ���䷽�б���Ӣ�ķֺ�����
        QString formulaString = pair.second.join(";");
        config.writeParam(QString::number(pair.first), formulaString);
    }
}

//�ӱ��ض�ȡ����
void qg_RobotProject_ED::readRobotProject()
{
    JsonConfigManager config("RobotToFormula.json");

    // �������ӳ��
    LSM->m_formulaToRobot.clear();

    // ��ձ���е��䷽��
    for (int row = 0; row < ui.tableWidget->rowCount(); ++row) {
        QTableWidgetItem* formulaItem = ui.tableWidget->item(row, 1);
        if (formulaItem) {
            formulaItem->setText("");
        }
    }

    // ��ȡ����JSON����
    QJsonObject jsonObj = config.readAllJsonObject();

    // ������ʱӳ�䣺�����˷��� -> �䷽�б�
    std::unordered_map<int, QStringList> robotToFormulas;

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString robotStr = it.key();

        bool ok;
        int robotNumber = robotStr.toInt(&ok);
        if (!ok || robotNumber < 1 || robotNumber > 9) continue;

        // ���ֵ�Ƿ�Ϊ�ַ���
        if (!it.value().isString()) continue;

        QString formulaStr = it.value().toString();

        // �ָ��䷽�ַ���
        QStringList formulas = formulaStr.split(QRegularExpression("[;��]"), Qt::SkipEmptyParts);

        // ������ʱӳ��
        for (const QString& formula : formulas) {
            QString trimmedFormula = formula.trimmed();
            if (!trimmedFormula.isEmpty()) {
                robotToFormulas[robotNumber].append(trimmedFormula);
            }
        }
    }

    // ����ȫ��ӳ��ͱ��
    for (const auto& pair : robotToFormulas) {
        int robotNumber = pair.first;
        const QStringList& formulas = pair.second;

        // ����ȫ��ӳ��
        for (const QString& formula : formulas) {
            LSM->m_formulaToRobot[formula] = robotNumber;
        }

        // ���±����ʾ
        int row = robotNumber - 1;
        if (row >= 0 && row < ui.tableWidget->rowCount()) {
            QTableWidgetItem* formulaItem = ui.tableWidget->item(row, 1);
            if (formulaItem) {
                formulaItem->setText(formulas.join(";"));
            }
        }
    }
}

void qg_RobotProject_ED::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);
}

