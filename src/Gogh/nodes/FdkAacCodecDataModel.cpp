#include <QWidget>
#include <QGridLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QtGlobal>

#include "Style.h"
#include "FdkAacCodecDataModel.h"
#include "AudioCodecData.h"

static QString getProfileIdentifier(int index) {
	switch (index) {
	case 0: return "aac_lc";
	case 1: return "aac_he";
	case 2: return "aac_he_v2";
	case 3: return "aac_ld";
	case 4: return "aac_eld";
	default: return "aac_lc";
	}
}

FdkAacCodecDataModel::FdkAacCodecDataModel()
	: _widget(new QWidget())
	, _cbrInput(new QLineEdit())
	, _cbrPresets(new QComboBox())
	, _vbrInput(new QComboBox())
	, _profileInput(new QComboBox())
	, _cbrSelect(new QRadioButton("CBR"))
	, _vbrSelect(new QRadioButton("VBR"))
{
	auto l = new QGridLayout(_widget);
	l->setMargin(0);

	_cbrSelect->click();
	l->addWidget(_cbrSelect, 0, 0);

	_cbrPresets->addItems(QStringList{
		"(presets)",
		"32k",
		"64k",
		"128k",
		"160k",
		"384k",
	});
	_cbrPresets->setCurrentIndex(0);
	l->addWidget(_cbrPresets, 0, 1);

	_cbrInput->setText("160k");
	l->addWidget(_cbrInput, 0, 2);

	l->addWidget(_vbrSelect, 1, 0);

	_vbrInput->addItems(QStringList{"1", "2", "3", "4", "5" });
	_vbrInput->setCurrentIndex(3);
	l->addWidget(_vbrInput, 1, 1);

	_profileInput->addItems(QStringList{
		"Low Complexity (aac_lc)",
		"High Efficiency (aac_he)",
		"High Efficiency v2 (aac_he_v2)",
		"Low Delay (aac_ld)",
		"Enhanced Low Delay (aac_eld)",
		});
	_profileInput->setCurrentIndex(0 /* slow */);
	l->addWidget(new QLabel("Profile"), 2, 0);
	l->addWidget(_profileInput, 2, 1);

	connect<void(QComboBox::*)(int)>(
		_cbrPresets, &QComboBox::currentIndexChanged,
		this, &FdkAacCodecDataModel::onPresetChanged);
	connect(_cbrSelect, &QRadioButton::toggled,
		this, &FdkAacCodecDataModel::onSelectChanged);
	connect(_vbrSelect, &QRadioButton::toggled,
		this, &FdkAacCodecDataModel::onSelectChanged);
	onSelectChanged();

	_widget->setStyleSheet(Gogh::Style::nodeStyle());
}

// ----------------------------------------------------------------------------

QJsonObject FdkAacCodecDataModel::save() const {
	QJsonObject modelJson = NodeDataModel::save();

	modelJson["cbr"] = _cbrInput->text();
	modelJson["vbr"] = _vbrInput->currentIndex();
	modelJson["profile"] = _profileInput->currentIndex();
	modelJson["useCbr"] = _cbrSelect->isDown();

	return modelJson;
}

void FdkAacCodecDataModel::restore(QJsonObject const &p) {
	QJsonValue v;

	v = p["cbr"];
	if (v.isString()) {
		_cbrInput->setText(v.toString());
	}

	v = p["vbr"];
	if (v.isDouble()) {
		_vbrInput->setCurrentIndex(v.toInt());
	}

	v = p["profile"];
	if (v.isDouble()) {
		_profileInput->setCurrentIndex(v.toInt());
	}

	v = p["useCbr"];
	if (v.isBool()) {
		if (v.toBool()) {
			_cbrSelect->click();
		} else {
			_vbrSelect->click();
		}
	}
}

// ----------------------------------------------------------------------------

unsigned int FdkAacCodecDataModel::nPorts(PortType portType) const {
	switch (portType) {
	case PortType::Out:
		return 1;

	case PortType::In:
	default:
		return 0;
	}
}

NodeDataType FdkAacCodecDataModel::dataType(PortType, PortIndex) const {
	return AudioCodecData().type();
}

std::shared_ptr<NodeData> FdkAacCodecDataModel::outData(PortIndex port) {
	if (!_codec) {
		_codec = std::make_shared<AudioCodecData>("libfdk_aac", QStringList());
	}

	bool isCbr = _cbrSelect->isDown();
	QString cbr = _cbrInput->text();
	QString vbr = _vbrInput->currentText();
	QString profile = getProfileIdentifier(_profileInput->currentIndex());

	if (isCbr) {
		if (!cbr.isEmpty()) {
			_codec->options() << "-b:a" << cbr; // TODO: does this set bitrate for all audio streams?
			_codec->options() << "-profile:a" << profile; // TODO: does this set bitrate for all audio streams?
		}
	} else {
		if (!vbr.isEmpty()) {
			_codec->options() << "-vbr" << vbr; // TODO: does this set bitrate for all audio streams?
		}
	}

	return _codec;
}

// ----------------------------------------------------------------------------

void FdkAacCodecDataModel::onSelectChanged()
{
	bool isCbr = _cbrSelect->isChecked();
	_cbrPresets->setEnabled(isCbr);
	_cbrInput->setEnabled(isCbr);
	_vbrInput->setEnabled(!isCbr);
}

void FdkAacCodecDataModel::onPresetChanged()
{
	if (_cbrPresets->currentIndex() > 0) {
		_cbrInput->setText(_cbrPresets->currentText());
		_cbrPresets->setCurrentIndex(0);
	}
}
