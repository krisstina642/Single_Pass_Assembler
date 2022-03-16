#ifndef RELOCATIONTABLE_H_
#define RELOCATIONTABLE_H_

#include <iostream>
#include <string.h>
#include <list>
#include "relocation.h"

class Symbol;

class RelocationTable
{
private:
        list<Relocation *> *data;
        string name;

public:
        RelocationTable(string name);
        void add(Relocation *component);
        ~RelocationTable();
        friend ostream& operator<<(ostream& os, const RelocationTable& ret);
        bool empty();
};

#endif
