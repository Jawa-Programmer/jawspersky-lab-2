


#include "cycloid.h"

using namespace geometric;

#include "../test/testing.hpp"

struct test_TEST: TestCase {
  void run() override {
	  //проверка конструктора на вызов ошибки при отрицательном радиусе
    ASSERT_THROWS(IncorrectRadiusException, cycloid(-1));
    ASSERT_THROWS(IncorrectRadiusException, cycloid(1));
	cycloid cy(1);
	// проверка метода coords
	{
		point et = {3.142, 1.999};
		point res = cy.coords(3.142);
		ASSERT_RELATION((et-res).length(), <=, 0.01);
	}
	// проверка метода curvature
	{
		double et = -4, res = cy.curvature(3.142);
		ASSERT_ALMOST_EQUAL(et, res, 0.01);
	}
	// проверка метода length (два теста: с положительным и отрицательным углом)
	{
		double et = 4, res = cy.length(3.142);
		ASSERT_ALMOST_EQUAL(et, res, 0.01);
	}
	{
		double et = 4, res = cy.length(-3.142);
		ASSERT_ALMOST_EQUAL(et, res, 0.01);

	}
	// проверка метода area
	{
		double et = 3.142*3, res = cy.area();
		ASSERT_ALMOST_EQUAL(et, res, 0.01);
	}
	// проверка метода surface_area
	{
		double et = 64*3.142/3, res = cy.surface_area();
		ASSERT_ALMOST_EQUAL(et, res, 0.01);
	}
	// проверка метода volume
	{
		double et = 5*3.14159*3.14159, res = cy.volume();
		ASSERT_ALMOST_EQUAL(et, res, 0.01);
	}
  }
};

REGISTER_TEST(test_TEST, "Arithmetic operations");
