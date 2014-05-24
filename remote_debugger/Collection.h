#ifndef COLLECTION_H
#define COLLECTION_H

#include "OSTypes.h"
#include <map>
#include <list>

#include "Process.h"

template <class Key_T,
		  class Value_T>
class Collection
{
public:

	class Iterator
	{
	public:

		Iterator(std::map<Key_T,Value_T> * map)
		{
			std::map<Key_T,Value_T>::const_iterator it;

			for(it = map->begin(); it != map->end(); ++it){
				the_list.push_back( (*it).second );
			}
		}

		~Iterator()
		{
			the_list.clear();
		}

		virtual Value_T Get()
		{
			return (Value_T)the_list.front();
		}

		virtual Value_T Next()
		{
			Value_T v = NULL;
			the_list.pop_front();
			if ( !the_list.empty() )
			{
				v = Get();
			}
			return v;
		}

		virtual BOOL IsEmpty()
		{
			return the_list.empty();
		}

	private:
		std::list<void*> the_list;
	};

	Collection()
	{
		items = new std::map<Key_T, Value_T>;
	}

	~Collection()
	{
		this->items->clear();
		delete this->items;
		items = NULL;
	}

	virtual BOOL Contains(Key_T k)
	{
		BOOL ret = FALSE;

		if(items->find(k) != this->items->end())
		{
			ret = TRUE;
		}

		return ret;
	}

	virtual Value_T GetItem(Key_T k)
	{
		Value_T p = NULL;

		if ( this->Contains(k) == TRUE )
		{
			//p = this->items[k];
			p = (*this->items->find(k)).second;
		}

		return p;
	}

	virtual BOOL AddItem(Key_T k, Value_T v)
	{
		BOOL ret = FALSE;

		if (  v != NULL && this->Contains(k) == FALSE )
		{
			this->items->insert( std::pair<Key_T,Value_T>(k,v) );
			ret = TRUE;
		}

		return ret;
	}

	virtual BOOL Remove(Key_T k)
	{
		BOOL ret = FALSE;

		if ( this->Contains(k) == TRUE)
		{
			this->items->erase(k);
			ret = TRUE;
		}

		return ret;
	}

	virtual BOOL IsEmpty()
	{
		return this->items->size() == 0;
	}

	virtual Iterator GetIterator()
	{
		Iterator i(items);
		return i;
	}

protected:

	std::map<Key_T, Value_T> * items;

};

#endif//COLLECTION_H