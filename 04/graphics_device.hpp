/*
* файл содержит класс графического устройства, а так же описание операции для графического устройства.
* 
* При проектировании архитектуры, в которой присутсвует описанное ниже графическое устройство, учитывайте, что активно используются регистры процессора под номерами 4, 5, 6, 7. 
* По этому в блоке ригистров процессора должно быть не менее 8 регистров.
*/
#ifndef JPL_GRAPHICS
#define JPL_GRAPHICS

#include <exception>
#include <unordered_map>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>

#include <GL/freeglut.h>

#include "types.h"
#include "operands.h"

namespace jpl 
{
	class GTX1050;
	
	namespace gd
	{	
		/// операция, выполняющая обработку команд взаимодействия с графическим устройством
		class GDO : public operation
		{
			private:
			/// ссылка на графическое устройство, к которому необходимо применить комманду
			GTX1050 &gd;
			public:
			GDO(GTX1050 &gtx) : gd(gtx) {req = {4,5,6,7};}
			virtual EXECUTOR executor() const override {return E_CONTROLLER;}
			virtual const char* lab() const override {return "GDO";}
			virtual void operator()(processor &proc, const command& cmd, long time) const override;
		};
	}
	namespace {
		std::mutex m_init; // защищает glut от попытки создать два окна из разных потоков в одно время
		struct key_pair{bool act; unsigned char key;};
		struct cmd {int i; std::vector<int> args;};
		std::unordered_map<int, key_pair> pairs;
		void onPress(unsigned char ch, int x, int y)
		{
			pairs[glutGetWindow()] = {true, ch};
		}
	}
	
	
	
	/// графическое устройство. Выполняет инициализацию и контроль над окном glut, предостовляет простой интерфейс рисования примитивов через openGL
	class GTX1050 {
		private:		
		GLint Width = 128, Height = 64;		
		gd::GDO _opr;		
		int window;
		bool run;
		std::thread *thr;
		std::queue<cmd> commands;
		void loop(){
			m_init.lock();
			glutInitDisplayMode(GLUT_RGB);
			glutInitWindowSize(Width*4, Height*4);
			window = glutCreateWindow("GTX1050");
	
			glViewport(0, 0, Width*4, Height*4);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, Width, Height, 0, -1.0, 1.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
	
			glLineWidth(2);
			glPointSize(2);
			
			glEnable(GL_POINT_SMOOTH);
			glutKeyboardFunc(onPress);
			glutDisplayFunc([]()->void{});
			
			m_init.unlock();			
			while(run)
			{
				glutSetWindow(window);
				
				while(!commands.empty())
				{
					cmd com = commands.front();
					commands.pop();
					//std::cerr << "gpu[" << com.i << "]" << std::endl;
					switch(com.i)
					{
						case 0x00: // очистить экран. в регистрах 4, 5, 6 лежат RGB компоненты фонового цвета
							clear(com.args[0],com.args[1],com.args[2]);
						break;
						case 0x01: // установить цвет кисти. в регистрах 4, 5, 6, 7 лежат RGBA компоненты цвета
							color(com.args[0],com.args[1],com.args[2],com.args[3]);
						break;
						case 0x02:	// начать отрисовку примитива. в регистре 4 лежит тип примитива (0 - точка, 1 - линия, 2 - замкнутая линия, 3 - цепочка линий, 4 - треугольник, 5 - цепочка треугольников, 
									// 6 - треугольник-вейер, 7 - квадраты, 8 - цепочка квадратов, 9 - произвольный многоугольник с заливкой)
							begin(com.args[0]);
						break;
						case 0x03:	// закончить отрисовку примитива. не требует аргументов.
							end();
						break;
						case 0x04:	// ввести вершину. в регистрах 4 и 5 лежат x и y координаты.
							vertex(com.args[0],com.args[1]);
						break;
						case 0x05:	// перенести изображение из буфера на экран. не требует аргументов.
							swap_buffer();
						break;
					
					}
				}
				
				glutMainLoopEvent();
				auto f = pairs.find(window);
				if(f != pairs.end() && f->second.act)
				{
					f->second.act = false;
					// код обработки нажатия. Вероятно, теперь придется писать аналог вектора прерываний :)
					//std::cerr << "u pressed: " << f->second.key << std::endl; 
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		
		public:
		GTX1050() : _opr(*this)
		{
			run = true;
			thr = new std::thread(GTX1050::loop, this);
			m_init.lock();
			m_init.unlock();
		}
		~GTX1050(){glutSetWindow(window); run = false; glutDestroyWindow(window); thr->join(); delete thr;}
		void close(){glutSetWindow(window); run = false; glutDestroyWindow(window); thr->join(); delete thr;}
		void begin(int mode) const {glutSetWindow(window); glBegin(mode);}
		void end() const {glutSetWindow(window); glEnd();}		
		void color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) const {glutSetWindow(window); glColor4ub(r,g,b,a);}
		void vertex(uint8_t x, uint8_t y) const {glutSetWindow(window); glVertex2i(x,y);}
		void swap_buffer(){glutSetWindow(window); glutSwapBuffers();}
		
		void clear(uint8_t r, uint8_t g, uint8_t b) const {
			glutSetWindow(window); 
			glClearColor(r, g, b, 1);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		
		void add_command(int cm, const std::vector<int> &args)
		{
			commands.push({cm, args});
		}
		
		const gd::GDO* get_operation() const {return &_opr;}
	};
	
	namespace gd 
	{
		void GDO::operator()(processor &proc, const command& cmd, long time) const {
			byte com = proc.get_value(*cmd.get_operand(0));
			std::vector<int> args(4);
			
			switch(com) // в качестве единственного параметра инструкция получает тип операции на графическом устройстве. Остальные параметры читаются напрямую из регистров процессора
			{
				case 0x00: // очистить экран. в регистрах 4, 5, 6 лежат RGB компоненты фонового цвета
					args = {proc.read_reg(4).data, proc.read_reg(5).data, proc.read_reg(6).data};
				break;
				case 0x01: // установить цвет кисти. в регистрах 4, 5, 6, 7 лежат RGBA компоненты цвета
					args = {proc.read_reg(4).data, proc.read_reg(5).data, proc.read_reg(6).data, proc.read_reg(7).data};
				break;
				case 0x02:	// начать отрисовку примитива. в регистре 4 лежит тип примитива (0 - точка, 1 - линия, 2 - замкнутая линия, 3 - цепочка линий, 4 - треугольник, 5 - цепочка треугольников, 
							// 6 - треугольник-вейер, 7 - квадраты, 8 - цепочка квадратов, 9 - произвольный многоугольник с заливкой)
					args = {proc.read_reg(4).data};
				break;
				/*case 0x03:	// закончить отрисовку примитива. не требует аргументов.
				break;*/
				case 0x04:	// ввести вершину. в регистрах 4 и 5 лежат x и y координаты.
					args = {proc.read_reg(4).data, proc.read_reg(5).data};
				break;
				/*case 0x05:	// перенести изображение из буфера на экран. не требует аргументов.
				break;*/
			}
			gd.add_command(com, args);
			proc.get_process().inc();
		}
	}
}


#endif