#ifndef JPL_MEMORY_H

#define JPL_MEMORY_H

#include "types.h"
#include <iostream>
#include <vector>

namespace jpl{
	
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
		
		friend std::ostream& operator<<(std::ostream&, const RAM&);
		
		void free(byte) = delete; // пока заглушка, но в будущем планирую разработать систему по-настоящему динамической памяти
	};
}
#endif