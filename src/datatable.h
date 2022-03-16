#ifndef SECTIONDATATABLE_H
#define SECTIONDATATABLE_H

#include <iostream>
#include <string.h>
#include <list>
#include "data.h"
#include "relocationtable.h"

class DataTable
{
private:
    list<Data *> *data;
    string name;

public:
    DataTable(string name);
    void add(Data *component);
    friend ostream& operator<<(ostream& os, const DataTable& d);
    bool empty();

    ~DataTable();
};

#endif