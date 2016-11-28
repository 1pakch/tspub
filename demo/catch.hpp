#ifndef CATCH_HPP
#define CATCH_HPP 

#include <iostream>
#include <typeinfo>

namespace ts {

template<typename Exception, typename Functor>
void catch_(Functor f)
{
  try {
    f();
  } catch (const Exception& e) {
    std::cout << "Caught an " << typeid(e).name()
              << ": " << e.what() << std::endl;
  }
}

}

#endif /* CATCH_HPP */
