#pragma once

#include <functional>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGlobal>

#if QT_VERSION < 0x050e00

namespace std
{
template<>
struct hash<QString>
{
  inline std::size_t
  operator()(QString const &s) const
  {
    return qHash(s);
  }
};
}

#endif // QT_VERSION
