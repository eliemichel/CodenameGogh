#ifndef H_PARAMETERWIDGET
#define H_PARAMETERWIDGET

#include "Ui/UiBase.h"
#include "ParameterType.h"

class Parameter;
class UiTextInput;

/**
 * A parameter widget is a widget designed to display and edit any parameter.
 * It monitors the parameter changes and automatically updates when the
 * parameter data is edited somewhere else.
 */
class ParameterWidget : public UiHBoxLayout
{
public:
	ParameterWidget();

	/**
	 * Set the parameter controlled by this widget.
	 * This must be a valid pointer or nullptr.
	 */
	void SetParameter(::Parameter *param);

	/**
	 * @return the currently controlled parameter or nullptr
	 */
	::Parameter * Parameter() const { return m_param; }

private:
	/**
	 * Rebuild the widget content, when controller parameter changes or switch types.
	 */
	void UpdateStructure();

	/**
	 * Destroy any control widget, prior to reconstruction
	 */
	void DestroyStructure();

private: // slots
	void UpdateValue();
	void UpdateName();
	void UpdateMenuLabel(int item);
	void InsertMenuItems(int first, int last);
	void RemoveMenuItems(int first, int last);

private:
	::Parameter *m_param;

	/// Type for which the parameter widget is currently set
	ParmType m_currentType;

	/// Label with the parameter name
	UiLabel *m_label;

	union
	{
		UiElement *widget;
		UiTextInput* lineEdit;
		UiLabel* spinBox;
		UiLabel* comboBox;
		UiLabel* pushButton;
	} m_input;
};

#endif // H_PARAMETERWIDGET
