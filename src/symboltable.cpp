#include "symboltable.h"
#include "assembler.h"

SymbolTable::SymbolTable()
{
    data = new list<Symbol *>();
}

Symbol *SymbolTable::addFromInstruction(string name)
{
    //cout << "Symbol " << name<< " added from instruction \n";

    Symbol *temp = checkIfSymbolExists(name);

    if (temp == NULL)
    {
        temp = new Symbol(name, false, 'l');
        data->push_back(temp);
    }
    return temp;
}

Symbol *SymbolTable::addFromDirective(string name)
{
    //cout << "Symbol " << name<< " added from directive \n";

    Symbol *temp = checkIfSymbolExists(name);

    if (temp!=NULL && temp->getWord_defined()){
        cout<< "GRESKA: Nemoguce definisati velicinu promenljive vise puta: "<<temp->getName();
        exit(0);
    }
    if (temp == NULL)
    {
        temp = new Symbol(name, false, 'l');
        data->push_back(temp);
    }
    temp->setWord_defined();
    return temp;
}

Symbol *SymbolTable::checkIfSymbolExists(string name)
{
    list<Symbol *>::iterator iter = (*data).begin();
    Symbol *temp;
    for (int i = 0; i < (*data).size(); i++)
    {
        temp = *iter;
        if (temp->getName() == name)
        {
            return temp;
        }

        advance(iter, 1);
    }
    return NULL;
}

Symbol* SymbolTable::add(string currentSymbol, bool b, Symbol* sec, int lc, char gle)
{

    Symbol *s=checkIfSymbolExists(currentSymbol);
    if ( s== NULL)
    {
        s = new Symbol(currentSymbol, b, sec, lc, gle);
        data->push_back(s);
        return s;
    }
    if (s->getDefined())
    {
        cout << "Greska: Simbol " + currentSymbol + " je vec definisan" << "\n";
            exit(0);
            return s;
    }
        deleteSymbol(s);

        s->setDefined(b);
        s->setValue(lc);
        s->setSection(sec);
        s->solveflink();
        data->push_back(s);

        return s;
}

void SymbolTable::deleteSymbol(Symbol *s)
{
    data->remove(s);
}

void SymbolTable::addDefined(Symbol* s){
    data->push_back(s);
}

void SymbolTable::externToBottom(){

    list<Symbol*> *p=new list<Symbol*>();

    list<Symbol *>::iterator iter = (*data).begin();
    Symbol *temp, *t;
    int k=(*data).size();
    for (int i = 0; i < k; i++)
    {
        temp = *iter;
        if (!temp->getDefined()){
            cout<< "GRESKA: SIMBOL : "<<temp->getName()<<" NIJE DEFINISAN"<<endl;
            cout<<*this;
            exit(0);
        }

        if (temp!=Assem::UND && temp->getSection()==Assem::UND && temp->getGle()=='g')
        {
            advance(iter, 1);
            data->remove(temp);
            p->push_back(temp);
        }
        else
        {
            advance(iter, 1);
        }
    }
    iter = (*p).begin();
    for (int i = 0; i < (*p).size(); i++)
    {
        temp = *iter;
        data->push_back(temp);
        advance(iter, 1);
    }
    delete(p);
}

ostream& operator<<(ostream &os, const SymbolTable &s) {

    if (s.data->empty()) return os;
    list<Symbol *>::iterator iter = s.data->begin();
    Symbol *temp;
    for (int i = 0; i < s.data->size(); i++)
    {
        temp = *iter; 
        if (temp!=Assem::UND && temp->getindex()==0) temp->setindex();
        if (temp!=Assem::UND && temp->getSection()==NULL) temp->setSection(Assem::UND);

            os << temp->getName();

            for (int i=0; i<5-temp->getName().size();i++)
            os << " ";
            os <<"| ";

               os << temp->getSection()->getindex();
                if (temp->getSection()->getindex()>15) os <<"      | ";
                else os << "       | ";
           
           

            os <<hex<< temp->getValue();
            if (temp->getValue()>15) os <<"       | ";
            else os << "        | ";
          
            os << temp->getGle()<<"   | ";

            os <<hex<< temp->getindex();
            if (temp->getindex()>15) os <<"   | ";
            else os << "    | ";

            os << temp->getRwx();
            for (int i=0; i<3-temp->getRwx().size();i++)
            os << " ";
            os << " | ";
            
            os << temp->getSize() << "\n";
        advance(iter, 1);
    }
    return os;
}

void SymbolTable::printData(ostream * file)
{
    list<Symbol *>::iterator iter = (*data).begin();
    Symbol *temp;
    for (int i = 0; i < (*data).size(); i++)
    {
        temp=*iter;
        string s=temp->getRwx();
         if (temp->getDataTable()!=NULL && !temp->getDataTable()->empty()) *file<< *(temp->getDataTable());
        advance(iter, 1);
    }
   
}

void SymbolTable::printRealocations(ostream *file)
{
    list<Symbol *>::iterator iter = (*data).begin();
    Symbol *temp;
    for (int i = 0; i < (*data).size(); i++)
    {
        temp=*iter;
       if (temp->getRelTable()!=NULL && !temp->getRelTable()->empty()) *file << *(temp->getRelTable());    
        advance(iter, 1);
    }
}

SymbolTable::~SymbolTable(){

    list<Symbol *>::iterator iter = (*data).begin();
     Symbol *temp;
          for (int i = 0; i < (*data).size(); i++)
         {
            temp=*iter;
            delete temp;  
            advance(iter, 1);
         }
}
    
