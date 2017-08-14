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
    m.setVector("vector_key", std::vector<int>({1, 2, 3}));
	VariantMap innerM;
	innerM.set("first", 1337);
	m.setObject("map_key", innerM);

	std::vector<VariantMap> objVec;
	innerM.set("first", 7331);
	objVec.push_back(innerM);
	m.setObjectVector("obj_vec_key", objVec);

    Packet packet;
    packet << m;
    VariantMap extracted_m;
    packet >> extracted_m;
    std::cout << "extracted string: " << extracted_m.get<std::string>("string_key") << std::endl;
    std::cout << "extracted int: " << extracted_m.get<int>("int_key") << std::endl;
    std::cout << "extracted vector element: " << extracted_m.getVector<int>("vector_key")[1] << std::endl;
	std::cout << "extracted inner object: " << extracted_m.getObject("map_key").get<int>("first") << std::endl;
	std::cout << "extracted inner object vec: " << extracted_m.getObjectVector("obj_vec_key")[0].get<int>("first") << std::endl;
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