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
		if(str)
			label = str;
	}
	command::command(OPERATION_TYPE op_, const char* str, const std::initializer_list<operand*> &_args, const operator_ *act_) : op(op_), act(act_)
	{
		//args.reserve(_args.size());
		auto end = _args.end();
		for(auto itr = _args.begin(); itr != end; ++itr)
			args.push_back((*itr));
		if(str)
			label = str;
	}
	
	command::command(const command& cmd) : op(cmd.op), label(cmd.label), act(cmd.act)
	{
		args.reserve(cmd.args.size());
		for(auto itr = cmd.args.begin(); itr != cmd.args.end(); ++itr)
			args.push_back((*itr)->copy());
	}
	command::command(command&& cmd) : op(cmd.op), label(cmd.label), act(cmd.act)
	{
		args = cmd.args;
		cmd.args.clear();
	}
	
	command::~command()
	{
		for(auto itr = args.begin(); itr != args.end(); ++itr)
			delete (*itr);
	}

	command& command::operator=(const command& cmd)
	{
		op = cmd.op;
		label = cmd.label;
		act = cmd.act;
		
		for(auto itr = args.begin(); itr != args.end(); ++itr)
			delete (*itr);
		args.clear();
		
		args.reserve(cmd.args.size());
		
		for(auto itr = cmd.args.begin(); itr != cmd.args.end(); ++itr)
			args.push_back((*itr)->copy());
		
		return *this;
	}
	
	command& command::operator=(command&& cmd)
	{
		op = cmd.op;
		label = cmd.label;
		act = cmd.act;
		
		args = cmd.args;
		cmd.args.clear();
		
		return *this;
	}
	
	void command::add_operand(const operand& arg) {
			args.push_back(arg.copy());
		}
	void command::remove_operand(int num) {
			args.erase(args.begin() + num);
	}
	
	void command::execute(processor &proc, long time) const
	{
		progmem &prog = *proc.get_progmem();
		
		int argc = args.size();
		
		if(op == OP_VAR)
		{
			std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << *this << std::endl;
			prog.inc();
		}
		else if(op == OP_ALLC)
		{
			if(argc == 1){
				if(proc.is_free(*args[0]))
				{
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << *this << std::endl;
					byte adr = proc.get_RAM() -> alloc(1);  // выделяем память размером в 1 байт. Указатель помещаем в аргумент
					proc.get_slot(*args[0]).data = adr;
					prog.inc();
				}
			}
			else if(args.size()==2){
				if(proc.is_free(*args[0]) && proc.is_free(*args[1]))
				{
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << *this << std::endl;
					byte adr = proc.get_RAM() -> alloc(proc.get_value(*args[1])); // выделяем память размером в переданное число вторым аргументом число. Указатель помещаем в первый аргумент
					proc.get_slot(*args[0]).data = adr;
					prog.inc();
				}
			}
			else throw std::logic_error("ALLC must have one or two args");
		}
		else if(op == OP_JUMP)
		{
			int cc = prog.count();
			bool is_e = false;
			if(argc == 1){
				if(proc.is_free(*args[0])){
					is_e = true;
					prog.jump(proc.get_value(*args[0]));
				}
			}
			else if(argc == 2){
				if(proc.is_free(*args[0]) && proc.is_free(*args[1])){
					is_e = true;
					if(proc.get_value(*args[0]))
						prog.jump(proc.get_value(*args[1]));
					else
						prog.inc();
				}
			}
			else{
				if(proc.is_free(*args[0]) && proc.is_free(*args[1])&& proc.is_free(*args[2])){
					is_e = true;
					if(proc.get_value(*args[0]))
						prog.jump(proc.get_value(*args[1]));
					else
						prog.jump(proc.get_value(*args[2]));
				}
			}
			if(is_e)
				std::cout << "(" << time << "t)\t[0x" << std::hex << cc << std::dec << "]\t" << *this << std::endl;
		} 
		else
		{
			bool is_exec = false;
			auto end = proc.al_end();
			for(auto al = proc.al_begin(); al != end; ++al)
				if((*al).is_available(*act) && (*al)(proc, *this, time)){
					is_exec = true;  
					std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << *this << std::endl;
					break;
				}
			if(is_exec)
				prog.inc();
		}
	}
	
	std::ostream& operator<<(std::ostream &out, const command &cmd){
		
		OPERATION_TYPE op_t = cmd.op;
		
		if(cmd.label.size()>0) out << cmd.label << ":\t";
		else out << "\t";
		if(op_t == OP_JUMP)
		{
			out << "JUMP\t";
			int cou = cmd.args.size();
			if(cou >= 1)
				cmd.args[0]->print(out) << "\t";
			if(cou >= 2)
				cmd.args[1]->print(out) << "\t";
			if(cou >= 3)
				cmd.args[2]->print(out) << "\t";
		}
		else if(op_t == OP_ALLC)
		{
			cmd.args[0] -> print(out << "ALLC\t");
			if(cmd.args.size() > 1) cmd.args[1] -> print(out << "\t");
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
	
	
	///--- controller ---///
	
	bool controller::on_tick(processor& proc)
	{
		progmem &prog = *proc.get_progmem();
		
		{
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
		
		prog.current().execute(proc, time);
		
		++time;
		return prog.has_next();
	}

	
	///--- ALU ---///
	
	bool ALU::lock(processor &proc, const operand& op)
	{
		OPERAND_TYPE opt = op.type();
		if(opt == OPR_CONST || opt == OPR_LABEL) return true;
		
		slot& sl = proc.get_slot(op);
		if(sl.locker && sl.locker != this) return false;
		sl.locker = this;
		return true;
	}
	
	void ALU::unlock(processor &proc, const operand& op)
	{
		OPERAND_TYPE opt = op.type();
		if(opt == OPR_CONST || opt == OPR_LABEL) return;
		slot& sl = proc.get_slot(op);
		if(sl.locker && sl.locker != this) throw std::logic_error("this operand was locked by another ALU");
		sl.locker = nullptr;
	}
	
	bool ALU::operator()(processor& proc, int time)
	{
		if(time>=fre)
		{
			if(last)
			{
				const command& cmd = *last;
				if(cmd.get_operation() == OP_UNARY)
				{
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0))});
					unlock(proc, *cmd.get_operand(0));
				}
				else if(cmd.get_operation() == OP_BINARY)
				{
					proc.get_slot(*cmd.get_operand(0)).data = (*cmd.get_operator())({proc.get_value(*cmd.get_operand(0)), proc.get_value(*cmd.get_operand(1))});
					unlock(proc, *cmd.get_operand(0));
					unlock(proc, *cmd.get_operand(1));
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
			if(lock(proc, *cmd_n.get_operand(0)))
			{
				last = &cmd_n;
				fre = time + dur;
				return true;
			}
		}
		else if(cmd_n.get_operation() == OP_BINARY)
		{
			if(lock(proc, *cmd_n.get_operand(0)))
			{
				if(lock(proc, *cmd_n.get_operand(1)))
				{
					last = &cmd_n;
					fre = time + dur;
					return true;
				}
				unlock(proc, *cmd_n.get_operand(0)); // первый операнд заблокировали, а второй - нет. Надо разблокировать первый.
			}
		}
		else throw std::logic_error("invalid operation type. ALU can proceed only binary and unary operations");
		
		return false;
	}
	///--- PROGMEM ---///
	void progmem::insert( const command& cmd, int p)
	{
		if(cmd.get_operation() == OP_VAR){
			set_name(((label_operand *) cmd.get_operand(0))->value(), weight);
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
					vars.set(lb->value(), weight);
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
	///--- processor ---///
	bool processor::is_free(const operand& op) const
	{
		switch(op.type())
		{
			case OPR_CONST:
			case OPR_LABEL:
				return true;
			break;
			case OPR_RAM:
				return get_slot(op).locker == nullptr;
			case OPR_REG:				
				return get_slot(op).locker == nullptr;
			break;
		}
	}
	
	slot& processor::get_slot(const operand& op) 
	{
		switch(op.type())
		{
			case OPR_CONST:
			case OPR_LABEL:
				throw std::logic_error("trying get memory slot of const operand");
			break;
			case OPR_RAM:{	
				const ram_operand *rm = (const ram_operand*) &op;
				int base = 0;
				if(rm->is_label())
					base = prog->get_base();
				return (*ram)[get_value(rm->value()) + base];
			}
			break;
			case OPR_REG:{
				const reg_operand *rg = (const reg_operand*) &op;
				return reg_access(get_value(rg->value()));
			}
			break;
		}
	}
	
	const slot& processor::get_slot(const operand& op) const
	{
		switch(op.type())
		{
			case OPR_CONST:
			case OPR_LABEL:
				throw std::logic_error("trying get memory slot of const operand");
			break;
			case OPR_RAM:{
				const ram_operand *rm = (const ram_operand*) &op;int base = 0;
				if(rm->is_label())
					base = prog->get_base();
				return (*ram)[get_value(rm->value()) + base];
			}
			break;
			case OPR_REG:{	
				const reg_operand *rg = (const reg_operand*) &op;
				return reg_access(get_value(rg->value()));
			}
			break;
		}
	}
	
	byte processor::get_value(const operand& op) const
	{
		switch(op.type())
		{
			case OPR_CONST:{
				const const_operand *cnst = (const const_operand*)&op;
				return cnst->value();
			}
			break;
			case OPR_LABEL:{
				const label_operand *lab = (const label_operand*) &op;
				return prog->unname(lab->value());
			}
			break;
			case OPR_RAM:{
				const ram_operand *rm = (const ram_operand*) &op;
				int base = 0;
				if(rm->is_label())
					base = prog->get_base();
				return (*ram)[get_value(rm->value()) + base].data;
			}
			break;
			case OPR_REG:{
				const reg_operand *rg = (const reg_operand*) &op;
				return read_reg(get_value(rg->value())).data;
			}
			break;
		}
	}
	
	void processor::run()
	{
		prog->jump(0);
		if(prog->get_weight()) prog->set_base(ram->alloc(prog->get_weight()));
		prog->set_name("end", prog->size()); // переход в конец программы
		while(control.on_tick(*this));
		auto end = al_end();
		for(auto al = al_begin(); al != end; ++al)
			while(al->is_running()) control.on_tick(*this);
	}
}