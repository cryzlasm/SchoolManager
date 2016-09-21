#include "stdafx.h"
#include <Winsock2.h>
#include <malloc.h>
#pragma comment(lib,"Ws2_32.lib")

void InitSock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    
    wVersionRequested = MAKEWORD( 2, 2 );
    
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return;
    }
    
    /* Confirm that the WinSock DLL supports 2.2.        */
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */
    
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup( );
        return; 
    }
}

//套接字对象
typedef struct _SOCKET_OBJ
{
    SOCKET s;  //套接字句柄
    HANDLE hEvent;  //与套接字相关联的事件对象句柄
    sockaddr_in addrRemote;  //客户端地址信息
    _SOCKET_OBJ *pNext;  //指向一个SOCKET_OBJ
}SOCKET_OBJ,*pSOCKET_OBJ;

//线程对象
typedef struct _THREAD_OBJ
{
    HANDLE hEvents[WSA_MAXIMUM_WAIT_EVENTS];  //记录当前线程要等待的事件对象的句柄
    int nSocketCount;       //记录当前线程处理的套接字数量
    pSOCKET_OBJ pSockHeader;   //当前线程处理的套接字对象列表的头指针
    pSOCKET_OBJ pSockTail;   //当前线程处理的套接字对象列表的尾指针
    CRITICAL_SECTION cs;   //关键代码段变量,为的是同步对本结构的访问
    _THREAD_OBJ* pNext;    //指向下一个THREAD_OBJ
}THREAD_OBJ,*pTHREAD_OBJ;

//申请一个套接字对象
pSOCKET_OBJ GetSocketObj(SOCKET s)
{
    pSOCKET_OBJ pSocket=(pSOCKET_OBJ)malloc(sizeof(SOCKET_OBJ));
    if(pSocket!=NULL)
    {
        pSocket->hEvent=WSACreateEvent();
        pSocket->s=s;
        pSocket->pNext=NULL;
    }
    return pSocket;
}

//释放一个套接字对象
void FreeSocketObj(pSOCKET_OBJ pSocket)
{
    ::CloseHandle(pSocket->hEvent);
    if(pSocket->s!=INVALID_SOCKET)
    {
        ::closesocket(pSocket->s);
    }
    free(pSocket);
    pSocket=NULL;
}

pTHREAD_OBJ g_pThreadList=NULL;//指向线程对象列表的表头
CRITICAL_SECTION g_cs;//同步对全局变量的访问

//申请一个线程对象,初始化它的成员,并将它添加到线程对象列表中
pTHREAD_OBJ GetThreadObj()
{
    pTHREAD_OBJ pThread=(pTHREAD_OBJ)malloc(sizeof(THREAD_OBJ));
    if(pThread!=NULL)
    {
        pThread->pNext=NULL;
        pThread->nSocketCount=0;
        pThread->pSockHeader=pThread->pSockTail=NULL;
        ::InitializeCriticalSection(&pThread->cs);
        //创建一个事件对象,用于指示该线程的句柄数组需要重组
        pThread->hEvents[0]=WSACreateEvent();
        //将线程对象加入线程对象列表中
        ::EnterCriticalSection(&g_cs);
        pThread->pNext=g_pThreadList;
        g_pThreadList=pThread;
        ::LeaveCriticalSection(&g_cs);
    }
    return pThread;
}

//释放一个线程对象
void FreeThreadObj(pTHREAD_OBJ pThread)
{
    ::EnterCriticalSection(&g_cs);
    pTHREAD_OBJ p=g_pThreadList;
    if(p==pThread)
    {
        g_pThreadList=pThread->pNext;
    }
    else
    {
        while(p!=NULL&&p->pNext!=pThread)   //找到pThread的前一个节点
        {
            p=p->pNext;
        }
        if(p!=NULL)
        {
            p->pNext=pThread->pNext;
        }
    }
    ::LeaveCriticalSection(&g_cs);
    //释放资源
    ::CloseHandle(pThread->hEvents[0]);
    ::DeleteCriticalSection(&pThread->cs);
    ::free(pThread);
}

//重新建立线程对象的events数组  即将pSocket中的event与pThread中的events关联起来
void RebuildArray(pTHREAD_OBJ pThread)
{
    ::EnterCriticalSection(&pThread->cs);
    pSOCKET_OBJ pSocket=pThread->pSockHeader;
    int n=1; //从下标为1开始
    while(pSocket!=NULL)
    {
        pThread->hEvents[n++]=pSocket->hEvent;
        pSocket=pSocket->pNext;
    }
    ::LeaveCriticalSection(&pThread->cs);
}

LONG g_nTotalConnections;//总共连接数量
LONG g_nCurrentConnections;//当前连接数量

//向线程的套接字列表中插入一个套接字对象
BOOL InsertSocketObj(pTHREAD_OBJ pThread,pSOCKET_OBJ pSocket)
{
    BOOL bRet=false;
    ::EnterCriticalSection(&pThread->cs);
    if(pThread->nSocketCount<4-1)
    {
        if(pThread->pSockHeader==NULL)
        {
            pThread->pSockHeader=pThread->pSockTail=pSocket;
        }
        else
        {
            pThread->pSockTail->pNext=pSocket;
            pThread->pSockTail=pSocket;
        }
        pThread->nSocketCount++;
        bRet=true;
    }
    ::LeaveCriticalSection(&pThread->cs);
    if(bRet)
    {
        InterlockedIncrement(&g_nTotalConnections);
        InterlockedIncrement(&g_nCurrentConnections);
    }
    return bRet;
}



//从给定线程的套接字对象列表中移除一个套接字对象
void RemoveSocketObj(pTHREAD_OBJ pThread,pSOCKET_OBJ pSocket)
{
    ::EnterCriticalSection(&pThread->cs);
    //在套接字对象列表中查找指定的套接字对象,找到后将它移除
    pSOCKET_OBJ pTemp=pThread->pSockHeader;
    if(pTemp==pSocket)
    {
        if(pThread->pSockHeader==pThread->pSockTail)
        {
            pThread->pSockHeader=pThread->pSockTail=pTemp->pNext;
        }
        else
        {
            pThread->pSockHeader=pTemp->pNext;
        }
    }
    else
    {
        while(pTemp!=NULL&&pTemp->pNext!=pSocket)
        {
            pTemp=pTemp->pNext;
        }
        if(pTemp!=NULL)
        {
            if(pThread->pSockTail==pSocket)
            {
                pThread->pSockTail=pTemp;
            }
            pTemp->pNext=pSocket->pNext;
        }
    }
    pThread->nSocketCount--;
    ::LeaveCriticalSection(&pThread->cs);
    ::WSASetEvent(pThread->hEvents[0]); //指示线程重建句柄数组
    ::InterlockedDecrement(&g_nCurrentConnections);
}

pSOCKET_OBJ FindSocketObj(pTHREAD_OBJ pThread,int nIndex)
{
    pSOCKET_OBJ pSocket=pThread->pSockHeader;
    while(--nIndex)
    {
        if(pSocket==NULL)
        {
            return NULL;
        }
        pSocket=pSocket->pNext;
    }
    return pSocket;
}

BOOL HandleIO(pTHREAD_OBJ pThread,pSOCKET_OBJ pSocket)
{
    ::WSANETWORKEVENTS wsaEvent;
    ::WSAEnumNetworkEvents(pSocket->s,pSocket->hEvent,&wsaEvent);
    if(wsaEvent.lNetworkEvents&FD_READ)
    {
        if(wsaEvent.iErrorCode[FD_READ_BIT]==0)
        {
            char *buf=new char[1024];
            __try
            {
                
                int nRec=recv(pSocket->s,buf,1024,0);
                if(nRec>0)
                {
                    buf[nRec]='/0';
                    printf("收到数据:%s/n",buf);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            __finally
            {
                delete[]buf;
            }
        }
    }
    else if(wsaEvent.lNetworkEvents&FD_CLOSE)
    {
        printf("一连接继开/n");
        RemoveSocketObj(pThread,pSocket);
        FreeSocketObj(pSocket);
        return false;
    }
    return true;
}

//处理I/O的线程
DWORD WINAPI ServerThread(LPVOID lpParam)
{
    pTHREAD_OBJ pThread=(pTHREAD_OBJ)lpParam;
    printf("新线程%d起动/n",GetCurrentThreadId());
    while(true)
    {
        //等待网络事件
        int nIndex=WSAWaitForMultipleEvents(pThread->nSocketCount+1,pThread->hEvents,false,WSA_INFINITE,false);
        nIndex=nIndex-WSA_WAIT_EVENT_0;
        //查看受信事件
        for(int i=nIndex;i<pThread->nSocketCount+1;i++)
        {
            nIndex=WSAWaitForMultipleEvents(1,&pThread->hEvents[i],true,0,false);
            if(WSA_WAIT_FAILED==nIndex||WSA_WAIT_TIMEOUT==nIndex)
            {
                continue;
            }
            else
            {
                if(0==i)
                {
                    RebuildArray(pThread);
                    if(pThread->nSocketCount==0)
                    {
                        FreeThreadObj(pThread);
                        printf("线程%d退出/n",GetCurrentThreadId());
                        return 0;
                    }
                    ::WSAResetEvent(pThread->hEvents[0]);
                }
                else
                {
                    pSOCKET_OBJ pSocket=FindSocketObj(pThread,i);
                    if(pSocket!=NULL)
                    {
                        if(!HandleIO(pThread,pSocket))
                        {
                            RebuildArray(pThread);
                        }
                    }
                    else
                    {
                        printf("unable to find socket object/n");
                    }
                }
            }
        }
    }
}

//将一个套接字对象安排给空闲的线程处理
void AssignToFreeThread(pSOCKET_OBJ pSocket)
{
    pSocket->pNext=NULL;
    ::EnterCriticalSection(&g_cs);
    pTHREAD_OBJ pThread=g_pThreadList;
    //试图插入到现存的线程
    while(pThread!=NULL)
    {
        if(InsertSocketObj(pThread,pSocket))
        {
            break;
        }
        pThread=pThread->pNext;
    }
    //如果没有空闲线程,为这个套接字对象创建新的线程
    if(pThread==NULL)
    {
        pThread=GetThreadObj();
        InsertSocketObj(pThread,pSocket);
        
        ::CreateThread(NULL,0,ServerThread,pThread,0,NULL);
    }
    ::LeaveCriticalSection(&g_cs);
    //指示线程重建句柄数组
    ::WSASetEvent(pThread->hEvents[0]);
}

int _tmain(int argc, _TCHAR* argv[])
{
    InitSock();
    SOCKET sListen=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in sin;
    sin.sin_addr.S_un.S_addr=INADDR_ANY;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(3456);
    if(SOCKET_ERROR==bind(sListen,(sockaddr*)&sin,sizeof(sockaddr)))
    {
        printf("绑定失败/n");
        ::WSACleanup();
        return 0;
    }
    listen(sListen,5);
    ::WSAEVENT wsaEvent=::WSACreateEvent();
    ::WSAEventSelect(sListen,wsaEvent,FD_ACCEPT);
    ::InitializeCriticalSection(&g_cs);
    while(true)
    {
        int nRet=::WaitForSingleObject(wsaEvent,10000);
        if(nRet==WAIT_FAILED)
        {
            printf("failed wait for single object/n");
            break;
        }
        else if(nRet==WSA_WAIT_TIMEOUT)
        {
            printf(" TotalConnections:%d/n",g_nTotalConnections);
            printf(" TotalCurrentConnections:%d/n",g_nCurrentConnections);
        }
        else
        {
            ::ResetEvent(wsaEvent);
            while(true)
            {
                sockaddr_in si;
                int nLen=sizeof(si);
                SOCKET client=accept(sListen,(sockaddr*)&si,&nLen);
                if(client==SOCKET_ERROR)
                    break;
                pSOCKET_OBJ pSocket=GetSocketObj(client);
                pSocket->addrRemote=si;
                ::WSAEventSelect(pSocket->s,pSocket->hEvent,FD_READ|FD_CLOSE);
                AssignToFreeThread(pSocket);
                printf("一新连接/n");
            }
        }
    }
    ::DeleteCriticalSection(&g_cs);
    return 0;
} 