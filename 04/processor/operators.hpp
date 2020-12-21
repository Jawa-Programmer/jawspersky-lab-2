#ifndef JPL_OPERATORS_H
#define JPL_OPERATORS_H

#include "types.h"

#include <iostream>
#include <initializer_list>

/*!
	@file
	@brief Файл содержит описание базовых математических операторов, которые можно использовать в АЛУ при проектировании эмулятора процессора
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

namespace jpl {
	
	/// пространство имён содержит базовые математические операторы.
	namespace operators {
		/// базовый класс математического оператора. Является функтором. Любой поддерживаемый АЛУ оператор является экземпляром потомка данного класса
		class base 
		{
			public:
			/// выводит в поток текстовое название оператора
			virtual std::ostream& print(std::ostream&) const = 0; 
			/*! 
				@brief производит вычисления над переданными аргументами, возвращает результат вычислений.
				@param args список аргументов
				@return результат вычисления
			*/
			virtual byte operator()(const std::initializer_list<byte>& args) const = 0; 
		};
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "ADD";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg + *(beg+1);
				}
		} ADD; ///< оператор возвращает сумму двух аргументов
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg - *(beg+1);
				}
		} SUB; ///< оператор возвращает разность двух аргументов
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "DIV";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg / *(beg+1);
				}
		} DIV; ///< оператор возвращает частное двух аргументов
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "MOD";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg % *(beg+1);
				}
		} MOD; ///< оператор возвращает остаток от деления первого аргумента на второй
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "MUL";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg * *(beg+1);
				}
		} MUL; ///< оператор возвращает произведение двух аргументов
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "INC";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg + 1;
				}
		} INC; ///< оператор возвращает аргумент, увеличенный на единицу
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "DEC";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg - 1;
				}
		} DEC; ///< оператор возвращает аргумент, меньшеный на единицу
		
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "ADD";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return - *beg;
				}
		} INV; ///< оператор возвращает аргумент, умноженный на минус один
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "EQL";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return (*beg) == (*(beg+1));
				}
		} EQL; ///< оператор равенства. Возвращает 1, если оба операнда равны
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "NEQL";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return (*beg) != (*(beg+1));
				}
		} NEQL; ///< тоже, что и NOT EQL, но выполняется за одно обращение к АЛУ
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "AND";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg && *(beg+1);
				}
		} AND; ///< оператор возвращает результат применения логического И к аргументам (не путать с побитовым И)
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "OR";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg || *(beg+1);
				}
		} OR; ///< оператор возвращает результат применения логического ИЛИ к аргументам (не путать с побитовым ИЛИ)
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "XOR";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return ((bool)*beg) != ((bool)*(beg+1));
				}
		} XOR; ///< оператор возвращает результат применения логического ИСКЛЮЧАЮЩЕГО-ИЛИ к аргументам (не путать с побитовым ИСКЛЮЧАЮЩИМ-ИЛИ)
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "NOT";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return !*beg;
				}
		} NOT; ///< оператор возвращает результат применения логического НЕ к аргументу (не путать с побитовым НЕ)
		
	}
}
#endif