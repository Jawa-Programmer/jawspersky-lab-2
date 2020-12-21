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

#include "processor/types.h"
#include "processor/operands.h"

namespace jpl 
{
	class GTX1050;
	class Keyboard;
	
	namespace gd
	{	
		/// операция, выполняющая обработку команд взаимодействия с графическим устройством
		class GDO : public operation
		{
			private:
			/// ссылка на графическое устройство, к которому необходимо применить комманду
			GTX1050 &gd;
			public:
			GDO(GTX1050 &gtx) : gd(gtx) {}
			virtual EXECUTOR executor() const override {return E_CONTROLLER;}
			virtual const char* lab() const override {return "GDO";}
			virtual void operator()(processor &proc, const command& cmd, long time) const override;
		};
		/// операция, выполняющая команды взаимодействия с клавиатурой
		class KBD : public operation
		{
			private:
			/// ссылка на клавиатуру, к которой необходимо применить комманду
			Keyboard &kb;
			public:
			KBD(Keyboard &kbd) : kb(kbd) {}
			virtual EXECUTOR executor() const override {return E_CONTROLLER;}
			virtual const char* lab() const override {return "KBD";}
			virtual void operator()(processor &proc, const command& cmd, long time) const override;
		};
	}
	
	/// класс описатель клавиатуры. Хранит очередь нажатий клавиш.
	class Keyboard
	{
		static std::mutex m_kb;
		struct kpair {int mod; unsigned char key;};
		struct spkpair {int mod; int key;};
		std::queue<kpair> keys;
		std::queue<spkpair> spkeys;
		
		gd::KBD kbd;
		
		public:
		Keyboard() : kbd(*this){};
		void push_key(int mod, unsigned char key){if(key==0)return; m_kb.lock(); keys.push({mod, key}); m_kb.unlock(); }
		void push_spkey(int mod, int spkey){if(spkey==0)return;m_kb.lock(); spkeys.push({mod, spkey}); m_kb.unlock();}
		
		kpair pop_key(){m_kb.lock(); kpair ret = keys.front();  keys.pop(); m_kb.unlock();  return ret;}
		spkpair pop_spkey(){m_kb.lock(); spkpair ret = spkeys.front(); spkeys.pop(); m_kb.unlock(); return ret; }
		
		bool has_keys() const {m_kb.lock(); bool ret = !keys.empty(); m_kb.unlock();return ret;}
		bool has_spkeys() const {m_kb.lock(); bool ret = !spkeys.empty(); m_kb.unlock();return ret;}
		
		const gd::KBD* get_operation() const {return &kbd;}
	};
	std::mutex Keyboard::m_kb;
	
	namespace {
		std::mutex m_init; // защищает glut от попытки создать два окна из разных потоков в одно время
		std::mutex m_drw; // защищает от попытки рисовать в два окна одновременно
		struct cmd {int i; std::vector<int> args;};
		std::unordered_map<int, Keyboard*> kboards;
		void onPress(unsigned char ch, int x, int y)
		{
			kboards[glutGetWindow()]->push_key(glutGetModifiers(), ch);
		}
		void onSpPress(int ch, int x, int y)
		{
			kboards[glutGetWindow()]->push_spkey(glutGetModifiers(), ch);
		}
	}
	
	
	
	/// графическое устройство. Выполняет инициализацию и контроль над окном glut, предостовляет простой интерфейс рисования примитивов через openGL
	/// Так же предоставляет доступ к объекту клавиатуры, в котором хранится последовательность нажатых клавиш
	class GTX1050 {
		private:		
		GLint Width = 128, Height = 64;		
		gd::GDO _opr;		
		int window;
		bool run;
		std::thread *thr;
		std::queue<cmd> commands;
		Keyboard kboard;
		
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
			
			kboards[window] = &kboard;
			
			glutKeyboardFunc(onPress);
			glutSpecialFunc(onSpPress);
			glutDisplayFunc([]()->void{});
			m_init.unlock();			
			while(run)
			{
				m_drw.lock();
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
				m_drw.unlock();
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
		~GTX1050(){close();}
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
		
		Keyboard& get_keyboard() {return kboard;}
		
		const gd::GDO* get_operation() const {return &_opr;}
	};
	
	namespace gd 
	{
		void GDO::operator()(processor &proc, const command& cmd, long time) const {
			proc.get_RAM()->lock_read();
			byte com = proc.get_value(*cmd.get_operand(0));
			proc.get_RAM()->unlock_read();
			std::vector<int> args(4);			
			switch(com) // в качестве единственного параметра инструкция получает тип операции на графическом устройстве. Остальные параметры читаются напрямую из регистров процессора
			{
				case 0x00: // очистить экран. в регистрах 4, 5, 6 лежат RGB компоненты фонового цвета
				if(!(proc.is_free(reg_operand(const_operand(4))) && proc.is_free(reg_operand(const_operand(5))) && proc.is_free(reg_operand(const_operand(6))))) return; // если нужные регистры еще заняты, то мы не переводим регистр команд. В следующей итерации попытка будет повторена
					args = {proc.read_reg(4).data, proc.read_reg(5).data, proc.read_reg(6).data};
				break;
				case 0x01: // установить цвет кисти. в регистрах 4, 5, 6, 7 лежат RGBA компоненты цвета
					if(!(proc.is_free(reg_operand(const_operand(4))) && proc.is_free(reg_operand(const_operand(5))) && proc.is_free(reg_operand(const_operand(6)))&& proc.is_free(reg_operand(const_operand(7))))) return;
					args = {proc.read_reg(4).data, proc.read_reg(5).data, proc.read_reg(6).data, proc.read_reg(7).data};
				break;
				case 0x02:	// начать отрисовку примитива. в регистре 4 лежит тип примитива (0 - точка, 1 - линия, 2 - замкнутая линия, 3 - цепочка линий, 4 - треугольник, 5 - цепочка треугольников, 
							// 6 - треугольник-вейер, 7 - квадраты, 8 - цепочка квадратов, 9 - произвольный многоугольник с заливкой)
					if(!(proc.is_free(reg_operand(const_operand(4))))) return;
					args = {proc.read_reg(4).data};
				break;
				/*case 0x03:	// закончить отрисовку примитива. не требует аргументов.
				break;*/
				case 0x04:	// ввести вершину. в регистрах 4 и 5 лежат x и y координаты.
					if(!(proc.is_free(reg_operand(const_operand(4)) )&& proc.is_free(reg_operand(const_operand(5))))) return;
					args = {proc.read_reg(4).data, proc.read_reg(5).data};
				break;
				/*case 0x05:	// перенести изображение из буфера на экран. не требует аргументов.
				break;*/
			}			
			gd.add_command(com, args);
			proc.get_process().inc();
		}
		void KBD::operator()(processor &proc, const command& cmd, long time) const {
			proc.get_RAM()->lock_read();
			byte com = proc.get_value(*cmd.get_operand(0));
			proc.get_RAM()->unlock_read();
			switch(com)
			{
				case 0x00: // проверить, находится ли в буфере клавиатуры информация о нажатой символьной клавише. Результат проверки помещается в регистр процессора под номером 3
					if(!(proc.is_free(reg_operand(const_operand(3))))) return; // нужный регистр занят, ожидаем освобождения
					proc.reg_access(3).data = kb.has_keys();
				break;
				case 0x01: // проверить, находится ли в буфере клавиатуры информация о нажатой спец. клавише (на подобии стрелок, delete, home, F1-F12 и так далее). Результат проверки помещается в регистр процессора под номером 3
					if(!(proc.is_free(reg_operand(const_operand(3))))) return; // нужный регистр занят, ожидаем освобождения
					proc.reg_access(3).data = kb.has_spkeys();				
				break;
				case 0x02: // считывает символ с буфера клавиатуры и помещает в регистр 3. В регистр 2 помещается информация об модификаторе (ALT CTRL SHIFT). Если буфер пуст, то процессор будет ожидать, пока пользователь не нажмет клавишу.
					if(!kb.has_keys()) return; // если клавиша не нажата, то ожидаем нажатия
					if(!(proc.is_free(reg_operand(const_operand(3))) && proc.is_free(reg_operand(const_operand(2))))) return; // нужный регистр занят, ожидаем освобождения
					{
						auto key = kb.pop_key();
						proc.reg_access(3).data	= key.key;
						proc.reg_access(2).data	= key.mod;
					}
				break;
				case 0x03: // то же, что и 0x02, но для специальных клавиш
					if(!kb.has_spkeys()) return; // если клавиша не нажата, то ожидаем нажатия
					if(!(proc.is_free(reg_operand(const_operand(3))) && proc.is_free(reg_operand(const_operand(2))))) return; // нужный регистр занят, ожидаем освобождения
					{
						auto key = kb.pop_spkey();
						proc.reg_access(3).data	= key.key;
						proc.reg_access(2).data	= key.mod;
					}
				break;
			}
			proc.get_process().inc(); //раз досрочного return не было, значит команды исполнились и можно перевести счетчик команд на 1 вперед
		}
	}
}


#endif