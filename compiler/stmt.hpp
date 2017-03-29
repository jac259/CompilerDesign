#ifndef STMT_HPP
#define STMT_HPP

struct Expr;
struct Decl;

struct Stmt {
  virtual ~Stmt() = default;
};

// Expression statement
struct Expr_Stmt : Stmt {
  Expr_Stmt(Expr* _e) : e(_e) {}
  Expr* e;
};

// Declaration statement
struct Decl_Stmt : Stmt {
  Decl_Stmt(Decl* _d) : d(_d) {}
  Decl* d;
};

#endif
