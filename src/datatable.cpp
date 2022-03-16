#include "datatable.h"

DataTable::DataTable(string name)
{
    this->name = name;
    data = new list<Data *>();
}

bool DataTable::empty(){
    return data->empty();
}

DataTable::~DataTable()
{
    list<Data *>::iterator it = data->begin();
    for (int i = 0; i < data->size(); i++)
    {
        Data *rec = *it;
        delete rec;
        advance(it, 1);
    }
    delete data;
}

ostream& operator<<(ostream &os, const DataTable &d) {
    
    int k=16;
     os << "#."<<d.name<< endl;
    list<Data *>::iterator iter = d.data->begin();
    Data *temp;
    for (int i = 0; i < d.data->size(); i++)
    {
        temp = *iter;
        int size = temp->getSize();
        int p = temp->getData();
        if (size > 7)
        {
            for (int i = 0; i < size; i++){
                 os << "00 ";
                 k--;
                if (k==0) { os<<endl; k=16;}
            }
                
        }
        else
            for (int i = 0; i <size; i++)
            {
                p = (temp->getData() >> (i * 8)) & 0xFF;
                if (p <= 0xF)
                    os << 0;
                os << hex << p << " ";
                k--;
                if (k==0) { os<<endl; k=16;}
            }

        advance(iter, 1);
    }
    os << endl;
    if (k!=16) os<<endl;
    return os; 
}

void DataTable::add(Data *component)
{
    data->push_back(component);
}
