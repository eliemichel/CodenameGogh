#include <QWidget>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QLabel>

#include "H264CodecDataModel.h"
#include "CodecData.h"

H264CodecDataModel::H264CodecDataModel()
	: _widget(new QWidget())
	, _crfInput(new QSpinBox())
{
	auto l = new QHBoxLayout(_widget);
	l->setMargin(0);
	_crfInput->setMinimum(0);
	_crfInput->setMaximum(51);
	_crfInput->setValue(22);
	l->addWidget(new QLabel("CRF"));
	l->addWidget(_crfInput);
	_widget->setStyleSheet("QWidget{background-color: rgba(0,0,0,0);color: white} QAbstractButton{background-color: rgba(96,96,96,204)}");
}

// ----------------------------------------------------------------------------

QJsonObject H264CodecDataModel::save() const {
	QJsonObject modelJson = NodeDataModel::save();

	modelJson["crf"] = _crfInput->value();

	return modelJson;
}

void H264CodecDataModel::restore(QJsonObject const &p) {
	 QJsonValue v = p["file"];

	if (!v.isUndefined()) {
		_crfInput->setValue(v.toInt());
	}
}

// ----------------------------------------------------------------------------

unsigned int H264CodecDataModel::nPorts(PortType portType) const {
	switch (portType) {
	case PortType::Out:
		return 1;

	case PortType::In:
	default:
		return 0;
	}
}

NodeDataType H264CodecDataModel::dataType(PortType, PortIndex) const {
	return CodecData().type();
}

std::shared_ptr<NodeData> H264CodecDataModel::outData(PortIndex port) {
	if (!_codec) {
		_codec = std::make_shared<CodecData>("libx264", QStringList());
	}
	QString crf = _crfInput->text();
	_codec->options().clear();
	_codec->options()
		<< "-preset" << "slow"
		<< "-crf" << crf
		<< "-pix_fmt" << "yuv420p"; // TODO: this is a filter actually, it does not belong in codec data. Equivalent to '-vf format=yuv420p'
	return _codec;
}
