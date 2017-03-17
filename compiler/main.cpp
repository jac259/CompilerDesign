#include "parser.hpp"

#include <stdio.h>
#include <sstream>

// See the GitHub README if instructions to use this program are required.
// https://github.com/jac259/CompilerDesign/blob/master/README.md

int main(int argc, char * argv[]) {

  char outputType = 'd';
  std::string str;
  std::stringstream output;

  // Input parameters
  if(argv[1])
    if((std::string)argv[1] == "-b")
      outputType = 'b';
    else if((std::string)argv[1] == "-h")
      outputType = 'h';
    else if((std::string)argv[1] != "-d")
      throw std::runtime_error("Invalid argument.");

  outputInt = outputType;

  while (std::getline(std::cin, str)) {
    Lexer * lexer = new Lexer(str, outputType);
    Parser * parser;
    std::vector<Token*> * tokens = new std::vector<Token*>();

    if(str.empty() || lexer->Eof()) // empty or fully-commented line
      continue;
    
    while(!lexer->Eof())
      tokens->push_back(lexer->Next());

    parser = new Parser(*tokens);
    parser->Print();
    
    delete lexer;
    delete parser;
  }

  return 0;
}
