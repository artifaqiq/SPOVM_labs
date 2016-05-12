#ifndef SERVERFTP_H
#define SERVERFTP_H

#include "shmemory.h"
#include "semaphore.h"

const int SHM_SIZE = 0x2000;
const int SHM_KEY_KEY = 6;
const int SEM_SESS_KEY = 7;

void server_thread_func(void* data);

int ls(ShMemory shm, Semaphore semCli, Semaphore semServ);
int putfile(ShMemory shm, Semaphore semCli, Semaphore semServ);
int getfile(ShMemory shm, Semaphore semCli, Semaphore semServ);
int rmfile(ShMemory shm, Semaphore semCli);

#endif // SERVERFTP_H
