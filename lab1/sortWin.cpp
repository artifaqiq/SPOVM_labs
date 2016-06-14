#include <iostream>
#include <Windows.h>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "shmemory.h"

const int ARR_SIZE = 150;

void randomizeIntArray(int* array, int size);

int main()
{
	int* arraySource = new int[ARR_SIZE];
	int sizeSort = ARR_SIZE / 2 + (ARR_SIZE % 2 == 1 ? 1 : 0);
	int* arrayTemp = new int[sizeSort];
	randomizeIntArray(arraySource, ARR_SIZE);
	std::cout << "Source array: " << std::endl;
	for (int i = 0; i < ARR_SIZE; i++) {
		std::cout << std::setw(5) << arraySource[i];
	}
	std::cout << std::endl;

	ShMemory shm;
	try {
		int* shmPtr = (int*)shm.create(223, ARR_SIZE - sizeSort + 1);
		shmPtr[0] = ARR_SIZE - sizeSort;
		for (int i = sizeSort, j = 1; i < ARR_SIZE; i++, j++) {
			shmPtr[j] = arraySource[i];
		}

		STARTUPINFOA stInfo;
		PROCESS_INFORMATION pInf;
		ZeroMemory(&stInfo, sizeof(stInfo));
		stInfo.cb = sizeof(stInfo);
		ZeroMemory(&pInf, sizeof(pInf));
		if (CreateProcessA(nullptr,
			const_cast<LPSTR>("D:\\Study\\SSoC\\Labs\\2\\ConsoleApplication1\\Debug\\ConsoleApplication1.exe"),
			NULL,
			NULL,
			false,
			NULL,
			NULL,
			NULL,
			&stInfo,
			&pInf) == false) {
			std::cerr << "Error. CreateProcess() error. last error: " << GetLastError() << std::endl;
			shm.remove();
			return 1;
		}

		std::sort(arraySource, arraySource + sizeSort);
		for (int i = 0; i < sizeSort; i++) {
			arrayTemp[i] = arraySource[i];
		}
		
		WaitForSingleObject(pInf.hProcess, INFINITE);
		CloseHandle(pInf.hThread);
		CloseHandle(pInf.hProcess);

		for (int i = 0, j = 1, k = 0; k < ARR_SIZE; k++) {
			if (i == sizeSort) {
				arraySource[k] = shmPtr[j++];
			}
			else if (j == ARR_SIZE - sizeSort + 1) {
				arraySource[k] = arrayTemp[i++];
			}
			else {
				if (arrayTemp[i] <= shmPtr[j]) {
					arraySource[k] = arrayTemp[i++];
				}
				else {
					arraySource[k] = shmPtr[j++];
				}
			}
		}

		std::cout << "Sorted array: " << std::endl;
		for (int i = 0; i < ARR_SIZE; i++) {
			std::cout << std::setw(5) << arraySource[i];
		}
	}
	catch (ShMemory::Exception) {
	}
	delete[] arraySource;
	if (arrayTemp)
		delete[] arrayTemp;
	shm.remove();
	return 0;
}

void randomizeIntArray(int* array, int size)
{
	srand(time(NULL));
	for (int i = 0; i < size; i++) {
		array[i] = rand() % 10000;
	}
}
