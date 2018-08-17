#include "InputNode.h"
#include "DefaultNodeEditor.h"
#include "Logger.h"

#include <QFileInfo>

#include <sstream>

InputNode::InputNode()
	: m_probeProcess(this)
{
	// Add slots
	newOutputSlot();

	// Quick tests with video samples
	m_filename = "/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample.mp4";

	connect(&m_probeProcess, &FileProbeProcess::probed, this, &InputNode::updateStreams);
}

QWidget * InputNode::createEditor(QWidget *parent)
{
	DefaultNodeEditor *ed = new DefaultNodeEditor(this, parent);
	connect(ed, &DefaultNodeEditor::buttonClicked, this, &InputNode::onButtonClicked);
	return ed;
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	/*if (outputIndex != 0) {
		return false;
	}*/

	QString filename = parmEvalAsString(0);
	QFileInfo fileinfo(filename);
	if (!fileinfo.isFile())
	{
		std::ostringstream ss;
		ss << "Input file does not exist: " << filename.toStdString();
		cmd.err = ss.str();
		return false;
	}

	cmd.env["path"] = fileinfo.absolutePath().toStdString() + "/";
	cmd.env["filename"] = fileinfo.baseName().toStdString();
	cmd.env["ext"] = fileinfo.suffix().toStdString();
	cmd.env["input"] = filename.toStdString();
	//TODO: Set default keys from ffprobe file properties, like "codec" or "scale"

	cmd.map = outputIndex;

	cmd.stream = m_probeProcess.streamsAsChar(outputIndex);

	cmd.cmd.push_back("-i");
	cmd.cmd.push_back(filename.toStdString());

	return true;
}

// Data model

int InputNode::parmCount() const
{
	return 2;
}

QString InputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	case 1:
		return "Analyze";
	default:
		return QString();
	}
}

ParmType InputNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return StringType;
	case 1:
		return ButtonType;
	default:
		return NoneType;
	}
}

QVariant InputNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return QString::fromStdString(m_filename);
	default:
		return QVariant();
	}
}

bool InputNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		m_filename = value.toString().toStdString();
		m_probeProcess.cancel();
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}

void InputNode::updateStreams()
{
	removeOutputSlots();
	for (auto stream : m_probeProcess.streams()) {
		switch (stream)
		{
		case VideoStream:
		case AudioStream:
		case SubtitleStream:
		case DataStream:
			newOutputSlot();
		}
	}
}

void InputNode::onButtonClicked(int parm)
{
	switch (parm)
	{
	case 1:
		m_probeProcess.probe(parmEvalAsString(0));
		break;
	}
}
