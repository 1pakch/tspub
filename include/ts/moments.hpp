// moments.hpp - calculation of moments for vectors and series

#ifndef MOMENTS_HPP
#define MOMENTS_HPP

#include <numeric>
#include <vector>

#include <ts/exceptions.hpp>
#include <ts/apply.hpp>
#include <ts/aggregators.hpp>
#include <ts/filters/online_moments.hpp>


namespace ts {

namespace moments {

//
// Public API for computing moments of Series/vectors
//
/*! \file */ 

/// The mean of a container.
template<bool skipna=true, typename Values>
double mean(const Values& values)
{
  using Est = filters::OnlineMean;
  auto est = apply_values<Values, Est, skipna>( values );
  if (!est.ready()) {
    throw SizeError(
        "ts::mean(): input too small or too many missing values."
    );
  }
  return est.value();
}

/// The variance of a container using a one-pass algorithm.
template<bool skipna=true, typename Values>
double var(const Values& values)
{
  using Est = filters::OnlineVarUnknownMean;
  auto est = apply_values<Values, Est, skipna>( values );
  if (!est.ready()) {
    throw SizeError(
        "ts::var(): input too small or too many missing values."
    );
  }
  return est.value();
}

/// The variance of a vector with a known mean.
template<bool skipna=true, typename Values>
double var(const Values& values, double mean)
{
  using Est = filters::OnlineVarKnownMean;
  auto est = apply_values<Values, Est, skipna>( values, Est(mean) );
  if (!est.ready()) {
    throw SizeError(
        "ts::var(): input too small or too many missing values."
    );
  }
  return est.value();
}

// Covariance of two series with unknown means.
template<typename Aggregator=Sum, typename Series>
double cov(const Series& x, const Series& y)
{
  using Est = filters::OnlineCovUnknownMeans;
  auto est = apply2<Est, Aggregator>( x, y );
  if (!est.ready()) {
    throw SizeError("moments::cov(): not enough non-missing values.");
  } 
  return est.value();
}

// Covariance of two series with known means.
template<typename Aggregator=Sum, typename Series>
double cov(const Series& x, const Series& y, double x_mean, double y_mean)
{
  using Est = filters::OnlineCovKnownMeans;
  auto est = apply2<Est, Aggregator>( x, y, Est(x_mean, y_mean) );
  if (!est.ready()) {
    throw SizeError("moments::cov(): not enough non-missing values.");
  } 
  return est.value();
}


// Correlation of two series with unknown means.
template<typename Aggregator=Sum, typename Series>
double corr(const Series& x, const Series& y)
{
  using Est = filters::OnlineCovUnknownMeans;
  auto est = apply2<Est, Aggregator>( x, y );
  if (!est.ready()) {
    throw SizeError("moments::corr(): not enough non-missing values.");
  } 
  return est.corr();
}

// Correlation of two series with known means.
template<typename Aggregator=Sum, typename Series>
double corr(const Series& x, const Series& y, double x_mean, double y_mean)
{
  using Est = filters::OnlineCovKnownMeans;
  auto est = apply2<Est, Aggregator>( x, y, Est(x_mean, y_mean) );
  if (!est.ready()) {
    throw SizeError("moments::corr(): not enough non-missing values.");
  }
  return est.corr();
}

} // namespace moments

} // namespace ts

#endif /* MOMENTS_HPP */
