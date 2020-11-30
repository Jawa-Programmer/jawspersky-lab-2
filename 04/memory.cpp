#include "memory.h"

namespace jpl{
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
}