#include <ts/ts.hpp>
#include <ts/printing.hpp>

#include "print.hpp"


using namespace std;
using namespace ts;
using namespace ts::filters;
using namespace ts::printing;


int main()
{
  auto vals = Sequence<double>(0, 0).take(8);
  auto x = AutoIndex<int>().zipValues(vals);
  
  x[2] = 1;
  x[3] = na::na<double>();
  x[4] = 1;

  {
    auto rm = RollingMean(3);
    // Accumulator is a functor that wraps a filter and
    // stores the output in a Series when called
    auto acc1 = Accumulator<RollingMean, int>(rm);
    auto acc2 = Accumulator<RollingMean, int>(rm);
    // Calls the acc functor on each (index, value) pair
    auto y1 = x.apply_pairs(acc1).value();
    auto y2 = x.apply_pairs(acc2, false).value(); // do not skip NA inputs

    print(" -- RollingMean(3) -- " );
    print( std::cout, {&x, &y1, &y2} );
  }


  {
    auto rm = RollingMedian<double>(3);
    auto acc1 = Accumulator<RollingMedian<double>, int>(rm);
    auto acc2 = Accumulator<RollingMedian<double>, int>(rm);
    auto y1 = x.apply_pairs(acc1).value();
    auto y2 = x.apply_pairs(acc2, false).value();

    print(" -- RollingMedian<double>(3) --" );
    print( std::cout, {&x, &y1, &y2} );
  }

  return 0;
}
