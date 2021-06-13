tests: FuncInterp.o FuncInterp_test.cpp
	g++ -o tests.exe FuncInterp_test.cpp FuncInterp.o

FuncInterp.o: FuncInterp.cpp FuncInterp.hpp
	g++ -Wall -Werror -o FuncInterp.o -c FuncInterp.cpp

main: FuncInterp.cpp FuncInterp.hpp main.cpp
	g++ -Wall -Werror -o func_cmd.exe FuncInterp.cpp main.cpp