#pragma once
#pragma execution_character_set("utf-8")
#include <QStyledItemDelegate>

/**
 * SwitchDelegate - 用于在表格中实现滑动开关的委托
 */
class SwitchDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SwitchDelegate(QObject* parent = nullptr);

    // 绘制滑动开关
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // 捕获鼠标事件并切换状态
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    // 禁止创建编辑器，防止双击进入编辑模式
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // 更新编辑器中的数据（空实现，防止不必要的行为）
    void setEditorData(QWidget* editor, const QModelIndex& index) const override {}

    // 提交编辑器数据到模型（空实现，防止不必要的行为）
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {}
};


