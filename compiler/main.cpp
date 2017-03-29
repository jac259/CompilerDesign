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
      throw std::runtime_error("Invalid output type argument.");

  Context* cxt = new Context(outputType);

  while (std::getline(std::cin, str)) {
    try {
      // skip empty & fully commented lines
      if(str.empty() || str.front() == '#')
	continue;

      // trim off comments
      size_t comment = str.find_first_of('#');
      if(comment != std::string::npos)
	str.erase(comment); 

      
      Lexer * lexer = new Lexer(str, cxt);
      std::vector<Token*> * tokens = new std::vector<Token*>();
      
      // lex tokens
      while(!lexer->Eof())
	tokens->push_back(lexer->Next());

      // parse tokens
      Parser * parser = new Parser(*tokens, cxt);
      parser->Print();
    }
    catch (std::runtime_error ex) {
      std::cout << "Input: " << str << "\n"
		<< "Error: " << ex.what() << "\n\n";
    }
  }

  return 0;
}
