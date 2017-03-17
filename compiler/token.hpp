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
  CaretCaret_Tok,  //  ^^
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
std::string Token_Names[27] = {
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
  "CaretCaret_Tok",
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
  std::string symbol;
  virtual ~Token() = default;
  std::string EnumName() { return Token_Names[kind]; }
  std::string Print() { return symbol; }
};

struct Punc_Op_Token : Token {
  // creates a non-literal token of the passed kind
  Punc_Op_Token(int tok) {
    kind = tok;

    switch(kind) {
    case Eof_Tok: symbol = ""; break;
    case Plus_Tok: symbol = "+"; break;
    case Minus_Tok: symbol = "-"; break;
    case Star_Tok: symbol = "*"; break;
    case Slash_Tok: symbol = "/"; break;
    case Percent_Tok: symbol = "%"; break;
    case Amp_Tok: symbol = "&"; break;
    case Pipe_Tok: symbol = "|"; break;
    case AmpAmp_Tok: symbol = "&&"; break;
    case PipePipe_Tok: symbol = "||"; break;
    case Caret_Tok: symbol = "^"; break;
    case CaretCaret_Tok: symbol = "^^"; break;
    case Tilde_Tok: symbol = "~"; break;
    case Bang_Tok: symbol = "!"; break;
    case Equal_Tok: symbol = "="; break;
    case EqualEqual_Tok: symbol = "=="; break;
    case LT_Tok: symbol = "<"; break;
    case GT_Tok: symbol = ">"; break;
    case LTE_Tok: symbol = "<="; break;
    case GTE_Tok: symbol = ">="; break;
    case Query_Tok: symbol = "?"; break;
    case Colon_Tok: symbol = ":"; break;
    case LParen_Tok: symbol = "("; break;
    case RParen_Tok: symbol = ")"; break;
    }
  }
};

struct Bool_Token : Token {
  // creates a boolean token with passed value
  bool value;
  Bool_Token(bool b) : value(b) {
    kind = Bool_Tok;
    symbol = value ? "true" : "false";
  };
};

struct Int_Token : Token {
  // creates an integer token with passed value
  int value;
  Int_Token(int i) : value(i) {
    kind = Int_Tok;
    symbol = std::to_string(value);
  };
};

#endif
