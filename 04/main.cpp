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
	
	progmem prog, prog2;	
	{
		std::ifstream in("tictactoe.jp");	
		read_program(in, prog); // чтение программы из файла
		in.close();  
	}
	{
		std::ifstream in("a.jp");	
		read_program(in, prog2); // чтение программы из файла
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
	
	
	std::vector<const operator_*> alu1_ops2;
	
	alu1_ops2.push_back(&operators::AND);
	alu1_ops2.push_back(&operators::OR);
	alu1_ops2.push_back(&operators::XOR);
	alu1_ops2.push_back(&operators::NOT);
	alu1_ops2.push_back(&operators::EQL);
	alu1_ops2.push_back(&operators::NEQL);
	
	std::vector<const operator_*> alu1_ops1;
	
	alu1_ops1.push_back(&operators::INC);
	alu1_ops1.push_back(&operators::DEC);
	
	
	
	processor proc(ALU(5,alu1_ops), 10, &ram); // у процессора будет десять регистров, так же передаём указатель на ОП
	processor proc2(ALU(6,alu1_ops), 10, &ram); // у процессора будет десять регистров, так же передаём указатель на ОП
	
	proc.add_alu(ALU(1,alu1_ops1));
	
	proc.add_alu(ALU(3,alu1_ops2));
	
	proc2.add_alu(ALU(2,alu1_ops1));
	
	proc2.add_alu(ALU(4,alu1_ops2));
		
	std::cout << "code:" << std::endl << "------------------------------------" << std::endl << prog << std::endl; // выводится код программы на моём языке)
	
	std::thread thr(processor::run, std::ref(proc), std::ref(prog));
	std::thread thr2(processor::run, std::ref(proc2), std::ref(prog2));
	
	thr.join();
	thr2.join();	
	std::cout << "------------------------------------" << std::endl << "RAM image:" << std::endl << ram; // выводится состояние оперативной памяти после выполнения программы
	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	return 0;
}