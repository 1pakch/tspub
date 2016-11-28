// aggregators.hpp - aggregation functors (sum, first, last).

#ifndef AGGREGATORS_HPP
#define AGGREGATORS_HPP 

#include <ts/na.hpp>


namespace ts {

// Functor computing the sum of the values; initially nan.
struct Sum
{
  Sum(): val(na::na<double>()) {}
  void operator() (double x){ val = na::is_na<double>(val) ? x : val + x; }
  double value() const { return val; }
 private:
  double val;
};

// Functor memorizing the first value; initially nan.
struct Last
{
  Last(): val(na::na<double>()) {}
  void operator() (double x){ val = x; }
  double value() const { return val; }
 private:
  double val;
};

// Functor memorizing the last value; initially nan.
struct First
{
  First(): val(na::na<double>()) {}
  void operator() (double x){ val = na::is_na<double>(val) ? x : val; }
  double value() const { return val; }
 private:
  double val;
};

}

#endif /* AGGREGATORS_HPP */
