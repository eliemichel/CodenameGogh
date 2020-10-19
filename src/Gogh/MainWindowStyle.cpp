#include <QStyleFactory>

#include "MainWindowStyle.h"

MainWindowStyle::MainWindowStyle()
{
	setBaseStyle(QStyleFactory::create("Fusion"));
}

QPalette MainWindowStyle::standardPalette() const
{
	QPalette pal = QProxyStyle::standardPalette();
	pal.setColor(QPalette::Window, QColor(68, 68, 68));
	pal.setColor(QPalette::WindowText, Qt::white);
	pal.setColor(QPalette::Base, QColor(51, 51, 51));
	pal.setColor(QPalette::AlternateBase, QColor(61, 61, 61));
	pal.setColor(QPalette::Text, Qt::white);
	pal.setColor(QPalette::Button, QColor(61, 61, 61));
	pal.setColor(QPalette::ButtonText, QColor(232, 232, 232));
	return pal;
}
