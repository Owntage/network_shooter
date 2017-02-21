#ifndef VARIANT_MAP_H
#define VARIANT_MAP_H

#include <unordered_map>
#include <typeinfo>
#include <typeindex>

namespace
{
    template<typename Type>
    struct VariantMapPart
    {
    protected:
        std::unordered_map<Type> m;
    };

    template<typename... Types>
    struct _VariantMap : VariantMapPart<Types>...
    {

    private:
        template<typename T, typename... OtherTypes>
        int getTypesCount()
        {
            return getTypesCount(OtherTypes...) + 1;
        }
        template<>
        int getTypesCount()
        {
            return 0;
        }

        template<typename CheckingType>
        int getTypeIndex()
        {
            return getTypeIndexInternal<CheckingType, Types...>();
        }

        template<typename CheckingType, typename T, typename... OtherTypes>
        int getTypeIndexInternal()
        {
            if(std::type_index(typeid(CheckingType)) == std::type_index(typeid(T))) return getTypesCount(OtherTypes...);
            return getTypeIndexInternal<CheckingType, OtherTypes...>();
        };

        template<typename CheckingType>
        int getTypeIndexInternal()
        {
            return -1;
        }
    };
}

#endif VARIANT_MAP_H
