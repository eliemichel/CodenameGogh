#include <QFontDatabase>
#include <QScrollBar>

#include "AddNodeMenu.h"
#include "ui_AddNodeMenu.h"
#include "Style.h"

AddNodeMenu::AddNodeMenu(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AddNodeMenu())
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	setFont(QFont(family));

	setStyleSheet(Gogh::Style::contextMenuStyle());

	ui->setupUi(this);

	ui->nodeListScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QScrollBar * scrollBar = ui->nodeListScrollArea->verticalScrollBar();
	ui->nodeListScrollAreaWrapper->insertWidget(0, scrollBar);
}
