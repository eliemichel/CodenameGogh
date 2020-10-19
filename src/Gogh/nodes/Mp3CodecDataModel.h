#pragma once

#include <nodes/NodeDataModel>
#include <memory>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class QSpinBox;
class QCheckBox;
class QWidget;
class QLineEdit;
class QRadioButton;
class QComboBox;
class AudioCodecData;

class Mp3CodecDataModel
	: public NodeDataModel
{
	Q_OBJECT

public:
	Mp3CodecDataModel();

	virtual
	~Mp3CodecDataModel() {}

public:

	QString
	caption() const override
	{ return QStringLiteral("MP3 Codec"); }

	bool
	captionVisible() const override
	{ return true; }

	static QString
	staticName()
	{ return QStringLiteral("MP3 Codec"); }

	QString
	name() const override
	{ return staticName(); }

public:

	QJsonObject
	save() const override;

	void
	restore(QJsonObject const &p) override;

public:

	unsigned int
	nPorts(PortType portType) const override;

	NodeDataType
	dataType(PortType, PortIndex) const override;

	std::shared_ptr<NodeData>
	outData(PortIndex port) override;

	void
	setInData(std::shared_ptr<NodeData>, int) override
	{ }

	QWidget *
	embeddedWidget() override { return _widget; }

private slots:
	void onBitrateModeChanged();
	void onUseCutoffChange();
	void onPresetChanged();

private:
	std::shared_ptr<AudioCodecData> _codec;
	QWidget * _widget;
	QComboBox * _bitrateMode;
	QLineEdit * _cbrInput;
	QComboBox * _cbrPresets;
	QComboBox * _qscaleInput;
	QCheckBox * _useCutoff;
	QSpinBox * _cutoffInput;
	QCheckBox * _jointStereoInput;
};
