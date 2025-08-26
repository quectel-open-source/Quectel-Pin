#ifndef ROWDATA_H
#define ROWDATA_H

#include <QString>
#include <QPushButton>
#include <QVariant>
#include "rs.h"
#include <QtWidgets/qtablewidget.h>

class RowData {
public:
    RowData(PointItemConfig config, QTableWidgetItem* item);

    PointItemConfig m_config;
    QPushButton* editButton;
    QPushButton* btn_upMove, * btn_downMove, * btnRemove;
    QTableWidgetItem* m_item;

    int currentRow();
};

#endif // ROWDATA_H
