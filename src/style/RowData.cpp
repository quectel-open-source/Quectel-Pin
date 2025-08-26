#include "RowData.h"


RowData::RowData(PointItemConfig config, QTableWidgetItem* item)
:m_config(config), m_item(item)
{
    editButton = new QPushButton("编辑");
    //btn_upMove = new QPushButton("上移");
    //btn_downMove = new QPushButton("下移");
    //btnRemove = new QPushButton("删除");

    // 为按钮设置自定义属性（行号）
    editButton->setProperty("row", QVariant(currentRow()));
    //btn_upMove->setProperty("row", QVariant(row));
    //btn_downMove->setProperty("row", QVariant(row));
    //btnRemove->setProperty("row", QVariant(row));
}

int RowData::currentRow(){
    return m_item->tableWidget()->row(m_item);
}