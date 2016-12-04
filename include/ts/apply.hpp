// apply.hpp - applying functors to series/vectors.

#ifndef APPLY_HPP
#define APPLY_HPP 

#include <numeric>
#include <vector>
#include <cmath>
#include <utility>

#include <ts/exceptions.hpp>
#include <ts/na.hpp>
#include <ts/aggregators.hpp>


namespace ts {

namespace impl {

/// Get const reference to values in a series.
template<typename Series>
auto valuesView(const Series& s) -> decltype(auto)
{ 
  return s.valuesView();
}

/// Get const referenct to values of a vector.
template<typename Value>
auto valuesView(const std::vector<Value>& x) -> decltype(auto)
{
  return x;
}

} // namespace impl

/// Call a functor on container's values optionally skipping na elements.
template<class Container, class Functor, bool na_guard=true>
Functor apply_values(const Container& container, Functor f=Functor())
{
  // allows for function to work both on series and vectors
  auto values = impl::valuesView(container);
  // get the input type
  using V = typename decltype(values)::value_type;
  // wrapper around the f functor which optionally skips the NA
  // values if supported by the value type and na_guard == true
  auto guarded_f = na::na_guard<V, Functor, na_guard>(
      std::forward<Functor>(f)
  );
  // apply the (possibly na-guarded) functor
  for (auto v: values) { guarded_f(v); }
  // take the functor out of the guard
  return guarded_f.extract_argument();
}

/// Call a functor on index-value pairs optinally skipping NA elements.
template<typename Functor, typename Series, bool na_guard=false>
Functor apply_pairs(const Series& x, Functor f=Functor())
{
  // get the input type
  using V = typename Series::value_type;
  // wrapper around the f functor which optionally skips the NA
  // values if supported by the value type and na_guard == true
  auto guarded_f = na::na_guard<V, Functor, na_guard>(
      std::forward<Functor>(f)
  );
  // apply the (possibly na-guarded) functor on all entries
  for (auto c = x.begin_paired(); c != x.end_paired(); ++c) {
    guarded_f(c.index(), c.value());
  }
  // take the functor out ouf the guard
  return guarded_f.extract_argument();
}


/// Apply a 2d filter on the values aggregated between same index values.
//
/// Aggregates the values in both series until the indices coincide. At
/// this point it calls the functor on aggregated values and continues
/// to iterate the series.
//
template<typename Functor, typename Aggregator, typename Series>
Functor apply2(const Series& x, const Series& y, Functor f=Functor())
{
  auto cx = x.begin_paired(), xend = x.end_paired();
  auto cy = y.begin_paired(), yend = y.end_paired();
  
  // Return if one of the series is empty
  if (cx == xend || cy == yend) { return std::forward<Functor>(f); }
  
  // Construct the aggregators and push the first values
  using V = typename Series::value_type;
  auto aggx = na::na_guard<V, Aggregator>();
  auto aggy = na::na_guard<V, Aggregator>();
  aggx(cx.value());
  aggy(cy.value());

  // wrapper around the f functor which optionally skips the NA
  // values if supported by the value type and na_guard == true
  auto guarded_functor = na::na_guard<V, Functor>( f );

  while (cx != xend && cy != yend) {
    if (cx.index() < cy.index()) {
      // accumulate the next value of the input x
      aggx((++cx).value());
    } else if (cx.index() > cy.index()) {
      // accumulate the next value of the input y
      aggy((++cy).value());
    } else {
      // call the functor on the accumulated values
      guarded_functor(aggx.argument().value(), aggy.argument().value());
      // reset the aggregators
      aggx = na::na_guard<V, Aggregator>();
      aggy = na::na_guard<V, Aggregator>();
      // 
      aggx((++cx).value());
      aggy((++cy).value());
    } 
  }
  return guarded_functor.argument();
}

} // namespace ts

#endif /* APPLY_HPP */
