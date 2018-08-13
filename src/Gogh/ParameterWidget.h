#ifndef H_PARAMETERWIDGET
#define H_PARAMETERWIDGET

#include <QWidget>
#include "Parameter.h"

class QLineEdit;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLayout;

/**
 * A parameter widget is a widget designed to display and edit any parameter.
 * It monitors the parameter changes and automatically updates when the
 * parameter data is edited somewhere else.
 */
class ParameterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ParameterWidget(QWidget *parent = nullptr);

	/**
	 * Set the parameter controlled by this widget.
	 * This must be a valid pointer or nullptr.
	 */
	void setParameter(Parameter *parm);

	/**
	 * @return the currently controlled parameter or nullptr
	 */
	Parameter * parameter() const { return m_parm; }

private:
	/**
	 * Rebuild the widget content, when controller parameter changes or switch types.
	 */
	void updateStructure();

	/**
	 * Destroy any control widget, prior to reconstruction
	 */
	void destroyStructure();

private slots:
	void updateValue();
	void updateMenuLabel(int item);
	void insertMenuItems(int first, int last);
	void removeMenuItems(int first, int last);

private:
	Parameter *m_parm;

	/// Type for which the parameter widget is currently set
	ParmType m_currentType;

	QLayout *m_layout;

	union
	{
		QWidget *widget;
		QLineEdit* lineEdit;
		QSpinBox* spinBox;
		QComboBox* comboBox;
		QPushButton* pushButton;
	} m_input;
};

#endif // H_PARAMETERWIDGET
