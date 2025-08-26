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
	// 添加无标题的构造函数
	explicit TechGroupBox(QWidget* parent = nullptr);
    // 添加有标题的构造函数
	explicit TechGroupBox(const QString& title, QWidget* parent = nullptr);

private:
	void initUI(); // 初始化UI函数
};

#endif // TECHGROUPBOX_H