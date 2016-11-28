// exceptions.hpp - exceptions used by the library

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <sstream>

namespace ts {

/// General exception class for the ts library
class TsException: public std::runtime_error{
 public:
  TsException(const std::string& what)
    : std::runtime_error(what)
  {}
};

/// Raised when one tries to access a missing index
template<typename Timestamp>
class IndexError: public TsException{
 public:
  const Timestamp loc;
  IndexError(Timestamp loc)
    : TsException("Index " + locStr(loc) + " not found."), loc(loc)
  {}
 private:
  static std::string locStr(Timestamp loc) {
      std::ostringstream out;
      out << loc;
      return out.str();
  }
};

/// Raised when an operation leads to a non-sorted index.
class IndexNotSorted: public TsException{
 public:
  using TsException::TsException;
};

/// Raised when the provided argument(s) size(s) are not as expected.
class SizeError: public TsException{
 public:
  using TsException::TsException;
};

}

#endif /* EXCEPTIONS_HPP */
