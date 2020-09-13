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
	_b = new int[sz];
}
matrix::~matrix()
{
	for(int i=0;i<_size;i++)
	{
		delete _arr[i];		
	}
	delete [] _arr;
	delete [] _b;
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
void matrix::set_comparator(bool (* pred)(int) )
{
	_comparator = pred;	
}
void matrix::calculate()
{
	for (int i=0;i<_size;i++)
	{
		_b[i] = 0;
		int sz = _arr[i]->size();
		for(int j = 0; j<sz;j++){
			if(_comparator( (*_arr[i])[j]))
				_b[i]+=(*_arr[i])[j];
		}
	}
}
void matrix::print_b()
{
	std::cout << "{";
	for(int i=0;i<_size;i++)
		std::cout << _b[i] <<",";
	std::cout << "}" << std::endl;
}