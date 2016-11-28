// series_tests.cpp - basic tests for the time series class
//
// Since the exercise asks not use an external libraries
// (including unit testing frameworks such as the one in Boost
// or the one Google's one) the tests are written as functions
// named `test_*()` and checks are done using the `assert()`
// function defined below.
//
// The resulting tests are somewhat messy and the approach should
// be redisigned. The drawbacks are:
// 1) In case there are multiple asserts the test writer needs to
//    keep track of the test result. As an alternative the asserts
//    should throw special exceptions which should be captured by
//    the test runner
// 2) Incoherent parameters in different assert functions. Absence
//    of reusable equality/almost-equality comparisons for both
//    scalars and vectors
// 3) Lack of automatic test detection

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <ts/series.hpp>
#include <ts/autoindex.hpp>
#include <ts/moments.hpp>
#include "testutils.hpp"

using namespace ts;
using namespace testutils;


// Sum of squares of first n integers
int sum_of_squares(int n)
{
  return n * (n+1) * (2*n + 1) / 6;
}


// Test the constructor without arguments
void test_parameterless_ctor()
{
  Series<int> s;
  Assert::equal<int>(s.size(), 0, "Expected size is zero", __func__);
}


// Test the vector-taking constructor
void test_vector_ctor_ok()
{
  std::vector<int> index = {1, 2, 3};
  Series<int, int> s(index, index);
  Assert::equal<int>(s.size(), 3, "Vector sizes don't match", __func__);
}


// Test the vector-taking constructor
void test_vector_ctor_nonincreasing()
{
  std::vector<int> index = {1, 3, 2};
  bool exception = false;
  try {
    Series<int, int> s(index, index);
  }
  catch (TsException) { 
    exception = true;
  }
  Assert::is_true(exception, "TsException was not raised", __func__);
}


// Test that the elements are inserted in the right order
void test_append_simple()
{
  Series<int, int> s;
  std::vector<int> index;
  const int n = 10;
  for (size_t i=0; i < n; ++i) {
    index.push_back(i);
    s.append(i, i);
  }
  bool match = ( 
      std::equal(index.begin(), index.end(), s.indexView().begin())
      && std::equal(index.begin(), index.end(), s.valuesView().begin())
  );
  Assert::is_true(match, "index or values are not set correctly", __func__);
}


// Test that the exception is raised when one tries to insert not
// in the right order
void test_append_nonincreasing()
{
  Series<int, int> s;
  s.append(1, 1);
  // catch the provoked exception
  bool exception = false;
  try {
    s.append(0, 0);
  }
  catch (TsException) {
    exception = true;
  }
  // check that the exception was raised
  Assert::is_true(
      exception && s.size() == 1,
      "exception was not raised or wrong size",
      __func__
  );
}


// Test the at() function when the index can be found
void test_at_ok()
{
  std::vector<int> index = {1, 2, 3};
  Series<int, int> s(index, index);
  Assert::equal<int>(s.at(2), 2, "Search result incorrect", __func__);
}


// Test the at() function when the index cannot be found
void test_at_fail()
{
  std::vector<int> index = {1, 2, 3};
  Series<int, int> s(index, index);
  bool exception = false;
  try {
    s.at(4);
  }
  catch (IndexError<int>) { 
    exception = true;
  }
  Assert::is_true(exception, "IndexError was not raised", __func__);
}


// Test the mean() method
void test_mean(int count)
{
  auto s = AutoIndex<int>(1).zipValues(Sequence<int>(1).take(count));
  auto expectedMean = double(count + 1) / 2;
  Assert::almost_equal(moments::mean(s), expectedMean,
                       "incorrect mean", __func__);
}


// Test the var(knownMean) 
void test_var_known_mean(int count)
{
  auto s = AutoIndex<int>(1).zipValues(Sequence<int>(1).take(count));
  double knownMean = 0;
  auto expectedVar = double(sum_of_squares(count)) / count;
  auto computedVar = moments::var(s, knownMean);
  Assert::almost_equal(computedVar, expectedVar, "wrong variance", __func__);
}


// Test the var() method on the series [1, 2, ..., 5]
void test_var_estimated_mean()
{
  int count = 5;
  auto s = AutoIndex<int>(1).zipValues(Sequence<int>(1).take(count));
  auto expectedVar = double(4 + 1 + 0 + 1 + 4) / count;
  auto computedVar = moments::var(s);
  Assert::almost_equal(computedVar, expectedVar, "wrong variance", __func__);
}


// Test the x.cov(y, mean_x, mean_y) method
void test_cov_known_means()
{
  auto s1 = AutoIndex<int>().zipValues({0.1, 0.5, 0.4, 0.2});
  auto s2 = AutoIndex<int>().zipValues({0.4, -0.8, 1.0, 0.0});
  double cov = moments::cov(s1, s2, 0, 0);
  double expcov = 0.01;
  Assert::almost_equal(cov, expcov , "wrong covariance", __func__);
}

// Test the cov() method
void test_cov_estimated_means()
{
  auto s1 = AutoIndex<int>().zipValues({0.1, 0.5, 0.4, 0.2});
  auto s2 = AutoIndex<int>().zipValues({0.4, -0.8, 1.0, 0.0});
  double cov = moments::cov(s1, s2);
  double expcov = -0.04666666666666;
  Assert::almost_equal(cov, expcov, "wrong covariance", __func__);
}


int main()
{
  test_parameterless_ctor();
  test_vector_ctor_ok();
  test_vector_ctor_nonincreasing();
  test_append_simple();
  test_append_nonincreasing();
  test_at_ok();
  test_at_fail();
  test_mean(10);
  test_mean(49);
  test_var_known_mean(13);
  test_var_estimated_mean();
  test_cov_known_means();
  test_cov_estimated_means();
}

