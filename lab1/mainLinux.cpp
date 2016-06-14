#include "shmemory.h"

#include <sys/wait.h>    

#include <iomanip>
#include <algorithm>

using namespace std;

const int ARR_SIZE = 100;

void randomizeIntArray(int* array, int size);

int main()
{
  int* arraySource = new int[ARR_SIZE];
  randomizeIntArray(arraySource, ARR_SIZE);
  std::cout << "Source array: " << std::endl;
  for(int i = 0; i < ARR_SIZE; i++) {
    std::cout << std::setw(5) << arraySource[i];
  }
  std::cout << std::endl;
  int sizeSort = ARR_SIZE / 2 + (ARR_SIZE % 2 == 1 ? 1 : 0);

  int childPid = fork();
  switch(childPid) {
  case -1:
    delete[] arraySource;
    err(-1, "fork() error. errno = %x", errno);
  case 0: {
    try {
      ShMemory shmChild;
      std::sort(arraySource + ARR_SIZE - sizeSort, arraySource + ARR_SIZE);

      int* shmAddr = (int*)shmChild.attach(2);
      for(int i = 0, j = ARR_SIZE - sizeSort; i < ARR_SIZE - sizeSort; i++, j++) {
        shmAddr[i] = arraySource[j];
      }
    } catch (ShMemory::Exception) {
    }
    delete[] arraySource;
    return 0;
  }
  default: {
    int* arrayTemp = new int[sizeSort];
    ShMemory shmParent;
    try {
      int* shmPtr = (int*)shmParent.create(2, ARR_SIZE - sizeSort);
      std::sort(arraySource, arraySource + sizeSort);
      for(int i = 0; i < sizeSort; i++) {
        arrayTemp[i] = arraySource[i];
      }

      int exitCode;
      waitpid(childPid, &exitCode, 0);

      for(int i = 0, j = 0, k = 0; k < ARR_SIZE; k++) {
        if(i == sizeSort) {
          arraySource[k] = shmPtr[j++];
        } else if (j == ARR_SIZE - sizeSort) {
          arraySource[k] = arrayTemp[i++];
        }
        else {
          if(arrayTemp[i] <= shmPtr[j]) {
            arraySource[k] = arrayTemp[i++];
          } else {
            arraySource[k] = shmPtr[j++];
          }
        }
      }

      std::cout << "Sorted array: " << std::endl;
      for(int i = 0; i < ARR_SIZE; i++) {
        std::cout << std::setw(5) << arraySource[i];
      }
      std::cout << std::endl;
      } catch (ShMemory::Exception) {
      }
      shmParent.remove();
      delete[] arrayTemp;
      delete[] arraySource;
  }
  }
  return 0;
}

void randomizeIntArray(int* array, int size) {
  srand(time(NULL));
  for(int i = 0; i < size; i++) {
      array[i] = rand() % 100;
    }
}

