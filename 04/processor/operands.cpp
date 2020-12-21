#include "operands.h"

namespace jpl{
	///--- reg_operand ---///
	std::ostream& reg_operand::print(std::ostream& out) const
	{
		adr->print(out << "(");
		return out;
	}
	
	///--- label_operand ---///
	
	label_operand::label_operand(const std::string &lab, int off, bool unnamed_) : offset(off), unnamed(unnamed_)
	{
		if(!is_valid_name(lab)) throw std::logic_error("incorrect var name");
		label = lab;
	}
	
	std::ostream& label_operand::print(std::ostream& out) const
	{
		if(unnamed) out << "*";
		if(offset)
			out << label << "+" << offset;
		else
			out << label;
		return out;
	}
	
	///--- ram_operand ---///
	ram_operand::ram_operand(const label_operand &adr_) : adr(adr_.copy()), is_lab(true) {}
	
	operand* ram_operand::copy() const {ram_operand *tmp = new ram_operand(*adr); tmp->is_lab = is_lab; return tmp;}
	
	std::ostream& ram_operand::print(std::ostream& out) const
	{
		if(!is_lab) out << "[";
		adr->print(out);
		return out;
	}
}