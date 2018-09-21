#include "InputNode.h"
#include "Ui/NodeDelegate.h"
#include "Logger.h"
#include "Parameter.h"

#include <QFileInfo>

#include <sstream>

InputNode::InputNode()
{
	appendOutputSlot();
	appendParams(2);

	param(0).setName("Filename");
	param(0).setType(StringType);
	param(0).valueChanged.connect([this]() {
		m_probeProcess.cancel();
	});

	// Quick tests with video samples
	param(0).set("C:/Users/emichel.ATI.000/Videos/LAllumeurDeReverberes_Demo/_DAVID_DAVID_MACAIGNE_MICHEL_LAllumeurDeReverberes.mp4");

	param(1).setName("Analyze");
	param(1).setType(ButtonType);

	m_probeProcess.probed.connect(this, &InputNode::updateStreams);
}

UiElement * InputNode::createDelegate(UiLayout *popupLayout)
{
	NodeDelegate *del = new NodeDelegate(this, popupLayout);
	del->buttonClicked.connect(this, &InputNode::onButtonClicked);
	return del;
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	/*if (outputIndex != 0) {
		return false;
	}*/

	QString filename = QString::fromStdString(param(0).evalAsString());
	QFileInfo fileinfo(filename);
	if (!fileinfo.isFile())
	{
		std::ostringstream ss;
		ss << "Input file does not exist: " << filename.toStdString();
		cmd.err = ss.str();
		return false;
	}

	//Image Sequence parsing
	//We are supposed to have the basename of the file, no path, no extension
	std::string firstImageName = "foo_00";
	bool isImageSequence = false;
	int digitCount = 0;
	int firstDigit = 0;
	if(isImageSequence)
	{
		const char *splitedName = firstImageName.c_str();
		for (int i = static_cast<int>(firstImageName.length())-1; i >= 0; i--)
		{
			if(isdigit(splitedName[i]))
			{
				digitCount++;
			}
			else
			{
				firstDigit = i + 1;
				break;
			}
		}
		//TODO: startframe if the first file number is not 0.
		std::ostringstream s;
		for (int i = 0; i < firstDigit; i++)
		{
			s << splitedName[i];
		}
		s << "%" << digitCount << "d";
		//TODO: Add the extension and the path to make the whole command.
		DEBUG_LOG << s.str();
		delete[] splitedName;
	}

	//Register the source
	bool isNewSource = true;
	for (int i = 0; i < cmd.sources.size(); i++)
	 {
		 if (filename.toStdString() == cmd.sources[i])
		 {
			 isNewSource = false;
		 }
	 }
	 if (isNewSource)
	 {
		 cmd.sources.push_back(filename.toStdString());
	 }

	//Output smart-renaming
	cmd.env["path"] = fileinfo.absolutePath().toStdString() + "/";
	cmd.env["filename"] = fileinfo.baseName().toStdString();
	cmd.env["ext"] = fileinfo.suffix().toStdString();
	cmd.env["input"] = filename.toStdString();
	//TODO: Set default keys from ffprobe file properties, like "codec" or "scale"

	//MixNode mapping
	cmd.os.input = std::make_pair(filename.toStdString(), outputIndex);
	if (m_probeProcess.streams().size() > outputIndex) {
		cmd.os.stream = m_probeProcess.streams()[outputIndex];
	}

	return true;
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
			appendOutputSlot();
		}
	}
}

void InputNode::onButtonClicked(int parm)
{
	switch (parm)
	{
	case 1:
		m_probeProcess.probe(param(0).evalAsString());
		break;
	}
}
