#include <ts/series.hpp>
#include <ts/autoindex.hpp>

#include "print.hpp"


using namespace std;
using namespace ts;


template<typename Series>
void print(const Series& s)
{
  std::cout << s.to_string() << std::endl;
}

int main()
{
  print(AutoIndex<int>().zipValues(sequence(5, 1, 1)));
  print(AutoIndex<int>(1, 2).zipValues({1, 2, 3, 4}));
  
  auto daix = AutoIndex<double>(0, 0.1);
  print(daix.zipValues(sequence(5, 1, 1)));
  print(daix.zipValues({1, 2, 3, 4}));
  
  return 0;
}
