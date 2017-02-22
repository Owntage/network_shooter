#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <variant_map.h>
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
    /*
	Foo<int, float, double> foo;
	std::cout << "int: " << foo.checkType<int>() << std::endl;
	std::cout << "float: " << foo.checkType<float>() << std::endl;
	std::cout << "double: " << foo.checkType<double>() << std::endl;
	std::cout << "string: " << foo.checkType<std::string>() << std::endl;
     */
    VariantMap m;
    std::cout << "int index: " << m.getTypeIndex<int>() << std::endl;
    std::cout << "float index: " << m.getTypeIndex<float>() << std::endl;
    std::cout << "double index: " << m.getTypeIndex<double>() << std::endl;
    std::cout << "string index: " << m.getTypeIndex<std::string>() << std::endl;

    if(std::type_index(typeid(int)) == std::type_index(typeid(const char*)))
    {
        std::cout << "holy mother of god!" << std::endl;

    }

    m.set("key1", "kekeke1");
    //m.set("key2", 1);
	return 0;
}