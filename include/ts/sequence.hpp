#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP 

#include <vector>

namespace ts {

namespace impl {

/// A sequence defined by the starting value and the increment (impl).
template<typename Value, typename Step=Value>
class SequenceBase
{  
  private:
    Value start_;
    Step step_;
  
  public:
    /// A rule to create a sequence from the starting value and the increment.
    SequenceBase(Value start, Step step):
      start_(start), step_(step)
    {};
    
    /// Returns the first n elements as a vector.
    std::vector<Value> take(size_t n){
      std::vector<Value> res;
      Value cur = start_;
      for (size_t i=0; i < n; ++i) {
        res.push_back(cur);
        cur = cur + step_;
      }
      return res;
    }
};

} // namespace impl

/// A sequence defined by the starting value and the increment (generic).
template<typename Value, typename Step=Value>
class Sequence: public impl::SequenceBase<Value, Step>
{
 public:
  using impl::SequenceBase<Value, Step>::SequenceBase;
};


/// A sequence defined by the starting value and the increment (ints).
template<>
class Sequence<int, int>: public impl::SequenceBase<int, int>
{
 public:
  Sequence(int start=0, int dt=1):
    SequenceBase<int, int>(start, dt)
  {};
};


template<typename T, typename DT=T>
std::vector<T> sequence(int size, T start, DT step)
{
  return Sequence<T, DT>(start, step).take(size);
}

} // namespace ts




#endif /* SEQUENCE_HPP */
