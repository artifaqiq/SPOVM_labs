#pragma once

template<class T>
class MemBlock
{
public:
	MemBlock(T* begin, size_t size): addr(begin), size(size) {count_ref = 1;}

	T* get_addr() const { return addr; }

	void set_addr(T* addr) { this->addr = addr; }

	int get_count_ref() const { return count_ref; }

	void inc_count_ref() { count_ref++; }

	void dec_count_ref() { count_ref--; }
	
	size_t get_size() const { return size; }
protected:
	T* addr;
	size_t size;
	int count_ref;
};