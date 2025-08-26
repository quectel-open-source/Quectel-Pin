#include "qg_MyMessageBox.h"

qg_MyMessageBox::qg_MyMessageBox(QWidget *parent,QString title,QString text)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle(title);
	ui.label->setText(text);
}

qg_MyMessageBox::~qg_MyMessageBox()
{}

