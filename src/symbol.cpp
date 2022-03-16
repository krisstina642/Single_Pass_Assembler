#include "symbol.h"

Symbol::Symbol(string name, bool b, Symbol *SEC, int lc, char gle)
{

    this->name = name;
    this->value = lc;
    this->section = SEC;
    this->gle = gle;
    this->size = 0;
    this->index = 0;
    this->defined = b;
    this->word_defined = false;
    flink = new list<Flink *>();
    relocationTable = new RelocationTable(name);
    dataTable = new DataTable(name);
}

Symbol::Symbol(string name)
{
    this->name = name;
    this->value = 0;
    this->gle = 'l';
    this->section = this;
    this->size = 0;
    this->word_defined = false;
    this->index = Symbol::Index++;
    relocationTable = new RelocationTable(name);
    dataTable = new DataTable(name);
    this->defined = true;
}

Symbol::Symbol(string name, bool defined, char gle)
{

    this->name = name;
    this->value = 0;
    this->gle = gle;
    this->section = NULL;
    this->index = 0;
    this->word_defined = false;
    this->size = 0;
    this->defined = false;
    flink = new list<Flink *>();
    relocationTable = new RelocationTable(name);
    dataTable = new DataTable(name);
}

bool Symbol::getDefined()
{
    return defined;
}

char Symbol::getGle()
{
    return gle;
}

void Symbol::setSection(Symbol *s)
{
    this->section = s;
}

void Symbol::setDefined(bool def)
{
    defined = def;
}

void Symbol::setWord_defined()
{
    this->word_defined = true;
}
bool Symbol::getWord_defined()
{
    return word_defined;
}

void Symbol::setGle(char gl)
{
    gle = gl;
}

void Symbol::solveflink()
{
    list<Flink *>::iterator it = flink->begin();
    for (int i = 0; i < flink->size(); i++)
    {
        Flink *p = *it;

        p->solve(this);

        advance(it, 1);
    }
}

void Symbol::addflink(Flink *f)
{
    flink->push_back(f);
}

Symbol::~Symbol()
{

    delete flink;
    if (relocationTable != NULL)
        delete relocationTable;
    if (dataTable != NULL)
        delete dataTable;
}

int Symbol::getValue()
{
    return value;
}
string Symbol::getName()
{
    return name;
}

void Symbol::setValue(int value)
{
    this->value = value;
}

void Symbol::setindex()
{
    this->index = Index++;
}

int Symbol::Index = 0;

RelocationTable *Symbol::getRelTable()
{
    return relocationTable;
}

DataTable *Symbol::getDataTable()
{
    return dataTable;
}

void Symbol::addRelocation(Relocation *r)
{
    relocationTable->add(r);
}

Data *Symbol::addData(Data *s)
{
    dataTable->add(s);
    return s;
}

string Symbol::getRwx()
{
    return rwx;
}

int Symbol::getSize()
{
    return size;
}

Symbol *Symbol::getSection()
{
    return section;
}

void Symbol::setSize(int size)
{
    this->size = size;
}

int Symbol::getIndex()
{
    return Index;
}

int Symbol::getindex()
{
    return index;
}

void Symbol::setRwx(string rwx)
{
    this->rwx = rwx;
}
