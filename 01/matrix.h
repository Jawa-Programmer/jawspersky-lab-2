
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
	/// функция, в которую пользователь может вставить критерий отбора числа.
	bool (*_comparator)(int);
	/// массив указателей на строки матрицы
	line **_arr;
	/// количество строк
	int _size;
	/// строка чисел b.
	int* _b;
	public:	
	/// конструктор. производит инициализацию массива указателей на строки
	matrix(int);
	/// деструктор. очищает память
	~matrix();
	/// выводит на экран матрицу
	void print();
	/// производит вычисление вектора b
	void calculate();
	/// выводит на экран искомый вектор b
	void print_b();
	/// оператор доступа к i строке (обратите внимание на то, что возвращает указатель по ссылке. для доступа к [i,j] числу используйте синтаксис (*mtrx[i])[j])
	line*& operator[] (const int);
	/// возвращает количество строк в матрице.
	int size(){return _size;}
	/// устанавливает функцию-предикат, отбирающая числа строки для сложения.
	void set_comparator(bool (*pred)(int));
};
#endif