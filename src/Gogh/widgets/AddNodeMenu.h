#pragma once

#include <QDialog>

namespace Ui {
	class AddNodeMenu;
}

class AddNodeMenu : public QWidget {
	Q_OBJECT
public:
	AddNodeMenu(QWidget *parent = nullptr);

private:
	Ui::AddNodeMenu *ui;
};
