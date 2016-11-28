// circular_buffer.hpp - circular buffer and the indirect comparison operator

#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP 

#include <vector>

namespace ts {

namespace filters {

namespace impl {

/// A simple circular buffer. New values overwrite the oldest ones.
template<typename T>
class CircularBuffer 
{
 public:
  using value_type = T;

  CircularBuffer(size_t size)
    : pos_(0),
      buf_(size),
      full_(false)
  {}

  /// Number of valid elements
  size_t count() const { return full_ ? size() : pos_; };

  /// Buffer capacity
  size_t size() const { return buf_.size(); };

  /// The writing position
  size_t pos() const { return pos_; }

  /// Is the buffer full?
  bool full() const { return full_; };
  
  /// Access operator. No checks.
  T operator[] (size_t index) const
  {
    return buf_[index];
  }

  /// Writes a new value and returns the old one
  T write(T in)
  {
    auto oldVal = buf_[pos_];
    if (!full_ && pos_ == 0) {
      // first insertion ever
      buf_[0] = in;
      pos_ = 1;
    } else {
      buf_[pos_] = in;
      pos_ = (pos_ + 1) % size();
      full_ = full_ || pos_ == 0;
    }
    return oldVal;
  }

 private:

  std::vector<T> buf_;   ///< The buffer
  size_t pos_;           ///< Current position
  bool full_;            ///< Is the buffer filled?
   
};

/// Compare the indices w.r.t. to the values they point to
template<class Values, class Compare=std::less<typename Values::value_type> >
class CompareReferencedValues
{
 private:

  const Values& values; ///< Reference to the values
  Compare comp; ///< Comparison operator for the values

 public:

  CompareReferencedValues(
      const Values& values,
      const Compare& comp = Compare()
  )
    : values(values), comp(comp)
  {}
  
  /// Compares values[i] and values[j]
  bool operator() (size_t i, size_t j) const
  {
    return comp(values[i], values[j]);
  }
};

} // namespace impl

} // namespace filters

} // namespace ts

#endif /* CIRCULAR_BUFFER_HPP */
