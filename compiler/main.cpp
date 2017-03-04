#include "expr.hpp"
#include "lexer.hpp"

#include <stdio.h>
#include <sstream>

int main(int argc, char * argv[]) {

  char outputType = 'd';
  std::string str;
  std::stringstream output;

  if(argv[1])
    if((std::string)argv[1] == "-b")
      outputType = 'b';
    else if((std::string)argv[1] == "-h")
      outputType = 'h';
    else if((std::string)argv[1] != "-d")
      throw std::runtime_error("Invalid argument.");

  while (std::getline(std::cin, str)) {
    Lexer * lexer = new Lexer(str, outputType);

    if(str.empty())
      continue;
    
    std::cout << "Input: " << str << "\n";
    output << "Output: ";
    
    while(!lexer->Eof()) {
      output << lexer->Print(lexer->Next());
      if(!lexer->Eof())
	output << ", ";
    }

    std::cout << output.str() << "\n\n";
    delete lexer;

    output.str(std::string());
  }

  /*  
    Expr * T = new Bool_Expr(true);
  Expr * F = new Bool_Expr(false);
  Expr * _10 = new Int_Expr(10);
  Expr * _25 = new Int_Expr(25);

  Expr * bit_and = new Bit_And_Expr(T, F);
  Expr * bit_or = new Bit_Or_Expr(T, F);
  Expr * bit_xor = new Bit_Xor_Expr(T, F);
  Expr * bit_comp = new Bit_Comp_Expr(T);

  Expr * bit_and_int = new Bit_And_Expr(_10, _25);
  Expr * bit_or_int = new Bit_Or_Expr(_10, _25);
  Expr * bit_xor_int = new Bit_Xor_Expr(_10, _25);
  Expr * bit_comp_int = new Bit_Comp_Expr(_10);

  std::cout << "Bitwise AND (T & F): " << std::boolalpha << (bool)bit_and->Eval() << "(" << bit_and->Eval() << ")\n";
  std::cout << "Bitwise AND (10 & 25): " << bit_and_int->Eval() << "\n";
  std::cout << "Bitwise OR (T | F): " << std::boolalpha << (bool)bit_or->Eval() << "(" << bit_or->Eval() << ")\n";
  std::cout << "Bitwise OR (10 | 25): " << bit_or_int->Eval() << "\n";
  std::cout << "Bitwise XOR (T ^ F): " << std::boolalpha << (bool)bit_xor->Eval() << "(" << bit_xor->Eval() << ")\n";
  std::cout << "Bitwise XOR (10 ^ 25): " << bit_xor_int->Eval() << "\n";
  std::cout << "Bitwise COMP (~T): " << std::boolalpha << (bool)bit_comp->Eval() << "(" << bit_comp->Eval() << ")\n";
  std::cout << "Bitwise COMP (~10): " << bit_comp_int->Eval() << "\n";

  std::cout << "-------------------------------------\n";
  */
  return 1;
}
