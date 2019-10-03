#include <QWidget>
#include <QSpinBox>
#include <QFormLayout>
#include <QJsonObject>
#include <QLabel>
#include <QComboBox>

#include "H264CodecDataModel.h"
#include "VideoCodecData.h"

H264CodecDataModel::H264CodecDataModel()
	: _widget(new QWidget())
	, _crfInput(new QSpinBox())
	, _presetInput(new QComboBox())
	, _tuneInput(new QComboBox())
{
	auto l = new QFormLayout(_widget);
	l->setMargin(0);

	_crfInput->setMinimum(0);
	_crfInput->setMaximum(51);
	_crfInput->setValue(22);
	l->addRow("CRF", _crfInput);

	_presetInput->addItems(QStringList{
		"ultrafast",
		"superfast",
		"veryfast",
		"faster",
		"fast",
		"medium",
		"slow",
		"slower",
		"veryslow",
		"placebo",
	});
	_presetInput->setCurrentIndex(6 /* slow */);
	l->addRow("Preset", _presetInput);

	_tuneInput->addItems(QStringList{
		"none",
		"film",
		"animation",
		"grain",
		"stillimage",
		"fastdecode",
		"zerolatency",
		"psnr",
		"ssim",
	});
	_tuneInput->setCurrentIndex(0 /* none */);
	l->addRow("Tune", _tuneInput);

	_widget->setStyleSheet(R"(
		QWidget { background-color: rgba(0,0,0,0); color: white }
		QAbstractButton { background-color: rgba(96,96,96,204) }
		QComboBox QAbstractItemView { background-color: rgba(96,96,96,204) }
	)");
}

// ----------------------------------------------------------------------------

QJsonObject H264CodecDataModel::save() const {
	QJsonObject modelJson = NodeDataModel::save();

	modelJson["crf"] = _crfInput->value();
	modelJson["preset"] = _presetInput->currentIndex();
	modelJson["tune"] = _tuneInput->currentIndex();

	return modelJson;
}

void H264CodecDataModel::restore(QJsonObject const &p) {
	QJsonValue v;

	v = p["crf"];
	if (v.isDouble()) {
		_crfInput->setValue(v.toInt());
	}

	v = p["preset"];
	if (v.isDouble()) {
		_presetInput->setCurrentIndex(v.toInt());
	}

	v = p["tune"];
	if (v.isDouble()) {
		_tuneInput->setCurrentIndex(v.toInt());
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
	return VideoCodecData().type();
}

std::shared_ptr<NodeData> H264CodecDataModel::outData(PortIndex port) {
	if (!_codec) {
		_codec = std::make_shared<VideoCodecData>("libx264", QStringList());
	}

	QString crf = _crfInput->text();
	QString preset = _presetInput->currentText();
	QString tune = _tuneInput->currentText();

	_codec->options().clear();
	if (preset != "medium") {
		_codec->options() << "-preset" << preset;
	}
	if (tune != "none") {
		_codec->options() << "-tune" << tune;
	}
	_codec->options()
		<< "-crf" << crf
		<< "-pix_fmt" << "yuv420p"; // TODO: this is a filter actually, it does not belong in codec data. Equivalent to '-vf format=yuv420p'
	return _codec;
}
