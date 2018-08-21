#include "OutputNodeEditor.h"
#include "DefaultNodeEditor.h"
#include "OutputNode.h"
#include "Dialogs/RenderDialog.h"
#include "Logger.h"

#include <QMessageBox>
#include <QFont>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>

OutputNodeEditor::OutputNodeEditor(OutputNode *_node, QWidget *parent)
	: DefaultNodeEditor(_node, parent)
	, m_node(_node)
{
	// TODO: clean up, avoid this cast
	m_filenameInput = static_cast<QLineEdit*>(inputWidget(0));
	connect(m_filenameInput, &QLineEdit::textEdited, this, &OutputNodeEditor::setUserDefined);

	QPushButton *renderButton = new QPushButton("Render");
	connect(this, &DefaultNodeEditor::buttonClicked, this, &OutputNodeEditor::onButtonClicked);
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

void OutputNodeEditor::onButtonClicked(int parm)
{
	switch (parm)
	{
	case 2:
		render();
		break;
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
