#include "ParameterWidget.h"
#include "Logger.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLayout>

ParameterWidget::ParameterWidget(QWidget *parent)
	: QWidget(parent)
	, m_parm(nullptr)
	, m_currentType(NoneType)
{
	m_layout = new QVBoxLayout();
	setLayout(m_layout);
}

void ParameterWidget::setParameter(Parameter *parm)
{
	// Disconnect slots from previous parameter
	if (m_parm)
	{
		disconnect(m_parm, 0, this, 0);
	}

	// Update
	m_parm = parm;

	// Connect slots to new parameter
	if (m_parm)
	{
		connect(m_parm, &Parameter::valueChanged, this, &ParameterWidget::updateValue);
		connect(m_parm, &Parameter::menuLabelChanged, this, &ParameterWidget::updateMenuLabel);
		connect(m_parm, &Parameter::aboutToInsertMenuItems, this, &ParameterWidget::insertMenuItems);
		connect(m_parm, &Parameter::aboutToRemoveMenuItems, this, &ParameterWidget::removeMenuItems);
	}

	// Rebuild UI
	updateStructure();
}

void ParameterWidget::updateStructure()
{
	if (m_currentType == parameter()->type() || (!parameter() && m_currentType == NoneType))
	{
		// Nothing to change
		return;
	}

	destroyStructure();

	if (!parameter())
	{
		m_currentType = NoneType;
		return;
	}

	m_currentType = parameter()->type();

	switch (parameter()->type())
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit = new QLineEdit();
		connect(m_input.lineEdit, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		m_layout->addWidget(m_input.lineEdit);
		break;
	}
	case IntType:
	{
		m_input.spinBox = new QSpinBox();
		m_input.spinBox->setMaximum(99999999);
		connect(m_input.spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) { parameter()->set(value); });
		m_layout->addWidget(m_input.spinBox);
		break;
	}
	case EnumType:
	{
		m_input.comboBox = new QComboBox();
		for (int j = 0; j < parameter()->menuCount(); ++j)
		{
			m_input.comboBox->addItem(parameter()->menuLabel(j));
		}
		connect(m_input.comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) { parameter()->set(index); });
		m_layout->addWidget(m_input.comboBox);
		break;
	}
	default:
	{
		ERR_LOG << "Not implemented: Node Type" << parameter()->type();
		break;
	}
	}

	updateValue();
}

void ParameterWidget::destroyStructure()
{
	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	case IntType:
	case EnumType:
		m_layout->removeWidget(m_input.widget);
		delete m_input.widget;
		break;
	default:
		ERR_LOG << "Not implemented: Node Type" << parameter()->type();
		break;
	}
}

void ParameterWidget::updateValue()
{
	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit->setText(parameter()->rawValue().toString());
		break;
	}
	case IntType:
	{
		m_input.spinBox->setValue(parameter()->rawValue().toInt());
		break;
	}
	case EnumType:
	{
		m_input.comboBox->setCurrentIndex(parameter()->rawValue().toInt());
		break;
	}
	default:
	{
		ERR_LOG << "Not implemented: Node Type" << parameter()->type();
		break;
	}
	}
}

void ParameterWidget::updateMenuLabel(int item)
{
	// TODO
}

void ParameterWidget::insertMenuItems(int first, int last)
{
	// TODO
}

void ParameterWidget::removeMenuItems(int first, int last)
{
	// TODO
}
