#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
using namespace std;

template<typename Type>
struct FooBase
{
	unordered_map<string, Type> m;
	FooBase()
	{
		std::cout << "FooBase ctor called" << std::endl;
	}
};

template<typename... Types>
struct Foo : FooBase<Types>...
{
	Foo()
	{
		std::cout << "Foo ctor called" << std::endl;
	}

	template<typename CheckingType>
	bool checkType()
	{
		return checkTypeInternal<CheckingType, Types...>();
	}
private:
	
	template<typename CheckingType, typename T, typename... OtherTypes>
	bool checkTypeInternal()
	{
		if(std::type_index(typeid(CheckingType)) == std::type_index(typeid(T))) return true;
		return checkTypeInternal<CheckingType, OtherTypes...>();
	};

	template<typename CheckingType>
	bool checkTypeInternal()
	{
		return false;
	};


};

int main()
{
	Foo<int, float, double> foo;
	std::cout << "int: " << foo.checkType<int>() << std::endl;
	std::cout << "float: " << foo.checkType<float>() << std::endl;
	std::cout << "double: " << foo.checkType<double>() << std::endl;
	std::cout << "string: " << foo.checkType<std::string>() << std::endl;
	return 0;
}