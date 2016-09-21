// ConnectPool.cpp: implementation of the CLoginCmd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnectPool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//============================================================================
// CPP Begin
CMyConnectPool::CMyConnectPool()
{
    m_hSemaphore = INVALID_HANDLE_VALUE;
    
    m_pThreadAry = NULL;
    m_dwThreadCount = 0;

    m_hExitEvent = INVALID_HANDLE_VALUE;
    
    memset(m_hWaitHandle, 0, sizeof(m_hWaitHandle));

    m_bIsFirstIntoDestroy = TRUE;

    m_CsLockCMDExec.Unlock();
}


CMyConnectPool::~CMyConnectPool()
{
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::~CMyConnectPool: �����������̳߳�"));
    //�����̳߳�
    Destroy();
}

LPCTSTR CMyConnectPool::GetErrStr() const
{
    return m_strErrStr.c_str();
}

BOOL CMyConnectPool::StartPool(LPCTSTR pszServerInfo, DWORD dwThreadCount)
{
    //����SQL��������Ϣ
    m_strServerInfo = pszServerInfo;
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: ����SQL..."));
    if(!CHandleSqlReply::InitSqlConnect(pszServerInfo))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: ����SQLʧ��"));
        return FALSE;
    }

    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: ����SQL�ɹ�"));

    //�����̳߳�
    return Create(dwThreadCount);
}

//����һ���̳߳أ�����ʼ����
BOOL CMyConnectPool::Create(INT nCount)
{
    BOOL bRet = TRUE;
    __try
    {
        if(nCount <= 0)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: �̲߳���Ϊ0 ��С��0"));
            bRet = FALSE;
        }
    
        //�����ź���,ֱ�Ӵ���21E���ź�
        m_hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
        if(m_hSemaphore == NULL)
        {
            TCHAR szBuf[MAXBYTE * 2] = {0};
            wsprintf(szBuf, TEXT("CMyConnectPool::Create: �����ź���ʧ�� : %s"), CMyConnectPool::GetErrorMsg());
            CHandleSqlReply::OutputErrorMsg(szBuf);
            bRet = FALSE;
        }

        m_hWaitHandle[0] = m_hSemaphore;

        //Ϊ�߳����鿪�ٿռ�
        m_pThreadAry = new HANDLE[nCount];
        if(m_pThreadAry == NULL)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: �����߳�����ʧ��!"));
            bRet = FALSE;
        }

        //����Event�¼�
        m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        //��������飬��ֵ
        m_hWaitHandle[1] = m_hExitEvent;

        //�����߳�
        for(int i = 0; i < nCount; i++)
        {
            //�������Դ����߳�
            static INT nFailCount = 0;

            //�����߳�
            CWinThread* pWinThread = AfxBeginThread((AFX_THREADPROC)ThreadFun, this);
            if(pWinThread == NULL || pWinThread == INVALID_HANDLE_VALUE)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: �����߳�ʧ��"));
                nFailCount++;
                if(nFailCount < 5)
                {
                    TCHAR szBuf[MAXBYTE * 2] = {0};
                    wsprintf(szBuf, TEXT("CMyConnectPool::Create: �����߳�ʧ��,������ %d ��, ������Ϣ : %s"), nFailCount, GetErrorMsg());
                    CHandleSqlReply::OutputErrorMsg(szBuf);
                    continue;
                }
                else
                {
                    //�ر�������߳�
                    for(int j = 0; j < i; j++)
                    {
                        TerminateThread(m_pThreadAry[j], 0);
                        CloseHandle(m_pThreadAry[j]);
                    }
                    delete m_pThreadAry;
                    m_pThreadAry = NULL;
                    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: �����߳�ʧ�ܣ����Գ��Ρ�"));
                    bRet = FALSE;
                }
            }

            //�߳̾�����飬��ֵ
            m_pThreadAry[i] = pWinThread->m_hThread;
            m_dwThreadCount++;
            nFailCount = 0;
        }//End for(int i = 0; i < nCount; i++)
    }//End __Try
    __finally
    {
        if(!bRet)
        {
            m_strErrStr = TEXT("CMyConnectPool::Create: �̳߳ش���ʧ�ܣ���򿪵��������Ϣ�����в鿴");
        }
    }

    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: �̳߳ش����ɹ�!"));
    return bRet;
}

//�̻߳ص�����
/*static*/

DWORD AFX_CDECL CMyConnectPool::ThreadFun(LPVOID lParam)
{
    //��ȡThisָ��
    if(lParam == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: �߳��У�This ָ�� ָ��0��ַ"));
        return FALSE;
    }
    CMyConnectPool& This = *(CMyConnectPool*) lParam;
    

    //����SQL������
    

    while(TRUE)
    {
        //ʹ��ͬ�����������m_cmdLst�Ƿ�����Ϣ��
        //���û����Ϣ����������߳�

        //�����źŵƶ��ԣ�ÿ�ε���WaitForSingleObject��
        //���Զ���hSemphore����Դ-1
        DWORD dwRet = WaitForMultipleObjects(HANDLE_COUNT, This.m_hWaitHandle, FALSE, INFINITE);
        
        //�ȴ�ʧ��
        if(dwRet == WAIT_FAILED)
        {
            //�ر�������߳�
            for(int i = 0; i < This.m_dwThreadCount; i++)
            {
                TerminateThread(This.m_pThreadAry[i], 0);
                CloseHandle(This.m_pThreadAry[i]);
            }
            delete This.m_pThreadAry;
            This.m_pThreadAry = NULL;
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: �����߳�ʧ�ܣ����Գ��Ρ�"));
            
            return OBJ_FAILED;
        }

        //�ź����п���
        if(dwRet - WAIT_OBJECT_0 == 0)
        {
            //�������Ӷ���
            This.m_CsLockCMDExec.Lock();
            BOOL bRet = This.m_Manager.Execute();
            if(!bRet)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("�̳߳أ�ִ������ʧ��"));
            }
            This.m_CsLockCMDExec.Unlock();
        }

        //ExitEvent �¼�
        if(dwRet - WAIT_OBJECT_0 == 1)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: �߳��˳�"));
            AfxEndThread(0);
        }
    }//End while(true)
    
    return 0;
}

//�����̳߳�

BOOL CMyConnectPool::Destroy()
{
    if(m_bIsFirstIntoDestroy)
    {
        //TRACE(TEXT("CMyConnectPool::Destroy: ��ν��������̳߳�"));

        m_bIsFirstIntoDestroy = FALSE;

        if(m_pThreadAry == NULL)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: �߳̾������Ϊ��, ����Ҫ�����̳߳�"));
            return FALSE;
        }

        if(m_hExitEvent != INVALID_HANDLE_VALUE)
        {
            //���߳�֪��Ҫ�˳�������Event�¼�
            SetEvent(m_hExitEvent);
        }
    

        //�ȴ������߳��˳�
        DWORD dwRet = WaitForMultipleObjects(m_dwThreadCount, m_pThreadAry, TRUE, INFINITE);
        if (dwRet > WAIT_ABANDONED_0 || dwRet == WAIT_TIMEOUT)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: �ȴ������߳��˳�����ʱ����� : %s"), GetErrorMsg());
            return FALSE;
        }
        else if(dwRet == WAIT_FAILED)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: �ȴ������߳��˳���WAIT_FAILED : %s"), GetErrorMsg());
            return FALSE;
        }

        m_bIsFirstIntoDestroy = FALSE;
    }//End if(!m_bIsFirstIntoDestroy)

    //Event�ֶ����룬����Ҫ�ر�
    if(m_hExitEvent != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hExitEvent);
        m_hExitEvent = INVALID_HANDLE_VALUE;
    }
    
    //m_hSemaphore�ֶ����룬����Ҫ�ر�
    if(m_hSemaphore != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hSemaphore);
        m_hSemaphore = INVALID_HANDLE_VALUE;
    }

    //�����߳��˳����ر��߳̾�� AfxBeginThread ���Զ��رվ��
//     for(DWORD i = 0; i < m_dwThreadCount; i++)
//     {
//         if(m_pThreadAry[i] != INVALID_HANDLE_VALUE)
//             ::CloseHandle(m_pThreadAry[i]);
// 
//         m_pThreadAry[i] = INVALID_HANDLE_VALUE;
//     }

    //�ͷſռ�
    if(m_pThreadAry != NULL)
    {
        delete m_pThreadAry;
        m_pThreadAry = NULL;
    }
    

    //��ն���
    m_Manager.RemoveAll();
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Destroy: �����̳߳سɹ�"));

    CHandleSqlReply::CloseSQLConnect();
    return TRUE;
}
//��������

BOOL CMyConnectPool::Handle(ICommand* pCmd)
{
    if(pCmd == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Handle: ����ָ��Ϊ��"));
        return FALSE;
    }
    
    //�����в���һ������������ɹ�������һ���źŵ�
    if(m_Manager.Insert(pCmd))
    {
        ReleaseSemaphore(m_hSemaphore, 1, NULL);
    }

    return TRUE;
}


//��ȡ������Ϣ
LPCTSTR CMyConnectPool::GetErrorMsg()      
{
    //��ʽ��������Ϣ
    LPTSTR lpMsgBuf = NULL;
    ::FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL);
    
    return lpMsgBuf;
}

//=========================================================================

CConnectPoolManager::CConnectPoolManager()
{
    
}


CConnectPoolManager::~CConnectPoolManager()
{
    //������������нڵ�
    RemoveAll();
}

//����һ������

BOOL CConnectPoolManager::Insert(ICommand* Cmd)
{
    m_CsLock.Lock();
    
    m_CmdList.AddTail(Cmd);
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//ɾ����������ڵ�

BOOL CConnectPoolManager::RemoveAll()
{
    if(m_CmdList.IsEmpty())
    {
        return true;
    }

    //��������
    ICommand* pCmd = NULL;
    
    m_CsLock.Lock();
    
    POSITION pos = m_CmdList.GetHeadPosition();
    while(pos)
    {
        pCmd = m_CmdList.GetNext(pos);
        
        if(pCmd != NULL)
        {
            delete pCmd;
            pCmd = NULL;
        }
    }
    
    //�ͷ����нڵ�
    m_CmdList.RemoveAll();
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//ִ������

BOOL CConnectPoolManager::Execute()
{
    ICommand * pCmd = NULL;
    
    BOOL bRet = TRUE;
    __try
    {
        //�ٽ���������������
        m_CsLock.Lock();
        
        if(m_CmdList.IsEmpty())
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CConnectPoolManager::Manager������Ϊ��"));
            bRet = TRUE;
            __leave;
        }
        
        //��ȡ������Ƴ�������
        pCmd = m_CmdList.RemoveHead();
        
        //����Ϊ��
        if(pCmd == NULL)
        {
            bRet = FALSE;
            __leave;
        }
        
        //ִ������
        pCmd->CmdExecute();
        
        //��������
        delete pCmd;
    }   
    __finally
    {
        m_CsLock.Unlock();
    }
    return bRet;
}
//============================================================================
CMySocket CHandleSqlReply::m_Sock;
_ConnectionPtr CHandleSqlReply::m_Conn;

CHandleSqlReply::CHandleSqlReply()
{
    CoInitialize(NULL);
}
CHandleSqlReply::~CHandleSqlReply()
{
    CoUninitialize();
}

BOOL CHandleSqlReply::CloseSQLConnect()
{
    //�ر����Ӷ���
    try
    {
        m_Conn->Close();
    }
    catch (...)
    {
        return FALSE;
    }
    OutputErrorMsg(TEXT("�ر�SQL �������ɹ�!"));
    return TRUE;
}

//���������Ϣ
void CHandleSqlReply::OutputErrorMsg(LPCTSTR strError)
{
    ::OutputDebugString(strError);
}


BOOL CHandleSqlReply::ConvertReplyStr(_RecordsetPtr& pRs, _OUT_ MyTagCSInfo& pCSInfo)
{
    tstring strReply = TEXT("");
    //��ʾ�ֶ�
    int nCount = pRs->Fields->GetCount();
    for (long i = 0; i < nCount; i++)
    {

        strReply = strReply
                    + (char*)pRs->Fields->Item[i]->Name
                    + TEXT("|");
    }
    strReply += TEXT("\r\n");
    try
    {
        //�������
        while (!pRs->adoEOF)
        {
            for (long i = 0; i < nCount; i++)
            {
                strReply = strReply
                    + (char*)(_bstr_t)pRs->Fields->Item[i]->Value 
                    + TEXT("|");
            }
            strReply += TEXT("\r\n");
            pRs->MoveNext();
        } 
    }
    catch (...)
    {
        
    }
    

    //�������ת���ַ���
    LPTSTR pszBuf = new TCHAR[strReply.length() + 1];
    if(pszBuf == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CHandleSqlReply::ConvertReplyStr: ���뻺����ʧ�ܣ�"));
        return FALSE;
    }

    memcpy(pszBuf, strReply.c_str(), strReply.length() + 1);
    
    pCSInfo.m_Deal.m_dwDataSize = strReply.length() + 1;
    pCSInfo.m_Deal.m_pszData = pszBuf;
    
    ConvertFlags(pCSInfo);
    return TRUE;
}

//ת����־λ
BOOL CHandleSqlReply::ConvertErrorFlags( _OUT_ MyTagCSInfo& pCSInfo)
{
    PACKET_FLAG& Flags = pCSInfo.m_Deal.m_flag;
    switch(Flags)
    {
    case CLIENT_MSG_ALTER:
        Flags = SERVER_ERROR_ALTER;
        break;
        
    case CLIENT_MSG_DEL:
        Flags = SERVER_ERROR_DEL;
        break;
        
    case CLIENT_MSG_INSERT:
        Flags = SERVER_ERROR_INSERT;
        break;
        
    default: 
        Flags = NONE;
        break;
    }
    
    return TRUE;
}

//ת����־λ
BOOL CHandleSqlReply::ConvertFlags( _OUT_ MyTagCSInfo& pCSInfo)
{
    PACKET_FLAG& Flags = pCSInfo.m_Deal.m_flag;
    switch(Flags)
    {
    case CLIENT_MSG_ALTER:
        Flags = SERVER_REPLY_ALTER;
        break;

    case CLIENT_MSG_DEL:
        Flags = SERVER_REPLY_DEL;
        break;

    case CLIENT_MSG_INSERT:
        Flags = SERVER_REPLY_INSERT;
        break;

    case CLIENT_MSG_QUERY:
        Flags = SERVER_REPLY_QUERY;
        break;

    default: 
        Flags = NONE;
        break;
    }

    return TRUE;
}


//�����Ӷ���ִ��Sql���
BOOL CHandleSqlReply::ExecSql(LPCTSTR pszSqlMsg, _RecordsetPtr& Rs, tstring& strErrMsg)
{
    BOOL bRet = FALSE;
    
    try
    {
        Rs = m_Conn->Execute(pszSqlMsg, NULL, adCmdText);
        bRet = TRUE;
    }
    catch (_com_error e)
    {
        strErrMsg = e.ErrorMessage();
        bRet = FALSE;
    }
    return bRet;
}

//��ʼ�����Ӷ���
BOOL CHandleSqlReply::InitSqlConnect(LPCTSTR strConnect)
{
    BOOL bRet = FALSE;
    try 
    {
        //Ҫ�ȳ�ʼ��COM��CoInitialize(NULL)�������ʧ��
        if(m_Conn.CreateInstance("ADODB.Connection") != S_OK)
        {
            OutputErrorMsg(TEXT("CHandleSqlReply::InitSqlConnect: ��ȡADODBʵ��ʧ��"));
            OutputErrorMsg(GetErrorMsg(GetLastError()));
            return FALSE;
        }
        
        //�������ݿ������
        if(m_Conn->Open(strConnect, "", "", adConnectUnspecified) == S_OK)
        {
            bRet = TRUE;
        }
    }
    catch (_com_error e) 
    {
        CHandleSqlReply::OutputErrorMsg(e.ErrorMessage());
        try  
        {
            if(m_Conn->Close() == S_OK)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::InitSqlConnect: �����ݿ�ʧ�ܣ��ر����Ӷ���"));
                bRet = FALSE;
            }
        } 
        catch (...) 
        {
            bRet = FALSE;
        }
    }
    
    
    return bRet;
}

//��ȡ������Ϣ
LPCTSTR CHandleSqlReply::GetErrorMsg(DWORD dwErrID)      
{
    //��ʽ��������Ϣ
    LPTSTR lpMsgBuf = NULL;
    ::FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwErrID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL);
    
    return lpMsgBuf;
}

//End CPP
//============================================================================