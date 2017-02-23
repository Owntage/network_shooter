#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <network.h>
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

    Packet packet;
    packet << m;

    VariantMap extracted_m;

    packet >> extracted_m;


    std::cout << "extracted string: " << extracted_m.get<std::string>("string_key") << std::endl;
    std::cout << "extracted int: " << extracted_m.get<int>("int_key") << std::endl;



    std::cout << m;

	return 0;
}