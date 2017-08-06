#ifndef NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H
#define NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H

template<typename Type>
struct VectorWrapper
{
	std::vector<Type> vec;

	template<typename StreamType>
	friend StreamType& operator<<(StreamType& s, VectorWRapper<Type>& v)
	{
		s << (int) vec.size();
		for (int i = 0; i < v.vec.size(); i++)
		{
			s << v.vec[i];
		}
		return s;
	}

	template<typename StreamType>
	friend StreamType& operator>>(StreamType& s, VectorWRapper<Type>& v)
	{
		int size;
		s >> size;
		for (int i = 0; i < size; i++)
		{
			s >> v.vec[i];
		}
		return s;
	}
};

#endif //NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H
