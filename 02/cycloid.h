#ifndef CYCLOID_H
#define CYCLOID_H

/// точка на плоскости в декартовых координатах
struct point
{
	double X, Y;
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
	cycloid(double, double);
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

#endif