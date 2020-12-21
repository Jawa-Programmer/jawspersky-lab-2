#ifndef JPL_MEMORY_H

#define JPL_MEMORY_H

#include <mutex>

#include "types.h"
#include <iostream>
#include <vector>


/*!
	@file
	@brief Файл содержит описание классов ОП и блока регистров
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

namespace jpl{
	/// класс блока регистров процессора. 
	class registers_block
	{
		private:
		/// указатель на массив ячеек памяти
		slot *regs;
		/// размер блока регистров
		size_t size;
		public:
		/*! 
			@brief конструктор, инициализирующий блок регистров указанного размера
			@param sz количество регистров в блоке регистров.
		*/
		registers_block(size_t sz);
		/*! 
			@brief копирующий конструктор
			@param old экземпляр для снятия копии
		*/
		registers_block(const registers_block& old);
		/*! 
			@brief перемещающий конструктор
			@param донор
		*/
		registers_block(registers_block&&);
		/*! 
			@brief деструктор. Высвобождает память
		*/
		~registers_block();
		/*!
			@brief оператор доступа к регистру из блока регистров по его номеру.
			@param adr_ номер регистра
		*/
		slot& operator[](const int &adr_);
		/*!
			@brief оператор доступа только для чтения к регистру из блока регистров по его номеру.
			@param adr_ номер регистра
		*/
		const slot& operator[](const int &adr_) const; 
		/*!
			@brief возвращает размер блока регистров
			@return размер блока регистров
		*/
		int get_size() const {return size;}
	};
	/// класс оперативной памяти
	class RAM
	{
		private:
		std::mutex locker; ///< мьютекс для блокировки шины памяти
		std::mutex c_locker; ///< мьютекс для работы со счетчиком чтения
		int mx_c = 0; ///< счетчик чтения (позволяет организовать паралельное чтение из ОП, но запрещая одновременное чтение и запись)
		/// структура описателя блока (участка) памяти
		struct block {
			/// адрес блока памяти
			byte adr; 
			/// длина блока памяти
			size_t len; 
			/// указатель на процесс-владелец данного блока данных
			const process *owner;
			};
		/// вектор ячеек памяти. "куча"
		std::vector<slot> ram;
		/// вектор блоков памяти
		std::vector<block> blocks; 
		public:		
		RAM(): mx_c(0) {}		
		/*!
			@brief оператор доступа только для чтения к регистру из блока регистров по его номеру.
			@param adr_ номер регистра
		*/		
		slot& operator[](const int &adr_) {return ram[adr_];} 		
		/*!
			@brief оператор доступа только для чтения к регистру из блока регистров по его номеру.
			@param adr_ номер регистра
		*/
		const slot& operator[](const int &adr_) const { return ram[adr_]; }		
		/*!
			@brief выделение блока памяти из кучи.
			@param sz размер выделяемого блока
			@param lc указатель на процесс-владелец (может быть nullptr)
		*/
		byte alloc(size_t sz, const process *lc); 
		/*!
			@brief возвращение в кучу блока памяти по указанному адресу
			@param adr адрес блока для освобождения
			@param lc указатель на процесс-владелец (может быть nullptr)
		*/
		void free(byte adr, const process *ls); 
		
		/// блокирует возможность читать/писать из других потоков (мьютекс)
		void lock_write() {locker.lock();}
		/// разблокирует возможность читать/писать из других потоков (мьютекс)
		void unlock_write() {locker.unlock();} 
		/// блокирует возможность писать из других потоков, но оставляет возможность читать(мьютекс)
		void lock_read() {
			c_locker.lock();
			if(mx_c == 0)
				locker.lock();
			++mx_c;
			c_locker.unlock();
			}
		/// отменяет действие lock_read
		void unlock_read() {
			c_locker.lock();
			--mx_c;
			if(mx_c == 0)
				locker.unlock();
			c_locker.unlock();
			} 
		
		/*!
			@brief вывод состояния ОП в указанный поток
			@param out адрес блока для освобождения		
			@param out ссылка на ОП для вывода
			@return ссылка на переданный поток
		*/
		friend std::ostream& operator<<(std::ostream& out, const RAM& ram);
		
	};
}
#endif