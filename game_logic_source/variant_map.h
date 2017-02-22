#ifndef VARIANT_MAP_H
#define VARIANT_MAP_H

#include <map>
#include <typeinfo>
#include <typeindex>
#include <string>

namespace
{
    template<typename Type>
    struct VariantMapPart
    {
    protected:
        std::map<std::string, Type> m;
    };


    template<typename... Types>
    struct _VariantMap : VariantMapPart<Types>...
    {

        template<typename CheckingType>
        int getTypeIndex()
        {
            return getTypeIndexInternal<CheckingType, Types...>();
        }

        template<typename T>
        void set(const std::string& key, const T& value)
        {
            VariantMapPart<T>::m[key] = value;
        }

        template<typename T>
        T& get(const std::string& key)
        {
            return VariantMapPart<T>::m[key];
        }

    private:
        template<typename UselessType, typename T, typename... OtherTypes>
        int getTypesCount()
        {
            return getTypesCount<UselessType, OtherTypes...>() + 1;
        }

        template<typename UselessType>
        int getTypesCount()
        {
            return 0;
        }


        template<typename CheckingType, typename T, typename... OtherTypes>
        int getTypeIndexInternal()
        {
            if(std::type_index(typeid(CheckingType)) == std::type_index(typeid(T))) return getTypesCount<int, OtherTypes...>();
            return getTypeIndexInternal<CheckingType, OtherTypes...>();
        };

        template<typename CheckingType>
        int getTypeIndexInternal()
        {
            return -1;
        }
    };


}

struct VariantMap : _VariantMap<int, std::string> {};

#endif