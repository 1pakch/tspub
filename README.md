# ts library
A basic library for manipulating sorted time series implemented in modern C++.
The design intends to allow for generic types in indices and values as well as
to provide basic support for NA values. The storage is columnar meaning that
indices and values are stored in separate contiguous containers (the
alternative would be to store indices and the correspnding values as
tuples).

## Overview
The library is header-only. The directory structure is the following

 * `include/ts` - the library itself
 * `demo` - demo programs
 * `test` - test programs (dirty code, used for regressions. In principle
    should be written with a proper unit testing framework but not to use
    them was a requirement).

## Library Structure

 * `ts.hpp` - the public header including everything except for `na_guard.hpp`
        and `printing.hpp`.

Core library:

 * `exceptions.hpp` - the exceptions used in the library
 * `series.hpp` - the time series class, the paired iterator over index/values
        and the convenience methods for computing mean and variance.
 * `apply.hpp` - application of functors to series which is how all the
        interesting operations (moments, rolling calculations) are done.
 * `na.hpp` - functionality to check avoid/process missing values in
        arbitrary types including a `na_guard` wrapper allowing to skip
        pushing NAs in a given functor.
 * `covariance.hpp` - public functions for covariance/correlation calculations
        using covariance extimators `filters/online_moments.hpp`.
 * `aggregators.hpp` - functors to aggregate values (e.g. first, last, sum)
        which are used for resampling.
 * `accumulator.hpp` - accumulating the output of a functor in a series.

Filters:

 * `filters/circular_buffer.hpp` - simple circular buffer for rolling filters.
 * `filters/rolling_mean.hpp` - rolling mean.
 * `filters/rolling_mean.hpp` - rolling median.
 * `filters/online_moments.hpp` - one-pass algorithms for computing moments.
 * `filters/validity.hpp` - expressing whether a filter output is good already.

Utilities:

 * `autoindex.hpp` - creating series with automatically generated indices.
 * `sequence.hpp` - a base class for auto-indices.
 * `printing.hpp` - printing operator (`<<`) for single series and columnar
        merging print for multiple series.

## Building

Build requires a C++14 compiler (was tested with gcc 5.4.1) and CMake.
To build run
```
  mkdir -p build
  cd build
  cmake .. && make
```
The tests/demos will be located in `build/test` and `build/demos`.

