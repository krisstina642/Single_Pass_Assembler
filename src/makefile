OBJS = assembler.cpp main.cpp datatable.cpp symbol.cpp symboltable.cpp relocation.cpp relocationtable.cpp data.cpp

prog: $(OBJS)
	g++ -std=c++11 -gdwarf-2 $(OBJS) -o assembler