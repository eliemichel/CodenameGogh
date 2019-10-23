#include <QFontDatabase>

#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "Style.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::AboutDialog())
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	setFont(QFont(family));
	setStyleSheet(Gogh::Style::dialogStyle());

	ui->setupUi(this);

	connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
	connect(ui->titleBarCloseButton, &QPushButton::clicked, this, &QDialog::close);
}
