#ifndef JPL_TYPES_H
#define JPL_TYPES_H

#include <string>


/*!
	@file
	@brief Файл содержит предварительные определения всех типов библиотеки
	@author Jawa Prog
	@version 1.0
	@date Декабрь 2020 года
*/

/// пространство имен Jawa Programmer Library
namespace jpl
{
	/*!
		@brief проверяет, соответсвует ли переданная строка формату имени переменной (содержит только буквы латинского алфавита и цифры. начинается с буквы)
		@param  lab строка для проверки
		@return результат проверки. true если строка соответсвуют формату имени переменной, иначе - false
	*/
	bool is_valid_name(const std::string& lab);
	
	/// @brief тип операции. используется для внутреннего пользования. При создании своего типа операции, следует использовать OP_EXTER
	enum OPERATION_TYPE {
		OP_VAR, ///< операция статического выделения. Обрабатывается обособленно, на этапе компиляции
		OP_EXTER, ///< тип обычной операции.
		OP_NONE ///< тип обстрактной операции, генерируемый command конструктором по умолчанию. Не должен быть вызван для исполнея.
		};
	/// @brief устройство, на котором выполняется операция
	enum EXECUTOR {
		E_CONTROLLER, ///< операция исполняется сразу на УУ
		E_ALU ///< в процессе выполнения операции УУ вызывает АЛУ для исполнения некоторой части работы
		};
	/// тип операнда
	enum OPERAND_TYPE {
		OPR_CONST, ///< константа
		OPR_LABEL, ///< переменная (метка)
		OPR_REG, ///< данные в регистре процессора
		OPR_RAM ///< данные в ОП
		};
	
	/// минимальноадресуемая единица памяти в структуре процессора.
	typedef uint16_t byte; 
	
	// прототипы классов
	
	class command;
	
	class operand;
	
	class controller;
	
	class ALU;
	
	class registers_block;
	
	class RAM;
	
	class progmem;
	
	
	class processor;
	
	class process;
	
	struct slot {const ALU *locker = nullptr; byte data = 0;};
	
	namespace operations {class base;}
	
	typedef operations::base operation;
	
	namespace operators {class base;}
	
	typedef operators::base operator_;
	
}

#endif