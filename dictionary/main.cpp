#include "dictionary.h"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using namespace jpl; 

int main()
{
	dictionary<std::string, int> dic;
	
	dic.set("abc", 2);
	dic.set("c", 3);
	dic.set("ab", 1);
	
	auto end = dic.end();
	auto cur = dic.begin();
	
	while(cur != end)
	{
		cout << "vec[" << (-cur) << "] = " << *cur << endl;
		cur = cur + 1;
	}
	cout << "-----------------------------" << endl;
	
	dic.unset("c");
	dic.set("abc", 4);
	
	cout << "dic[ab] = " << dic["ab"] << endl;
	
	end = dic.end();
	cur = dic.begin();
	
	cout << "-----------------------------" << endl;
	while(cur != end)
	{
		cout << "vec[" << (-cur) << "] = " << *cur << endl;
		++cur;
	}
	
	return 0;
}