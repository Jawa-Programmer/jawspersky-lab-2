#ifndef JPL_OPERATORS_H
#define JPL_OPERATORS_H

#include "types.h"

#include <iostream>
#include <initializer_list>


namespace jpl {
	namespace operators {
		
		class base 
		{
			public:
			virtual std::ostream& print(std::ostream&) const = 0; // выводит в поток текстовое название оператора (совпадает с названием константы безымянного класса)
			virtual byte operator()(const std::initializer_list<byte>&) const = 0; // передается список фактических значений, возвращается результат выполнения оператора.
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
		} ADD;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SUB";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg - *(beg+1);
				}
		} SUB;
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "DIV";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg / *(beg+1);
				}
		} DIV;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "MOD";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg % *(beg+1);
				}
		} MOD;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "MUL";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg * *(beg+1);
				}
		} MUL;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "INC";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg + 1;
				}
		} INC;
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "DEC";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg - 1;
				}
		} DEC;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "SET";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *(beg+1);
				}
		} SET;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "ADD";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return - *beg;
				}
		} INV;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "AND";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg && *(beg+1);
				}
		} AND;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "OR";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return *beg || *(beg+1);
				}
		} OR;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "XOR";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 2) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return ((bool)*beg) != ((bool)*(beg+1));
				}
		} XOR;
		
		const class : public base 
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "NOT";}
			virtual byte operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				const byte *beg = args.begin();
				return !*beg;
				}
		} NOT;
		
	}
}
#endif