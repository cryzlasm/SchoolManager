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
    //��ʼ��
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

    m_DataLock.ResetEvent();    //Ĭ�ϴ�����������
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
    //�Ƿ�Ϊ��һ�ν���
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //10��
        #define TEN_SECOND 10000

        //�ñ�־λ
        m_bIsNoExit = FALSE;

        //����WSAEventһ���˳��ź�
        WSASetEvent(m_hEventAry[0]);

        //����߳���������
        if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
        {
            //�ȴ��߳��˳�,��30��
            DWORD dwRet = WaitForSingleObject(m_pEventThread->m_hThread, TEN_SECOND);
            if(dwRet == WAIT_TIMEOUT)
            {
                CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer���ȴ��߳��˳���ʱ��"));
                
                //��ֹ�̣߳�
                if(!TerminateThread(m_pEventThread->m_hThread, 1))
                {
                    CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer����ֹ�߳�ʧ�ܣ�����Ȩ�޲���"));
                    
                    //����߳��޷���ֹ,����ͣ��
                    if(m_pEventThread->SuspendThread() != -1)
                    {
                        CMySocket::OutputErrorMsg(TEXT("CEventServer::EndServer����ͣ�̣߳���ֹ����Ұָ��"));
                    }

                }//End if(!TerminateThread(m_pEventThread->m_hThread, 1))
            }//End if(dwRet == WAIT_TIMEOUT)

            
        }//End if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
        

        


        //�����ر�Socket
        for(int i = m_dwCount; i >= 0; i--)
        {
            m_Sock.CloseSocket(m_hSocketAry[i]);
            
//             //�ر� �ں˾��
//             if(m_hEventAry[i] != INVALID_HANDLE_VALUE)
//             {
//                 WSACloseEvent(m_hEventAry[i]);
//                 m_hEventAry[i] = INVALID_HANDLE_VALUE;
//             }
        }

        //�ر��߳̾�� AfxBeginThread���Զ��رվ��
//         if(m_pEventThread != NULL && m_pEventThread->m_hThread != INVALID_HANDLE_VALUE)
//         {
//             ::CloseHandle(m_pEventThread->m_hThread);
//             m_pEventThread->m_hThread = INVALID_HANDLE_VALUE;
//         }

        bIsFirstInto = FALSE;

        //������֪ͨ�ⲿ�˳�
        m_DataLock.UnLock();
        CMySocket::OutputErrorMsg(TEXT("EndServer���˳�Eventģ��"));
    }//End if(bIsFirstInto)

    return TRUE;
}

BOOL CEventServer::GetExitInfo() CONST
{
    return m_bIsNoExit;
}

//����Server�� nSocketPort(�������˿�)  lpszSocketAddress(��������ַ) nSocketType(UDP || TCP)
BOOL CEventServer::StartServer(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType) throw(int)
{
    BOOL bRet = FALSE;
    
    bRet = m_Sock.StartServer(nSocketPort, lpszSocketAddress, nSocketType);

    //��������Socket
    m_hSocketAry[m_dwCount] = m_Sock.GetMainSock();

    //����һ���¼�
    //ÿ����һ���׽��֣��ʹ���һ���¼�����
    //�����ǵľ���ֱ����������������У�
    //������ WSAEventSelect ������ǵĹ�����
    m_hEventAry[m_dwCount] = WSACreateEvent();

    //�����ַ
    m_addrClient[m_dwCount] = m_Sock.GetMainAddr();

    bRet =  WSAEventSelect(m_hSocketAry[m_dwCount],
                           m_hEventAry[m_dwCount],
                           FD_ACCEPT | FD_READ | FD_CLOSE);

    if (SOCKET_ERROR == bRet)
    {
        //�ͷ���Դ
        WSACloseEvent(m_hEventAry[m_dwCount]);

        OutputDebugString(TEXT("CEventServer���¼�ģ�ʹ���ʧ��"));
        
        throw(RET_ERROR);
    }
    
    m_dwCount++;
    
    //����һ���߳������մ���socket����
    m_pEventThread = AfxBeginThread((AFX_THREADPROC)ThreadFunc, this);

    return TRUE;
}

DWORD CEventServer::ThreadFunc(LPVOID lparam)
{
    CEventServer& Server = *(CEventServer*) lparam;

    BOOL bIsNoExit = FALSE;
    //ͨ���ȴ��¼������������жϺʹ���socket��Ϣ
    char szBuf[MAXBYTE] = {0};
    while(TRUE)
    {
        //��ȡ��ǰ�¼�
        DWORD  dwRet = WSAWaitForMultipleEvents(
                                Server.m_dwCount,    //EventAry�ĸ���
                                Server.m_hEventAry, //EventAry����ָ��
                                FALSE,       //�Ƿ�ȴ������¼����󱻴���
                                INFINITE,    //�ȴ��¼�
                                FALSE);
        if (dwRet == WSA_WAIT_FAILED)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFun: Event �ȴ�ʧ��"));
            closesocket(Server.m_hSocketAry[0]);
            WSACleanup();
            return -1;
        }

        //�߳��˳���־
        if(!(bIsNoExit = Server.GetExitInfo()))
        {
            
            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFunc���߳�֪ͨ�˳����˳��ɹ�"));
            AfxEndThread(bIsNoExit);
        }
        
        //�ɹ����ȡEvent�±�
        int nIndex = dwRet - WSA_WAIT_EVENT_0;

        for (int i = nIndex; i < Server.m_dwCount; i++)
        {
            //����һ���¼�һ��ִ�еĻ���
            dwRet = WSAWaitForMultipleEvents(
                            1,    //EventAry�ĸ���
                            &Server.m_hEventAry[i], //EventAry����ָ��
                            FALSE,       //�Ƿ�ȴ������¼����󱻴���
                            0,    //�ȴ��¼�
                            FALSE);
            //�����һ����Ϊ�գ��������
            if (dwRet == WSA_WAIT_FAILED || dwRet == WSA_WAIT_TIMEOUT)
            {
                continue;
            }

            WSANETWORKEVENTS wsaNetworkEvents;

            //ö�������¼�����ȡ�¼���
            int nRet = WSAEnumNetworkEvents(Server.m_hSocketAry[i],
                                            Server.m_hEventAry[i],
                                            &wsaNetworkEvents);
            //��ʾ�ɹ�
            if (nRet == 0)
            {
                //����¼�ΪFD_����
                if ( wsaNetworkEvents.lNetworkEvents & FD_ACCEPT)
                {
                    //��ErrorCodeΪ0
                    if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    {
                        //�������
                        Server.EventAcceptOfThread(i);
                    }//End if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    else
                    {
                        //���ݴ����봦�����
                    }
                }//End if ( wsaNetworkEvents.lNetworkEvents &FD_ACCEPT)

                //��������¼�ΪFD_��
                else if ( wsaNetworkEvents.lNetworkEvents & FD_READ)
                {
                    //��ErrorCodeΪ0
                    if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
                    {
                        if(!Server.EventReadOfThread(i))
                        {
                            continue;
                        }

                    }//End if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
                    else
                    {
                        //���ݴ����봦�����
                    }
                }//End else if ( wsaNetworkEvents.lNetworkEvents &FD_READ)

                else if(wsaNetworkEvents.lNetworkEvents & FD_CLOSE)
                {
                    //��ErrorCodeΪ0
                    if (wsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] == 0)
                    {
                        //����ر�Socket
                        if(!Server.EventCloseOfThread(i))
                        {
                            CMySocket::OutputErrorMsg(TEXT("CEventServer::ThreadFun: �رտͻ����׽���ʧ��"));
                        }
                        
                        //��Ϊ����Socket�Ѿ����ر�,ԭ����һ�ε�SocketΪ����
                        i--;

                    }//End if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    else
                    {
                        //���ݴ����봦�����
                    }
                }//End else if ( wsaNetworkEvents.lNetworkEvents & FD_CLOSE)
            }//End if (nRet == 0)
        }//End for (int i = nIndex; i < m_nCount; i++)
    }//End While(true)

    return 0;
}

BOOL CEventServer::EventAcceptOfThread(int i)     //�����߳��ж�����
{
    if(m_dwCount == WSA_MAXIMUM_WAIT_EVENTS)
    {
        CMySocket::OutputErrorMsg(TEXT("CEventServer::EventAcceptOfThread: �ͻ������Ӵﵽ����"));
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

    //����ClientAddr
    m_addrClient[m_dwCount] = m_Sock.GetNewAddr();
    
    //�ٴν��²�����ͨ��socket���¼�����
    m_hSocketAry[m_dwCount] = m_Sock.GetNewSock();
    m_hEventAry[m_dwCount] = WSACreateEvent();
    
    int nRet =  WSAEventSelect(m_hSocketAry[m_dwCount],
                               m_hEventAry[m_dwCount],
                               FD_READ | FD_ACCEPT | FD_CLOSE);
    
    if (SOCKET_ERROR == nRet)
    {
        //�ͷ���Դ
        OutputDebugString("CEventServer::EventAcceptOfThread: ��ʧ�ܣ����Event��");
        WSACloseEvent(m_hEventAry[m_dwCount]);
        return 0;
    }    
    
    //��ʾ����IP��Ϣ
//     char szBufPort[8] = {0};
//     
//     wsprintf(szBufPort, "%d", m_nCount);
//     int nRow = m_ListCtl.InsertItem(m_nCount, szBufPort);
//     
//     wsprintf(szBufPort, "%d", m_addrClient[i].sin_port);
//     m_ListCtl.SetItemText(nRow, 1, inet_ntoa(m_addrClient[i].sin_addr));
//     m_ListCtl.SetItemText(nRow, 2, szBufPort);
    
 
    //Event����
    m_dwCount++; 
    
    return nRet;
}

BOOL CEventServer::EventReadOfThread(int i)
{
    BOOL bRet = FALSE;
    DWORD dwBufSize = 0;
    int nRecvBytes = 0;//ÿ�ν��յ�����

#ifndef __SERVER
    const DWORD DEAL_SIZE = TagDealSize;
#else
    const DWORD DEAL_SIZE = TagCSInfoSize;
#endif
    
    static BOOL bIsFirstInto = TRUE;

    //����ѽ������ݣ��Ƚṹ��С��
    if (m_SRInfo[i].m_dwRecvedSize < DEAL_SIZE)
    {
#ifndef __SERVER
        //�ͻ��˷��͵���Deal
        nRecvBytes = ::recv(m_hSocketAry[i], 
                          (char*)&m_SRInfo[i].m_Deal + m_SRInfo[i].m_dwRecvedSize,
                          DEAL_SIZE - m_SRInfo[i].m_dwRecvedSize,
                          0);
#else
        //Mid������CSinfo
        nRecvBytes = ::recv(m_hSocketAry[i], 
                          (char*)&m_SRInfo[i] + m_SRInfo[i].m_dwRecvedSize,
                          DEAL_SIZE - m_SRInfo[i].m_dwRecvedSize,
                          0);
#endif
        
        
        //�ۼ�m_dwRecvedSize  �ѽ�������
        m_SRInfo[i].m_dwRecvedSize += nRecvBytes;
        
        if(m_SRInfo[i].m_dwRecvedSize != DEAL_SIZE)
        {
            return FALSE;
        }
    }

    //���Σ���һ�ο�ʼ��������ʱ�����浱ǰSocket
    if(bIsFirstInto)
    {
        //����˱����м���Socket
#ifndef __SERVER
        //�м�㱣��Client��Socket
        m_SRInfo[i].m_Socket = m_hSocketAry[i];
#else
        m_SRInfo[i].m_MidSock = m_hSocketAry[i];
#endif
        
        bIsFirstInto = FALSE;
    }
    
    //˵��ͷ���Ѿ���ȡ���
    //��ʼ����ռ�
    if (m_SRInfo[i].m_Deal.m_pszData == NULL)
    {
        //����һ���㹻��Ļ�������Ŵ���������
        m_SRInfo[i].m_Deal.m_pszData = new char[m_SRInfo[i].m_Deal.m_dwDataSize + 1];
        if (m_SRInfo[i].m_Deal.m_pszData == NULL)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: ����BMP������ʧ��"));
            //�������ͷ���Դ
            return FALSE;
        }

        memset(m_SRInfo[i].m_Deal.m_pszData, 0, m_SRInfo[i].m_Deal.m_dwDataSize + 1);
    }
    
    LPTSTR pszBuf = m_SRInfo[i].m_Deal.m_pszData;

    //��ȡ��������
    int nRecvedBufBytes = m_SRInfo[i].m_dwRecvedSize - DEAL_SIZE;

    if (nRecvedBufBytes < (int)m_SRInfo[i].m_Deal.m_dwDataSize)
    {

        nRecvBytes = ::recv(m_hSocketAry[i], 
                          pszBuf + nRecvedBufBytes,
                          m_SRInfo[i].m_Deal.m_dwDataSize - nRecvedBufBytes,
                          0);


        if(nRecvBytes == SOCKET_ERROR)
        {
            CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: ���պ�������ʧ��"));
            return FALSE;
        }
        
        m_SRInfo[i].m_dwRecvedSize += nRecvBytes;

        //�����������û�н�����ɣ������
        if(m_SRInfo[i].m_dwRecvedSize - DEAL_SIZE != m_SRInfo[i].m_Deal.m_dwDataSize)
        {
            return FALSE;
        }
        
    }

    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventReadOfThread: �������ݳɹ�"));
    
    //���ν�����ɣ�����ס�̣߳��ȴ���ȡ����
    m_ThreadLock.Lock();

    //�ṩ���ݽӿ�
    m_pInfoRet = &m_SRInfo[i];
    
    //���ݽ�����֪ͨ�ⲿ���Բ�������
    m_DataLock.UnLock();
    
    
    //���سɹ����գ��ұ��ν��ս������ñ�־λΪ�棬�´ν�����Ϊ�½���
    bIsFirstInto = TRUE;
    return TRUE;
}

//�����߳��н�������
BOOL CEventServer::EventCloseOfThread(int nIndex)
{
    BOOL bRet = FALSE;
    //��������-����
    m_ThreadCS.Lock();
    
    //�ر�Socket
    if(!m_Sock.CloseSocket(m_hSocketAry[nIndex]))
    {
        //�ر�ʧ�ܣ��ύ������Ϣ
        CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: �ر��׽���ʧ��"));
        CMySocket::OutputErrorMsg(m_Sock.GetErrorMsg());
        return FALSE;
    }

    //Socket�Ѿ��رգ���ͻ�������-1
    m_dwCount--;

    //�ر�Event���
    ::CloseHandle(m_hEventAry[nIndex]);

    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: �ر��׽��ֳɹ�"));


    //�ƶ�����
    int i = nIndex;
    for(int j = i + 1; j < m_dwCount + 1; j++, i++)
    {
        
        if(m_hSocketAry[j] != INVALID_SOCKET)
        {
            //���������ǰ�ƶ�
            m_hSocketAry[i] = m_hSocketAry[j];
            m_hEventAry[i]  = m_hEventAry[j];
            m_addrClient[i] = m_addrClient[j];
        }
        else
        {
            //��һ�������ƶ�֮������,�ó�ʼ
            m_hSocketAry[j] = INVALID_SOCKET;
            m_hEventAry[j]  = INVALID_HANDLE_VALUE;
            memset(&m_addrClient[j], 0, sizeof(sockaddr_in));

            break;
        }
    }

    TCHAR szBuf[MAXBYTE] = {0};
    wsprintf(szBuf, TEXT("CEventServer::EventCloseOfThread: ��ʣ%d���ͻ���."), m_dwCount - 1);
    CMySocket::OutputErrorMsg(szBuf);
    CMySocket::OutputErrorMsg(TEXT("CEventServer::EventCloseOfThread: Socket�ر�"));


    //��������-����
    m_ThreadCS.Unlock();
    bRet = TRUE;

    return bRet;
}

//�Ƿ���Բ�������,��������ǰ�̣߳�
//��������ɻ�ȡ֮���߳�֪ͨ����
BOOL CEventServer::IsCanHandleData()
{
    //����HandleOver() �������Ѿ�������
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
        MessageBox(NULL, TEXT("CEventServer::HandleOver: �����IsCanHandleData()."), TEXT("Error!"), MB_OK);
        CMySocket::OutputErrorMsg(TEXT("CEventServer::HandleOver: IsCanHandleData() δ���ã����������"));
        return FALSE;
    }
    
    //��ʼ��CSInfo
    memset(&CSInfo, 0, TagCSInfoSize);

    //�ͷŻ�����
    if(CSInfo.m_Deal.m_pszData != NULL)
    {
        delete CSInfo.m_Deal.m_pszData;
        CSInfo.m_Deal.m_pszData = NULL;
    }
    
    //���β������������ñ�־λ�����´εļ��
    m_bDataIsLock = FALSE;

    //����������֪ͨ�߳̽�����֪ͨ�ⲿ����
    //����IsCanHandleData �Ѿ���Data����������Ҫ����
    return m_ThreadLock.UnLock();
}

//================================CEventServer================================


//=============================CMyEvent=======================================

CMyEvent::CMyEvent(BOOL bInitiallyOwn, 
                   BOOL bManualReset,
                   LPCTSTR lpszNAme, 
                   LPSECURITY_ATTRIBUTES lpsaAttribute)  throw(BOOL)
{
    m_hObject = ::CreateEvent(lpsaAttribute,    //��ȫ����
                              bManualReset,     //�Ƿ��Զ�������False��ʾΪ�Զ�
                              bInitiallyOwn,    //False��Ϊnonsignaled
                              lpszNAme);        //Event����
    if (m_hObject == NULL)
		throw(FALSE);

    //��ʼ��״̬
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

//����
BOOL CMyEvent::SetEvent()
{ 
    BOOL bRet = ::SetEvent(m_hObject);
    
    if(bRet)
    {
        //ִ�гɹ�����Ϊ����״̬
        m_bIsLock = FALSE;
    }
    
    return bRet; 
}

BOOL CMyEvent::PulseEvent()
{ 
    return ::PulseEvent(m_hObject); 
}

//����
BOOL CMyEvent::ResetEvent()
{ 
    BOOL bRet = ::ResetEvent(m_hObject);
    
    if(bRet)
    {
        //���óɹ�����Ϊ����״̬
        m_bIsLock = TRUE;
    }
    
    return bRet; 
}

//����
BOOL CMyEvent::Lock(DWORD dwTimeout)
{
    BOOL bRet = FALSE;

    if(!m_bIsAutoLock)
    {
        //����
        bRet = ResetEvent();
    }
    else
    {
        if (::WaitForSingleObject(m_hObject, dwTimeout) == WAIT_OBJECT_0)
            //���÷���ֵ����Ϊ�Զ�����ʱ����Ϊ����״̬
            m_bIsLock = bRet = TRUE;
        else
		    bRet = FALSE;
    }
    
    return bRet;
}

//����
BOOL CMyEvent::UnLock()
{
    BOOL bRet = FALSE;
    
    //����
    bRet = SetEvent();

    return bRet;
}

//���״̬
BOOL CMyEvent::IsLock() CONST
{
//     BOOL bRet = FALSE;
//     
//     bRet = WaitForSingleObject(m_hObject, 0);
// 
//     //û����
//     if(bRet == WAIT_OBJECT_0)
//     {
//         ResetEvent();
//         bRet = TRUE;
//     }
//     //����
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

//������
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
    OutputErrorMsg(TEXT("CMySocket::~CMySocket: �ѽ���Socket"));
}
BOOL CMySocket::InitSocketWSA()      //��ʼ���׽��ֻ���
{
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        m_wVersionRequested = MAKEWORD( 2, 2 );
        
        m_err = ::WSAStartup( m_wVersionRequested, &m_wsaData );
        if ( m_err != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            OutputErrorMsg(TEXT("CMySocket::InitSocketWSA: ��ʼ��Socket����ʧ�ܣ�"));
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
            OutputErrorMsg(TEXT("CMySocket::InitSocketWSA: ��ʼ��Socket����ʧ�ܣ�"));
            return FALSE; 
        }
        
        /* The WinSock DLL is acceptable. Proceed. */

        bIsFirstInto = FALSE;
    }
    return TRUE;
}

//�ṩ�ر�Socket����
BOOL CMySocket::SocketOver()
{
    OutputErrorMsg(TEXT("CMySocket::SocketOver: ����Socket"));
    return DestroySocketWSA();
}

BOOL CMySocket::DestroySocketWSA()      //�ر��׽��ֻ���
{
    static BOOL bIsFirstInto = TRUE;    //�Ƿ�Ϊ��һ�ν���
    BOOL bRet = TRUE;

    if(bIsFirstInto)
    {
        //�ر�Socket
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
        
        //������
        ::WSACleanup();
        
        OutputErrorMsg(TEXT("CMySocket::DestroySocketWSA: �ر�Socket�ɹ���"));

        bIsFirstInto = FALSE;
    }
    
    return TRUE;
}


LPCTSTR CMySocket::GetErrorMsg() 
{
    //ת��������
    LPCTSTR pStr = NULL;
    pStr = ConvertErrorMsg(m_nErrorCode);
    
    return pStr;
}

BOOL CMySocket::Create(UINT nSocketPort,
                       LPCTSTR lpszSocketAddress/* = NULL*/,
                       int nSocketType/* = SOCK_STREAM*/)
{
    //�����׽���
    if(!CreateSocket(nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::Create�������׽���ʧ��"));
        return FALSE;
    }

    //��ʼ����ַ
    if(lpszSocketAddress != NULL)
    {
        //�������ӵ�ַ����ΪClient
        m_bIsServer = FALSE;
        SetAddr(lpszSocketAddress, nSocketPort);
    }
    else
    {
        //û�и����ַ����ΪServer
        m_bIsServer = TRUE;
        SetAddr(INADDR_LOOPBACK, nSocketPort);
    }

    return TRUE;
}

BOOL CMySocket::CreateSocket(int type/* = SOCK_STREAM*/,
                             int af/* = AF_INET*/,
                             int protocol/* = 0*/)    //�����׽���
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

BOOL CMySocket::CloseSocket(SOCKET& sCur)     //�ر�ָ���׽���
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
    //��ʼ��Socket����
    if(!InitSocketWSA())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: ��ʼ��Socket�ɹ�"));
    }
    
    //�����׽���
    if(!Create(nSocketPort, lpszSocketAddress, nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: �����׽���ʧ��"));
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: �����ɹ�"));
    }

    //�󶨶˿�
    if(!Bind())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: �󶨳ɹ�"));
    }
    

    //����
    if(!Listen())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartServer: �����ɹ�"));
    }

    return TRUE;
}

BOOL CMySocket::StartClient(UINT nSocketPort, LPCTSTR lpszSocketAddress, INT nSocketType)
{
    //��ʼ��Socket����
    if(!InitSocketWSA())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: ��ʼ��Socket�ɹ�"));
    }
    
    //�����׽���
    if(!Create(nSocketPort, lpszSocketAddress, nSocketType))
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: �����׽���ʧ��"));
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: �����׽��ֳɹ�"));
    }

    //���ӷ�����
    if(!Connect())
    {
        OutputErrorMsg(GetErrorMsg());
        return FALSE;
    }
    else
    {
        OutputErrorMsg(TEXT("CMySocket::StartClient: ���ӳɹ�"));
    }

    //����

    //����

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
            OutputErrorMsg(TEXT("CMySocket::SendData:���Ȼ򻺳���Ϊ��"));
            __leave;
        }
        
        //ѭ��дSocket����
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
            OutputErrorMsg(TEXT("CMySocket::RecvData:���ȿ�"));
            __leave;
        }
//         if(!strBuf.empty())
//         {
//             strBuf = TEXT("");
//             OutputErrorMsg(TEXT("��ʼ��������"));
//         }
        
        //ѭ����Socket����
        while (dwSize > nRecvedTotalBytes)
        {
            nRecvedTmpBytes  = ::recv(sDstScok, 
                                      szBuf + nRecvedTotalBytes, 
                                      dwSize - nRecvedTotalBytes,
                                      0);
            
            if(nRecvedTmpBytes == SOCKET_ERROR)
            {
                //�������еĴ�����
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


//��ȡSocket
SOCKET CMySocket::GetMainSock() CONST
{
    return m_sMainSock;
}

SOCKET CMySocket::GetNewSock() CONST
{
    return m_sNewSock;
}

//��ȡAddr
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
        ::wsprintf(szBuf, TEXT("CMySocket::OutputErrorMsg: �޷�������������룺%d��"), m_nErrorCode);
        strError = szBuf;
    }
    
    ::OutputDebugString(strError);
}

LPCTSTR CMySocket::ConvertErrorMsg(int nErrorCode)
{
    switch(nErrorCode)
    {
    case WSA_INVALID_HANDLE:
        return TEXT("ָ�����¼�������Ч����ʹ����Win32������Ӧ��Winsock���������п��ܲ���������Win32�������������ݸ�WSAWaitForMultipleEvents��һ���������Ч��");
        break;
        
    case WSA_NOT_ENOUGH_MEMORY:
        return TEXT("�ڴ治�������Win32����ָ���ڴ��������㣬�޷����ָ���Ĳ���");
        break;
        
    case WSA_INVALID_PARAMETER:
        return TEXT("һ������������Ч�����Win32����������ݵ������ڲ��Ĳ�����Ч�������¼�����������Ч����ô��ִ��WSAWaitForMultipleEvents��ʱ��Ҳ�ᷢ�������Ĵ���");
        break;
        
    case WSA_WAIT_TIMEOUT:
        return TEXT("������ʱ�����Win32����ָ���ص�I/O����δ�ڹ涨��ʱ�������");
        break;
        
    case WSA_OPERATION_ABORTED:
        return TEXT("�ص�������ȡ�������Win32����ָ�������׽��ֵĹرգ����һ���ص�I/O������ȡ�����������⣬�ô���Ҳ������ִ��SIO_FLUSH���I/O��������ʱ����");
        break;
        
    case WSA_IO_INCOMPLETE:
        return TEXT("�ص�I/O�¼�����δ���ڴ���״̬�����Win32����Ҳ���ص�I/O����������أ��ڵ���WSAGetOverlappedResults������ʱ�������ָ���ص�I/O������δ���");
        break;
        
    case WSA_IO_PENDING:
        return TEXT("�ص����������Ժ���ɡ���Winsock��������һ���ص�I/O����ʱ��������������Win32���󣬱����������δ��ɣ����һ����Ժ��ĳ��ʱ�����");
        break;
        
    case WSAEINTR:
        return TEXT("���������жϡ��ô���������ڶ�WSACancelBlockingCall�ĵ��ã������һ�ε��ñ�ǿ���ж�");
        break;
        
    case WSAEBADF:
        return TEXT("�ļ�������󡣸ô�������ṩ���ļ������Ч����MicrosoftWindowsCE�£�socket�������ܷ���������󣬱��������ڴ��ڡ�æ��״̬");
        break;
        
    case WSAEACCES:
        return TEXT("Ȩ�ޱ��ܡ����Զ��׽��ֽ��в�����������ֹ������ͼ��sendto��WSASendTo��ʹ��һ���㲥��ַ��������δ��setsockopt��SO_BROADCAST������ѡ�����ù㲥Ȩ�ޣ��������������");
        break;
        
    case WSAEFAULT:
        return TEXT("��ַ��Ч������Winsock������ָ���ַ��Ч����ָ���Ļ�����̫С��Ҳ������������");
        break;
        
    case WSAEINVAL:
        return TEXT("������Ч��ָ����һ����Ч���������磬����ΪWSAIoctl����ָ����һ����Ч���ƴ��룬����������������⣬��Ҳ���ܱ����׽��ֵ�ǰ��״̬�д�������һ��û�м������׽����ϵ���accept��WSAAccept��ʱ��");
        break;
        
    case WSAEMFILE:
        return TEXT("���ļ����ࡣ��ʾ�򿪵��׽���̫���ˡ�ͨ����Microsoft�ṩ��ֻ�ܵ�ϵͳ�ڿ�����Դ����������");
        break;
        
    case WSAEWOULDBLOCK:
        return TEXT("��Դ��ʱ�����á��Է������׽�����˵��������������������ִ�еĻ���ͨ���᷵��������󡣱���˵����һ���������׽����ϵ���connect���ͻ᷵�����������Ϊ��������������ִ��");
        break;
        
    case WSAEINPROGRESS:
        return TEXT("�������ڽ����С���ǰ����ִ�з�����������һ����˵�������������󣬳������ڿ���16λWinsockӦ�ó���");
        break;
        
    case WSAEALREADY:
        return TEXT("��������ɡ�һ����˵���ڷ������׽����ϳ����Ѵ��ڽ����еĲ���ʱ�������������󡣱��磬��һ���Ѵ������ӽ��̵ķ������׽����ϣ���һ�ε���connect��WSAConnect�����⣬�����ṩ�ߴ���ִ�лص����������֧�ֻص����̵�Winsock�������Ľ�����ʱҲ������������");
        break;
        
    case WSAENOTSOCK:
        return TEXT("��Ч�׽����ϵ��׽��ֲ������κ�һ����SOCKET�������������Winsock�������᷵����������������ṩ���׽��־����Ч");
        break;
        
    case WSAEDESTADDRREQ:
        return TEXT("��ҪĿ���ַ������������û���ṩ�����ַ���ȷ�˵�������ڵ���sendtoʱ����Ŀ���ַ��ΪINADDR_ANY�������ַ������᷵���������");
        break;
        
    case WSAEMSGSIZE:
        return TEXT("��Ϣ�������������ĺ���ܶࡣ�����һ�����ݱ��׽����Ϸ���һ����Ϣ��������Ϣ���ڲ�����������̫��Ļ����ͻ������������ٱ��磬����������������ƣ�ʹһ����Ϣ������Ҳ�������������������յ����ݱ�֮�󣬻�����̫С�����ܽ�����Ϣʱ��Ҳ������������");
        break;
        
    case WSAEPROTOTYPE:
        return TEXT("�׽���Э������������socket��WSASocket������ָ����Э�鲻֧��ָ�����׽������͡����磬Ҫ����SOCK_STREAM���͵�һ��IP�׽��֣�ͬʱָ��Э��ΪIPPROTO_UDP�������������Ĵ���");
        break;
        
    case WSAENOPROTOOPT:
        return TEXT("Э��ѡ����󡣱�����getsockopt��setsockopt�����У�ָ�����׽���ѡ��򼶱�����δ��֧�ֻ�����Ч");
        break;
        
    case WSAEPROTONOSUPPORT:
        return TEXT("��֧�ֵ�Э�顣ϵͳ��û�а�װ�����Э���û����Ӧ��ʵʩ���������磬���ϵͳ��û�а�װTCP/IP�������Ž���TCP��UDP�׽���ʱ���ͻ�����������");
        break;
        
    case WSAESOCKTNOSUPPORT:
        return TEXT("��֧�ֵ��׽������͡���ָ���ĵ�ַ������˵��û����Ӧ�ľ����׽�������֧�֡����磬����һ����֧��ԭʼ�׽��ֵ�Э��������һ��SOCK_RAW�׽�������ʱ���ͻ�����������");
        break;
        
    case WSAEOPNOTSUPP:
        return TEXT("��֧�ֵĲ������������ָ���Ķ�����ͼ��ȡ�Ĳ���δ��֧�֡�ͨ�������������һ����֧�ֵ���Winsock�������׽����ϵ�����Winsockʱ���ͻ����������󡣱��磬��һ�����ݱ��׽����ϵ���accept��WSAAccept����ʱ���ͻ���������Ĵ���");
        break;
        
    case WSAEPFNOSUPPORT:
        return TEXT("��֧�ֵ�Э����塣�����Э����岻���ڣ���ϵͳ����δ��װ����������£�����������WSAEAFNOSUPPORT���������ߵȼۣ������߳��ֵø�ΪƵ��");
        break;
        
    case WSAEAFNOSUPPORT:
        return TEXT("��ַ���岻֧������Ĳ��������׽������Ͳ�֧�ֵĲ�����˵��������ִ����ʱ���ͻ����������󡣱��磬������ΪSOCK_STREAM��һ���׽����ϵ���sendto��WSASendTo����ʱ���ͻ��������������⣬�ڵ���socket��WSASocket������ʱ����ͬʱ������һ����Ч�ĵ�ַ���塢�׽������ͼ�Э����ϣ�Ҳ������������");
        break;
        
    case WSAEADDRINUSE:
        return TEXT("�µ�ַ����ʹ�á���������£�ÿ���׽���ֻ����ʹ��һ���׽��ֵ�ַ�����磬һ��IP�׽��ֵ�ַ�ɱ���IP��ַ���˿ں���ɣ����������һ���bind��connect��WSAConnect�����������йء�����setsockopt�����������׽���ѡ��SO_REUSEADDR���������׽��ַ���ͬһ������IP��ַ���˿ں�");
        break;
        
    case WSAEADDRNOTAVAIL:
        return TEXT("���ܷ�������ĵ�ַ��API������ָ���ĵ�ַ���Ǹ�������˵��Чʱ���ͻ���������Ĵ������磬����bind������ָ��һ��IP��ַ����ȴû�ж�Ӧ�ı���IP�ӿڣ������������Ĵ������⣬ͨ��connect��WSAConnect��sendto��WSASendTo��WSAJoinLeaf���ĸ�����Ϊ׼�����ӵ�Զ�̼����ָ���˿�0ʱ��Ҳ����������Ĵ���");
        break;
        
    case WSAENETDOWN:
        return TEXT("����Ͽ�����ͼ��ȡһ�����ʱ��ȴ�������������жϡ�����������������ջ�Ĵ�������ӿڵĹ��ϣ����߱��������������ɵ�");
        break;
        
    case WSAENETUNREACH:
        return TEXT("���粻�ɵִ��ͼ��ȡһ�����ʱ��ȴ����Ŀ�����粻�ɵִ���ɷ��ʣ�������ζ�ű���������֪����εִ�һ��Զ������������֮��Ŀǰû����֪��·�ɿɵִ��Ǹ�Ŀ������");
        break;
        
    case WSAENETRESET:
        return TEXT("��������ʱ�Ͽ������ӡ����ڡ����ֻ��������⵽һ����������������ӵ��жϡ�����һ���Ѿ���Ч������֮�ϣ�ͨ��setsockopt��������SO_KEEPALIVEѡ�Ҳ����������Ĵ���");
        break;
        
    case WSAECONNABORTED:
        return TEXT("����������ȡ������������������һ���Ѿ����������ӱ�ȡ������������£�����ζ������������Э���ʱ�������ȡ����");
        break;
        
    case WSAECONNRESET:
        return TEXT("���ӱ��Է����衣һ���Ѿ����������ӱ�Զ������ǿ�йرա���Զ�������ϵĽ����쳣��ֹ���У������ڴ��ͻ��Ӳ�����ϣ�����������׽���ִ����һ��ǿ�йرգ������������Ĵ������ǿ�йرյ����������SO_LINGER�׽���ѡ���setsockopt������һ���׽���");
        break;
        
    case WSAENOBUFS:
        return TEXT("û�л������ռ䡣����ϵͳȱ���㹻�Ļ������ռ䣬����Ĳ�������ִ��");
        break;
        
    case WSAEISCONN:
        return TEXT("�׽����Ѿ����ӡ�������һ���ѽ������ӵ��׽����ϣ���ͼ�ٽ���һ�����ӡ�Ҫע����ǣ����ݱ����������׽��־��п��ܳ��������Ĵ���ʹ�����ݱ��׽���ʱ������������ͨ��connect��WSAConnect���ã�Ϊ���ݱ�ͨ�Ź�����һ���˵�ĵ�ַ����ô�Ժ���ͼ�ٴε���sendto��WSASendTo�������������Ĵ���");
        break;
        
    case WSAENOTCONN:
        return TEXT("�׽�����δ���ӡ�����һ����δ�������ӵġ��������ӡ��׽����Ϸ��������շ����󣬱����������Ĵ���");
        break;
        
    case WSAESHUTDOWN:
        return TEXT("�׽��ֹرպ��ܷ��͡�������ͨ����shutdown��һ�ε��ã����ֹر����׽��֣����º�������������ݵ��շ�������Ҫע����ǣ����ִ���ֻ�����Ѿ��رյ��Ǹ��������������ϲŻᷢ�����ٸ�������˵��������ݷ��ͺ�������shutdown����ô�Ժ��κ����ݷ��͵��ö�����������Ĵ���");
        break;
        
    case WSAETIMEDOUT:
        return TEXT("���ӳ�ʱ����������һ���������󣬵������涨��ʱ�䣬Զ�̼������δ������ȷ����Ӧ�������û���κ���Ӧ������ᷢ�������Ĵ���Ҫ���յ������Ĵ���ͨ����Ҫ�����׽��������ú�SO_SNDTIMEO��SO_RCVTIMEOѡ�Ȼ�����connect��WSAConnect������Ҫ���˽����׽���������SO_SNDTIMEO��SO_RCVTIMEOѡ������飬�ɲο���9��");
        break;
        
    case WSAECONNREFUSED:
        return TEXT("���ӱ��ܡ����ڱ�Ŀ������ܾ��������޷���������ͨ����������Զ�̻����ϣ�û���κ�Ӧ�ó�������Ǹ���ַ֮�ϣ�Ϊ�����ṩ����");
        break;
        
    case WSAEHOSTDOWN:
        return TEXT("�����رա��������ָ������Ŀ�������رգ���ɲ���ʧ�ܡ�Ȼ����Ӧ�ó����ʱ���п����յ�����һ��WSAETIMEDOUT�����ӳ�ʱ��������Ϊ�Է��ػ������ͨ��������ͼ����һ�����ӵ�ʱ������");
        break;
        
    case WSAEHOSTUNREACH:
        return TEXT("û�е�������·�ɡ�Ӧ�ó�����ͼ����һ�����ɵִ���������ô���������WSAENETUNREACH");
        break;
        
    case WSAEPROCLIM:
        return TEXT("���̹��ࡣ��ЩWinsock�����ṩ�߶��ܹ�ͬʱ�������ǵĽ�����������������");
        break;
        
    case WSASYSNOTREADY:
        return TEXT("������ϵͳ�����á�����WSAStartupʱ�����ṩ�߲������������������ṩ����Ļ���ϵͳ�����ã�����᷵�����ִ���");
        break;
        
    case WSAVERNOTSUPPORTED:
        return TEXT("Winsock.dll�汾���󡣱�����֧�������Winsock�ṩ�߰汾");
        break;
        
    case WSANOTINITIALISED:
        return TEXT("Winsock��δ��ʼ������δ�ɹ���ɶ�WSAStartup��һ�ε���");
        break;
        
    case WSAEDISCON:
        return TEXT("���ڴ��ݹرա������������WSARecv��WSARecvFrom���صģ�ָ��Զ�������ѳ�ʼ����һ�δ��ݹرղ������ô���������ATM�����ġ�������Ϣ��Э���Ϸ�����");
        break;
        
    case WSAENOMORE:
        return TEXT("�Ҳ�������ļ�¼�����������WSALookupServiceNext�������أ�ָ���Ѿ�û�����¸���ļ�¼���������ͨ������WSA_E_NO_MORE����ʹ�á���Ӧ�ó����У�Ӧͬʱ�����������Լ�WSA_E_NO_MORE");
        break;
        
    case WSAECANCELLED:
        return TEXT("������ȡ�����������ָ����WSALookupServiceNext�������ڴ����ڼ䣬�����˶�WSALookupServiceEnd��������ֹ����һ�����á���ʱ��WSALookupServiceNext��᷵����������������������WSA_E_CANCELLED����ʹ�á���ΪӦ�ó���Ӧͬʱ�����������Լ�WSA_E_CANCELLED");
        break;

    case WSAEINVALIDPROVIDER:
        return TEXT("��Ч�ķ����ṩ�ߡ��������ͬ�����ṩ�߹�����һ�����ṩ�߲��ܽ�����ȷ��Winsock�汾���Ӷ��޷�����������ǰ���²���");
        break;
        
    case WSAEPROVIDERFAILEDINIT:
        return TEXT("�ṩ�߳�ʼ��ʧ�ܡ��������ͬ�����ṩ�߹�����һ��ͨ�������ṩ�߲���������Ҫ��DLLʱ");
        break;
        
    case WSASYSCALLFAILURE:
        return TEXT("ϵͳ����ʧ�ܡ��������Բ�Ӧʧ�ܵ�һ��ϵͳ����ȴ�����ź���ʧ����");
        break;
        
    case WSASERVICE_NOT_FOUND:
        return TEXT("�Ҳ��������ķ����������ͨ����ע������ֽ���������أ��ڲ�ѯ����ʱ��������10�¶���Щ�����������꾡���ͣ����ô���������ڸ��������ֿռ��ڣ��Ҳ�������ķ���");
        break;
        
    case WSATYPE_NOT_FOUND:
        return TEXT("�Ҳ���������͡��ô���Ҳ��ע�ἰ���ֽ�������������һ���ڴ�������ࣨService Class��ʱ��������ע���һ�������ʵ��������������һ����ǰͨ��WSAInstallServiceClass��װ�õķ���");
        break;
        
    case WSA_E_NO_MORE:
        return TEXT("�Ҳ�������ļ�¼�������������WSALookupServiceNext���÷��صģ�ָ���Ѿ�û��ʣ�µļ�¼���ô���ͨ������WSAENOMORE����ʹ�á���Ϊһ��Ӧ�ó���Ӧͬʱ�����������Լ�WSAENOMORE");
        break;
        
    case WSA_E_CANCELLED:
        return TEXT("������ȡ�����ô���ָ���ڶ�WSALookupServiceNext�ĵ�����δ��ɵ�ʱ���ַ����˶�WSALookupServiceEnd����ֹ���񣩵�һ�����á�������WSALookupServiceNext�ͻ᷵�ظô����������������WSAECANCELLED����ʹ�á���Ϊһ��Ӧ�ó���Ӧͬʱ�����������Լ�WSAECANCELLED");
        break;
        
    case WSAEREFUSED:
        return TEXT("��ѯ���ܡ����ڱ������ܾ�������һ�����ݿ��ѯ����ʧ��");
        break;
        
    case WSAHOST_NOT_FOUND:
        return TEXT("����û���ҵ�������������ڵ���gethostbyname��gethostbyaddrʱ�����ģ�����û���ҵ�һ����ȨӦ��������AuthoritativeAnswerHost��");
        break;
        
    case WSATRY_AGAIN:
        return TEXT("����Ȩ����û���ҵ����������Ҳ���ڵ���gethostbyname��gethostbyaddrʱ�����ģ�����û���ҵ�һ������Ȩ���������������˷���������");
        break;
        
    case WSANO_RECOVERY:
        return TEXT("����һ�����ɻָ��Ĵ����������Ҳ���ڵ���gethostbyname��gethostbyaddrʱ�����ģ�ָ������һ�����ɻָ��Ĵ���Ӧ�ٴγ��Բ���");
        break;
        
    case WSANO_DATA:
        return TEXT("û���ҵ��������͵����ݼ�¼���������Ҳ���ڵ���gethostbyname��gethostbyaddrʱ�����ģ�ָ�������ṩ��������Ч����ȴû���ҵ����������Ͷ�Ӧ�����ݼ�¼");
        break;
        
    case WSA_QOS_RECEIVERS:
        return TEXT("������һ��ԤԼ��Ϣ�ִ���ֵͬIP����������QoS���������еĹ�ϵ����ʵ������һ�������ġ����󡱣�QoS���������12�£�����ָ��������������һ������ϣ������QoSͨ��");
        break;
        
    case WSA_QOS_SENDERS:
        return TEXT("������һ��·����Ϣ�ִ���ֵͬQoS������һ����ʵ����һ��״̬������Ϣ����ָ���������ϣ�������һ������ϣ������QoS���ݵķ���");
        break;
        
    case WSA_QOS_NO_SENDERS:
        return TEXT("û��QoS�����ߡ����ֵͬQoS������һ��ָ���������κν��̶�QoS���ݵķ�������Ȥ������ĵ�12�£��˽��ڷ��������Ĵ���ʱ���������������һϵ������˵��");
        break;
        
    case WSA_QOS_NO_RECEIVERS:
        return TEXT("û��QoS�����ߡ����ֵͬQoS������һ��ָ���������κν��̶�QoS���ݵĽ�������Ȥ������ĵ�12�£����Ķ�������������˵��");
        break;
        
    case WSA_QOS_REQUEST_CONFIRMED:
        return TEXT("ԤԼ�����ѱ�ȷ�ϡ�QoSӦ�ÿ����ȷ�������ϣ������׼���Լ�����������ԤԼ������յ�֪ͨ��������������������һ����׼������յ���������Ϣ������ĵ�12�£��˽�Դ���Ϣ����ϸ˵��");
        break;
        
    case WSA_QOS_ADMISSION_FAILURE:
        return TEXT("ȱ����Դ�´���Դ�������������޷�����QoS��������");
        break;
        
    case WSA_QOS_POLICY_FAILURE:
        return TEXT("֤����Ч����������QoSԤԼ�����ʱ��Ҫô�û������߱���ȷ��Ȩ�ޣ�Ҫô�ṩ��֤����Ч");
        break;
        
    case WSA_QOS_BAD_STYLE:
        return TEXT("δ֪���ͻ����ʽ��QoSӦ�ó�������һ��ָ���ĻỰ��������ͬ�Ĺ�������ʽ����������һ���󣬱���ָ������ʽ����Ҫôδ֪��Ҫô���ڳ�ͻ������ĵ�12�£��˽�Թ�������ʽ����ϸ˵��");
        break;
        
    case WSA_QOS_BAD_OBJECT:
        return TEXT("��Ч��FILTERSPEC�ṹ�����ṩ�����ж��󡣼���ΪQoS�����ṩ��FILTERSPEC�ṹ��Ч�������ṩ�����еĻ�������Ч����᷵�������Ĵ��������12��");
        break;
        
    case WSA_QOS_TRAFFIC_CTRL_ERROR:
        return TEXT("FLOWSPEC�����⡣����ͨ�ſ����������ָ����FLOWSPEC�����������⣨��ΪQoS�����һ����Ա���ݣ�����᷵�������Ĵ���");
        break;
        
    case WSA_QOS_GENERIC_ERROR:
        return TEXT("����QoS��������һ���ȽϷ����Ĵ��󣻼�������QoS���󶼲��ʺϣ��㷵���������");
        break;
    }
    return NULL;
}
