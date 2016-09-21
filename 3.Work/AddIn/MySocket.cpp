// MySocket.cpp: implementation of the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//================================CEventServer================================
CEventServer::CEventServer()
    :m_Sock()
{
    //初始化
    for(int i = 0; i < sizeof(m_hEventAry) / sizeof(m_hEventAry[0]); i++)
    {
        m_hEventAry[i] = INVALID_HANDLE_VALUE;
    }

    for(int j = 0; j < sizeof(m_hSocketAry) / sizeof(m_hSocketAry[0]); j++)
    {
        m_hSocketAry[j] = INVALID_SOCKET;
    }

    m_dwCount = 0;

    m_pInfoRet = NULL;
    
    memset(m_SRInfo, 0, sizeof(m_SRInfo));

    m_DataLock.ResetEvent();    //默认创建就是上锁
    m_ThreadLock.UnLock();

    m_bDataIsLock = FALSE;
    
    m_bIsNoExit = TRUE;

    m_pEventThread = NULL;
}

CEventServer::~CEventServer()
{
    EndServer();
}

BOOL CEventServer::EndServer()
{
    //是否为第一次进入
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //10秒
        #define TEN_SECOND 10000

        //置标志位
        m_bIsNoExit = FALSE;

        //给主WSAEvent一个退出信号
        WSASetEvent(m_hEventAry[0]);

        //如果线程启动则处理
        if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
        {
            //等待线程退出,等30秒
            DWORD dwRet = WaitForSingleObject(m_pEventThread->m_hThread, TEN_SECOND);
            if(dwRet == WAIT_TIMEOUT)
            {
                CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer：等待线程退出超时！"));
                
                //终止线程，
                if(!TerminateThread(m_pEventThread->m_hThread, 1))
                {
                    CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer：终止线程失败，可能权限不够"));
                    
                    //如果线程无法终止,则暂停掉
                    if(m_pEventThread->SuspendThread() != -1)
                    {
                        CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer：暂停线程，防止操作野指针"));
                    }

                }//End if(!TerminateThread(m_pEventThread->m_hThread, 1))
            }//End if(dwRet == WAIT_TIMEOUT)

            
        }//End if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
        

        


        //挨个关闭Socket
        for(int i = m_dwCount; i >= 0; i--)
        {
            m_Sock.CloseSocket(m_hSocketAry[i]);
            
//             //关闭 内核句柄
//             if(m_hEventAry[i] != INVALID_HANDLE_VALUE)
//             {
//                 WSACloseEvent(m_hEventAry[i]);
//                 m_hEventAry[i] = INVALID_HANDLE_VALUE;
//             }
        }

        //关闭线程句柄 AfxBeginThread会自动关闭句柄
//         if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
//         {
//             ::CloseHandle(m_pEventThread->m_hThread);
//             m_pEventThread->m_hThread = INVALID_HANDLE_VALUE;
//         }

        bIsFirstInto = FALSE;

        //解锁，通知外部退出
        m_DataLock.UnLock();
        CMySocket::OutputErrorMsg(TEXT("EndServer：退出Event模型"));
    }//End if(bIsFirstInto)

    return TRUE;
}

BOOL CEventServer::GetExitInfo() CONST
{
    return m_bIsNoExit;
}

//创建Server， nSocketPort(服务器端口)  lpszSocketAddress(服务器地址) nSocketType(UDP || TCP)
BOOL CEventServer::StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType) throw(int)
{
    BOOL bRet = FALSE;
    
    bRet = m_Sock.StartServer(nSocketPort, lpszSocketAddress, nSocketType);

    //保存服务端Socket
    m_hSocketAry[m_dwCount] = m_Sock.GetMainSock();

    //创建一个事件
    //每创建一个套接字，就创建一个事件对象，
    //把它们的句柄分别放入上面的两个表中，
    //并调用 WSAEventSelect 添加它们的关联。
    m_hEventAry[m_dwCount] = WSACreateEvent();

    //保存地址
    m_addrClient[m_dwCount] = m_Sock.GetMainAddr();

    bRet =  WSAEventSelect(m_hSocketAry[m_dwCount],
                           m_hEventAry[m_dwCount],
                           FD_ACCEPT | FD_READ | FD_CLOSE);

    if (SOCKET_ERROR == bRet)
    {
        //释放资源
        WSACloseEvent(m_hEventAry[m_dwCount]);

        OutputDebugString(TEXT("CEventServer：事件模型创建失败"));
        
        throw(RET_ERROR);
    }
    
    m_dwCount++;
    
    //开启一个线程来接收处理socket数据
    m_pEventThread = AfxBeginThread((AFX_THREADPROC)ThreadFunc, this);

    return TRUE;
}

DWORD CEventServer::ThreadFunc(LPVOID lparam)
{
    CEventServer& Server = *(CEventServer*) lparam;

    BOOL bIsNoExit = FALSE;
    //通过等待事件被触发，来判断和处理socket消息
    char szBuf[MAXBYTE] = {0};
    while(TRUE)
    {
        //获取当前事件
        DWORD  dwRet = WSAWaitForMultipleEvents(
                                Server.m_dwCount,    //EventAry的个数
                                Server.m_hEventAry, //EventAry数组指针
                                FALSE,       //是否等待所有事件对象被触发
                                INFINITE,    //等待事件
                                FALSE);
        if (dwRet == WSA_WAIT_FAILED)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFun: Event 等待失败"));
            closesocket(Server.m_hSocketAry[0]);
            WSACleanup();
            return -1;
        }

        //线程退出标志
        if(!(bIsNoExit = Server.GetExitInfo()))
        {
            
            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFunc：线程通知退出，退出成功"));
            AfxEndThread(bIsNoExit);
        }
        
        //成功则获取Event下标
        int nIndex = dwRet - WSA_WAIT_EVENT_0;

        for (int i = nIndex; i < Server.m_dwCount; i++)
        {
            //给下一个事件一个执行的机会
            dwRet = WSAWaitForMultipleEvents(
                            1,    //EventAry的个数
                            &Server.m_hEventAry[i], //EventAry数组指针
                            FALSE,       //是否等待所有事件对象被触发
                            0,    //等待事件
                            FALSE);
            //如果下一个不为空，则给机会
            if (dwRet == WSA_WAIT_FAILED || dwRet == WSA_WAIT_TIMEOUT)
            {
                continue;
            }

            WSANETWORKEVENTS wsaNetworkEvents;

            //枚举网络事件，获取事件码
            int nRet = WSAEnumNetworkEvents(Server.m_hSocketAry[i],
                                            Server.m_hEventAry[i],
                                            &wsaNetworkEvents);
            //表示成功
            if (nRet == 0)
            {
                //如果事件为FD_连接
                if ( wsaNetworkEvents.lNetworkEvents & FD_ACCEPT)
                {
                    //且ErrorCode为0
                    if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    {
                        //处理监听
                        Server.EventAcceptOfThread(i);
                    }//End if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    else
                    {
                        //根据错误码处理错误
                    }
                }//End if ( wsaNetworkEvents.lNetworkEvents &FD_ACCEPT)

                //如果网络事件为FD_读
                else if ( wsaNetworkEvents.lNetworkEvents & FD_READ)
                {
                    //且ErrorCode为0
                    if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
                    {
                        if(!Server.EventReadOfThread(i))
                        {
                            continue;
                        }

                    }//End if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
                    else
                    {
                        //根据错误码处理错误
                    }
                }//End else if ( wsaNetworkEvents.lNetworkEvents &FD_READ)

                else if(wsaNetworkEvents.lNetworkEvents & FD_CLOSE)
                {
                    //且ErrorCode为0
                    if (wsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] == 0)
                    {
                        //处理关闭Socket
                        if(!Server.EventCloseOfThread(i))
                        {
                            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFun: 关闭客户端套接字失败"));
                        }
                        
                        //因为当次Socket已经被关闭,原本下一次的Socket为本次
                        i--;

                    }//End if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    else
                    {
                        //根据错误码处理错误
                    }
                }//End else if ( wsaNetworkEvents.lNetworkEvents & FD_CLOSE)
            }//End if (nRet == 0)
        }//End for (int i = nIndex; i < m_nCount; i++)
    }//End While(true)

    return 0;
}

BOOL CEventServer::EventAcceptOfThread(int i)     //用于线程中读数据
{
    if(m_dwCount == WSA_MAXIMUM_WAIT_EVENTS)
    {
        CMySocket::OutputErrorMsg(TEXT("CEventServer::EventAcceptOfThread: 客户端连接达到上限"));
        return FALSE;
    }

    //sockaddr_in addrRecv;
    //memset(&addrRecv, 0, sizeof(sockaddr_in));
    int nAddrRecvLen = sizeof(sockaddr_in);
    
    if(!m_Sock.Accept())
    {
        m_Sock.OutputErrorMsg(m_Sock.GetErrorMsg());
        return FALSE;
    }

    //保存ClientAddr
    m_addrClient[m_dwCount] = m_Sock.GetNewAddr();
    
    //再次将新产生的通信socket与事件关联
    m_hSocketAry[m_dwCount] = m_Sock.GetNewSock();
    m_hEventAry[m_dwCount] = WSACreateEvent();
    
    int nRet =  WSAEventSelect(m_hSocketAry[m_dwCount],
                               m_hEventAry[m_dwCount],
                               FD_READ | FD_ACCEPT | FD_CLOSE);
    
    if (SOCKET_ERROR == nRet)
    {
        //释放资源
        OutputDebugString("CEventServer::EventAcceptOfThread: 绑定失败，清除Event！");
        WSACloseEvent(m_hEventAry[m_dwCount]);
        return 0;
    }    
    
    //显示连接IP信息
//     char szBufPort[8] = {0};
//     
//     wsprintf(szBufPort, "%d", m_nCount);
//     int nRow = m_ListCtl.InsertItem(m_nCount, szBufPort);
//     
//     wsprintf(szBufPort, "%d", m_addrClient[i].sin_port);
//     m_ListCtl.SetItemText(nRow, 1, inet_ntoa(m_addrClient[i].sin_addr));
//     m_ListCtl.SetItemText(nRow, 2, szBufPort);
    
 
    //Event自增
    m_dwCount++; 
    
    return nRet;
}

BOOL CEventServer::EventReadOfThread(int i)
{
    BOOL bRet = FALSE;
    DWORD dwBufSize = 0;
    int nRecvBytes = 0;//每次接收的数据

#ifndef __SERVER
    const DWORD DEAL_SIZE = TagDealSize;
#else
    const DWORD DEAL_SIZE = TagCSInfoSize;
#endif
    
    static BOOL bIsFirstInto = TRUE;

    //如果已接收数据，比结构体小。
    if (m_SRInfo[i].m_dwRecvedSize < DEAL_SIZE)
    {
#ifndef __SERVER
        //客户端发送的是Deal
        nRecvBytes = ::recv(m_hSocketAry[i], 
                          (char*)&m_SRInfo[i].m_Deal + m_SRInfo[i].m_dwRecvedSize,
                          DEAL_SIZE - m_SRInfo[i].m_dwRecvedSize,
                          0);
#else
        //Mid发的是CSinfo
        nRecvBytes = ::recv(m_hSocketAry[i], 
                          (char*)&m_SRInfo[i] + m_SRInfo[i].m_dwRecvedSize,
                          DEAL_SIZE - m_SRInfo[i].m_dwRecvedSize,
                          0);
#endif
        
        
        //累加m_dwRecvedSize  已接收数据
        m_SRInfo[i].m_dwRecvedSize += nRecvBytes;
        
        if(m_SRInfo[i].m_dwRecvedSize != DEAL_SIZE)
        {
            return FALSE;
        }
    }

    //本次，第一次开始接收数据时，保存当前Socket
    if(bIsFirstInto)
    {
        //服务端保留中间层的Socket
#ifndef __SERVER
        //中间层保留Client的Socket
        m_SRInfo[i].m_Socket = m_hSocketAry[i];
#else
        m_SRInfo[i].m_MidSock = m_hSocketAry[i];
#endif
        
        bIsFirstInto = FALSE;
    }
    
    //说明头部已经读取完毕
    //开始申请空间
    if (m_SRInfo[i].m_Deal.m_pszData == NULL)
    {
        //创建一个足够大的缓冲区存放待接收数据
        m_SRInfo[i].m_Deal.m_pszData = new char[m_SRInfo[i].m_Deal.m_dwDataSize + 1];
        if (m_SRInfo[i].m_Deal.m_pszData == NULL)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: 申请BMP缓冲区失败"));
            //出错处理，释放资源
            return FALSE;
        }

        memset(m_SRInfo[i].m_Deal.m_pszData, 0, m_SRInfo[i].m_Deal.m_dwDataSize + 1);
    }
    
    LPTSTR pszBuf = m_SRInfo[i].m_Deal.m_pszData;

    //收取后续数据
    int nRecvedBufBytes = m_SRInfo[i].m_dwRecvedSize - DEAL_SIZE;

    if (nRecvedBufBytes < (int)m_SRInfo[i].m_Deal.m_dwDataSize)
    {

        nRecvBytes = ::recv(m_hSocketAry[i], 
                          pszBuf + nRecvedBufBytes,
                          m_SRInfo[i].m_Deal.m_dwDataSize - nRecvedBufBytes,
                          0);


        if(nRecvBytes == SOCKET_ERROR)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: 接收后续数据失败"));
            return FALSE;
        }
        
        m_SRInfo[i].m_dwRecvedSize += nRecvBytes;

        //如果后续数据没有接收完成，则继续
        if(m_SRInfo[i].m_dwRecvedSize - DEAL_SIZE != m_SRInfo[i].m_Deal.m_dwDataSize)
        {
            return FALSE;
        }
        
    }

    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: 接收数据成功"));
    
    //本次接收完成，则锁住线程，等待提取数据
    m_ThreadLock.Lock();

    //提供数据接口
    m_pInfoRet = &m_SRInfo[i];
    
    //数据解锁，通知外部可以操作数据
    m_DataLock.UnLock();
    
    
    //返回成功接收，且本次接收结束，置标志位为真，下次接收则为新进入
    bIsFirstInto = TRUE;
    return TRUE;
}

//用于线程中结束连接
BOOL CEventServer::EventCloseOfThread(int nIndex)
{
    BOOL bRet = FALSE;
    //保护数据-加锁
    m_ThreadCS.Lock();
    
    //关闭Socket
    if(!m_Sock.CloseSocket(m_hSocketAry[nIndex]))
    {
        //关闭失败，提交错误信息
        CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: 关闭套接字失败"));
        CMySocket::OutputErrorMsg(m_Sock.GetErrorMsg());
        return FALSE;
    }

    //Socket已经关闭，则客户端数量-1
    m_dwCount--;

    //关闭Event句柄
    ::CloseHandle(m_hEventAry[nIndex]);

    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: 关闭套接字成功"));


    //移动数据
    int i = nIndex;
    for(int j = i + 1; j < m_dwCount + 1; j++, i++)
    {
        
        if(m_hSocketAry[j] != INVALID_SOCKET)
        {
            //后边数据向前移动
            m_hSocketAry[i] = m_hSocketAry[j];
            m_hEventAry[i]  = m_hEventAry[j];
            m_addrClient[i] = m_addrClient[j];
        }
        else
        {
            //后一个数据移动之后，无用,置初始
            m_hSocketAry[j] = INVALID_SOCKET;
            m_hEventAry[j]  = INVALID_HANDLE_VALUE;
            memset(&m_addrClient[j], 0, sizeof(sockaddr_in));

            break;
        }
    }

    TCHAR szBuf[MAXBYTE] = {0};
    wsprintf(szBuf, TEXT("CEventServer::EventCloseOfThread: 还剩%d个客户端."), m_dwCount - 1);
    CMySocket::OutputErrorMsg(szBuf);
    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: Socket关闭"));


    //保护数据-解锁
    m_ThreadCS.Unlock();
    bRet = TRUE;

    return bRet;
}

//是否可以操作数据,会阻塞当前线程，
//在数据完成获取之后，线程通知解锁
BOOL CEventServer::IsCanHandleData()
{
    //告诉HandleOver() 本函数已经被调用
    m_bDataIsLock = TRUE;
    return m_DataLock.Lock();
}

PMyTagCSInfo CEventServer::GetRetData()
{
    ASSERT(m_pInfoRet != NULL);
    return m_pInfoRet;
}

BOOL CEventServer::HandleOver(MyTagCSInfo& CSInfo)
{
    if(!m_bDataIsLock)
    {
        MessageBox(NULL, TEXT("CEventServer::HandleOver: 请调用IsCanHandleData()."), TEXT("Error!"), MB_OK);
        CMySocket::OutputErrorMsg(TEXT("CEventServer::HandleOver: IsCanHandleData() 未调用，会造成死锁"));
        return FALSE;
    }
    
    //初始化CSInfo
    memset(&CSInfo, 0, TagCSInfoSize);

    //释放缓冲区
    if(CSInfo.m_Deal.m_pszData != NULL)
    {
        delete CSInfo.m_Deal.m_pszData;
        CSInfo.m_Deal.m_pszData = NULL;
    }
    
    //本次操作结束，重置标志位用于下次的检查
    m_bDataIsLock = FALSE;

    //操作结束，通知线程解锁，通知外部加锁
    //由于IsCanHandleData 已经给Data上锁，则不需要上锁
    return m_ThreadLock.UnLock();
}

//================================CEventServer================================


//=============================CMyEvent=======================================

CMyEvent::CMyEvent(BOOL bInitiallyOwn, 
                   BOOL bManualReset,
                   LPCTSTR lpszNAme, 
                   LPSECURITY_ATTRIBUTES lpsaAttribute)  throw(BOOL)
{
    m_hObject = ::CreateEvent(lpsaAttribute,    //安全属性
                              bManualReset,     //是否自动上锁，False表示为自动
                              bInitiallyOwn,    //False，为nonsignaled
                              lpszNAme);        //Event名字
    if (m_hObject == NULL)
		throw(FALSE);

    //初始化状态
    m_bIsAutoLock = !bManualReset;
    m_bIsLock = !bInitiallyOwn;
}

CMyEvent::~CMyEvent()
{
    if (m_hObject != NULL)
    {
        ::CloseHandle(m_hObject);
        m_hObject = NULL;
	}
}

//解锁
BOOL CMyEvent::SetEvent()
{ 
    BOOL bRet = ::SetEvent(m_hObject);
    
    if(bRet)
    {
        //执行成功，则为解锁状态
        m_bIsLock = FALSE;
    }
    
    return bRet; 
}

BOOL CMyEvent::PulseEvent()
{ 
    return ::PulseEvent(m_hObject); 
}

//上锁
BOOL CMyEvent::ResetEvent()
{ 
    BOOL bRet = ::ResetEvent(m_hObject);
    
    if(bRet)
    {
        //设置成功，则为上锁状态
        m_bIsLock = TRUE;
    }
    
    return bRet; 
}

//上锁
BOOL CMyEvent::Lock(DWORD dwTimeout)
{
    BOOL bRet = FALSE;

    if(!m_bIsAutoLock)
    {
        //上锁
        bRet = ResetEvent();
    }
    else
    {
        if (::WaitForSingleObject(m_hObject, dwTimeout) == WAIT_OBJECT_0)
            //设置返回值，当为自动上锁时，则为上锁状态
            m_bIsLock = bRet = TRUE;
        else
		    bRet = FALSE;
    }
    
    return bRet;
}

//解锁
BOOL CMyEvent::UnLock()
{
    BOOL bRet = FALSE;
    
    //解锁
    bRet = SetEvent();

    return bRet;
}

//检查状态
BOOL CMyEvent::IsLock() CONST
{
//     BOOL bRet = FALSE;
//     
//     bRet = WaitForSingleObject(m_hObject, 0);
// 
//     //没上锁
//     if(bRet == WAIT_OBJECT_0)
//     {
//         ResetEvent();
//         bRet = TRUE;
//     }
//     //上锁
//     else if(bRet == WAIT_TIMEOUT)
//     {
//         bRet = FALSE;
//     }

    return m_bIsLock;
}

CMyEvent::operator HANDLE() CONST
{
    return m_hObject;
}

//=============================CMyEvent=======================================


//================================CMySocket===================================

//--------------------------------Init_Fun------------------------------------

//错误码
INT CMySocket::m_nErrorCode = 0;


CMySocket::CMySocket()
{
    m_sMainSock = INVALID_SOCKET;
    m_sNewSock = INVALID_SOCKET;

    m_bIsServer = FALSE;

    memset(&m_MainAddr, 0, sizeof(m_MainAddr));
    memset(&m_NewAddr, 0, sizeof(m_NewAddr));
    
    InitSocketWSA();
}

CMySocket::~CMySocket()
{
    DestroySocketWSA();
    OutputErrorMsg(TEXT("CMySocket::~CMySocket: 已结束Socket"));
}
BOOL CMySocket::InitSocketWSA()      //初始化套接字环境
{
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        m_wVersionRequested = MAKEWORD( 2, 2 );
        
        m_err = ::WSAStartup( m_wVersionRequested, &m_wsaData );
        if ( m_err != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            OutputErrorMsg(TEXT("CMySocket::InitSocketWSA: 初始化Socket环境失败！"));
            return FALSE;
        }
        
        /* Confirm that the WinSock DLL supports 2.2.*/
        /* Note that if the DLL supports versions greater    */
        /* than 2.2 in addition to 2.2, it will still return */
        /* 2.2 in wVersion since that is the version we      */
        /* requested.                                        */
        
        if ( LOBYTE( m_wsaData.wVersion ) != 2 ||
            HIBYTE( m_wsaData.wVersion ) != 2 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            ::WSACleanup( );
            OutputErrorMsg(TEXT("CMySocket::InitSocketWSA: 初始化Socket环境失败！"));
            return FALSE; 
        }
        
        /* The WinSock DLL is acceptable. Proceed. */

        bIsFirstInto = FALSE;
    }
    return TRUE;
}

//提供关闭Socket功能
BOOL CMySocket::SocketOver()
{
    OutputErrorMsg(TEXT("CMySocket::SocketOver: 结束Socket"));
    return DestroySocketWSA();
}

BOOL CMySocket::DestroySocketWSA()      //关闭套接字环境
{
    static BOOL bIsFirstInto = TRUE;    //是否为第一次进入
    BOOL bRet = TRUE;

    if(bIsFirstInto)
    {
        //关闭Socket
        if(m_sMainSock != INVALID_SOCKET)
        {
            ::closesocket(m_sMainSock);
            m_sMainSock = INVALID_SOCKET;
        }
        
        if(m_sNewSock != INVALID_SOCKET)
        {
            ::closesocket(m_sNewSock);
            m_sNewSock = INVALID_SOCKET;
        }
        
        //清理环境
        ::WSACleanup();
        
        OutputErrorMsg(TEXT("CMySocket::DestroySocketWSA: 关闭Socket成功！"));

        bIsFirstInto = FALSE;
    }
    
    return TRUE;
}


LPCTSTR CMySocket::GetErrorMsg() 
{
    //转换错误码
    LPCTSTR pStr = NULL;
    pStr = ConvertErrorMsg(m_nErrorCode);
    
    return pStr;
}

BOOL CMySocket::Create(UINT nSocketPort,
                       LPCTSTR lpszSocketAddress/* = NULL*/,
                       int nSocketType/* = SOCK_STREAM*/)
{
    //创建套接字
    if(!CreateSocket(nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::Create：创建套接字失败"));
        return FALSE;
    }

    //初始化地址
    if(lpszSocketAddress != NULL)
    {
        //给予连接地址，则为Client
        m_bIsServer = FALSE;
        SetAddr(lpszSocketAddress, nSocketPort);
    }
    else
    {
        //没有给予地址，则为Server
        m_bIsServer = TRUE;
        SetAddr(INADDR_LOOPBACK, nSocketPort);
    }

    return TRUE;
}

BOOL CMySocket::CreateSocket(int type/* = SOCK_STREAM*/,
                             int af/* = AF_INET*/,
                             int protocol/* = 0*/)    //创建套接字
{
    BOOL bRet = TRUE;
    
    m_sMainSock = ::socket(af, type, protocol);
    if(m_sMainSock == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        bRet = FALSE;
    }
    
    return bRet;
}

BOOL CMySocket::CloseSocket(SOCKET& sCur)     //关闭指定套接字
{
    BOOL bRet = FALSE;

    bRet = ::closesocket(sCur);
    if(bRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }
    sCur = INVALID_SOCKET;
    return bRet;
}

BOOL CMySocket::SetAddr(/*sockaddr_in& Addr = m_MainAddr,*/ LPCTSTR pIP, WORD wPort)
{
    ::memset(&m_MainAddr, 0, sizeof(sockaddr_in));
    
    m_MainAddr.sin_family = AF_INET;
    m_MainAddr.sin_port = ::htons(wPort);
    m_MainAddr.sin_addr.s_addr = ::inet_addr(pIP);
    return TRUE;
}

BOOL CMySocket::SetAddr(/*sockaddr_in& Addr = m_MainAddr,*/ DWORD dwIP, WORD wPort)
{
    ::memset(&m_MainAddr, 0, sizeof(sockaddr_in));
    
    m_MainAddr.sin_family = AF_INET;
    m_MainAddr.sin_port = ::htons(wPort);
    m_MainAddr.sin_addr.s_addr = ::htonl(dwIP);
    return TRUE;
}

BOOL CMySocket::Bind(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/)
{
    int nRet = ::bind(m_sMainSock, (sockaddr*)&m_MainAddr, sizeof(sockaddr_in));
    if(nRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        return FALSE;
    }
    
    return TRUE;
}

BOOL CMySocket::Listen(/*SOCKET sScok = m_MainSock,*/ int backlog)
{
    int nRet = ::listen(m_sMainSock, backlog);
    if(nRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        return FALSE;
    }

    return TRUE;
}

BOOL CMySocket::Accept(/*_OUT_ SOCKET& sNewScok, _OUT_ sockaddr_in& NewAddr*/)
{
    int nLen = sizeof(sockaddr_in);
    m_sNewSock = ::accept(m_sMainSock, (sockaddr*)&m_NewAddr, &nLen);
    if(m_sNewSock == INVALID_SOCKET)
    {
        m_nErrorCode = ::WSAGetLastError();
        return FALSE;
    }

    return TRUE;
}

BOOL CMySocket::Connect(/*SOCKET sScok = m_MainSock, sockaddr_in& Addr = m_MainAddr*/)
{
    int nRet = ::connect(m_sMainSock, (sockaddr*)&m_MainAddr, sizeof(sockaddr_in));
    if(nRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        return FALSE;
    }
    return TRUE;
}

//--------------------------------Init_Fun------------------------------------

BOOL CMySocket::StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType)
{
    //初始化Socket环境
    if(!InitSocketWSA())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: 初始化Socket成功"));
    }
    
    //创建套接字
    if(!Create(nSocketPort, lpszSocketAddress, nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: 创建套接字失败"));
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: 创建成功"));
    }

    //绑定端口
    if(!Bind())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: 绑定成功"));
    }
    

    //监听
    if(!Listen())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: 监听成功"));
    }

    return TRUE;
}

BOOL CMySocket::StartClient(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType)
{
    //初始化Socket环境
    if(!InitSocketWSA())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: 初始化Socket成功"));
    }
    
    //创建套接字
    if(!Create(nSocketPort, lpszSocketAddress, nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: 创建套接字失败"));
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: 创建套接字成功"));
    }

    //连接服务器
    if(!Connect())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: 连接成功"));
    }

    //发送

    //接收

    return TRUE;
}

INT CMySocket::Send(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, _IN_ DWORD dwSize)
{
    int nRet = ::send(sDstScok, pszBuf, dwSize, 0);

    if(nRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        return nRet;
    }

    return nRet;
}

INT CMySocket::Recv(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, _IN_ DWORD dwSize)
{
    ASSERT(pszBuf != NULL);

    TCHAR szBuf[MAX_RECV_BYTES] = {0};

    int nRet = ::recv(sDstScok, szBuf, dwSize, 0);
    if(nRet == SOCKET_ERROR)
    {
        m_nErrorCode = ::WSAGetLastError();
        return nRet;
    }
    
    memcpy(pszBuf, szBuf, nRet);


    return nRet;
}

BOOL CMySocket::SendAll(SOCKET sDstScok, _IN_ LPCTSTR pszBuf, DWORD dwSize)
{
    BOOL bRet = FALSE;
    int nSendedTotalBytes = 0;
    int nSendedTmpBytes = 0;
    
    __try
    {
        if (dwSize <= 0 || pszBuf == NULL)
        {
            OutputErrorMsg(TEXT("CMySocket::SendData:长度或缓冲区为空"));
            __leave;
        }
        
        //循环写Socket数据
        while (dwSize > nSendedTotalBytes)
        {
            nSendedTmpBytes  = ::send(sDstScok, 
                                      pszBuf + nSendedTotalBytes, 
                                      dwSize - nSendedTotalBytes, 0);
            
            if (nSendedTmpBytes == SOCKET_ERROR )
            {
                m_Event.Lock();
                
                m_nErrorCode = ::WSAGetLastError();
                
                OutputDebugString(GetErrorMsg());

                m_Event.Unlock();
                __leave;
            }
            
            nSendedTotalBytes += nSendedTmpBytes;
        }
        
        bRet = TRUE;
    }
    __finally
    {
        m_Event.Unlock();
    }
    
    return bRet;
}
BOOL CMySocket::RecvAll(SOCKET sDstScok, _IN_OUT_ LPTSTR pszBuf, DWORD dwSize)
{
    BOOL bRet = FALSE;
    int nRecvedTotalBytes = 0;
    int nRecvedTmpBytes = 0;
    
    ASSERT(pszBuf != NULL);
    
    TCHAR szBuf[MAX_RECV_BYTES] = {0};

    __try
    {
        if (dwSize <= 0 )
        {
            OutputErrorMsg(TEXT("CMySocket::RecvData:长度空"));
            __leave;
        }
//         if(!strBuf.empty())
//         {
//             strBuf = TEXT("");
//             OutputErrorMsg(TEXT("初始化缓冲区"));
//         }
        
        //循环读Socket数据
        while (dwSize > nRecvedTotalBytes)
        {
            nRecvedTmpBytes  = ::recv(sDstScok, 
                                      szBuf + nRecvedTotalBytes, 
                                      dwSize - nRecvedTotalBytes,
                                      0);
            
            if(nRecvedTmpBytes == SOCKET_ERROR)
            {
                //保护类中的错误码
                m_Event.Lock();

                m_nErrorCode = ::WSAGetLastError();
                
                OutputDebugString(GetErrorMsg());

                m_Event.Unlock();
                __leave;
            }
            
            nRecvedTotalBytes += nRecvedTmpBytes;
        }    
        memcpy(pszBuf, szBuf, dwSize);
        bRet = TRUE;
    }
    __finally
    {
        m_Event.Unlock();
    }
    
    return bRet;
}


//获取Socket
SOCKET CMySocket::GetMainSock() CONST
{
    return m_sMainSock;
}

SOCKET CMySocket::GetNewSock() CONST
{
    return m_sNewSock;
}

//获取Addr
sockaddr_in CMySocket::GetMainAddr() CONST
{
    return m_MainAddr;
}

sockaddr_in CMySocket::GetNewAddr() CONST
{
    return m_NewAddr;
}



//==========================OutPutErrorMsg====================================
void CMySocket::OutputErrorMsg(LPCTSTR strError)
{
    TCHAR szBuf[MAXBYTE] = {0};
    if(strError == NULL)
    {
        ::wsprintf(szBuf, TEXT("CMySocket::OutputErrorMsg: 无法解析这个错误码：%d。"), m_nErrorCode);
        strError = szBuf;
    }
    
    ::OutputDebugString(strError);
}

LPCTSTR CMySocket::ConvertErrorMsg(int nErrorCode)
{
    switch(nErrorCode)
    {
    case WSA_INVALID_HANDLE:
        return TEXT("指定的事件对象无效。若使用与Win32函数对应的Winsock函数，便有可能产生这样的Win32错误。它表明传递给WSAWaitForMultipleEvents的一个句柄是无效的");
        break;
        
    case WSA_NOT_ENOUGH_MEMORY:
        return TEXT("内存不够。这个Win32错误指出内存数量不足，无法完成指定的操作");
        break;
        
    case WSA_INVALID_PARAMETER:
        return TEXT("一个或多个参数无效。这个Win32错误表明传递到函数内部的参数无效。假若事件计数参数无效，那么在执行WSAWaitForMultipleEvents的时候，也会发生这样的错误");
        break;
        
    case WSA_WAIT_TIMEOUT:
        return TEXT("操作超时。这个Win32错误指出重叠I/O操作未在规定的时间内完成");
        break;
        
    case WSA_OPERATION_ABORTED:
        return TEXT("重叠操作被取消。这个Win32错误指出由于套接字的关闭，造成一次重叠I/O操作的取消。除此以外，该错误也可能在执行SIO_FLUSH这个I/O控制命令时出现");
        break;
        
    case WSA_IO_INCOMPLETE:
        return TEXT("重叠I/O事件对象未处于传信状态。这个Win32错误也和重叠I/O操作密切相关，在调用WSAGetOverlappedResults函数的时候产生，指出重叠I/O操作尚未完成");
        break;
        
    case WSA_IO_PENDING:
        return TEXT("重叠操作将在以后完成。用Winsock函数发出一次重叠I/O操作时，若出现这样的Win32错误，便表明操作尚未完成，而且会在以后的某个时间完成");
        break;
        
    case WSAEINTR:
        return TEXT("函数调用中断。该错误表明由于对WSACancelBlockingCall的调用，造成了一次调用被强行中断");
        break;
        
    case WSAEBADF:
        return TEXT("文件句柄错误。该错误表明提供的文件句柄无效。在MicrosoftWindowsCE下，socket函数可能返回这个错误，表明共享串口处于“忙”状态");
        break;
        
    case WSAEACCES:
        return TEXT("权限被拒。尝试对套接字进行操作，但被禁止。若试图在sendto或WSASendTo中使用一个广播地址，但是尚未用setsockopt和SO_BROADCAST这两个选项设置广播权限，便会产生这类错误");
        break;
        
    case WSAEFAULT:
        return TEXT("地址无效。传给Winsock函数的指针地址无效。若指定的缓冲区太小，也会产生这个错误");
        break;
        
    case WSAEINVAL:
        return TEXT("参数无效。指定了一个无效参数。例如，假如为WSAIoctl调用指定了一个无效控制代码，便会产生这个错误。另外，它也可能表明套接字当前的状态有错，例如在一个没有监听的套接字上调用accept或WSAAccept的时候");
        break;
        
    case WSAEMFILE:
        return TEXT("打开文件过多。提示打开的套接字太多了。通常，Microsoft提供者只受到系统内可用资源数量的限制");
        break;
        
    case WSAEWOULDBLOCK:
        return TEXT("资源暂时不可用。对非阻塞套接字来说，如果请求操作不能立即执行的话，通常会返回这个错误。比如说，在一个非阻塞套接字上调用connect，就会返回这个错误。因为连接请求不能立即执行");
        break;
        
    case WSAEINPROGRESS:
        return TEXT("操作正在进行中。当前正在执行非锁定操作。一般来说不会出现这个错误，除非正在开发16位Winsock应用程序");
        break;
        
    case WSAEALREADY:
        return TEXT("操作已完成。一般来说，在非锁定套接字上尝试已处于进程中的操作时，会产生这个错误。比如，在一个已处于连接进程的非锁定套接字上，再一次调用connect或WSAConnect。另外，服务提供者处于执行回调函数（针对支持回调例程的Winsock函数）的进程中时也会出现这个错误");
        break;
        
    case WSAENOTSOCK:
        return TEXT("无效套接字上的套接字操作。任何一个把SOCKET句柄当作参数的Winsock函数都会返回这个错误。它表明提供的套接字句柄无效");
        break;
        
    case WSAEDESTADDRREQ:
        return TEXT("需要目标地址。这个错误表明没有提供具体地址。比方说，假如在调用sendto时，将目标地址设为INADDR_ANY（任意地址），便会返回这个错误");
        break;
        
    case WSAEMSGSIZE:
        return TEXT("消息过长。这个错误的含义很多。如果在一个数据报套接字上发送一条消息，这条消息对内部缓冲区而言太大的话，就会产生这个错误。再比如，由于网络自身的限制，使一条消息过长，也会产生这个错误。最后，如果收到数据报之后，缓冲区太小，不能接收消息时，也会产生这个错误");
        break;
        
    case WSAEPROTOTYPE:
        return TEXT("套接字协议类型有误。在socket或WSASocket调用中指定的协议不支持指定的套接字类型。比如，要求建立SOCK_STREAM类型的一个IP套接字，同时指定协议为IPPROTO_UDP，便会产生这样的错误");
        break;
        
    case WSAENOPROTOOPT:
        return TEXT("协议选项错误。表明在getsockopt或setsockopt调用中，指定的套接字选项或级别不明、未获支持或者无效");
        break;
        
    case WSAEPROTONOSUPPORT:
        return TEXT("不支持的协议。系统中没有安装请求的协议或没有相应的实施方案。比如，如果系统中没有安装TCP/IP，而试着建立TCP或UDP套接字时，就会产生这个错误");
        break;
        
    case WSAESOCKTNOSUPPORT:
        return TEXT("不支持的套接字类型。对指定的地址家族来说，没有相应的具体套接字类型支持。比如，在向一个不支持原始套接字的协议请求建立一个SOCK_RAW套接字类型时，就会产生这个错误");
        break;
        
    case WSAEOPNOTSUPP:
        return TEXT("不支持的操作。表明针对指定的对象，试图采取的操作未获支持。通常，如果试着在一个不支持调用Winsock函数的套接字上调用了Winsock时，就会产生这个错误。比如，在一个数据报套接字上调用accept或WSAAccept函数时，就会产生这样的错误");
        break;
        
    case WSAEPFNOSUPPORT:
        return TEXT("不支持的协议家族。请求的协议家族不存在，或系统内尚未安装。多数情况下，这个错误可与WSAEAFNOSUPPORT互换（两者等价）；后者出现得更为频繁");
        break;
        
    case WSAEAFNOSUPPORT:
        return TEXT("地址家族不支持请求的操作。对套接字类型不支持的操作来说，在试着执行它时，就会出现这个错误。比如，在类型为SOCK_STREAM的一个套接字上调用sendto或WSASendTo函数时，就会产生这个错误。另外，在调用socket或WSASocket函数的时候，若同时请求了一个无效的地址家族、套接字类型及协议组合，也会产生这个错误");
        break;
        
    case WSAEADDRINUSE:
        return TEXT("下地址正在使用。正常情况下，每个套接字只允许使用一个套接字地址（例如，一个IP套接字地址由本地IP地址及端口号组成）。这个错误一般和bind、connect和WSAConnect这三个函数有关。可在setsockopt函数中设置套接字选项SO_REUSEADDR，允许多个套接字访问同一个本地IP地址及端口号");
        break;
        
    case WSAEADDRNOTAVAIL:
        return TEXT("不能分配请求的地址。API调用中指定的地址对那个函数来说无效时，就会产生这样的错误。例如，若在bind调用中指定一个IP地址，但却没有对应的本地IP接口，便会产生这样的错误。另外，通过connect、WSAConnect、sendto、WSASendTo和WSAJoinLeaf这四个函数为准备连接的远程计算机指定端口0时，也会产生这样的错误");
        break;
        
    case WSAENETDOWN:
        return TEXT("网络断开。试图采取一项操作时，却发现网络连接中断。这可能是由于网络堆栈的错误，网络接口的故障，或者本地网络的问题造成的");
        break;
        
    case WSAENETUNREACH:
        return TEXT("网络不可抵达。试图采取一项操作时，却发现目标网络不可抵达（不可访问）。这意味着本地主机不知道如何抵达一个远程主机。换言之，目前没有已知的路由可抵达那个目标主机");
        break;
        
    case WSAENETRESET:
        return TEXT("网络重设时断开了连接。由于“保持活动”操作检测到一个错误，造成网络连接的中断。若在一个已经无效的连接之上，通过setsockopt函数设置SO_KEEPALIVE选项，也会出现这样的错误");
        break;
        
    case WSAECONNABORTED:
        return TEXT("软件造成连接取消。由于软件错误，造成一个已经建立的连接被取消。典型情况下，这意味着连接是由于协议或超时错误而被取消的");
        break;
        
    case WSAECONNRESET:
        return TEXT("连接被对方重设。一个已经建立的连接被远程主机强行关闭。若远程主机上的进程异常中止运行（由于内存冲突或硬件故障），或者针对套接字执行了一次强行关闭，便会产生这样的错误。针对强行关闭的情况，可用SO_LINGER套接字选项和setsockopt来配置一个套接字");
        break;
        
    case WSAENOBUFS:
        return TEXT("没有缓冲区空间。由于系统缺少足够的缓冲区空间，请求的操作不能执行");
        break;
        
    case WSAEISCONN:
        return TEXT("套接字已经连接。表明在一个已建立连接的套接字上，试图再建立一个连接。要注意的是，数据报和数据流套接字均有可能出现这样的错误。使用数据报套接字时，假如事先已通过connect或WSAConnect调用，为数据报通信关联了一个端点的地址，那么以后试图再次调用sendto或WSASendTo，便会产生这样的错误");
        break;
        
    case WSAENOTCONN:
        return TEXT("套接字尚未连接。若在一个尚未建立连接的“面向连接”套接字上发出数据收发请求，便会产生这样的错误");
        break;
        
    case WSAESHUTDOWN:
        return TEXT("套接字关闭后不能发送。表明已通过对shutdown的一次调用，部分关闭了套接字，但事后又请求进行数据的收发操作。要注意的是，这种错误只会在已经关闭的那个数据流动方向上才会发生。举个例子来说，完成数据发送后，若调用shutdown，那么以后任何数据发送调用都会产生这样的错误");
        break;
        
    case WSAETIMEDOUT:
        return TEXT("连接超时。若发出了一个连接请求，但经过规定的时间，远程计算机仍未作出正确的响应（或根本没有任何响应），便会发生这样的错误。要想收到这样的错误，通常需要先在套接字上设置好SO_SNDTIMEO和SO_RCVTIMEO选项，然后调用connect及WSAConnect函数。要想了解在套接字上设置SO_SNDTIMEO和SO_RCVTIMEO选项的详情，可参考第9章");
        break;
        
    case WSAECONNREFUSED:
        return TEXT("连接被拒。由于被目标机器拒绝，连接无法建立。这通常是由于在远程机器上，没有任何应用程序可在那个地址之上，为连接提供服务");
        break;
        
    case WSAEHOSTDOWN:
        return TEXT("主机关闭。这个错误指出由于目标主机关闭，造成操作失败。然而，应用程序此时更有可能收到的是一条WSAETIMEDOUT（连接超时）错误，因为对方关机的情况通常是在试图建立一个连接的时候发生的");
        break;
        
    case WSAEHOSTUNREACH:
        return TEXT("没有到主机的路由。应用程序试图访问一个不可抵达的主机。该错误类似于WSAENETUNREACH");
        break;
        
    case WSAEPROCLIM:
        return TEXT("进程过多。有些Winsock服务提供者对能够同时访问它们的进程数量进行了限制");
        break;
        
    case WSASYSNOTREADY:
        return TEXT("网络子系统不可用。调用WSAStartup时，若提供者不能正常工作（由于提供服务的基层系统不可用），便会返回这种错误");
        break;
        
    case WSAVERNOTSUPPORTED:
        return TEXT("Winsock.dll版本有误。表明不支持请求的Winsock提供者版本");
        break;
        
    case WSANOTINITIALISED:
        return TEXT("Winsock尚未初始化。尚未成功完成对WSAStartup的一次调用");
        break;
        
    case WSAEDISCON:
        return TEXT("正在从容关闭。这个错误是由WSARecv和WSARecvFrom返回的，指出远程主机已初始化了一次从容关闭操作。该错误是在像ATM这样的“面向消息”协议上发生的");
        break;
        
    case WSAENOMORE:
        return TEXT("找不到更多的记录。这个错误自WSALookupServiceNext函数返回，指出已经没有留下更多的记录。这个错误通常可与WSA_E_NO_MORE互换使用。在应用程序中，应同时检查这个错误以及WSA_E_NO_MORE");
        break;
        
    case WSAECANCELLED:
        return TEXT("操作被取消。这个错误指出当WSALookupServiceNext调用仍在处理期间，发出了对WSALookupServiceEnd（服务中止）的一个调用。此时，WSALookupServiceNext便会返回这个错误。这个错误代码可与WSA_E_CANCELLED互换使用。作为应用程序，应同时检查这个错误以及WSA_E_CANCELLED");
        break;

    case WSAEINVALIDPROVIDER:
        return TEXT("无效的服务提供者。这个错误同服务提供者关联在一起，在提供者不能建立正确的Winsock版本，从而无法正常工作的前提下产生");
        break;
        
    case WSAEPROVIDERFAILEDINIT:
        return TEXT("提供者初始化失败。这个错误同服务提供者关联在一起，通常见于提供者不能载入需要的DLL时");
        break;
        
    case WSASYSCALLFAILURE:
        return TEXT("系统调用失败。表明绝对不应失败的一个系统调用却令人遗憾地失败了");
        break;
        
    case WSASERVICE_NOT_FOUND:
        return TEXT("找不到这样的服务。这个错误通常与注册和名字解析函数相关，在查询服务时产生（第10章对这些函数进行了详尽解释）。该错误表明，在给定的名字空间内，找不到请求的服务");
        break;
        
    case WSATYPE_NOT_FOUND:
        return TEXT("找不到类的类型。该错误也与注册及名字解析函数关联在一起，在处理服务类（Service Class）时发生。若注册好一个服务的实例，它必须引用一个以前通过WSAInstallServiceClass安装好的服务");
        break;
        
    case WSA_E_NO_MORE:
        return TEXT("找不到更多的记录。这个错误是自WSALookupServiceNext调用返回的，指出已经没有剩下的记录。该错误通常可与WSAENOMORE互换使用。作为一个应用程序，应同时检查这个错误以及WSAENOMORE");
        break;
        
    case WSA_E_CANCELLED:
        return TEXT("操作被取消。该错误指出在对WSALookupServiceNext的调用尚未完成的时候，又发出了对WSALookupServiceEnd（中止服务）的一个调用。这样，WSALookupServiceNext就会返回该错误。这个错误代码可与WSAECANCELLED互换使用。作为一个应用程序，应同时检查这个错误以及WSAECANCELLED");
        break;
        
    case WSAEREFUSED:
        return TEXT("查询被拒。由于被主动拒绝，所以一个数据库查询操作失败");
        break;
        
    case WSAHOST_NOT_FOUND:
        return TEXT("主机没有找到。这个错误是在调用gethostbyname和gethostbyaddr时产生的，表明没有找到一个授权应答主机（AuthoritativeAnswerHost）");
        break;
        
    case WSATRY_AGAIN:
        return TEXT("非授权主机没有找到。这个错误也是在调用gethostbyname和gethostbyaddr时产生的，表明没有找到一个非授权主机，或者遇到了服务器故障");
        break;
        
    case WSANO_RECOVERY:
        return TEXT("遇到一个不可恢复的错误。这个错误也是在调用gethostbyname和gethostbyaddr时产生的，指出遇到一个不可恢复的错误，应再次尝试操作");
        break;
        
    case WSANO_DATA:
        return TEXT("没有找到请求类型的数据记录。这个错误也是在调用gethostbyname和gethostbyaddr时产生的，指出尽管提供的名字有效，但却没有找到与请求类型对应的数据记录");
        break;
        
    case WSA_QOS_RECEIVERS:
        return TEXT("至少有一条预约消息抵达。这个值同IP服务质量（QoS）有着密切的关系，其实并不是一个真正的“错误”（QoS的详情见第12章）。它指出网络上至少有一个进程希望接收QoS通信");
        break;
        
    case WSA_QOS_SENDERS:
        return TEXT("至少有一条路径消息抵达。这个值同QoS关联在一起，其实更像一种状态报告消息。它指出在网络上，至少有一个进程希望进行QoS数据的发送");
        break;
        
    case WSA_QOS_NO_SENDERS:
        return TEXT("没有QoS发送者。这个值同QoS关联在一起，指出不再有任何进程对QoS数据的发送有兴趣。请参阅第12章，了解在发生这样的错误时，对所发生情况的一系列完整说明");
        break;
        
    case WSA_QOS_NO_RECEIVERS:
        return TEXT("没有QoS接收者。这个值同QoS关联在一起，指出不再有任何进程对QoS数据的接收有兴趣。请参阅第12章，查阅对这个错误的完整说明");
        break;
        
    case WSA_QOS_REQUEST_CONFIRMED:
        return TEXT("预约请求已被确认。QoS应用可事先发出请求，希望在批准了自己对网络带宽的预约请求后，收到通知。若发出了这样的请求，一旦批准，便会收到这样的消息。请参阅第12章，了解对此消息的详细说明");
        break;
        
    case WSA_QOS_ADMISSION_FAILURE:
        return TEXT("缺乏资源致错。资源不够，以至于无法满足QoS带宽请求");
        break;
        
    case WSA_QOS_POLICY_FAILURE:
        return TEXT("证书无效。表明发出QoS预约请求的时候，要么用户并不具备正确的权限，要么提供的证书无效");
        break;
        
    case WSA_QOS_BAD_STYLE:
        return TEXT("未知或冲突的样式。QoS应用程序可针对一个指定的会话，建立不同的过滤器样式。若出现这一错误，表明指定的样式类型要么未知，要么存在冲突。请参阅第12章，了解对过滤器样式的详细说明");
        break;
        
    case WSA_QOS_BAD_OBJECT:
        return TEXT("无效的FILTERSPEC结构或者提供者特有对象。假如为QoS对象提供的FILTERSPEC结构无效，或者提供者特有的缓冲区无效，便会返回这样的错误，详见第12章");
        break;
        
    case WSA_QOS_TRAFFIC_CTRL_ERROR:
        return TEXT("FLOWSPEC有问题。假如通信控制组件发现指定的FLOWSPEC参数存在问题（作为QoS对象的一个成员传递），便会返回这样的错误");
        break;
        
    case WSA_QOS_GENERIC_ERROR:
        return TEXT("常规QoS错误。这是一个比较泛泛的错误；假如其他QoS错误都不适合，便返回这个错误");
        break;
    }
    return NULL;
}
