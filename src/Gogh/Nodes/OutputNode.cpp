#include "OutputNode.h"
#include "ui_OutputNode.h"

#include "RenderDialog.h"
#include "Logger.h"

#include <QMessageBox>
#include <QFont>

OutputNode::OutputNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::OutputNode)
	, m_isFilenameUserDefined(false)
{
	ui->setupUi(this);

	connect(ui->renderButton, &QPushButton::clicked, this, &OutputNode::render);
	connect(ui->filenameInput, &QLineEdit::textEdited, this, &OutputNode::setUserDefined);

	// Add slots
	newInputSlot();

	//Quick tests with video samples
	ui->filenameInput->setText("/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample_h264.mp4");
	//DEBUG_LOG << ui->filenameInput->text().toStdString();
	ui->filenameInput->setPlaceholderText("Path/to/output_file");
}

OutputNode::~OutputNode()
{
}

bool OutputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	// special output index for render function
	if (outputIndex != -1) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	cmd.cmd.push_back(parmFullEval(0).toStdString());
	return true;
}

// Data model

int OutputNode::parmCount() const
{
	return 1;
}

QString OutputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	default:
		return QString();
	}
}

QVariant OutputNode::parmEval(int parm) const
{
	switch (parm)
	{
	case 0:
		return ui->filenameInput->text();
	default:
		return QVariant();
	}
}

void OutputNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		ui->filenameInput->setText(value.toString());
		setUserDefined();
		break;
	}
}

void OutputNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		if (!m_isFilenameUserDefined)
		{
			// TODO: auto name output
			setParm(0, "[auto generated name]");
		}
	}
}

void OutputNode::render()
{
	RenderCommand cmd;
	if (buildRenderCommand(-1, cmd))
	{
		std::string cmdString;
		for (auto const& s : cmd.cmd) { cmdString += s; }
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

void OutputNode::setUserDefined()
{
	m_isFilenameUserDefined = !ui->filenameInput->text().isEmpty();

	QFont font = ui->filenameInput->font();
	font.setBold(m_isFilenameUserDefined);
	ui->filenameInput->setFont(font);
}

