#include <QWidget>
#include <QGridLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QtGlobal>

#include "Style.h"
#include "Mp3CodecDataModel.h"
#include "AudioCodecData.h"

Mp3CodecDataModel::Mp3CodecDataModel()
	: _widget(new QWidget())
	, _bitrateMode(new QComboBox())
	, _cbrInput(new QLineEdit())
	, _cbrPresets(new QComboBox())
	, _qscaleInput(new QComboBox())
	, _cutoffInput(new QSpinBox())
	, _useCutoff(new QCheckBox("Cutoff"))
	, _jointStereoInput(new QCheckBox("Joint Stereo"))
{
	_cbrInput->setText("160k");

	_bitrateMode->addItems(QStringList{
		"Constant (CBR)",
		"Variable (VBR)",
		"Average (ABR)",
		});
	_bitrateMode->setCurrentIndex(1);

	_cbrPresets->addItems(QStringList{
		"(presets)",
		"32k",
		"64k",
		"128k",
		"160k",
		"384k",
		});
	_cbrPresets->setCurrentIndex(0);

	_qscaleInput->addItems(QStringList{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" });
	_qscaleInput->setCurrentIndex(3);

	_cutoffInput->setRange(0, 128000);

	_jointStereoInput->setChecked(true);

	auto l = new QGridLayout(_widget);
	l->setMargin(0);

	l->addWidget(_bitrateMode, 0, 0);
	l->addWidget(new QLabel("CBR"), 1, 0);
	l->addWidget(_cbrInput, 1, 1);
	l->addWidget(_cbrPresets, 1, 2);
	l->addWidget(new QLabel("QScale"), 2, 0);
	l->addWidget(_qscaleInput, 2, 1);

	l->addWidget(_useCutoff, 3, 0);
	l->addWidget(_cutoffInput, 3, 1);

	l->addWidget(_jointStereoInput, 4, 0);

	connect<void(QComboBox::*)(int)>(
		_cbrPresets, &QComboBox::currentIndexChanged,
		this, &Mp3CodecDataModel::onPresetChanged);
	connect<void(QComboBox::*)(int)>(
		_bitrateMode, &QComboBox::currentIndexChanged,
		this, &Mp3CodecDataModel::onBitrateModeChanged);
	connect(_useCutoff, &QCheckBox::stateChanged,
		this, &Mp3CodecDataModel::onUseCutoffChange);
	onBitrateModeChanged();

	_widget->setStyleSheet(Gogh::Style::nodeStyle());
}

// ----------------------------------------------------------------------------

QJsonObject Mp3CodecDataModel::save() const {
	QJsonObject modelJson = NodeDataModel::save();

	modelJson["bitrateMode"] = _bitrateMode->currentIndex();
	modelJson["cbr"] = _cbrInput->text();
	modelJson["qscale"] = _qscaleInput->currentIndex();
	modelJson["useCutoff"] = _useCutoff->isChecked();
	modelJson["cutoff"] = _cutoffInput->value();
	modelJson["jointStereo"] = _jointStereoInput->isChecked();

	return modelJson;
}

void Mp3CodecDataModel::restore(QJsonObject const &p) {
	QJsonValue v;

	v = p["bitrateMode"];
	if (v.isDouble()) {
		_bitrateMode->setCurrentIndex(v.toInt());
	}

	v = p["cbr"];
	if (v.isString()) {
		_cbrInput->setText(v.toString());
	}

	v = p["qscale"];
	if (v.isDouble()) {
		_qscaleInput->setCurrentIndex(v.toInt());
	}

	v = p["useCutoff"];
	if (v.isBool()) {
		_useCutoff->setChecked(v.toBool());
	}

	v = p["cutoff"];
	if (v.isDouble()) {
		_cutoffInput->setValue(v.toInt());
	}

	v = p["jointStereo"];
	if (v.isBool()) {
		_jointStereoInput->setChecked(v.toBool());
	}
}

// ----------------------------------------------------------------------------

unsigned int Mp3CodecDataModel::nPorts(PortType portType) const {
	switch (portType) {
	case PortType::Out:
		return 1;

	case PortType::In:
	default:
		return 0;
	}
}

NodeDataType Mp3CodecDataModel::dataType(PortType, PortIndex) const {
	return AudioCodecData().type();
}

std::shared_ptr<NodeData> Mp3CodecDataModel::outData(PortIndex port) {
	if (!_codec) {
		_codec = std::make_shared<AudioCodecData>("libmp3lame", QStringList());
	}

	int bitrateMode = _bitrateMode->currentIndex();
	bool use_cutoff = _useCutoff->isChecked();
	int cutoff = _cutoffInput->value();
	bool joint_stereo = _jointStereoInput->isChecked();
	bool joint_stereo_default = true;
	QString cbr = _cbrInput->text();
	QString qscale = _qscaleInput->currentText();

	// TODO: does this set bitrate for all audio streams?
	switch (bitrateMode) {
	case 0: // CBR
		_codec->options() << "-b:a" << cbr;
		break;
	case 1: // VBR
		_codec->options() << "-q:a" << qscale;
		break;
	case 2: // ABR
		_codec->options()
			<< "-abr" << "1"
			<< "-b:a" << cbr
			<< "-q:a" << qscale;
		break;
	}

	if (use_cutoff) {
		_codec->options() << "-cutoff" << QString::number(cutoff);
	}

	if (joint_stereo != joint_stereo_default) {
		_codec->options() << "-joint_stereo" << (joint_stereo ? "1" : "0");
	}

	return _codec;
}

// ----------------------------------------------------------------------------

void Mp3CodecDataModel::onBitrateModeChanged()
{
	bool isCbr = _bitrateMode->currentIndex() == 0 || _bitrateMode->currentIndex() == 2;
	bool isVbr = _bitrateMode->currentIndex() == 1 || _bitrateMode->currentIndex() == 2;
	_cbrPresets->setEnabled(isCbr);
	_cbrInput->setEnabled(isCbr);
	_qscaleInput->setEnabled(isVbr);
}

void Mp3CodecDataModel::onUseCutoffChange()
{
	_cutoffInput->setEnabled(_useCutoff->isChecked());
}

void Mp3CodecDataModel::onPresetChanged()
{
	if (_cbrPresets->currentIndex() > 0) {
		_cbrInput->setText(_cbrPresets->currentText());
		_cbrPresets->setCurrentIndex(0);
	}
}
