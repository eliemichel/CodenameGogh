#include <string>
#include <vector>

typedef std::vector<std::string> stringlist;

template<typename Out>
void split(const std::string & s, char delim, Out result);

stringlist split(const std::string & s, char delim);

std::string toString(stringlist sl);

// Overloaders
std::ostream& operator<<(std::ostream& stream, const stringlist& sl);
