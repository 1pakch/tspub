// accumulator.hpp - accumulation of a filter's output in a series.

#ifndef ACCUMULATOR_HPP
#define ACCUMULATOR_HPP

#include <ts/exceptions.hpp>
#include <ts/series.hpp>
#include <ts/na.hpp>


namespace ts {

/// Pushes value to the filter and stores its output in the time series.
template<class Filter, class Timestamp>
class Accumulator
{
  using input_type = typename Filter::input_type;
  using output_type = typename Filter::output_type; 
 
 public:

  Accumulator(Filter filter)
    : filter(filter)
  {}

  /// Process the series. The behavior can be further parametrized if necessary.
  void operator() (Timestamp t, input_type v)
  {
    // update the filter if the input is not NA
    if (na::can_na<input_type>) {
      if (!na::is_na<input_type>(v)) {
        filter(v);
      }
    } else {
      filter(v);
    }
    // write to output if the current output is not NA
    if (na::can_na<output_type>) {
      auto cur_output = filter.value();
      if (!na::is_na<output_type>(cur_output)) {
        output.append(t, filter.value());
      }
    } else {
      output.append(t, filter.value());
    }
  }

  const Series<Timestamp, output_type>& value() const 
  {
    return output;
  }

 private:
  Filter filter; ///< The filter
  Series<Timestamp, output_type> output; //< The result

};


} // namespace ts

#endif /* ACCUMULATOR_HPP */
