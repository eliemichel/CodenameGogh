#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>

#include "FileInputWidget.h"
#include "ImageSequenceDetector.h"

FileInputWidget::FileInputWidget(FileInputType type, QWidget *parent)
	: QWidget(parent)
	, _type(type)
	, _defaultDir(QDir::currentPath())
	, _lineEdit(new QLineEdit)
	, _browseButton(new QToolButton())
{
	connect(_browseButton, &QToolButton::clicked, this, &FileInputWidget::browse);
	connect(_lineEdit, &QLineEdit::textChanged, this, &FileInputWidget::fileChanged);

	auto layout = new QHBoxLayout(this);
	layout->addWidget(_lineEdit);
	layout->addWidget(_browseButton);
	layout->setSpacing(0);
	layout->setMargin(0);
	_browseButton->setText("...");
}

QString FileInputWidget::filename() const {
	return _lineEdit->text();
}

void FileInputWidget::setFilename(const QString & value) {
	_lineEdit->setText(ImageSequenceDetector::autodetect(value));
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
