/*
* файл содержит класс графического устройства, а так же описание операторов для графического устройства.
*/

namespace jpl 
{
	
	namespace gpd
	{
		/// устанавливает
		const class : public operator_
		{
			public:
			virtual std::ostream& print(std::ostream& out) const override {return out << "DRCH";}
			virtual void operator()(const std::initializer_list<byte> &args) const override {
				if(args.size() != 1) throw std::logic_error("incorrect count of args");
				
				}
		} DRCH;
	}
}