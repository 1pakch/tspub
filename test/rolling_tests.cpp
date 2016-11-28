// rolling_tests.cpp - basic tests for the rolling filters
//
// See the notes for series_tests.cpp 

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <ts/series.hpp>
#include <ts/autoindex.hpp>
#include <ts/accumulator.hpp>
#include <ts/apply.hpp>
#include <ts/filters.hpp>
#include "testutils.hpp"

using namespace ts;
using namespace ts::filters;
using namespace testutils;


// Tests for the rolling mean and the rolling median on a sequence
// of natural numbers. This is dirty code which should be refactored
// with reusable assertion functions or a proper testing framework.
class RollingTest
{
 public:
  RollingTest(int width, int input_size):
    width(width)
  {
    s = AutoIndex<int>(1).zipValues(Sequence<double>(1, 1).take(input_size));
    // construct the expected output
    int index = width;
    double value = (1 + width) / 2.0;
    int count = input_size - width + 1;
    while (count > 0) {
      expectedOutput.append(index, value);
      index += 1;
      value += 1;
      --count;
    }
  }

  void test_mean()
  {
    auto rm = RollingMean(width);
    auto acc = Accumulator<RollingMean, int>(rm);
    auto ma = apply_pairs(s, acc).value();
    compare_index(ma, __func__);
    compare_values(ma, __func__);
  }

  void test_median()
  {
    auto rm = RollingMedian<double>(width);
    auto acc = Accumulator<RollingMedian<double>, int>(rm);
    auto ma = apply_pairs(s, acc).value();
    compare_index(ma, __func__);
    compare_values(ma, __func__);
  }
  
  private:
   Series<int, double> s;
   Series<int, double> expectedOutput;
   int width;
  
  bool compare_index(const Series<int, double>& ma, const char context[])
  {
    return Assert::vector_equal<int>(ma.indexView(),
                                     expectedOutput.indexView(),
                                     "indices differ",
                                     context);
  }

  bool compare_values(const Series<int, double>& ma, const char context[])
  {
    auto vals = ma.valuesView();
    auto expvals = expectedOutput.valuesView();
    bool eq = true;
    for (size_t i=0; i < vals.size(); ++i) {
      eq = eq && std::abs(vals[i] - expvals[i]) < 1e-7;
      if (!eq) {
        std::cerr << "At position " << i
                  << ". Expected = " << expvals[i]
                  << ". Actual = " << vals[i]
                  << std::endl;
        //break;
      }
    }
    return eq;
  }
};


int main()
{
  //test_rolling_mean_5();
  
  RollingTest rt1(3, 5);
  rt1.test_mean();
  rt1.test_median();

  RollingTest rt2(4, 10);
  rt1.test_mean();
  rt2.test_median();
  
  std::cout << std::endl;
  std::cout << "-- The demo of the median algorithm --" << std::endl;

  std::vector<int> input = {5, 9, 6, 100, 102, 7};
  RollingMedian<double> rm(4);
  for (auto val: input) {
    std::cout << "Input = " << val << std::endl;
    rm(val);
    rm.print_state();
    std::cout << "Median = " << rm.value() << std::endl;
    std::cout << std::endl;
  }

  return 0;
}

