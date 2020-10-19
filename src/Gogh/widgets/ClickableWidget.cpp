#include "ClickableWidget.h"

ClickableWidget::ClickableWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

void ClickableWidget::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
