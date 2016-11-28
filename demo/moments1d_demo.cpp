#include <vector>

#include <ts/series.hpp>
#include <ts/moments.hpp>
#include <ts/autoindex.hpp>
#include <ts/printing.hpp>
#include <ts/na.hpp>

#include "print.hpp"
#include "catch.hpp"

using namespace std;
using namespace ts;
using namespace ts::moments;
using namespace ts::printing;


int main()
{
  double nan = ts::na::na<double>();

  {
    print( "-- Mean and var --" );
    auto x = AutoIndex<int>().zipValues({0, 1, 1});
    print( "x =", x);
    print( "mean(x)  =", mean(x)  );
    print( "var(x)   =", var(x)   , "(estimated mean, n=2)" );
    print( "var(x,0) =", var(x,0) , "(fixed mean = 0, n=3)" );
    print();
  }

  {
    print( "-- Mean and var with nans --" );
    auto x = AutoIndex<int>().zipValues({0.0, 1.0, nan, 1.0, nan});
    print( "x =", x);
    print( "mean(x)  =", mean(x)  );
    print( "var(x)   =", var(x)   , "(estimated mean, n=2)" );
    print( "var(x,0) =", var(x,0) , "(fixed mean = 0, n=3)" );
    print();
  }

  {
    print( "-- Mean - not skipping NAs --" );
    auto x = AutoIndex<int>().zipValues({0.0, nan, 1., 1., nan});
    auto y = AutoIndex<int>().zipValues({0.0, 2.0, 10.0});
    print( "x =", x);
    print( "mean<false>(x) = ", mean<false>(x) );
    print( "y =", y);
    print( "mean<false>(y) = ", mean<false>(y) );
    print();
  }

  {
    print( "-- Mean - too few elements --" );
    auto x = AutoIndex<int>().zipValues({nan});
    print( "x =", x);
    catch_<SizeError>(
      [x](){ mean( x ); }
    );
    print();
  }
}
