#ifndef JPL_MEMORY_H

#define JPL_MEMORY_H

#include <mutex>

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
		
		int get_size() const {return size;}
	};
	class RAM
	{
		private:		
		std::recursive_mutex locker; // блокирует шину обращения к ОП
		struct block {byte adr; size_t len; const process *owner;};
		std::vector<slot> ram;
		std::vector<block> blocks; // управление выделенными блоками памяти.  
		public:
		// определять свои конструкторы не надо, так как поля сами заботятся о своем копировании и уничтожении
		
		slot& operator[](const int &adr_) {return ram[adr_];} // так будет проще. Сама память не следит за правами доступа, это делает ALU
		const slot& operator[](const int &adr_) const { return ram[adr_]; } // так будет проще. Сама память не следит за правами доступа, это делает ALU
		
		byte alloc(size_t, const process *); // выделяет новую память из "кучи" и передает указатель на начало блока
		
		void free(byte, const process *); // получает указатель на начало блока. Возвращает его в "кучу"
		
		/// блокирует доступ к ОП для других потоков (рекурсивный мьютекс)
		void lock() {locker.lock();}
		/// снимает блокировку доступа к ОП для других потоков (рекурсивный мьютекс)
		void unlock() {locker.unlock();} 
		
		friend std::ostream& operator<<(std::ostream&, const RAM&);
		
	};
}
#endif