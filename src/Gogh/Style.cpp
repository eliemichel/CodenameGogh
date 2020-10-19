#include "Style.h"

QString Gogh::Style::nodeStyle()
{
	return R"(
QWidget {
	background-color: rgba(0,0,0,0);
	color: white;
}
QAbstractButton {
	background-color: rgba(96,96,96,204);
}
QMenu {
	background-color: rgba(64,64,64,204);
}
QMenu::item:selected {
	background-color: #654321;
}
QComboBox QAbstractItemView {
	background-color: rgba(96,96,96,204);
}
	)";
}

QString Gogh::Style::dialogStyle()
{
	return R"(
QDialog {
	background-color: #1d1d1d;
	color: #e1e1e1;
}
QLabel {
	background-color: transparent;
	color: #e1e1e1;
}
QPushButton {
	background-color: #353535;
	color: #e1e1e1;
	font-size: 12pt;
	border: none;
	min-width: 80px;
	min-height: 24px;
}
QPushButton:hover {
	background-color: #3f3f3f;
}
QPushButton:pressed {
	background-color: #353535;
}
QPushButton:disabled {
	background-color: #252525;
	color: #525252;
}
QWidget[objectName^="newButtonContent"],
QWidget[objectName^="openButtonContent"] {
	background-color: #282828;
}
QWidget[objectName^="newButtonContent"]:hover,
QWidget[objectName^="openButtonContent"]:hover {
	background-color: #353535;
}
QWidget[objectName^="titleBarCloseButton"],
QWidget[objectName^="cancelButton"] {
	min-width: 32px;
	background-color: transparent;
}
QWidget[objectName^="titleBarCloseButton"]:hover,
QWidget[objectName^="cancelButton"]:hover {
	background-color: #7d1313;
	border: none;
}
	)";
}

QString Gogh::Style::contextMenuStyle()
{
	return R"(
QWidget {
	color: #e1e1e1;
	background-color: transparent;
}
QLabel {
	color: #e1e1e1;
}
QWidget[objectName^="leftSide"] {
	background-color: #1d1d1d;
}
QWidget[objectName^="rightSide"] {
	background-color: #252526;
}

QWidget[objectName^="searchInput"] {
	font-size: 12pt;
	border: none;
	background-color: #2a2a2c;
	padding-left: 4px;
}

QPushButton {
	background-color: #353535;
	color: #e1e1e1;
	font-size: 12pt;
	border: none;
	min-width: 80px;
	min-height: 24px;
}
QPushButton:hover {
	background-color: #3f3f3f;
}
QPushButton:pressed {
	background-color: #353535;
}
QPushButton:disabled {
	background-color: #252525;
	color: #525252;
}

QScrollBar:vertical {
	border: none;
	background:#1d1d1d;
	width:6px;
	margin: 0px 0px 0px 0px;
}

QScrollBar::handle:vertical {
	background: #2a2a2c;
	min-height: 0px;
}

QScrollBar::add-line:vertical {
	height: 0px;
	subcontrol-position: bottom;
	subcontrol-origin: margin;
}

QScrollBar::sub-line:vertical {
	height: 0 px;
	subcontrol-position: top;
	subcontrol-origin: margin;
}
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
	background: none;
}
	)";
}

