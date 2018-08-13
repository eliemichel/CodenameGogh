#ifndef H_PARAMETER
#define H_PARAMETER

#include <QObject>
#include <QString>
#include <QVariant>
#include <vector>

enum ParmType
{
	NoneType,
	StringType,
	IntType,
	EnumType,
	ButtonType,
};

/**
 * A parameter is a user editable property of a node. They represent properties
 * that are specific to a given node, and not generic properties like node
 * position or name. The latter are rather provided node attributes @see Node.
 */
class Parameter : public QObject
{
	Q_OBJECT

private:
	struct MenuItem
	{
		QString label;
		QVariant value;
	};

public:
	Parameter(QObject *parent = nullptr);

	// // Getters // //

	/// Name of the parameter, used for display
	inline QString name() const { return m_name; }

	/// Type of parameter @see ParmType
	inline ParmType type() const { return m_type; }

	/// Raw value of the parameter. It is often prefered to use one of the eval functions
	inline QVariant rawValue() const { return m_rawValue; }

	/**
	 * Number of menu items in this parameter.
	 * Menu items are used for parm of type EnumType
	 * A menu item is made of a display label and an internal value. The latter
	 * is used when evaluating an enum parameter.
	 */
	int menuCount() const;

	/// Label of the menu item at index <item>, used for display
	QString menuLabel(int item) const;

	/// Value of the menu item at index <item>, used parameter for evaluation
	QVariant menuValue(int item) const;

	// // Setters // //

	/**
	 * Set the raw value of the parameter. This fires a valueChanged() signal
	 * and returns true if the value is valid for this parameter.
	 */
	bool set(const QVariant & value);

	/**
	 * Set the label of a menu entry if <item> refers to a valid menu item, and
	 * emit a menuLabelChanged signal.
	 */
	void setMenuLabel(int item, const QString & label);

	/**
	* Set the label of a menu entry if <item> refers to a valid menu item, and
	* emit a menuLabelChanged signal.
	*/
	void setMenuValue(int item, const QVariant & value);

	/**
	 * Insert new menu items. The inserted items will have indexes from <first>
	 * to <last> and the next items will be shifted by last - first + 1.
	 * The emits the signal aboutToInsertMenuItems before changing the underlying data
	 */
	void insertMenuItems(int first, int last);

	/**
	* Remove menu items from <first> to <last> included.
	* The emits the signal aboutToDeleteMenuItems before changing the underlying data
	*/
	void removeMenuItems(int first, int last);

	// // Evaluators // //

	// TODO:
	//QString evalAsString(int parm) const;
	//int evalAsInt(int parm) const;

signals:
	/// emitted when the raw value of the parameter changed
	void valueChanged();

	/// emitted when the menu item label at <item> changed
	void menuLabelChanged(int item);

	/// emitted when the menu item value at <item> changed
	void menuValueChanged(int item);

	/// emitted before inserting new items from position <first> to <last> included
	void aboutToInsertMenuItems(int first, int last);

	/// emitted before removing items from position <first> to <last> included
	void aboutToRemoveMenuItems(int first, int last);

private:
	QString m_name;
	ParmType m_type;
	QVariant m_rawValue;
	std::vector<MenuItem> m_menu;
};

#endif // H_PARAMETER
