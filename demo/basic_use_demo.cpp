#include <ts/ts.hpp>
#include <ts/printing.hpp>

#include "print.hpp"
#include "catch.hpp"


using namespace std;
using namespace ts;
using namespace ts::printing;


int main()
{
  print("-- Constructors --"); 

  print(
    Series<int, int>()
  );

  print(
    Series<int, int>(vector<int>({0, 1, 2}), vector<int>({0, 1, 2}))
  );

  print(
    Series<int, int>({0, 1, 2}, {0, 1, 2})
  );

  print("-- Failing constructors --"); 

  catch_<SizeError>(
    [](){ Series<int, int>({0, 1}, {0}); } 
  );

  catch_<IndexNotSorted>(
    [](){ Series<int, int>(vector<int>({2, 1}), {0, 0}); }
  );

  print("-- Element access --"); 
  
  auto x = Series<int, int>({0, 2}, {0, 3});  

  print(x);
  
  print(x[2]);

  print("-- Replacing elements --"); 
  
  x[2] = 1;

  print(x); 

  print("-- Appending elements --"); 
  
  x.append(4, 3);

  print(x); 


  print("-- Appending elements failure --"); 
  
  catch_<IndexNotSorted>(
    [](){ Series<int, int>({1}, {0}).append(-1, 0); } 
  );

  print(x); 

  print("-- Failing element access --"); 

  catch_<IndexError<int>>(
    [](){ Series<int, int>()[0]; } 
  );

  print("-- Copy constructor --"); 

  auto y = x;

  print(x);

  print(y);

  std::cout << bool(x == y) << std::endl;

  x.append(5, 10);
  
  std::cout << bool(x == y) << std::endl;
  
  print("-- Move constructor --"); 
  
  auto z = Series<int, int>(Series<int, int>({0, 1}, {0, 3}));

  print(z);

  return 0;
}
