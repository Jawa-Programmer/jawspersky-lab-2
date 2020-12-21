#ifndef JPL_OPERATIONS_H
#define JPL_OPERATIONS_H

#include <vector>
#include <iostream>

#include "types.h"
#include "operators.hpp"
#include "operands.h"

/*!
	@file
	@brief Файл содержит описание базовых операций, на основе которых можно создавать команды.
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

namespace jpl {
	/// это пространстро имён содержит глобальные константы, определяющие базовые операции 
	namespace operations {
		
		namespace
		{
			enum {STATE_FREE, STATE_WRITE, STATE_READ} _state = STATE_FREE;
			std::mutex in_mx; // защищает от попыток менять _state из разных потоков одновременно
			/// вспомогательный метод, используемый для простой блокировки ОП (так как операнды могут передаватсья по ссылке, блокировка на запись устанавливается при наличии хотя бы одного операнда, но вот чтение блокируется только при необходимости что-то записать)
			void lock_ram_by_operands(const command& cmd, processor &proc)
			{
				in_mx.lock();
				_state = STATE_FREE;
				int op_c = cmd.get_args_count();
				if(op_c)
				{
					if(cmd.get_operand(0)->type() == OPR_RAM){ proc.get_RAM()->lock_write(); _state = STATE_WRITE;}
					else{
						proc.get_RAM()->lock_read();
						_state = STATE_READ;
					}
				}
			}
			/// вспомогательный метод для снятия блокировки с ОП
			void unlock_ram(processor &proc){
					if(_state == STATE_READ)
						proc.get_RAM()->unlock_read();
					else if(_state == STATE_WRITE)
						proc.get_RAM()->unlock_write();
					_state = STATE_FREE;
				in_mx.unlock();
				}
		}
		/// базовый класс операции. Определяет поведение контроллера. 
		class base {
				public:
				/// возвращает тип операции. используется для внутреннего пользования. При определении производных операций, эту функцию переопределять не рекомендуется
				virtual OPERATION_TYPE type() const {return OP_EXTER;}
				/// сообщает ИУ, стоит ли передать команду в АЛУ или исполнить ее самому. При определении собственных операций, использущих АЛУ, учитывайте, что если команда не будет содержать оператор, или оператор не будет в списке поддерживаемых команд хотя бы одного АЛУ, то УУ уйдет в бесконечный цикл, так как не сможет выполнить данную команду и будет пытаться найти подходящее АЛУ снова и снова.
				virtual EXECUTOR executor() const = 0;
				/// возвращает текстовое название операции. Для операций, которые предпологают выполнение на АЛУ (executor() вернул E_ALU) может возвращается nullptr
				virtual const char* lab() const = 0;
				/*! данный оператор производит фактичское исполнение команды и вносит соответсвующие изменения в регистры процессора. ВАЖНО! При определении собственных операций для УУ, не забывайте увеличивать значение счётчика команды на единицу (или, в зависимости от функционала, совершать переход в любую другую область программы). Операции, выполняемые на АЛУ, не требуют инкремента счёчика, это происходит автоматически.
				@param proc ссылка на процессора
				@param cmd ссылка на команду
				@param time текущее системное время				
				*/
				virtual void operator()(processor &proc, const command& cmd, long time) const = 0;
		}; 
		
		const class : public base {
				public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual const char* lab() const override {return "ALLC";}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					RAM *ram = proc.get_RAM();
					if(argc == 1){
						ram->lock_write();
					//	std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						byte adr = ram -> alloc(1, &prog);  // выделяем память размером в 1 байт. Указатель помещаем в аргумент
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
						ram->unlock_write();
					}
					else if(argc == 2){
						ram->lock_write();
					//	std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cmd << std::endl;
						byte adr = ram -> alloc(proc.get_value(*cmd.get_operand(1)), &prog); // выделяем память размером в переданное число вторым аргументом число. Указатель помещаем в первый аргумент
						proc.get_slot(*cmd.get_operand(0)).data = adr;
						prog.inc();
						ram->unlock_write();
					}
					else throw std::logic_error("ALLC must have one or two args");
			}				
		} ALLC;///< Одна из базовых операций. Просит ОП выделить новый блок из кучи, результат помещается в первый операнд. Если присутсвует второй операнд, его значение интерпретируется как размер выделяемого блока в байтах
		
		const class : public base {
				public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1){
							RAM *ram = proc.get_RAM();
							ram->lock_write();
							ram -> free(proc.get_value(*cmd.get_operand(0)), &prog);  // освобождение динамически выделенной памяти
							prog.inc();
							ram->unlock_write();
					}
					else throw std::logic_error("FREE must have only one argument");
			}		
				virtual const char* lab() const override {return "FREE";}		
		} FREE;///< Одна из базовых операций. Просит ОП вернуть блок памяти с указанным адресом в кучу.
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					lock_ram_by_operands(cmd, proc);
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
					unlock_ram(proc);
				}
				virtual const char* lab() const override {return "JUMP";}	
		} JUMP;	///< Одна из базовых операций. Если задан один операнд, то совершается безусловный переход. Операнд интерпретируется как номер инструкции. Если здано два операнда, то в случае истинности первого операнда происходит переход по второму, иначе - переход к следующей инструкции. Если задано три операнда, то в случае истинности первого операнда происходит переход по второму, иначе - переход по третьему
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					proc.get_RAM()->lock_read();
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.push(proc.get_value(*cmd.get_operand(0)));			
					proc.get_process().inc();		
					proc.get_RAM()->unlock_read();			
				}
				virtual const char* lab() const override {return "PUSH";}	
		} PUSH;///< Одна из базовых операций. Помещает переданный параметром аргумент на стек процесса
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					lock_ram_by_operands(cmd, proc);
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					proc.get_slot(*cmd.get_operand(0)).data = prog.pop();	
					proc.get_process().inc();		
					unlock_ram(proc);					
				}
				virtual const char* lab() const override {return "POP";}	
		} POP;///< Одна из базовых операций. Изимает данные с вершины стека процесса и помещает в переданный аргумент
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					proc.get_RAM()->lock_read();
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					int cc = proc.reg_count();
					for(int i = 0; i < cc; ++i)
						prog.push(proc.read_reg(i).data);	
					prog.inc();			
					proc.get_RAM()->unlock_read();					
				}
				virtual const char* lab() const override {return "PUSHR";}	
		} PUSHR;///< Одна из базовых операций. Помещает состояние всех регистров блока регистров процессора на вершину стека процесса (первый регистр в помещается первым)
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					lock_ram_by_operands(cmd, proc);
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					if(argc == 1)
						proc.get_slot(*cmd.get_operand(0)).data = prog.stack_i(0);
					else
						proc.get_slot(*cmd.get_operand(0)).data = prog.stack_i(proc.get_value(*cmd.get_operand(1)));	
					prog.inc();	
					unlock_ram(proc);
				}
				virtual const char* lab() const override {return "PEEK";}	
		} PEEK;///< Одна из базовых операций. Помещает в первый операнд содержимое стека с отступом от вершины, равным второму операнду (по умолчанию 0)
		

		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					proc.get_RAM()->lock_read();
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					for(int i = proc.reg_count() - 1; i >=0 ; --i)
						proc.reg_access(i).data = prog.pop();
					proc.get_process().inc();		
					proc.get_RAM()->unlock_read();
				}
				virtual const char* lab() const override {return "POPR";}	
		} POPR;	///< Одна из базовых операций. Переносит значения с вершины стека процесса в регисты процессора (так, что вершина стека станет последним регистром)
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					proc.get_RAM()->lock_read();
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.push(prog.count()+1);
					prog.jump(proc.get_value(*cmd.get_operand(0)));
					proc.get_RAM()->unlock_read();
				}
				virtual const char* lab() const override {return "CALL";}	
		} CALL;	///< Одна из базовых операций. Выполняет переход по переданному параметром адресу. При этом на вершину стека процесса помещается адрес возврата
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					proc.get_RAM()->lock_read();
					int argc = cmd.get_args_count();
					process &prog = proc.get_process();
					prog.jump(prog.pop());
					if(argc == 1)
					{
						int mx = proc.get_value(*cmd.get_operand(0));
						for(int i=0; i < mx; ++i)
							prog.pop();
					}
					proc.get_RAM()->unlock_read();
				}
				virtual const char* lab() const override {return "RET";}	
		} RET;///< Одна из базовых операций. Выполняет переход к инструкции, адрес которой лежит на стеке процесса. Необязательный аргумент сообщает, сколько позиций необходимо снять со стека (например, если стек использовался для передачи аргументов)
		
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
		} VAR;///< Фиктивная операция. Используется компилятором для выделения памяти под статические переменные программы. 
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{		
					lock_ram_by_operands(cmd, proc);			
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0))});		
					unlock_ram(proc);
				}
				virtual const char* lab() const override {return nullptr;}	
		} UNARY;///< Одна из базовых операций. Производит вычисления унарных операторов на АЛУ. Результат помещает в операнд.
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_ALU;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{		
					lock_ram_by_operands(cmd, proc);			
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0)), proc.get_value(*cmd.get_operand(1))});					
					unlock_ram(proc);
				}
				virtual const char* lab() const override {return nullptr;}	
		} BINARY;///< Одна из базовых операций. Производит вычисления бинарных операторов на АЛУ. Результат помещает в первый операнд.
		
	
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{
					lock_ram_by_operands(cmd, proc);
					proc.get_slot(*cmd.get_operand(0)).data = proc.get_value(*cmd.get_operand(1));		
					proc.get_process().inc();
					unlock_ram(proc);
				}
				virtual const char* lab() const override {return "SET";}	
		} SET;	///< Одна из базовых операций. Копирует фактические данные второго операнда и помещает их в первый.
		
		
		const class : public base 
		{
			public:
				virtual EXECUTOR executor() const override {return E_CONTROLLER;}
				virtual void operator()(processor &proc, const command& cmd, long time) const override{					
					throw std::logic_error("executed none operation");
				}
				virtual const char* lab() const override {return "NONE";}	
		} NONE;///< Абстрактная операция. Используется как заглушка. Исполнение данной оперцаии вызывает исключение
	}
}

#endif