#ifndef CYCLOID_H
#define CYCLOID_H
#ifndef PI
#define PI 3.14159265358979323846
#endif
/// пространство имён для классов геометрических объектов
namespace geometric{
	/// мой класс ошибок.
	struct MyException
	{
		private:
		const char* _msg;
		public:
		MyException(const char* err):_msg(err){}
		inline const char* what() {return _msg;}
	};
	/// ошибка радиуса
	struct IncorrectRadiusException : MyException
	{
		public:
		IncorrectRadiusException(const char* err):MyException(err){}
	};
	/// точка на плоскости в декартовых координатах.
	struct point
	{
		double X, Y;
		/// покоординатная сумма точек
		point operator+(point p)
		{
			return {X+p.X, Y+p.Y};
		}
		/// покоординатная разность точек 
		point operator-(point p)
		{
			return {X-p.X, Y-p.Y};
		}
		/// длина радиусвектора, указывающего на точку.
		double length();
	};
	/// класс, описывающий циклоиду
	class cycloid
	{
		private:
		/// начальный угол
		double _phase;
		/// радиус циклоиды
		double _radius;	
		public:
		/// конструктор класса. Инициализирует объект класса циклоида
		cycloid(double radius, double phase = 0);
		/// геттеры и сеттеры
		inline double get_phase(){return _phase;}
		inline double get_radius(){return _radius;}
		inline void set_phase(double phase){_phase = phase;}
		inline void set_radius(double radius){
			if(radius <0) 
				throw IncorrectRadiusException("Radius must be positive or zero");
			else
				_radius = radius;
			}
		
		/// возвращает декартовы координаты точки, на которую будет указывать циклоида после вращения на заданный угол
		point coords(double);
		/// возвращает радиус кривизны циклоиды в точке, на которую она будет указывать после поворота на заданный угол
		double curvature(double);
		/// возвращает длинну кривой, зажатой между начальным смещением и начным смещениме+заданный угол
		double length(double);
		/// возвращает площадь циклоиды
		double area();
		/// возвращает площадь поверхности вращения циклоиды
		double surface_area();
		/// объем тела вращения циклоиды
		double volume();
	};
}
#endif