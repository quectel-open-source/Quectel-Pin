#include "SwitchDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

SwitchDelegate::SwitchDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

// 绘制滑动开关
void SwitchDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // 获取状态值（0 或 1）
    int state = index.data(Qt::EditRole).toInt();

    // 保存画笔状态
    painter->save();

    // 定义开关的绘制区域
    QRect rect = option.rect;
    int height = rect.height() / 2;
    int width = height * 2;
    QRect switchRect(rect.x() + (rect.width() - width) / 2, rect.y() + (rect.height() - height) / 2, width, height);

    // 绘制开关背景
    QColor backgroundColor = (state == 1) ? QColor(0, 120, 215) : QColor(200, 200, 200); // 蓝色表示开，灰色表示关
    painter->setBrush(backgroundColor);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(switchRect, height / 2, height / 2);

    // 绘制滑块
    int sliderDiameter = height - 4;
    int sliderX = (state == 1) ? switchRect.right() - sliderDiameter - 2 : switchRect.left() + 2;
    QRect sliderRect(sliderX, switchRect.top() + 2, sliderDiameter, sliderDiameter);
    painter->setBrush(Qt::white);
    painter->drawEllipse(sliderRect);

    // 恢复画笔状态
    painter->restore();
}

// 捕获点击事件并切换状态
bool SwitchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    // 屏蔽双击事件，防止进入编辑模式
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        return true; // 返回 true，表示事件已处理，不会进入编辑模式
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        // 捕获鼠标点击事件
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (option.rect.contains(mouseEvent->pos())) {
            // 获取当前状态
            int currentState = index.data(Qt::EditRole).toInt();

            // 切换状态
            int newState = (currentState == 1) ? 0 : 1;

            // 更新模型数据
            model->setData(index, newState, Qt::EditRole);

            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

// 禁止创建编辑器，防止双击进入编辑模式
QWidget* SwitchDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr; // 返回空指针，表示不允许创建编辑器
}
