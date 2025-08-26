#ifndef TECHGROUPBOX_H
#define TECHGROUPBOX_H

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QListWidget>
#include <QGraphicsEffect>
#include <QString>

class TechGroupBox : public QGroupBox
{
	Q_OBJECT

public:
	// ����ޱ���Ĺ��캯��
	explicit TechGroupBox(QWidget* parent = nullptr);
    // ����б���Ĺ��캯��
	explicit TechGroupBox(const QString& title, QWidget* parent = nullptr);

private:
	void initUI(); // ��ʼ��UI����
};

#endif // TECHGROUPBOX_H