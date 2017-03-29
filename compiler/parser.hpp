#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "expr.hpp"
#include "stmt.hpp"

struct Parser {
private:
  std::vector<Token*> tokens;
  std::vector<Token*>::iterator it;
  Context* cxt;
  
  const std::string& GetSyntaxError() {
    static std::string SyntaxError("Invalid syntax.");
    return SyntaxError;
  } // Error message for invalid syntax

  // Iteration & validation
  bool Eof() { return it == tokens.end(); }
  Token * LookAhead() { return (this->Eof() ? new Punc_Op_Token(Eof_Tok) : *it); }
  Token * LookAhead(int);
  void Consume() { ++it; }
  Token * ConsumeThis();
  bool Match_If(Token * t, Token_Kind k) { return t->kind == k; } // compares two token kinds
  bool Match_If(Token_Kind k) { return LookAhead()->kind == k; }
  Token * Match(Token_Kind k);
  Token * Require(Token_Kind k);

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

  Stmt * ParseStmt();
  Stmt * ParseDeclStmt();
  Stmt * ParseExprStmt();
  
  Decl * ParseDecl();
  Decl * ParseVarDecl();
  Decl * ParseVarReDecl();

  const Type * ParseType();
  const std::string ParseId();
  
  
public:
  Stmt * Parse() { return ParseStmt(); }
  void Print();

  // Constructor
  Parser(std::vector<Token*> _tokens, Context* _cxt) : tokens(_tokens), cxt(_cxt) {
    it = tokens.begin();
  }
  ~Parser() {}
};

// Parses tokens and displays results
void Parser::Print() {
  Stmt * s = Parse(); // get statement
  
  if(Expr_Stmt* exp = dynamic_cast<Expr_Stmt*>(s)) { // Statement is an expressions
    std::cout << "Input: " << exp->e->Print() << "\n"
	      << "Result: " << exp->e->Evaluate() << "\n\n";
  }
  else if(Decl_Stmt* dec = dynamic_cast<Decl_Stmt*>(s)) { // Statement is a declaration
    if(Var_Decl* vd = dynamic_cast<Var_Decl*>(dec->d)) { // Declaration is a variable declaration
      std::cout << "Input: " << vd->name << " = " << vd->fullInit->Print() << "\n"
      		<< "Result: " << vd->name << " = " << vd->init->Evaluate() << "\n\n";
    }
  }
}

// Peek a given number of tokens ahead
Token * Parser::LookAhead(int count) {
  if(this->Eof())
    return new Punc_Op_Token(Eof_Tok);

  std::vector<Token*>::iterator temp = it;
  
  for(int i = 0; i < count; ++i) {
    ++temp;
    if(temp == tokens.end())
      return nullptr;
  }
  return *temp;
}

// Consumes and returns consumed token
Token * Parser::ConsumeThis() {
  Token * t = LookAhead();
  Consume();
  return t;
}

// Compares lookahead kind to passed token kind, consumes and returns token
Token * Parser::Match(Token_Kind k) {
  if(LookAhead()->kind == k)
    return ConsumeThis();
  else
    return nullptr;
}

// Same as Match above but throws exception on a failure
Token * Parser::Require(Token_Kind k) {
  if(LookAhead()->kind == k)
    return ConsumeThis();
  else
    throw std::runtime_error("Missing expected symbol: " + Token_Names[k]);
}

// Parses a statement
Stmt * Parser::ParseStmt() {
  switch (LookAhead()->kind) {
  case Var_KW:
    return ParseDeclStmt(); // var -> declaration
  case Id_Tok:
    if(LookAhead(1) && Match_If(LookAhead(1), Equal_Tok))
      return ParseDeclStmt(); // 'id_token =' -> declaration
  default:
    return ParseExprStmt(); // otherwise expression
  }
}

// Parses a declaration statement
Stmt * Parser::ParseDeclStmt() {
  Decl* d = ParseDecl();
  return new Decl_Stmt(d);
}

// Parses an expression statement
Stmt * Parser::ParseExprStmt() {
  Expr* e = ParseExpr();
  Match(Semicolon_Tok); // allow semicolon but don't require yet
  return new Expr_Stmt(e);
}

// Parses a declaration
Decl * Parser::ParseDecl() {
  switch (LookAhead()->kind) {
  case Var_KW:
    return ParseVarDecl();
  case Id_Tok:
    return ParseVarReDecl();
  default:
    break;
  }
  throw std::runtime_error("Invalid declaration.");
}

// Parses a variable declaration
Decl * Parser::ParseVarDecl() {
  Require(Var_KW); // require var
  const Type* t = ParseType(); // get type
  const std::string n = ParseId(); // get identifier

  if(cxt->FindSymbol(n)) // check for existing var
    throw std::runtime_error("That variable name already exists.");
  
  Var_Decl* var = new Var_Decl(cxt, n, t);
  Require(Equal_Tok); // require =
  
  Expr* e = ParseExpr();
  
  if(e->Check() != t) // compare var type to expr type
    throw std::runtime_error("Expression type does not match variable type.");

  var->fullInit = e; // store expanded expression for printing
  var->init = e->Precompute(); // store compressed expression for calculations
  
  Match(Semicolon_Tok); // allow semicolon
  cxt->InsertSymbol(var); // add var to symbol table
  
  return var;
}

// Parses a variable reassignment
Decl * Parser::ParseVarReDecl() {
  Id_Token* t = dynamic_cast<Id_Token*>(Require(Id_Tok)); // get identifier

  if(Var_Decl* var = dynamic_cast<Var_Decl*>(cxt->FindSymbol(t->name))) {
    Require(Equal_Tok); // require =

    Expr* e = ParseExpr();

    if(e->Check() != var->type) // compare var type to expr type
      throw std::runtime_error("Expression type does not match variable type.");

    var->fullInit = e; // store expanded expression for printing
    var->init = e->Precompute(); // store compressed expression for calculations

    Match(Semicolon_Tok); // allow semicolon
    cxt->UpdateSymbol(var->getName(), var); // update var on symbol table
    
    return var;
  }

  throw std::runtime_error("Variable does not exist.");
  
}

// Parses a type identifier
const Type * Parser::ParseType() {
  switch(LookAhead()->kind) {
  case Bool_KW:
    Consume();
    return &(cxt->Bool_);
  case Int_KW:
    Consume();
    return &(cxt->Int_);
  }
  throw std::runtime_error("Missing variable type definition.");
}

// Parses an identifier
const std::string Parser::ParseId() {
  return dynamic_cast<Id_Token*>(Require(Id_Tok))->name;
}

// Parse expression
Expr * Parser::ParseExpr() {
  return ParseCond();
}

// Parse conditional
Expr * Parser::ParseCond() {
  Expr * e = ParseOr();
  while(true) {
    if(Match_If(Query_Tok)) {
      Consume();
      Expr * e1 = ParseExpr();
      if(Match_If(Colon_Tok)) {
	Consume();
	e = new Cond_Expr(e, e1, ParseExpr(), cxt);
      }
      else
	throw std::runtime_error(GetSyntaxError());
    }
    else
      return e;
  }
}

// Parse logical OR
Expr * Parser::ParseOr() {
  Expr * e = ParseAnd();
  while(true) {
    if(Match_If(PipePipe_Tok)) {
      Consume();
      e = new Or_Expr(e, ParseAnd(), cxt);
    }
    else
      return e;
  }
}

// Parse logical AND
Expr * Parser::ParseAnd() {
  Expr * e = ParseBitOr();
  while(true) {
    if(Match_If(AmpAmp_Tok)) {
      Consume();
      e = new And_Expr(e, ParseBitOr(), cxt);
    }
    else
      return e;
  }
}

// Parse bitwise OR
Expr * Parser::ParseBitOr() {
  Expr * e = ParseBitXor();
  while(true) {
    if(Match_If(Pipe_Tok)) {
      Consume();
      e = new Bit_Or_Expr(e, ParseBitXor(), cxt);
    }
    else
      return e;
  }
}

// Parse bitwise XOR
Expr * Parser::ParseBitXor() {
  Expr * e = ParseBitAnd();
  while(true) {
    if(Match_If(Caret_Tok)) {
      Consume();
      e = new Bit_Xor_Expr(e, ParseBitAnd(), cxt);
    }
    else
      return e;
  }
}

// Parse bitwise AND
Expr * Parser::ParseBitAnd() {
  Expr * e = ParseEqual();
  while(true) {
    if(Match_If(Amp_Tok)) {
      Consume();
      e = new Bit_And_Expr(e, ParseEqual(), cxt);
    }
    else
      return e;
  }
}

// Parse equality & inequality
Expr * Parser::ParseEqual() {
  Expr * e = ParseOrdering();
  while(true) {
    if(Match_If(EqualEqual_Tok)) {
      Consume();
      e = new Equal_Equal_Expr(e, ParseOrdering(), cxt);
    }
    else if(Match_If(Not_Equal_Tok)) {
      Consume();
      e = new Not_Equal_Expr(e, ParseOrdering(), cxt);
    }
    else
      return e;
  }
}

// Parse <, >, <=, >=
Expr * Parser::ParseOrdering() {
  Expr * e = ParseAdd();
  while(true) {
    if(Match_If(LT_Tok)) {
      Consume();
      e = new Less_Than_Expr(e, ParseAdd(), cxt);
    }
    else if(Match_If(GT_Tok)) {
      Consume();
      e = new Greater_Than_Expr(e, ParseAdd(), cxt);
    }
    else if(Match_If(LTE_Tok)) {
      Consume();
      e = new Less_Than_Equal_Expr(e, ParseAdd(), cxt);
    }
    else if(Match_If(GTE_Tok)) {
      Consume();
      e = new Greater_Than_Equal_Expr(e, ParseAdd(), cxt);
    }
    else
      return e;
  }
}

// Parse addition & subtraction
Expr * Parser::ParseAdd() {
  Expr * e = ParseMult();
  while(true) {
    if(Match_If(Plus_Tok)) {
      Consume();
      e = new Add_Expr(e, ParseMult(), cxt);
    }
    else if(Match_If(Minus_Tok)) {
      Consume();
      e = new Sub_Expr(e, ParseMult(), cxt);
    }
    else
      return e;
  }
}

// Parse multiplication, division, & modulo
Expr * Parser::ParseMult() {
  Expr * e = ParseUnary();
  while(true) {
    if(Match_If(Star_Tok)) {
      Consume();
      e = new Mult_Expr(e, ParseUnary(), cxt);
    }
    else if(Match_If(Slash_Tok)) {
      Consume();
      e = new Div_Expr(e, ParseUnary(), cxt);
    }
    else if(Match_If(Percent_Tok)) {
      Consume();
      e = new Rem_Expr(e, ParseUnary(), cxt);
    }
    else
      return e;
  }
}

// Parse logical NOT, arithmetic negation, & bitwise complement
Expr * Parser::ParseUnary() {
  if(Match_If(Bang_Tok)) {
    Consume();
    return new Not_Expr(ParseUnary(), cxt);
  }
  else if(Match_If(Minus_Tok)) {
    Consume();
    return new Neg_Expr(ParseUnary(), cxt);
  }
  else if(Match_If(Tilde_Tok)) {
    Consume();
    return new Bit_Comp_Expr(ParseUnary(), cxt);
  }
  else
    return ParsePrimary();
}

// Parse integers, booleans, parenthesized expressions, & identifiers 
Expr * Parser::ParsePrimary() {
  if(Match_If(Int_Tok)) {
    Token * t = LookAhead();
    Consume();
    return new Int_Expr(dynamic_cast<Int_Token*>(t)->value, cxt);
  }
  else if(Match_If(True_KW)) {
    Consume();
    return new Bool_Expr(true, cxt);
  }
  else if(Match_If(False_KW)) {
    Consume();
    return new Bool_Expr(false, cxt);
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
  else if(Match_If(Id_Tok)) {
    Token * t = LookAhead();
    Consume();
    
    if(Var_Decl * vd = dynamic_cast<Var_Decl*>(cxt->FindSymbol(dynamic_cast<Id_Token*>(t)->name)))
      return vd->init;
    
    throw std::runtime_error("Undeclared variable.");       
  }
  else
    throw std::runtime_error("Invalid statement. Could not parse.");
}

#endif
