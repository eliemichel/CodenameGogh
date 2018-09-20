#include "ParameterDelegate.h"
#include "Logger.h"
#include "Parameter.h"
#include "Ui/UiTextInput.h"
#include "Ui/UiIntInput.h"
#include "Ui/UiEnumInput.h"
#include "Ui/UiButton.h"

ParameterDelegate::ParameterDelegate(UiLayout *popupLayout)
	: m_param(nullptr)
	, m_currentType(NoneType)
	, m_label(nullptr)
	, m_popupLayout(popupLayout)
{
}

ParameterDelegate::~ParameterDelegate() {
	DisconnectSlots();
}

void ParameterDelegate::SetParameter(::Parameter *param)
{
	// Disconnect slots from previous parameter
	DisconnectSlots();

	// Update
	m_param = param;

	// Connect slots to new parameter
	ConnectSlots();

	// Rebuild UI
	UpdateStructure();
}

void ParameterDelegate::ConnectSlots() {
	if (m_param) {
		m_valueChangedConnection = m_param->valueChanged.connect(this, &ParameterDelegate::UpdateValue);
		m_nameChangedConnection = m_param->nameChanged.connect(this, &ParameterDelegate::UpdateName);
		m_typeChangedConnection = m_param->typeChanged.connect(this, &ParameterDelegate::UpdateStructure);
		m_menuLabelChangedConnection = m_param->menuLabelChanged.connect(this, &ParameterDelegate::UpdateMenuLabel);
		m_aboutToInsertMenuItemsConnection = m_param->aboutToInsertMenuItems.connect(this, &ParameterDelegate::InsertMenuItems);
		m_aboutToRemoveMenuItemsConnection = m_param->aboutToRemoveMenuItems.connect(this, &ParameterDelegate::RemoveMenuItems);
		m_destroyedConnection = m_param->destroyed.connect(this, &ParameterDelegate::OnParameterDestroyed);
	}
}

void ParameterDelegate::DisconnectSlots() {
	if (m_param) {
		m_param->valueChanged.disconnect(m_valueChangedConnection);
		m_param->nameChanged.disconnect(m_nameChangedConnection);
		m_param->typeChanged.disconnect(m_typeChangedConnection);
		m_param->menuLabelChanged.disconnect(m_menuLabelChangedConnection);
		m_param->aboutToInsertMenuItems.disconnect(m_aboutToInsertMenuItemsConnection);
		m_param->aboutToRemoveMenuItems.disconnect(m_aboutToRemoveMenuItemsConnection);
		m_param->destroyed.disconnect(m_destroyedConnection);
	}
}

void ParameterDelegate::UpdateStructure()
{
	if ((m_param && m_currentType == m_param->type()) || (!m_param && m_currentType == NoneType)) {
		// Nothing to change
		return;
	}

	DestroyStructure();

	if (!m_param) {
		m_currentType = NoneType;
		return;
	}

	m_currentType = m_param->type();

	// Exception: no label when the param is a button
	if (m_currentType == ButtonType) {
		m_input.pushButton = new UiButton();
		m_input.pushButton->SetText(m_param->name());
		m_input.pushButton->clicked.connect([this]() { buttonClicked.fire(); });
		AddItem(m_input.pushButton);
		return;
	}

	m_label = new UiLabel();
	m_label->SetText(m_param->name());
	AddItem(m_label);

	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit = new UiTextInput();
		m_input.lineEdit->textEdited.connect([=](const std::string & text) { m_param->set(text); });
		AddItem(m_input.lineEdit);
		break;
	}
	case IntType:
	{
		m_input.spinBox = new UiIntInput();
		m_input.spinBox->SetValue(0);
		// TODO
		//connect(m_input.spinBox, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		AddItem(m_input.spinBox);
		break;
	}
	case EnumType:
	{
		std::vector<std::string> labels;
		for (int j = 0; j < Parameter()->menuCount(); ++j) {
			labels.push_back(Parameter()->menuLabel(j));
		}
		m_input.comboBox = new UiEnumInput(m_popupLayout);
		m_input.comboBox->SetItemLabels(labels);
		//connect(m_input.comboBox, &QLineEdit::textEdited, [=](const QString &text) { parameter()->set(text); });
		AddItem(m_input.comboBox);
		break;
	}
	default:
	{
		ERR_LOG << "Not implemented: Node Type " << m_param->type();
		break;
	}
	}

	UpdateValue();
}

void ParameterDelegate::DestroyStructure()
{
	if (m_label) {
		RemoveItem(m_label);
	}

	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
		RemoveItem(m_input.lineEdit);
		delete m_input.lineEdit;
		break;
	case IntType:
		RemoveItem(m_input.spinBox);
		delete m_input.spinBox;
		break;
	case EnumType:
		RemoveItem(m_input.comboBox);
		delete m_input.comboBox;
		break;
	case ButtonType:
		RemoveItem(m_input.pushButton);
		delete m_input.pushButton;
		break;
	default:
		ERR_LOG << "Not implemented: Node Type " << m_param->type();
		break;
	}
}

void ParameterDelegate::UpdateValue()
{
	switch (m_currentType)
	{
	case NoneType:
		break;
	case StringType:
	{
		m_input.lineEdit->SetText(m_param->rawValue().toString());
		break;
	}
	case IntType:
	{
		m_input.spinBox->SetValue(m_param->rawValue().toInt());
		break;
	}
	case EnumType:
	{
		m_input.comboBox->SetCurrentItemIndex(m_param->rawValue().toInt());
		break;
	}
	case ButtonType:
		break;
	default:
	{
		ERR_LOG << "Not implemented: Node Type " << m_param->type();
		break;
	}
	}
}

void ParameterDelegate::UpdateName() {
	if (m_label && m_param) {
		m_label->SetText(m_param->name());
	}
}

void ParameterDelegate::UpdateMenuLabel(int item) {
	if (m_currentType != EnumType) {
		return;
	}

	// TODO: avoid updating all the items? or do we really care?
	std::vector<std::string> labels;
	for (int j = 0; j < Parameter()->menuCount(); ++j) {
		labels.push_back(Parameter()->menuLabel(j));
	}
	m_input.comboBox->SetItemLabels(labels);
}

void ParameterDelegate::InsertMenuItems(int first, int last) {
	UpdateMenuLabel(-1);
}

void ParameterDelegate::RemoveMenuItems(int first, int last) {
	UpdateMenuLabel(-1);
}

void ParameterDelegate::OnParameterDestroyed() {
	m_param = nullptr;
	UpdateStructure();
}
