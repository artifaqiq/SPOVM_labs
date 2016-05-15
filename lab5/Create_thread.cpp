#include "libraries.h"
#include "Create_thread.h"

#ifdef _WIN32
Create_thread::Create_thread(int & thread_number) {
	this_thread_number = thread_number;
	InitializeCriticalSection(&mutex_fot_print);
	InitializeCriticalSection(&mutex_for_close);
	if (thread_number == 1) {
		InitializeCriticalSection(&critical_dection_for_threads);
	}
	thread_handle = CreateThread(NULL, 100, thread_print, (void*)this, 0, &threadID);
}
#elif __linux__
Create_thread::Create_thread(int &thread_number) {
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

bool Create_thread::can_print() {
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

void Create_thread::print_end() {
#ifdef __linux__
	pthread_mutex_lock(mutex_fot_print);
	pthread_mutex_unlock(critical_dection_for_threads);
#elif _WIN32
	LeaveCriticalSection(&mutex_fot_print);
	LeaveCriticalSection(&critical_dection_for_threads);
#endif
}

bool Create_thread::cloused() {
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

bool Create_thread::pritn_end() {
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

void Create_thread::start_print() {
#ifdef __linux__
	pthread_mutex_lock(critical_dection_for_threads);
	pthread_mutex_unlock(mutex_fot_print);
#elif _WIN32
	EnterCriticalSection(&mutex_fot_print);
#endif
}

void Create_thread::end_of_print() {
#ifdef __linux__
	pthread_mutex_lock(mutex_for_close);
#elif _WIN32
	EnterCriticalSection(&mutex_for_close);
#endif
}