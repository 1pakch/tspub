#include <ts/ts.hpp>
#include <ts/printing.hpp>

#include "print.hpp"
#include "catch.hpp"


using namespace std;
using namespace ts;
using namespace ts::printing;


int main()
{  
  double nan = numeric_limits<double>::quiet_NaN();
  
  {
    auto x = Series<int, double>({0, 1, 2, 4}, {0, 0.5, 0.5, 1});  
    auto y = Series<int, double>({0,    2, 4}, {0,        1, 1});  

    print("-- Covariance with aggregation (sum) --"); 
    print("x =", x );
    print("y =", y );
    // known means
    print("cov(x,y)     =", cov(x, y),       "(means estimated, n=2)" );
    print("cov(x,y,0,0) =", cov(x, y, 0, 0), "(means 0, n=3 )" );
    print();
  } 

  { 
    auto x = Series<int, double>({1, 2, 3}, {1, nan, 2});  
    auto y = Series<int, double>({3}, {1});  

    print("-- Covariance using different aggregations --"); 
    print("x =", x );
    print("y =", y );
    // known means
    print("cov<Sum>  (x,y,0,0) =", cov<Sum>(x, y, 0, 0) );
    print("cov<Last> (x,y,0,0) =", cov<Last>(x, y, 0, 0) );
    print("cov<First>(x,y,0,0) =", cov<First>(x, y, 0, 0) );
    print();
  }

  {
    print("-- Correlation using different aggregations --"); 
    auto x = Series<int, double>({0, 1, 2}, {-1, 0.1, 0.9});  
    auto y = Series<int, double>({0,    2}, { 1,       -1});  

    print("x =", x );
    print("y =", y );
    print("corr<Sum>  (x,y) =", corr<Sum>(x, y) );
    print("corr<Last> (x,y) =", corr<Last>(x, y) );
    print("corr<First>(x,y) =", corr<First>(x, y) );
    print("corr<Sum>  (x,y,0,0) =",  corr<Sum>(x, y, 0, 0) );
    print("corr<Last> (x,y,0,0) =",  corr<Last>(x, y, 0, 0) );
    print("corr<First>(x,y,0,0) =",  corr<First>(x, y, 0, 0) );
    print();
  }

  return 0;
}
