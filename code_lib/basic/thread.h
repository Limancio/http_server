#ifndef THREAD_H

#if _WIN32
#include <windows.h>

typedef HANDLE thread_handle;
typedef HANDLE mutex_handle;

#define THREAD_FUNCTION DWORD WINAPI
#define create_thread(_attributes, _stack_size, _start_function, _args, _flags_, thread_id) CreateThread(_attributes, _stack_size, _start_function, _args, _flags_, thread_id)

#define interlocked_compare_exchange(_pointer, _new_value, _old_value) InterlockedCompareExchange(_pointer, _new_value, _old_value)
#define thread_mutex_lock(_mutex_handle, _time_interval, _param) WaitForSingleObjectEx(_mutex_handle, _time_interval, _param)
#define thread_mutex_unlock(_mutex_handle, _release_count, _prev_count) ReleaseSemaphore(_mutex_handle, _release_count, _prev_count);
#define thread_mutex_init(_attributes, _initial_count, _max_count, _name, _flags, _access) CreateSemaphoreEx(_attributes, _initial_count, _max_count, _name, _flags, _access)

typedef LARGE_INTEGER ticks;

internal inline
void get_ticks(ticks *value) {
    QueryPerformanceFrequency(value);
}

#else
#include <pthread.h>

typedef pthread_t thread_handle;
struct mutex_handle {
    pthread_cond_t condition;
    pthread_mutex_t mutex;
};


#define THREAD_FUNCTION void* 
#define create_thread(_thread, _attributes, _start_function, _args) pthread_create(_thread, _attributes, _start_function, _args)

#define interlocked_compare_exchange(_pointer, _add_value) __sync_fetch_and_add(_pointer, _add_value)
#define thread_mutex_lock(_mutex_handle) pthread_mutex_lock(_mutex_handle)
#define thread_mutex_unlock(_mutex_handle) pthread_mutex_unlock(_mutex_handle)
#define thread_mutex_init(_mutex_condition, _mutex)    \
    pthread_cond_init(_mutex_condition, NULL);         \
    pthread_mutex_init(_mutex, NULL); 

typedef unsigned long long ticks;

internal inline
void get_ticks(ticks *value) {
    u32 a, d;
    asm("cpuid");
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    
    *value = ((ticks)a) | (((ticks)d) << 32);
}

#endif

#define THREAD_H
#endif
