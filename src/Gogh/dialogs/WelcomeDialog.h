#pragma once

#include <QDialog>

namespace Ui {
	class WelcomeDialog;
}

class WelcomeDialog : public QDialog {
	Q_OBJECT
public:
	enum WelcomeAction {
		NewGraph,
		OpenGraph,
		Cancel,
	};
public:
	WelcomeDialog(QWidget *parent = nullptr);

	WelcomeAction selectedAction() const { return m_selectedAction; }

private slots:
	void newGraph();
	void openGraph();

private:
	Ui::WelcomeDialog *ui;
	WelcomeAction m_selectedAction;
};
