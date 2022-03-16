#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <string.h>
#include <list>
#include "symbol.h"

class Assem;

class SymbolTable
{
private:
    list<Symbol *> *data;
    
public:
    SymbolTable();
    Symbol* add(string currentSymbol, bool defined, Symbol* sec, int lc, char gle);
    Symbol *addFromInstruction(string name); 
    Symbol *addFromDirective(string name);
    void addDefined(Symbol* s);
    Symbol *checkIfSymbolExists(string name);
    
    void deleteSymbol(Symbol* s);
    ~SymbolTable();

    void printRealocations(ostream* os);
    void printData(ostream* os);
    void externToBottom();
    friend ostream& operator<<(ostream& os, const SymbolTable& s);
    
};

#endif