#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>

struct Type {
  virtual ~Type() = default;
  virtual std::string Print() const = 0;
};
struct Bool_Type : Type {
  std::string Print() const { return "bool"; }
};
struct Int_Type : Type {
  std::string Print() const { return "int";  }
};

#endif
