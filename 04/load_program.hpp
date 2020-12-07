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
		std::unordered_set<std::string> labels;
		
		struct operation_ {const operation *tp; const operator_ *op;};
		
		std::map<std::string, operation_> DCTR = 
		{
			{"ALLC", {&operations::ALLC, nullptr}},
			{"VAR", {&operations::VAR, nullptr}},
			{"JUMP", {&operations::JUMP, nullptr}},
			{"FREE", {&operations::FREE, nullptr}},
			
			{"ADD", {&operations::BINARY, &operators::ADD}},
			{"SUB", {&operations::BINARY, &operators::SUB}},
			{"MUL", {&operations::BINARY, &operators::MUL}},
			{"DIV", {&operations::BINARY, &operators::DIV}},
			{"MOD", {&operations::BINARY, &operators::MOD}},
			{"INC", {&operations::UNARY, &operators::INC}},
			{"DEC", {&operations::UNARY, &operators::DEC}},
			{"SET", {&operations::BINARY, &operators::SET}},
			{"INV", {&operations::UNARY, &operators::INV}},
			{"AND", {&operations::BINARY, &operators::AND}},
			{"OR", {&operations::BINARY, &operators::OR}},
			{"XOR", {&operations::BINARY, &operators::XOR}},
			{"NOT", {&operations::UNARY, &operators::NOT}}
		};
		
		operand* op_from_string(const std::string& str)
		{		
			if(str[0] == '('){return new reg_operand(*op_from_string(str.substr(1)));}
			else if(str[0] == '['){return new ram_operand(*op_from_string(str.substr(1)));}
			else if(str[0] == '*'){return new label_operand(str.substr(1), 0, true);}
			else if(isdigit(str[0])){return new const_operand(std::stoi(str));}
			else{
				auto plus = str.find('+');
				std::string label_ = str;
				int of_ = 0;
				if(plus != std::string::npos){
					label_ = str.substr(0, plus);
					of_ = std::stoi(str.substr(plus+1));
				}				
				if(labels.find(label_) == labels.end())
					return new ram_operand(label_operand(label_, of_));
				else
					return new label_operand(label_, of_);
			}
		}
		
		command cmd_from_string(const std::vector<std::string> line)
		{
			operation_ oper = DCTR[line[0]];
			
			if(oper.tp == &operations::VAR)
			{	
				if(line.size() > 2 )
					return command(*oper.tp, nullptr, {new label_operand(line[1]), new const_operand(std::stoi(line[2]))});					
				else
					return command(*oper.tp, nullptr, {new label_operand(line[1])});
			}
			
			std::vector<operand*> args;
			for(int i = 1; i < line.size(); ++i)
				args.push_back(op_from_string(line[i]));
			
			command cmd;
			if(args.size() == 0)
				cmd = command(*oper.tp, nullptr, oper.op);
			else if(args.size() == 1)
				cmd = command(*oper.tp, nullptr, {args[0]}, oper.op);
			else if(args.size() == 2)
				cmd = command(*oper.tp, nullptr, {args[0], args[1]}, oper.op);
			else if(args.size() == 3)
				cmd = command(*oper.tp, nullptr, {args[0], args[1], args[2]}, oper.op);
			
			args.clear();
			return cmd;
		}		
	}
	/// функция стирает програму по ссылке и записывает на ее место из указанного потока новую,
	void read_program(std::istream &in, progmem &prog)
	{
		prog.clear();
		local::labels.clear();
		local::labels.insert("end"); // указатель по-умолчанию. По "стандарту" моего языка, "end" является меткой конца программы. То есть JUMP end значит закончить исполнение программы немедленно.
		
		std::stringstream iost; // костыль - мы делаем копию исходного кода программы, так как в общем поиск в потоке не возможен, но для правильной интерпретации меток необходимо делать два захода на чтение кода - в первый заход читаются только метки, во второй заход читается весь код.		
		{
			std::string line;
			int i = 0;
			while(getline(in, line))
			{
				iost << line << std::endl;
				auto fnd = line.find(":");
				if(fnd != std::string::npos)
					local::labels.insert(line.substr(0, fnd));
				i++;
			}			
		}
		
		
		std::string line;
		while(getline(iost, line))
		{
			line = line.substr(0, line.find(';')); // позволит писать комментарий с середины строки
			if(line.length() == 0) continue;
			
			std::vector<std::string> line_n; 
			std::istringstream iss(line);
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(line_n));
			
			if(line_n.size() == 0) continue;
			
			std::string lab;
			
			if(line_n[0][line_n[0].size()-1] == ':')
			{
				lab = line_n[0].substr(0, line_n[0].size()-1);
				prog.set_name(lab, prog.size());
				line_n.erase(line_n.begin());
				if(line_n.size() == 0) continue;
			}	
			command cmd = local::cmd_from_string(line_n);
			cmd.set_label(lab);
			prog.insert(cmd);
			//std::cout << "command = " << cmd << std::endl;
		}
		prog.set_name("end", prog.size());
		local::labels.clear();
	}
}

#endif