// merge.hpp - merge iterator for series.

#ifndef MERGE_HPP
#define MERGE_HPP 

#include <vector>
#include <initializer_list>
#include <iostream>

#include <ts/series.hpp>


namespace ts {


/// Merges series on their timestamps.
template<typename Series>
class MergeIterator
{
  using this_type =  MergeIterator<Series>;
  using itr_type = typename Series::paired_iterator_type;

  std::vector<itr_type> itrs_; // current iterators for each series
  std::vector<itr_type> ends_; // end iteratators for each series
  int cur_; // index of the current series or -1

  // set cur to the index of the iterator with the minimum timestamp.
  void set_current()
  {
    cur_ = -1;
    for (size_t i=0; i < itrs_.size(); ++i) {
      if (itrs_[i] == ends_[i]) continue;
      if (-1 == cur_ || itrs_[i].index() < itrs_[cur_].index()) cur_ = i;
    }
  }

 public:

  MergeIterator(std::vector<itr_type>&& itrs,
                std::vector<itr_type>&& ends)
    : itrs_(std::move(itrs)),
      ends_(std::move(ends))
  {
    set_current();
  }

  /// Creates a merge iterator from a collection of pointers to Series.
  template<class PtrCollection>
  static auto from_pointers(const PtrCollection& col) -> decltype(auto)
  {
    std::vector<itr_type> begins;
    std::vector<itr_type> ends;
    for (auto& s: col) {
      begins.emplace_back(s->begin_paired());
      ends.emplace_back(s->end_paired());
    }
    return MergeIterator(std::move(begins), std::move(ends));
  }

  /// Index of the series where the current observation comes from.
  auto series() const -> decltype(auto) { return cur_; }
  
  /// Current timestamp.
  auto timestamp() const -> decltype(auto) { return itrs_[cur_].index(); }

  /// Current value 
  auto value() const -> decltype(auto) { return itrs_[cur_].value(); }
 
  /// Move to the next observation.
  void operator++ ()
  {
    itrs_[cur_] = ++itrs_[cur_];
    set_current();
  }

  /// Are there observations left?
  bool at_end() const { return cur_ == -1; }

  /// Number of input series.
  size_t n_series() const { return itrs_.size(); }
};





template<typename Series>
class SeriesCollection: public std::vector<const Series*>
{
 public:

  using this_type =  SeriesCollection<Series>;
  using itr_type = typename Series::paired_iterator_type;

  using std::vector<const Series*>::vector;

  auto merge_iterator() const -> decltype(auto)
  {
    return MergeIterator<Series>(get_begins(), get_ends());
  }

 private:

  auto get_begins() const -> decltype(auto)
  {
    std::vector<itr_type> res;
    for (auto& s: *this) { res.emplace_back(s->begin_paired()); }
    return res;
  }

  auto get_ends() const -> decltype(auto)
  {
    std::vector<itr_type> res;
    for (auto& s: *this) { res.emplace_back(s->end_paired()); }
    return res;
  }

};

} // namespace ts

#endif /* MERGE_HPP */
