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
	explicit EnvDialog(EnvModel *envModel, QWidget *parent = nullptr);

private:
	Ui::EnvDialog *ui;
};

#endif // H_ENVDIALOG
