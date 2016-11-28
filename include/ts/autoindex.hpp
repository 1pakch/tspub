#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP 

#include <ts/series.hpp>
#include <ts/sequence.hpp>

namespace ts {

/// A rule to create an index as a sequence.
///
/// Can be utilized for non-trivial index types such as dates provided
/// Timestamp implements operator+ taking Delta and returning a Timestamp.
template<typename Timestamp, typename Delta=Timestamp>
class AutoIndex : public Sequence<Timestamp, Delta> {
   
  public:
    /// A rule to create an index with a given frequency
    using Sequence<Timestamp, Delta>::Sequence;
    
    /// Creates a Series with the index calculated for the given values.
    template<typename Values>
    Series<Timestamp, typename Values::value_type> zipValues(Values values)
    {
      return Series<Timestamp, typename Values::value_type>(
          (*this).take(values.size()),
          values
      );
    }

    /// Creates a Series with the index calculated for the given values.
    template<typename Value>
    Series<Timestamp, Value> zipValues(std::initializer_list<Value> values)
    {
      return Series<Timestamp, Value>(
          (*this).take(values.size()),
          std::vector<Value>(values)
      );
    }

  private:

};

} // namespace ts

#endif /* AUTOINDEX_HPP */
