#ifndef PRINT_HPP
#define PRINT_HPP 

#include <iostream>


namespace ts {

void print() {
  std::cout << std::endl;
}

template <typename T, typename... Args>
void print(T t, Args... args)
{
  std::cout << t << " ";
  print(args...);
}

}

#endif /* PRINT_HPP */
