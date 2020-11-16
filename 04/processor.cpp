#include "processor.h"

namespace jpl
{
	
	bool is_valid_name(const std::string& lab)
	{
		int len = lab.length();
		if(len>8) return false;
		if(!isalpha(lab[0])) return false;
		for(int i=1; i < len; ++i)
			if(!isalpha(lab[i]) && !isdigit(lab[i])) return false;
		return true;
	}
	
	command::command(OPERATION_TYPE op_, const char* str, const operator_ *act_) : op(op_), act(act_)
	{
		args[0] = nullptr;
		args[1] = nullptr;
		args[2] = nullptr;
		if(str)
			label = str;
	}
	command::command(OPERATION_TYPE op_, const char* str, const operand& op1, const operator_ *act_) : op(op_), act(act_)
	{
		args[0] = op1.copy();
		args[1] = nullptr;
		args[2] = nullptr;
		if(str)
			label = str;
	}
	command::command(OPERATION_TYPE op_, const char* str, const operand& op1, const operand& op2, const operator_ *act_) : op(op_), act(act_)
	{
		args[0] = op1.copy();
		args[1] = op2.copy();
		args[2] = nullptr;
				
		if(str)
			label = str;
	}
	command::command(OPERATION_TYPE op_, const char* str, const operand& op1, const operand& op2, const operand& op3, const operator_ *act_) : op(op_), act(act_)
	{
		args[0] = op1.copy();
		args[1] = op2.copy();
		args[2] = op3.copy();
		if(str)
			label = str;
	}
	
	command::command(const command& cmd) : op(cmd.op), label(cmd.label), act(cmd.act)
	{
		args[0] = cmd.args[0] ? cmd.args[0] -> copy() : nullptr;
		args[1] = cmd.args[1] ? cmd.args[1] -> copy() : nullptr;
		args[2] = cmd.args[2] ? cmd.args[2] -> copy() : nullptr;
	}
	command::command(command&& cmd) : op(cmd.op), label(cmd.label), act(cmd.act)
	{
		args[0] = cmd.args[0];
		args[1] = cmd.args[1];
		args[2] = cmd.args[2];
		cmd.args[0] = nullptr;
		cmd.args[1] = nullptr;
		cmd.args[2] = nullptr;
	}
	
	command::~command()
	{
		if(args[0]) delete args[0];
		if(args[1]) delete args[1];
		if(args[2]) delete args[2];
	}

	command& command::operator=(const command& cmd)
	{
		op = cmd.op;
		label = cmd.label;
		act = cmd.act;
		if(args[0]) delete args[0];
		args[0] = cmd.args[0] ? cmd.args[0] -> copy() : nullptr;
		if(args[1]) delete args[1];
		args[1] = cmd.args[1] ? cmd.args[1] -> copy() : nullptr;
		if(args[2]) delete args[2];
		args[2] = cmd.args[2] ? cmd.args[2] -> copy() : nullptr;
		return *this;
	}
	
	command& command::operator=(command&& cmd)
	{
		op = cmd.op;
		label = cmd.label;
		act = cmd.act;
		
		args[0] = cmd.args[0];
		args[1] = cmd.args[1];
		args[2] = cmd.args[2];
		cmd.args[0] = nullptr;
		cmd.args[1] = nullptr;
		cmd.args[2] = nullptr;
		return *this;
	}
	
	void command::set_operand(int num, const operand* arg) {
			if(num < 0 || num > 2)				
				throw std::logic_error("out bounds");
			if(arg) 
				args[num] = arg->copy(); 
			else {
				if(args[num]) 
					delete args[num];
				args[num] = nullptr;
			}
		}
	
	int command::get_args_count() const 
	{
		if(!args[0]) return 0;
		if(!args[1]) return 1;
		if(!args[2]) return 2;
		return 3;
	}
	
	std::ostream& operator<<(std::ostream &out, const command &cmd){
		
		OPERATION_TYPE op_t = cmd.op;
		
		if(cmd.label.size()>0) out << cmd.label << ":\t";
		else out << "\t";
		if(op_t == OP_JUMP)
		{
			out << "JUMP\t";
			if(cmd.args[0])
				cmd.args[0]->print(out) << "\t";
			if(cmd.args[1])
				cmd.args[1]->print(out) << "\t";
			if(cmd.args[2])
				cmd.args[2]->print(out) << "\t";
		}
		else if(op_t == OP_ALLC)
		{
			cmd.args[0] -> print(out << "ALLC\t");
			if(cmd.args[1]) cmd.args[1] -> print(out << "\t");
		}
		else if(op_t == OP_VAR)
		{
			cmd.args[0] -> print(out << "VAR\t");
		}
		else if(op_t == OP_UNARY)
		{
			cmd.args[0]->print(cmd.act->print(out) << "\t");
		}
		else if(op_t == OP_BINARY)
		{
			cmd.args[1]->print(cmd.args[0]->print(cmd.act->print(out) << "\t") << "\t");
		}
		return out;
	}
	///--- reg_operand ---///
	byte reg_operand::value(const processor* p) const {if(p == nullptr) throw std::logic_error("nullptr was given as argument"); return p->read_reg(adr->value(p)).data;}
	void reg_operand::set(processor &p, byte val) const {p.reg_access(adr->value(&p)).data = val;}
	bool reg_operand::lock(processor &p, ALU* a) const 
	{
		slot &s = p.reg_access(adr->value(&p));
		if(s.locker && s.locker != a) return false;
		s.locker = a;
		return true;
	}
	void reg_operand::unlock(processor &p, ALU* a) const 
	{
		slot &s = p.reg_access(adr->value(&p));
		if(s.locker && s.locker != a) throw std::logic_error("ALU tried to unlock a memory slot that does not belong to it");
		s.locker = nullptr;
	}
	std::ostream& reg_operand::print(std::ostream& out) const
	{
		adr->print(out << "(");
		return out;
	}
	
	///--- label_operand ---///
	
	label_operand::label_operand(const std::string &lab)
	{
		if(!is_valid_name(lab)) throw std::logic_error("incorrect var name");
		label = lab;
	}
	
	byte label_operand::value(const processor* p) const {
		if(p == nullptr) throw std::logic_error("nullptr was given as argument"); 
		return (*(p->get_progmem())).unname(label);
	}
		
	std::ostream& label_operand::print(std::ostream& out) const
	{
		out << label;
		return out;
	}
	
	///--- ram_operand ---///
	ram_operand::ram_operand(const label_operand &adr_) : adr(adr_.copy()), is_lab(true) {}
	
	operand* ram_operand::copy() const {ram_operand *tmp = new ram_operand(*adr); tmp->is_lab = is_lab; return tmp;}
	
	byte ram_operand::value(const processor* p) const {
		if(p == nullptr) throw std::logic_error("nullptr was given as argument");
		int base = 0;
		if(is_lab) base = p->get_progmem()->get_base();			
		return (*(p->get_RAM()))[adr->value(p)+ base].data;
	}
	void ram_operand::set(processor &p, byte val) const 
	{
		int base = 0;
		if(is_lab) base = p.get_progmem()->get_base();	
		(*(p.get_RAM()))[adr->value(&p)+base].data = val;
	}
	bool ram_operand::lock(processor &p, ALU* a) const 
	{
		slot &s = (*(p.get_RAM()))[adr->value(&p)];
		if(s.locker && s.locker != a) return false;
		s.locker = a;
		return true;
	}
	void ram_operand::unlock(processor &p, ALU* a) const 
	{
		slot &s = (*(p.get_RAM()))[adr->value(&p)];
		if(s.locker && s.locker != a) throw std::logic_error("ALU tried to unlock a memory slot that does not belong to it");
		s.locker = nullptr;
	}
	std::ostream& ram_operand::print(std::ostream& out) const
	{
		if(!is_lab) out << "[";
		adr->print(out);
		return out;
	}
	
	///--- controller ---///
	
	bool controller::on_tick(processor& proc)
	{
		progmem &prog = *proc.get_progmem();
		
		{
			bool is_exec = false;
			auto end = proc.al_end();
			for(auto al = proc.al_begin(); al != end; ++al)
				if(al->is_running())
					(*al)(proc, time);
		}
		if(!prog.has_next())
		{
			std::cout << "(" << time << "t)\t" << "awaiting for ALUs" << std::endl;
			++time;
			return false;
		}
		
		const command& cur = prog.current();
		OPERATION_TYPE cur_op = cur.get_operation();
		if(cur_op == OP_VAR)
		{
			std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cur << std::endl;
			/*const std::string &name = ((label_operand *) cur.get_operand(0))->get_label();
			if(!is_valid_name(name))  throw std::logic_error("incorrect var name");
			int adr = proc.get_RAM()->alloc(1);
			prog.set_name(name, adr);*/
			prog.inc();
		}
		else if(cur_op == OP_ALLC)
		{
			if(cur.get_args_count()==1){
				if(cur.get_operand(0) -> lock(proc, nullptr))
				{
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cur << std::endl;
					byte adr = proc.get_RAM() -> alloc(1);  // выделяем память размером в 1 байт. Указатель помещаем в аргумент
					cur.get_operand(0) -> set(proc, adr);
					prog.inc();
				}
			}
			else if(cur.get_args_count()==2){
				if(cur.get_operand(0) -> lock(proc, nullptr) && cur.get_operand(1) -> lock(proc, nullptr))
				{
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cur << std::endl;
					byte adr = proc.get_RAM() -> alloc(cur.get_operand(1) -> value(&proc)); // выделяем память размером в переданное число вторым аргументом число. Указатель помещаем в первый аргумент
					cur.get_operand(0) -> set(proc, adr);
					prog.inc();
				}
			}
			else throw std::logic_error("ALLC param must have one or two args");
		}
		else if(cur_op == OP_JUMP)
		{
			int cc = prog.count();
			int argc = cur.get_args_count();
			bool is_e = false;
			if(argc == 1){
				if(cur.get_operand(0) -> lock(proc, nullptr)){
					is_e = true;
					prog.jump(cur.get_operand(0) -> value(&proc));
				}
			}
			else if(argc == 2){
				if(cur.get_operand(0) -> lock(proc, nullptr) && cur.get_operand(1) -> lock(proc, nullptr)){
					is_e = true;
					if(cur.get_operand(0) -> value(&proc))
						prog.jump(cur.get_operand(1) -> value(&proc));
					else
						prog.inc();
				}
			}
			else{
				if(cur.get_operand(0) -> lock(proc, nullptr) && cur.get_operand(1) -> lock(proc, nullptr) && cur.get_operand(2) -> lock(proc, nullptr)){
					is_e = true;
					if(cur.get_operand(0) -> value(&proc))
						prog.jump(cur.get_operand(1) -> value(&proc));
					else
						prog.jump(cur.get_operand(2) -> value(&proc));
				}
			}
			if(is_e)
				std::cout << "(" << time << "t)\t[0x" << std::hex << cc << std::dec << "]\t" << cur << std::endl;
		} 
		else
		{
			bool is_exec = false;
			auto end = proc.al_end();
			for(auto al = proc.al_begin(); al != end; ++al)
				if((*al).is_available(*cur.get_operator()) && (*al)(proc, cur, time)){
					is_exec = true;  
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << cur << std::endl;
					break;
				}
			if(is_exec)
				prog.inc();
		}
		++time;
		return prog.has_next();
	}

	///--- ALU ---///
	
	bool ALU::operator()(processor& proc, int time)
	{
		if(time>=fre)
		{
			if(last)
			{
				const command& cmd = *last;
				if(cmd.get_operation() == OP_UNARY)
				{
					byte a = cmd.get_operand(0)->value(&proc);
					cmd.get_operand(0)->set(proc, (*cmd.get_operator())(&a));
					cmd.get_operand(0)->unlock(proc, this);
				}
				else if(cmd.get_operation() == OP_BINARY)
				{
					byte a[2] = {cmd.get_operand(0)->value(&proc), cmd.get_operand(1)->value(&proc)};
					cmd.get_operand(0)->set(proc, (*cmd.get_operator())(a));
					cmd.get_operand(0)->unlock(proc, this);
					cmd.get_operand(1)->unlock(proc, this);
				}
				else throw std::logic_error("invalid operation type. ALU can proceed only binary and unary operations");
				last = nullptr;
				return true;
			}
			else return true;
		}
		return false;
	}
	
	
	bool ALU::operator()(processor& proc, const command& cmd_n, int time)
	{
		
		if(last) return false;
		
		if(!is_available(*cmd_n.get_operator())) return false;
		
		if(cmd_n.get_operation() == OP_UNARY)
		{
			if(cmd_n.get_operand(0)->lock(proc, this))
			{
				last = &cmd_n;
				fre = time + dur;
				return true;
			}
		}
		else if(cmd_n.get_operation() == OP_BINARY)
		{
			if(cmd_n.get_operand(0)->lock(proc, this))
			{
				if(cmd_n.get_operand(1)->lock(proc, this))
				{
					last = &cmd_n;
					fre = time + dur;
					return true;
				}
				cmd_n.get_operand(0)->unlock(proc, this); // первый операнд заблокировали, а второй - нет. Надо разблокировать первый.
			}
		}
		else throw std::logic_error("invalid operation type. ALU can proceed only binary and unary operations");
		
		return false;
	}
	///--- registers_block ---///
	registers_block::registers_block(size_t sz) : size(sz) 
	{
		regs = new slot[size];
	}
	registers_block::registers_block(const registers_block& old) : size(old.size)
	{
		regs = new slot[size];
		for(int i = 0; i< size; ++i)
			regs[i] = old.regs[i];
	}
	registers_block::registers_block(registers_block&& old) : size(old.size)
	{
		regs = old.regs;
		old.regs = nullptr;
		old.size = 0;
	}
	registers_block::~registers_block()
	{
		if(regs) delete [] regs;
	}
	slot& registers_block::operator[](const int &adr_)
	{
		if(adr_ < 0 || adr_ >= size) throw std::logic_error("index out bounds");
		return regs[adr_];
	}
	const slot& registers_block::operator[](const int &adr_) const
	{
		if(adr_ < 0 || adr_ >= size) throw std::logic_error("index out bounds");
		return regs[adr_];
	}
	///--- RAM ---///
	byte RAM::alloc(size_t s)
	{
		if(s < 1) throw std::logic_error("memory block size must be greater then zero");
		int ret = ram.size();
		ram.resize(ret + s);
		return ret;
	}
	std::ostream& operator<<(std::ostream& out, const RAM& ram)
	{
		auto end = ram.ram.cend();
		int i = 0;
		for(auto cur = ram.ram.cbegin(); cur != end; ++cur)
			out << "[0x" << std::hex << i++ <<  "]:\t" << std::dec << (*cur).data  << std::endl;
		return out;
	}
	///--- PROGMEM ---///
	void progmem::insert( const command& cmd, int p)
	{
		if(cmd.get_operation() == OP_VAR){
			set_name(((label_operand *) cmd.get_operand(0))->get_label(), weight);
			++weight;
		}
		if(p >= 0)
			prog.insert(prog.begin() + p, cmd);
		else
			prog.push_back(cmd);			
	}
	
	void progmem::erase(int nm){
		if(prog[nm].get_operation() == OP_VAR) { // при удалении объявления переменной, структура статических переменных поменялась
			prog.erase(prog.begin()+nm);
			weight = 0;
			auto cend = prog.cend();
			for(auto cur = prog.cbegin(); cur != cend; ++cur)
			{
				if(cur->get_operation() == OP_VAR)
				{
					label_operand *lb = (label_operand *) (cur -> get_operand(0));
					vars.set(lb->get_label(), weight);
					++weight;
				}
			}
				
		}
		else
			prog.erase(prog.begin()+nm);
		}
	
	std::ostream& operator<<(std::ostream& out, const progmem& prog)
	{
		int i = 0;
		auto end = prog.prog.end();
		for(auto cur = prog.prog.begin(); cur != end; ++cur)
		{
			out << "[0x" << std::hex << i << std::dec << "]:\t" << *cur << std::endl;
			i++;
		}
		return out;
	}
	
	
	void processor::run()
	{
		prog->jump(0);
		prog->set_base(ram->alloc(prog->get_weight()));
		prog->set_name("end", prog->size()); // переход в конец программы
		while(control.on_tick(*this));
		auto end = al_end();
		for(auto al = al_begin(); al != end; ++al)
			while(al->is_running()) control.on_tick(*this);
	}
}