#ifndef JPL_OPERANDS_H
#define JPL_OPERANDS_H

#include <iostream>
#include "types.h"

/*!
	@file
	@brief Файл содержит описание операндов, которые могут использоваться для передачи параметров в командах
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

namespace jpl 
{
	/// абстрактный класс описателя операнда
	class operand
	{
		public:
		/// виртуальный деструктор. Нужен для наследования
		virtual ~operand() {}
		/*!
			@brief возвращает указатель на точную копию данного экземпляра
			@return указатель на компию
		*/
		virtual operand* copy() const = 0; 
		/*!
			@brief возвращает тип операнда
			@return тип операнда (OPERAND_TYPE)
		*/
		virtual OPERAND_TYPE type() const = 0;
		/*!
			@brief вывод в поток текстового представления операнда
			@param ссылка на поток
			@return ссылка на поток
		*/
		virtual std::ostream& print(std::ostream&) const = 0;
		};
	/// класс константного операнда
	class const_operand : public operand
	{
		private:
		byte val;
		public:
		/// инициализируется константным значением
		const_operand(byte val_) : val(val_) {}
		
		const_operand(const const_operand& old) = default;
		const_operand(const_operand&& old) = default;
		const_operand& operator=(const const_operand& old) = default;
		const_operand& operator=(const_operand&& old) = default;		
		/*!
			@brief возвращает значение операнда
			@return данные
		*/
		byte value() const {return val;}
		
		virtual OPERAND_TYPE type() const override {return OPR_CONST;}
		
		virtual const_operand* copy() const override {return new const_operand(val);}
		virtual std::ostream& print(std::ostream& out) const override { return out << val;}
	};
	/// класс операнда - указатель на регистр
	class reg_operand : public operand
	{
		private:
		/// операнд, в котором хранится номер регистра
		operand *adr; 
		public:
		/// @param adr_ операнд, в котором хранится номер регистра процессора
		reg_operand(const operand &adr_) : adr(adr_.copy()) {}
		~reg_operand(){	delete adr;}
		
		/// @return операнд, в котором хранится номер регистра процессора
		const operand& value() const {return *adr;}
		
		virtual OPERAND_TYPE type() const override {return OPR_REG;}
		
		virtual reg_operand* copy() const override {return new reg_operand(*adr);}
		virtual std::ostream& print(std::ostream&) const override;
	};		
	/// класс операнда метки. 
	/*!
		@details метки заносятся в словарь при сборке программы. Если метка означает метку перехода, то в словарь заносится инструкция, перед которой стоит метка. Если метка является именем переменной, то в метке хранится относительный адрес статической переменной в ОП.
	*/
	class label_operand : public operand
	{
		private:
		/// флаг того, что метка разыменовывается
		bool unnamed;
		/// смещение (синтаксис a+1)
		int offset;
		/// текстовая метка
		std::string label; 
		public:
		/*!
			@brief Конструктор. Проверяет соответсвие строки стандарту языка (от одного до 8 латинских букв)
			@throws std::logic_error если lab не соответсвует стандарту языка (от одного до 8 латинских букв)
			@param lab текстовая метка
			@param off числовое смещение (по умолчанию 0)
			@param unnamed флаг того, что метка разименована (при выводе отображается *lab вместо lab)
		*/
		label_operand(const std::string& lab, int off = 0, bool unnamed = false); 
		/// возвращает текстовую метку
		const std::string& value() const {return label;}
		/// возвращает смещение
		int get_offset() const {return offset;}
		
		virtual OPERAND_TYPE type() const override {return OPR_LABEL;}
		/// возвращает флаг unnamed
		bool is_unnamed() const {return unnamed;}
		
		virtual label_operand* copy() const override {return new label_operand(label, offset, unnamed);}
		
		virtual std::ostream& print(std::ostream&) const override;
	};
	/// класс операнда - указатель на ячейку в ОП
	class ram_operand : public operand
	{
		private:
		/// флаг, указывающий, что операнд следует интерпретировать как метку
		bool is_lab;
		/// этот операнд хранит адрес в памяти. Возможна последовательность вложенных операндов, что позволяет создавать ссылки и указатели.
		operand *adr;
		public:
		/// конструктор инициализирует операнд. 
		/// @param adr_ операнд, в котором хранится адрес ОП
		ram_operand(const operand &adr_) : adr(adr_.copy()), is_lab(false) {}
		/// конструктор инициализирует операнд. 
		/// @param adr_ метка, хранящая адрес статической переменной
		ram_operand(const label_operand &adr_);
		~ram_operand(){	delete adr;}
		
		/// @return операнд, храняций адрес данного операнда в ОП
		const operand& value() const {return *adr;}
		
		virtual OPERAND_TYPE type() const override {return OPR_RAM;}
		
		/// @return флаг is_lab
		bool is_label() const {return is_lab;}
		
		virtual operand* copy() const override;
		
		virtual std::ostream& print(std::ostream&) const override;
	};
}

#endif