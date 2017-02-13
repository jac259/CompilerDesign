#ifndef EXPR_HPP
#define EXPR_HPP

#include "type.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <stdio.h>
#include <limits>

// Global - defining constant type objects to use for comparison and checks
const Bool_Type Bool_;
const Int_Type Int_;

struct Expr {
  const Type* ExprType; // Type ptr used in derived expressions; will point to a global type object

  const std::string& GetTypeError() {
    static std::string TypeError("Invalid expression type.");
    return TypeError; // Defines static string message for a type error
  }

  const std::string& GetOverflowIntError() {
    static std::string OverflowIntError("Integer overflow.");
    return OverflowIntError; // Defines static string message for an integer overflow
  }
  
  virtual ~Expr() = default; // virtual destructor
  virtual int Weight() = 0; // Weight of expression + Weight of branch expressions
  virtual int Eval() = 0; // Meaning of the expression; for Bool types return 0,1 for false,true
  const Type* Check() { return ExprType; } // Returns expression type
};

struct Bool_Expr : Expr {
  // true, false
private:
  bool value;

public:
  Bool_Expr(bool _value) : value(_value) { ExprType = &Bool_; }; // initialize value & type

  int Weight() { return 1; }
  int Eval() { return value; } // returns value as int
};

struct Int_Expr : Expr {
  // [-2^31, 2^31) 
private:
  int value;

public:
  Int_Expr(int _value) : value(_value) { ExprType = &Int_; }; // initialize value & type

  int Weight() { return 1; }
  int Eval() { return value; }
};

struct And_Expr : Expr {
  // e1 AND e2
private:
  Expr * e1, * e2;
  
public:
  And_Expr(Expr * _e1, Expr * _e2) : e1(_e1), e2(_e2) {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed
  
  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() & e2->Eval(); }
};

struct Or_Expr : Expr {
  // e1 OR e2
private:
  Expr * e1, * e2;

public:
  Or_Expr(Expr * _e1, Expr * _e2) : e1(_e1), e2(_e2) {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() | e2->Eval(); } 
};

struct Xor_Expr : Expr {
  // e1 XOR e2
private:
  Expr * e1, * e2;

public:
  Xor_Expr(Expr * _e1, Expr * _e2) : e1(_e1), e2(_e2) {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() != e2->Eval(); }
};

struct Not_Expr : Expr {
  // !e
private:
  Expr * e;

public:
  Not_Expr(Expr * _e) : e(_e) {
    if(e->Check() == &Bool_)
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize arg and confirm it is well-typed

  int Weight() { return 1 + e->Weight(); }
  int Eval() { return !(e->Eval()); }
};

struct And_Then_Expr : Expr {
  // if e1 then e2 else false (short-circuit AND)
private:
  Expr * e1, * e2;

public:
  And_Then_Expr(Expr * _e1, Expr *_e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() ? e2->Eval() : false; }
};

struct Or_Else_Expr : Expr {
  // if e1 then true else e2 (short-circuit OR)
private:
  Expr * e1, * e2;

public:
  Or_Else_Expr(Expr * _e1, Expr * _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() ? true : e2->Eval(); }
};

struct Cond_Expr : Expr {
  // if e1 then e2 else e3
private:
  Expr * e1, * e2, * e3;

public:
  Cond_Expr(Expr * _e1, Expr * _e2, Expr * _e3) : e1(_e1), e2(_e2), e3(_e3) {
    if((e1->Check() == &Bool_) && (e2->Check() == e3->Check()))
      ExprType = e2->Check(); // Expression type matching that of e2 & e3
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight() + e3->Weight(); }
  int Eval() { return e1->Eval() ? e2->Eval() : e3->Eval(); }
};

struct Equal_Expr : Expr {
  // e1 == e2
private:
  Expr * e1, * e2;

public:
  Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if(e1->Check() == e2->Check())
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() == e2->Eval(); }
};

struct Not_Equal_Expr : Expr {
  // e1 != e2
private:
  Expr * e1, * e2;

public:
  Not_Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if(e1->Check() == e2->Check())
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() != e2->Eval(); }
};

struct Less_Than_Expr : Expr {
  // e1 < e2
private:
  Expr * e1, * e2;

public:
  Less_Than_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() < e2->Eval(); }
};

struct Greater_Than_Expr : Expr {
  // e1 > e2
private:
  Expr * e1, * e2;

public:
  Greater_Than_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() > e2->Eval(); }
};

struct Less_Than_Equal_Expr : Expr {
  // e1 <= e2
private:
  Expr * e1, * e2;

public:
  Less_Than_Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() <= e2->Eval(); }
};

struct Greater_Than_Equal_Expr : Expr {
  // e1 >= e2
private:
  Expr * e1, * e2;

public:
  Greater_Than_Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Bool_; // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() >= e2->Eval(); }
};

struct Add_Expr : Expr {
  // e1 + e2
private:
  Expr * e1, * e2;

public:
  Add_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Int_; // Expression type of int
    else
      throw std::runtime_error(GetTypeError());    
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // e1 & e2 positive; sum could exceed maximum value of int
    if ((_e1 > 0 && _e2 > 0) && (std::numeric_limits<int>::max() - _e2 < _e1))
      throw std::runtime_error(GetOverflowIntError());

    // e1 & e2 negative; sum could exceed minimum value of int
    if ((_e1 < 0 && _e2 < 0) && (std::numeric_limits<int>::min() - _e2 > _e1))
      throw std::runtime_error(GetOverflowIntError());
    
    return _e1 + _e2;
  }
};

struct Sub_Expr : Expr {
  // e1 - e2
private:
  Expr * e1, * e2;

public:
  Sub_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Int_; // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();
    
    // e1 negative, e2 positive; diff could exceed minimum value of int
    if ((_e1 < 0 && _e2 > 0) && (std::numeric_limits<int>::min() + _e2 > _e1))
      throw std::runtime_error(GetOverflowIntError()); 

    // e1 positive, e2 negative; diff could exceed maximum value of int
    if ((_e1 > 0 && _e2 < 0) && (std::numeric_limits<int>::max() + _e2 < _e1))
      throw std::runtime_error(GetOverflowIntError());

    return _e1 - _e2;
  }
};

struct Mult_Expr : Expr {
  // e1 * e2
private:
  Expr * e1, * e2;

public:
  Mult_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Int_;
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // these three checks allow testing for overflow using division
    if (_e1 == 0 || _e2 == 0)
      return 0;
    if (_e1 == -1)
      return -_e2;
    if (_e2 == -1)
      return -_e1;

    // e1 & e2 are the same sign; product could exceed maximum value of int
    if (((_e1 > 0 && _e2 > 0) || (_e1 < 0 && _e2 < 0))
	&& (std::numeric_limits<int>::max() /  _e2 < _e1))
      throw std::runtime_error(GetOverflowIntError());

    // e1 & e2 are different signs; product could exceed minimum value of int
    if (((_e1 < 0 && _e2 > 0) && (std::numeric_limits<int>::min() / _e2 > _e1)) ||
	((_e1 > 0 && _e2 < 0) && (std::numeric_limits<int>::min() / _e1 > _e2)))
      throw std::runtime_error(GetOverflowIntError());
    
    return _e1 * _e2;
  }
};

struct Div_Expr : Expr {
  // e1 / e2
private:
  Expr * e1, * e2;

public:
  Div_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Int_; // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // dividing any number by zero or the min will be undefined
    // dividing the minimum value of int by -1 will overflow
    if (_e2 == 0 || _e2 == std::numeric_limits<int>::min() ||
	(_e1 == std::numeric_limits<int>::min() && _e2 == -1))
      throw std::runtime_error(GetOverflowIntError());
    
    return _e1 / _e2;
  }
};

struct Rem_Expr : Expr {
  // e1 % e2
private:
  Expr * e1, * e2;

public:
  Rem_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {
    if((e1->Check() == &Int_) && (e2->Check() == &Int_))
      ExprType = &Int_; // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // dividing any number by zero or the min will be undefined
    // dividing the minimum value of int by -1 will overflow
    if (_e2 == 0 || _e2 == std::numeric_limits<int>::min() ||
	(_e1 == std::numeric_limits<int>::min() && _e2 == -1))
      throw std::runtime_error(GetOverflowIntError());
    
    return _e1 % _e2;
  }
};

struct Neg_Expr : Expr {
  // -e
private:
  Expr * e;

public:
  Neg_Expr(Expr* _e) : e(_e) {
    if(e->Check() == &Int_)
      ExprType = &Int_; // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  }; // initialize arg and confirm it is well-typed

  int Weight() { return 1 + e->Weight(); }
  int Eval() {
    int _e = e->Eval();

    // attempting to invert the sign of the minimum value of int will overflow
    if (_e == std::numeric_limits<int>::min())
      throw std::runtime_error(GetOverflowIntError());
    
    return -_e;
  }
};

#endif
