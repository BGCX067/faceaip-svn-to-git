/*
 * thread.cpp
 *
 *  Created on: Apr 17, 2012
 *      Author: kyy
 */

#include "common/thread.h"

#ifdef WIN32  //define the macro for Win32 thread


/***********************************************************************
**  Function : This function is used to create a new thread. The new thread's
               function has the form as follows:
                    int (* pThreadFun) (void *)
              (one argument which is void * and return an integer type data)
**      Input: pThreadFun -- the address of function to create a new thread
**                 pParam -- the parament of the new thread
**  return the handle of the new thread
************************************************************************/

THREAD_HANDLE CREATE_THREAD(int (* pThreadFun)( void * ),  void * pParam)
{
    return _beginthreadex(NULL, 0, (THREAD_FUN_TYPE) pThreadFun, pParam, NULL, NULL);
}


/***********************************************************************
**  Function : This function is used to detach a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
void DETACH_THREAD(THREAD_HANDLE hThread)
{
	CloseHandle((HANDLE*) hThread);
}

/***********************************************************************
**  Function : This function is used to end a thread. This function is
               usually used at the end of the Thread function.
************************************************************************/
void END_THREAD()
{
}

void END_THREAD(THREAD_HANDLE hThread)
{
}


/***********************************************************************
**  Function : This function is used to close the handle of a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
void CLOSE_THREAD(THREAD_HANDLE hThread)
{
    CloseHandle((HANDLE *) hThread);
}



/***********************************************************************
**  Function : This function is used to wait for another thread.
               So the main thread used this function will suspend
              until the waiting thread finished.
**     input : hThread -- the handle of waiting thread.
************************************************************************/
int WAIT_THREAD_EXIT(THREAD_HANDLE hThread)
{
   int nRes = WaitForSingleObject((HANDLE*)hThread, 5 * 1000);
   return nRes;
}



#else  //define the macro for POSIX thread


/***********************************************************************
**  Function : This function is used to create a new thread. The new thread's
               function has the form as follows:
                    int (* pThreadFun) (void *)
              (one argument which is void * and return an integer type data)
**      Input: pThreadFun -- the address of function to create a new thread
**                 pParam -- the parament of the new thread
**  return the handle of the new thread
************************************************************************/
THREAD_HANDLE CREATE_THREAD(int (* pThreadFun)( void * ),  void * pParam)
{
    pthread_t threadID;
    pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
    pthread_create(&threadID, &threadAttr, (THREAD_FUN_TYPE)pThreadFun, pParam);
	pthread_attr_destroy(&threadAttr);
	return threadID;
}


/***********************************************************************
**  Function : This function is used to detach a thread.
**     input : hThread -- the handle of the thread.
************************************************************************/
void DETACH_THREAD(THREAD_HANDLE hThread)
{
	pthread_detach(hThread);
}

/***********************************************************************
**  Function : This function is used to end a thread. This function is
               usually used at the end of the Thread function.
************************************************************************/

void END_THREAD(THREAD_HANDLE hThread)
{
	pthread_cancel(hThread);
}



/***********************************************************************
**  Function : This function is used to close the handle of a thread.
               But the POSIX thread use the pthread_join function to
              clean the handle of the thread. So this funciton do
              nothing in Linux platform.
************************************************************************/
void CLOSE_THREAD(THREAD_HANDLE hThread)
{
}

/***********************************************************************
**  Function : This function is used to wait for another thread.
               So the main thread used this function will suspend
              until the waiting thread finished.
**     input : hThread -- the handle of waiting thread.
************************************************************************/
int WAIT_THREAD_EXIT(THREAD_HANDLE handle)
{
	int nRes = pthread_join(handle, NULL);

	return nRes;
}

#endif  //WIN32
