#include "SwitchDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

SwitchDelegate::SwitchDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

// ���ƻ�������
void SwitchDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // ��ȡ״ֵ̬��0 �� 1��
    int state = index.data(Qt::EditRole).toInt();

    // ���滭��״̬
    painter->save();

    // ���忪�صĻ�������
    QRect rect = option.rect;
    int height = rect.height() / 2;
    int width = height * 2;
    QRect switchRect(rect.x() + (rect.width() - width) / 2, rect.y() + (rect.height() - height) / 2, width, height);

    // ���ƿ��ر���
    QColor backgroundColor = (state == 1) ? QColor(0, 120, 215) : QColor(200, 200, 200); // ��ɫ��ʾ������ɫ��ʾ��
    painter->setBrush(backgroundColor);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(switchRect, height / 2, height / 2);

    // ���ƻ���
    int sliderDiameter = height - 4;
    int sliderX = (state == 1) ? switchRect.right() - sliderDiameter - 2 : switchRect.left() + 2;
    QRect sliderRect(sliderX, switchRect.top() + 2, sliderDiameter, sliderDiameter);
    painter->setBrush(Qt::white);
    painter->drawEllipse(sliderRect);

    // �ָ�����״̬
    painter->restore();
}

// �������¼����л�״̬
bool SwitchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    // ����˫���¼�����ֹ����༭ģʽ
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        return true; // ���� true����ʾ�¼��Ѵ����������༭ģʽ
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        // ����������¼�
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (option.rect.contains(mouseEvent->pos())) {
            // ��ȡ��ǰ״̬
            int currentState = index.data(Qt::EditRole).toInt();

            // �л�״̬
            int newState = (currentState == 1) ? 0 : 1;

            // ����ģ������
            model->setData(index, newState, Qt::EditRole);

            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

// ��ֹ�����༭������ֹ˫������༭ģʽ
QWidget* SwitchDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr; // ���ؿ�ָ�룬��ʾ���������༭��
}
