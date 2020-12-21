#ifndef JPL_COMMAND_H
#define JPL_COMMAND_H

#include <vector>

#include "types.h"
#include "operands.h"

/*!
	@file
	@brief Файл содержит описание описателя команды
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

namespace jpl
{

	/// @brief класс команды процессора.
	class command
	{
		private:
		/// @brief тип операции, которую данная команда исполнит
		const operation *op;
		/// @brief метка команды. Используется при выводе команды в поток в виде текста. Соответсвует метке перехода
		std::string label;
		/// @brief список аргументов команды.
		std::vector<operand*> args;
		/// @brief указатель на функтор, который выполняет математическую операцию
		const operator_ *act;
		public:
		/// @brief конструктор по-умолчанию. необходим для возможности работы с vector. Попытка выполнить команду, созданную этим конструктором, приведет к выбрасыванию исключения std::logic_error
		command();
		
		/*!	
			@brief	конструктор для команды без операндов
			@param	op_ ссылка на класс операции. Основные операции вынесены в файл operations.h, пользовательские операции так же поддерживаются.
			@param	str текстовая метка команды. Используется при выводе команды в поток. Может быть nullptr. 
			@param	act указатель на оператор. Оператор выполняет математическую операцию на АЛУ. Если команда адресована к УУ, то этот параметр должен быть nullptr (по-умолчанию)
		*/
		command(const operation &op_, const char* str, const operator_ *act_ = nullptr);
		
		/*!	
			@brief	конструктор для команды со списком операндов
			@param	op_ ссылка на класс операции. Основные операции вынесены в файл operations.h, пользовательские операции так же поддерживаются.
			@param	str текстовая метка команды. Используется при выводе команды в поток. Может быть nullptr. 
			@param	_args список указателей на операнды. ВАЖНО! следует использовать синтаксис {new operand(), ...} так как класс команды не создает копию объекта по ссылке. Все операнды удаляются в деструкторе
			@param	act указатель на оператор. Оператор выполняет математическую операцию на АЛУ. Если команда адресована к УУ, то этот параметр должен быть nullptr (по-умолчанию)
		*/
		command(const operation &op_, const char* str, const std::initializer_list<operand*> &_args, const operator_ *act_ = nullptr);
		/*!	
			@brief	копирующий конструктор. Операнды так же копируюстя
			@param	cmd команда, с которой необходимо снять копию
		*/
		command(const command& cmd);
		/*!
			@brief	перемещающий конструктор. 
			@param	cmd команда, ресурсы которой необходимо перенести
		*/
		command(command&& cmd);
		/*!
			@brief	копирующее присваивание
			@param	cmd команда, с которой необходимо снять копию
			@return	ссылка на себя
		*/
		command& operator=(const command&);
		/*!
			@brief	перемещающие присваивание. 
			@param	cmd команда, ресурсы которой необходимо перенести
			@return	ссылка на себя
		*/
		command& operator=(command&&);
		/// @brief	деструктор. Уничтожает объекты операндов, переданные в конструкторе
		~command();
		
		/*!
			@brief	выполняет команду с использованием ресурсов указанного процессора, считая что в данный момент переданное системное время (в тиках)
			@param	proc ссылка на процессор, на котором небходимо выполнить данную команду
			@param	time показание регистра таймера процессора. Необходимо для того, что бы была возможность симулировать длительность выполнения операций
		*/
		void execute(processor &proc, long time) const;
		/*!
			@brief	заменить операцию на переданную
			@param	op_ новая операция
		*/
		void set_operation(const operation &op_);
		/*!
			@brief	заменить оператор на переданный
			@param	act_ новый оператор
		*/
		void set_operator(const operator_ *act_){act = act_;}		
		/*!
			@brief	заменить метку на переданную
			@param	lab новая метка
		*/
		void set_label(const std::string &lab) {label = lab;}
		/*!
			@brief	добавить операнд в список операндов
			@param	arg операнд для добавления
		*/
		void add_operand(const operand& arg);
		/*!
			@brief	удалить операнд из списка операндов
			@param	num номер операнда (нумерация начинается с 0)
		*/
		void remove_operand(int num);
		
		/*!
			@brief	возвращает константную ссылку на операцию
			@return	константная ссылка на операцию
		*/
		const operation& get_operation() const {return *op;}
		
		/*!
			@brief	возвращает константный указатель на оператор
			@return	константный указатель на оператор
		*/
		const operator_* get_operator() const {return act;}
		/*!
			@brief	возвращает текстовое представление метки
			@return	строка метка
		*/
		const std::string& get_label() const {return label;}		
		/*!
			@brief	возвращает операнд по его номеру
			@param	num номер операнда (нумерация с 0)
			@return	константный указатель на операнд
		*/
		const operand* get_operand(int num) const {if(num < 0|| num >= args.size())	throw std::logic_error("out bounds"); return args[num];}	
		/*!
			@brief	возвращает количество операндов у данного экземпляра команды
			@return	количество операндов
		*/		
		int get_args_count() const {return args.size();}
		/*!
			@brief	выводит текстовое представление данной команды в указанный поток. Перевод строки не осуществляется
			@param	out	ссылка на поток
			@param	cmd	константная ссылка на команду, которую необходимо вывести в поток
			@return	ссылка на переданный поток
		*/	
		friend std::ostream& operator<<(std::ostream &out, const command& cmd);
	};	
}

#endif