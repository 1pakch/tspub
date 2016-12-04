#include <ts/ts.hpp>
#include <ts/printing.hpp>

//#include "print.hpp"


using namespace std;
using namespace ts;
using namespace ts::printing;


int main()
{
  auto x = AutoIndex<int>(-1).zipValues(sequence(5, 1, 1));
  auto y = AutoIndex<int>(3).zipValues(sequence(3, 1, 1));
  auto z = AutoIndex<int>(2).zipValues(sequence(6, 6, 1));
  
  auto c = std::vector<const Series<int, int>*>({&x, &y, &z});
  //auto p = ts::printing::impl::SeriesPrinter<decltype(x)>(c);

  //p.print(std::cout);
  //p.print(std::cout);

  print(std::cout, c);
  std::cout << std::endl;
  
  print(std::cout, x);
  std::cout << std::endl;
  
  print(std::cout, {&x, &y, &z});
  std::cout << std::endl;


}
