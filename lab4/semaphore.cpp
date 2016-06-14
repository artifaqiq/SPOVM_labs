#include "semaphore.h"

Semaphore::Semaphore(){}

void Semaphore::create(int keyValue)
{
#ifdef __linux__
  int key = ftok("/tmp", keyValue);
  if(key == -1) {
      throw Exception("Semaphore::create() ftok() error");
  }
  mSemId = semget(key, 1, IPC_CREAT | 0666);
  if(-1 == mSemId) {
    throw Exception("Semaphore::create() semget() error");
  }
#elif _WIN32
  char name[20];
  sprintf(name, "%d", keyValue);
  mHsem = CreateSemaphoreA(NULL, 0, 1, name);
  if (GetLastError() == ERROR_ALREADY_EXISTS || mHsem == NULL) {
    throw Exception("Semaphore::create() CreateSemaphore() error");
  }
#endif
}

void Semaphore::attach(int keyValue)
{
#ifdef __linux__
  int key = ftok("/tmp", keyValue);
  if(key == -1) {
    throw Exception("Semaphore::create() ftok() error");
  }
  mSemId  = semget(key, 0, 0);
  if(mSemId == -1) {
    throw Exception("semget() error");
  }
#elif _WIN32
  char name[20];
  sprintf(name, "%d", keyValue);
  mHsem = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, true, name);
  if (mHsem == NULL) {
    throw Exception("Semaphore::attach() OpenSemaphore() error");
  }
#endif
}

void Semaphore::remove()
{
#ifdef __linux__
  if(-1 == semctl(mSemId, -1, IPC_RMID)) {
      throw Exception("Semapgore::remove() semctl() error");
   }
#elif _WIN32
  CloseHandle(mHsem);
#endif
}

void Semaphore::waitNotZero(int mSec)
{
#ifdef __linux__
  struct sembuf semBufWait;
  semBufWait.sem_flg = 0;
  semBufWait.sem_num = 0;
  semBufWait.sem_op = -1;
  if(-1 == mSec) {
    if(-1 == semop(mSemId, &semBufWait, 1)) {
      throw Exception("Semaphore::waitNotZero semop() error");
    }
  } else {
    struct timespec time = {
      0, mSec * 1000000
    };
    if(-1 == semtimedop(mSemId, &semBufWait, 1, &time)) {
      throw Exception("Semaphore::waitNotZero semop() error");
    }
  }
#elif _WIN32
  if(mSec == -1) {
    if(WAIT_FAILED == WaitForSingleObject(mHsem, INFINITE)) {
      throw Exception("Semaphore::waitNotZero WaitForSingleObject() error");
    }
  } else {
      if(WAIT_FAILED == WaitForSingleObject(mHsem, mSec * 1000)) {
        throw Exception("Semaphore::waitNotZero WaitForSingleObject() error");
      }
  }
#endif
}

void Semaphore::op(int value)
{
#ifdef __linux__
  struct sembuf semBufWait;
  semBufWait.sem_flg = 0;
  semBufWait.sem_num = 0;
  semBufWait.sem_op = value;
  if(-1 == semop(mSemId, &semBufWait, 1)) {
    throw Exception("Semaphore::setValue semop() error");
  }
#elif _WIN32
  if(0 == ReleaseSemaphore(mHsem, value, NULL)) {
    throw Exception("Semaphore::setValue ReleaseSemaphore() error");
  }
#endif
}
