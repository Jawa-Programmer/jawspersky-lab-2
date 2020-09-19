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

int tests = 0, passed = 0;

/// тест конструктора. Он обязан вызвать исключение типа IncorrectRadiusException на отрицательный результат, но ноль и положительный результат ошибки вызвать не должны.
void case_1()
{
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
}
/// циклоида без начального смещения и с единичным радиусом
cycloid cy(1, 0);

///проверка метода coords. Результат не должен отличатся от эталона больше одной сотой, так как при предварительных рассчетах использовалось значение числа pi = 3,142.
/// для покрытия модуля достаточно двух тестов, потому что одно число может случайно совпасть, но для двух тестов совпадение исключено (хотя если не быть параноиком, то тут хватит и одного теста, так как метод не содержит ветвлений)))
void case_2(){
	SEP;		
	cout << "case #2. testing method coords(). Error must be less than 0,01" << endl;	
	{
		cout << "test #1. Ph = 3.142 (PI)" << endl;
		tests ++;
		point et = {3.142, 1.999};
		point res = cy.coords(3.142);
		if((et-res).length()>0.01)
		{
			cerr << "case #2 test #1 failed." << endl << 
			"We waited X = " << et.X <<" Y = " << et.Y<< endl <<
			"but recived X = " << res.X <<" Y = "<<res.Y <<endl;
		}
		else 
		{
			cout << "case #2 test #1 passed." <<endl;
			passed++;			
		}
	}
	{
		cout << "test #2. Ph = 321" << endl;
		tests ++;
		point et = {320.471, 0.151};
		point res = cy.coords(321);
		if((et-res).length()>0.01)
		{
			cerr << "case #2 test #1 failed." << endl << 
			"We waited X = " << et.X <<" Y = " << et.Y<< endl <<
			"but recived X = " << res.X <<" Y = "<<res.Y <<endl;
		}
		else 
		{
			cout << "case #2 test #1 passed." <<endl;
			passed++;			
		}
	}
	
	
}
/// тест метода, возвращающего радиуса кривизны. Так как она отображает множество всех действительных чисел на множество от 0 до -4*r достаточно одного теста, показывающего, что радиус кривизны рассчитанный вручную совпал с радиусом кривизны из данной метода.
void case_3(){
	SEP;
	cout << "case #3. testing method curvature(). Error must be less than 0,01" << endl;	
	{
		tests++;
		cout << "test #1. Ph = 3.142" << endl;
		double et = -4, res = cy.curvature(3.142);
		if(abs(et-res)>0.01)
		{
			cerr << "case #3 test #1 failed." << endl << 
			"We waited R = " << et << endl <<
			"but recived R = " << res << endl;
		}
		else
		{
			cout << "case #3 test #1 passed." <<endl;
			passed++;		
		}
	}
}
/// проверка метода, возвращающей длинну кривой от смещения до указанного угла
/// возможны два случая - переданный угол больше начального смещения и меньше начадьного смещения.
/// на каждый случай по одному тесту
void case_4()
{
	SEP;
	cout << "case #4. testing method length(). Error must be less than 0,01" << endl;
	{
		tests++;
		cout << "test #1. Ph = 3.142" << endl;
		double et = 4, res = cy.length(3.142);
		if(abs(et-res)>0.01)
		{
			cerr << "case #3 test #1 failed." << endl << 
			"We waited R = " << et << endl <<
			"but recived R = " << res << endl;
		}
		else
		{
			cout << "case #3 test #1 passed." <<endl;
			passed++;		
		}
	}
	{
		tests++;
		cout << "test #2. Ph = -3.142" << endl;
		double et = 4, res = cy.length(-3.142);
		if(abs(et-res)>0.01)
		{
			cerr << "case #3 test #2 failed." << endl << 
			"We waited R = " << et << endl <<
			"but recived R = " << res << endl;
		}
		else
		{
			cout << "case #3 test #2 passed." <<endl;
			passed++;		
		}
	}
}

/// проверка метода, возвращающего площадь одной арки циклоиды.
/// метод не содержит ветвлений и покрывается одним тестом
void case_5(){
	SEP;
	cout << "case #5. testing method area(). Error must be less than 0,01" << endl;	
	{
		tests++;
		cout << "test #1. Ph = 3.142" << endl;
		double et = 3.142*3, res = cy.area();
		if(abs(et-res)>0.01)
		{
			cerr << "case #3 test #1 failed." << endl << 
			"We waited R = " << et << endl <<
			"but recived R = " << res << endl;
		}
		else
		{
			cout << "case #5 test #1 passed." <<endl;
			passed++;		
		}
	}
}

int main()
{
	
	//case_1();
	//case_2();
	//case_3();
	//case_4();
	case_5();
	
	/*for (int i=1; i <= TESTS; i++)
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
	}*/
	
	
	
	
	SEP;
	cout << "tests (passed/total): " << passed << "/" << tests << " (" << 100.0*passed/tests<<"%)"<<endl;
	return 0;
}