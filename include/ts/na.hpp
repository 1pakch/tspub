// na.hpp - generic API for types internally supporting NA values.
//
// This is different from boost::optional<T> (std::optional<T> in c++17)
// which adds NA value for a given type T by offering a type optional<T>
// having an additional boolean flag.

#ifndef NA_HPP
#define NA_HPP 

#include <cmath>
#include <limits>
#include <typeinfo>
#include <type_traits>
#include <string>
#include <iostream>

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
}

/// Wrapper around a functor optionally skipping NA values - base.
template<typename T, class F>
struct na_guard_base
{
 protected:
  
  F f;

 public:

  na_guard_base(){}
  na_guard_base(F& f): f(f) {}
  na_guard_base(F&& f_): f(std::move(f_)) {}

  //auto value() const -> decltype(auto) { return f.value(); } 
  const F& argument() const { return f; }
  F&& extract_argument(){ return std::move(f); }
};

/// Wrapper around a functor optionally skipping NA values.
template<typename T, class F, bool enabled=true>
struct na_guard: public na_guard_base<T, F>
{
  using na_guard_base<T, F>::na_guard_base;
  
  void operator() (const T& val)
  {
    //std::cout << "na_guard::operator()" << std::endl;
    if (can_na<T>()) {
      if (!is_na<T>(val)) {
        //std::cout << "can_na && f()" << std::endl;
        this->f(val);
      }
    } else {
      //std::cout << "!can_na && f()" << std::endl;
      this->f(val);
    }
  }

  // only called if Timestamp and T are different types
  template<typename Timestamp>
  void operator() (Timestamp t, const T& val)
  {
    if (can_na<T>()) {
      if (!is_na<T>(val)) {
        this->f(t, val);
      }
    } else {
      this->f(t, val);
    }
  }

  void operator() (const T& val1, const T& val2)
  {
    // std::cout << "na_guard::operator()" << std::endl;
    if (can_na<T>()) {
      if (!is_na<T>(val1) && !is_na<T>(val2)) {
        return this->f(val1, val2);
      }
    } else {
      return this->f(val1, val2);
    }
  }
};

/// Wrapper around a functor optionally skipping NA values - disabled.
template<typename T, class F>
struct na_guard<T, F, false>: public na_guard_base<T, F>
{
  using na_guard_base<T, F>::na_guard_base;

  void operator() (const T& val)
  {
    this->f(val);
  }

  // only called if Timestamp and T are different types
  template<typename Timestamp>
  void operator() (Timestamp t, const T& val)
  {
    this->f(t, val);
  }

  void operator() (const T& val1, const T& val2)
  {
    this->f(val1, val2);
  }
};

} // namespace na

} // namespace ts

#endif /* NA_HPP */
