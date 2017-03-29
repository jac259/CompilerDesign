#ifndef DECL_HPP
#define DECL_HPP

//#include "expr.hpp"
//#include "type.hpp"

//#include "context.hpp"

struct Expr;
struct Type;
struct Context;

struct Decl {
  Context* cxt;
  virtual ~Decl() = default;
  virtual const std::string getName() = 0;
};

struct Var_Decl : Decl {
  const Type* type; // type of expression stored
  Expr* init; // precomputed, evaluated expression
  Expr* fullInit; // non-reduced expression used for printing
  const std::string name;
  Var_Decl(Context* _cxt, const std::string n, const Type* t) : type(t), name(n) {
    cxt = _cxt;
  }
  const std::string getName() { return name; }
};

#endif
