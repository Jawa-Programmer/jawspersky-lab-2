#include <iostream>

#include "cycloid.h"

using std::cin;
using std::cout;
using std::endl;


/// Универсальная функция безопасного ввода значений из входного потока
template <typename T> void safeGet(T& variable)
{
	cin >> variable;
	while (!cin)
	{
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		std::cerr << "incorrect input, try again: ";
		cin >> variable;
	}
	return;
}

int main()
{
	char map[60][20];	
	// инициализация стандартной циклоиды
	geometric::cycloid cycl(1);
	char key = 0;
	while(1)
	{
		system("cls");
		cout << "r - change radius, c - get coords, k - get curvature, l - get length, a - get area, s - get surface area, v - get volume, q - exit"<<endl;
		
		for(int x = 0; x< 60;x++)
			for(int y = 0; y< 20;y++)
				{
					map[x][y] = ' ';
				}
		for(double FI = 0; FI < PI*2*cycl.get_radius(); FI+=0.1)
		{
			geometric::point po = cycl.coords(FI);
			int xx = (int) (po.X/2/PI/cycl.get_radius()*60);
			int yy = (20 - po.Y/2/cycl.get_radius()*20);
			if (xx<0||xx>=60||yy<0||yy>=20)continue;
			map[xx][yy]='*';
		}
		
		for(int y = 0; y< 20;y++){
			for(int x = 0; x< 60;x++){
				cout << map[x][y];
			}
			cout<<endl;
		}
		cout << "-------------------------------------------------------------"<<endl;
		safeGet(key);
		if(key == 'q') break;
		double r, p;
		switch(key)
		{
			case 'r':
				cout << "type new cycloid radius: " <<endl;
				safeGet(r);
				try {
					cycl.set_radius(r);
					cout << "radius sucsessful changed."<<endl;
				}
				catch (geometric::IncorrectRadiusException &ex)
				{
					std::cerr << ex.what() <<endl << "data not changed"<<endl;
				}
			break;
			case 'c':
			{
				cout << "type angle: " <<endl;
					safeGet(r);
					geometric::point po = cycl.coords(r);
					cout << "X = " << po.X << " Y = " << po.Y << endl;
				break;
			}
			case 'l':
			cout << "type angle: " <<endl;
				safeGet(r);
				p = cycl.length(r);
				cout << "L = " << p << endl;
			break;
			case 'a':
				p = cycl.area();
				cout << "area = " << p << endl;
			break;
			case 's':
				p = cycl.surface_area();
				cout << "surface area = " << p << endl;
			break;
			case 'v':
				p = cycl.volume();
				cout << "volume = " << p << endl;
			break;
		}
	cout << "type any key to continue" << endl;
	safeGet(key);
	}	
	return 0;
}
