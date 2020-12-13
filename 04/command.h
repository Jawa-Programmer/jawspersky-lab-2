#ifndef JPL_COMMAND_H
#define JPL_COMMAND_H

#include <vector>

#include "types.h"
#include "operands.h"

namespace jpl
{
	class command
	{
		private:
		/// тип операции
		const operation *op;
		/// метка
		std::string label;
		/// список аргументов
		std::vector<operand*> args;
		/// указатель на функтор, который выполняет операцию (нужен если операция математическая)
		const operator_ *act;
		public:
		
		command();
		
		command(const operation &op_, const char* str, const operator_ *act_ = nullptr);
		
		/// важно: полученные по указателю операнды не копируются. По этому используйте синтаксис вроде {new ram_operand(...), new const_operans(...), ...}
		/// данные, переданные в _args будут уничтожены в деструкторе
		command(const operation &op_, const char* str, const std::initializer_list<operand*> &_args, const operator_ *act_ = nullptr);
		
		command(const command& cmd);
		command(command&& cmd);
		
		command& operator=(const command&);
		command& operator=(command&&);
		
		~command();
		
		/// выполняет команду с использованием ресурсов указанного процессора, считая что в данный момент переданное системное время (в тиках)
		void execute(processor &proc, long time) const;
		
		void set_operation(const operation &op_);
		void set_operator(const operator_ *act_){act = act_;}
		void set_label(const std::string &lab) {label = lab;}
		void add_operand(const operand& arg);
		void remove_operand(int num);
		
		const operation& get_operation() const {return *op;}
		const operator_* get_operator() const {return act;}
		const std::string& get_label() const {return label;}
		const operand* get_operand(int num) const {if(num < 0|| num >= args.size())	throw std::logic_error("out bounds"); return args[num];}
		
		int get_args_count() const {return args.size();}
		
		/// вывод в поток текста программы
		friend std::ostream& operator<<(std::ostream &out, const command& cmd);
	};	
}

#endif