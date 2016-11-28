// printing.hpp - ostream output operator for the custom classes

#ifndef PRINTING_HPP
#define PRINTING_HPP

namespace ts {

namespace printing {

/// A printing operator for all types T providing t.to_string(). All the
/// other types use their default operators due to SFINAE in the decltype.
/// Enabled by `using namespace ts::printing`.
template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.to_string(), os) 
{
    os << t.to_string();
    return os; 
}

} // namespace printing

} // namespace ts

#endif /* PRINTING_HPP */
