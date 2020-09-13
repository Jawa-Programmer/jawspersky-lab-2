
#ifndef MATRIX_H
#define MATRIX_H
/// класс обертка для массива целых чисел
/// позволяет вводить числа более простым способом, нежели обычный массив.
class line
{
	private:
	/// Поле указатель на массив чисел строки
	int * _arr;
	/// поле размер массива
	int _size;
	public:
	/// Конструктор класса строки. Производит выделение памяти под массив чисел.
	line(int);
	/// Деструктор класса строки. Производит очистку памяти
	~line();
	/// Выводит строку в консоль
	void print();
	/// Оператор доступа к элементам массива по индексу.
	int& operator[] (const int);
	/// Возвращает размер массива.
	inline int size() {return _size;}
	
};
/// класс непрямоугольной матрицы
class matrix
{
	private:
	line **_arr;
	int _size;
	public:	
	matrix(int);
	~matrix();
	void print();
	void print_b();
	line*& operator[] (const int);
	int size(){return _size;}
};
#endif