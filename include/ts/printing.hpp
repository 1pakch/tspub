// printing.hpp - ostream output operator for the custom classes

#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <map>
#include <iomanip>
#include <ios>
#include <initializer_list>

#include <ts/merge.hpp>


namespace ts {

namespace printing {

/// A printing operator for all types T providing t.to_string(). All the
/// other types use their default operators due to SFINAE in the decltype.
/// Enabled by `using namespace ts::printing`.
template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.to_string(), os) 
{
    os << t.to_string();
    return os; 
}

/// A struct defining the parameters for printing Series in columns.
struct SeriesPrintSettings
{
  const int index_width = 4;
  
  const int values_width = 4;
  
  const std::string index_value_sep = std::string(" | ");

  const int get_line_width(int n_series) {
    return index_width
            + index_value_sep.length()
            + values_width * n_series;
  }
};


// Forward declaration of the implementation
namespace impl {
template<typename Series> class SeriesPrinter;
}


/// Prints a collection of series.
template<class Timestamp, class Value>
void print(std::ostream& s,
           std::vector<const Series<Timestamp, Value>* >& pseries,
           SeriesPrintSettings settings=SeriesPrintSettings())
{
  typedef Series<Timestamp, Value> S;
  auto p = impl::SeriesPrinter<S>(settings);
  //auto mergeitr = col.merge_iterator();
  auto mergeitr = MergeIterator<S>::from_pointers(pseries);
  const int N = mergeitr.n_series();
  typename S::timestamp_type curind;
  std::map<int, typename S::value_type> vals;

  p.print_header(s, N);
  // the main loop over the values
  if (!mergeitr.at_end()) curind = mergeitr.timestamp();
  while (!mergeitr.at_end())
  {
    if (curind != mergeitr.timestamp()) {
      // print all the values collected for the previous index
      p.print_values(s, curind, vals, N);
      // clear the collected values
      vals.clear();
      // remember the new index
      curind = mergeitr.timestamp();
    }
    // remember the current observation
    vals[mergeitr.series()] = mergeitr.value();
    ++mergeitr;
  }
  p.print_values(s, curind, vals, N);
}


/// Prints one series.
template<typename Timestamp, typename Value>
void print(std::ostream& s,
           const Series<Timestamp, Value>& x,
           SeriesPrintSettings settings=SeriesPrintSettings())
{
  typedef Series<Timestamp, Value> S;
  std::vector<const S*> col;
  col.push_back(&x);
  print(s, col, settings);
}


/// Prints series given an initializer list.
template<typename Timestamp, typename Value>
void print(std::ostream& s,
           std::initializer_list<const Series<Timestamp, Value>*> list,
           SeriesPrintSettings settings=SeriesPrintSettings())
{
  typedef Series<Timestamp, Value> S;
  std::vector<const S*> col(list);
  print(s, col, settings);
}


namespace impl {

// Implementation of the printer class
template<typename Series>
class SeriesPrinter
{
 public:

  typedef std::ios_base::fmtflags fmtflags;

  SeriesPrintSettings set;

 public:

  SeriesPrinter(
      SeriesPrintSettings settings=SeriesPrintSettings()
  )
  {}

 public:

  /// Prints the header and the separator line.
  template<class Stream>
  void print_header(Stream& s, int n_series)
  {
    // print the top line with 
    auto indices = Sequence<int>(0, 1).take(n_series);
    print_line(s, "ix", indices);
    // print the separator line
    auto len = set.get_line_width(n_series);
    s << std::string(len, '-') << std::endl;
  }

  /// Prints the values at a given timestamp.
  template<class Stream, class Timestamp, class ValuesMap>
  void print_values(Stream& s, Timestamp ts, ValuesMap vals, int n_series)
  {
    print_one(s, ts, set.index_width);
    print_one(s, set.index_value_sep);
    for (int i=0; i < n_series; ++i){
       if (vals.count(i) > 0) {
        print_one(s, vals[i], set.values_width);
      } else {
        print_one(s, "", set.values_width);
      }     
    }
    s << std::endl;
  }

  /// Prints a line accorfing to the settings.
  template<class Stream, class Ts, class Vals>
  void print_line(Stream& s, Ts ts, Vals vals)
  {
    print_one(s, ts, set.index_width, std::ios::right);
    print_one(s, set.index_value_sep);
    for (auto& v: vals){
      print_one(s, v, set.values_width, std::ios::right);
    }
    s << std::endl;
  }

  /// Prints one element on the stream.
  template<class Stream, class Value>
  void print_one(Stream& s,
                 Value value,
                 int width=0,
                 fmtflags flags=fmtflags()
  ){
    if (flags) s.setf(flags);
    if (width) s << std::setw(width);
    s << value;
  }

};

} // namespace impl

} // namespace printing

} // namespace ts

#endif /* PRINTING_HPP */
