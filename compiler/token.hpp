#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <string>
#include <sstream>

enum Token_Kind {
  Eof_Tok,         //  End of file, 0, null
  Plus_Tok,        //  +
  Minus_Tok,       //  -
  Star_Tok,        //  *
  Slash_Tok,       //  /
  Percent_Tok,     //  %
  Amp_Tok,         //  &
  Pipe_Tok,        //  |
  AmpAmp_Tok,      //  &&
  PipePipe_Tok,    //  ||
  Caret_Tok,       //  ^
  Tilde_Tok,       //  ~
  Bang_Tok,        //  !
  Equal_Tok,       //  =
  EqualEqual_Tok,  //  ==
  Not_Equal_Tok,   //  !=
  LT_Tok,          //  <
  GT_Tok,          //  >
  LTE_Tok,         //  <=
  GTE_Tok,         //  >=
  Query_Tok,       //  ?
  Colon_Tok,       //  :
  LParen_Tok,      //  (
  RParen_Tok,      //  )
  Bool_Tok,        //  true/false
  Int_Tok          //  integers
};

// Used for printing -- this array needs to match the enum above
std::string Token_Names[26] = {
  "Eof_Tok",
  "Plus_Tok",
  "Minus_Tok",
  "Star_Tok",
  "Slash_Tok",
  "Percent_Tok",
  "Amp_Tok",
  "Pipe_Tok",
  "AmpAmp_Tok",
  "PipePipe_Tok",
  "Caret_Tok",
  "Tilde_Tok",
  "Bang_Tok",
  "Equal_Tok",
  "EqualEqual_Tok",
  "Not_Equal_Tok",
  "LT_Tok",
  "GT_Tok",
  "LTE_Tok",
  "GTE_Tok",
  "Query_Tok",
  "Colon_Tok",
  "LParen_Tok",
  "RParen_Tok",
  "Bool_Tok",
  "Int_Tok"
};

struct Token {
  int kind; // this value defines the kind of Token in the enum
  virtual ~Token() = default;
  std::string EnumName() { return Token_Names[kind]; }
};

struct Punc_Op_Token : Token {
  // creates a non-literal token of the passed kind
  Punc_Op_Token(int tok) { kind = tok; };
};

struct Bool_Token : Token {
  // creates a boolean token with passed value
  bool value;
  Bool_Token(bool b) : value(b) { kind = Bool_Tok; };
};

struct Int_Token : Token {
  // creates an integer token with passed value
  int value;
  Int_Token(int i) : value(i) { kind = Int_Tok; };
};

#endif
