#pragma once

#include <QString>

namespace Gogh {

class Style {
public:
	/**
	 * Utility function returning the css style for node data model classes
	 */
	static QString nodeStyle();

	static QString dialogStyle();
};

} // namespace Gogh
