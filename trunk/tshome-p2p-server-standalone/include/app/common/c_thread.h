
#ifndef __THREAD_H__
#define __THREAD_H__

#include <stdio.h>
#include <pthread.h>

/*************  mutex (use CRITICAL_SECTION in other platforms) *******/
#define       THREAD_MUTEX             pthread_mutex_t
#define       INITIALIZE_MUTEX(mutex)  pthread_mutex_init(mutex, NULL)    
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
/**********************************************************************/

typedef void *( * THREAD_FUN_TYPE)(void *);

#endif

 

 

/***********************************************************************
**  Function : This function is used to create a new thread. The new thread's
               function has the form as follows: 
                    int (* pThreadFun) (void *)  
              (one argument which is void * and return an integer type data)
**      Input: pThreadFun -- the address of function to create a new thread
**                 pParam -- the parament of the new thread
**  return the handle of the new thread
************************************************************************/
inline extern THREAD_HANDLE CREATE_THREAD(int (* pThreadFun)( void * ),  void * pParam);

 
/***********************************************************************
**  Function : This function is used to detach a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
inline extern void DETACH_THREAD(THREAD_HANDLE hThread);


/***********************************************************************
**  Function : This function is used to end a thread. This function is 
               usually used at the end of the Thread function. 
************************************************************************/
inline extern void EXIT_THREAD();
inline extern void END_THREAD(THREAD_HANDLE hThread);
 

/***********************************************************************
**  Function : This function is used to close the handle of a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
inline extern void CLOSE_THREAD(THREAD_HANDLE hThread);

 

/***********************************************************************
**  Function : This function is used to wait for another thread.
               So the main thread used this function will suspend
              until the waiting thread finished. 
**     input : hThread -- the handle of waiting thread.
************************************************************************/
inline extern void WAIT_THREAD(THREAD_HANDLE hThread);

