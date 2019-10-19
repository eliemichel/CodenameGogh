#include <QFontDatabase>

#include "WelcomeDialog.h"
#include "ui_WelcomeDialog.h"
#include "Style.h"

WelcomeDialog::WelcomeDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::WelcomeDialog())
	, m_selectedAction(Cancel)
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	setFont(QFont(family));
	setStyleSheet(Gogh::Style::dialogStyle());

	ui->setupUi(this);

	connect(
		ui->newButton, &ClickableWidget::clicked,
		this, &WelcomeDialog::newGraph);

	connect(
		ui->openButton, &ClickableWidget::clicked,
		this, &WelcomeDialog::openGraph);

	connect(
		ui->closeButton, &QPushButton::clicked,
		this, &QDialog::close);
}

void WelcomeDialog::newGraph()
{
	m_selectedAction = NewGraph;
	close();
}

void WelcomeDialog::openGraph()
{
	m_selectedAction = OpenGraph;
	close();
}
