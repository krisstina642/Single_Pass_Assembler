#include "data.h"
#include "flink.h"
#include "symbol.h"
#include "assembler.h"
#include "equsymbols.h"

Element::Element(Symbol *s, bool sign)
{
    symbols = new list<Symbol *>();
    symbols->push_back(s);
    signs.push_back(sign);
    this->section = s->getSection();
}
Element::Element(){};

Element::~Element()
{
    symbols->~list();
};

EquSymbols::EquSymbols(Symbol *s, bool sign, Symbol *mainSymbol)
{

    this->mainSymbol = mainSymbol;
    symbols = new list<Symbol *>();
    lista = new list<Element *>();
    symbols->push_back(s);
    signs.push_back(sign);
}

EquSymbols::EquSymbols()
{
}

EquSymbols::~EquSymbols()
{
    symbols->~list();
    lista->~list();
}

bool EquSymbols::sort()
{
    lista->clear();

    list<Symbol *>::iterator iterS = (*symbols).begin();
    list<bool>::iterator iterB = (signs).begin();
    Symbol *s;
    bool sign;
    Element *temp;
    for (int j = 0; j < (*symbols).size(); j++)
    {
        s = *iterS;
        sign = *iterB;

        if (!s->getDefined())
            return false;

        list<Element *>::iterator iter = (*lista).begin();
        temp = NULL;
        for (int i = 0; i < (*lista).size(); i++)
        {
            if ((*iter)->section == s->getSection())
            {
                temp = *iter;
                break;
            }
            advance(iter, 1);
        }

        if (temp != NULL && temp->section == Assem::UND && s->getSection() == Assem::UND)
        {
            cout << "GRESKA: vise od jedne externe promenljive u equ direktivi";
            exit(0);
        }
        if (temp == NULL)
        {
            Element *ee = new Element(s, sign);
            lista->push_back(ee);
        }
        else if (temp != NULL)
        {
            temp->symbols->push_back(s);
            temp->signs.push_back(sign);
        }

        advance(iterS, 1);
        advance(iterB, 1);
    }
    return true;
}

void EquSymbols::add(Symbol *s, bool sign)
{

    symbols->push_back(s);
    signs.push_back(sign);
}

EquSymbols *EquSymbols::solve()
{
    if (!sort())
        return NULL;
    bool ext = false;
    int prev = 0;
    int num = 0;
    Symbol *rel = NULL, *currel = NULL;
    ext = false;

    list<Element *>::iterator iter = (*lista).begin();
    Element *elem;

    while (0 < (*lista).size())
    {

        elem = *iter;

       // cout << "Equ sort sekcija: " << elem->section->getName() << endl;

        if (elem->section == Assem::UND)
        {
            ext = true;
            rel = elem->symbols->front();
          //  cout << "Externi: " << rel->getName() << endl;
            mainSymbol->setDefined(true);
            mainSymbol->setGle('g');
            mainSymbol->setSection(Assem::UND);
            advance(iter, 1);
            lista->remove(elem);
            continue;
        }
        num = 0;

        list<Symbol *>::iterator iter2 = (*(elem->symbols)).begin();
        list<bool>::iterator iter3 = elem->signs.begin();
        Symbol *s;
        while (0 < (*(elem->symbols)).size())
        {

            s = *iter2;
           // cout << ((*iter3) ? '+' : '-') << s->getName() << " " << endl;

            if (!s->getDefined())
                return NULL;

            if (*iter3 == true && currel == NULL)
            {
                currel = s;
            }
            if (*iter3 == true)
            {
                num = num + 1;
                mainSymbol->setValue(mainSymbol->getValue() + s->getValue());
            }
            else
            {
                num = num - 1;
                mainSymbol->setValue(mainSymbol->getValue() - s->getValue());
            }

            advance(iter2, 1);
            advance(iter3, 1);
            elem->symbols->pop_front();
            elem->signs.pop_front();
        }

        if (num < 0)
        {
            cout << "GRESKA: Equ direktiva nije ispravna - negativna vrednost promenljivih"<<endl;
            cout<< "Simbol: "<<mainSymbol->getName()<<endl;
            exit(0);
        }
        if (num > 1)
        {
            cout << "GRESKA: Equ direktiva nije ispravna - postoji vise od jedne promenljive iste sekcije"<<endl;
            cout<< "Simbol: "<<mainSymbol->getName()<<endl;
            exit(0);
        }
        if (ext && num != 0)
        {
            cout << "GRESKA: Equ direktiva nije ispravna - nemoguce napraviti realokacije i za externi i za interni/globalni simbol"<<endl;
            cout<< "Simbol: "<<mainSymbol->getName()<<endl;
            exit(0);
        }
        if (prev > 0 && num > 0)
        {
            cout << "GRESKA: Equ direktiva nije ispravna - postoje realokacije za vise sekcija"<<endl;
            cout<< "Simbol: "<<mainSymbol->getName()<<endl;
            exit(0);
        }
        if (prev == 0 && num == 1)
        {
            prev = 1;
            rel = currel;
        }
        currel = NULL;
        advance(iter, 1);
        lista->remove(elem);
    }
    if (ext == true && prev > 0)
    {
        cout << "GRESKA: Equ direktiva nije ispravna - nemoguce napraviti realokacije i za externi i za interni/globalni simbol "<<endl;
        cout<< "Simbol: "<<mainSymbol->getName()<<endl;
        exit(0);
    }
    mainSymbol->setDefined(true);

    if (rel != NULL)
    {
        mainSymbol->setGle(rel->getGle());
        mainSymbol->setSection(rel->getSection());
    }
    else
    {
        mainSymbol->setGle('l');
    }
    return this;
}

Data::Data(int size, int data)
{
    this->data = data;
    this->size = size;
}

Data::Data(int size)
{
    this->size = size;
}

void Data::addData(int data)
{
    this->data = this->data + data;
}

void Data::setData(int data)
{
    this->data = data;
}

void Data::subData(int data)
{
    this->data = this->data - data;
}

int Data::getData()
{
    return data;
}

int Data::getSize()
{
    return size;
}

Flink::Flink(Data *data, bool sign)
{
    this->data = data;
    this->sign = sign;
}

void Flink::solve(Symbol *a)
{
    (sign) ? data->addData(a->getValue()) : data->subData(a->getValue());
}

void Data::setSize(int s)
{
    this->size = s;
}