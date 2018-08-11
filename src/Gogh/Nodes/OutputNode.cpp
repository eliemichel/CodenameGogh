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
, m_node_name("output")
{
	ui->setupUi(this);

	connect(ui->renderButton, &QPushButton::clicked, this, &OutputNode::render);
	connect(ui->filenameInput, &QLineEdit::textEdited, this, &OutputNode::setUserDefined);

	// Add slots
	newInputSlot();

	//Quick tests with video samples
	//ui->filenameInput->setText("/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample_h264.mp4");
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
		break;
	}
}

void OutputNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		if (!m_isFilenameUserDefined)
		{
			// Smart ouput renaming
			RenderCommand cmd;

			//TODO: userPattern from user_preferences menu
			std::string userPattern = "$path$filename_$codec_$scale.$ext";

			std::string s = userPattern;
			s.erase(remove_if(s.begin(), s.end(), [](char c) { return !isalpha(c) && c != '$'; } ), s.end());

			stringlist patternKeys = split(s, '$');

			if (buildRenderCommand(-1, cmd))
			{
				for (auto const& p : patternKeys)
				{
					replace(userPattern, "$" + p, cmd.keys[p]);
				}
			}
			setParm(0, QString().fromStdString(userPattern));
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
	font.setBold(!m_isFilenameUserDefined);
	ui->filenameInput->setFont(font);
}
