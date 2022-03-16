#ifndef FLINK_H_
#define FLINK_H_

#include "data.h"

class Symbol;

class Flink
{
private:
    Data* data;
    bool sign;
    
public:
    Flink(Data* data, bool sign);
    void solve(Symbol* s);
};

#endif
