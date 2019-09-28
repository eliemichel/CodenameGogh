#pragma once

#include <QWidget>

class QLineEdit;
class QToolButton;

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
	void setFilename(const QString & value) const;

signals:
	void fileChanged(const QString & string);

private slots:
	void browse();

private:
	FileInputType _type;
	QLineEdit * _lineEdit;
	QToolButton * _browseButton;
};
