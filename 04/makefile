all: main.cpp processor/processor.cpp processor/operands.cpp processor/memory.cpp 
	cls || clear
	g++ main.cpp processor/operands.cpp processor/memory.cpp processor/processor.cpp  -o main.exe -lopengl32 -lglu32 -lfreeglut
	
	
test: test.cpp processor/processor.cpp processor/operands.cpp processor/memory.cpp 
	g++ test.cpp processor/processor.cpp processor/operands.cpp processor/memory.cpp  -o test.exe
	
memtest: memtest.cpp  processor/memory.cpp 
	g++ memtest.cpp processor/memory.cpp  -o memtest.exe