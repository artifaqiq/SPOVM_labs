#include "serverftp.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>

int get_error_code()
{
#ifdef _WIN32
  return GetLastError();
#elif __linux__
  return errno;
#endif
}

void server_thread_func(void* data)
{
  ShMemory shm;
  Semaphore semCli, semServ;

  int key = *((int*)data);
  char cmd;
  try {
    char* ptr = (char*)shm.create(key, SHM_SIZE);
    semCli.create(key + 1);
    semServ.create(key + 2);
      std::cout << " -- wait clients" << std::endl;
      semCli.op(1);
      semServ.waitNotZero();
      std::cout << " -- one client connected" << std::endl;

      cmd = ptr[0];
      switch(cmd) {
      case 'l':
        if(-1 == ls(shm, semCli, semServ)) {
          semCli.remove();
          semServ.remove();
          shm.remove();
        }
        break;
      case 'p':
        if(-1 == putfile(shm, semCli, semServ)) {
          semCli.remove();
          semServ.remove();
          shm.remove();
        }
        break;
      case 'g':
        if(-1 == getfile(shm, semCli, semServ)) {
          semCli.remove();
          semServ.remove();
          shm.remove();
        }
        break;
      case 'r':
          if(-1 == rmfile(shm, semCli)) {
            semCli.remove();
            semServ.remove();
            shm.remove();
          }
          break;
      default:
          std::cout << " -- incorrect request" << std::endl;
      }
  } catch (ShMemory::Exception) {
    semCli.remove();
    semServ.remove();
    shm.remove();
#ifdef __linux__
    pthread_exit(NULL);
#endif
  } catch (Semaphore::Exception) {
    semCli.remove();
    semServ.remove();
    shm.remove();
#ifdef __linux__
    pthread_exit(NULL);
#endif
  }
  semCli.remove();
  semServ.remove();
  shm.remove();
#ifdef __linux__
  pthread_exit(NULL);
#endif
}

int ls(ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  std::cout << " === request 'LS'" << std::endl;
  try {
    int indx = 0;
    char* ptr = (char*)shm.getAddr();

#ifdef __linux__
    DIR *mydir;
    struct dirent *myfile;
    mydir = opendir(get_current_dir_name());
    while((myfile = readdir(mydir)) != NULL)
    {
      if(0 == strcmp(myfile->d_name, ".") || 0 == strcmp(myfile->d_name, "..")) {
        continue;
      }
      if(SHM_SIZE - indx <= strlen(myfile->d_name) + 1) {
        ptr[indx] = '\0';
        semCli.op(1);
        semServ.waitNotZero();
        indx = 0;
      }
      sprintf(ptr + indx, "%s", myfile->d_name);
      indx += strlen(myfile->d_name) + 1;
      ptr[indx - 1] = '\0';
    }

    ptr[indx] = EOF;

    closedir(mydir);
    std::cout << std::endl;

#elif _WIN32
    char currentPath[255];
    if(GetCurrentDirectoryA(255, currentPath) == 0) {
      semCli.op(1);
      std::cout << "GetCurrentDirectory() error" << std::endl;
      return -1;
    }
    strcat(currentPath, "\\*");

    WIN32_FIND_DATAA findFileData;
    HANDLE hFile;
    hFile = FindFirstFileA(currentPath, &findFileData);
    if (hFile != INVALID_HANDLE_VALUE){
      do {
          if(0 == strcmp(findFileData.cFileName, ".") || 0 == strcmp(findFileData.cFileName, "..")) {
            continue;
          }
          if(SHM_SIZE - indx <= strlen(findFileData.cFileName) + 1) {
            ptr[indx] = '\0';
            semCli.op(1);
            semServ.waitNotZero();
            indx = 0;
          }
          sprintf(ptr + indx, "%s", findFileData.cFileName);
          indx += strlen(findFileData.cFileName) + 1;
          ptr[indx - 1] = '\0';
      } while (FindNextFileA(hFile, &findFileData)!=0);

      FindClose(hFile);
      ptr[indx] = EOF;
    }
#endif
    semCli.op(1);
  } catch(Semaphore::Exception) {
    return -1;
  }
  return 0;
}

int putfile(ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  std::cout << " === request 'PUTFILE' ";
  try {
    char* ptr = (char*)shm.getAddr();
    int* ptrInt = (int*)shm.getAddr();
    char fname[255];
    strcpy(fname, ptr + 1);
    std::cout << fname << std::endl;

    FILE* tempFile;
    if ((tempFile = fopen(fname, "rb")) != NULL) {
      std::cout << " === " << fname << " already exists" << std::endl;
      ptrInt[0] = -1;
      semCli.op(1);
      fclose (tempFile);
      return 0;
    }

    semCli.op(1);

    FILE* destFile = fopen(fname, "wb");
    if(destFile == NULL) {
      std::cerr << "putfile() fopen() error. errno = " << errno << std::endl;
      return -1;
    }

    semServ.waitNotZero();

    int size, sizeFull = 0;
    while(true) {
      size = ptrInt[0];
      sizeFull+=size;
      std::cout << " ==== << " << sizeFull / 1024 <<  " KB downloaded\r";
      fwrite(ptr + sizeof(int), 1,  size, destFile);
      if(size < SHM_SIZE - sizeof(int)) {
        break;
      }
      semCli.op(1);
      semServ.waitNotZero();
    }
    std::cout << std::endl;
    fclose(destFile);
  } catch(ShMemory::Exception) {
    return -1;
  } catch(Semaphore::Exception) {
    return -1;
  }
  return 0;
}

int getfile(ShMemory shm, Semaphore semCli, Semaphore semServ)
{
  std::cout << " === request 'GETFILE' ";
  try {
    char* ptr = (char*)shm.getAddr();
    int* ptrInt = (int*)shm.getAddr();
    char fname[255];
    strcpy(fname, ptr + 1);
    std::cout << fname << std::endl;

    FILE* sourceFile = fopen(fname, "rb");
    if (sourceFile == NULL) {
      std::cout << " === " << fname <<" doesn't exists" << std::endl;
      ptrInt[0] = -1;
      semCli.op(1);
      return 1;
    }

    int size, sizeFull = 0;
    while(true) {
      size = fread(ptr + sizeof(int), 1, 8192 - sizeof(int), sourceFile);
      sizeFull += size;
      std::cout << " === >> " << sizeFull /1024 <<" KB unloaded\r";
      if(size < 0) {
        std::cerr << "getfile() fread() error. errno = " << errno << std::endl;
        return -1;
      } else if (size < SHM_SIZE - sizeof(int)) {
        ptrInt[0] = size;
        semCli.op(1);
        break;
      } else {
        ptrInt[0] = size;
        semCli.op(1);
        semServ.waitNotZero();
      }
    }
    std::cout << std::endl;
    fclose(sourceFile);
  } catch(ShMemory::Exception) {
    return -1;
  } catch (Semaphore::Exception) {
    return -1;
  }
  return 0;
}

int rmfile(ShMemory shm, Semaphore semCli)
{
  std::cout << " === request 'RMFILE' ";
  char* ptr = (char*)shm.getAddr();
  char fname[255];
  strcpy(fname, ptr + 1);
  std::cout << fname <<std::endl;

  FILE* file = fopen(fname, "rb");
  if (file == NULL) {
    if(errno == EINVAL) {
      std::cout << " === " << fname <<" can not be open" << std::endl;
      ptr[0] = 2;
      semCli.op(1);
      return 1;
    } else {
      std::cout << "file doesn't exists" << std::endl;
      ptr[0] = 1;
      semCli.op(1);
      return 1;
    }
  } else {
    fclose(file);
    if(remove(fname) == 0) {
      ptr[0] = 0;
      std::cout << " === " << fname << " success removed"  << std::endl;
    } else {
      ptr[0] = get_error_code();
      std::cout << " === " << fname << " can not be removed. ";
      printf("error code = %x\n", ptr[0]);
    }
    semCli.op(1);
    return 0;
  }
}
