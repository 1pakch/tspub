# ts library
## Overview
The library is header-only. The directory structure is the following

 * `include/ts` - the library itself
 * `demo` - demo programs
 * `test` - test programs (dirty code, used for regressions)

## Library Structure

Core library:

 * `exceptions.hpp` - the exceptions used in the library
 * `series.hpp` - the time series class and the paired iterator over
        index/values.
 * `apply.hpp` - application of functors to series which is how all the
        interesting operations (moments, rolling calculations) are done.
 * `na.hpp` - functionality to check avoid/process missing values in
        arbitrary types including a `na_guard` wrapper allowing to skip
        pushing NAs in a given functor.
 * `moments.hpp` - public functions for moments calculations dispatching
        real work to `apply.hpp` and `filters/online_moments.hpp`.
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
 * `printing.hpp` - printing operator (`<<`) for series.

## Building

Build requires a C++14 compiler (was tested with gcc 5.4.1) and CMake.
To build run
```
  mkdir -p build
  cd build
  cmake .. && make
```
The tests/demos will be located in `build/test` and `build/demos`.

## Notes

### Introduction

I attempted to create a more generic library than requested having the
following goals in mind

 1. Not restricting possible types for indices/values unless necessary.
 2. Allowing for calculation algorithms on arbitrary types with or without
    NA-guarding depending on the types involved and the usage semantics.

The second point is better illustrated by the following desirable behaviors:

 * Enabling/disabling NA-guarding depending on input types:
     * Computing the sum of values of an int-valued series
       (no NA guarding needed)
     * Computing the sum of values of a double-valued series
       (NA guarding needed)
 * Enabling/disabling NA-guarding and providing information on the validity of
   the output depending on usage semantics:
     * Aggregating values over non-empty buckets
       (NA-guarding needed, output can be missing)
     * Aggregating index over non-empty buckets
       (no NA-guarding needed, output is always present)
 * Output validity can depend on user preferences:
     * Rolling mean/median considered valid or not depending on when the number 
       of already observed non-NA inputs is smaller than the window length.
 * Validity of calculation depending on input length:
     * The mean (var) of int-valued series - valid after 1 (2) obs.
     * The mean (var) of double-valued series - valid after 1 (2) non-NA obs.

### Functional Approach

I think the desired flexibility in behavior can be achieved if the operations
on the time series are thought of as applications of function-like objects on
the series data where the function-like objects are easily composed (wrapped)
around each other. A typical composition, for instance, is adding a NA-guard
before a functor not expecting NA values.

A less trivial composition is putting a functor behind an accumulator proxy
which dispatches the calls to the functor and stores its output as a collection.
The least trivial one encountered in the library is the one required for
calculating the covariance of two series with different indices (`apply2`
in `ts\include\apply.hpp`). It requires two aggregator functions (for
aggregating values when one of the series is sampled more frequently)
and calls the desired estimator (e.g. covariance) on the aggregated values.

Since one may want to configure how the covariance is calculated (e.g. are
the means known or not) and how the values are aggregated (e.g. whether to
use the sum of the values of more frequent series or maybe the last value)
if would be the most desirable to allow the end user to specify the exact
calculation pipeline in a concise and clear manner. The alternative currently
used is essentially exposing user-friendly end-functions for each typical
calculation (e.g. mean, variance, covariance or correlation).

### Missing Values

Note that NA values are natural to dealing with time series even if they
are not present in the input data. For instance, rolling mean can be
considered invalid up until the point when the rolling window is completely
full. Another example is the online variance estimator with unknown mean
which needs at least two observations to provide a meaningful output.

Provided that calculation outputs are sometimes uncertain (invalid) it is
also quite natural to consider them as having an extended output type. E.g.
a functor `F` returning an `int` might be considered as returning an `int`
and a validity flag (which may be captured by the `boost::optional<T>`
type standardized as `std::optional<T>` in C++17).

The benefit of such approach is that if functor's output is optional it
cannot be fed in an another functor expecting a *clean* type which can
be tracked by the compiler. Note that one has a choice of using `nan`
to represent the validity of `double` or other types supporting NA values
internally (which is not done by `boost::optional`). Regardless of the
implementation distinguishing *clean* doubles and *optional* in terms
of types  is still valuable. 

### Implementation and Findings

The desired calculations are done in a functional manner. For instance,
the rolling mean and the covariance are calculated as following.

Rolling mean:
```
// from demo/rolling_demo.hpp
// x is a time series with int indexes
auto rm = RollingMean(3);
auto acc = Accumulator<RollingMean, int>(rm);
auto y = apply_pairs(x, acc).value();
```

Covariance
```
// from ts/include/moments.hpp
// x and y are time series
// x_mean and y_mean are their means (known a priori)
// Aggregator is a functor downsampling the values of a more frequent series
using Est = filters::OnlineCovKnownMeans;
auto est = apply2<Est, Aggregator>( x, y, Est(x_mean, y_mean) );
```

`Accumulator` object and `apply2` function automatically choose whether to
skip NAs depending on the input type or the user override. A desired solution
would be to disable NA checking at compile time when it is not used but this
is not always done in the current implementation.

Lacking is a well-thought interface for the functors which should rely on
the `optional`-like to show whether it can consume/produce NA values. The
interface should at the least include the following information
  
  * Input type
  * Output type
  * Statefulness (is it a pure function or there is memory)

Absent of this interface it is difficult to introduce a coherent way to
compose functors. One way would be introducing a *composition* template
which would take two functors `F` and `G` and implement the composition
`x -> G(F(x))`. In particular, it would check the compatibility and may
automatically choose the implementation of `G` depending on whether `F`
can produce nans.

## Conclusion

Implementing an extensible NA-enabled library for time series processing
is a challenging task of one aims to

  1. avoid the overhead of NA-checking when not nesseary,
  2. implement commonly used operations by relying on reusable components
  3. to have flexibility in configuring the operations.
  
I find that the most promising toolkit to approach this problem is
  
  1. relying on an optional type to signals the validity of the output
     (which can be done with no overhead for doubles)
  2. a well designed interface for functors doing the actual computational work
  3. making compositions of such functors easy.

