
#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef WIN32  //define the macro for Win32 thread

#define _WIN32_WINNT 0x0400 
#include <windows.h>
#include <process.h>

#include <WINSOCK2.H>
#include <ws2tcpip.h>


/*************  mutex (use CRITICAL_SECTION in windows) ***************/
#define       THREAD_MUTEX             CRITICAL_SECTION
#define       INITIALIZE_MUTEX(mutex)  InitializeCriticalSection(mutex)
#define       DESTROY_MUTEX            DeleteCriticalSection
#define       LOCK_MUTEX               EnterCriticalSection
#define       UNLOCK_MUTEX             LeaveCriticalSection
#define       TRYLOCK_MUTEX            TryEnterCriticalSection
#define       THREAD_HANDLE            unsigned long

// added by keyy 2011.6.27
#define		  TIMER_COND			   HANDLE
#define		  COND_INIT()			   CreateEvent(NULL, false, false, NULL)
#define		  COND_RESET			   ResetEvent
#define		  COND_DESTROY			   CloseHandle
#define		  SET_SIGNAL			   SetEvent
/**********************************************************************/

typedef unsigned int (__stdcall * THREAD_FUN_TYPE)(void *);

#else  //define the macro for POSIX thread

#include <pthread.h>
#include <stdio.h>

/*************  mutex (use CRITICAL_SECTION in other platforms) *******/
#define       THREAD_MUTEX             pthread_mutex_t
#define       INITIALIZE_MUTEX(mutex)  pthread_mutex_init(mutex, NULL)  
#define       INIT_MUTEX(mutex)  	pthread_mutex_init(mutex, NULL)  
#define       DESTROY_MUTEX            pthread_mutex_destroy
#define		  LOCK_MUTEX               pthread_mutex_lock
#define       UNLOCK_MUTEX             pthread_mutex_unlock
#define       TRYLOCK_MUTEX            pthread_mutex_trylock
#define       THREAD_HANDLE            pthread_t

// added by keyy 2011.6.27
#define		  TIMER_COND			   pthread_cond_t
#define		  COND_INIT(cond)		   pthread_cond_init(cond, NULL)
#define		  COND_DESTROY(single)	   pthread_cond_destroy(&single)
#define		  SET_SIGNAL(single)	   pthread_cond_signal(&single)

//"c"
#define		  THREAD_COND			   pthread_cond_t
#define		  INIT_COND(cond)		   	pthread_cond_init(cond, NULL)
#define		  INIT_SIGNAL			   	pthread_cond_signal
#define		  SIGNAL_COND			   	pthread_cond_signal
#define		  DESTROY_COND	   		   	pthread_cond_destroy
#define 	  COND_WAIT				   	pthread_cond_wait
#define 	  COND_TIME_WAIT			pthread_cond_timedwait

/**********************************************************************/

typedef void *( * THREAD_FUN_TYPE)(void *);

#endif  //WIN32




/***********************************************************************
 **  Function : This function is used to create a new thread. The new thread's
 function has the form as follows: 
 int (* pThreadFun) (void *)  
 (one argument which is void * and return an integer type data)
 **      Input: pThreadFun -- the address of function to create a new thread
 **                 pParam -- the parament of the new thread
 **  return the handle of the new thread
 ************************************************************************/
THREAD_HANDLE CREATE_THREAD(int (* pThreadFun)( void * ),  void * pParam);


/***********************************************************************
 **  Function : This function is used to detach a thread.
 **     input : hThread -- the handle of the thread.
 ************************************************************************/
void DETACH_THREAD(THREAD_HANDLE hThread);


/***********************************************************************
 **  Function : This function is used to end a thread. This function is 
 usually used at the end of the Thread function. 
 ************************************************************************/
#ifndef __cplusplus
void EXIT_THREAD();
#else
void END_THREAD();
#endif

void END_THREAD(THREAD_HANDLE hThread);

/***********************************************************************
 **  Function : This function is used to close the handle of a thread.
 **     input : hThread -- the handle of the thread.
 ************************************************************************/
void CLOSE_THREAD(THREAD_HANDLE hThread);



/***********************************************************************
 **  Function : This function is used to wait for another thread.
 So the main thread used this function will suspend
 until the waiting thread finished. 
 **     input : hThread -- the handle of waiting thread.
 ************************************************************************/
int WAIT_THREAD_EXIT(THREAD_HANDLE hThread);





#endif //__THREAD_H__
