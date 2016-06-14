#pragma warning (disable:4996)

#include <iostream>
#include <Windows.h>
#include <algorithm>

#include "shmemory.h"

using namespace std;

const int SHM_SIZE = 4096;

int main()
{
	ShMemory shm;
	try {
		int* shmPtr = (int*)shm.attach(223);
		int size = shmPtr[0];
		sort(shmPtr + 1, shmPtr + 1 + size);
	}	catch (ShMemory::Exception) {}
	shm.detach();
	return 0;
}
