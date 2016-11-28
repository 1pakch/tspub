// rolling_median.hpp - implementation of a rolling median

#ifndef ROLLING_MEDIAN_HPP
#define ROLLING_MEDIAN_HPP 

#include <typeinfo>
#include <vector>
#include <set>
#include <iostream>

#include <ts/exceptions.hpp>
#include <ts/na.hpp>
#include <ts/filters/circular_buffer.hpp>


namespace ts {

namespace filters {

namespace impl {

/// A (sorted) multiset with convenience methods.
template<typename T, class Compare>
class MultiSet: public std::multiset<T, Compare>
{
 public:
  MultiSet(const Compare& comp)
    : std::multiset<T, Compare>(comp)
  {}

  /// First element
  T first() const { return *(this->begin()); }

  /// Last element
  T last() const { return *(--this->end()); }
  
  /// Removes a first occurence of a given value
  ///
  /// The method is different from multiset::erase(val) which removes
  /// all the elements identical to the given one w.r.t. to the comparison
  bool remove_first(T val)
  {
    auto it = this->begin();
    while (it != this->end()) {
      if (*it == val) {
        this->erase(it);
        return true;
      }
      ++it;
    }
    return false;
  }
};

/// Prints the mapping from the indices to the values
template<class Indices, class Values>
void print_map(
    const Indices& indices,
    const Values& values
){
  for (auto it = indices.cbegin(); it != indices.cend(); ++it) {
    std::cout << *it << " -> " <<  values[*it] << ", ";
  }
  std::cout << std::endl;
}

} // namespace impl


/// Rolling median filter
///
/// The data in the current window is stored in a circular buffer (valuesBuf).
/// The index of each element belongs to one of the two sets: upperInds and
/// lowerInds. Both sets are sorted so that the values referenced by the
/// consequent indices are nondecreasing and the values referenced by
/// lowerInds are not greater than those referenced by upperInds.
///
/// Moreover, the numbers of elements in both sets are either equal or
/// lowerInds have one element more. The median equals either the average of
/// the valuesBuf[last(lowerInds)] and valuesBuf[first(upperInds)] or
/// valuesBuf[last(lowerInds)].
///
/// The filter is considered to be ready to provide an ouput only when
/// the window_size observations has been processed. To have expanding
/// window behavior one could add another parameter min_window_size so
/// that the filter will be ready once that many observations has been
/// read.

template<typename T=double>
class RollingMedian
{
 public:
  typedef T input_type;
  typedef T output_type;

 private:

  using CircularBuffer = impl::CircularBuffer<T>;
  using ValuesLess = impl::CompareReferencedValues<CircularBuffer>;
  using IndexSet = impl::MultiSet<size_t, ValuesLess>;

  CircularBuffer valuesBuf; ///< Circular buffer of values
  ValuesLess valuesLess; ///< Compares values given indices
  IndexSet upperInds; ///< The indices of values below or equal to the median
  IndexSet lowerInds; ///< The indices of values above the median

 public:

  /// Constructs a rolling median filter with a given window size
  RollingMedian(size_t window_size)
    : valuesBuf(window_size),
      valuesLess(valuesBuf), // compares the values given indices
      upperInds(valuesLess), // initialize the sets
      lowerInds(valuesLess)
  {
    if (window_size < 2)
      throw TsException(
          "RollingMedianFilter(): window_size must be at least 2"
      );
  }

  /// Need a copy ctor because the comparator references the values buffer.
  RollingMedian(const RollingMedian& f)
    : valuesBuf(f.valuesBuf),
      valuesLess(valuesBuf), // reinitialize with the new buffer!!!
      upperInds(valuesLess),
      lowerInds(valuesLess)
  {}

  /// Is the buffer already full?
  bool ready() const { return valuesBuf.full(); }

  /// Returns the current median
  T value() const
  {
    if (!ready()) return na::na<T>();
  /// Returns the current value of the mean
    int k = upperInds.size() - lowerInds.size();
    if (k == 0) {
      return (valuesBuf[upperInds.first()]
            + valuesBuf[lowerInds.last()]) / 2.0;
    }
    else if (k > 0)
      return valuesBuf[upperInds.first()];
    else if (k < 0)
      return valuesBuf[lowerInds.last()];
    else {
      // should not happen
      throw TsException(
        "Difference of sizes of upperInds and lowerInds sets > 2"
      );
    }
  }
  
  /// Put the new observation in and return the updated median
  T operator() (T in)
  {
    if (valuesBuf.count() == 0) {
      // first insertion ever
      valuesBuf.write(in);
      upperInds.insert(0);
    } else {
      // any other update
      auto pos = valuesBuf.pos();
      auto old_med = value();
      // remove the reference to the old value
      if (valuesBuf.full()) {
        upperInds.remove_first(pos);
        lowerInds.remove_first(pos);
      }
      // put the new value in the valuesBuffer
      valuesBuf.write(in);
      // add the reference to the new value
      if (in < old_med) {
        lowerInds.insert(pos);
      } else {
        upperInds.insert(pos);
      }
      // ensure the number of elements in the upperInds and the lowerInds sets
      // do not differ by more than 1 
      rebalance();
    }
    return value();
  }

  /// Print the state of the filter (for debugging).
  void print_state()
  {
    std::cout << "Lower: ";
    impl::print_map<IndexSet, CircularBuffer>(lowerInds, valuesBuf);
    std::cout << "Upper: ";
    impl::print_map<IndexSet, CircularBuffer>(upperInds, valuesBuf);
    std::cout << "Value = " <<  value() << std::endl;
    std::cout << "Pos = " << valuesBuf.pos() << std::endl << std::endl;
  }

 private:

  // Rebalance if the number of elements in the the upperInds
  // and the lowerInds sets differs by more than one
  void rebalance()
  {
    int k = upperInds.size() - lowerInds.size();
    if (k > 1) {
      auto ix = upperInds.first();
      upperInds.remove_first(ix);
      lowerInds.insert(ix);
    }
    else if (k < -1) {
      auto ix = lowerInds.last();
      lowerInds.remove_first(ix);
      upperInds.insert(ix);
    }
  }
};

} // namespace filters

} // namespace ts

#endif /* ROLLING_MEDIAN_HPP */
