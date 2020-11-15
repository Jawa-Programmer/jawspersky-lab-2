#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <exception>

#include <vector>
#include <unordered_set>
#include "../dictionary/dictionary.hpp"

namespace jpl
{
	bool is_valid_name(const std::string& lab);
	
	// перечисления
	enum OPERATION_TYPE {OP_INIT, OP_JUMP, OP_UNARY, OP_BINARY};
	
	typedef uint16_t byte; // минимальноадресуемой еденицей памяти будет 16 бит
	
	// прототипы классов
	/// класс команды. Принемается УУ или ИУ и исполняется.
	class command;
	/// класс операнда (константа, ссылка на регистр, ссылка на память)
	class operand;
	/// класс устройства управления. Выбирает подходящего исполнителя команды и передает на исполнение
	class controller;
	/// класс исполнительного устройства. Имеет список доступных команд, время исполнение команды.
	class ALU;
	/// класс блока регистров фиксированной длинны
	class registers_block;
	/// класс памяти данных (считается бесконечной, но ссылать можно только на предварительно выделенные учаски
	class RAM;
	/// класс памяти данных. набор команд и счетчик. содержит словарь соответсвия "имя - адрес"
	class progmem;
	
	/// класс процессор. Связывает между собой УУ, ИУ, память и регистры
	class processor;
	/// ячейка памяти. Содержит данные и информацию о блокировавшем устойстве
	struct slot {const ALU *locker; byte data;};
	
	/// класс операции. (унарная, бинарная, переход, инициализация памяти)
	//namespace operations {class base;}
	//typedef operations::base operation;
	/// класс оператора (ADD SUB MUL DIV и тд)
	namespace operators {class base;}
	typedef operators::base operator_;
	
	// реализация классов 
	
	class command
	{
		private:
		OPERATION_TYPE op;
		std::string label;
		operand *args[3];// считаем, что может быть не более трёх операндов (три операнда нужны только в одном случае - расширенный условный переход (если правда, то адрес1, если ложь, то адрес2)
		const operator_ *act;
		public:
		
		command(OPERATION_TYPE op_, const char* str, const operator_ *act_ = nullptr);
		command(OPERATION_TYPE op_, const char* str, const operand& op1, const operator_ *act_ = nullptr);
		command(OPERATION_TYPE op_, const char* str, const operand& op1, const operand& op2, const operator_ *act_ = nullptr);
		command(OPERATION_TYPE op_, const char* str, const operand& op1, const operand& op2, const operand& op3, const operator_ *act_ = nullptr); // чет vararg функции в с++ сложные. Просто имитирую их поведение путем ввода трех конструкторов)
		
		command(const command& cmd);
		command(command&& cmd);
		
		command& operator=(const command&);
		command& operator=(command&&);
		
		~command();
		
		void set_operation(OPERATION_TYPE op_){op = op_;}
		void set_operator(const operator_ *act_){act = act_;}
		void set_label(const std::string &lab) {label = lab;}
		void set_operand(int num, const operand* arg);
		
		OPERATION_TYPE get_operation() const {return op;}
		const operator_* get_operator() const {return act;}
		const std::string& get_label() const {return label;}
		const operand* get_operand(int num) const {if(num < 0||num > 1)	throw std::logic_error("out bounds"); return args[num];}
		
		int get_args_count() const;
		
		/// вывод в поток текста программы
		friend std::ostream& operator<<(std::ostream &out, const command& cmd);
	};	
	
	class operand
	{
		public:
		virtual ~operand() {}
		virtual byte value(const processor*) const = 0;
		virtual void set(processor&, byte) const = 0; // пытается записать указанное значение в фактический адрес оператора (если попытаться записать данные по адресу константного операнда, будет выброшено исключение 
		virtual operand* copy() const = 0; // возвращает указатель на копию объекта операнда. Пользователь должен сам удалить его после использования! Что касается других классов библиотеки, они ганатированно вызывают delete после каждого вызова clone()
		virtual bool lock(processor&, ALU*) const = 0; // пытается заблокировать адрес, куда ссылается операнд, в случае если его уже заблокировало другое устройство, вернет false
		virtual void unlock(processor&, ALU*) const = 0; // пытается разблокировать адрес, куда ссылается операнд. В случае неудачи вызывается исключение (в боудущем это будет заменено на помещение флага ошибки в регистр процессора).
		/// вывод в поток текстового представления операнда
		virtual std::ostream& print(std::ostream&) const = 0;
		};
	
	class const_operand : public operand
	{
		private:
		byte val;
		public:
		const_operand(byte val_) : val(val_) {}
		
		const_operand(const const_operand& old) = default;
		const_operand(const_operand&& old) = default;
		const_operand& operator=(const const_operand& old) = default;
		const_operand& operator=(const_operand&& old) = default;
		
		virtual byte value(const processor* p) const override{return val;}
		virtual void set(processor&, byte) const override {throw std::logic_error("Trying set value to constant");} 
		virtual operand* copy() const  override {return new const_operand(val);}
		virtual bool lock(processor&, ALU*) const override {return true;} // константный операнд лежит в памяти программы и всегда доступен для чтения
		virtual void unlock(processor&, ALU*) const override {} // константный операнд лежит в памяти программы и всегда доступен для чтения
		virtual std::ostream& print(std::ostream& out) const override { return out << val;}
	};
	
	class reg_operand : public operand
	{
		private:
		operand *adr; // этот операнд хранит номер регистра. Возможна последовательность вложенных операндов, что позволяет создавать ссылки.
		public:
		reg_operand(const operand &adr_) : adr(adr_.copy()) {}
		~reg_operand(){	delete adr;}
		virtual byte value(const processor* p) const override;
		virtual void set(processor&, byte) const override;
		virtual operand* copy() const override {return new reg_operand(*adr);}
		virtual bool lock(processor&, ALU*) const override; 
		virtual void unlock(processor&, ALU*) const override;
		virtual std::ostream& print(std::ostream&) const override;
	};
	class ram_operand : public operand
	{
		private:
		operand *adr; // этот операнд хранит адрес в памяти. Возможна последовательность вложенных операндов, что позволяет создавать ссылки.
		public:
		ram_operand(const operand &adr_) : adr(adr_.copy()) {}
		~ram_operand(){	delete adr;}
		virtual byte value(const processor* p) const override;
		virtual void set(processor&, byte) const override;
		virtual operand* copy() const override {return new ram_operand(*adr);}
		virtual bool lock(processor&, ALU*) const override; 
		virtual void unlock(processor&, ALU*) const override;
		virtual std::ostream& print(std::ostream&) const override;
	};
	class var_operand : public operand
	{
		private:
		std::string label; // этот операнд хранит адрес в памяти. Возможна последовательность вложенных операндов, что позволяет создавать ссылки.
		public:
		var_operand(const std::string& lab); // проверяет соответсвие строки стандарту языка (от одного до 8 латинских букв) Если содержит неподходящие символы или имеет неправильную длинну, то выбрасывается исключение
		virtual byte value(const processor* p) const override;
		virtual void set(processor&, byte) const override;
		virtual operand* copy() const override {return new var_operand(label);}
		virtual bool lock(processor&, ALU*) const override; 
		virtual void unlock(processor&, ALU*) const override;
		virtual std::ostream& print(std::ostream&) const override;
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
		
		//bool lock(processor& proc,const operand* op) {return op -> lock(proc, this);} // пытается заблокировать операнд собой
		//void unlock(processor& proc,const operand* op){op->unlock(proc, this);} // пытается разблокировать операнд собой
		/// добавить оператор, который можно использовать
		void add_operator(const operator_* op){avcom.insert(op);}
		void remove_operator(const operator_* op){avcom.erase(op);}
		
		bool is_running() const {return last;}
		
		bool is_available(const operator_ &op) const {return avcom.find(&op) != avcom.cend();}
	};
	class registers_block
	{
		private:
		slot *regs;
		size_t size;
		public:
		registers_block(size_t sz);
		registers_block(const registers_block&);
		registers_block(registers_block&&);
		~registers_block();
		slot& operator[](const int &adr_); // так будет проще. Сама память не следит за правами доступа, это делает ALU
		const slot& operator[](const int &adr_) const; // так будет проще. Сама память не следит за правами доступа, это делает ALU
	};
	class RAM
	{
		private:
		std::vector<slot> ram;
		public:
		// определять свои конструкторы не надо, так как поле типа vector само заботится о своем копировании и уничтожении
		
		slot& operator[](const int &adr_) {return ram[adr_];} // так будет проще. Сама память не следит за правами доступа, это делает ALU
		const slot& operator[](const int &adr_) const {return ram[adr_];} // так будет проще. Сама память не следит за правами доступа, это делает ALU
		
		byte alloc(size_t); // выделяет новую память и передает указатель на начало блока
		
		void free(byte) = delete; // пока заглушка, но в будущем планирую разработать систему по-настоящему динамической памяти
	};
	class progmem
	{
		private:
		/// сами комманды
		std::vector<command> prog;
		/// таблица соответсвия имен переменных и адресов в ОП
		dictionary<std::string, int> vars;
		int counter = 0;
		public:
		// определять свои конструкторы не надо, так как поля типа vector и int сами заботятся о своем копировании и уничтожении
		
		/// передает константный указатель на текущую команду для исполнения
		const command& current() const {return prog[counter];}
		/// вставляет команду на указанную позицию. если число меньше нуля, то вставка идет в конец
		void insert(const command& cmd, int p = -1);
		/// возвращает ссылку на команду для редактирования.
		command& operator[](int nm){return prog[nm];}
		/// устанавливает счетчик
		void jump(int a) {counter = a;}
		/// инкрементирует счетчик
		void inc() {++counter;}
		
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
		
		progmem* get_progmem() {return prog;}
		const progmem* get_progmem() const {return prog;}
		
		void add_alu(const ALU& alu){alus.push_back(alu);}
		void add_alu(ALU&& alu){alus.push_back(alu);} /// для семантики proc.add_alu(ALU());
		void remove_alu(int a){alus.erase(alus.begin()+a);}
		ALU& edit_alu(int a){return alus[a];}
				
		auto al_begin(){return alus.begin();}
		auto al_end() const {return alus.end();}
		
		slot& reg_access(byte i) {return regs[i];} // предостовляет доступ к указанному регистру из блока регистров процессора
		const slot& read_reg(byte i) const {return regs[i];} // получает регистр только для чтения
		
		void set_RAM(RAM *ram_){ram = ram_;}
		RAM* get_RAM() const {return ram;}
	};
	
	namespace operators { 
		class base 
		{
			public:
			virtual std::ostream& print(std::ostream&) const = 0; // выводит в поток текстовое название оператора (совпадает с названием константы безымянного класса)
			virtual byte operator()(const byte*) const = 0; // передается список фактических значений, возвращается результат выполнения оператора.
		};
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "ADD";}
			virtual byte operator()(const byte *args) const override {return args[0] + args[1];}
		} ADD;
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const byte *args) const override {return args[0] - args[1];}
		} SUB;
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const byte *args) const override {return args[0] / args[1];}
		} DIV;
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const byte *args) const override {return args[0] * args[1];}
		} MUL;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const byte *args) const override {return args[1];}
		} SET;
	}
}
#endif