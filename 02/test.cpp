/**
* фреймворк модульного тестирования лабараторных работ
* ©Jawa Testing Enterprises, 2020 все права украдены. 
*/
#include <iostream>
#include <random>
#include <math.h>

#include "cycloid.h"

/// количество тестов для каждого модуля
#define TESTS 10
#define SEP	std::cout << "--------------" << std::endl

using std::cout;
using std::cerr;
using std::endl;
using namespace geometric;

std::random_device rd; 
std::mt19937 rnd(rd());


double rand(double begin, double end)
{
	if (end < begin) throw "end must be bigger than begin";
	return  rnd()/4294967296.0*(end-begin) + begin;
}

int main()
{
	int tests = 0, passed = 0;
	/// тест конструктора. Он обязан вызвать исключение типа IncorrectRadiusException на отрицательный результат, но ноль и положительный результат ошибки вызвать не должны.
	cout << "case #1. testing constructor" << endl;
	/// первая попытка
	{
		SEP;
		cout << "test #1. ";
		tests++;
		double R = rand(-10000, -1), Ph = rand(-100,100);
		cout << "R = " << R << ", Ph = "<< Ph << ". We wait exception" << endl;
		cycloid* cy;
		try{
			cy = new cycloid(R, Ph);
			cerr << "case #1. test #1 failed"<< endl;
			delete cy;	
		}
		catch(IncorrectRadiusException &ex)
		{			
			cout << "case #1. test #1 passed."<< endl;
			cout << "ex msg: " << ex.what() << endl;
			passed++;
		}	
	}
	/// вторая попытка
	{
		SEP;
		cout << "test #2. ";
		tests++;
		double R = 0;		
		cout << "R = " << R << ", Ph by default. We didn't wait exception" << endl;
		cycloid* cy;
		try{
			cy = new cycloid(R);
			cout << "case #1. test #2 passed"<< endl;
			passed++;
			delete cy;	
		}
		catch(IncorrectRadiusException ex)
		{			
			cerr << "case #1. test #2 failed"<< endl;
			cout << "ex msg: " << ex.what() << endl;
		}	
	}
	/// третья попытка
	{
		SEP;
		cout << "test #3" << endl;
		tests++;
		double R = rand(1, 10000);
		cout << "R = " << R << ", Ph by default. We didn't wait exception" << endl;
		cycloid* cy;
		try{
			cy = new cycloid(R);
			cout << "case #1. test #3 passed"<< endl;
			passed++;
			delete cy;	
		}
		catch(IncorrectRadiusException ex)
		{			
			cerr << "case #1. test #3 failed"<< endl;
			cout << "ex msg: " << ex.what() << endl;
		}	
	}
	double PHASE = rand(-100, 100), RADIUS = rand(0, 1000);
	SEP;
	cout << "then we test object of class cycloid with params: Ph = " << PHASE << ", Radius = " << RADIUS << "." << endl;
	SEP;
	cycloid cy(RADIUS, PHASE);
	cout << "case #2. testing function coords. Error must be less than 0,0000001" << endl;
	///проверка метода coords. Результат не должен отличатся от эталона больше миллионной.
	for (int i=1; i <= TESTS; i++)
	{
		SEP;
		double a = rand(-1000,1000);
		cout << "test #"<< i <<". Angle = " << a << "." <<endl;
		tests++;
		point etalon = {RADIUS*(a-PHASE-sin(a-PHASE)),RADIUS*(1-cos(a-PHASE))};
		point result = cy.coords(a);
		if((etalon-result).length() < 0.0000001) 
		{
			cout << "test #" <<i<<" passed"<<endl;
			passed++;
		}
		else 
			cerr << "test #" <<i<<"failed" <<endl;
		cout << "waited result: X = " <<etalon.X<<", Y = "<< etalon.Y <<endl;
		cout << "real result: X = " <<result.X<<", Y = " <<result.Y <<endl;
	}
	
	
	
	SEP;
	cout << "tests passed / total: " << passed << "/" << tests << " (" << 100.0*passed/tests<<"%)"<<endl;
	return 0;
}