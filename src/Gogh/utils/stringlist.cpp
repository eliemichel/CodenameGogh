#include "utils/stringlist.h"

#include "Logger.h"

#include <sstream>
#include <vector>
#include <iterator>
#include <string>

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

std::string toString(stringlist sl)
{
  std::string s = "";
  for (int i = 0; i  < sl.size(); i++)
  {
    s += sl[i];
  }
  return s;
}
