#pragma once
#include <QStyledItemDelegate>
#include <QPainter>

class StatusDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StatusDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        // 获取状态值
        int status = index.data(Qt::EditRole).toInt();

        // 保存画笔状态
        painter->save();

        // 确定灯的颜色
        QColor color = (status == 1) ? Qt::green : Qt::red;

        // 绘制灯
        QRect rect = option.rect;
        int radius = qMin(rect.width(), rect.height()) / 2 - 2; // 确定圆形的半径
        QRect circleRect(rect.center().x() - radius, rect.center().y() - radius, radius * 2, radius * 2);

        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(circleRect);

        // 恢复画笔状态
        painter->restore();
    }

    // 禁止用户直接编辑该列
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return nullptr;
    }
};
