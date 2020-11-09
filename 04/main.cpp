
#include <iostream>

class A {	
public:
	virtual void B(std::ostream &st) const = 0;
};

int main()
{
	const class : public A {
		public:
		virtual void B(std::ostream &st) const override {
		st << "it is anonymos class" << std::endl;
		}
	} LOL;
	
	LOL.B(std::cout);
	return 0;
}