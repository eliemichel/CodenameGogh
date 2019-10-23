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

