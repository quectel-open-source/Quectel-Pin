#include "techgroupbox.h"
#include <QPropertyAnimation>

TechGroupBox::TechGroupBox(QWidget* parent /*= nullptr*/)
{
	initUI();
}


TechGroupBox::TechGroupBox(const QString& title, QWidget* parent)
	: QGroupBox(title, parent)
{
	initUI();
}

void TechGroupBox::initUI()
{
	// 设置基础样式
	setStyleSheet(
		"QGroupBox {"
		"   background-color: rgba(14,24,39);"
		"   border: 2px solid;"
		"   border-radius: 5px;"
		"   padding: 5px;"
		"   margin-top: 18px;"
		"   color: transparent;"
		"   border-color:rgba(37, 79, 135, 150);"
		"}"

		"QGroupBox::title {"
		"   subcontrol-origin: margin;"
		"   background-color: transparent;"
		"   padding-left: -20px;"
		"   subcontrol-position: top center;"// 可以设置标题位置 top/bottom left/right 
		"   color: #ffffff;"
		"}"
	);

	// 添加发光效果
	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
	shadowEffect->setBlurRadius(15);
	shadowEffect->setColor(QColor(58, 141, 212, 180));
	shadowEffect->setOffset(0, 0);
	setGraphicsEffect(shadowEffect);
}
