#include "matrix.h"

line::line(int sz)
{
	_size = sz;
	_pos = 0;
	_arr = new int[_size];
}
line::~line()
{
	delete[] _arr;
}
void line::operator <<(int data)
{
	if(_pos<_size)
		_arr[_pos++] = data;
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
