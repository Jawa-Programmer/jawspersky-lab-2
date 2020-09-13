#include "matrix.h"
#include <iostream>
matrix::matrix(int m, int n)
{
	_m = m;
	_n = n;
	_arr = new int*[_m];
	for(int i=0;i<_m;i++)
		_arr[i] = new int[_n];
	_b = new int[_m];
}
matrix::~matrix()
{
	for(int i=0;i<_m;i++)
	{
		delete _arr[i];		
	}
	delete [] _arr;
	delete [] _b;
}
int*& matrix::operator[](const int i)
{
	return _arr[i];
}
void matrix::print()
{
	for(int i=0; i < _m;i++){
		std::cout << "["<<i<<"]:\t";
		for(int j=0; j<_n;j++)
			std::cout << _arr[i][j] << ", ";
		std::cout << std::endl;
	}
}
void matrix::set_comparator(bool (* pred)(int) )
{
	_comparator = pred;	
}
void matrix::calculate()
{
	for (int i=0;i<_m;i++)
	{
		_b[i] = 0;
		for(int j = 0; j < _n; j++){
			if(_comparator(_arr[i][j]))
				_b[i]+=_arr[i][j];
		}
	}
}
void matrix::print_b()
{
	std::cout << "{";
	for(int i=0; i < _m; i++)
		std::cout << _b[i] <<",";
	std::cout << "}" << std::endl;
}