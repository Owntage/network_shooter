#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <variant_map.h>
using namespace std;



int main()
{
    VariantMap m;

    if(std::type_index(typeid(int)) == std::type_index(typeid(const char*)))
    {
        std::cout << "holy mother of god!" << std::endl;

    }

    m.set("string_key", "kekeke1");
    m.set("int_key", 10);

    std::vector<int> vec = {1, 2, 3, 4, 5};

    m.set("int_vec_key", vec);

    std::cout << "extracted string: " << m.get<std::string>("string_key") << std::endl;
    std::cout << "extracted int: " << m.get<int>("int_key") << std::endl;

    auto& extracted_vec = m.get<std::vector<int> >("int_vec_key");

    std::cout << "vec: ";
    for(int i = 0; i < extracted_vec.size(); i++)
    {
        std::cout << extracted_vec[i] << " ";
    }
    std::cout << endl;
	return 0;
}