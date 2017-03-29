#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "type.hpp"
#include "token.hpp"
#include "decl.hpp"

#include <unordered_map>

struct Context {

  const Bool_Type Bool_; // bool type
  const Int_Type Int_; // int type
  char outputFormat; // output format for integers
  std::unordered_map<std::string, Decl*> SymTable; // symbol table

  Context(char _outputFormat) : outputFormat(_outputFormat) {} // constructor
  Token * CheckKeyword(const std::string);
  Token * InsertSymbol(Decl*);
  Decl * FindSymbol(const std::string);
  void UpdateSymbol(const std::string, Decl*);
};

// Find keyword in keyword table
Token * Context::CheckKeyword(const std::string str) {
  Keyword_Table kws;
  auto it = kws.find(str);
  if(it != kws.end()) {
    Token * t = new Id_Token(str, it->second);
    return t;
  }
  return nullptr;
}

// Add symbol to symbol table
Token * Context::InsertSymbol(Decl* d) {
  const std::string name = d->getName();
  auto it = SymTable.find(name);
  if(it == SymTable.end())
    SymTable.insert({name, d}); // only add when not already existing
  return new Id_Token(name);
}

// Find symbol in symbol table
Decl * Context::FindSymbol(const std::string str) {
  auto it = SymTable.find(str);
  if(it != SymTable.end())
    return it->second;

  return nullptr;
}

// Change symbol in symbol table
void Context::UpdateSymbol(const std::string str, Decl* d) {
  auto it = SymTable.find(str);
  if(it == SymTable.end())
    throw std::runtime_error("Symbol does not exist. Could not update symbol.");

  SymTable[str] = d;
  return;
}

#endif
