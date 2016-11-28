// series.hpp - the time series class

#ifndef SERIES_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <initializer_list>
#include <numeric>
#include <sstream>
#include <algorithm>

#include "exceptions.hpp"
#include "sequence.hpp"


namespace ts {

/// Tuple of iterators with increment operations.
template<typename Itr1, typename Itr2>
struct IndexValueIter: public std::pair<Itr1, Itr2>
{
  typedef IndexValueIter<Itr1, Itr2> this_type;

  // Inherit constructors
  using std::pair<Itr1, Itr2>::pair;

  /// Prefix increment
  this_type& operator++()
  {
    ++(this->first);
    ++(this->second);
    return *this;
  }

  /// Prefix decrement
  this_type& operator--()
  {
    --(this->first);
    --(this->second);
    return *this;
  }
  
  /// Postfix increment
  this_type operator++(int)
  {
    auto old = *this;
    operator++();
    return old;
  }

  /// Postfix decrement
  this_type operator--(int)
  {
    auto old = *this;
    operator--();
    return old;
  }

  /// Iterator to the index
  auto indexIter() const -> decltype(auto) { return this->first; }
  
  /// Iterator on the values
  auto valueIter() const -> decltype(auto) { return this->second; }

  /// Index at the current position
  auto index() const -> decltype(auto) { return *(indexIter()); }
  
  /// Value at the current position
  auto value() const -> decltype(auto) { return *(valueIter()); }
};


/// A class for storing ordered time series data.
///
/// Optimal internal storage depends on the usage scenarios.
/// I assumed that the time series objects will be mostly stored
/// elsewhere in a sorted way (if the index goes from oldest to newest)
/// or appended with new observations in real time.
///
/// Internally the index is stored as a sorted vector. To find the element
/// by index the binary search is used.
///
template<typename Timestamp, typename Value=double>
class Series{
 public: // declarations

  typedef Series<Timestamp, Value> this_type;
  typedef Timestamp timestamp_type;
  typedef Value value_type;
  typedef typename std::vector<Timestamp> index_type;
  typedef typename std::vector<Value> values_type;
  typedef IndexValueIter<typename index_type::const_iterator,
                         typename values_type::const_iterator>
                         paired_iterator_type;

 private: // variables

  index_type index;
  values_type values;

 public: // methods

  /// Creates an empty Series
  Series(){};

  /// Creates a Series from index and value vectors
  Series(const index_type& index_, const values_type& values_):
    index(index_),
    values(values_)
  {
    post_construction_checks();
  }

  /// Creates a Series from temp index and value vectors or init lists.
  Series(index_type&& index_, values_type&& values_):
    // not having underscores in parameter names does not work
    index(std::move(index_)),
    values(std::move(values_))
  {
    post_construction_checks();
  }

  /// Returns the length of the time series
  size_t size() const { return index.size(); }

  //auto begin() -> decltype(auto) const { return values.cbegin(); }; 
  //auto end() -> decltype(auto) const { return values.cend(); }; 

  /// Adds a new observation at the end. Throws a TsException
  /// if the new observation's index precedes the last value of
  /// the current index
  void append(Timestamp ix, Value val);

  /// Finds the value corresponding to a given index value
  Value& at(Timestamp x);

  /// Finds the value corresponding to a given index value
  Value& operator[](Timestamp x){ return at(x); }

  /// Compares first the index and then the values
  bool operator==(const this_type& other) const;

  /// Returns the read-only "view" of the index
  const index_type& indexView() const { return index; }

  /// Returns the read-only "view" of an index
  const values_type& valuesView() const { return values; }

  /// Paired (index, value) iterators to the beginning
  paired_iterator_type begin_paired() const
  {
    return paired_iterator_type(index.cbegin(), values.cbegin());
  }

  /// Paired (index, value) iterators to the end
  paired_iterator_type end_paired() const
  {
    return paired_iterator_type(index.cend(), values.cend());
  }
  
  /// Convert to a human-readable string
  std::string to_string(std::string sep=std::string(", ")) const;

private: // methods

  /// Checks if the index is sorted and has the same size as the values.
  void post_construction_checks();

};

//
// Implementation of longer methods
//

template<typename Timestamp, typename Value>
void Series<Timestamp, Value>::post_construction_checks()
{
  if (index.size() != values.size()) {
    throw SizeError("The index and the values must be of the same size.");
  }
  if (!std::is_sorted(index.begin(), index.end())) {
    throw IndexNotSorted("Provided a non-sorted index in a constructor.");
  };
}

template<typename Timestamp, typename Value>
void Series<Timestamp, Value>::append(Timestamp ix, Value val)
{
  if (index.size() > 0 && ix <= index.back()) {
    throw IndexNotSorted(
      "Appending with a timestamp not greater than the last index element."
    );
  }
  index.push_back(ix);
  values.push_back(val);
}

template<typename Timestamp, typename Value>
Value& Series<Timestamp, Value>::at(Timestamp x)
{
  auto begin = index.cbegin();
  auto end = index.cend();
  auto loc = std::lower_bound(begin, end, x);
  if (loc == end){
    throw IndexError<Timestamp>(x);
  }
  return values[loc - begin];
}

// Equality comparison operators
template<typename Timestamp, typename Value>
bool Series<Timestamp, Value>::operator==(const this_type& other) const
{
  return (indexView() == other.indexView()
          && valuesView() == other.valuesView());
}

template<typename Timestamp, typename Value>
std::string Series<Timestamp, Value>::to_string(std::string sep) const
{
  std::ostringstream out;
  for (size_t i=0; i < size(); ++i)
  {
    out << indexView()[i] << ":" << valuesView()[i] << sep;
  }
  return out.str();
}

} // namespace ts

#define SERIES_HPP
#endif /* SERIES_HPP */
