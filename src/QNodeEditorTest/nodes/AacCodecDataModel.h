#pragma once

#include <nodes/NodeDataModel>
#include <memory>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class QWidget;
class QLineEdit;
class QRadioButton;
class QComboBox;
class AudioCodecData;

class AacCodecDataModel
	: public NodeDataModel
{
	Q_OBJECT

public:
	AacCodecDataModel();

	virtual
	~AacCodecDataModel() {}

public:

	QString
	caption() const override
	{ return QStringLiteral("AAC Codec"); }

	bool
	captionVisible() const override
	{ return true; }

	static QString
	staticName()
	{ return QStringLiteral("AAC Codec"); }

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
	void onSelectChanged();
	void onPresetChanged();

private:
	std::shared_ptr<AudioCodecData> _codec;
	QWidget * _widget;
	QLineEdit * _cbrInput;
	QLineEdit * _vbrInput;
	QComboBox * _cbrPresets;
	QComboBox * _vbrPresets;
	QRadioButton * _cbrSelect;
	QRadioButton * _vbrSelect;
};
