#include "EnvDialog.h"
#include "ui_EnvDialog.h"

#include "EnvModel.h"

EnvDialog::EnvDialog(EnvModel *envModel, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::EnvDialog)
{
	ui->setupUi(this);

	connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);

	ui->envView->setModel(envModel);
}
