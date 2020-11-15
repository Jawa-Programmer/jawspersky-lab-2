#include <iostream>
#include "processor.h"

using namespace jpl;

int main()
{
	progmem prog;
	prog.insert(command(OP_INIT, "a", const_operand(1)));	
	prog.insert(command(OP_INIT, "b", const_operand(2)));
	prog.insert(command(OP_INIT, "c", const_operand(4)));	
	prog.insert(command(OP_INIT, "d", const_operand(3)));
	prog.insert(command(OP_BINARY, nullptr, var_operand("a"), var_operand("b"), &operators::ADD));
	prog.insert(command(OP_BINARY, nullptr, var_operand("c"), var_operand("d"), &operators::SUB));
	
	
	RAM ram;
	
	std::vector<const operator_*> alu1_ops;
	alu1_ops.push_back(&operators::ADD);
	alu1_ops.push_back(&operators::SUB);
	
	processor proc(ALU(4,alu1_ops), 10, &ram);
	
	proc.add_alu(ALU(3,alu1_ops));
		
	proc.load_program(&prog);
	
	/*auto end = proc.al_end();
	for(auto al = proc.al_begin(); al != end; ++al)
		std::cout << "(-)" << std::endl;*/
	
	std::cout << prog << std::endl;
	
	
	proc.run();	
	
	std::cout << ram[0].data << std::endl;
	std::cout << ram[1].data << std::endl;
	std::cout << ram[2].data << std::endl;
	std::cout << ram[3].data << std::endl;
	return 0;
}