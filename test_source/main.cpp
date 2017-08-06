#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <network.h>
#include <variant/variant_map.h>
#include <variant/variant_event.h>
using namespace std;



int main()
{

    //variant map test
    VariantMap m;
    m.set("string_key", "kekeke1");
    m.set("int_key", 10);
    Packet packet;
    packet << m;
    VariantMap extracted_m;
    packet >> extracted_m;
    std::cout << "extracted string: " << extracted_m.get<std::string>("string_key") << std::endl;
    std::cout << "extracted int: " << extracted_m.get<int>("int_key") << std::endl;
    std::cout << extracted_m << std::endl;

    //variant event test:
    VariantEvent variantEvent("type1");
    variantEvent.set("string_key", "some_important_data");
    VariantEvent copyEvent = variantEvent;
    packet.reset();
    packet << copyEvent;
    VariantEvent extractedEvent;
    packet >> extractedEvent;
    std::cout << "extracted event: " << extractedEvent << std::endl;
    std::cout << "non-extracted event: " << variantEvent << std::endl;

    std::cout << "type: " << extractedEvent.name << std::endl;
    std::cout << "string key: " << extractedEvent.get<std::string>("string_key") << std::endl;

	return 0;
}