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

//�׽��ֶ���
typedef struct _SOCKET_OBJ
{
    SOCKET s;  //�׽��־��
    HANDLE hEvent;  //���׽�����������¼�������
    sockaddr_in addrRemote;  //�ͻ��˵�ַ��Ϣ
    _SOCKET_OBJ *pNext;  //ָ��һ��SOCKET_OBJ
}SOCKET_OBJ,*pSOCKET_OBJ;

//�̶߳���
typedef struct _THREAD_OBJ
{
    HANDLE hEvents[WSA_MAXIMUM_WAIT_EVENTS];  //��¼��ǰ�߳�Ҫ�ȴ����¼�����ľ��
    int nSocketCount;       //��¼��ǰ�̴߳�����׽�������
    pSOCKET_OBJ pSockHeader;   //��ǰ�̴߳�����׽��ֶ����б��ͷָ��
    pSOCKET_OBJ pSockTail;   //��ǰ�̴߳�����׽��ֶ����б��βָ��
    CRITICAL_SECTION cs;   //�ؼ�����α���,Ϊ����ͬ���Ա��ṹ�ķ���
    _THREAD_OBJ* pNext;    //ָ����һ��THREAD_OBJ
}THREAD_OBJ,*pTHREAD_OBJ;

//����һ���׽��ֶ���
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

//�ͷ�һ���׽��ֶ���
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

pTHREAD_OBJ g_pThreadList=NULL;//ָ���̶߳����б�ı�ͷ
CRITICAL_SECTION g_cs;//ͬ����ȫ�ֱ����ķ���

//����һ���̶߳���,��ʼ�����ĳ�Ա,��������ӵ��̶߳����б���
pTHREAD_OBJ GetThreadObj()
{
    pTHREAD_OBJ pThread=(pTHREAD_OBJ)malloc(sizeof(THREAD_OBJ));
    if(pThread!=NULL)
    {
        pThread->pNext=NULL;
        pThread->nSocketCount=0;
        pThread->pSockHeader=pThread->pSockTail=NULL;
        ::InitializeCriticalSection(&pThread->cs);
        //����һ���¼�����,����ָʾ���̵߳ľ��������Ҫ����
        pThread->hEvents[0]=WSACreateEvent();
        //���̶߳�������̶߳����б���
        ::EnterCriticalSection(&g_cs);
        pThread->pNext=g_pThreadList;
        g_pThreadList=pThread;
        ::LeaveCriticalSection(&g_cs);
    }
    return pThread;
}

//�ͷ�һ���̶߳���
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
        while(p!=NULL&&p->pNext!=pThread)   //�ҵ�pThread��ǰһ���ڵ�
        {
            p=p->pNext;
        }
        if(p!=NULL)
        {
            p->pNext=pThread->pNext;
        }
    }
    ::LeaveCriticalSection(&g_cs);
    //�ͷ���Դ
    ::CloseHandle(pThread->hEvents[0]);
    ::DeleteCriticalSection(&pThread->cs);
    ::free(pThread);
}

//���½����̶߳����events����  ����pSocket�е�event��pThread�е�events��������
void RebuildArray(pTHREAD_OBJ pThread)
{
    ::EnterCriticalSection(&pThread->cs);
    pSOCKET_OBJ pSocket=pThread->pSockHeader;
    int n=1; //���±�Ϊ1��ʼ
    while(pSocket!=NULL)
    {
        pThread->hEvents[n++]=pSocket->hEvent;
        pSocket=pSocket->pNext;
    }
    ::LeaveCriticalSection(&pThread->cs);
}

LONG g_nTotalConnections;//�ܹ���������
LONG g_nCurrentConnections;//��ǰ��������

//���̵߳��׽����б��в���һ���׽��ֶ���
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



//�Ӹ����̵߳��׽��ֶ����б����Ƴ�һ���׽��ֶ���
void RemoveSocketObj(pTHREAD_OBJ pThread,pSOCKET_OBJ pSocket)
{
    ::EnterCriticalSection(&pThread->cs);
    //���׽��ֶ����б��в���ָ�����׽��ֶ���,�ҵ������Ƴ�
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
    ::WSASetEvent(pThread->hEvents[0]); //ָʾ�߳��ؽ��������
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
                    printf("�յ�����:%s/n",buf);
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
        printf("һ���Ӽ̿�/n");
        RemoveSocketObj(pThread,pSocket);
        FreeSocketObj(pSocket);
        return false;
    }
    return true;
}

//����I/O���߳�
DWORD WINAPI ServerThread(LPVOID lpParam)
{
    pTHREAD_OBJ pThread=(pTHREAD_OBJ)lpParam;
    printf("���߳�%d��/n",GetCurrentThreadId());
    while(true)
    {
        //�ȴ������¼�
        int nIndex=WSAWaitForMultipleEvents(pThread->nSocketCount+1,pThread->hEvents,false,WSA_INFINITE,false);
        nIndex=nIndex-WSA_WAIT_EVENT_0;
        //�鿴�����¼�
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
                        printf("�߳�%d�˳�/n",GetCurrentThreadId());
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

//��һ���׽��ֶ����Ÿ����е��̴߳���
void AssignToFreeThread(pSOCKET_OBJ pSocket)
{
    pSocket->pNext=NULL;
    ::EnterCriticalSection(&g_cs);
    pTHREAD_OBJ pThread=g_pThreadList;
    //��ͼ���뵽�ִ���߳�
    while(pThread!=NULL)
    {
        if(InsertSocketObj(pThread,pSocket))
        {
            break;
        }
        pThread=pThread->pNext;
    }
    //���û�п����߳�,Ϊ����׽��ֶ��󴴽��µ��߳�
    if(pThread==NULL)
    {
        pThread=GetThreadObj();
        InsertSocketObj(pThread,pSocket);
        
        ::CreateThread(NULL,0,ServerThread,pThread,0,NULL);
    }
    ::LeaveCriticalSection(&g_cs);
    //ָʾ�߳��ؽ��������
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
        printf("��ʧ��/n");
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
                printf("һ������/n");
            }
        }
    }
    ::DeleteCriticalSection(&g_cs);
    return 0;
} 