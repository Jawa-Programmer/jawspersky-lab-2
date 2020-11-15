#ifndef LOAD_FROM_FILE_H
#define LOAD_FROM_FILE_H
#include "processor.h"
#include <map>
#include <iterator>
#include <sstream>

namespace jpl
{
	namespace local
	{
		struct operation {OPERATION_TYPE tp; const operator_ *op;};
		
		std::map<std::string, operation> DCTR = 
		{
			{"INIT", {OP_INIT, nullptr}},
			{"JUMP", {OP_JUMP, nullptr}},
			
			{"ADD", {OP_BINARY, &operators::ADD}},
			{"SUB", {OP_BINARY, &operators::SUB}},
			{"MUL", {OP_BINARY, &operators::MUL}},
			{"DIV", {OP_BINARY, &operators::DIV}},
			{"MOD", {OP_BINARY, &operators::MOD}},
			{"INC", {OP_UNARY, &operators::INC}},
			{"DEC", {OP_UNARY, &operators::DEC}},
			{"SET", {OP_BINARY, &operators::SET}},
			{"INV", {OP_UNARY, &operators::INV}},
			{"AND", {OP_BINARY, &operators::AND}},
			{"OR", {OP_BINARY, &operators::OR}},
			{"XOR", {OP_BINARY, &operators::XOR}},
			{"NOT", {OP_UNARY, &operators::NOT}},
		};
		
		operand* op_from_string(const std::string& str)
		{
			if(str[0] == '('){return new reg_operand(*op_from_string(str.substr(1)));}
			else if(str[0] == '['){return new ram_operand(*op_from_string(str.substr(1)));}
			else if(isdigit(str[0])){return new const_operand(std::stoi(str));}
			else{return new var_operand(str);}
		}
		
		command cmd_from_string(const std::string& str)
		{
			std::vector<std::string> line; 
			std::istringstream iss(str);
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(line));
			operation oper = DCTR[line[0]];
			
			if(oper.tp == OP_INIT)
			{
				return command(oper.tp, &line[1][0]);;
			}
			
			
			std::vector<operand*> args;
			for(int i = 1; i < line.size(); ++i)
				args.push_back(op_from_string(line[i]));
			
			command cmd;
			if(args.size() == 0)
				cmd = command(oper.tp, nullptr, oper.op);
			else if(args.size() == 1)
				cmd = command(oper.tp, nullptr, *args[0], oper.op);
			else if(args.size() == 2)
				cmd = command(oper.tp, nullptr, *args[0], *args[1], oper.op);
			else if(args.size() == 3)
				cmd = command(oper.tp, nullptr, *args[0], *args[1], *args[2], oper.op);
			
			while(!args.empty()){
				delete args[args.size()-1];
				args.pop_back();
			}
			return cmd;
		}		
	}
	void read_program(std::istream &in, progmem &prog)
	{
		std::string line;
		while(getline(in, line))
		{
			if(line[0] == '#' || line.length() == 0) continue;
			command cmd = local::cmd_from_string(line);
			prog.insert(cmd);
			std::cout << "command = " << cmd << std::endl;
		}
	}
}

#endif