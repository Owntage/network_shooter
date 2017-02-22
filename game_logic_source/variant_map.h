#ifndef VARIANT_MAP_H
#define VARIANT_MAP_H

#include <map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>

namespace
{
    template<typename Left, typename Right, bool assignable>
    struct AssignHelper
    {
        static void assign(Left& left, Right& right)
        {
            left = right;
        }
    };

    template<typename Left, typename Right>
    struct AssignHelper<Left, Right, false>
    {
        static void assign(Left& left, Right& right) {} //do nothing
    };

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
        void set(const std::string& key, T value)
        {
            //VariantMapPart<T>::m[key] = value;
            SetFunctorInvoker<T> setFunctorInvoker(key, value);
            callByTypeIndex<decltype(setFunctorInvoker), SetFunctor>(setFunctorInvoker, getTypeEquivalenceClass<T>());
        }

        template<typename T>
        T& get(const std::string& key)
        {
            return VariantMapPart<T>::m[key];
        }

    private:


        template<typename T>
        struct SetFunctorInvoker
        {
            SetFunctorInvoker(const std::string& key, const T& value) : key(key), value(value) {}
            const T& value;
            std::string key;

            template<typename Functor>
            void operator()(Functor& functor)
            {
                functor(key, value);
            }
        };

        template<typename T>
        struct SetFunctor
        {
            SetFunctor(_VariantMap& parent) : parent(parent) {}
            _VariantMap& parent;

            template<typename SetType>
            void operator()(const std::string& key, const SetType& value)
            {
                AssignHelper<decltype(parent.VariantMapPart<T>::m[key]), decltype(value),
                        std::is_assignable<T, SetType>::value || std::is_convertible<T, SetType>::value >::assign(parent.VariantMapPart<T>::m[key], value);

            }
        };

        template<typename FunctorInvoker, template< class > class Functor>
        void callByTypeIndex(FunctorInvoker& invoker, int typeIndex)
        {
            callByTypeIndexInternal<FunctorInvoker, Functor, Types...>(invoker, typeIndex);
        }
        template<typename FunctorInvoker, template< class > class Functor, typename T, typename... OtherTypes>
        void callByTypeIndexInternal(FunctorInvoker& invoker, int typeIndex)
        {
            if(getTypeIndex<T>() == typeIndex)
            {
                Functor<T> functor(*this);
                invoker(functor);
            }
            else
            {
                callByTypeIndexInternal<FunctorInvoker, Functor, OtherTypes...>(invoker, typeIndex);
            }
        };
        template<typename FunctorInvoker, template< class > class Functor>
        void callByTypeIndexInternal(FunctorInvoker& invoker, int typeIndex) {} //do nothing, because we don't have type with such index

        template<typename T>
        int getTypeEquivalenceClass()
        {
            if(std::type_index(typeid(T)) == std::type_index(typeid(const char*))) return getTypeIndex<std::string>();

            return getTypeIndex<T>();
        }


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

struct VariantMap : _VariantMap<std::string, int, std::vector<int>, std::vector<std::string> > {};

#endif