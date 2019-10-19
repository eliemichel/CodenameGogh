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

