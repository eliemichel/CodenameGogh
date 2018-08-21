#include "ParameterWidget.h"
#include "Logger.h"
#include "Parameter.h"
#include "Ui/UiTextInput.h"

ParameterWidget::ParameterWidget()
	: m_param(nullptr)
	, m_currentType(NoneType)
	, m_label(nullptr)
{
}

void ParameterWidget::SetParameter(::Parameter *param)
{
	// Disconnect slots from previous parameter
	if (m_param) {
		// TODO
		//disconnect(m_param, 0, this, 0);
	}

	// Update
	m_param = param;

	// Connect slots to new parameter
	if (m_param) {
		// TODO
		/*
		connect(m_param, &Parameter::valueChanged, this, &ParameterWidget::UpdateValue);
		connect(m_param, &Parameter::nameChanged, this, &ParameterWidget::UpdateName);
		connect(m_param, &Parameter::typeChanged, this, &ParameterWidget::UpdateStructure);
		connect(m_param, &Parameter::menuLabelChanged, this, &ParameterWidget::UpdateMenuLabel);
		connect(m_param, &Parameter::aboutToInsertMenuItems, this, &ParameterWidget::InsertMenuItems);
		connect(m_param, &Parameter::aboutToRemoveMenuItems, this, &ParameterWidget::RemoveMenuItems);
		*/
	}

	// Rebuild UI
	UpdateStructure();
}

void ParameterWidget::UpdateStructure()
{
	if (m_currentType == m_param->type() || (!m_param && m_currentType == NoneType)) {
		// Nothing to change
		return;
	}

	DestroyStructure();

	if (!m_param) {
		m_currentType = NoneType;
		return;
	}

	m_label = new UiLabel();
	m_label->SetText(m_param->name().toStdString());
	AddItem(m_label);

	m_currentType = m_param->type();

	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit = new UiTextInput();
		m_input.lineEdit->SetText("<string>");
		// TODO
		//connect(m_input.lineEdit, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		AddItem(m_input.lineEdit);
		break;
	}
	case IntType:
	{
		// TODO
		m_input.spinBox = new UiLabel();
		m_input.spinBox->SetText("<int>");
		//connect(m_input.spinBox, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		AddItem(m_input.spinBox);
		break;
	}
	case EnumType:
	{
		/*
		for (int j = 0; j < parameter()->menuCount(); ++j)
		{
		m_input.comboBox->addItem(parameter()->menuLabel(j));
		}
		*/
		m_input.comboBox = new UiLabel();
		m_input.comboBox->SetText("<enum>");
		//connect(m_input.comboBox, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		AddItem(m_input.comboBox);
		break;
	}
	default:
	{
		ERR_LOG << "Not implemented: Node Type" << m_param->type();
		break;
	}
	}

	UpdateValue();
}

void ParameterWidget::DestroyStructure()
{
	if (m_label) {
		RemoveItem(m_label);
	}

	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	case IntType:
	case EnumType:
		RemoveItem(m_input.widget);
		delete m_input.widget;
		break;
	default:
		ERR_LOG << "Not implemented: Node Type" << m_param->type();
		break;
	}
}

void ParameterWidget::UpdateValue()
{
	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit->SetText(m_param->rawValue().toString().toStdString());
		break;
	}
	case IntType:
	{
		m_input.spinBox->SetText(m_param->rawValue().toString().toStdString());
		break;
	}
	case EnumType:
	{
		m_input.comboBox->SetText(m_param->rawValue().toString().toStdString());
		break;
	}
	default:
	{
		ERR_LOG << "Not implemented: Node Type" << m_param->type();
		break;
	}
	}
}

void ParameterWidget::UpdateName() {
	if (m_label && m_param) {
		m_label->SetText(m_param->name().toStdString());
	}
}

void ParameterWidget::UpdateMenuLabel(int item)
{
	// TODO
}

void ParameterWidget::InsertMenuItems(int first, int last)
{
	// TODO
}

void ParameterWidget::RemoveMenuItems(int first, int last)
{
	// TODO
}
