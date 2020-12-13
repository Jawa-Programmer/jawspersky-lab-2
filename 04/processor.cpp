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
	
	command::command() : op(&operations::NONE), act(nullptr) {}
	
	
	void command::set_operation(const operation &op_){op = &op_;}
	
	command::command(const operation &op_, const char* str, const operator_ *act_) : op(&op_), act(act_)
	{
		if(str)
			label = str;
	}
	command::command(const operation &op_, const char* str, const std::initializer_list<operand*> &_args, const operator_ *act_) : op(&op_), act(act_)
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
		process &prog = proc.get_process();
		
		int argc = args.size();		
		
		if(op->executor() == E_CONTROLLER)
		{
			auto vec = op->get_requirements();
			for(int i=0; i < vec.size(); i++)
				if(!proc.is_free(reg_operand(const_operand(vec[i]))))
					return;
			
			for(int i=0; i < args.size(); i++)
				if(!proc.is_free(*args[i])) return; 
			(*op)(proc, *this, time);
		}
		else
		{
			bool is_exec = false;
			auto end = proc.al_end();
			for(auto al = proc.al_begin(); al != end; ++al)
				if((*al)(proc, *this, time)){
					is_exec = true;  
				//	std::cout << "(" << time << "t)\t[0x" << std::hex << prog.count() << std::dec << "]\t" << *this << std::endl;
					break;
				}
			if(is_exec)
				prog.inc();
		}
	}
	
	std::ostream& operator<<(std::ostream &out, const command &cmd){
		
		EXECUTOR op_t = cmd.op->executor();
		
		if(cmd.label.size()>0) out << cmd.label << ":\t";
		else out << "\t";
		if(cmd.act)
			cmd.act->print(out);
		else 
			out << cmd.op->lab();
		int c = cmd.args.size();
		for(int i=0; i < c; i++)
			cmd.args[i]->print(out << '\t');
		return out;
	}
	
	
	///--- controller ---///
	
	bool controller::on_tick(processor& proc)
	{
		process &prog = proc.get_process();
		
		{
			auto end = proc.al_end();
			for(auto al = proc.al_begin(); al != end; ++al)
				if(al->is_running())
					(*al)(proc, time);
		}
		if(!prog.has_next())
		{
			
			++time;
			return false;
		}
	//	std::cout << prog.current() << std::endl;
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
				cmd.get_operation()(proc, cmd, time);
				for(int i=0; i < cmd.get_args_count(); i++)
					unlock(proc, *cmd.get_operand(i));
				auto vec = cmd.get_operation().get_requirements();
				for(int i=0; i < vec.size(); i++)
					unlock(proc, reg_operand(const_operand(vec[i])));
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
		
		bool is_brk = true;
		auto vec = cmd_n.get_operation().get_requirements();
		
		for(int i=0; i < vec.size(); i++)
			if(!lock(proc, reg_operand(const_operand(vec[i]))))
				{
					is_brk = false;
					for(int j=0; j < i; j++)
						unlock(proc, reg_operand(const_operand(vec[j])));
					break;
				}
		if(is_brk)
			for(int i=0; i < cmd_n.get_args_count(); i++)
					if(!lock(proc, *cmd_n.get_operand(i)))
					{
						is_brk = false;
						for(int j=0; j < i; j++)
							unlock(proc, *cmd_n.get_operand(j));
						break;
					}		
		if(is_brk)
		{
			last = &cmd_n;
			fre = time + dur;
			return true;
		}
		return false;
	}
	
	///--- PROGMEM ---///
	void progmem::insert( const command& cmd, int p)
	{
		if(cmd.get_operation().type() == OP_VAR){			
			set_name(((label_operand *) cmd.get_operand(0))->value(), weight);
			if(cmd.get_args_count() > 1){
				if (cmd.get_operand(1)->type() != OPR_CONST) throw std::logic_error("incorrect operand. Parameters of static initialisation by INIT must be constant");
				weight += ((const_operand *) cmd.get_operand(1))->value();
			}
			else
				++weight;
		}
		if(p >= 0)
			prog.insert(prog.begin() + p, cmd);
		else
			prog.push_back(cmd);	
	}
	
	void progmem::erase(int nm){
		if(prog[nm].get_operation().type() == OP_VAR) { // при удалении объявления переменной, структура статических переменных поменялась
			prog.erase(prog.begin()+nm);
			weight = 0;
			auto cend = prog.cend();
			for(auto cur = prog.cbegin(); cur != cend; ++cur)
			{
				if(cur->get_operation().type() == OP_VAR)
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
					base = prog.get_base();
				ram -> lock();	
				slot &tmp =  (*ram)[get_value(rm->value()) + base];
				ram -> unlock();	
				return tmp;
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
					base = prog.get_base();
				ram -> lock();	
				const slot &tmp = (*ram)[get_value(rm->value()) + base];
				ram -> unlock();		
				return tmp;
			}
			break;
			case OPR_REG:{	
				const reg_operand *rg = (const reg_operand*) &op;
				return read_reg(get_value(rg->value()));
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
				return prog.get_progmem().unname(lab->value()) + lab->get_offset() + (lab->is_unnamed()? prog.get_base() : 0);
			}
			break;
			case OPR_RAM:{
				const ram_operand *rm = (const ram_operand*) &op;
				int base = 0;
				if(rm->is_label())
					base = prog.get_base();
					
				ram -> lock();					
				byte tmp = (*ram)[get_value(rm->value()) + base].data;
				ram -> unlock();		
				return tmp;
			}
			break;
			case OPR_REG:{
				const reg_operand *rg = (const reg_operand*) &op;
				return read_reg(get_value(rg->value())).data;
			}
			break;
		}
	}
	
	void processor::run(const progmem &program)
	{
		prog = process(program);
		ram->lock();
		if(prog.get_progmem().get_weight()) prog.set_base(ram->alloc(prog.get_progmem().get_weight(), &prog));
		ram->unlock();
		//prog->set_name("end", prog->size()); // переход в конец программы
		while(control.on_tick(*this));
		auto end = al_end();
		for(auto al = al_begin(); al != end; ++al)
			while(al->is_running()) control.on_tick(*this);			
		if(prog.get_progmem().get_weight()){
			ram -> lock();
			ram -> free(prog.get_base(), &prog);
			ram -> unlock();
		}
	}
}