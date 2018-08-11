#include "OutputNodeEditor.h"
#include "DefaultNodeEditor.h"
#include "OutputNode.h"
#include "RenderDialog.h"
#include "Logger.h"

#include <QMessageBox>
#include <QFont>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>

OutputNodeEditor::OutputNodeEditor(OutputNode *_node, QWidget *parent)
	: QWidget(parent)
	, m_node(_node)
{
	QVBoxLayout *layout = new QVBoxLayout();
	DefaultNodeEditor *ed = new DefaultNodeEditor(node(), parent);
	layout->addWidget(ed);

	// TODO: clean up, avoid this cast
	m_filenameInput = static_cast<QLineEdit*>(ed->inputWidget(0));
	connect(m_filenameInput, &QLineEdit::textEdited, this, &OutputNodeEditor::setUserDefined);

	QPushButton *renderButton = new QPushButton("Render");
	connect(renderButton, &QPushButton::clicked, this, &OutputNodeEditor::render);
	layout->addWidget(renderButton);

	setLayout(layout);
}

void OutputNodeEditor::render()
{
	RenderCommand cmd;
	if (node()->buildRenderCommand(-1, cmd))
	{
		RenderDialog renderDialog(cmd.cmd);
		renderDialog.exec();
	}
	else
	{
		ERR_LOG << "Render failed";
		QMessageBox errDialog;
		errDialog.setText("Render failed");
		errDialog.setInformativeText(QString::fromStdString(cmd.err));
		errDialog.setIcon(QMessageBox::Critical);
		errDialog.exec();
	}
}

void OutputNodeEditor::setUserDefined()
{
	m_isFilenameUserDefined = !m_filenameInput->text().isEmpty();
	node()->setFilenameUserDefined(m_isFilenameUserDefined);

	QFont font = m_filenameInput->font();
	font.setBold(!m_isFilenameUserDefined);
	m_filenameInput->setFont(font);
}
