#ifndef H_VARIANT
#define  H_VARIANT

#if CPP17 // this variable does not exist, this is just for the semantics
#include <variant>
#else // CPP17
// Polyfill C++17 variant
#include "polyfill/variant.hpp"
namespace std {
	using mpark::variant;
	using mpark::get;
	using mpark::holds_alternative;
};
#endif // CPP17

#include <string>

class Variant
{
private:
	enum TypeIndex {
		IntIndex
	};
public:
	// TODO
	//variant & operator=(const variant&);
	//variant& operator=(variant&&) noexcept(see below);
	//template <class T> variant& operator=(T&&) noexcept(see below);

	template <typename T>
	Variant operator=(const T & value) {
		content = value;
		return *this;
	}

	int toInt() const {
		if (std::holds_alternative<IntIndex>(content)) {
			return std::get<IntIndex>(content);
		} else {
			return 0;
		}
	}

private:
	std::variant<int, float, bool, std::string> content;
};


#endif H_VARIANT

