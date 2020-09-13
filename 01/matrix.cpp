#include "matrix.h"
#include <iostream>

line::line(int sz)
{
	_size = sz;
	_arr = new int[_size];
}
line::~line()
{
	delete[] _arr;
}
int& line::operator[](const int i)
{
	return _arr[i];
}
void line::print()
{
	for(int i=0;i<_size;i++)
		std::cout << _arr[i] << ", ";
}
matrix::matrix(int sz)
{
	_size = sz;
	_arr = new line*[_size];
}
matrix::~matrix()
{
	for(int i=0;i<_size;i++)
	{
		delete _arr[i];		
	}
	delete [] _arr;
}
line*& matrix::operator[](const int i)
{
	return _arr[i];
}
void matrix::print()
{
	for(int i=0; i<_size;i++){
		std::cout << "["<<i<<"]:\t";
		_arr[i]->print();
		std::cout << std::endl;
	}
}