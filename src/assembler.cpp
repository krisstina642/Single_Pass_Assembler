#include "assembler.h"
#include <list>
#include <iterator>
#include <regex>
#include <cmath>
#include "relocationtable.h"
#include "symboltable.h"
#include <ctype.h>

using namespace std;

Symbol *Assem::UND = new Symbol("UND");

Assem::~Assem()
{
    delete sections;
    delete symbolList;
    delete equsymbols;
}

void Assem::init()
{
    lc = 0;
    end = false;
    row = 1;
    symbolList = new SymbolTable();
    sections = new SymbolTable();
    equsymbols = new list<EquSymbols *>();
    currentSection = NULL;
    Assem::UND->setRwx("-");
    sections->addDefined(Assem::UND);
}

void Assem::start(){

    ifstream inputFile(inputfileName, ios::in);
    char line[40];

    ofstream *file = new ofstream(outputFileName);
    *file << "GRESKA";
    file->close();

    init();
    while (inputFile && !end)
    {
        inputFile.getline(line, 40);
        string *newline = new string(line);
        if ((*newline).empty() || (*newline)[0] == '\n')
        {
            row++;
            continue;
        }

        int start = (*newline).find_first_not_of(" \t\n");
        if (start == string::npos)
        {
            row++;
            continue;
        }

        (*newline) = (*newline).substr(start);
        cout << "load counter: " << lc << endl;
        cout << *newline << endl;
        parse(newline);
        row++;
    }
    inputFile.close();
    setOutput(outputFileName);

}

Assem::Assem(string inputfileName, string outputFileName)
{
    this->inputfileName=inputfileName;
    this->outputFileName=outputFileName;  
}

void Assem::parse(string *line)
{

    if ((*line)[0] == '.')
    {
        checkIfDirective(*line);
        return;
    }

    int result = (*line).find_first_of(":");
    if (result != string::npos)
    {
        string currentSymbol = (*line).substr(0, result);
        (*line) = (*line).substr(result + 1);
        // cout << (*line) << endl;

        result = currentSymbol.find_first_of(" \t\n");

        if (result != string::npos)
        {
            currentSymbol = currentSymbol.substr(0, result);
        }
        Symbol *s = symbolList->add(currentSymbol, true, currentSection, lc, 'l');

        result = (*line).find_first_not_of(" \t\n");
        if (result == string::npos)
            return;
        (*line) = (*line).substr(result);

        if ((*line)[0] == '.')
        {
           // cout << "new directive" << *line << endl;
            checkIfDirective(*line);
            return;
        }
    }

    // cout << "instrukcija"<< endl;
    checkIfInstruction(*line);
}

void Assem::checkIfDirective(string line)
{
    int startIndex = line.find_first_of(" \t\n");
    string directive;
    if (startIndex != string::npos)
    {
        directive = line.substr(0, startIndex);
        line = line.substr(startIndex + 1);
        int ind = line.find_first_not_of(" \t\n");
        line = line.substr(ind);
    }
    else
    {
        directive = line;
        if (startIndex = directive.find_first_of("\r") != string::npos)
        {
            directive = directive.substr(0, startIndex);
        }
    }
    regex reg;
    smatch matches;
    string sectionName, rwx, newSymbol;
    Symbol *findSymb;
    Symbol *newSection = NULL, *s = NULL;
    DataTable *sdt;
    Data *newData;
    RelocationTable *rt;
    int it = 0, result = 0;

    Directive dir = hashDirective(directive);

    switch (dir)
    {

    case EQU:
    {
        int it = 0;
        reg = ("[+-]?([0-9]+|[a-zA-Z][a-zA-Z0-9]*)");
        EquSymbols *es = NULL;
        line = removeAllSpaces(line);
        Symbol *mainSymb;

        for (auto iter = sregex_iterator(line.begin(), line.end(), reg);
             iter != sregex_iterator(); ++iter)
        {
            matches = *iter;
            newSymbol = matches.str();
            regex reg2;
            smatch match;

            reg2 = "([a-zA-Z][a-zA-Z0-9]*)";
            if (it == 0)
            {
                if (regex_search(newSymbol, match, reg2))
                {
                    mainSymb = symbolList->add(newSymbol, false, currentSection, 0, 'l');
                    it++;
                }
                else
                {
                    cout << "Nije dobar simbol u EQU direktivi" << endl;
                    exit(0);
                }
            }
            else if (regex_search(newSymbol, match, reg2))
            {
                newSymbol = match.str();
                findSymb = sections->checkIfSymbolExists(newSymbol);
                if (findSymb == NULL)
                    findSymb = symbolList->checkIfSymbolExists(newSymbol);
                if (findSymb == NULL)
                    findSymb = symbolList->addFromInstruction(newSymbol);

                if (es == NULL && matches.str()[0] == '-')
                    es = new EquSymbols(findSymb, false, mainSymb);
                else if (es == NULL)
                    es = new EquSymbols(findSymb, true, mainSymb);
                else if (matches.str()[0] == '-')
                    es->add(findSymb, false);
                else
                    es->add(findSymb, true);
            }
            else if (matches.str()[0] == '-')
                mainSymb->setValue(mainSymb->getValue() - atoi(newSymbol.substr(1).c_str()));
            else if (matches.str()[0] == '+')
                mainSymb->setValue(mainSymb->getValue() + atoi(newSymbol.substr(1).c_str()));
            else
                mainSymb->setValue(mainSymb->getValue() + atoi(newSymbol.c_str()));
        }
        if (es != NULL)
            equsymbols->push_back(es);
        else
            mainSymb->setDefined(true);
    }

    break;
    case SECTION:

        reg = ("(\\w+)");
        it = 0;
        for (auto i = sregex_iterator(line.begin(), line.end(), reg);
             i != sregex_iterator(); ++i)
        {
            matches = *i;

            if (it == 0)
                sectionName = matches.str();
            else if (it == 1)
                rwx = matches.str();
            else
            {
                cout << "GRESKA: previse argumenata za .Section nakon sekcije " << currentSection->getName() << endl;
                exit(0);
            }
            it++;
        }
        if (it != 2 || rwx=="")
            {
                rwx="rw";
            }

        s=checkIfSectionUsed(sectionName);
        if (s== NULL)
        {
            s = new Symbol(sectionName);
            s->setRwx(rwx);
            sections->addDefined(s);
            currentSection = s;
            lc = 0;
        }
        else if (s->getName()!="text" && s->getName()!="data" && s->getName()!="bss"){
            s->setRwx(rwx);
        }
        break;

    case TEXT:
    case DATA:
    case BSS:
        directive = line.substr(1);
        if (checkIfSectionUsed(directive) == NULL)
        {
            s = new Symbol(directive);
            if (directive == "text")
                s->setRwx("rxp");
            else if (directive == "data")
                s->setRwx("rwp");
            else
                s->setRwx("rw");
            sections->addDefined(s);
            currentSection = s;
            lc = 0;
        }
        break;
    case RODATA:
        if (checkIfSectionUsed("RODATA") == NULL)
        {
            s = new Symbol("RODATA");
            s->setRwx("r");
            sections->addDefined(s);
            currentSection = s;
            lc = 0;
        }
        break;

    case EXTERN:

        reg = ("([0-9]+|[a-zA-Z][a-zA-Z0-9]*)");
        for (auto iter = sregex_iterator(line.begin(), line.end(), reg);
             iter != sregex_iterator(); ++iter)
        {
            matches = *iter;
            newSymbol = matches.str();
            findSymb = checkIfSymbolExists(newSymbol);
            if (findSymb == NULL)
            {
                symbolList->add(newSymbol, true, Assem::UND, 0, 'g');
            }
            else if (findSymb->getDefined() == false && findSymb->getGle() == 'l')
            {
                findSymb->setDefined(true);
                findSymb->setGle('g');
            }
            else
            {
                cout << "GRESKA: SIMBOL " << newSymbol << " JE VEC DEFINISAN";
            }
        }
        break;

    case GLOBAL:

        reg = ("([0-9]+|[a-zA-Z][a-zA-Z0-9]*)");
        for (auto iter = sregex_iterator(line.begin(), line.end(), reg);
             iter != sregex_iterator(); ++iter)
        {
            matches = *iter;
            newSymbol = matches.str();
            findSymb = checkIfSymbolExists(newSymbol);
            if (findSymb == NULL)
            {
                symbolList->add(newSymbol, false, NULL, 0, 'g');
            }
            else
            {
                cout << "GRESKA: SIMBOL " << newSymbol << " je vec koriscen, nemoguce promeniti vid";
            }
        }

        break;
    case BYTE:
    case WORD:

        if (!dataAllowed())
        {
            cout << "GRESKA: Ne moze byte/Word u" << currentSection->getName() << " sekciji" << endl;
            exit(0);
        }

        reg = ("([0-9]+|[a-zA-Z][a-zA-Z0-9]*)");
      //  cout << line << endl;

        for (auto iter = sregex_iterator(line.begin(), line.end(), reg);
             iter != sregex_iterator(); ++iter)
        {
            matches = *iter;
            newSymbol = matches.str();
            regex reg2;
            smatch match;
            reg2 = "[a-zA-Z][a-zA-Z0-9]*";
            if (regex_search(newSymbol, match, reg2))
            {
                if (dir == WORD)
                    addOperand(newSymbol, false, WORD_addr, false);
                else if (dir == BYTE)
                    addOperand(newSymbol, false, BYTE_addr, false);
                continue;
            }
            else
            {
                currentSection->addData((dir == WORD) ? new Data(2, atoi(newSymbol.c_str())) : new Data(1, atoi(newSymbol.c_str())));
                lc += (dir == WORD) ? 2 : 1;
                continue;
            }
        }

        break;
    case SKIP:

        reg = ("(\\d+)");
        if (regex_search(line, matches, reg))
        {
            string help = matches[1];
            int a = atoi(help.c_str());
            lc += a;
            if (dataAllowed())
                currentSection->addData(new Data(a, 0));
        }
        break;
    case END:
        if (currentSection != NULL)
            currentSection->setSize(lc);
        end = true;
        break;

    case NOTFOUND:
    {
        cout << "Neispravna direktiva" << endl;
        exit(0);
    }
    default:
        break;
    }
}

void Assem::checkIfInstruction(string line)
{
    int startIndex, index;
    if ((startIndex = line.find_first_not_of(" \t")) != string::npos)
        line = line.substr(startIndex);
    if (line.empty())
        return;
    if (line[0] == '.')
        return;

    string instruction, operands;
    if ((startIndex = line.find_first_of(" ")) != string::npos)
    {
        instruction = line.substr(0, startIndex);
        operands = line.substr(startIndex + 1, string::npos);
        operands = removeAllSpaces(operands);
    }
    else
    {
        instruction = line;
    }
    if (!executionAllowed())
    {
        cout << "GRESKA: Ne moze instrukcija u " << currentSection->getName() << " sekciji\n";
        return;
    }

    string operand1 = "";
    string operand2 = "";
    if (!operands.empty())
    {
        if ((index = operands.find_first_of(',')) != string::npos)
        {
            operand1 = operands.substr(0, index);
            operand2 = operands.substr(index + 1, string::npos);
        }
        else
        {
            operand1 = operands;
        }
    }
    Instruction instr = hashInstruction(instruction);
    Addressing am1, am2;
    am1 = hashAddressing(operand1, instr);
    bool isByteInstr = isByteInstruction(instruction);
    unsigned word = 4;

    switch (instr)
    {
    case HALT:
    case IRET:
    case RET:
        currentSection->addData(new Data(1, instr));
        lc += 1;
        break;
    case JMP:
    case JEQ:
    case JNE:
    case JGT:
    case CALL:
        if (am1 < 0 || operand2 != "")
        {
            cout << "GRESKA: PREVISE OPERANADA u instrukciji za skok na liniji: " << row << endl;
            exit(0);
            return;
        }

       // cout << "Jmp instrukcija "  << endl;
        currentSection->addData(new Data(1, instr));
        lc++;
        addOperand(operand1, false, am1, true);

        break;
    case PUSH:
    case POP:
    case NOT:
        if (am1 < 0 || operand2 != "")
        {
            cout << "GRESKA << Previse operanada u push/pop/not instrukciji ili druga greska\n";
            return;
        }
        if (isByteInstr)
            currentSection->addData(new Data(1, instr));
        else
            currentSection->addData(new Data(1, instr | word));

        lc ++;

        addOperand(operand1, isByteInstr, am1, false);
        break;

    case MOV:
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case CMP:
    case AND:
    case OR:
    case XOR:
    case XCHG:
    case SHL:
    case SHR:
    case TEST:
    {

       // cout << " mov instruction " << operand1 << " " << operand2 << endl;

        if (operand2 == "")
        {
            cout << "GRESKA: Nedovoljno operanada na liniji: " << row << endl;
            exit(0);
        }
        if (instr == SHR && (am2 == IMMED || am2 == IMMED_DEC || am2 == IMMED_HEX))
        {
            cout << "GRESKA: Ne moze IMMED za destinacioni na liniji: " << row;
            exit(0);
        }
        am2 = hashAddressing(operand2, instr);

        if (am2 == IMMED || am2 == IMMED_DEC || am2 == IMMED_HEX)
        {
            cout << "GRESKA: Ne moze IMMED za destinacioni na liniji: " << row << endl;
            exit(0);
        }

        if (isByteInstr)
            currentSection->addData(new Data(1, instr));
        else
            currentSection->addData(new Data(1, instr | word));

        lc += 1;

        Data *dt = addOperand(operand1, isByteInstr, am1, false);
        int temp = lc;

        addOperand(operand2, isByteInstr, am2, false);

        if (am1 == PCREL)
            dt->subData(lc - temp);
        break;
    }
    case ILLEGALINSTRUCTION:
    {
        cout << "Neispravna instrukcija" << endl;
        exit(0);
    }
    }
}

bool Assem::executionAllowed()
{
    if (currentSection->getRwx().find_first_of('x') != string::npos)
        return true;
    return false;
}

bool Assem::dataAllowed()
{
    if (currentSection->getRwx().find_first_of('p') != string::npos)
        return true;
    return false;
}

Addressing Assem::hashAddressing(string operand, Instruction i)
{
    regex reg;
    smatch match;

    //cout << operand << " ";

    if (i == JMP || i == JEQ || i == JNE || i == JGT || i == CALL)
    {

        reg = ("^0[xX][0-9a-fA-F]+$"); //hex
        if (regex_search(operand, match, reg))
        {
            string temp = match[0];
            int immed = stoi(temp, 0, 16);
            //   cout << "immed hex" << endl;
            if (immed >= 0 && immed <= 65535)
                return IMMED_HEX;
            else
                return UNKNOWN;
        }

        reg = "^[a-zA-Z][0-9a-zA-Z]*$";
        if (regex_search(operand, match, reg))
        {
            //   cout << " immed simbola"  << endl;
            return IMMED;
        }
        //pc rel
        reg = ("^\\*[a-zA-Z][0-9a-zA-Z]*\\((%pc|%r7)\\)$");
        if (regex_search(operand, match, reg))
        {

            //  cout << " pcrel"  << endl;
            return PCREL;
        }
        //immed
        reg = ("^-?[0-9]+$"); //decimal
        if (regex_search(operand, match, reg))
        {
            string temp = match[0];
            int immed = atoi(temp.c_str());
            // cout << "immed decimal" << endl;

            if (immed >= -32768 && immed < 32768)
            {
                return IMMED_DEC;
                //  cout << "returned iimdec";
            }
            else
            {
                return UNKNOWN;
                //  cout << "returned unk";
            }
        }
        reg = ("^0[xX][0-9a-fA-F]+$"); //hex
        if (regex_search(operand, match, reg))
        {
            string temp = match[0];
            int immed = stoi(temp, 0, 16);
            //  cout << "immed hex"<< endl;
            if (immed >= 0 && immed <= 65535)
                return IMMED_HEX;
            else
                return UNKNOWN;
        }

        //regdir
        reg = ("^(\\*%r[0-7])(l|h)?$");
        if (regex_search(operand, match, reg))
        {
            //  cout << "regdir" << endl;
            return REGDIR;
        }

        //regind
        reg = ("^\\((\\*%r[0-7])\\)$");
        if (regex_search(operand, match, reg))
        {
            //     cout << "regind"  << endl;
            return REGIND_BEZ_POMERAJA;
        }

        //memdir
        reg = ("^\\*[a-zA-Z][0-9a-zA-Z]*$");
        if (regex_search(operand, match, reg))
        {
            //  cout << "memdir"  << endl;
            return MEM_SYMB;
        }

        reg = ("^\\*[0-9]+$"); //memdir decimal
        if (regex_search(operand, match, reg))
        {
            //  cout << "memdir DEC" << endl;
            return MEM_DEC;
        }

        //regdirpom
        reg = ("^\\*[a-zA-Z][0-9a-zA-Z]*\\((%r[0-7])\\)$");
        if (regex_search(operand, match, reg))
        {
            cout << "regdirpom"  << endl;
            return REGDIRPOM;
        }
    }
    else
    {

        reg = ("^0[xX][0-9a-fA-F]+$"); //hex
        if (regex_search(operand, match, reg))
        {
            string temp = match[0];
            int immed = stoi(temp, 0, 16);
            //  cout << "immed hex" << endl;
            if (immed >= 0 && immed <= 65535)
                return IMMED_HEX;
            else
                return UNKNOWN;
        }

        reg = "^\\$[a-zA-Z][0-9a-zA-Z]*$";
        if (regex_search(operand, match, reg))
        {
            //   cout << " immed simbol"<< endl;
            return IMMED;
        }
        //pc rel
        reg = ("^[a-zA-Z][0-9a-zA-Z]*\\((%pc|%r7)\\)$");
        if (regex_search(operand, match, reg))
        {

            //    cout << " pcrel" << endl;
            return PCREL;
        }
        //immed
        reg = ("^[0-9]+$"); //decimal
        if (regex_search(operand, match, reg))
        {
            string temp = match[0];
            int immed = atoi(temp.c_str());
           // cout << "immed decimal" << endl;

            if (immed >= -32768 && immed < 32768)
                return IMMED_DEC;
            else
            {
                cout << "GRESKA: Prevelik operand na liniji:  " << row;
            }
        }

        //regdir
        reg = ("^(%r[0-7])(l|h)?$");
        if (regex_search(operand, match, reg))
        {
            //   cout << "regdir" << endl;
            return REGDIR;
        }

        //regind
        reg = ("^\\((%r[0-7])\\)$");
        if (regex_search(operand, match, reg))
        {
            //  cout << "regind"  << endl;
            return REGIND_BEZ_POMERAJA;
        }

        //memdir
        reg = ("^[a-zA-Z][0-9a-zA-Z]*$");
        if (regex_search(operand, match, reg))
        {
            //   cout << "memdir"<< endl;
            return MEM_SYMB;
        }

        reg = ("^[0-9]+$"); //memdir decimal
        if (regex_search(operand, match, reg))
        {
            //   cout << "memdir DEC" << endl;
            return MEM_DEC;
        }

        //regdirpom
        reg = ("^[a-zA-Z][0-9a-zA-Z]*\\((%r[0-7])\\)$");
        if (regex_search(operand, match, reg))
        {
            //  cout << "regdirpom" << endl;
            return REGDIRPOM;
        }
    }

    cout << " UNKNOWN" << endl;
    return UNKNOWN;
}

void Assem::setOutput(string outputName)
{

    int solved;
    list<EquSymbols *>::iterator iter;
    do
    {
        solved = 0;
        iter = (*equsymbols).begin();
        EquSymbols *temp;
        int size = (*equsymbols).size();
        cout << "velicina EQU NIZA: " << size << endl;

        for (int i = 0; i < size; i++)
        {
            temp = *iter;
            EquSymbols *es = temp->solve();
            advance(iter, 1);
            if (es != NULL)
            {
                solved++;
                equsymbols->remove(es);
            }
        }

    } while (solved > 0);

    if ((*equsymbols).size() > 0)
    {
        cout << "GRESKA: Kruzna zavisnost u equ direktivama" << endl;
        exit(0);
    }
    
    symbolList->externToBottom();

    cout << " IME | SEKCIJA | VREDNOST | VID | R.B. | atr | VELICINA" << endl;
    cout << *sections;
    cout << *symbolList << endl;

    sections->printRealocations(&cout);
    sections->printData(&cout);

    ofstream *file = new ofstream(outputName);
    *file << "#tabela simbola\n";
    *file << " IME | SEKCIJA | VREDNOST | VID | R.B. | atr | VELICINA" << endl;
    *file << *sections;
    *file << *symbolList << endl;
    sections->printRealocations(file);
    sections->printData(file);
    file->close();
}

bool Assem::isByteInstruction(string instruction)
{
    if (instruction[instruction.size() - 1] == 'b' && instruction != "sub")
        return true;
    return false;
}

string Assem::removeAllSpaces(string in)
{
    int index;
    string ret = in;
    while ((index = ret.find_first_of(" \t")) != string::npos)
    {
        ret = ret.substr(0, index).append(ret.substr(index + 1, string::npos));
    }
    return ret;
}

Directive Assem::hashDirective(string in)
{
    if (in == ".equ")
        return EQU;
    if (in == ".section")
        return SECTION;
    if (in == ".text")
        return TEXT;
    if (in == ".data")
        return DATA;
    if (in == ".bss")
        return BSS;
    if (in == ".extern")
        return EXTERN;
    if (in == ".byte")
        return BYTE;
    if (in == ".word")
        return WORD;
    if (in == ".skip")
        return SKIP;
    if (in == ".end")
        return END;
    if (in == ".global")
        return GLOBAL;
    if (in == ".rodata" || in == ".RODATA")
        return RODATA;
    return NOTFOUND;
}

Instruction Assem::hashInstruction(string in)
{
    if (in == "halt")
        return HALT;
    if (in == "xchg" || in == "xchgw" || in == "xchgb")
        return XCHG;
    if (in == "mov" || in == "movb" || in == "movw")
        return MOV;
    if (in == "add" || in == "addw" || in == "addb")
        return ADD;
    if (in == "sub" || in == "subw" || in == "subb")
        return SUB;
    if (in == "mul" || in == "mulw" || in == "mulb")
        return MUL;
    if (in == "div" || in == "divw" || in == "divb")
        return DIV;
    if (in == "cmp" || in == "cmpw" || in == "cmpb")
        return CMP;
    if (in == "not" || in == "notw" || in == "notb")
        return NOT;
    if (in == "and" || in == "andw" || in == "andb")
        return AND;
    if (in == "or" || in == "orw" || in == "orb")
        return OR;
    if (in == "xor" || in == "xorw" || in == "xorb")
        return XOR;
    if (in == "test" || in == "testw" || in == "testb")
        return TEST;
    if (in == "shl" || in == "shlw" || in == "shlb")
        return SHL;
    if (in == "shr" || in == "shrw" || in == "shrb")
        return SHR;
    if (in == "push" || in == "pushw" || in == "pushb")
        return PUSH;
    if (in == "pop" || in == "popw" || in == "popb")
        return POP;
    if (in == "jmp")
        return JMP;
    if (in == "jeq")
        return JEQ;
    if (in == "jne")
        return JNE;
    if (in == "jgt")
        return JGT;
    if (in == "call")
        return CALL;
    if (in == "ret")
        return RET;
    if (in == "iret")
        return IRET;
    return ILLEGALINSTRUCTION;
}

Symbol *Assem::checkIfSymbolExists(string name)
{
    Symbol *s = symbolList->checkIfSymbolExists(name);
    if (s != NULL)
        return s;
    s = sections->checkIfSymbolExists(name);
    return s;
}

Symbol *Assem::checkIfSectionUsed(string name)
{
    Symbol *newSection = symbolList->checkIfSymbolExists(name);
    if (newSection != NULL)
    {
        if (newSection->getDefined())
        {
            cout << "GRESKA: Korisceno ime sekcije kao promenljiva ";
            exit(0);
        }
        if (newSection->getName() == "data")
            newSection->setRwx("rwp");
        else if (newSection->getName() == "bss")
            newSection->setRwx("rw");
        else if (newSection->getName() == "text")
            newSection->setRwx("rxp");

        sections->addDefined(newSection);
        symbolList->deleteSymbol(newSection);
        newSection->setSize(0);
        newSection->setSection(newSection);
        newSection->setindex();
    }
    else
        newSection = sections->checkIfSymbolExists(name);

    if (currentSection != NULL)
        currentSection->setSize(lc);

    if (newSection != NULL)
    {
        currentSection = newSection;
        lc = currentSection->getSize();
    }
    return newSection;
}

Data *Assem::addOperand(string operand, bool byte, Addressing adr, bool jmp)
{

    int oc;
    Data *d = NULL;

    if (adr == WORD_addr || adr == BYTE_addr)
    {
        Symbol *temp = symbolList->addFromDirective(operand);
        currentSection->addRelocation(new Relocation(lc, false, temp));

        if (temp->getGle() == 'g')
        {
            d = (adr == WORD_addr) ? new Data(2, 0) : new Data(1, 0);
        }
        else if (temp->getDefined())
        {
            d = (adr == WORD_addr) ? new Data(2, temp->getValue()) : new Data(1, temp->getValue());
        }
        else
        {
            d = (adr == WORD_addr) ? new Data(2, 0) : new Data(1, 0);
            temp->addflink(new Flink(d, true));
        }

        currentSection->addData(d);

        lc = (adr == WORD_addr) ? lc + 2 : lc + 1;
        return d;
    }

    if (adr == PCREL)
    {
        oc = 7 << 1 | (3 << 5); // 7.registar
        currentSection->addData(new Data(1, oc));
        lc += 1;
        string symbolName = (jmp) ? operand.substr(1, operand.size() - 6) : operand.substr(0, operand.size() - 5);

        Symbol *temp = checkIfSymbolExists(symbolName);
        if (temp == NULL)
            temp = symbolList->addFromInstruction(symbolName);

        if (temp->getGle() == 'g')
        {
            d = currentSection->addData(new Data(2, -2));
            currentSection->addRelocation(new Relocation(lc, true, temp));
        }
        else if (temp->getDefined())
        {
            d = currentSection->addData(new Data(2, temp->getValue() - 2));
            currentSection->addRelocation(new Relocation(lc, true, temp));
        }
        else
        {
            d = new Data(2, -2);
            currentSection->addData(d);
            currentSection->addRelocation(new Relocation(lc, true, temp));
            temp->addflink(new Flink(d, true));
        }
        lc += 2;

        return d;
    }

    if (adr == IMMED_DEC)
    {
        // cout << "IMMED_DEC";

        int i = atoi(operand.c_str());
        currentSection->addData(new Data(1, 0));
        lc++;

        if (byte)
        {
            if (i <= -128 || i >= 127)
            {
                cout << "GRESKA: Prevelik operand za byte instrukciju na liniji: " << row;
                exit(0);
            }
            d = currentSection->addData(new Data(1, i));
            lc++;
        }
        else
        {
            d = currentSection->addData(new Data(2, i));
            lc += 2;
        }
        return d;
    }
    if (adr == IMMED_HEX)
    {

        int i = stoi(operand, 0, 16);
        d = currentSection->addData(new Data(1, 0));
        lc++;
        if (byte)
        {
            if (i < 0 || i > 15)
            {
                cout << "GRESKA: Prevelik operand za byte instrukciju na liniji: " << row;
                exit(0);
            }

            d = currentSection->addData(new Data(1, i));
            lc++;
        }
        else
        {
            d = currentSection->addData(new Data(2, i));
            lc += 2;
        }
        return d;
    }

    if (adr == IMMED)
    {

        currentSection->addData(new Data(1, 0));
        lc++;

        string symbolName = (jmp) ? operand : operand.substr(1);

        Symbol *temp = checkIfSymbolExists(symbolName);
        if (temp == NULL)
            temp = symbolList->addFromInstruction(symbolName);

        if (temp->getGle() == 'g')
        {
            d = new Data(2, 0);
        }
        else if (temp->getDefined())
        {
            d = new Data(2, temp->getValue());
        }
        else
        {
            d = new Data(2, 0);
            temp->addflink(new Flink(d, true));
        }

        currentSection->addData(d);
        currentSection->addRelocation(new Relocation(lc, false, temp));
        lc += 2;
        return d;
    }

    if (adr == REGDIR)
    {
        //cout << "Registarsko direkno" << endl;
        int rNum;
        oc = 0;
        if (jmp)
        {
            rNum = operand[3] - '0';
            if (operand.size() == 5)
            {
                if (!byte)
                {
                    cout << "GRESKA: nije u pitanju byte instrukcija: " << lc << " sekcija: " << currentSection->getName() << endl;
                    exit(0);
                }
                if (operand[4] == 'h')
                    oc = 1;
            }
        }
        else
        {
            rNum = operand[2] - '0';
            if (operand.size() == 4)
            {
                if (!byte)
                {
                    cout << "GRESKA: nije u pitanju byte instrukcija: " << lc << " sekcija: " << currentSection->getName() << endl;
                    exit(0);
                }
                if (operand[3] == 'h')
                    oc = 1;
            }
        }
        oc = oc | (1 << 5) | (rNum << 1);
        d = currentSection->addData(new Data(1, oc));
        lc++;
        return d;
    }

    if (adr == REGIND_BEZ_POMERAJA)
    {

        oc = (2 << 5) | (operand[operand.size() - 2] - '0' << 1);
        currentSection->addData(new Data(1, oc));
        lc++;
        return NULL;
    }

    if (adr == REGIND_16_NUM)
    {
        oc = (operand[operand.size() - 2] - '0' << 1) | (3 << 5);
        string temp = (jmp) ? operand.substr(1, operand.size() - 6) : operand.substr(0, operand.size() - 6);
        // cout << temp << " regind num" << endl;

        int offset = atoi(temp.c_str());
        currentSection->addData(new Data(1, oc));
        lc++;
        d = currentSection->addData(new Data(2, offset));
        lc += 2;
        return d;
    }

    if (adr == REGIND_16_SYMB)
    {

        oc = operand[operand.size() - 2] - '0' << 1 | (3 << 5);
        currentSection->addData(new Data(1, oc));
        lc++;
        string symbolName = (jmp) ? operand.substr(1, operand.size() - 6) : operand.substr(0, operand.size() - 6);

        Symbol *temp = checkIfSymbolExists(symbolName);
        if (temp == NULL)
            temp = symbolList->addFromInstruction(symbolName);
        if (temp->getGle() == 'g')
        {
            d = new Data(2, 0);
        }
        else if (temp->getDefined())
        {
            d = new Data(2, temp->getValue());
        }
        else
        {
            d = new Data(2, 0);
            temp->addflink(new Flink(d, true));
        }

        d = currentSection->addData(d);
        currentSection->addRelocation(new Relocation(lc, false, temp));
        lc += 2;
        return d;
    }

    if (adr == MEM_SYMB)
    {
        oc = 4 << 5;
        currentSection->addData(new Data(1, oc));
        lc += 1;
        string symbolName = (jmp) ? operand.substr(1) : operand;

        Symbol *temp = checkIfSymbolExists(symbolName);
        if (temp == NULL)
            temp = symbolList->addFromInstruction(symbolName);

        if (temp->getGle() == 'g')
        {
            d = new Data(2, 0);
        }
        else if (temp->getDefined())
        {
            d = new Data(2, temp->getValue());
        }
        else
        {
            d = new Data(2, 0);
            temp->addflink(new Flink(d, true));
        }

        currentSection->addData(d);
        currentSection->addRelocation(new Relocation(lc, false, temp));
        lc += 2;
        return d;
    }

    if (adr == MEM_DEC)
    {
        oc = 4 << 5;
        currentSection->addData(new Data(1, oc));
        lc += 1;

        string symbolName = (jmp) ? operand.substr(1) : operand;
        d = currentSection->addData(new Data(2, atoi(symbolName.c_str())));
        lc += 2;
        return d;
    }
    return NULL;
}