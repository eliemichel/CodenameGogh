#pragma once

#include <QDialog>

namespace Ui {
	class WelcomeDialog;
}

class WelcomeDialog : public QDialog {
	Q_OBJECT
public:
	WelcomeDialog(QWidget *parent = nullptr);

private:
	Ui::WelcomeDialog *ui;
};
