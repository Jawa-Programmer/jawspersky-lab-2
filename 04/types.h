#ifndef JPL_TYPES_H
#define JPL_TYPES_H

#include <string>

namespace jpl
{
	bool is_valid_name(const std::string& lab);
	
	// перечисления
	enum OPERATION_TYPE {OP_ALLC, OP_JUMP, OP_UNARY, OP_BINARY, OP_VAR, OP_FREE, OP_NONE};
	enum OPERAND_TYPE {OPR_CONST, OPR_LABEL, OPR_REG, OPR_RAM};
	enum EXECUTOR {E_CONTROLLER, E_ALU};
	
	
	typedef uint16_t byte; // минимальноадресуемой еденицей памяти будет 16 бит
	
	// прототипы классов
	/// класс команды. Принемается УУ или ИУ и исполняется.
	class command;
	/// класс операнда (константа, ссылка на регистр, ссылка на память)
	class operand;
	/// класс устройства управления. Выбирает подходящего исполнителя команды и передает на исполнение
	class controller;
	/// класс исполнительного устройства. Имеет список доступных команд, время исполнение команды.
	class ALU;
	/// класс блока регистров фиксированной длинны
	class registers_block;
	/// класс памяти данных (считается бесконечной, но ссылать можно только на предварительно выделенные учаски
	class RAM;
	/// класс памяти данных. набор команд и счетчик. содержит словарь соответсвия "имя - адрес"
	class progmem;
	
	/// класс процессор. Связывает между собой УУ, ИУ, память и регистры
	class processor;
	/// описатель процесса. Содержит ссылку на память программы, счётчик команд, адрес статически выделенной памяти памяти
	class process;
	/// ячейка памяти. Содержит данные и информацию о блокировавшем устойстве
	struct slot {const ALU *locker = nullptr; byte data = 0;};
	
	/// класс операции. (унарная, бинарная, переход, инициализация памяти, выделение памяти)
	namespace operations {class base;}
	typedef operations::base operation;
	
	/// класс оператора (ADD SUB MUL DIV и тд)
	namespace operators {class base;}
	typedef operators::base operator_;
	
}

#endif