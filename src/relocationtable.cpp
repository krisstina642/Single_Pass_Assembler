#include "relocationtable.h"
#include "symbol.h"

RelocationTable::RelocationTable(string name)
{
    this->name = name;
    data = new list<Relocation *>();
}

bool RelocationTable::empty(){
    return data->empty();
}

RelocationTable::~RelocationTable()
{
    list<Relocation *>::iterator it = data->begin();
    for (int i = 0; i < data->size(); i++)
    {
        Relocation *rec = *it;
        delete rec;
        advance(it, 1);
    }
    delete data;
}

void RelocationTable::add(Relocation *component)
{
    data->push_back(component);
}

ostream& operator<<(ostream &os, const RelocationTable &ret) {

    if (ret.data->empty()) return os;
    os << "#.rel."<<ret.name<< endl;
    os << " ofset |    tip     |   vr"<< endl;

    list<Relocation *>::iterator iter = ret.data->begin();
    Relocation *temp;
    for (int i = 0; i < ret.data->size(); i++)
    {
        temp = *iter;
        os << hex << temp->getOffset();

        if (temp->getOffset()<16) os << "      | ";
        else if (temp->getOffset()<16^2) os << "     | ";
        else os << "    | ";
        
         os << ((temp->getType() == true) ? "R_386_PC32 | " : "R_386_32   | ") <<hex<< ((temp->getSymbol()->getGle() == 'g' || temp->getSymbol()->getSection()->getindex()==0)? temp->getSymbol()->getindex() : temp->getSymbol()->getSection()->getindex()) << "\n";
        advance(iter, 1);
    }
    os << "\n";
    return os;
}
