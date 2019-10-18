#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>

#include "FileInputWidget.h"
#include "ImageSequence.h"

FileInputWidget::FileInputWidget(FileInputType type, QWidget *parent)
	: QWidget(parent)
	, _type(type)
	, _defaultDir(QDir::currentPath())
	, _lineEdit(new QLineEdit)
	, _browseButton(new QToolButton())
	, _startNumberInput(new QSpinBox())
{
	connect(_browseButton, &QToolButton::clicked, this, &FileInputWidget::browse);
	connect(_lineEdit, &QLineEdit::textChanged, this, &FileInputWidget::fileChanged);
	connect(_startNumberInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &FileInputWidget::onStartNumberChanged);

	auto layout = new QVBoxLayout(this);

	auto l = new QHBoxLayout();
	l->addWidget(_lineEdit);
	l->addWidget(_browseButton);
	l->setSpacing(0);
	l->setMargin(0);
	_browseButton->setText("...");
	layout->addLayout(l);

	// Start Frame input
	l = new QHBoxLayout();
	l->addWidget(new QLabel("Start Number"));
	_startNumberInput->setRange(0, 9999999);
	l->addWidget(_startNumberInput);
	layout->addLayout(l);
}

QString FileInputWidget::filename() const {
	return _lineEdit->text();
}

void FileInputWidget::setFilename(const QString & value) {
	_imageSequence = ImageSequence::fromAutodetect(value);
	if (_imageSequence.isValid()) {
		_lineEdit->setText(_imageSequence.basename);
		_startNumberInput->setValue(_imageSequence.startFrame);
	} else {
		_lineEdit->setText(value);
	}
}

int FileInputWidget::startNumber() const
{
	return _startNumberInput->value();
}

void FileInputWidget::setStartNumber(int start_number)
{
	// will also trigger onStartNumberChanged and hence set _imageSequence.startFrame
	_startNumberInput->setValue(start_number);
}

bool FileInputWidget::isImageSequence() const
{
	return _imageSequence.isValid();
}

ImageSequence FileInputWidget::imageSequence() const
{
	return _imageSequence;
}

void FileInputWidget::setDefaultDir(const QString & dir) {
	_defaultDir = dir;
}

void FileInputWidget::browse() {
	QString dir = _defaultDir;
	QFileInfo info(filename());
	if (info.isDir()) dir = info.path();
	if (info.dir().exists()) dir = info.dir().path();

	QString filename;
	switch (_type) {
	case ReadFile:
		filename = QFileDialog::getOpenFileName(this, tr("Open Media"), dir);
		break;

	case WriteFile:
	default:
		filename = QFileDialog::getSaveFileName(this, tr("Save Media"), dir);
	}

	if (!filename.isEmpty()) {
		setFilename(filename);
	}
}

void FileInputWidget::onStartNumberChanged(int value)
{
	_imageSequence.startFrame = value;
	emit fileChanged(filename());
}
