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
	byte RAM::alloc(size_t s, const process * own)
	{
		if(s < 1) throw std::logic_error("memory block size must be greater then zero");
		byte ret = 0;
		auto beg = blocks.cbegin();
		for(; beg != blocks.cend(); ++beg)
		{
			if(beg -> adr >= ret + s) break;
			ret = beg -> adr + beg -> len;
		}
		blocks.insert(beg, {ret, s, own});
		if(ret+s > ram.size()) 
			ram.resize(ret + s);
		return ret;
	}
	
	void RAM::free(byte adr, const process *own)
	{
		auto beg = blocks.cbegin();
		for(; beg != blocks.cend() && beg -> adr != adr; ++beg); // можно было бы и дехотомию запилить, но мне лень :)
		if(beg == blocks.cend()) throw std::logic_error("block with this addres not exist");
		if(beg -> owner != own) throw std::logic_error("this processor not owns this block");
		blocks.erase(beg);
	}
	
	std::ostream& operator<<(std::ostream& out, const RAM& ram)
	{
		auto end = ram.ram.cend();
		auto blk = ram.blocks.cbegin();
		int i = 0;
		for(auto cur = ram.ram.cbegin(); cur != end; ++cur){
			bool end = (blk->adr + blk->len-1) == i;
			if(blk->adr == i){ out << blk->owner << ":{" <<std::endl; ++blk;}
			out << "[0x" << std::hex << i++ <<  "]:\t" << std::dec << (*cur).data;
			if(end) out << "}";
			out << std::endl;
			}
		return out;
	}
}