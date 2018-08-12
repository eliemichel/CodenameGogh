#include "InputNode.h"

#include "Logger.h"

#include <QFileInfo>

#include <sstream>

InputNode::InputNode()
{
	// Add slots
	newOutputSlot();

	// Quick tests with video samples
	m_filename = "/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample.mp4";
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

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

	cmd.cmd.push_back("-i");
	cmd.cmd.push_back(filename.toStdString());

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

ParmType InputNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return StringType;
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
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}
