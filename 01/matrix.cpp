#include "matrix.h"

#include <iostream>

 list::~list() {
	while (_start) {
		item * tmp = _start -> next;
		delete _start;
		_start = tmp;
	}
}
void list::insert(int pos, int dt) {
	if (dt == 0) return;
	if (pos>_size) throw "index bigger then list size";
	if (_start!= nullptr) {
		
		if(_start->pos > pos){
			item * tmp = new item();
			tmp -> data = dt;
			tmp -> pos = pos;
			tmp -> next = _start;
			_start = tmp;
			return;
		}
		
		item * cur = _start;
		std::cout << "lol 1" <<std::endl;
		while (cur -> next != nullptr && pos < cur -> next -> pos)
			cur = cur -> next;
		std::cout << "lol 2" <<std::endl;
		item  * tmp = new item  ();
		tmp -> data = dt;
		tmp -> pos = pos;
		tmp -> next = cur -> next;
		cur -> next = tmp;
	} else {
		_start = new item  ();
		_start -> data = dt;
		_start -> pos = pos;
		_start -> next = nullptr;
	}
}
int list::operator[](const int i) {
	if (i>_size) throw "index bigger then list size";
	if (_start!= nullptr) {
		item * cur = _start;
		while (cur -> next != nullptr && i != cur -> pos)
			cur = cur -> next;
		if (cur -> pos == i) return cur -> data;
	}
	return 0;
}
matrix::matrix(int m, int n) {
	_m = m;
	_n = n;
	_arr = new list*[_m];
	for(int i=0;i<_m;i++)
		_arr[i] = new list(_n);	
	_b = new int[_m];
}
matrix::~matrix() {
	for(int i=0;i<_m;i++)
		delete _arr[i];	
	delete[] _arr;
	delete[] _b;
}

list*& matrix::operator[](const int i) {
	return _arr[i];
}
void matrix::print() {
	for (int i = 0; i < _m; i++) {
		std::cout << "[" << i << "]:\t";
		for (int j = 0; j < _n; j++)
			std::cout << (*_arr[i])[j] << ", ";
		std::cout << std::endl;
	}
}
void matrix::calculate() {
	for (int i = 0; i < _m; i++) {
		_b[i] = 0;
		for (int j = 0; j < _m; j++) {
			int tmp = (*_arr[i])[j];
			if(_comparator(tmp))
				_b[i]+= tmp;
		}
	}
}
void matrix::print_b() {
	std::cout << "{";
	for (int i = 0; i < _m; i++)
		std::cout << _b[i] << ",";
	std::cout << "}" << std::endl;
}