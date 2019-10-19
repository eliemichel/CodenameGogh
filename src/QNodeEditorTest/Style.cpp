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
			color: #e1e1e1;
		}
		QWidget[objectName^="newButtonContent"],
		QWidget[objectName^="openButtonContent"] {
			background-color: #282828;
		}
		QWidget[objectName^="newButtonContent"]:hover,
		QWidget[objectName^="openButtonContent"]:hover {
			background-color: #353535;
		}
	)";
}

