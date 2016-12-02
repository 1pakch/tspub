#include <ts/series.hpp>
#include <ts/autoindex.hpp>
#include <ts/printing.hpp>

#include "print.hpp"


using namespace std;
using namespace ts;
using namespace ts::printing;


int main()
{
  print(AutoIndex<int>().zipValues(sequence(5, 1, 1)));
  print(AutoIndex<int>(1, 2).zipValues({1, 2, 3, 4}));
  
  auto daix = AutoIndex<double>(0, 0.1);
  print(daix.zipValues(sequence(5, 1, 1)));
  print(daix.zipValues({1, 2, 3, 4}));
  
  return 0;
}
