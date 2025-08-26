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
    // 设置窗口标志，添加最大化按钮
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    //初始化表格
    initTableWidget(ui.tableWidget);
    //读取文件
    readRobotProject();
}

qg_RobotProject_ED::~qg_RobotProject_ED()
{}

// 初始化表格函数
void qg_RobotProject_ED::initTableWidget(QTableWidget* tableWidget)
{
    //隐藏行头
    tableWidget->verticalHeader()->setVisible(false);
    // 设置行列数
    tableWidget->setRowCount(9);
    tableWidget->setColumnCount(2);

    // 设置表头
    QStringList headers;
    headers << "机器人方案" << "配方方案";
    tableWidget->setHorizontalHeaderLabels(headers);

    // 设置表头样式
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列固定宽度
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 第二列自动拉伸

    // 设置第一列宽度（紧凑）
    tableWidget->setColumnWidth(0, 80); // 根据内容调整宽度
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter); // 居中对齐

    // 填充数据 - 机器人方案列(1-9)
    for (int row = 0; row < 9; ++row) {
        // 第一列：机器人方案(1-9)
        QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(row + 1));
        item1->setTextAlignment(Qt::AlignCenter); // 居中对齐
        //不可编辑
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(row, 0, item1);

        // 第二列：配方(初始为空)
        QTableWidgetItem* item2 = new QTableWidgetItem("");
        item2->setTextAlignment(Qt::AlignCenter); // 居中对齐
        tableWidget->setItem(row, 1, item2);
    }

    tableWidget->setAlternatingRowColors(true); // 交替行颜色
}

//保存
void qg_RobotProject_ED::on_pushButton_save_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "确定要保存吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    // 先清除之前保存的记录
    LSM->m_formulaToRobot.clear();

    // 用于检测重复配方
    QSet<QString> allFormulas;

    // 遍历表格的每一行
    for (int row = 0; row < ui.tableWidget->rowCount(); ++row)
    {
        // 获取配方列的项
        QTableWidgetItem* formulaItem = ui.tableWidget->item(row, 1);

        if (!formulaItem) continue;

        QString formulaText = formulaItem->text().trimmed();
        if (formulaText.isEmpty()) continue;

        // 获取机器人方案列的项
        QTableWidgetItem* robotItem = ui.tableWidget->item(row, 0);
        if (!robotItem) continue;

        int robotNumber = robotItem->text().toInt();

        // 分割配方字符串（支持中文和英文分号）
        QStringList formulas = formulaText.split(QRegularExpression("[;；]"), Qt::SkipEmptyParts);

        for (QString& formula : formulas) {
            formula = formula.trimmed();
            if (formula.isEmpty()) continue;

            // 检查配方是否重复
            if (allFormulas.contains(formula)) {
                QMessageBox::warning(this, "警告", QString("配方 '%1' 在多个机器人方案中重复出现").arg(formula));
                return;
            }
            allFormulas.insert(formula);

            // 添加到映射中
            LSM->m_formulaToRobot[formula] = robotNumber;
        }
    }

    //保存到本地
    saveRobotProject();
    QMessageBox::information(this, "成功", "保存成功！");
}

//保存参数到本地
void qg_RobotProject_ED::saveRobotProject()
{
    if (LSM->m_formulaToRobot.empty())
        return;

    JsonConfigManager config("RobotToFormula.json");

    // 清空文件内容
    if (!config.clearAll()) {
        QMessageBox::warning(this, "错误", "清空配置文件失败！");
        return;
    }

    // 创建反转映射：机器人方案 -> 配方列表
    std::unordered_map<int, QStringList> robotToFormulas;
    for (const auto& pair : LSM->m_formulaToRobot) {
        robotToFormulas[pair.second].append(pair.first);
    }

    // 写入参数到JSON
    for (const auto& pair : robotToFormulas) {
        // 将配方列表用英文分号连接
        QString formulaString = pair.second.join(";");
        config.writeParam(QString::number(pair.first), formulaString);
    }
}

//从本地读取参数
void qg_RobotProject_ED::readRobotProject()
{
    JsonConfigManager config("RobotToFormula.json");

    // 清空现有映射
    LSM->m_formulaToRobot.clear();

    // 清空表格中的配方列
    for (int row = 0; row < ui.tableWidget->rowCount(); ++row) {
        QTableWidgetItem* formulaItem = ui.tableWidget->item(row, 1);
        if (formulaItem) {
            formulaItem->setText("");
        }
    }

    // 读取整个JSON对象
    QJsonObject jsonObj = config.readAllJsonObject();

    // 创建临时映射：机器人方案 -> 配方列表
    std::unordered_map<int, QStringList> robotToFormulas;

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString robotStr = it.key();

        bool ok;
        int robotNumber = robotStr.toInt(&ok);
        if (!ok || robotNumber < 1 || robotNumber > 9) continue;

        // 检查值是否为字符串
        if (!it.value().isString()) continue;

        QString formulaStr = it.value().toString();

        // 分割配方字符串
        QStringList formulas = formulaStr.split(QRegularExpression("[;；]"), Qt::SkipEmptyParts);

        // 更新临时映射
        for (const QString& formula : formulas) {
            QString trimmedFormula = formula.trimmed();
            if (!trimmedFormula.isEmpty()) {
                robotToFormulas[robotNumber].append(trimmedFormula);
            }
        }
    }

    // 更新全局映射和表格
    for (const auto& pair : robotToFormulas) {
        int robotNumber = pair.first;
        const QStringList& formulas = pair.second;

        // 更新全局映射
        for (const QString& formula : formulas) {
            LSM->m_formulaToRobot[formula] = robotNumber;
        }

        // 更新表格显示
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

