#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <exception>

#include <vector>
#include <unordered_set>
#include <initializer_list>
#include "../dictionary/dictionary.hpp"

#include "types.h"
#include "operands.h"
#include "memory.h"

namespace jpl
{
	// реализация классов 
	
	class command
	{
		private:
		/// тип операции
		OPERATION_TYPE op;
		/// метка
		std::string label;
		/// список аргументов
		std::vector<operand*> args;
		/// указатель на функтор, который выполняет операцию (нужен если операция математическая)
		const operator_ *act;
		public:
		
		command() : op(OP_NONE), act(nullptr) {}
		
		command(OPERATION_TYPE op_, const char* str, const operator_ *act_ = nullptr);
		
		/// важно: полученные по указателю операнды не копируются. По этому используйте синтаксис вроде {new ram_operand(...), new const_operans(...), ...}
		/// данные, переданные в _args будут уничтожены в деструкторе
		command(OPERATION_TYPE op_, const char* str, const std::initializer_list<operand*> &_args, const operator_ *act_ = nullptr);
		
		command(const command& cmd);
		command(command&& cmd);
		
		command& operator=(const command&);
		command& operator=(command&&);
		
		~command();
		
		/// выполняет команду с использованием ресурсов указанного процессора, считая что в данный момент переданное системное время (в тиках)
		void execute(processor &proc, long time) const;
		
		void set_operation(OPERATION_TYPE op_){op = op_;}
		void set_operator(const operator_ *act_){act = act_;}
		void set_label(const std::string &lab) {label = lab;}
		void add_operand(const operand& arg);
		void remove_operand(int num);
		
		OPERATION_TYPE get_operation() const {return op;}
		const operator_* get_operator() const {return act;}
		const std::string& get_label() const {return label;}
		const operand* get_operand(int num) const {if(num < 0|| num >= args.size())	throw std::logic_error("out bounds"); return args[num];}
		
		int get_args_count() const {return args.size();}
		
		/// вывод в поток текста программы
		friend std::ostream& operator<<(std::ostream &out, const command& cmd);
	};	
	
	
	/// устройство управления не требует инициализации, оно просто передает очередную команду процессора в свободное ALU, или само выполняет команду, если она связана с переходом или выделением памяти.
	class controller
	{
		private:
		uint64_t time = 0;
		public:
		bool on_tick(processor&);
		
	};
	class ALU
	{
		private:
		uint64_t dur, fre;
		std::unordered_set<const operator_*> avcom;
		const command *last ; // последняя переданная комманда. нужно для имитации работы более одного такта
		public:
		ALU(uint64_t dur_) : dur(dur_), fre(0), last(nullptr) {}
		ALU(uint64_t dur_, std::vector<const operator_*> cmds) : dur(dur_), fre(0), last(nullptr)
		{
			avcom.insert(cmds.begin(), cmds.end());
		}
		/// пытается исполнить команду. Если не удалось, то возвращает false
		bool operator()(processor&, const command&, int);
		///  пытается завершить уже начатую комманду. Если команда не завершена, то возвращает false
		bool operator()(processor&, int);
		
		/// методы блокировки операндов. Константные операнды пропускаются (возвращается true). Если уже заблокированно чем-то другим, то возвращается false	
		bool lock(processor &proc, const operand& op);
		void unlock(processor &proc, const operand& op);
		
		/// добавить оператор, который можно использовать
		void add_operator(const operator_* op){avcom.insert(op);}
		void remove_operator(const operator_* op){avcom.erase(op);}
		
		bool is_running() const {return last;}
		
		bool is_available(const operator_ &op) const {return avcom.find(&op) != avcom.cend();}
	};
	
	class progmem
	{
		private:
		/// сами комманды
		std::vector<command> prog;
		/// таблица соответсвия имен переменных и адресов в ОП
		dictionary<std::string, int> vars;
		int counter = 0, weight = 0, base = 0; // под весом подразумевается суммарный вес всех выделеных заранее переменных. base - базовый адрес статических переменных
		// Таким образом, синтаксис "VAR a" привод к созданию статической переменной, а "INIT a" к выделению дополнительной памяти
		public:
		// определять свои конструкторы не надо, так как поля типа vector и int сами заботятся о своем копировании и уничтожении
		
		/// передает константный указатель на текущую команду для исполнения
		const command& current() const {return prog[counter];}
		/// вставляет команду на указанную позицию. если число меньше нуля, то вставка идет в конец
		void insert(const command& cmd, int p = -1);
		/// удаляет комманду
		void erase(int nm);
		
		void clear() {prog.clear(), weight = 0;}
		/// устанавливает счетчик
		void jump(int a) {counter = a;}
		/// инкрементирует счетчик
		void inc() {++counter;}

		int size() const {return prog.size();}
		
		int get_weight() const {return weight;}
		
		void set_base(int b) {base = b;}
		int get_base() const {return base;}
		
		int count() const {return counter;}
		
		bool has_next(){return counter < prog.size();}
		// работа с именами переменных
		/// получить адрес по имени
		int unname(const std::string &var) const{return vars[var];}
		/// поставить имени в соответсвие указанный адрес
		void set_name(const std::string &var, const int &adr) {return vars.set(var, adr);}
		/// удалить указанное имя из списка переменных
		void remove_name(const std::string &var) {return vars.unset(var);}
		
		/// вывод в поток исходного текста программы
		friend std::ostream& operator<<(std::ostream&, const progmem&);
	};
	
	class processor
	{
		private:
		controller control;
		std::vector<ALU> alus;
		registers_block regs; // блок регистров это свойство самого процессора
		RAM *ram; // но оперативная память явлется самостоятельным устройством, к которому могут обращаться несколько процессоров.
		progmem *prog; // На реальных машинах это вообще часть ОП )
		public:
		/// обязательным параметром является размер блока регистров. Можно передать список исполнительных устройств или какое-то одно устойство. Оперативная память не является обязательным атрибутом, но тогда в программе не должно быть операндов из ОП.
		processor(const ALU& alu, int regs_, RAM *ram_ = nullptr) : ram(ram_), regs(regs_) {alus.push_back(alu);}
		processor(const std::vector<ALU> &alus_, int regs_, RAM *ram_ = nullptr) : ram(ram_), alus(alus_), regs(regs_) {}
		processor(std::vector<ALU> &&alus_, int regs_, RAM *ram_ = nullptr) : ram(ram_), alus(alus_), regs(regs_) {}
		/// переопределять конструкторы по умолчанию не надо, все поля передаются по значнию (даже указатель на RAM)
		void load_program(progmem *program) {prog = program;}
		/// запускает исполнение переданной программы. Гарантируется, что изменится только счётчик, но не сама программа.
		void run();
		
		bool is_free(const operand& op) const;
		
		slot& get_slot(const operand& op);
		const slot& get_slot(const operand& op) const;
		byte get_value(const operand& op) const;
		
		progmem* get_progmem() {return prog;}
		const progmem* get_progmem() const {return prog;}
		
		void add_alu(const ALU& alu){alus.push_back(alu);}
		void add_alu(ALU&& alu){alus.push_back(alu);} /// для семантики proc.add_alu(ALU());
		
		void remove_alu(int a){alus.erase(alus.begin()+a);}
		ALU& edit_alu(int a){return alus[a];}
				
		auto al_begin(){return alus.begin();}
		auto al_end() const {return alus.end();}
		
		slot& reg_access(byte i) {return regs[i];} // предостовляет доступ к указанному регистру из блока регистров процессора
		const slot& reg_access(byte i) const {return regs[i];} // предостовляет доступ к указанному регистру из блока регистров процессора только для чтения
		const slot& read_reg(byte i) const {return regs[i];} // получает регистр только для чтения
		
		void set_RAM(RAM *ram_){ram = ram_;}
		RAM* get_RAM() const {return ram;}
	};
}
#include "operators.hpp"
#endif