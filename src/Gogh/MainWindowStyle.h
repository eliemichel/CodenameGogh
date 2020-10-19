#pragma once

#include <QProxyStyle>

class MainWindowStyle : public QProxyStyle {
public:
	MainWindowStyle();

	QPalette standardPalette() const override;
};

