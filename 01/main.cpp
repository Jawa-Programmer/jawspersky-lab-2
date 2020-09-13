#include <iostream>
#include "matrix.h"

/// Зато без <code> using namespace std; </code> ))))))))
using std::cout;
using std::cin;
using std::endl;

/*
* © Лабараторная работа №1. Вариант №14. Студент Фиреръ Анатолий (Б19-513)
*
* программа позволяет ввести непрямоугольную матрицу в следующем формате:
* <число строк n> <n строк вида 'm, a[1], a[2]... a[m]'>
*
* в ответ выводится сама матрица и вектор b, каждый iй элемент которого есть сумма
* удовлетворяющих предикату чисел iй строки.
* как и было указано в условии, есть возможность быстро сменить предикат, задающий условие.
* Это достигнуто путем того, что предикат задается ссылкой на функцию. Достатоно написать
* новую функцию в main и во время отладки менять передаваемую функцию, тем самым быстро
* меня предикат.
*/

/// предикат, используемый для отбора чисел, входящих в сумму.
bool pred(int a)
{
	return (a%2)==0;
} 

int main()
{
	setlocale(LC_ALL, "");
	cout << "Введите количество строк в матрице: " << endl;
	int lines;
	cin >> lines;
	matrix mt(lines);
	mt.set_comparator(pred);
	// ввод матрицы
	for (int i=0; i<lines;i++)
	{
		cout << endl << "Введите количество чисел в " << (i+1) << "й строке матрицы: " <<endl;
		int lnsz;
		cin >> lnsz;
		mt[i] = new line(lnsz);
		cout << "Введите " << lnsz << " целых чисел: " << endl;
		for (int j=0;j<lnsz;j++)
		{
			int tmp;
			cin >> tmp;
			(*mt[i])[j] = tmp;
		}
	}
	cout << "----------| МАТРИЦА  |--------------" << endl;
	mt.print();
	mt.calculate();
	cout << "----------| ВЕКТОР Б |--------------" << endl;
	mt.print_b();
	return 0;
}