#include "thread.h"
#include "serverftp.h"

ShMemory shmKey;
Semaphore semSess;

#define MAX_INT 0xFFFFFFC

int key = 10;
int generateKey()
{
  if (key >= MAX_INT - 3) {
    key = 10;
  } else {
    key += 3;
  }
  return key;
}

int getKey() {
  return key;
}

#ifdef __linux__
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>

void handlerOnTerm(int signum)
{
  puts("\n\n ### Keyboard interrupt. All IPCs removed\n");
  shmKey.remove();
  semSess.remove();
  exit(EXIT_SUCCESS);
}

void* run_io(void* data)
{
  server_thread_func(data);
}

#elif _WIN32
DWORD __stdcall run_io(CONST LPVOID data)
{
  server_thread_func(data);
  return 0;
}

#endif

int main()
{
  std::cout << " -- start server" << std::endl;

#ifdef __linux__
  /*set handler on SIGINT (Ctrl + C - correct stop server)*/
  struct sigaction actOnInt;
  actOnInt.sa_handler = handlerOnTerm;
  sigaction(SIGINT, &actOnInt, nullptr);
#endif

  try {
    int* ptrShmKey = (int*)shmKey.create(SHM_KEY_KEY, sizeof(int));
    semSess.create(SEM_SESS_KEY);

    while (true) {
      *ptrShmKey = generateKey();
      semSess.waitNotZero();

      int keyThread = getKey();
#ifdef _WIN32
      Thread().startThread(run_io, reinterpret_cast<void*>(&keyThread));
#elif __linux__
      Thread().startThread(run_io, reinterpret_cast<void*>(&keyThread));
#endif
    }
  } catch(ShMemory::Exception) {
    shmKey.remove();
    semSess.remove();
    return EXIT_FAILURE;
  } catch(Semaphore::Exception) {
    semSess.remove();
    shmKey.remove();
    return EXIT_FAILURE;
  }
}
