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
#include "AacCodecDataModel.h"
#include "AudioCodecData.h"

AacCodecDataModel::AacCodecDataModel()
	: _widget(new QWidget())
	, _cbrLabel(new QLabel("CBR"))
	, _vbrLabel(new QLabel("VBR"))
	, _cbrInput (new QLineEdit())
	, _vbrInput (new QLineEdit())
	, _cbrPresets (new QComboBox())
	, _vbrPresets (new QComboBox())
	, _cbrSelect (new QRadioButton("CBR"))
	, _vbrSelect (new QRadioButton("VBR"))
{
	auto l = new QGridLayout(_widget);
	l->setMargin(0);

	_cbrSelect->click();
	l->addWidget(_cbrSelect, 0, 0);
	l->addWidget(_vbrSelect, 0, 1);


	l->addWidget(_cbrLabel, 1, 0);
	_cbrInput->setText("160k");
	l->addWidget(_cbrInput, 1, 1);

	_cbrPresets->addItems(QStringList{
		"(presets)",
		"32k",
		"64k",
		"128k",
		"160k",
		"384k",
	});
	_cbrPresets->setCurrentIndex(0);
	l->addWidget(_cbrPresets, 1, 2);

	l->addWidget(_vbrLabel, 2, 0);

	_vbrInput->setText("2");
	l->addWidget(_vbrInput, 2, 1);

	_vbrPresets->addItems(QStringList{
		"(presets)",
		"0.1",
		"0.5",
		"1",
		"2",
	});
	_vbrPresets->setCurrentIndex(0);
	l->addWidget(_vbrPresets, 2, 2);

	connect<void(QComboBox::*)(int)>(
		_cbrPresets, &QComboBox::currentIndexChanged,
		this, &AacCodecDataModel::onPresetChanged);
	connect<void(QComboBox::*)(int)>(
		_vbrPresets, &QComboBox::currentIndexChanged,
		this, &AacCodecDataModel::onPresetChanged);
	connect(_cbrSelect, &QRadioButton::toggled,
		this, &AacCodecDataModel::onSelectChanged);
	connect(_vbrSelect, &QRadioButton::toggled,
		this, &AacCodecDataModel::onSelectChanged);
	onSelectChanged();

	_widget->setStyleSheet(Gogh::Style::nodeStyle());
}

// ----------------------------------------------------------------------------

QJsonObject AacCodecDataModel::save() const {
	QJsonObject modelJson = NodeDataModel::save();

	modelJson["cbr"] = _cbrInput->text();
	modelJson["vbr"] = _vbrInput->text();
	modelJson["useCbr"] = _cbrSelect->isDown();

	return modelJson;
}

void AacCodecDataModel::restore(QJsonObject const &p) {
	QJsonValue v;

	v = p["cbr"];
	if (v.isString()) {
		_cbrInput->setText(v.toString());
	}

	v = p["vbr"];
	if (v.isString()) {
		_vbrInput->setText(v.toString());
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

unsigned int AacCodecDataModel::nPorts(PortType portType) const {
	switch (portType) {
	case PortType::Out:
		return 1;

	case PortType::In:
	default:
		return 0;
	}
}

NodeDataType AacCodecDataModel::dataType(PortType, PortIndex) const {
	return AudioCodecData().type();
}

std::shared_ptr<NodeData> AacCodecDataModel::outData(PortIndex port) {
	if (!_codec) {
		_codec = std::make_shared<AudioCodecData>("aac", QStringList());
	}

	bool isCbr = _cbrSelect->isDown();
	QString cbr = _cbrInput->text();
	QString vbr = _vbrInput->text();

	if (isCbr) {
		if (!cbr.isEmpty()) {
			_codec->options() << "-b:a" << cbr; // TODO: does this set bitrate for all audio streams?
		}
	} else {
		if (!vbr.isEmpty()) {
			_codec->options() << "-q:a" << vbr; // TODO: does this set bitrate for all audio streams?
		}
	}

	return _codec;
}

// ----------------------------------------------------------------------------

void AacCodecDataModel::onSelectChanged()
{
	bool isCbr = _cbrSelect->isChecked();
	_cbrLabel->setVisible(isCbr);
	_cbrPresets->setVisible(isCbr);
	_cbrInput->setVisible(isCbr);
	_vbrLabel->setVisible(!isCbr);
	_vbrPresets->setVisible(!isCbr);
	_vbrInput->setVisible(!isCbr);
}

void AacCodecDataModel::onPresetChanged()
{
	if (_cbrPresets->currentIndex() > 0) {
		_cbrInput->setText(_cbrPresets->currentText());
		_cbrPresets->setCurrentIndex(0);
	}

	if (_vbrPresets->currentIndex() > 0) {
		_vbrInput->setText(_vbrPresets->currentText());
		_vbrPresets->setCurrentIndex(0);
	}
}
