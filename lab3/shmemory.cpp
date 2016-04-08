#include "shmemory.h"

ShMemory::ShMemory(){}

void* ShMemory::create(int key, int size)
{ 
#ifdef __linux__
  key_t ftokKey  = ftok("/tmp/", key);
  mShmId = shmget(ftokKey, size, IPC_CREAT | IPC_EXCL | 0666);
  if (mShmId == -1) {
    throw Exception("ShMemory::create(). shmget() err");
  }  
  mShmAddr = shmat(mShmId, nullptr, 0);
  if(mShmAddr == (void*) - 1) {
      shmctl(mShmId, IPC_RMID, nullptr);
      throw Exception("ShMemory::create(). shmat() err");
  }
  return mShmAddr;
  
#elif _WIN32
  #pragma warning (disable:4996)

  char name[20];
  sprintf(name, "%d", key);
  mhMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
  if (GetLastError() == ERROR_ALREADY_EXISTS || mhMap == NULL) {
    throw Exception("CreateFileMappingA() error");
  }
  mShmAddr = MapViewOfFile(mhMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
  if(mShmAddr == NULL) {
    throw Exception("MapViewOfFile() error");
  }
  return mShmAddr;
#endif
}

void ShMemory::remove()
{
#ifdef __linux__
  shmctl(mShmId, IPC_RMID, nullptr);
#elif _WIN32
  CloseHandle(mhMap);
#endif
}

void* ShMemory::attach(int key)
{
#ifdef __linux__
  key_t ftokKey  = ftok("/tmp", key);
  mShmId = shmget(ftokKey, 0, 0);
  if (mShmId == -1) {
    throw Exception("ShMemory::attach(). shmget() err");
  }
  mShmAddr = shmat(mShmId, nullptr, 0);
  if(mShmAddr == (void*) - 1) {
      shmctl(mShmId, IPC_RMID, nullptr);
      throw Exception("ShMemory()::attach(). shmat() err");
  }
  return mShmAddr;
#elif _WIN32
  char name[20];
  sprintf(name, "%d", key);

  mhMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, true, name);
  if (mhMap == NULL) {
    throw Exception("OpenFileMappingA() error");
  }
  mShmAddr = MapViewOfFile(mhMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
  if (mShmAddr == NULL) {
    throw Exception("MapViewOfFile() error");
  }
  return mShmAddr;
#endif
}

void ShMemory::detach()
{
#ifdef __linux__
  shmdt(mShmAddr);
#elif _WIN32
  CloseHandle(mhMap);
#endif
}

void* ShMemory::getAddr()
{
  return mShmAddr;
}

int ShMemory::getSize()
{
#ifdef __linux__
  shmid_ds data;
  if(-1 == shmctl(mShmId, IPC_STAT, &data)) {
    throw Exception("ShMemory::getSize() shmctl error");
  }
  return data.shm_segsz;
#elif _WIN32
	return mSize;
#endif
}



