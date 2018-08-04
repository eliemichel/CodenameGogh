#ifndef H_ENVDIALOG
#define H_ENVDIALOG

#include <QDialog>

class EnvModel;

namespace Ui {
	class EnvDialog;
}

class EnvDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EnvDialog(QWidget *parent = nullptr);
	~EnvDialog();

private:
	Ui::EnvDialog *ui;
	EnvModel *m_model;
};

#endif // H_ENVDIALOG
