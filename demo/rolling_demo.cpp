#include <iostream>

#include <ts/series.hpp>
#include <ts/na.hpp>
#include <ts/autoindex.hpp>
#include <ts/apply.hpp>
#include <ts/accumulator.hpp>
#include <ts/printing.hpp>
#include <ts/filters.hpp>

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
  print( "x =", x );

  {
    auto rm = RollingMean(3);
    // Accumulator is a functor that wraps a filter and
    // stores the output in a Series when called
    auto acc = Accumulator<RollingMean, int>(rm);
    // Calls the acc functor on each (index, value) pair
    auto y = apply_pairs(x, acc).value();

    print(" -- RollingMean(3) -- " );
    print( y );
  }
  
  {
    auto rm = RollingMedian<double>(3);
    auto acc = Accumulator<RollingMedian<double>, int>(rm);
    auto y = apply_pairs(x, acc).value();

    print(" -- RollingMedian<double>(3) --" );
    print("y =", y );
  }

  return 0;
}
