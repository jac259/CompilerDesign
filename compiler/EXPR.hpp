#include "TYPE.hpp"

// Global
const Bool_Type Bool_;
const Int_Type Int_;

struct Expr {
  virtual ~Expr() = default;
  virtual int Weight() = 0; // Weight of expression + Weight of branch expressions
  virtual int Eval() = 0; // Meaning of the expression; for Bool types return 0,1 for false,true
  virtual Type* Check() = 0; // Checks type of member expressions & returns return type
};

struct Bool_Expr : Expr {
  // true, false
private:
  bool value;

public:
  Bool_Expr(bool _value) : value(_value) {}; // initialize value - true/false

  int Weight() { return 1; }
  int Eval() { return value; }
  Type* Check() { return &Bool_; } // type is Bool
};

struct Int_Expr : Expr {
  // [-2^32, 2^32) 
private:
  int value;

public:
  Int_Expr(int _value) : value(_value) {}; // initialize value - integer

  int Weight() { return 1; }
  int Eval() { return value; }
  Type* Check() { return &Int_; } // type is Int
};

struct And_Expr : Expr {
  // e1 AND e2
private:
  Expr* e1, e2;

public:
  And_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {}; // initialize args
  
  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() & e2->Eval(); }
  Type* Check() {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      return &Bool_;
    else
      throw Type_Error();
  } // args must both be of type Bool; returns Bool
};

struct Or_Expr : Expr {
  // e1 OR e2
private:
  Expr* e1, e2;

public:
  Or_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() | e2->Eval(); }
  Type* Check() {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      return &Bool_;
    else
      throw Type_Error();
  } // args must both be of type Bool; returns Bool
};

struct Xor_Expr : Expr {
  // e1 XOR e2
private:
  Expr* e1, e2;

public:
  Xor_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() != e2->Eval(); }
  Type* Check() {
    if ((e1->Check() == &Bool_) && (e2->Check() == &Bool_))
      return &Bool_;
    else
      throw Type_Error();
  } // args must both be of type Bool; returns Bool
};

struct Not_Expr : Expr {
  // !e
private:
  Expr* e;

public:
  Not_Expr(Expr* _e) : e(_e) {}; // initialize arg

  int Weight() { return 1 + e->Weight(); }
  int Eval() { return !(e->Eval()); }
  Type* Check() {
    if (e->Check() == &Bool_)
      return &Bool_;
    else
      throw Type_Error();
  } // arg must be of type Bool; returns Bool
};

struct Cond_Expr : Expr {
  // if e1 then e2 else e3
private:
  Expr* e1, e2, e3;

public:
  Cond_Expr(Expr* _e1, Expr* _e2, Expr* _e3) : e1(_e1), e2(_e2), e3(_e3) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight() + e3->Weight(); }
  int Eval() { return e1->Eval() ? e2->Eval() : e3->Eval(); }
  Type* Check() {
    if ((e1->Check() == &Bool_) && (e2->Check() == e3->Check()))
      return e2->Check();
    else
      throw Type_Error();
  } // e1 must be of type Bool, e2 & e3 must be of the same type; returns type of e2 & e3
};

struct Equal_Expr : Expr {
  // e1 == e2
private:
  Expr& e1, e2;

public:
  Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() == e2->Eval(); }
  Type* Check() {
    if (e1->Check() == e2->Check())
      return &Bool_;
    else
      throw Type_Error();
  } // args must be of the same type; returns Bool
};

struct Not_Equal_Expr : Expr {
  // e1 != e2
private:
  Expr& e1, e2;

public:
  Not_Equal_Expr(Expr* _e1, Expr* _e2) : e1(_e1), e2(_e2) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  int Eval() { return e1->Eval() != e2->Eval(); }
  Type* Check() {
    if (e1->Check() == e2->Check())
      return &Bool_;
    else
      throw Type_Error();
  } // args must be of the same type; returns Bool
};

struct Less_Than_Expr : Expr {
  // e1 < e2
private:
  Expr& e1, e2;

public:
  Less_Than_Expr(Expr* e1, Expr* e2) : e1(_e1), e2(_e2) {}; // initialize args

  int Weight() { return 1 + e1->Weight() + e2->Weight(); }
  };
