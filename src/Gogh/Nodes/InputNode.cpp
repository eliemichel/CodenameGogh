#include "InputNode.h"
#include "ui_InputNode.h"

#include "Logger.h"

#include <QFileInfo>

#include <sstream>

InputNode::InputNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::InputNode)
{
	ui->setupUi(this);
	m_node_name = "Input";

	// Add slots
	newOutputSlot();

	//Quick tests with video samples
	ui->filenameInput->setText("/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample.mp4");
	//DEBUG_LOG << ui->filenameInput->text().toStdString();
	ui->filenameInput->setPlaceholderText("Path/to/input_file");
}

InputNode::~InputNode()
{
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	stringlist pattern;
	return buildRenderCommand(outputIndex, cmd, pattern);
}
bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd, stringlist & pattern) const
{
	if (outputIndex != 0) {
		return false;
	}

	QString filename = parmFullEval(0);
	QFileInfo fileinfo(filename);
	if (!fileinfo.isFile())
	{
		std::ostringstream ss;
		ss << "Input file does not exist: " << filename.toStdString();
		cmd.err = ss.str();
		return false;
	}

	if(isPatterned(pattern)){
		cmd.cmd.push_back(fileinfo.absolutePath().toStdString());
		cmd.cmd.push_back(fileinfo.baseName().toStdString());
		cmd.cmd.push_back("." + fileinfo.suffix().toStdString());
	}
	else
	{
		cmd.cmd.push_back("-i");
		cmd.cmd.push_back(filename.toStdString());
	}
	return true;
}

// Data model

int InputNode::parmCount() const
{
	return 1;
}

QString InputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	default:
		return QString();
	}
}

QVariant InputNode::parmEval(int parm) const
{
	switch (parm)
	{
	case 0:
		return ui->filenameInput->text();
	default:
		return QVariant();
	}
}

void InputNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		ui->filenameInput->setText(value.toString());
		break;
	}
}
