#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <iomanip>
#include <algorithm>

struct Lexer {
private:
  std::string::iterator first, last; // iterator used to step through the string
  std::string buffer; // buffer while reading multi-digit numbers
  std::string str; // input
  char outputFormat; // b = binary, h = hex, d = decimal

  const std::string& GetInvalidCharError() {
    static std::string InvalidCharError("Invalid character.");
    return InvalidCharError;
  } // message thrown in invalid character
  
  char LookAhead() const { return *first; } // look at current character
  void Consume() { ++first; } // step to next character
  char Buffer(); // add lookahead to buffer
  bool isHex(char); // checks if valid hex digit (0-9, a-f)
  bool isBin(char); // checks if valid binary digit (0,1)
  std::string dec2bin(int); // converts int to binary string for output
  
public:
  bool Eof() const { return first == last; } // checks if the string is at its end
  Token * Next(); // returns the next token
  std::string Print(Token *); // return the given token for printing
  Lexer(std::string, char); // constructor, takes input text and output type for numbers
};

// Constructor, stores input, sets iterators and output format for numbers
Lexer::Lexer(std::string _str, char _outputFormat) : str(_str), outputFormat(_outputFormat) {
  size_t comment = str.find_first_of('#');
  if(comment != std::string::npos)
    str.erase(comment); // cut off comments
  
  first = str.begin();
  last = str.end();
}

// adds the character to the buffer and returns the added character
char Lexer::Buffer() {
  if(Eof())
    return 0;
  buffer += LookAhead();
  Consume();
  return buffer.back();
}

// prints the given token with formatting
std::string Lexer::Print(Token * token) {
  std::stringstream ss;
  ss << token->EnumName(); // all tokens print their name
  
  switch(token->kind) {
  case Int_Tok: { // ints print their value as decimal, hex, or binary
    Int_Token * int_token = dynamic_cast<Int_Token*>(token);
    ss << ": ";
    switch(outputFormat) {
    case 'd':
      ss << int_token->value;
      break;
    case 'h':
      ss << "0x" << std::hex << int_token->value;
      break;
    case 'b':
      ss << "0b" << dec2bin(int_token->value);
      break;
    }
    break;
  }
  case Bool_Tok: { // bools print their value
    Bool_Token * bool_token = dynamic_cast<Bool_Token*>(token);
    ss << ": " << std::boolalpha << bool_token->value;
    break;
  }
  }
  
  return ss.str();
}

// checks if the character is a valid hex digit
bool Lexer::isHex(char c) {
  return (isdigit(c) || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'
	  || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f');
}

// checks if the character is a valid binary digit
bool Lexer::isBin(char c) {
  return (c == '1' || c == '0');
}

// converts the number to a binary string
std::string Lexer::dec2bin(int n) {
  std::stringstream ss;

  while(n != 0) {
    ss << (n & 1);
    n >>= 1;
  }

  std::string str = ss.str();
  std::reverse(str.begin(), str.end());
  return str;
}

// reads along the string and returns the next token
Token * Lexer::Next() {
  buffer.clear();
  
  while(!Eof()) {
    switch(LookAhead()) {
    case ' ':
    case '\n':
    case '\t':
    case '\v':
      Consume(); // skip white space
      continue;
    case '(':
      Consume();
      return new Punc_Op_Token(LParen_Tok); // left paren
    case ')':
      Consume();
      return new Punc_Op_Token(RParen_Tok); // right paren
    case '=':
      Consume();
      if(LookAhead() == '=') {
	Consume();
	return new Punc_Op_Token(EqualEqual_Tok); // ==
      }
      return new Punc_Op_Token(Equal_Tok); // =
    case '!':
      Consume();
      if(LookAhead() == '=') {
	Consume();
	return new Punc_Op_Token(Not_Equal_Tok); // !=
      }
      return new Punc_Op_Token(Bang_Tok); // !
    case '<':
      Consume();
      if(LookAhead() == '=') {
	Consume();
	return new Punc_Op_Token(LTE_Tok); // <=
      }
      return new Punc_Op_Token(LT_Tok); // <
    case '>':
      Consume();
      if(LookAhead() == '=') {
	Consume();
	return new Punc_Op_Token(GTE_Tok); // >=
      }
      return new Punc_Op_Token(GT_Tok); // >
    case '+':
      Consume();
      return new Punc_Op_Token(Plus_Tok); // +
    case '-':
      Consume();
      return new Punc_Op_Token(Minus_Tok); // -
    case '*':
      Consume();
      return new Punc_Op_Token(Star_Tok); // *
    case '/':
      Consume();
      return new Punc_Op_Token(Slash_Tok); // /
    case '%':
      Consume();
      return new Punc_Op_Token(Percent_Tok); // %
    case '&':
      Consume();
      if(LookAhead() == '&') {
	Consume();
	return new Punc_Op_Token(AmpAmp_Tok); // &&
      }
      return new Punc_Op_Token(Amp_Tok); // &
    case '|':
      Consume();
      if(LookAhead() == '|') {
	Consume();
	return new Punc_Op_Token(PipePipe_Tok); // ||
      }
      return new Punc_Op_Token(Pipe_Tok); // |
    case '^':
      Consume();
      return new Punc_Op_Token(Caret_Tok); // ^
    case '~':
      Consume();
      return new Punc_Op_Token(Tilde_Tok); // ~
    case '?':
      Consume();
      return new Punc_Op_Token(Query_Tok); // ?
    case ':':
      Consume();
      return new Punc_Op_Token(Colon_Tok); // :
    case 't': {
      Consume();
      if(LookAhead() == 'r') {
	Consume();
	if(LookAhead() == 'u') {
	  Consume();
	  if(LookAhead() == 'e') {
	    Consume();
	    return new Bool_Token(true); // true
	  }
	}
      }
      throw std::runtime_error(GetInvalidCharError());
    }
    case 'f': {
      Consume();
      if(LookAhead() == 'a') {
	Consume();
	if(LookAhead() == 'l') {
	  Consume();
	  if(LookAhead() == 's') {
	    Consume();
	    if(LookAhead() == 'e') {
	      Consume();
	      return new Bool_Token(false); // false
	    }
	  }
	}
      }
      throw std::runtime_error(GetInvalidCharError());
    }
    case '0':
      Buffer();
      // Checks for hex declaration
      if(LookAhead() == 'x' || LookAhead() == 'X') {
	Buffer();
	while(!Eof() && isHex(LookAhead()))
	  Buffer();
	return new Int_Token(std::stoi(buffer, nullptr, 16)); // converts hex string to int
      }
      // Checks for binary declaration
      if(LookAhead() == 'b' || LookAhead() == 'B') {
	Buffer();
	buffer.clear(); // need to drop the 0b; hex can parse this part but not binary
	while(!Eof() && isBin(LookAhead()))
	  Buffer();
	return new Int_Token(std::stoi(buffer, nullptr, 2)); // converts binary string to int
      }
      // If not hex/binary, check for another digit.
      //    If no other digit, return 0. Otherwise drop through into standard number loop.
      if(Eof() || !isdigit(LookAhead()))
	return new Int_Token(0);
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      Buffer();
      while(!Eof() && std::isdigit(LookAhead()))
	Buffer();
      return new Int_Token(std::stoi(buffer)); // converts decimal string to int
    }
    default:
      throw std::runtime_error(GetInvalidCharError());
    }
  }
  return new Punc_Op_Token(Eof_Tok);
}

#endif
