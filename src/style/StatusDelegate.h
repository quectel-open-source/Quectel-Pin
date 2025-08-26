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
        // ��ȡ״ֵ̬
        int status = index.data(Qt::EditRole).toInt();

        // ���滭��״̬
        painter->save();

        // ȷ���Ƶ���ɫ
        QColor color = (status == 1) ? Qt::green : Qt::red;

        // ���Ƶ�
        QRect rect = option.rect;
        int radius = qMin(rect.width(), rect.height()) / 2 - 2; // ȷ��Բ�εİ뾶
        QRect circleRect(rect.center().x() - radius, rect.center().y() - radius, radius * 2, radius * 2);

        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(circleRect);

        // �ָ�����״̬
        painter->restore();
    }

    // ��ֹ�û�ֱ�ӱ༭����
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return nullptr;
    }
};
