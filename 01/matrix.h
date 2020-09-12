/// класс обертка для массива целых чисел
/// позволяет вводить числа более простым способом, нежели обычный массив.
class line
{
	private:
	int * _arr;
	int _size;
	int _pos;
	public:
	line(int);
	~line();
	void operator<<(int);
	int& operator[](int);
	inline int size() {return _size;}
	
};
/// класс непрямоугольной матрицы
class matrix
{
	private:
	line *_arr;
	int _size;
	public:	
	matrix(int);
	~matrix();
	void print();
	void print_b();
	line& opertor[](int);
	int size(){return _size;}
};