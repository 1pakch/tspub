// rolling_mean.hpp - implementation of a rolling mean

#ifndef ROLLING_MEAN_HPP
#define ROLLING_MEAN_HPP 

#include <vector>

#include <ts/exceptions.hpp>
#include <ts/na.hpp>
#include <ts/filters/circular_buffer.hpp>


namespace ts {

namespace filters {

/// Simple moving average using a circular buffer.
///
/// The filter is considered to be ready to provide an ouput only when
/// the window_size observations has been processed. To have expanding
/// window behavior one could add another parameter min_window_size so
/// that the filter will be ready once that many observations has been
/// read.
/// 
class RollingMean
{
 public:
  typedef double input_type;
  typedef double output_type;
  
  /// Constructs the RollingMean filter
  RollingMean(size_t window_size):
    mean_(0),
    k_(1.0 / window_size),
    buf(window_size)
  {}

  /// Returns the current value of the mean
  double value() const { return ready() ? mean_ : na::na<double>();  }

  /// Are we ready to provide the output?
  bool ready() const { return buf.full(); }

  /// Puts the new observation in the window and returns the mean 
  double operator() (double valIn)
  {
    if (buf.full()) {
      auto valOut = buf.write(valIn);
      mean_ += k_ * (valIn - valOut);
    } else {
      buf.write(valIn);
      mean_ += k_ * valIn;
    }
    return mean_;
  }
  
 private:
  double mean_;
  double k_;
  impl::CircularBuffer<double> buf;
};

} // namespace filters

} // namespace ts

#endif /* ROLLING_MEAN_HPP */
