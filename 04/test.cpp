#include "processor/processor.h"

using namespace jpl;

#include "../test/testing.hpp"

struct test_TEST: TestCase {
  void run() override {
	  //проверка на вызовы исключений.
	  processor proc(ALU(1), 2);
    ASSERT_THROWS(std::logic_error, proc.get_slot(const_operand(0)));
    ASSERT_THROWS(std::logic_error, proc.get_slot(label_operand("lab")));
	{
		ALU al1(1), al2(1);
		reg_operand reg(const_operand(1));
		al1.lock(proc,reg);
		al2.lock(proc, reg);
		ASSERT_THROWS(std::logic_error, al2.unlock(proc, reg));	
		al1.unlock(proc,reg);	
	}
	{
		command cmd(operations::NONE, nullptr, {new const_operand(2)});
		ASSERT_THROWS(std::logic_error, cmd.get_operand(-1));
		ASSERT_THROWS(std::logic_error, cmd.get_operand(1));
		ASSERT_THROWS(std::logic_error, operations::NONE(proc, cmd, 10));
	}
	ASSERT_THROWS(std::logic_error, label_operand("Not Allowed String $_AS"));
	ASSERT_THROWS(std::logic_error, label_operand("1С"));
	ASSERT_THROWS(std::logic_error, label_operand("@@ds"));
	ASSERT_THROWS(std::logic_error, label_operand("lolololololol"));
  }
};

REGISTER_TEST(test_TEST, "Testing of exceptions");
