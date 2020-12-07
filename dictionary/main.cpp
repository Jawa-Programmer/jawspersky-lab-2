#include "dictionary.hpp"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using namespace jpl; 

int main()
{
dictionary<std::string, int> dic({{"ab", 2}, {"c", 1}});
	
	
	
	auto end = dic.end();
	auto cur = dic.begin();
	
	while(cur != end)
	{
		cout << "vec[" << cur->key << "] = " << cur->value << endl;
		cur = cur + 1;
	}
	cout << "-----------------------------" << endl;
	
	dic.unset("c");
	dic["abc"] = 4;
	
	cout << "dic[ab] = " << dic["ab"] << endl;
	
	end = dic.end();
	cur = dic.begin();
	
	cout << "-----------------------------" << endl;
	while(cur != end)
	{
		cout << "vec[" << cur->key << "] = " << cur->value << endl;
		++cur;
	}
	
	return 0;
}