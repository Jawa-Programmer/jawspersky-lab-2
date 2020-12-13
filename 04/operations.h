#ifndef JPL_OPERATIONS_H
#define JPL_OPERATIONS_H

#include <vector>
#include <iostream>

#include "types.h"
#include "operators.hpp"
#include "operands.h"

namespace jpl {
	namespace operations {
		class base {
				protected:
				/// вектор используемых регистров процессоров. Используется, например, в функциях ввода/вывода для передачи параметров через регистры
				std::vector<int> req;
				public:
				/// возвращает тип операции. используется для внутреннего пользования. При определении производных операций, эту функцию переопределять не рекомендуется
				virtual OPERATION_TYPE type() const {return OP_EXTER;}
				/// сообщает ИУ, стоит ли передать команду в АЛУ или исполнить ее самому
				virtual EXECUTOR executor() const = 0;
				/// если executer вернул E_CONTROLLER, то данный метод возвращает строковое название операции. В противном случае, указатель на пустоту
				virtual const char* lab() const = 0;
				/// класс является функтором, оператор выполняет команду на конкретном АЛУ (или сразу на контроллере). В случае успеха, счётчик в памяти программы будет переведен
				virtual void operator()(processor &proc, const command& cmd, long time) const = 0;
				const std::vector<int>& get_requirements() const {return req;}
		}; 
		
		const class : public base {
				public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual const char* lab() const override {return "ALLC";}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1){
					//	std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						RAM *ram = proc.get_RAM();
						ram->lock();
						byte adr = ram -> alloc(1, &prog);  // выделяем память размером в 1 байт. Указатель помещаем в аргумент
						ram->unlock();
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
					}
					else if(argc == 2){
					//	std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						RAM *ram = proc.get_RAM();
						ram->lock();
						byte adr = ram -> alloc(proc.get_value(*cmd.get_operand(1)), &prog); // выделяем память размером в переданное число вторым аргументом число. Указатель помещаем в первый аргумент
						ram->unlock();
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
					}
					else throw std::logic_error("ALLC must have one or two args");
			}				
		} ALLC;
		
		const class : public base {
				public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1){
					//		std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
							RAM *ram = proc.get_RAM();
							ram->lock();
							ram -> free(proc.get_value(*cmd.get_operand(0)), &prog);  // освобождение динамически выделенной памяти
							ram->unlock();
							prog.inc();
					}
					else throw std::logic_error("FREE must have only one");
			}		
				virtual const char* lab() const override {return "FREE";}		
		} FREE;
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					int cc = prog.count();
					if(argc == 1){		
						prog.jump(proc.get_value(*cmd.get_operand(0)));
					}
					else if(argc == 2){
						if(proc.get_value(*cmd.get_operand(0)))
							prog.jump(proc.get_value(*cmd.get_operand(1)));
						else
							prog.inc();
						
					}
					else{
						if(proc.get_value(*cmd.get_operand(0)))
							prog.jump(proc.get_value(*cmd.get_operand(1)));
						else
							prog.jump(proc.get_value(*cmd.get_operand(2)));
					}
				}
				virtual const char* lab() const override {return "JUMP";}	
		} JUMP;
		/// помещает переданный параметром аргумент на стек процесса
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.push(proc.get_value(*cmd.get_operand(0)));			
					proc.get_process().inc();					
				}
				virtual const char* lab() const override {return "PUSH";}	
		} PUSH;
		/// изимает данные с вершины стека процесса и помещает в переданный аргумент
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					proc.get_slot(*cmd.get_operand(0)).data = prog.pop();	
					proc.get_process().inc();				
				}
				virtual const char* lab() const override {return "POP";}	
		} POP;
		
		/// помещает состояние всех регистров блока регистров процессора на вершину стека процесса (первый регистр в помещается первым)
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					int cc = proc.reg_count();
					for(int i = 0; i < cc; ++i)
						prog.push(proc.read_reg(i).data);	
					prog.inc();			
					
				}
				virtual const char* lab() const override {return "PUSHR";}	
		} PUSHR;
		
		
		/// помещает в первый операнд содержимое стека с отступом от вершины, равным второму операнду (по умолчанию 0)
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1)
						proc.get_slot(*cmd.get_operand(0)).data = prog.stack_i(0);
					else
						proc.get_slot(*cmd.get_operand(0)).data = prog.stack_i(proc.get_value(*cmd.get_operand(1)));	
					prog.inc();				
				}
				virtual const char* lab() const override {return "PEEK";}	
		} PEEK;
		
		/// переносит значения с вершины стека процесса в регисты процессора (так, что вершина стека станет последним регистром)
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					for(int i = proc.reg_count() - 1; i >=0 ; --i)
						proc.reg_access(i).data = prog.pop();
					proc.get_process().inc();			
				}
				virtual const char* lab() const override {return "POPR";}	
		} POPR;
		/// выполняет переход по указанной метке, при этом на вершину стека процесса помещается адрес возврата
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.push(prog.count()+1);
					prog.jump(proc.get_value(*cmd.get_operand(0)));
				}
				virtual const char* lab() const override {return "CALL";}	
		} CALL;
		
		/// выполняет переход к команде, адрес которой лежит на стеке процесса. Необязательный аргумент говорит, сколько позиций необходимо снять со стека (например, если стек использовался для передачи аргументов)
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.jump(prog.pop());					
					if(argc == 1)
					{
						int mx = proc.get_value(*cmd.get_operand(0));
						for(int i=0; i < mx; ++i)
							prog.pop();
					}
				}
				virtual const char* lab() const override {return "RET";}	
		} RET;
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual OPERATION_TYPE type() const {return OP_VAR;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
				//	std::cout << "(" << time << "t)\t[0x" << std::hex << proc.get_process().count() << std::dec << "]\t" << cmd << std::endl;
					proc.get_process().inc();
				}
				virtual const char* lab() const override {return "VAR";}	
		} VAR;
				
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0))});			
				}
				virtual const char* lab() const override {return nullptr;}	
		} UNARY;
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0)), proc.get_value(*cmd.get_operand(1))});					
				}
				virtual const char* lab() const override {return nullptr;}	
		} BINARY;
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					proc.get_slot(*cmd.get_operand(0)).data = proc.get_value(*cmd.get_operand(1));		
					proc.get_process().inc();
				}
				virtual const char* lab() const override {return "SET";}	
		} SET;
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					throw std::logic_error("executed none operation");
				}
				virtual const char* lab() const override {return "NONE";}	
		} NONE;
	}
}

#endif