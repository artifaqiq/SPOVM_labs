#include "libraries.h"
#include "Create_thread.h"

#ifdef _WIN32
Create_thread::Create_thread(int & thread_number)
{
  this_thread_number = thread_number;
  InitializeCriticalSection(&mutex_fot_print);
  InitializeCriticalSection(&mutex_for_close);
  if (thread_number == 1) {
    InitializeCriticalSection(&critical_dection_for_threads);
  }
  thread_handle = CreateThread(NULL, 100, thread_print, (void*)this, 0, &threadID);
}
#elif __linux__
Create_thread::Create_thread(int &thread_number)
{
  this_thread_number = thread_number;
  pthread_mutex_init(mutex_fot_print, NULL);
  pthread_mutex_lock(mutex_fot_print);
  pthread_mutex_init(mutex_for_close, NULL);
  if (thread_number == 1) {
    pthread_mutex_init(critical_dection_for_threads, NULL);
  }
  pthread_create(thread, NULL, thread_print, (void*)this);
}
#endif

bool Create_thread::can_print()
{
#ifdef __linux__
  if (pthread_mutex_trylock(critical_dection_for_threads) != 0) return false;
  pthread_mutex_unlock(critical_dection_for_threads);
  return true;
#elif _WIN32
  if (TryEnterCriticalSection(&critical_dection_for_threads) == 0) return false;
  LeaveCriticalSection(&critical_dection_for_threads);
  return true;
#endif
}

void Create_thread::print_end()
{
#ifdef __linux__
  pthread_mutex_lock(mutex_fot_print);
  pthread_mutex_unlock(critical_dection_for_threads);
#elif _WIN32
  LeaveCriticalSection(&mutex_fot_print);
  LeaveCriticalSection(&critical_dection_for_threads);
#endif
}

bool Create_thread::cloused()
{
#ifdef __linux__
  if (pthread_mutex_trylock(mutex_for_close) != 0) return true;
  pthread_mutex_unlock(mutex_for_close);
  return false;
#elif _WIN32
  if (TryEnterCriticalSection(&mutex_for_close) == 0) return true;
  LeaveCriticalSection(&mutex_for_close);
  return false;
#endif
}

bool Create_thread::pritn_end()
{
#ifdef __linux__
  if (pthread_mutex_trylock(mutex_fot_print) != 0) {
    return true;
  }
  pthread_mutex_unlock(mutex_fot_print);
  return false;
#elif _WIN32
  if (TryEnterCriticalSection(&mutex_fot_print) == 0) return false;
  LeaveCriticalSection(&mutex_fot_print);
  return true;
#endif
}

void Create_thread::start_print()
{
#ifdef __linux__
  pthread_mutex_lock(critical_dection_for_threads);
  pthread_mutex_unlock(mutex_fot_print);
#elif _WIN32
  EnterCriticalSection(&mutex_fot_print);
#endif
}

void Create_thread::end_of_print()
{
#ifdef __linux__
  pthread_mutex_lock(mutex_for_close);
#elif _WIN32
  EnterCriticalSection(&mutex_for_close);
#endif
}

#ifdef __linux__
char _getch()
{
  char buf = 0;
  struct termios old = { 0 };
  fflush(stdout);
  if (tcgetattr(0, &old)<0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old)<0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1)<0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old)<0)
    perror("tcsetattr ~ICANON");

  return buf;
}

int _kbhit(void)
{
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET(STDIN_FILENO, &rdfs);

  select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}
#endif

int getch_noblock()
{
  if (_kbhit()) {
    return _getch();
  } else {
    return -1;
  }
}

int main(int argc, char *argv[])
{
  int number_of_thread = 0;
  int current_thread = 1;
  Create_thread *mass_of_threads[MAX_NUMBER_OF_THREADS];
  int thread_numbers[MAX_NUMBER_OF_THREADS];
  string param;
  param.reserve(MAX_NUMBER_OF_THREADS);

  while (1) {
    char symbol = 0;
    while (symbol != 'q') {
      symbol = getch_noblock();
      switch (symbol) {
      case '+': {
        if (number_of_thread < MAX_NUMBER_OF_THREADS) {
          number_of_thread++;
          thread_numbers[number_of_thread] = number_of_thread;
          Create_thread *thread = new Create_thread(thread_numbers[number_of_thread]);
          mass_of_threads[number_of_thread - 1] = thread;
        };
        break;
      }
      case '-': {
        if (number_of_thread > 0) {
          mass_of_threads[number_of_thread - 1]->end_of_print();
          number_of_thread--;
        };
        break;
      }
      default: {
        if (mass_of_threads[current_thread]->can_print() && number_of_thread > 0) {
          if (current_thread > number_of_thread) {
            current_thread = 1;
          }
          mass_of_threads[current_thread - 1]->start_print();
          current_thread++;
        }
        break;
      }
      }
      usleep(SLEEP_TIME);
    }

    for (int i = 0; i < number_of_thread; i++) {
      mass_of_threads[i]->end_of_print();
    }
    exit(0);
  }
  return 0;
}

#ifdef __linux__
void *thread_print(void * name)
{
#elif _WIN32
DWORD WINAPI thread_print(void * name) {
#endif
  Create_thread *thread = (Create_thread*)name;
  while (true) {
    if (!(thread->pritn_end())) {
#ifdef _WIN32
      EnterCriticalSection(&critical_dection_for_threads);
#endif
      for (int i = 0; threads_names[(thread->this_thread_number) - 1][i] != 0; i++) {
        printf("%c", threads_names[(thread->this_thread_number) - 1][i]);
        fflush(stdout);
        usleep(SLEEP_TIME_PRINT);
      }
      for (int i = 0; threads_names[MAX_NUMBER_OF_THREADS][i] != 0; i++) {
        printf("%c", threads_names[MAX_NUMBER_OF_THREADS][i]);
        fflush(stdout);
        usleep(SLEEP_TIME_PRINT);
      }
      fflush(stdout);
      thread->print_end();
    } else {
      usleep(SLEEP_TIME);
    }
    if (thread->cloused()) {
      break;
    }
  }
  return 0;
}