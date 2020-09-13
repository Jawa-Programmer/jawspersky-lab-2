#include "matrix.h"

#include <iostream>

template < class T > list<T>::~list() {
	while (_start) {
		item < T > * tmp = _start -> next;
		delete _start;
		_start = tmp;
	}
}
template < class T > void list<T>::insert(int pos, T dt) {
	if (_start) {
		item < T > * cur = _start;
		while (cur -> next && pos < cur -> next -> pos)
			cur = cur -> next;
		item < T > * tmp = new item < T > ();
		tmp -> data = dt;
		tmp -> next = cur -> next;

		cur -> next = tmp;
	} else {
		_start = new item < T > ();
		_start -> dt;
	}
}
template < class T > T list<T>::operator[](const int i) {
	if (i>_size) throw "index bigger then list size";
	if (_start) {
		item < T > * cur = _start;
		while (cur -> next && i != cur -> pos)
			cur = cur -> next;
		if (cur -> pos == i) return cur -> data;
	}
	return *(new T());
}
template < class T >matrix<T>::matrix(int m, int n) {
	_m = m;
	_n = n;
	_arr = new list < T > [_m](_n);
	_b = new T[_m];
}
template < class T >matrix<T>::~matrix() {
	delete[] _arr;
	delete[] _b;
}

template < class T > list<T>& matrix<T>::operator[](const int i) {
	return _arr[i];
}
template < class T >void matrix<T>::print() {
	for (int i = 0; i < _m; i++) {
		std::cout << "[" << i << "]:\t";
		for (int j = 0; j < _n; j++)
			std::cout << _arr[i][j] << ", ";
		std::cout << std::endl;
	}
}
template < class T >void matrix<T>::calculate() {
	for (int i = 0; i < _m; i++) {
		_b[i] = *(new T());
		for (int j = 0; j < _m; j++) {
			T tmp = _arr[i][j];
			if(_comparator(tmp))
				_b[i] = _add(_b[i], tmp);
		}
	}
}
template < class T >void matrix<T>::print_b() {
	std::cout << "{";
	for (int i = 0; i < _m; i++)
		std::cout << _b[i] << ",";
	std::cout << "}" << std::endl;
}