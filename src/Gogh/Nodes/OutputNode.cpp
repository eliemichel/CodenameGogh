#include "OutputNode.h"

#include "OutputNodeEditor.h"
#include "Logger.h"

OutputNode::OutputNode()
	: m_isFilenameUserDefined(false)
{
	// Add slots
	newInputSlot();
}

QWidget * OutputNode::createEditor(QWidget * parent) 
{
	return new OutputNodeEditor(this, parent);
}

bool OutputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	// special output index for render function
	if (outputIndex != -1) {
		ERR_LOG << "Invalid output index: #" << outputIndex;
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	cmd.cmd.push_back(parmEvalAsString(0).toStdString());
	return true;
}

// Data model

int OutputNode::parmCount() const
{
	return 2;
}

QString OutputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	case 1:
		return "Render";
	default:
		return QString();
	}
}

ParmType OutputNode::parmType(int parm) const
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

QVariant OutputNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return QString::fromStdString(m_filename);
	default:
		return QVariant();
	}
}

bool OutputNode::setParm(int parm, QVariant value)
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

void OutputNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		if (!m_isFilenameUserDefined)
		{
			// TODO: auto name output
			RenderCommand cmd;
			std::string cmdString;
			if (buildRenderCommand(-1, cmd))
			{
				int i = 0;
				for (auto const& s : cmd.cmd)
				{
					cmdString = "/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample.mov";
					if (i == 3)
					{
						cmdString = "/Users/felixdavid/Documents/Logiciels/Tunnel/data/GoghTestSample_" + s + ".mov";
						break;
					}
					i++;
				}
			}
			setParm(0, QString().fromStdString(cmdString));
		}
	}
}
