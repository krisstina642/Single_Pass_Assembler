#ifndef EQUSYMBOLS_H_
#define EQUSYMBOLS_H_

#include "symbol.h"
#include "data.h"

class Element
{
public:
    Symbol *section;
    list<Symbol *> *symbols;
    list<bool> signs;
    Element();
    Element(Symbol *s, bool sign);
    ~Element();
};

class EquSymbols
{
private:
    Symbol *mainSymbol;
    list<Element *> *lista;
    list<Symbol *> *symbols;
    list<bool> signs;

public:
    EquSymbols(Symbol *symbolKojiSedodaje, bool sign, Symbol *mainSymbol); // section - sekcija u kojoj se equ nalazi
    EquSymbols();
    ~EquSymbols();
    void add(Symbol *symbol, bool sign);
    bool sort();
    EquSymbols *solve();
};

#endif