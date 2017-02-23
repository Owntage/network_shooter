#ifndef VARIANT_MAP_H
#define VARIANT_MAP_H

#include <map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <set>

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
        template<typename T>
        void set(const std::string& key, T value)
        {
            //VariantMapPart<T>::m[key] = value;

            keyToType[key] = getTypeIndex<T>();
            typeToKeys[getTypeEquivalenceClass<T>()].insert(key);
            SetFunctorInvoker<T> setFunctorInvoker(key, value);
            callByTypeIndex<decltype(setFunctorInvoker), SetFunctor>(setFunctorInvoker, getTypeEquivalenceClass<T>());
        }

        template<typename T>
        T& get(const std::string& key)
        {
            return VariantMapPart<T>::m[key];
        }

        template<typename StreamType>
        friend StreamType& operator<<(StreamType& s, _VariantMap& m)
        {

            int numberOfTypes = sizeof... (Types);
            for(int i = 0; i < numberOfTypes; i++)
            {
                s << (int32_t) m.typeToKeys[i].size();
                for(auto it = m.typeToKeys[i].begin(); it != m.typeToKeys[i].end(); it++)
                {
                    StreamFunctorInvoker<StreamType> invoker(*it, s);
                    m.callByTypeIndex<decltype(invoker), ToStreamFunctor>(invoker, i);
                }
            }
            return s;
        }

        template<typename StreamType>
        friend StreamType& operator>>(StreamType& s, _VariantMap& m)
        {
            int numberOfTypes = sizeof... (Types);
            for(int i = 0; i < numberOfTypes; i++)
            {
                int numberOfKeys = 0;
                s >> numberOfKeys;
                for(int j = 0; j < numberOfKeys; j++)
                {
                    std::string key;
                    s >> key;
                    StreamFunctorInvoker<StreamType> invoker(key, s);
                    m.callByTypeIndex<decltype(invoker), FromStreamFunctor>(invoker, i);
                }
            }
            return s;
        }

    private:
        std::map<std::string, int> keyToType;
        std::map<int, std::set<std::string> > typeToKeys;

        template<typename StreamType>
        struct StreamFunctorInvoker
        {
            StreamFunctorInvoker(const std::string& key, StreamType& stream) : stream(stream), key(key) {}
            StreamType& stream;
            const std::string& key;

            template<typename Functor>
            void operator()(Functor& functor)
            {
                functor(key, stream);
            }
        };


        template<typename T>
        struct ToStreamFunctor
        {
            ToStreamFunctor(_VariantMap& parent) : parent(parent) {}
            _VariantMap& parent;

            template<typename StreamType>
            void operator()(const std::string& key, StreamType& s)
            {
               s << key << parent.VariantMapPart<T>::m[key];
            }
        };

        template<typename T>
        struct FromStreamFunctor
        {
            FromStreamFunctor(_VariantMap& parent) : parent(parent) {}
            _VariantMap& parent;

            template<typename StreamType>
            void operator()(const std::string& key, StreamType& s)
            {
                T t;
                s >> t;
                //parent.VariantMapPart<T>::m[key] = t;
                parent.set(key, t);
            }
        };

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

        template<typename CheckingType>
        int getTypeIndex()
        {
            return getTypeIndexInternal<CheckingType, Types...>();
        }

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

struct VariantMap : _VariantMap<std::string, int> {};

#endif