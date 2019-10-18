#pragma once

#include <QWidget>
#include "ImageSequence.h"

class QLineEdit;
class QToolButton;
class QSpinBox;

class FileInputWidget : public QWidget {
	Q_OBJECT
public:
	enum FileInputType {
		ReadFile,
		WriteFile
	};

public:
	explicit FileInputWidget(FileInputType type, QWidget *parent = nullptr);

	QString filename() const;
	void setFilename(const QString & value);

	int startNumber() const;
	void setStartNumber(int start_number);

	bool isImageSequence() const;
	ImageSequence imageSequence() const;

	void setDefaultDir(const QString & dir);

signals:
	void fileChanged(const QString & string);

private slots:
	void browse();
	void onStartNumberChanged(int value);

private:
	FileInputType _type;
	QString _defaultDir;
	QLineEdit * _lineEdit;
	QToolButton * _browseButton;
	QSpinBox * _startNumberInput;

	ImageSequence _imageSequence;
};
