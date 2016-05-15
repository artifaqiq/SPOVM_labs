#include "libraries.h"

#ifdef __linux__
void * thread_print(void *);
#elif _WIN32
void usleep(int time)
{
  Sleep(time / SLEEP_TIME);
}
DWORD WINAPI thread_print(void *);
CRITICAL_SECTION critical_dection_for_threads;
#endif

class Create_thread
{
public:
  int this_thread_number;
  int number;

#ifdef __linux__
  pthread_t *thread = new pthread_t();
  pthread_mutex_t *mutex_fot_print = new pthread_mutex_t();
  pthread_mutex_t *mutex_for_close = new pthread_mutex_t();
#endif

#ifdef _WIN32
  HANDLE thread_handle;
  CRITICAL_SECTION mutex_fot_print;
  CRITICAL_SECTION mutex_for_close;
  DWORD threadID;

  Create_thread(int & thread_number);

#elif __linux__
  Create_thread(int &thread_number);
#endif

  bool can_print();
  bool pritn_end();
  bool cloused();
  void start_print();
  void print_end();
  void end_of_print();
};