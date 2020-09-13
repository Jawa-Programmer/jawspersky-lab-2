#include <iostream>
#include "matrix.h"

/// ���� ��� <code> using namespace std; </code> ))))))))
using std::cout;
using std::cin;
using std::endl;

int main()
{
	setlocale(LC_ALL, "");
	cout << "������� ���������� ����� � �������: " << endl;
	int lines;
	cin >> lines;
	matrix mt(lines);
	for (int i=0; i<lines;i++)
	{
		cout << endl << "������� ���������� ����� � " << (i+1) << "� ������ �������: " <<endl;
		int lnsz;
		cin >> lnsz;
		mt[i] = new line(lnsz);
		cout << "������� " << lnsz << " ����� �����: " << endl;
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