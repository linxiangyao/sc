#ifndef S_STL_COMM_H_
#define S_STL_COMM_H_
#include <string>
#include <map>
#include <vector>
#include "ns.h"
S_NAMESPACE_BEGIN



template<typename T>
T min(T t1, T t2)
{
	return t1 < t2 ? t1 : t2;
}

template<typename T>
T max(T t1, T t2)
{
	return t1 < t2 ? t2 : t1;
}



// all collections
template<typename CollectionT>
void delete_and_erase_collection_elements(CollectionT* c)
{
	if (c == NULL)
		return;
	for (typename CollectionT::iterator it = c->begin(); it != c->end(); ++it)
	{
		delete *it;
	}
	c->clear();
}




// vector ---------------------------------
template<typename VectorT>
int get_vector_index_by_element(const VectorT& v, typename VectorT::value_type value)
{
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (v[i] == value)
			return (int)i;
	}
	return -1;
}


template<typename VectorT>
void add_vector_unique_element(VectorT* v, typename VectorT::value_type value)
{
	int index = get_vector_index_by_element(*v, value);
	if (index >= 0)
		return;
	v->push_back(value);
}

template<typename VectorT>
void erase_vector_element_by_value(VectorT* v, typename VectorT::value_type value)
{
	int index = get_vector_index_by_element(*v, value);
	if (index < 0)
		return;

	v->erase(v->begin() + index);
}

template<typename VectorT>
void erase_vector_element_by_index(VectorT* v, int index)
{
	if (index < 0 || index > (int)v->size())
		return;
	v->erase(v->begin() + index);
}

template<typename VectorT>
bool is_vector_contain_element(const VectorT& v, typename VectorT::value_type value)
{
	return get_vector_index_by_element(v, value) >= 0;
}


template<typename VectorT>
void delete_and_erase_vector_element_by_index(VectorT* v, int index)
{
	if (index < 0 || index >= (int)v->size())
		return;
	delete (*v)[index];
	v->erase(v->begin() + index);
}


template<typename VectorT>
void delete_and_erase_vector_element_by_value(VectorT* v, typename VectorT::value_type value)
{
	int index = get_vector_index_by_element(*v, value);
	delete_and_erase_vector_element_by_index(v, index);
}


template<typename VectorT>
void delete_and_erase_vector_element_by_indexs(VectorT* v, std::vector<size_t> indexs)
{
	for (int i = (int)indexs.size() - 1; i >= 0; --i)
	{
		delete_and_erase_vector_element_by_index(v, indexs[i]);
	}
}




// map ------------------------------------------------------------
template<typename MapT>
void add_map_to_map(const MapT& m, MapT* to_m)
{
	for (typename MapT::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		(*to_m)[it->first] = it->second;
	}
}


template<typename KeyT, typename ValueT>
void delete_and_erase_collection_elements(std::map<KeyT, ValueT>* c)
{
	if (c == NULL)
		return;
	for (typename std::map<KeyT, ValueT>::iterator it = c->begin(); it != c->end(); ++it)
	{
		delete it->second;
	}
	c->clear();
}


template<typename MapT>
void delete_and_erase_map_element_by_key(MapT* m, typename MapT::key_type k)
{
	typename MapT::iterator it = m->find(k);
	if (it == m->end())
		return;

	delete it->second;
	m->erase(it);
}


template<typename MapT, typename VectorT>
void delete_and_erase_map_elements_by_keys(MapT* m, const VectorT& ks)
{
	for (size_t i = 0; i < ks.size(); ++i)
	{
		delete_and_erase_map_element_by_key(m, ks[i]);
	}
}


template<typename MapT>
bool is_map_contain_key(const MapT& m, typename MapT::key_type k)
{
	typename MapT::const_iterator it = m.find(k);
	if (it == m.end())
		return false;
	return true;
}


template<typename KeyT, typename ValueT>
ValueT* get_map_element_by_key(std::map<KeyT, ValueT*>& m, KeyT k)
{
	typename std::map<KeyT, ValueT*>::iterator it = m.find(k);
	if (it == m.end())
		return NULL;

	return it->second;
}


template<typename KeyT, typename ValueT>
void clone_map(const std::map<KeyT, ValueT>& from_map, std::map<KeyT, ValueT*>* to_map)
{
	for (typename std::map<KeyT, ValueT>::const_iterator it = from_map.begin(); it != from_map.end(); ++it)
	{
		ValueT* v = new ValueT();
		*v = it->second;
		(*to_map)[it->first] = v;
	}
}


template<typename KeyT, typename ValueT>
void copy_map_values_to_vector(const std::map<KeyT, ValueT>& from_map, std::vector<ValueT>* to_vector)
{
	for (typename std::map<KeyT, ValueT>::const_iterator it = from_map.begin(); it != from_map.end(); ++it)
	{
		to_vector->push_back(it->second);
	}
}









S_NAMESPACE_END
#endif



