#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <iostream>
#include <string.h>
#include <list>
#include "relocation.h"
#include "flink.h"
#include "datatable.h"
#include "relocationtable.h"

using namespace std;

class Symbol
{
private:
    static int Index;
    int index;

    string name;
    Symbol *section;

    int value;
    int size;
    bool word;
    bool word_defined;
    bool defined;
    char gle; //global local extern
    string rwx;

    DataTable *dataTable;
    RelocationTable *relocationTable;
    list<Flink*> *flink;

public:
    Symbol(string name, bool defined, char gle);
    Symbol(string sectionName);
    Symbol(string currentSymbol, bool b, Symbol* currentSectino, int lc, char gle);
    ~Symbol();

    void addflink(Flink* patch);
    void solveflink();

    int getValue();
    Symbol *getSection();
    bool getDefined();
    char getGle();
    RelocationTable *getRelTable();
    DataTable *getDataTable();

    void setGle(char gle);
    void setDefined(bool defined);
    void setSection(Symbol *section);

    void setValue(int value);

    void addRelocation(Relocation *r);
    void printRelocation2( ostream *file);

    Data *addData(Data *s);

    void setSize(int size);
    int getSize();

    void setWord_defined();
    bool getWord_defined();

    string getName();
    string getRwx();
    int getindex();

    void setRwx(string rwx);
    void setindex();

    static int getIndex();
};

#endif
