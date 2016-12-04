// covariance.hpp - calculation of covariances/correlatons.

#ifndef COVARIANCE_HPP
#define COVARIANCE_HPP

#include <numeric>
#include <vector>

#include <ts/exceptions.hpp>
#include <ts/aggregators.hpp>
#include <ts/na_guard.hpp>
#include <ts/filters/online_moments.hpp>


namespace ts {

namespace impl {

/// Apply a 2d filter on the values aggregated between same index values.
//
/// Aggregates the values in both series until the indices coincide. At
/// this point it calls the functor on aggregated values and continues
/// to iterate the series.
//
template<typename Aggregator, typename Functor,
         typename Timestamp, typename Value>
Functor& aggregate_and_apply(
    Functor& f,
    const Series<Timestamp, Value>& x,
    const Series<Timestamp, Value>& y
){
  auto cx = x.begin_paired(), xend = x.end_paired();
  auto cy = y.begin_paired(), yend = y.end_paired();
  
  // Return if one of the series is empty
  if (cx == xend || cy == yend) { return f; }
  
  // Construct the aggregators and push the first values
  auto aggx = Aggregator();
  auto aggy = Aggregator();
  aggx(cx.value());
  aggy(cy.value());

  // wrapper around the f functor which optionally skips the NA
  // values if supported by the value type and na_guard == true
  using AggResult = decltype(Aggregator().value());
  auto guarded_functor = na::na_guard<Functor, AggResult>( f );

  while (cx != xend && cy != yend) {
    if (cx.index() < cy.index()) {
      // accumulate the next value of the input x
      aggx((++cx).value());
    } else if (cx.index() > cy.index()) {
      // accumulate the next value of the input y
      aggy((++cy).value());
    } else {
      // call the functor on the accumulated values
      guarded_functor(aggx.value(), aggy.value());
      // reset the aggregators
      aggx = Aggregator();
      aggy = Aggregator();
      // 
      aggx((++cx).value());
      aggy((++cy).value());
    } 
  }
  return f;
}

} //namespace impl

//
// Public API for computing moments of Series/vectors
//
/*! \file */ 

// Apply a covariance filter to the series (unknown means).
template<typename Aggregator=Sum, typename Series>
auto apply_cov(const Series& x, const Series& y) -> decltype(auto)
{
  auto est = filters::OnlineCovUnknownMeans();
  impl::aggregate_and_apply<Aggregator>( est, x, y );
  return est;
}

// Apply a covariance filter to the series (known means).
template<typename Aggregator=Sum, typename Series>
auto apply_cov(const Series& x, const Series& y,
               double x_mean, double y_mean) -> decltype(auto)
{
  auto est = filters::OnlineCovKnownMeans(x_mean, y_mean);
  impl::aggregate_and_apply<Aggregator>( est, x, y );
  return est;
}

// Covariance of two series with unknown means.
template<typename Aggregator=Sum, typename Series>
double cov(const Series& x, const Series& y)
{
  return apply_cov<Aggregator>(x, y).cov();
}

// Covariance of two series with known means.
template<typename Aggregator=Sum, typename Series>
double cov(const Series& x, const Series& y,
           double x_mean, double y_mean)
{
  return apply_cov<Aggregator>(x, y, x_mean, y_mean).cov();
}

// Correlation of two series with unknown means.
template<typename Aggregator=Sum, typename Series>
double corr(const Series& x, const Series& y)
{
  return apply_cov<Aggregator>(x, y).corr();
}

// Correlation of two series with known means.
template<typename Aggregator=Sum, typename Series>
double corr(const Series& x, const Series& y, double x_mean, double y_mean)
{
  return apply_cov<Aggregator>(x, y, x_mean, y_mean).corr();
}

} // namespace ts

#endif /* COVARIANCE_HPP */
