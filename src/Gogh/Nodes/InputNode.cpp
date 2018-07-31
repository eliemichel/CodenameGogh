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

	// Add slots
	newOutputSlot();
}

InputNode::~InputNode()
{
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	QString filename = ui->filenameInput->text();
	QFileInfo fileinfo(filename);
	if (!fileinfo.isFile())
	{
		std::ostringstream ss;
		ss << "Input file does not exist: " << filename.toStdString();
		cmd.err = ss.str();
		return false;
	}

	cmd.cmd = "-i " + filename.toStdString();
	return true;
}

void InputNode::read(QDataStream & stream)
{
	QString filename;
	stream >> filename;
	ui->filenameInput->setText(filename);
}

void InputNode::write(QDataStream & stream) const
{
	QString filename = ui->filenameInput->text();
	stream << filename;
}

