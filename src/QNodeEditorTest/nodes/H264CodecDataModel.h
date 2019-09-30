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
class QSpinBox;
class CodecData;

class H264CodecDataModel
	: public NodeDataModel
{
	Q_OBJECT

public:
	H264CodecDataModel();

	virtual
	~H264CodecDataModel() {}

public:

	QString
	caption() const override
	{ return QStringLiteral("h264 Codec"); }

	bool
	captionVisible() const override
	{ return true; }

	static QString
	staticName()
	{ return QStringLiteral("h264 Codec"); }

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

private:
	std::shared_ptr<CodecData> _codec;
	QWidget * _widget;
	QSpinBox * _crfInput;
};
