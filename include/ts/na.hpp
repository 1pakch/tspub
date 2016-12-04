// na.hpp - support for types with NA values.

#ifndef NA_HPP
#define NA_HPP 

#include <cmath>
#include <limits>
#include <typeinfo>
#include <string>

#include <ts/exceptions.hpp>


namespace ts {

namespace na {

template<typename T>
struct NaNotSupported: public TsException
{
  NaNotSupported():
    TsException(
      std::string("No support for NA values for type ")
      + typeid(T).name()
    )
  {}
};

// Does the type has a NA value?
template<class T> constexpr bool can_na(){ return false; }
template<> constexpr bool can_na<double>(){ return true; };
template<> constexpr bool can_na<float>(){ return true; };

// Is the given value NA?
template<class T> constexpr bool is_na(T v){ throw NaNotSupported<T>(); }
template<> constexpr bool is_na(double v){ return std::isnan(v); };
template<> constexpr bool is_na(float v){ return std::isnan(v); };
// one can add specializations for custom types

/// Create a NA value for the given type()
template<class T> constexpr T na()
{
  if (std::numeric_limits<T>::has_quiet_NaN) {
    return std::numeric_limits<T>::quiet_NaN();
  } else {
    throw NaNotSupported<T>();
  }
};

} // namespace na

} // namespace ts

#endif /* NA_HPP */
