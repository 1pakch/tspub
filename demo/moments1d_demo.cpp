#include <ts/ts.hpp>
#include <ts/printing.hpp>

#include "print.hpp"
#include "catch.hpp"


using namespace std;
using namespace ts;
using namespace ts::printing;


int main()
{
  double nan = ts::na::na<double>();

  {
    print( "-- Mean and var --" );
    auto x = AutoIndex<int>().zipValues({0, 1, 1});
    print( "x =", x);
    print( "mean(x)  =", x.mean()  );
    print( "var(x)   =", x.var()   , "(estimated mean, n=2)" );
    print( "var(x,0) =", x.var(0) ,  "(fixed mean = 0, n=3)" );
    print();
  }

  {
    print( "-- Mean and var with nans --" );
    auto x = AutoIndex<int>().zipValues({0.0, 1.0, nan, 1.0, nan});
    print( "x =", x);
    print( "mean(x)  =", x.mean()  );
    print( "var(x)   =", x.var()   , "(estimated mean, n=2)" );
    print( "var(x,0) =", x.var(0) ,  "(fixed mean = 0, n=3)" );
    print();
  }

  {
    print( "-- Mean - too few elements --" );
    auto x = AutoIndex<int>().zipValues({nan});
    print( "x =", x);
    print( "mean(x)  =", x.mean()  );
    print();
  }
}
