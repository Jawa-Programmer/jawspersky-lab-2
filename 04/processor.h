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

#include "command.h"

namespace jpl
{
	// реализация классов 
	
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
		int  weight = 0; // под весом подразумевается суммарный вес всех выделеных заранее переменных. base - базовый адрес статических переменных
		// Таким образом, синтаксис "VAR a" привод к созданию статической переменной, а "INIT a" к выделению дополнительной памяти
		public:
		// определять свои конструкторы не надо, так как поля типа vector и int сами заботятся о своем копировании и уничтожении
		
		/// вставляет команду на указанную позицию. если число меньше нуля, то вставка идет в конец
		void insert(const command& cmd, int p = -1);
		/// удаляет комманду
		void erase(int nm);
		
		const command& operator[](int i) const {return prog[i];}
		
		void clear() {prog.clear(), weight = 0;}

		int size() const {return prog.size();}
		
		int get_weight() const {return weight;}
		
		// работа с именами переменных
		/// получить адрес по имени
		byte unname(const std::string &var) const {return vars[var];}
		/// поставить имени в соответсвие указанный адрес
		void set_name(const std::string &var, const int &adr) {return vars.set(var, adr);}
		/// удалить указанное имя из списка переменных
		void remove_name(const std::string &var) {return vars.unset(var);}
		
		/// вывод в поток исходного текста программы
		friend std::ostream& operator<<(std::ostream&, const progmem&);
	};
	class process {
		private:
		std::vector<byte> stack; // область стека процесса. хранится на векторе для возможности произвольного доступа
		int st_cou;
		
		const progmem *prog; // указатель на область памяти процесса
		int counter = 0, base = 0; // счётчик команд и смещение области данных процесса в ОП.
		public:		
		process() : prog(nullptr), st_cou(-1) {} 		
		process(const progmem &prog_) : prog(&prog_), st_cou(-1) {
			try{
				counter = prog->unname("start");
			}
			catch (const std::exception &e)
			{
				counter = 0;
			}
		} 		
		const progmem& get_progmem() const {return *prog;}		
		bool has_next(){return counter < prog->size();}
		
		void set_base(int b) {base = b;}
		int get_base() const {return base;}
		
		int count() const {return counter;}
		/// инкрементирует счетчик
		void inc() {++counter;}
		/// устанавливает счетчик
		void jump(int a) {counter = a;}	
		
		/// помещает байт информации на стек
		void push(byte b){
			if(st_cou < (int)stack.size()){
				stack.push_back(b);
			}
			else stack[st_cou] = b;
			++st_cou;
		}
		/// помещает байт информации на стек
		byte pop(){return stack[st_cou];}
		/// произвольный доступ к данным со стека (i - отступ от вершины стека)
		byte stack_i(int i){return stack[stack.size()-1-i];}		
		/// передает константный указатель на текущую команду для исполнения
		const command& current() const {return (*prog)[counter];}
		
	};
	
	class processor
	{
		private:
		controller control;
		std::vector<ALU> alus;
				
		registers_block regs; // блок регистров это свойство самого процессора
		RAM *ram; // но оперативная память явлется самостоятельным устройством, к которому могут обращаться несколько процессоров.
		process prog; 
		public:
		/// обязательным параметром является размер блока регистров. Можно передать список исполнительных устройств или какое-то одно устойство. Оперативная память не является обязательным атрибутом, но тогда в программе не должно быть операндов из ОП.
		processor(const ALU& alu, int regs_, RAM *ram_ = nullptr) : ram(ram_), regs(regs_) {alus.push_back(alu);}
		processor(const std::vector<ALU> &alus_, int regs_, RAM *ram_ = nullptr) : ram(ram_), alus(alus_), regs(regs_) {}
		processor(std::vector<ALU> &&alus_, int regs_, RAM *ram_ = nullptr) : ram(ram_), alus(alus_), regs(regs_) {}
		
		/// запускает исполнение переданного процесса программы. Гарантируется, что программа не будет изменена, только описатель процесса
		void run(const progmem &progmem);
		
		bool is_free(const operand& op) const;
		
		slot& get_slot(const operand& op);
		const slot& get_slot(const operand& op) const;
		byte get_value(const operand& op) const;
		
		process& get_process() {return prog;}
		const process& get_process() const {return prog;}
		
		void add_alu(const ALU& alu){alus.push_back(alu);}
		void add_alu(ALU&& alu){alus.push_back(alu);} /// для семантики proc.add_alu(ALU());
		
		void remove_alu(int a){alus.erase(alus.begin()+a);}
		ALU& edit_alu(int a){return alus[a];}
				
		auto al_begin(){return alus.begin();}
		auto al_end() const {return alus.end();}
		
		int reg_count() const {return regs.get_size();}
		slot& reg_access(byte i) {return regs[i];} // предостовляет доступ к указанному регистру из блока регистров процессора
		const slot& read_reg(byte i) const {return regs[i];} // предостовляет доступ к указанному регистру из блока регистров процессора только для чтения
		
		void set_RAM(RAM *ram_){ram = ram_;}
		RAM* get_RAM() const {return ram;}
	};
}

#include "operators.hpp"
#include "operations.h"

#endif