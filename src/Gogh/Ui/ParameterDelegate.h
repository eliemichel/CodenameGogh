#ifndef H_PARAMETERWIDGET
#define H_PARAMETERWIDGET

#include "Ui/UiBase.h"
#include "Signal.h"
#include "ParameterType.h"

class Parameter;
class UiTextInput;
class UiIntInput;
class UiEnumInput;
class UiButton;
class UiCheckbox;

/**
 * A parameter widget is a widget designed to display and edit any parameter.
 * It monitors the parameter changes and automatically updates when the
 * parameter data is edited somewhere else.
 */
class ParameterDelegate : public UiHBoxLayout
{
public:
	ParameterDelegate(UiLayout *popupLayout = nullptr);
	~ParameterDelegate();

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
	 * Connect parameter's signal to this delegate's slots
	 */
	void ConnectSlots();
	/**
	 * Disconnect parameter's signal from this delegate's slots
	 */
	void DisconnectSlots();

	/**
	 * Rebuild the widget content, when controller parameter changes or switch types.
	 */
	void UpdateStructure();

	/**
	 * Destroy any control widget, prior to reconstruction
	 */
	void DestroyStructure();

public: // signal
	Signal<> buttonClicked;

private: // slots
	void UpdateValue();
	void UpdateName();
	void UpdateMenuLabel(int item);
	void InsertMenuItems(int first, int last);
	void RemoveMenuItems(int first, int last);
	void OnParameterDestroyed();

private:
	::Parameter *m_param;

	/// Type for which the parameter widget is currently set
	ParmType m_currentType;

	/// Label with the parameter name
	UiLabel *m_label;

	/// Layout used to pop up enum menu
	UiLayout *m_popupLayout;

	SignalConnection m_valueChangedConnection;
	SignalConnection m_nameChangedConnection;
	SignalConnection m_typeChangedConnection;
	SignalConnection m_menuLabelChangedConnection;
	SignalConnection m_aboutToInsertMenuItemsConnection;
	SignalConnection m_aboutToRemoveMenuItemsConnection;
	SignalConnection m_destroyedConnection;

	// TODO: use a variant
	union
	{
		UiElement *widget;
		UiTextInput* lineEdit;
		UiIntInput* spinBox;
		UiEnumInput* comboBox;
		UiButton* pushButton;
		UiCheckbox* checkbox;
	} m_input;
};

#endif // H_PARAMETERWIDGET
