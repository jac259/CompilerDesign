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
  Expr * ParseOr();
  Expr * ParseAnd();
  Expr * ParseBitXor();
  Expr * ParseBitOr();
  Expr * ParseBitAnd();
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
	    << "Result: " << e->Evaluate() << "\n\n";
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
}

Expr * Parser::ParseExpr() {
  return ParseCond();
}

Expr * Parser::ParseCond() {
  Expr * e = ParseOr();
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

Expr * Parser::ParseOr() {
  Expr * e = ParseAnd();
  while(true) {
    if(Match_If(PipePipe_Tok)) {
      Consume();
      e = new Or_Expr(e, ParseAnd());
    }
    else
      return e;
  }
}

Expr * Parser::ParseAnd() {
  Expr * e = ParseBitOr();
  while(true) {
    if(Match_If(AmpAmp_Tok)) {
      Consume();
      e = new And_Expr(e, ParseBitOr());
    }
    else
      return e;
  }
}

Expr * Parser::ParseBitOr() {
  Expr * e = ParseBitXor();
  while(true) {
    if(Match_If(Pipe_Tok)) {
      Consume();
      e = new Bit_Or_Expr(e, ParseBitXor());
    }
    else
      return e;
  }
}

Expr * Parser::ParseBitXor() {
  Expr * e = ParseBitAnd();
  while(true) {
    if(Match_If(Caret_Tok)) {
      Consume();
      e = new Bit_Xor_Expr(e, ParseBitAnd());
    }
    else
      return e;
  }
}

Expr * Parser::ParseBitAnd() {
  Expr * e = ParseEqual();
  while(true) {
    if(Match_If(Amp_Tok)) {
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
    throw std::runtime_error("Invalid statement. Could not parse.");
}

#endif
