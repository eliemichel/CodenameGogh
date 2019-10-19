#include <QFontDatabase>

#include "WelcomeDialog.h"
#include "ui_WelcomeDialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::WelcomeDialog())
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	setFont(QFont(family));

	ui->setupUi(this);
}
