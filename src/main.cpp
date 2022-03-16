#include <string.h>
#include <list>
#include "assembler.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    string inputFileName;
    string outputFileName;
   if (argc == 4 && strcmp(argv[1],"-o")==0) {
            outputFileName = argv[2];
            inputFileName = argv[3];
    } else {
        cout << "GRESKA: Nisu dobri argumenti" << endl;
        return 0;
    }
        
    Assem* a =new Assem(inputFileName, outputFileName);
    a->start();
    delete a;

    return 0;

}