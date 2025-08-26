#include "RowData.h"


RowData::RowData(PointItemConfig config, QTableWidgetItem* item)
:m_config(config), m_item(item)
{
    editButton = new QPushButton("�༭");
    //btn_upMove = new QPushButton("����");
    //btn_downMove = new QPushButton("����");
    //btnRemove = new QPushButton("ɾ��");

    // Ϊ��ť�����Զ������ԣ��кţ�
    editButton->setProperty("row", QVariant(currentRow()));
    //btn_upMove->setProperty("row", QVariant(row));
    //btn_downMove->setProperty("row", QVariant(row));
    //btnRemove->setProperty("row", QVariant(row));
}

int RowData::currentRow(){
    return m_item->tableWidget()->row(m_item);
}