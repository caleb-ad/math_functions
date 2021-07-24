ifeq ($(MAKECMDGOALS), func_interp.obj)
SHELL = "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Visual Studio 2019\Visual Studio Tools\Developer Command Prompt for VS 2019.lnk"
endif

ifeq ($(MAKECMDGOALS), func_interp.lib)
SHELL = "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Visual Studio 2019\Visual Studio Tools\Developer Command Prompt for VS 2019.lnk"
endif

tests.exe: FuncInterp.o FuncInterp_test.cpp
	g++ -o tests.exe FuncInterp_test.cpp FuncInterp.o

FuncInterp.o: FuncInterp.cpp FuncInterp.hpp func_except.hpp
	g++ -Wall -Werror -o FuncInterp.o -c FuncInterp.cpp

func_cmd.exe: FuncInterp.cpp FuncInterp.hpp CMDtool.cpp func_except.hpp
	g++ -Wall -Werror -o func_cmd.exe FuncInterp.cpp CMDtool.cpp

clean:
	rm *.o; rm *.exe; rm *.obj

FuncInterp.obj: FuncInterp.cpp FuncInterp.hpp func_except.hpp
	cl -W3 -EHsc -c -Fo"FuncInterp.obj" FuncInterp.cpp

FuncInterp.lib: func_interp.obj
	lib FuncInterp.obj

