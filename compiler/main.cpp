#include "expr.hpp"
#include "lexer.hpp"

#include <stdio.h>
#include <sstream>

// See the GitHub README if instructions to use this program are required.
// https://github.com/jac259/CompilerDesign/blob/master/README.md

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

  return 1;
}
