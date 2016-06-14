#include "shmemory.h"
#include "semaphore.h"

#include <cstring>
#include <getopt.h>

int ls(ShMemory shm, Semaphore semCli, Semaphore semServ);
int putfile(const char* fname, ShMemory shm, Semaphore semCli, Semaphore semServ);
int getfile(const char* fname, ShMemory shm, Semaphore semCli, Semaphore semServ);
int rmfile(const char* fname, ShMemory shm, Semaphore semCli, Semaphore semServ);
void help();

const char* SHORT_OPTIONS = "p:g:r:lh";

const struct option LONG_OPTIONS[] = {
  {"putfile",required_argument,NULL,'p'},
  {"getfile",required_argument,NULL,'g'},
  {"rmfile",required_argument,NULL,'r'},
  {"ls", no_argument, NULL, 'l'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};

const int SHM_KEY = 2;
const int SEM_SESS_KEY = 3;
const int SEM_CLI_KEY = 4;
const int SEM_SERV_KEY = 5;
const int SHM_SIZE = 8192;

int main (int argc, char *argv[]) {
  ShMemory shm;
  Semaphore semCli, semServ;
  try {
    shm.attach(SHM_KEY);
    semCli.attach(SEM_CLI_KEY);
    semServ.attach(SEM_SERV_KEY);

    /*Parsing command arg*/
    char cmd;
    int option_index;
    while (-1 != (cmd = getopt_long(argc, argv, SHORT_OPTIONS,
              LONG_OPTIONS, &option_index))) {
      switch(cmd) {
        case 'p': {
          putfile(optarg, shm, semCli, semServ);
          break;
        }
        case 'g': {
          getfile(optarg, shm, semCli, semServ);
          break;
        }
        case 'r': {
          rmfile(optarg, shm, semCli, semServ);
          break;
        }
        case 'l': {
          ls(shm, semCli, semServ);
          break;
        }
        case 'h': {
          help();
          break;
        }
        case '?':
        default: {
          printf("found unknown option\n");
          shm.detach();
          exit(EXIT_SUCCESS);
        }
      }
    }
  } catch(Semaphore::Exception) {
    std::cout << "Server is offline" << std::endl;
    shm.detach();
    return EXIT_FAILURE;
  } catch(ShMemory::Exception) {
    shm.detach();
    std::cout << "Server is offline" << std::endl;
    return EXIT_FAILURE;
  }

  //On windows remows semaphors
  shm.detach();
  return EXIT_SUCCESS;
}

int ls(ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  try {
    semCli.waitNotZero();

    char* ptr = (char*)shm.getAddr();
    ptr[0] = 'l';

    semServ.op(1);
    semCli.waitNotZero();

    int indx = 0;
    while(true) {
      std::cout <<":: ";
      for(; ptr[indx] != '\0'; indx++) {
        std::cout << ptr[indx];
      }
      std::cout << std::endl;
      if(ptr[++indx] == -1) {
        break;
      } else if (ptr[indx] == '\0') {
        semServ.op(1);
        indx = 0;
      }
    }

  } catch(Semaphore::Exception) {
    return -1;
  } catch(ShMemory::Exception) {
    return -1;
  }
  return 0;
}

int putfile(const char* fname, ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  FILE* f;
  if((f = fopen(fname, "rb")) == NULL) {
    std::cout << "-- " << fname << " not found" << std::endl;
    return 1;
  }
  fclose(f);

  FILE* sourceFile;
  try {
    semCli.waitNotZero();

    char* ptr = (char*)shm.getAddr();
    int* ptrInt = (int*)shm.getAddr();
    ptr[0] = 'p';
    sprintf(ptr + 1, "%s", fname);
    ptr[strlen(fname) + 1] = '\0';

    semServ.op(1);

    sourceFile = fopen(fname, "rb");
    if (sourceFile == NULL) {
      std::cerr << "putfile() fopen() error. errno = " << errno << std::endl;
      return -1;
    }

    semCli.waitNotZero();

    if(ptr[0] == -1) {
      std::cout << "-- " << fname << " already exists" << std::endl;
      return 0;
    }

    int fullSize = 0;
    int size;
    while(true) {
      size = fread(ptr + sizeof(int), 1, SHM_SIZE - sizeof(int), sourceFile);
      fullSize += size;
      std::cout << " << " << fullSize / 1024 <<" KB unloaded\r";
      if(size < 0) {
        std::cerr << "putfile() read() error. errno = " << errno << std::endl;
        return -1;
      } else if (size < SHM_SIZE - sizeof(int)) {
        ptrInt[0] = size;
        semServ.op(1);
        break;
      } else {
        ptrInt[0] = size;
        semServ.op(1);
        semCli.waitNotZero();
      }
    }

  } catch(Semaphore::Exception) {
    fclose(sourceFile);
    return -1;
  } catch(ShMemory::Exception) {
    fclose(sourceFile);
    return -1;
  }
  fclose(sourceFile);
  std::cout << "-- " << fname <<" has been successfully unloaded" << std::endl;
  return 0;
}

int getfile(const char *fname, ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  FILE* destFile;
  try {
    semCli.waitNotZero();

    char* ptr = (char*)shm.getAddr();
    int* ptrInt = (int*)shm.getAddr();

    ptr[0] = 'g';
    sprintf(ptr + 1, "%s", fname);
    ptr[strlen(fname) + 1] = '\0';

    semServ.op(1);
    semCli.waitNotZero();

    if(ptrInt[0] == -1) {
      std::cout << "-- " << fname << " doesn't exists" <<std::endl;
      return 1;
    }

    destFile = fopen(fname, "wb");
    if (destFile == NULL) {
      std::cerr << "getfile() fopen() error. errno = " << errno << std::endl;
      return -1;
    }

    int size, fullSize = 0;
    while(true) {
      size = ptrInt[0];
      fullSize += size;
      std::cout << " >> " << fullSize / 1024 << " KB downloaded\r";
      fwrite(ptr + sizeof(int), 1, size, destFile);
      if(size < SHM_SIZE - sizeof(int)) {
        break;
      }
      semServ.op(1);
      semCli.waitNotZero();
    }
  } catch(ShMemory::Exception) {
    fclose(destFile);
    return -1;
  } catch(Semaphore::Exception) {
    fclose(destFile);
    return -1;
  }
  fclose(destFile);
  std::cout << "-- " << fname <<" downloaded successfull" << std::endl;
  return 0;
}

int rmfile(const char* fname, ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  try {
    semCli.waitNotZero();

    char* ptr = (char*)shm.getAddr();
    ptr[0] = 'r';
    sprintf(ptr + 1, "%s", fname);
    ptr[strlen(fname) + 1] = '\0';

    semServ.op(1);
    semCli.waitNotZero();
    if(ptr[0] == 0) {
      std::cout << "-- " << fname << " removed successfulld" << std::endl;
      return 0;
    } else if (ptr[0] == 1) {
      std::cout << "-- " << fname << " not found" << std::endl;
      return 1;
    } else {
      std::cout << "-- " << fname << " can not be removed" << std::endl;
      return 2;
    }
  } catch (ShMemory::Exception) {
    return -1;
  } catch (Semaphore::Exception) {
    return -1;
  }
}

void help()
{
  std::cout << "usage: client option [file]" << std::endl;
  std::cout << "\n\t-h --help           show this message" << std::endl;
  std::cout << "\n\t-r --rmfile         remove file on the server" << std::endl;
  std::cout << "\n\t-p --putfile=<file> unload file from the server" << std::endl;
  std::cout << "\n\t-g --getfile=<file> load the file from the server" << std::endl;
}
