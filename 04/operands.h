#ifndef JPL_OPERANDS_H
#define JPL_OPERANDS_H

#include <iostream>
#include "types.h"

namespace jpl 
{
	
	class operand
	{
		public:
		virtual ~operand() {}
		virtual operand* copy() const = 0; // возвращает указатель на копию объекта операнда. Пользователь должен сам удалить его после использования! Что касается других классов библиотеки, они ганатированно вызывают delete после каждого вызова clone()
		virtual OPERAND_TYPE type() const = 0;
		/// вывод в поток текстового представления операнда
		virtual std::ostream& print(std::ostream&) const = 0;
		};
	
	class const_operand : public operand
	{
		private:
		byte val;
		public:
		const_operand(byte val_) : val(val_) {}
		
		const_operand(const const_operand& old) = default;
		const_operand(const_operand&& old) = default;
		const_operand& operator=(const const_operand& old) = default;
		const_operand& operator=(const_operand&& old) = default;
		
		byte value() const {return val;}
		
		virtual OPERAND_TYPE type() const override {return OPR_CONST;}
		
		virtual const_operand* copy() const override {return new const_operand(val);}
		virtual std::ostream& print(std::ostream& out) const override { return out << val;}
	};
	
	class reg_operand : public operand
	{
		private:
		operand *adr; // этот операнд хранит номер регистра. Возможна последовательность вложенных операндов, что позволяет создавать ссылки.
		public:
		reg_operand(const operand &adr_) : adr(adr_.copy()) {}
		~reg_operand(){	delete adr;}
		
		const operand& value() const {return *adr;}
		
		virtual OPERAND_TYPE type() const override {return OPR_REG;}
		
		virtual reg_operand* copy() const override {return new reg_operand(*adr);}
		virtual std::ostream& print(std::ostream&) const override;
	};		
	class label_operand : public operand
	{
		private:
		bool unnamed;
		int offset;
		std::string label; // этот операнд хранит число. используется для более легкого перехода в программе
		public:
		
		label_operand(const std::string& lab, int off = 0, bool unnamed = false); // проверяет соответсвие строки стандарту языка (от одного до 8 латинских букв) Если содержит неподходящие символы или имеет неправильную длинну, то выбрасывается исключение
		const std::string& value() const {return label;}
		int get_offset() const {return offset;}
		
		virtual OPERAND_TYPE type() const override {return OPR_LABEL;}
		
		bool is_unnamed() const {return unnamed;}
		
		virtual label_operand* copy() const override {return new label_operand(label, offset, unnamed);}
		
		virtual std::ostream& print(std::ostream&) const override;
	};
	
	class ram_operand : public operand
	{
		private:
		bool is_lab;
		operand *adr; // этот операнд хранит адрес в памяти. Возможна последовательность вложенных операндов, что позволяет создавать ссылки и указатели.
		public:
		ram_operand(const operand &adr_) : adr(adr_.copy()), is_lab(false) {}
		ram_operand(const label_operand &adr_);
		~ram_operand(){	delete adr;}
		
		const operand& value() const {return *adr;}
		
		virtual OPERAND_TYPE type() const override {return OPR_RAM;}
		
		bool is_label() const {return is_lab;}
		
		virtual operand* copy() const override;
		
		virtual std::ostream& print(std::ostream&) const override;
	};
}

#endif