// testutils.hpp - functions used in the unit tests

#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP 

#include <cmath>
#include <iostream>
#include <sstream>

#include <ts/series.hpp>
#include <ts/sequence.hpp>

namespace testutils {


// A class containing different assert functions 
class Assert
{
 public:

  // Checks the predicate and prints OK/fail message
  // Returns the predicate value
  // Usage:
  //   assert(x.value == 1, "x.value != 1", "current_func()")
  static bool is_true(
      bool condition,
      const std::string& msgfail, 
      const char* context
  ){
    if (condition) {
      log_assert_ok(context);
    } else {
      log_assert_fail(msgfail, context);
    }
    return condition;
  }

  template<typename T>
  static bool equal(
      T actual,
      T expected,
      const std::string& msgfail, 
      const char* context
  ){
    bool condition = actual == expected;
    if (condition) {
      log_assert_ok(context);
    } else {
      std::ostringstream msg;
      msg << msgfail
          << " actual (" << actual << ") !="
          << " expected (" << expected << ")";
      log_assert_fail(msg.str(), context);
    }
    return condition;
  }

  template<typename T>
  static bool vector_equal(
      const std::vector<T>& actual,
      const std::vector<T>& expected,
      const std::string& msgfail, 
      const char* context
  ){
    bool condition = actual == expected;
    if (condition) {
      log_assert_ok(context);
    } else {
      log_assert_fail(msgfail, context);
    }
    return condition;
  }

  static bool almost_equal(
      double actual,
      double expected,
      const std::string& msgfail, 
      const char* context,
      double tol=1e-8
  ){
    bool condition = std::abs(actual - expected) < tol;
    if (condition) {
      log_assert_ok(context);
    } else {
      std::ostringstream msg;
      msg << msgfail
          << " actual (" << actual << ") !="
          << " expected (" << expected << ")"
          << " (tol " << tol << ")";
      log_assert_fail(msg.str(), context);
    }
    return condition;
  }
 
 private:
  // Prints a line on std::cerr
  static void log(const std::string& msg)
  {
    std::cerr << msg << std::endl;
  }

  // Log a succeded assertion
  static void log_assert_ok(const char* context)
  {
    log(context + std::string(": OK"));
  }

  // Log a failed assertion
  static void log_assert_fail(
      const std::string& msgfail,
      const char* context_
  ){
    std::string context(context_);
    if (!context.empty()) context += ": ";
    log(context + msgfail);
  }

};

} // namespace testutils

#endif /* TESTUTILS_HPP */
