#pragma once

#include "mempage.hpp"

#include <vector>

template<class T>
class StupidPtr
{
public:
	StupidPtr<T>(size_t size) 
	{ 
		addr = page.alloc_block(size); 
	}

	virtual StupidPtr<T>& operator=(StupidPtr<T>& obj)
	{
		page.inc_count_ref(obj.addr);
		page.dec_count_ref(addr);
		return *this;
	}

	virtual ~StupidPtr()
	{
		if (addr) 
			page.dec_count_ref(addr);
	}

	static void defragmentation() { page.defragmentation(); gc(); }

	static void gc() { page.defragmentation(); page.defragmentation(); }

	static std::string get_memory_map() { return page.to_memory_map(); }

protected:
	static MemPage<T> page;

	T* addr;

};

template<class T>
MemPage<T> StupidPtr<T>::page;