#include "relocation.h"
//#include "symbol.h"

Relocation::Relocation(int offset, bool type, Symbol *symbol)
{
    this->offset = offset;
    this->type = type;
    this->symbol = symbol; 
}

Symbol *Relocation::getSymbol()
{
    return symbol;
}

bool Relocation::getType()
{
    return type;
}

int Relocation::getOffset()
{
    return offset;
}

void Relocation::setSymbol(Symbol *symbol)
{
    this->symbol = symbol;
}
