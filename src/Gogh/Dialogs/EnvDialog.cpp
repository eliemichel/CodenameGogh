#include "EnvDialog.h"
#include "ui_EnvDialog.h"

#include "EnvModel.h"

EnvDialog::EnvDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::EnvDialog)
{
	ui->setupUi(this);

	connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);

	m_model = new EnvModel();
	ui->envView->setModel(m_model);
}

EnvDialog::~EnvDialog()
{
	delete m_model;
}
