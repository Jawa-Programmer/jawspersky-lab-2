#include <iostream>
#include "matrix.h"

/// Зато без <code> using namespace std; </code> ))))))))
using std::cout;
using std::cin;
using std::endl;

int main()
{
	setlocale(LC_ALL, "");
	cout << "Введите количество строк в матрице: " << endl;
	int lines;
	cin >> lines;
	matrix mt(lines);
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
	cout<<"------------------------"<<endl;
	mt.print();
	return 0;
}