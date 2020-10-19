#pragma once

#include <QWidget>

class ClickableWidget : public QWidget {
	Q_OBJECT
public:
	explicit ClickableWidget(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
};
