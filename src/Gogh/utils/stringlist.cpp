#include "utils/stringlist.h"

#include <sstream>
#include <vector>
#include <iterator>

template<typename Out>
void split(const std::string & s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

stringlist split(const std::string & s, char delim) {
    stringlist elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

bool replace(std::string & str, const std::string & from, const std::string & to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
