#include <iostream>
#include <thread>
#include <fstream>

#include "processor/processor.h"
#include "load_program.hpp"
#include "graphics_device.hpp"

using namespace jpl;


int main(int argc, char **args)
{	
	glutInit(&argc, args);
	
	GTX1050 gtx;
	
	local::DCTR["GDO"] = {gtx.get_operation(), nullptr}; // в файле load_program.hpp содержится список лишь базовых команд. В этой строке мы указываем, что "GDO" необходимо интерпретировать как соответсвующую операцию для контроллера. При этом оператора для АЛУ нет (так как контроллер сам исполнит эту команду)
	local::DCTR["KBD"] = {gtx.get_keyboard().get_operation(), nullptr};
	
	progmem prog;	
	{
		std::ifstream in("towers.jp");	
		read_program(in, prog); // чтение программы из файла
		in.close();  
	}
	

	RAM ram;	
	
	// настраеваем вычислительные машины процессора. Три АЛУ, один может выполнять арифметические дейсвия за 5 тактов, другой делает инкремент за 1 такт и третий выполняет логические операции за 3 такта 
	std::vector<const operator_*> alu1_ops;
	alu1_ops.push_back(&operators::ADD);
	alu1_ops.push_back(&operators::SUB);
	alu1_ops.push_back(&operators::MUL);
	alu1_ops.push_back(&operators::DIV);
	alu1_ops.push_back(&operators::MOD);
	alu1_ops.push_back(&operators::INV);
	alu1_ops.push_back(&operators::NEG);
	
	
	std::vector<const operator_*> alu1_ops2;
	
	alu1_ops2.push_back(&operators::AND);
	alu1_ops2.push_back(&operators::OR);
	alu1_ops2.push_back(&operators::XOR);
	alu1_ops2.push_back(&operators::NOT);
	alu1_ops2.push_back(&operators::EQL);
	alu1_ops2.push_back(&operators::NEQL);
	alu1_ops2.push_back(&operators::NEG);
	
	std::vector<const operator_*> alu1_ops1;
	
	alu1_ops1.push_back(&operators::INC);
	alu1_ops1.push_back(&operators::DEC);
	
	
	
	processor proc(ALU(0,alu1_ops), 10, &ram); // у процессора будет десять регистров, так же передаём указатель на ОП
	
	proc.add_alu(ALU(0,alu1_ops1));	
	proc.add_alu(ALU(0,alu1_ops2));
	
	proc.add_alu(ALU(0,alu1_ops));
	proc.add_alu(ALU(0,alu1_ops1));	
	proc.add_alu(ALU(0,alu1_ops2));
		
	std::cout << "code:" << std::endl << "------------------------------------" << std::endl << prog << std::endl; // выводится код программы на моём языке)
	
	proc.run(prog);
	std::cout << "------------------------------------" << std::endl << "RAM image:" << std::endl << ram; // выводится состояние оперативной памяти после выполнения программы
	
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	
	return 0;
}