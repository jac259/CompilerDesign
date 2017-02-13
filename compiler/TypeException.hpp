#ifndef TYPEEXCEPTION_HPP
#define TYPEEXCEPTION_HPP

#include <stdexcept>
#include <exception>
#include <string>

struct TypeException : std::exception {
public:
  const char* what() const throw() {
    return "Type exception.";
  }
}
#endif
