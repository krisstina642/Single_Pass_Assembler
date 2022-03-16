#ifndef RELOCATION_H_
#define RELOCATION_H_

#include <string.h>
#include <iostream>
// #include "symbol.h"

class Symbol;

using namespace std;

class Relocation
{
private:
    int offset;
    bool type; // false: R_386_32  true: R_386_PC32
    Symbol *symbol;

public:
    Relocation(int offset, bool type, Symbol *symbol);
    Symbol *getSymbol();
    bool getType();
    int getOffset();
    void setSymbol(Symbol *symbol);
};

#endif
