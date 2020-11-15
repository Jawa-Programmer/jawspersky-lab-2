#include <iostream>
#include "processor.h"
#include "load_program.hpp"

#include <fstream>

using namespace jpl;


int main()
{
	progmem prog;
	
	std::ifstream in("a.jp");
	
	read_program(in, prog); // чтение программы из файла
	
	in.close();  
	
	RAM ram;
	// настраеваем вычислительные машины процессора. Три АЛУ, один может выполнять арифметические дейсвия за 5 тактов, другой делает инкремент за 1 такт и третий выполняет логические операции за 3 такта 
	std::vector<const operator_*> alu1_ops;
	alu1_ops.push_back(&operators::ADD);
	alu1_ops.push_back(&operators::SUB);
	alu1_ops.push_back(&operators::MUL);
	alu1_ops.push_back(&operators::DIV);
	alu1_ops.push_back(&operators::INV);
	alu1_ops.push_back(&operators::SET);
	
	
	std::vector<const operator_*> alu1_ops2;
	
	alu1_ops2.push_back(&operators::AND);
	alu1_ops2.push_back(&operators::OR);
	alu1_ops2.push_back(&operators::XOR);
	alu1_ops2.push_back(&operators::NOT);
	alu1_ops2.push_back(&operators::SET);
	
	std::vector<const operator_*> alu1_ops1;
	
	alu1_ops1.push_back(&operators::INC);
	alu1_ops1.push_back(&operators::DEC);
	alu1_ops1.push_back(&operators::SET);
	
	processor proc(ALU(5,alu1_ops), 10, &ram); // у процессора будет десять регистров, так же передаём указатель на ОП
	
	proc.add_alu(ALU(1,alu1_ops1));
	
	proc.add_alu(ALU(3,alu1_ops2));
		
	proc.load_program(&prog);
	
	std::cout << "code:" << std::endl << "------------------------------------" << std::endl << prog << std::endl; // выводится код программы на моём языке)
	std::cout << "exec log:" << std::endl << "------------------------------------" << std::endl;
	proc.run();	
	
	std::cout << "------------------------------------" << std::endl << "RAM image:" << std::endl << ram; // выводится состояние оперативной памяти после выполнения программы
	
	return 0;
}