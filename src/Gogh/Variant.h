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

class Variant : public std::variant<int, float, bool, std::string>
{
private:
	enum TypeIndex {
		IntIndex,
		FloatIndex,
		BoolIndex,
		StringIndex,
	};
public:
	Variant() {}

	// TODO: avoid this duplication
	Variant(int i)
		: std::variant<int, float, bool, std::string>(i)
	{}
	Variant(float f)
		: std::variant<int, float, bool, std::string>(f)
	{}
	Variant(const std::string & str)
		: std::variant<int, float, bool, std::string>(str)
	{}
	Variant(const char *cstr)
		: std::variant<int, float, bool, std::string>(std::string(cstr))
	{}
	Variant(bool b)
		: std::variant<int, float, bool, std::string>(b)
	{}

	Variant operator=(const char *cstr) {
		(*this) = Variant(cstr);
		return *this;
	}

	int toInt() const {
		if (std::holds_alternative<IntIndex>(*this)) {
			return std::get<IntIndex>(*this);
		} else if (std::holds_alternative<FloatIndex>(*this)) {
			return static_cast<int>(std::get<FloatIndex>(*this));
		} else if (std::holds_alternative<BoolIndex>(*this)) {
			return std::get<BoolIndex>(*this) ? 1 : 0;
		} else {
			return 0;
		}
	}

	float toFloat() const {
		if (std::holds_alternative<FloatIndex>(*this)) {
			return std::get<FloatIndex>(*this);
		} else if (std::holds_alternative<IntIndex>(*this)) {
			return static_cast<float>(std::get<IntIndex>(*this));
		} else if (std::holds_alternative<BoolIndex>(*this)) {
			return std::get<BoolIndex>(*this) ? 1.f : 0.f;
		} else {
			return 0.f;
		}
	}

	bool toBool() const {
		if (std::holds_alternative<BoolIndex>(*this)) {
			return std::get<BoolIndex>(*this);
		} else if (std::holds_alternative<IntIndex>(*this)) {
			return std::get<IntIndex>(*this) != 0;
		} else if (std::holds_alternative<FloatIndex>(*this)) {
			return std::get<FloatIndex>(*this) != 0.f;
		} else {
			return false;
		}
	}

	std::string toString() const {
		if (std::holds_alternative<StringIndex>(*this)) {
			return std::get<StringIndex>(*this);
		} else if (std::holds_alternative<IntIndex>(*this)) {
			return std::to_string(std::get<IntIndex>(*this));
		} else if (std::holds_alternative<FloatIndex>(*this)) {
			return std::to_string(std::get<FloatIndex>(*this));
		} else if (std::holds_alternative<BoolIndex>(*this)) {
			return std::get<BoolIndex>(*this) ? "True" : "False";
		} else {
			return std::string();
		}
	}
};


#endif H_VARIANT

