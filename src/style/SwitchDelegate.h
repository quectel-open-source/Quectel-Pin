#pragma once
#pragma execution_character_set("utf-8")
#include <QStyledItemDelegate>

/**
 * SwitchDelegate - �����ڱ����ʵ�ֻ������ص�ί��
 */
class SwitchDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SwitchDelegate(QObject* parent = nullptr);

    // ���ƻ�������
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // ��������¼����л�״̬
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    // ��ֹ�����༭������ֹ˫������༭ģʽ
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // ���±༭���е����ݣ���ʵ�֣���ֹ����Ҫ����Ϊ��
    void setEditorData(QWidget* editor, const QModelIndex& index) const override {}

    // �ύ�༭�����ݵ�ģ�ͣ���ʵ�֣���ֹ����Ҫ����Ϊ��
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {}
};


