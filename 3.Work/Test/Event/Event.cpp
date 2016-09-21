// Event.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
    
#include <AFXMT.H>
#include <WINDOWS.H>
#include <Winbase.h>

#include "../../AddIn/MyEvent.h"


#define THREAD_COUNT 2
#define READ_BUF_SIZE 1024 * 8

typedef struct _TAGTHREADPARAM
{
    int nCount;
    CMyEvent*  pEvent;
}THREADPARAM,*PTHREADPARAM;

DWORD ThreadCopyFile(LPVOID lpParam)
{
    if (lpParam == NULL)
    {
        return 0;
    }

    PTHREADPARAM pstParam = (PTHREADPARAM)lpParam;
    
    printf("Ïß³ÌID: %d", GetCurrentThreadId());
    printf("Ëø×´Ì¬£º%d\r\n", pstParam->pEvent->IsLock());
    pstParam->pEvent->Lock();
    
    for(int i = 0; i < 1000000; i++)
    {
        pstParam->nCount++;
        
        //Sleep(0);
    }
    
    printf("Ïß³ÌID: %d", GetCurrentThreadId());
    printf("Ëø×´Ì¬£º%d\r\n", pstParam->pEvent->IsLock());
    pstParam->pEvent->UnLock();

    return 0;
}


int main(int argc, char* argv[])
{
    CMyEvent Event(TRUE);

    printf("Ëø×´Ì¬£º%d\r\n\r\n", Event.IsLock());
    THREADPARAM stThreadParam = {0};
    stThreadParam.pEvent = &Event;
    
    HANDLE hThread[THREAD_COUNT];
    
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        hThread[i] =  CreateThread(NULL,
                                  0, 
                                  (LPTHREAD_START_ROUTINE)ThreadCopyFile,
                                  &stThreadParam,
                                  0,
                                  NULL
                                  );
    }

    printf("Ëø×´Ì¬£º%d\r\n", Event.IsLock());

    WaitForMultipleObjects(THREAD_COUNT, hThread, TRUE, INFINITE);

    printf("nCount = %d\r\n", stThreadParam.nCount);

    system("pause");
    printf("Ëø×´Ì¬£º%d\r\n", Event.IsLock());

    return 0;
}
