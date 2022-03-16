#ifndef Assem_H_
#define Assem_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <list>

#include "symbol.h"
#include "symboltable.h"
#include "datatable.h"
#include "equsymbols.h"


enum Directive
{
    BYTE,
    WORD,
    GLOBAL,
    EXTERN,
    RODATA,
    EQU,
    SECTION,
    TEXT,
    DATA,
    BSS,
    SKIP,
    END,
    NOTFOUND,
};

enum Instruction
{
    HALT = 0,
    IRET = 1 << 3,
    RET = 2 << 3,
    INT_INSTRUCTION = 3 << 3,
    CALL = 4 << 3,
    JMP = 5 << 3,
    JEQ = 6 << 3,
    JNE = 7 << 3,
    JGT = 8 << 3,
    PUSH = 9 << 3,
    POP = 10 << 3,
    XCHG = 11 << 3,
    MOV = 12 << 3,
    ADD = 13 << 3,
    SUB = 14 << 3,
    MUL = 15 << 3,
    DIV = 16 << 3,
    CMP = 17 << 3,
    NOT = 18 << 3,
    AND = 19 << 3,
    OR = 20 << 3,
    XOR = 21 << 3,
    TEST = 22 << 3,
    SHL = 23 << 3,
    SHR = 24 << 3,
    ILLEGALINSTRUCTION
};

enum Addressing
{
    IMMED = 0,
    IMMED_HEX,
    IMMED_DEC,                    //vrednost moz da se kodira sa 1 ili 2 operanda --- ne moze destinacioni
    REGDIR = 1 << 5,              //vrednost se nalazi u registru kodiranom u instrukciji moze l/h u slucaju 8 bitnog podatka---
    REGIND_BEZ_POMERAJA = 2 << 5, // vr se nalazi na adresi iz registra 2 bajta instrukcija
    REGIND_16_NUM = 3 << 5,       // podatak sa 16bitni - 3 bajta instrukcija
    REGIND_16_SYMB,
    MEM_DEC = 4 << 5, // nalazi se na adresi koja zauzima 2 bajta
    MEM_SYMB,
    PCREL,
    WORD_addr,
    BYTE_addr,
    REGDIRPOM,
    UNKNOWN
};

class Assem
{
private:
    SymbolTable *symbolList;
    SymbolTable *sections;
    Symbol *currentSection;
    list <EquSymbols *> *equsymbols;
    string inputfileName;
    string outputFileName;

    int lc;
    bool end;
    int row;
    void init();
    void setOutput(string outputFileName);
    Directive hashDirective(string in);
    Instruction hashInstruction(string in);
    Addressing hashAddressing(string in, Instruction i);
    bool isByteInstruction(string instruction);
    string removeAllSpaces(string line);
    void checkIfDirective(string line);
    void checkIfInstruction(string line);
    bool executionAllowed();
    bool dataAllowed();
    bool findSymbol(string s);
    int regNum(string line, Addressing addressing, Instruction instruction);
    Symbol *checkIfSymbolExists(string name);
    Symbol *checkIfSectionUsed(string name);
    Data *addOperand(string operand, bool isbyteinstruction, Addressing addressing, bool jmp);
    void writeErrorInFile(string message);
    void parse(string *line);

public:
    static Symbol *UND;
    void start();
    Assem(string inputfileName, string outputFileName);
    

    ~Assem();
};

#endif
