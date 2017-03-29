#ifndef EXPR_HPP
#define EXPR_HPP

#include "type.hpp"
#include "context.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <stdio.h>
#include <limits>
#include <algorithm>

// Global - defining constant type objects to use for comparison and checks
// const Bool_Type Bool_;
// const Int_Type Int_;
// char outputInt;

struct Context;

struct Expr {
  const Type* ExprType; // Type ptr used in derived expressions; will point to a global type object
  Context* cxt;
  
  const std::string& GetTypeError() {
    static std::string TypeError("Invalid expression type.");
    return TypeError; // Defines static string message for a type error
  }

  const std::string& GetOverflowIntError() {
    static std::string OverflowIntError("Integer overflow.");
    return OverflowIntError; // Defines static string message for an integer overflow
  }

  const std::string& GetUndefBehavError() {
    static std::string UndefBehavError("Undefined behavior.");
    return UndefBehavError; // Defines static string message for undefined behavior
  }
  
  virtual ~Expr() = default; // virtual destructor
  virtual int Weight() = 0; // Weight of expression + Weight of branch expressions
  virtual int Eval() = 0; // Meaning of the expression; for Bool types return 0,1 for false,true
  virtual std::string Print() = 0;
  const Type* Check() { return ExprType; } // Returns expression type
  std::string Evaluate() {
    if(Check() == &(cxt->Bool_))
      return Eval() ? "true" : "false";
    else if(Check() == &(cxt->Int_))
      return FormatInt(Eval());
    else
      throw std::runtime_error(GetUndefBehavError());
  }
  Expr* Precompute();
  std::string FormatInt(int value);
};

struct Bool_Expr : Expr {
  // true, false
private:
  bool value;

public:
  Bool_Expr(bool _value, Context* _cxt) : value(_value) {
    cxt = _cxt;
    ExprType = &(cxt->Bool_);
  } // initialize value & type

  int Weight() { return 1; }
  int Eval() { return value; } // returns value as int
  std::string Print() { return value ? "true" : "false"; }
};

struct Int_Expr : Expr {
  // [-2^31, 2^31) 
private:
  int value;

public:
  Int_Expr(int _value, Context* _cxt) : value(_value) {
    cxt = _cxt;
    ExprType = &(cxt->Int_);
  } // initialize value & type

  int Weight() { return 1; }
  int Eval() { return value; }
  std::string Print() {
    return FormatInt(value);
  }
};

struct And_Expr : Expr {
  // e1 AND e2
private:
  Expr * e1, * e2;
  
public:
  And_Expr(Expr * _e1, Expr * _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if ((e1->Check() == &(cxt->Bool_)) && (e2->Check() == &(cxt->Bool_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed
  
  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() ?  e2->Eval() : false; }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " && " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Or_Expr : Expr {
  // e1 OR e2
private:
  Expr * e1, * e2;

public:
  Or_Expr(Expr * _e1, Expr * _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if ((e1->Check() == &(cxt->Bool_)) && (e2->Check() == &(cxt->Bool_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() ? true : e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " || " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};


struct Not_Expr : Expr {
  // !e
private:
  Expr * e;

public:
  Not_Expr(Expr * _e, Context* _cxt) : e(_e) {
    cxt = _cxt;
    if(e->Check() == &(cxt->Bool_))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize arg and confirm it is well-typed

  int Weight() { return 1 + e->Weight(); }
  int Eval() { return !(e->Eval()); }
  std::string Print() { return "!" + (e->Weight() == 1 ? e->Print() : ("(" + e->Print() + ")")); }
};

struct Bit_And_Expr : Expr {
  // e1 & e2 (bitwise AND)
private:
  Expr * e1, * e2;

public:
  Bit_And_Expr(Expr * _e1, Expr *_e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if(e1->Check() == e2->Check())
      ExprType = e1->Check(); // Expression type matching that of e1 & e2
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() & e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " & " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Bit_Or_Expr : Expr {
  // e1 | e2 (bitwise OR)
private:
  Expr * e1, * e2;

public:
  Bit_Or_Expr(Expr * _e1, Expr * _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if(e1->Check() == e2->Check())
      ExprType = e1->Check(); // Expression type matching that of e1 & e2
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() | e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " | " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Bit_Xor_Expr : Expr {
  // e1 ^ e2 (bitwise XOR)
private:
  Expr * e1, * e2;

public:
  Bit_Xor_Expr(Expr * _e1, Expr * _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if(e1->Check() == e2->Check())
      ExprType = e1->Check(); // Expression type matching that of e1 & e2
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed
  
  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() ^ e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " ^ " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Bit_Comp_Expr : Expr {
  // ~e (bitwise NOT)
private:
  Expr * e;

public:
  Bit_Comp_Expr(Expr * _e, Context* _cxt) : e(_e) {
    cxt = _cxt;
    ExprType = e->Check(); // Expression type matching that of e
  }

  int Weight() { return 1 + e->Weight(); }
  int Eval() { return ExprType == &(cxt->Bool_) ? (e->Eval() ? 0 : 1) : ~(e->Eval()); }
  std::string Print() { return "~" + (e->Weight() == 1 ? e->Print() : ("(" + e->Print() + ")")); }
};

struct Cond_Expr : Expr {
  // if e1 then e2 else e3
private:
  Expr * e1, * e2, * e3;

public:
  Cond_Expr(Expr * _e1, Expr * _e2, Expr * _e3, Context* _cxt) : e1(_e1), e2(_e2), e3(_e3) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Bool_)) && (e2->Check() == e3->Check()))
      ExprType = e2->Check(); // Expression type matching that of e2 & e3
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight() + e3->Weight(); }
  int Eval() { return e1->Eval() ? e2->Eval() : e3->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " ? " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"))
      + " : " +
      (e3->Weight() == 1 ? e3->Print() : ("(" + e3->Print() + ")"));
  }
};

struct Equal_Equal_Expr : Expr {
  // e1 == e2
private:
  Expr * e1, * e2;

public:
  Equal_Equal_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if(e1->Check() == e2->Check())
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() == e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " == " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Not_Equal_Expr : Expr {
  // e1 != e2
private:
  Expr * e1, * e2;

public:
  Not_Equal_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if(e1->Check() == e2->Check())
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() != e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " != " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Less_Than_Expr : Expr {
  // e1 < e2
private:
  Expr * e1, * e2;

public:
  Less_Than_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() < e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " < " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Greater_Than_Expr : Expr {
  // e1 > e2
private:
  Expr * e1, * e2;

public:
  Greater_Than_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() > e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " > " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Less_Than_Equal_Expr : Expr {
  // e1 <= e2
private:
  Expr * e1, * e2;

public:
  Less_Than_Equal_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() <= e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " <= " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Greater_Than_Equal_Expr : Expr {
  // e1 >= e2
private:
  Expr * e1, * e2;

public:
  Greater_Than_Equal_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Bool_); // Expression type of bool
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() >= e2->Eval(); }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " >= " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Add_Expr : Expr {
  // e1 + e2
private:
  Expr * e1, * e2;

public:
  Add_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Int_); // Expression type of int
    else
      throw std::runtime_error(GetTypeError());    
  } // initialize args and confirm they are well-typed

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

  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " + " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Sub_Expr : Expr {
  // e1 - e2
private:
  Expr * e1, * e2;

public:
  Sub_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Int_); // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

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

  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " - " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Mult_Expr : Expr {
  // e1 * e2
private:
  Expr * e1, * e2;

public:
  Mult_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Int_);
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

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
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " * " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Div_Expr : Expr {
  // e1 / e2
private:
  Expr * e1, * e2;

public:
  Div_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Int_); // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // dividing any number by zero or the min will be undefined
    // dividing the minimum value of int by -1 will overflow
    if (_e2 == 0 || _e2 == std::numeric_limits<int>::min() ||
	(_e1 == std::numeric_limits<int>::min() && _e2 == -1))
      throw std::runtime_error(GetUndefBehavError());
    
    return _e1 / _e2;
  }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " / " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Rem_Expr : Expr {
  // e1 % e2
private:
  Expr * e1, * e2;

public:
  Rem_Expr(Expr* _e1, Expr* _e2, Context* _cxt) : e1(_e1), e2(_e2) {
    cxt = _cxt;
    if((e1->Check() == &(cxt->Int_)) && (e2->Check() == &(cxt->Int_)))
      ExprType = &(cxt->Int_); // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  } // initialize args and confirm they are well-typed

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() {
    int _e1 = e1->Eval();
    int _e2 = e2->Eval();

    // dividing any number by zero or the min will be undefined
    // dividing the minimum value of int by -1 will overflow
    if (_e2 == 0 || _e2 == std::numeric_limits<int>::min() ||
	(_e1 == std::numeric_limits<int>::min() && _e2 == -1))
      throw std::runtime_error(GetUndefBehavError());
    
    return _e1 % _e2;
  }
  std::string Print() {
    return (e1->Weight() == 1 ? e1->Print() : ("(" + e1->Print() + ")"))
      + " % " +
      (e2->Weight() == 1 ? e2->Print() : ("(" + e2->Print() + ")"));
  }
};

struct Neg_Expr : Expr {
  // -e
private:
  Expr * e;

public:
  Neg_Expr(Expr* _e, Context* _cxt) : e(_e) {
    cxt = _cxt;
    if(e->Check() == &(cxt->Int_))
      ExprType = &(cxt->Int_); // Expression type of int
    else
      throw std::runtime_error(GetTypeError());
  } // initialize arg and confirm it is well-typed

  int Weight() { return 1 + e->Weight(); }
  int Eval() {
    int _e = e->Eval();

    // attempting to invert the sign of the minimum value of int will overflow
    if (_e == std::numeric_limits<int>::min())
      throw std::runtime_error(GetOverflowIntError());
    
    return -_e;
  }
  std::string Print() { return "-" + (e->Weight() == 1 ? e->Print() : ("(" + e->Print() + ")")); }
};

std::string Expr::FormatInt(int value) {
  switch(cxt->outputFormat) {
  case 'd':
    return std::to_string(value);
  case 'h': {
    std::stringstream ss;
    ss << (value < 0 ? "-" : "") << "0x" << std::hex << (value < 0 ? -value : value);
    return ss.str();
  }
  case 'b': {
    std::stringstream ss;
    
    int n = value < 0 ? -value : value;
    while(n != 0) {
      ss << (n & 1);
      n >>= 1;
    }
    
    ss << "b0" << (value < 0 ? "-" : "");
    
    std::string str = ss.str();
    std::reverse(str.begin(), str.end());
    return str;
  }
  default:
    throw std::runtime_error("Invalid output type.");
  }
}

Expr* Expr::Precompute() {
  Expr* e;
  
  if(Check() == &(cxt->Bool_))
    e = new Bool_Expr(Eval(), cxt);
  else if(Check() == &(cxt->Int_))
    e = new Int_Expr(Eval(), cxt);
  else
    throw std::runtime_error(GetUndefBehavError());
  
  return e;
}


#endif
