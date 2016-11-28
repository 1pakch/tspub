#ifndef VALIDITY_HPP
#define VALIDITY_HPP

namespace ts {

namespace filters {

/// An estimator counting the number of elements it has processed.
class CountingFilter
{
 private:

   size_t n = 0; ///< the number of processed elements

 protected:
   
  /// Increment the counter
  void inc() { ++n; } 

 public:

  /// Number of processed values
  size_t n_processed() const { return n; }
};


/// An estimator becoming valid after a certain number of observations.
class DeterministicallyValidFilter: public CountingFilter
{
 protected:

  DeterministicallyValidFilter(size_t required_input_size):
    required_input_size(required_input_size)
  {}

 public:

  /// Minimum number of observations before the estimator is valid.
  const size_t required_input_size;
  
  /// Did the estimator process enough observations?
  bool ready() const { return n_processed() >= required_input_size; }

};

} // namespace filters

} // namespace ts 

#endif /* VALIDITY_HPP */
