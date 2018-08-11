#include "OutputNode.h"

#include "OutputNodeEditor.h"
#include "Logger.h"

OutputNode::OutputNode()
	: m_isFilenameUserDefined(false)
	, m_node_name("output")
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

ParmType OutputNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return StringType;
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
					if (p != "")
					{
						replace(userPattern, "$" + p, cmd.keys[p]);
					}
				}
			}
			setParm(0, QString().fromStdString(userPattern));
		}
	}
}
