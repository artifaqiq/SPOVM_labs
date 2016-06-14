#include <iostream>
#include <Windows.h>

#include "stupidptr.hpp"

void fun();

int main()
{
	StupidPtr<int> x(5);
	StupidPtr<int> y(20);
	std::cout << " :: x(5) and y(20) : " << std::endl;
	std::cout << StupidPtr<int>::get_memory_map() << std::endl;

	x = y;
	std::cout << " :: x = y : " << std::endl;
	std::cout << StupidPtr<int>::get_memory_map() << std::endl;

	fun();
	std::cout << " :: after fun() : " << std::endl;
	std::cout << StupidPtr<int>::get_memory_map() << std::endl;

	StupidPtr<int>::gc();
	std::cout << " :: defragmentation and garbage collector : " << std::endl;
	std::cout << StupidPtr<int>::get_memory_map() << std::endl;

	Sleep(500);
	return 0;
}

void fun()
{
	StupidPtr<int> x(3);
	StupidPtr<int> y(6);
	StupidPtr<int> z(9);
	std::cout << " :: fun() : x(3), y(6), z(9)" << std::endl;
	std::cout << StupidPtr<int>::get_memory_map() << std::endl;
}