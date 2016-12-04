// na_guard.hpp - functor proxy optionally skipping NA values.

#ifndef NA_GUARD_HPP
#define NA_GUARD_HPP 

#include <ts/na.hpp>


namespace ts {

namespace na {

// implementation
namespace impl {

/// functor proxy - NA checks off.
template<typename Functor, typename Value, bool On = false>
class na_guard_base
{
  Functor& f;

 public:

  na_guard_base(){}
  na_guard_base(Functor& f): f(f) {}

  /// Forward the call (value type cannot be NA).
  void operator() (Value val)
  {
    f(val);
  }

  /// Forward the call (value type cannot be NA).
  template<typename Timestamp>
  void operator() (Timestamp t, Value val)
  {
    f(t, val);
  }

  /// Forward the call (value type cannot be NA).
  void operator() (Value val1, Value val2)
  {
    f(val1, val2);
  }
};

/// functor proxy - NA checks on.
template<typename Functor, typename Value>
class na_guard_base<Functor, Value, true>
{
  Functor& f;

 public:

  na_guard_base(){}
  na_guard_base(Functor& f): f(f) {}

  /// Forward the call (value type can be NA).
  void operator() (Value val)
  {
    if (!is_na<Value>(val)) { f(val); }
  }

  /// Forward the call (value type can be NA).
  template<typename Timestamp>
  void operator() (Timestamp t, Value val)
  {
    if (!is_na<Value>(val)) {
      f(t, val);
    }
  }
  /// Forward the call (value type can be NA).
  void operator() (Value val1, Value val2)
  {
    if (!is_na<Value>(val1) && !is_na<Value>(val2)) {
      f(val1, val2);
    }
  }
};

} // namespace impl

/// Wrapper around a functor optionally skipping NA values.
template<typename Functor, typename Value>
class na_guard: public impl::na_guard_base<Functor, Value, na::can_na<Value>()>
{
  using impl::na_guard_base<Functor, Value, na::can_na<Value>()>::na_guard_base;
};

} // namespace na

} // namespace ts

#endif /* NA_GUARD_HPP */
