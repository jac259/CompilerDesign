#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "expr.hpp"

struct Parser {
private:
  std::vector<Token*> tokens;
  std::vector<Token*>::iterator it;
  
  const std::string& GetSyntaxError() {
    static std::string SyntaxError("Invalid syntax.");
    return SyntaxError;
  } // Error message for invalid syntax

  // Iteration & validation
  bool Eof() { return it == tokens.end(); }
  Token * LookAhead() { return (this->Eof() ? new Punc_Op_Token(Eof_Tok) : *it); }
  void Consume() { ++it; }
  Token * ConsumeThis();
  bool Match_If(Token * t, Token_Kind k) { return t->kind == k; }
  bool Match_If(Token_Kind k) { return LookAhead()->kind == k; }
  Token * Match(Token_Kind k);

  // Parse functions
  Expr * ParseExpr();
  Expr * ParseCond();
  Expr * ParseXor();
  Expr * ParseOr();
  Expr * ParseAnd();
  Expr * ParseEqual();
  Expr * ParseOrdering();
  Expr * ParseAdd();
  Expr * ParseMult();
  Expr * ParseUnary();
  Expr * ParsePrimary();
  
public:
  Expr * Parse() { return ParseExpr(); }
  void Print();

  Parser(std::vector<Token*> _tokens) : tokens(_tokens) { it = tokens.begin(); }
  ~Parser() {}
};

void Parser::Print() {
  Expr * e = Parse();
  std::cout << "Input: " << e->Print() << "\n"
	    << "Result: "
	    << e->Evaluate()
	    << "\n\n";

  //(e->Check() == &Bool_ ? (e->Eval() ? "true" : "false") : e->Evaluate())
}

Token * Parser::ConsumeThis() {
  Token * t = LookAhead();
  Consume();
  return t;
}

Token * Parser::Match(Token_Kind k) {
  if(LookAhead()->kind == k)
    return ConsumeThis();
  else
    return nullptr;
    //throw std::runtime_error(GetSyntaxError());
}

Expr * Parser::ParseExpr() {
  return ParseCond();
}

Expr * Parser::ParseCond() {
  Expr * e = ParseXor();
  while(true) {
    if(Match_If(Query_Tok)) {
      Consume();
      Expr * e1 = ParseExpr();
      if(Match_If(Colon_Tok)) {
	Consume();
	e = new Cond_Expr(e, e1, ParseExpr());
      }
      else
	throw std::runtime_error(GetSyntaxError());
    }
    else
      return e;
  }
}

Expr * Parser::ParseXor() {
  Expr * e = ParseOr();
  while(true) {
    if(Match_If(CaretCaret_Tok)) {
      Consume();
      e = new Xor_Expr(e, ParseOr());
    }
    else if(Match_If(Caret_Tok)) {
      Consume();
      e = new Bit_Xor_Expr(e, ParseOr());
    }
    else
      return e;
  }
}

Expr * Parser::ParseOr() {
  Expr * e = ParseAnd();
  while(true) {
    if(Match_If(PipePipe_Tok)) {
      Consume();
      e = new Or_Expr(e, ParseAnd());
    }
    else if(Match_If(Pipe_Tok)) {
      Consume();
      e = new Bit_Or_Expr(e, ParseAnd());
    }
    else
      return e;
  }
}

Expr * Parser::ParseAnd() {
  Expr * e = ParseEqual();
  while(true) {
    if(Match_If(AmpAmp_Tok)) {
      Consume();
      e = new And_Expr(e, ParseEqual());
    }
    else if(Match_If(Amp_Tok)) {
      Consume();
      e = new Bit_And_Expr(e, ParseEqual());
    }
    else
      return e;
  }
}

Expr * Parser::ParseEqual() {
  Expr * e = ParseOrdering();
  while(true) {
    if(Match_If(EqualEqual_Tok)) {
      Consume();
      e = new Equal_Equal_Expr(e, ParseOrdering());
    }
    else if(Match_If(Not_Equal_Tok)) {
      Consume();
      e = new Not_Equal_Expr(e, ParseOrdering());
    }
    else
      return e;
  }
}

Expr * Parser::ParseOrdering() {
  Expr * e = ParseAdd();
  while(true) {
    if(Match_If(LT_Tok)) {
      Consume();
      e = new Less_Than_Expr(e, ParseAdd());
    }
    else if(Match_If(GT_Tok)) {
      Consume();
      e = new Greater_Than_Expr(e, ParseAdd());
    }
    else if(Match_If(LTE_Tok)) {
      Consume();
      e = new Less_Than_Equal_Expr(e, ParseAdd());
    }
    else if(Match_If(GTE_Tok)) {
      Consume();
      e = new Greater_Than_Equal_Expr(e, ParseAdd());
    }
    else
      return e;
  }
}

Expr * Parser::ParseAdd() {
  Expr * e = ParseMult();
  while(true) {
    if(Match_If(Plus_Tok)) {
      Consume();
      e = new Add_Expr(e, ParseMult());
    }
    else if(Match_If(Minus_Tok)) {
      Consume();
      e = new Sub_Expr(e, ParseMult());
    }
    else
      return e;
  }
}

Expr * Parser::ParseMult() {
  Expr * e = ParseUnary();
  while(true) {
    if(Match_If(Star_Tok)) {
      Consume();
      e = new Mult_Expr(e, ParseUnary());
    }
    else if(Match_If(Slash_Tok)) {
      Consume();
      e = new Div_Expr(e, ParseUnary());
    }
    else if(Match_If(Percent_Tok)) {
      Consume();
      e = new Rem_Expr(e, ParseUnary());
    }
    else
      return e;
  }
}

Expr * Parser::ParseUnary() {
  if(Match_If(Bang_Tok)) {
    Consume();
    return new Not_Expr(ParseUnary());
  }
  else if(Match_If(Minus_Tok)) {
    Consume();
    return new Neg_Expr(ParseUnary());
  }
  else if(Match_If(Tilde_Tok)) {
    Consume();
    return new Bit_Comp_Expr(ParseUnary());
  }
  else
    return ParsePrimary();

  // Expr * u;
  // if(Match_If(Bang_Tok)) {
  //   Consume();
  //   u = new Not_Expr(ParsePrimary());
  // }
  // else if(Match_If(Minus_Tok)) {
  //   Consume();
  //   u = new Neg_Expr(ParsePrimary());
  // }
  // else
  //   return ParsePrimary();

  // while(true) {
  //   if(Match_If(Bang_Tok)) {
  //     Consume();
  //     u = new Not_Expr(ParsePrimary());
  //   }
  //   else if(Match_If(Minus_Tok)) {
  //     Consume();
  //     u = new Neg_Expr(ParsePrimary());
  //   }
  //   else
  //     return u;
  // }
}

Expr * Parser::ParsePrimary() {
  if(Match_If(Int_Tok)) {
    Token * t = LookAhead();
    Consume();
    return new Int_Expr(dynamic_cast<Int_Token*>(t)->value);
  }
  else if(Match_If(Bool_Tok)) {
    Token * t = LookAhead();
    Consume();
    return new Bool_Expr(dynamic_cast<Bool_Token*>(t)->value);
  }
  else if(Match_If(LParen_Tok)) {
    Consume();
    Expr * e = ParseExpr();
    if(Match_If(RParen_Tok)) {
      Consume();
      return e;
    }
    else
      throw std::runtime_error(GetSyntaxError());
  }
  else
    throw std::runtime_error("Bottomed out.");
    //return ParseExpr();
}

// struct Token {
//   int kind; // this value defines the kind of Token in the enum
//   virtual ~Token() = default;
//   std::string EnumName() { return Token_Names[kind]; }
// };

// struct Punc_Op_Token : Token {
//   // creates a non-literal token of the passed kind
//   Punc_Op_Token(int tok) { kind = tok; };
// };

// struct Bool_Token : Token {
//   // creates a boolean token with passed value
//   bool value;
//   Bool_Token(bool b) : value(b) { kind = Bool_Tok; };
// };

// struct Int_Token : Token {
//   // creates an integer token with passed value
//   int value;
//   Int_Token(int i) : value(i) { kind = Int_Tok; };
// };

// enum Token_Kind {
//   Eof_Tok,         //  End of file, 0, null
//   Plus_Tok,        //  +
//   Minus_Tok,       //  -
//   Star_Tok,        //  *
//   Slash_Tok,       //  /
//   Percent_Tok,     //  %
//   Amp_Tok,         //  &
//   Pipe_Tok,        //  |
//   AmpAmp_Tok,      //  &&
//   PipePipe_Tok,    //  ||
//   Caret_Tok,       //  ^
//   Tilde_Tok,       //  ~
//   Bang_Tok,        //  !
//   Equal_Tok,       //  =
//   EqualEqual_Tok,  //  ==
//   Not_Equal_Tok,   //  !=
//   LT_Tok,          //  <
//   GT_Tok,          //  >
//   LTE_Tok,         //  <=
//   GTE_Tok,         //  >=
//   Query_Tok,       //  ?
//   Colon_Tok,       //  :
//   LParen_Tok,      //  (
//   RParen_Tok,      //  )
//   Bool_Tok,        //  true/false
//   Int_Tok          //  integers
// };


#endif
