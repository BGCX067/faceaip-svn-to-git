
//Thread.c

#include <app/common/c_thread.h>
 
/***********************************************************************
**  Function : This function is used to create a new thread. The new thread's
               function has the form as follows: 
                    int (* pThreadFun) (void *)  
              (one argument which is void * and return an integer type data) 
**      Input: pThreadFun -- the address of function to create a new thread
**                 pParam -- the parament of the new thread
**  return the handle of the new thread
************************************************************************/
inline THREAD_HANDLE CREATE_THREAD(int (* pThreadFun)( void * ),  void * pParam)
{
    pthread_t threadID;
    pthread_create(&threadID, NULL, (THREAD_FUN_TYPE)pThreadFun, pParam);
    return threadID;
}

 
/***********************************************************************
**  Function : This function is used to detach a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
inline void DETACH_THREAD(THREAD_HANDLE hThread)
{
	pthread_detach(hThread);
}

/***********************************************************************
**  Function : This function is used to end a thread. This function is 
               usually used at the end of the Thread function. 
************************************************************************/
inline void EXIT_THREAD()
{
    pthread_exit(0);
}

inline void END_THREAD(THREAD_HANDLE hThread)
{
	pthread_cancel(hThread);
}

 

/***********************************************************************
**  Function : This function is used to close the handle of a thread.
               But the POSIX thread use the pthread_join function to
              clean the handle of the thread. So this funciton do
              nothing in Linux platform.
************************************************************************/
inline void CLOSE_THREAD(THREAD_HANDLE hThread)
{
}

/***********************************************************************
**  Function : This function is used to wait for another thread.
               So the main thread used this function will suspend
              until the waiting thread finished.
**     input : hThread -- the handle of waiting thread.
************************************************************************/
inline void WAIT_THREAD(THREAD_HANDLE hThread)
{
    void * retval;
    pthread_join(hThread, &retval);
}
