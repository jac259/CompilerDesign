#include "EXPR.hpp"

int main() {

  std::cout << std::endl;
  
  Expr * trueExpr = new Bool_Expr(true);
  Expr * falseExpr = new Bool_Expr(false);

  Expr * andExpr = new And_Expr(trueExpr, falseExpr);
  std::cout << "AND: T & F = " << andExpr->Eval() << std::endl;

  Expr * orExpr = new Or_Expr(trueExpr, falseExpr);
  std::cout << "OR: T | F = " << orExpr->Eval() << std::endl;

  Expr * xorExpr = new Xor_Expr(andExpr, orExpr);
  std::cout << "XOR: (T & F) ^ (T | F) = " << xorExpr->Eval() << std::endl;

  Expr * notExpr = new Not_Expr(xorExpr);
  std::cout << "NOT: !((T & F) ^ (T | F)) = " << notExpr->Eval() << std::endl;

  Expr * ifTrueExpr = new Cond_Expr(trueExpr, andExpr, orExpr);
  Expr * ifFalseExpr = new Cond_Expr(falseExpr, andExpr, orExpr);
  std::cout << "COND: If (T) then (T & F) else (T | F) = " << ifTrueExpr->Eval() << std::endl;
  std::cout << "COND: If (F) then (T & F) else (T | F) = " << ifFalseExpr->Eval() << std::endl;

  Expr * eqExpr = new Equal_Expr(trueExpr, falseExpr);
  Expr * neqExpr = new Not_Equal_Expr(trueExpr, falseExpr);
  std::cout << "EQUAL: T == F = " << eqExpr->Eval() << std::endl;
  std::cout << "NOT EQUAL: T != F = " << neqExpr->Eval() << std::endl;

  std::cout << "-------------------------------" << std::endl;
  
  Expr * zero = new Int_Expr(0);
  Expr * one = new Int_Expr(1);
  Expr * two = new Int_Expr(2);
  Expr * three = new Int_Expr(3);
  Expr * four = new Int_Expr(4);
  Expr * five = new Int_Expr(5);
  Expr * six = new Int_Expr(6);
  Expr * seven = new Int_Expr(7);
  Expr * eight = new Int_Expr(8);
  Expr * nine = new Int_Expr(9);
  
  Expr * ltExpr = new Less_Than_Expr(zero, four);
  Expr * gtExpr = new Greater_Than_Expr(zero, four);
  Expr * lteExpr = new Less_Than_Equal_Expr(zero, four);
  Expr * gteExpr = new Greater_Than_Equal_Expr(four, four);

  std::cout << "LESS THAN: 0 < 4 = " << ltExpr->Eval() << std::endl;
  std::cout << "GREATER THAN: 0 > 4 = " << gtExpr->Eval() << std::endl;
  std::cout << "LESS THAN EQUAL: 0 <= 4 = " << lteExpr->Eval() << std::endl;
  std::cout << "GREATER THAN EQUAL: 4 >= 4 = " << gteExpr->Eval() << std::endl;

  std::cout << "-------------------------------" << std::endl;
  
  Expr * addExpr = new Add_Expr(two, new Add_Expr(six, seven));
  Expr * subExpr = new Sub_Expr(nine, new Sub_Expr(one, two));
  Expr * multExpr = new Mult_Expr(four, new Mult_Expr(eight, two));
  Expr * divExpr = new Div_Expr(nine, two);
  Expr * remExpr = new Rem_Expr(nine, two);
  Expr * negExpr = new Neg_Expr(new Add_Expr(nine, two));
  Expr * divRemExpr = new Add_Expr(new Mult_Expr(new Div_Expr(nine, five), five),
				   new Rem_Expr(nine, five));

  std::cout << "ADD: 2 + 6 + 7 = " << addExpr->Eval() << std::endl;
  std::cout << "SUB: 9 - (1 - 2) = " << subExpr->Eval() << std::endl;
  std::cout << "MULT: 4 * 8 * 2 = " << multExpr->Eval() << std::endl;
  std::cout << "DIV: 9 / 2 = " << divExpr->Eval() << std::endl;
  std::cout << "REM: 9 % 2 = " << remExpr->Eval() << std::endl;
  std::cout << "NEG: -(9 + 2) = " << negExpr->Eval() << std::endl;
  std::cout << "DIV/REM: ((9 / 5) * 5) + (9 % 5) = " << divRemExpr->Eval() << std::endl;

  std::cout << "-------------------------------" << std::endl;

  try {
    Expr * illFormExpr = new And_Expr(trueExpr, zero);
    bool illForm = illFormExpr->Eval();
    std::cout << "ILLFORM AND: true & 0 performed successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "ILLFORM AND: true & 0 is undefined: " << e.what() << std::endl;
  }
  
  try {
    Expr * illFormExpr = new Less_Than_Expr(falseExpr, seven);
    bool illForm = illFormExpr->Eval();
    std::cout << "ILLFORM LESS THAN: false < 7 performed successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "ILLFORM LESS THAN: false < 7 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * illFormExpr = new Add_Expr(two, trueExpr);
    int illForm = illFormExpr->Eval();
    std::cout << "ILLFORM ADD: 2 + true added successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "ILLFORM ADD: 2 + true is undefined: " << e.what() << std::endl;
  }
  
  std::cout << "-------------------------------" << std::endl;

  Expr * maxInt = new Int_Expr(std::numeric_limits<int>::max());
  Expr * minInt = new Int_Expr(std::numeric_limits<int>::min());
  
  try {
    Expr * badAddExpr = new Add_Expr(maxInt, two);
    int badAdd = badAddExpr->Eval();
    std::cout << "ADD OVFLOW: (2^31 - 1) + 2 added successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "ADD OVFLOW: (2^31 - 1) + 2 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * badSubExpr = new Sub_Expr(minInt, two);
    int badSub = badSubExpr->Eval();
    std::cout << "SUB OVFLOW: -2^31 - 2 subtracted successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "SUB OVFLOW: -2^31 - 2 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * badMultExpr = new Mult_Expr(minInt, two);
    int badMult = badMultExpr->Eval();
    std::cout << "MULT OVFLOW: -2^31 * 2 multiplied successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "MULT OVFLOW: -2^31 * 2 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * badDivExpr = new Div_Expr(minInt, zero);
    int badDiv = badDivExpr->Eval();
    std::cout << "DIV OVFLOW: -2^31 / 0 divided successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "DIV OVFLOW: -2^31 / 0 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * badRemExpr = new Rem_Expr(minInt, zero);
    int badRem = badRemExpr->Eval();
    std::cout << "REM OVFLOW: -2^31 % 0 remaindered successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "REM OVFLOW: -2^31 % 0 is undefined: " << e.what() << std::endl;
  }

  try {
    Expr * badNegExpr = new Neg_Expr(minInt);
    int badNeg = badNegExpr->Eval();
    std::cout << "NEG OVFLOW: -(-2^31) inverted successfully." << std::endl;
  } catch (const std::runtime_error& e) {
    std::cout << "NEG OVFLOW: -(-2^31) is undefined: " << e.what() << std::endl;
  }

  std::cout << std::endl;
}
