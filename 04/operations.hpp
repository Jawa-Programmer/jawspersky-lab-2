#ifndef JPL_OPERATIONS_H
#define JPL_OPERATIONS_H

#include "types.h"
#include "operators.hpp"

#include <iostream>


namespace jpl {
	namespace operations {
		class base {
				public:
				/// возвращает тип оператора. Временный костыль для текстого представления комманды
				virtual OPERATION_TYPE type() const = 0;
				/// сообщает ИУ, стоит ли передать команду в АЛУ или исполнить ее самому
				virtual EXECUTOR executor() const = 0;
				/// класс является функтором, оператор выполняет команду на конкретном АЛУ (или сразу на контроллере). В случае успеха, счётчик в памяти программы будет переведен
				virtual void operator()(processor &proc, const command& cmd, long time) const = 0;
		}; 
		
		const class : public base {
				public:
				virtual OPERATION_TYPE type() const override {return OP_ALLC;}
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1){
						std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						byte adr = proc.get_RAM() -> alloc(1, &prog);  // выделяем память размером в 1 байт. Указатель помещаем в аргумент
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
					}
					else if(argc == 2){
						std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						byte adr = proc.get_RAM() -> alloc(proc.get_value(*cmd.get_operand(1)), &prog); // выделяем память размером в переданное число вторым аргументом число. Указатель помещаем в первый аргумент
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
					}
					else throw std::logic_error("ALLC must have one or two args");
			}				
		} ALLC;
		
		const class : public base {
				public:
				virtual OPERATION_TYPE type() const override {return OP_FREE;}
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1){
							std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
							proc.get_RAM() -> free(proc.get_value(*cmd.get_operand(0)), &prog);  // освобождение динамически выделенной памяти
							prog.inc();
					}
					else throw std::logic_error("FREE must have only one");
			}				
		} FREE;
		
		const class : public base 
		{
			public:
				virtual OPERATION_TYPE type() const override {return OP_JUMP;}
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					
					int cc = prog.count();
					bool is_e = false;
					if(argc == 1){						
						is_e = true;
						prog.jump(proc.get_value(*cmd.get_operand(0)));
					}
					else if(argc == 2){
						is_e = true;
						if(proc.get_value(*cmd.get_operand(0)))
							prog.jump(proc.get_value(*cmd.get_operand(1)));
						else
							prog.inc();
						
					}
					else{
						is_e = true;
						if(proc.get_value(*cmd.get_operand(0)))
							prog.jump(proc.get_value(*cmd.get_operand(1)));
						else
							prog.jump(proc.get_value(*cmd.get_operand(2)));
					}
					if(is_e)
						std::cout << "(" << time << "t)\t[0x" << std::hex << cc << std::dec << "]\t" << cmd << std::endl;
				}
		} JUMP;
		const class : public base 
		{
			public:
				virtual OPERATION_TYPE type() const override {return OP_VAR;}
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					std::cout << "(" << time << "t)\t[0x" << std::hex << proc.get_process().count() << std::dec << "]\t" << cmd << std::endl;
					proc.get_process().inc();
				}
		} VAR;
		
		const class : public base 
		{
			public:
				virtual OPERATION_TYPE type() const override {return OP_NONE;}
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					throw std::logic_error("executed none operation");
				}
		} NONE;
		
		const class : public base 
		{
			public:
				virtual OPERATION_TYPE type() const override {return OP_UNARY;}
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0))});			
				}
		} UNARY;
		
		const class : public base 
		{
			public:
				virtual OPERATION_TYPE type() const override {return OP_BINARY;}
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0)), proc.get_value(*cmd.get_operand(1))});					
				}
		} BINARY;
	}
}

#endif