#ifndef PODRUM_WORKER_H
#define PODRUM_WORKER_H

#include <podrum/debug.h>

#ifdef _WIN32

#include <windows.h>

#else

#include <pthread.h>

#endif

#ifdef _WIN32

typedef HANDLE worker_t;
#define RETURN_WORKER_EXECUTOR DWORD WINAPI
#define ARGS_WORKER_EXECUTOR LPVOID
typedef DWORD (WINAPI *worker_executor_t)(LPVOID argvp);
typedef HANDLE worker_mutex_t;

#else

typedef pthread_t worker_t;
#define RETURN_WORKER_EXECUTOR void *
#define ARGS_WORKER_EXECUTOR void *
typedef void *(*worker_executor_t)(void *argv);
typedef pthread_mutex_t worker_mutex_t;

#endif

worker_t create_worker(worker_executor_t executor, void *args);

void join_worker(worker_t worker);

void worker_create_mutex(worker_mutex_t *lock);

void worker_destroy_mutex(worker_mutex_t *lock);

void worker_mutex_lock(worker_mutex_t *lock);

void worker_mutex_unlock(worker_mutex_t *lock);

#endif
