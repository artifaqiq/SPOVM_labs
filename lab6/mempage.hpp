#pragma once
#pragma warning (disable: 4996)

#include <vector>
#include <string>
#include <sstream>

#include "memblock.hpp"

inline void contraction(char* dest, char* source, size_t size)
{
	for (int i = 0; i < size; i++) {
		dest[i] = source[i];
	}
}

template<class T, size_t page_size = 0x10'000>
class MemPage
{
public:
	MemPage<T, page_size>()
	{
		start_addr = (T*)malloc(page_size);
		head = 0;
	}

	T* alloc_block(size_t size)
	{
		if (size < 1)
			throw std::exception("size < 1");
		if ((head + size) * sizeof(T) > page_size) {
			return nullptr;
		}
		
		MemBlock<T> block(&start_addr[head], size);
		blocks_pool.push_back(block);

		head += size;

		return blocks_pool.back().get_addr();
	}

	void free_block(T* addr)
	{
		for (auto it = blocks_pool.begin(); it != blocks_pool.end(); it++) {
			if (it->get_addr() == addr) {

				if (std::distance(blocks_pool.end(), it) == -1) {
					head -= it->get_size();
				}
				blocks_pool.erase(it);
				break;
			}
		}
	}
	
	void defragmentation()
	{
		char* last_addr = (char*)start_addr;
		for (auto it = blocks_pool.begin(); it != blocks_pool.end(); it++) {
			size_t from_begin_page_to_current_block = 0;
			for (auto jt = blocks_pool.begin(); jt != it; jt++) {
				from_begin_page_to_current_block += jt->get_size() * sizeof(T);
			}

			if (last_addr != (char*)it->get_addr()) {
				int delta = (int)((char*)it->get_addr() - last_addr);

				for (auto jt = it; jt != blocks_pool.end(); jt++) {
					jt->set_addr((T*)((char*)jt->get_addr() - delta));
				}

				contraction(last_addr, (char*)it->get_addr(), page_size - from_begin_page_to_current_block);
			}

			if (it->get_count_ref() == 0) {		
				int delta = it->get_size() * sizeof(T);

				auto jt = it;
				jt++;
				for (jt; jt != blocks_pool.end(); jt++) {
					jt->set_addr((T*)((char*)jt->get_addr() - delta));
				}

				contraction(last_addr, (char*)it->get_addr() + it->get_size(), page_size - from_begin_page_to_current_block - it->get_size() * sizeof(T));
				
				it = blocks_pool.erase(it);
				if (it == blocks_pool.end())
					break;
			}
			last_addr = (char*)it->get_addr() + it->get_size() * sizeof(T);
			
		}
	}
	MemBlock<T> get_blocks_pool(int indx) { return blocks_pool; }

	std::string to_string() const
	{
		std::stringstream res;
		res << "head = " << head << std::endl;
		res << "head_addr = " << &start_addr[head] << std::endl;
		res << "blocks = " << blocks_pool.size() << std::endl;
		return res.str();
	}

	std::string to_memory_map() const
	{
		std::stringstream res;
		T* last_addr = start_addr;
		for (auto it = blocks_pool.begin(); it != blocks_pool.end(); it++) {
			for (int j = 0; j < it->get_addr() - last_addr; j++) {
				res << "f ";
			}
			if (it->get_addr() != last_addr)
				res << "| ";


			for (int j = 0; j < it->get_size(); j++) {
				res << it->get_count_ref() << " ";
			}
			last_addr = it->get_addr() + it->get_size();
			res << "| ";
		}
		res << " . . . " << std::endl;
		return res.str();
	}

	void inc_count_ref(T* addr)
	{
		for (auto it = blocks_pool.begin(); it != blocks_pool.end(); it++) {
			if (it->get_addr() == addr) {
				it->inc_count_ref();
				return;
			}
		}
	}

	void dec_count_ref(T* addr)
	{
		for (auto it = blocks_pool.begin(); it != blocks_pool.end(); it++) {
			if (it->get_addr() == addr) {
				it->dec_count_ref();
				return;
			}
		}
	}

protected:
	T* start_addr;
	std::vector<MemBlock<T>> blocks_pool;

	int head;
};