#pragma once

#include <iostream>
#include <string>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

#ifdef __linux__
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <linux/sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
struct termios orig_termios;
pthread_mutex_t * critical_dection_for_threads = new pthread_mutex_t();
#endif

#define NB_ENABLE 1
#define NB_DISABLE 0
#define MAX_NUMBER_OF_THREADS 20
#define SLEEP_TIME 1000
#define SLEEP_TIME_PRINT 50000

static const char *threads_names[] = { "1.First", "2.Second",
                                       "3.Third", "4.Fourth", "5.Fifth", "6.Sixth", "7.Seventh", "8.Eighth",
                                       "9.Ninth", "10.Tenth", "11.Eleventh", "12.Twelfth", "13.Thirteen",
                                       "14.Fourteenth", "15.Fifteenth", "16.Sixteenth", "17.Seventeenth",
                                       "18.Eighteenth", "19.Nineteenth", "20.Twentieth", " thread\n",
                                     };