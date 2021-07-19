tests.exe: FuncInterp.o FuncInterp_test.cpp
	g++ -o tests.exe FuncInterp_test.cpp FuncInterp.o

FuncInterp.o: FuncInterp.cpp FuncInterp.hpp func_except.hpp
	g++ -Wall -Werror -o FuncInterp.o -c FuncInterp.cpp

func_cmd.exe: FuncInterp.cpp FuncInterp.hpp main.cpp func_except.hpp
	g++ -Wall -Werror -o func_cmd.exe FuncInterp.cpp main.cpp

clean:
	rm *.o
