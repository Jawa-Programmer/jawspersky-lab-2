#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include "processor/memory.h"

// выполним небольшую проверку того, что правильно ли работает блокировка ОП
// вывод - блокировка работает как и ожидалось. А правильно это или нет - зависит от того, что есть "правильно" :)

/// генератор псевдослучайных чисел
int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0); 
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

using namespace jpl;
using std::cout;
using std::endl;
bool flag = true;

std::mutex iom; // мьютекс для синхронизации вывода в поток

unsigned int start;

void thr1(RAM &ram)
{
	while(flag)
	{
		ram.lock_write();
		iom.lock(); cout << "(" << (clock()-start) << ") THR1 LOCKED WRITE" << endl; iom.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(200,400)));
		iom.lock(); cout << "(" << (clock()-start) << ") THR1 UNLOCKED WRITE" << endl; iom.unlock();
		ram.unlock_write();	
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(0,100)));
	}
}

void thr2(RAM &ram)
{
	while(flag)
	{
		ram.lock_read();
		iom.lock(); cout << "(" << (clock()-start) << ") THR2 LOCKED READ" << endl; iom.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(50,500)));
		iom.lock(); cout << "(" << (clock()-start) << ") THR2 UNLOCKED READ" << endl; iom.unlock();
		ram.unlock_read();	
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(0,100)));
	}
}
void thr3(RAM &ram)
{
	while(flag)
	{
		ram.lock_read();
		iom.lock(); cout << "(" << (clock()-start) << ") THR3 LOCKED READ" << endl; iom.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(100,250)));
		iom.lock(); cout << "(" << (clock()-start) << ") THR3 UNLOCKED READ" << endl; iom.unlock();
		ram.unlock_read();	
		std::this_thread::sleep_for(std::chrono::milliseconds(getRandomNumber(0,100)));
	}
}
int main()
{
	RAM ram;
	std::thread th1(thr1, std::ref(ram)), th2(thr2, std::ref(ram)), th3(thr3, std::ref(ram));
	start = clock();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	flag = false;
	th1.join();
	th2.join();
	th3.join();
	return 0;
}