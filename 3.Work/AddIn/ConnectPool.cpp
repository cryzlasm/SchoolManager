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
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::~CMyConnectPool: 析构，销毁线程池"));
    //销毁线程池
    Destroy();
}

LPCTSTR CMyConnectPool::GetErrStr() const
{
    return m_strErrStr.c_str();
}

BOOL CMyConnectPool::StartPool(LPCTSTR pszServerInfo, DWORD dwThreadCount)
{
    //设置SQL服务器信息
    m_strServerInfo = pszServerInfo;
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: 连接SQL..."));
    if(!CHandleSqlReply::InitSqlConnect(pszServerInfo))
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: 连接SQL失败"));
        return FALSE;
    }

    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::SetConnectInfo: 连接SQL成功"));

    //开启线程池
    return Create(dwThreadCount);
}

//创建一个线程池，（初始化）
BOOL CMyConnectPool::Create(INT nCount)
{
    BOOL bRet = TRUE;
    __try
    {
        if(nCount <= 0)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: 线程不能为0 或小于0"));
            bRet = FALSE;
        }
    
        //创建信号量,直接创建21E的信号
        m_hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
        if(m_hSemaphore == NULL)
        {
            TCHAR szBuf[MAXBYTE * 2] = {0};
            wsprintf(szBuf, TEXT("CMyConnectPool::Create: 创建信号量失败 : %s"), CMyConnectPool::GetErrorMsg());
            CHandleSqlReply::OutputErrorMsg(szBuf);
            bRet = FALSE;
        }

        m_hWaitHandle[0] = m_hSemaphore;

        //为线程数组开辟空间
        m_pThreadAry = new HANDLE[nCount];
        if(m_pThreadAry == NULL)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: 创建线程数组失败!"));
            bRet = FALSE;
        }

        //创建Event事件
        m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        //给句柄数组，赋值
        m_hWaitHandle[1] = m_hExitEvent;

        //创建线程
        for(int i = 0; i < nCount; i++)
        {
            //用于重试创建线程
            static INT nFailCount = 0;

            //创建线程
            CWinThread* pWinThread = AfxBeginThread((AFX_THREADPROC)ThreadFun, this);
            if(pWinThread == NULL || pWinThread == INVALID_HANDLE_VALUE)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: 创建线程失败"));
                nFailCount++;
                if(nFailCount < 5)
                {
                    TCHAR szBuf[MAXBYTE * 2] = {0};
                    wsprintf(szBuf, TEXT("CMyConnectPool::Create: 创建线程失败,重试中 %d 次, 错误信息 : %s"), nFailCount, GetErrorMsg());
                    CHandleSqlReply::OutputErrorMsg(szBuf);
                    continue;
                }
                else
                {
                    //关闭申请的线程
                    for(int j = 0; j < i; j++)
                    {
                        TerminateThread(m_pThreadAry[j], 0);
                        CloseHandle(m_pThreadAry[j]);
                    }
                    delete m_pThreadAry;
                    m_pThreadAry = NULL;
                    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: 创建线程失败，重试超次。"));
                    bRet = FALSE;
                }
            }

            //线程句柄数组，赋值
            m_pThreadAry[i] = pWinThread->m_hThread;
            m_dwThreadCount++;
            nFailCount = 0;
        }//End for(int i = 0; i < nCount; i++)
    }//End __Try
    __finally
    {
        if(!bRet)
        {
            m_strErrStr = TEXT("CMyConnectPool::Create: 线程池创建失败，请打开调试输出信息，进行查看");
        }
    }

    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Create: 线程池创建成功!"));
    return bRet;
}

//线程回调函数
/*static*/

DWORD AFX_CDECL CMyConnectPool::ThreadFun(LPVOID lParam)
{
    //获取This指针
    if(lParam == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: 线程中：This 指针 指向0地址"));
        return FALSE;
    }
    CMyConnectPool& This = *(CMyConnectPool*) lParam;
    

    //连接SQL服务器
    

    while(TRUE)
    {
        //使用同步对象，来检测m_cmdLst是否有消息，
        //如果没有消息，则会挂起该线程

        //对于信号灯而言，每次调用WaitForSingleObject，
        //会自动将hSemphore的资源-1
        DWORD dwRet = WaitForMultipleObjects(HANDLE_COUNT, This.m_hWaitHandle, FALSE, INFINITE);
        
        //等待失败
        if(dwRet == WAIT_FAILED)
        {
            //关闭申请的线程
            for(int i = 0; i < This.m_dwThreadCount; i++)
            {
                TerminateThread(This.m_pThreadAry[i], 0);
                CloseHandle(This.m_pThreadAry[i]);
            }
            delete This.m_pThreadAry;
            This.m_pThreadAry = NULL;
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: 创建线程失败，重试超次。"));
            
            return OBJ_FAILED;
        }

        //信号量有空闲
        if(dwRet - WAIT_OBJECT_0 == 0)
        {
            //保护连接对象
            This.m_CsLockCMDExec.Lock();
            BOOL bRet = This.m_Manager.Execute();
            if(!bRet)
            {
                CHandleSqlReply::OutputErrorMsg(TEXT("线程池：执行命令失败"));
            }
            This.m_CsLockCMDExec.Unlock();
        }

        //ExitEvent 事件
        if(dwRet - WAIT_OBJECT_0 == 1)
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::ThreadFun: 线程退出"));
            AfxEndThread(0);
        }
    }//End while(true)
    
    return 0;
}

//销毁线程池

BOOL CMyConnectPool::Destroy()
{
    if(m_bIsFirstIntoDestroy)
    {
        //TRACE(TEXT("CMyConnectPool::Destroy: 多次进入销毁线程池"));

        m_bIsFirstIntoDestroy = FALSE;

        if(m_pThreadAry == NULL)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: 线程句柄数组为空, 不需要销毁线程池"));
            return FALSE;
        }

        if(m_hExitEvent != INVALID_HANDLE_VALUE)
        {
            //让线程知道要退出，解锁Event事件
            SetEvent(m_hExitEvent);
        }
    

        //等待所有线程退出
        DWORD dwRet = WaitForMultipleObjects(m_dwThreadCount, m_pThreadAry, TRUE, INFINITE);
        if (dwRet > WAIT_ABANDONED_0 || dwRet == WAIT_TIMEOUT)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: 等待所有线程退出，超时或错误 : %s"), GetErrorMsg());
            return FALSE;
        }
        else if(dwRet == WAIT_FAILED)
        {
            TRACE(TEXT("CMyConnectPool::Destroy: 等待所有线程退出，WAIT_FAILED : %s"), GetErrorMsg());
            return FALSE;
        }

        m_bIsFirstIntoDestroy = FALSE;
    }//End if(!m_bIsFirstIntoDestroy)

    //Event手动申请，则需要关闭
    if(m_hExitEvent != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hExitEvent);
        m_hExitEvent = INVALID_HANDLE_VALUE;
    }
    
    //m_hSemaphore手动申请，则需要关闭
    if(m_hSemaphore != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hSemaphore);
        m_hSemaphore = INVALID_HANDLE_VALUE;
    }

    //所有线程退出，关闭线程句柄 AfxBeginThread 会自动关闭句柄
//     for(DWORD i = 0; i < m_dwThreadCount; i++)
//     {
//         if(m_pThreadAry[i] != INVALID_HANDLE_VALUE)
//             ::CloseHandle(m_pThreadAry[i]);
// 
//         m_pThreadAry[i] = INVALID_HANDLE_VALUE;
//     }

    //释放空间
    if(m_pThreadAry != NULL)
    {
        delete m_pThreadAry;
        m_pThreadAry = NULL;
    }
    

    //清空队列
    m_Manager.RemoveAll();
    
    CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Destroy: 销毁线程池成功"));

    CHandleSqlReply::CloseSQLConnect();
    return TRUE;
}
//处理命令

BOOL CMyConnectPool::Handle(ICommand* pCmd)
{
    if(pCmd == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::Handle: 传入指令为空"));
        return FALSE;
    }
    
    //往队列插入一个命令，如果插入成功，解锁一个信号灯
    if(m_Manager.Insert(pCmd))
    {
        ReleaseSemaphore(m_hSemaphore, 1, NULL);
    }

    return TRUE;
}


//获取错误信息
LPCTSTR CMyConnectPool::GetErrorMsg()      
{
    //格式化错误信息
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
    //结束，清空所有节点
    RemoveAll();
}

//插入一个命令

BOOL CConnectPoolManager::Insert(ICommand* Cmd)
{
    m_CsLock.Lock();
    
    m_CmdList.AddTail(Cmd);
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//删除所有命令节点

BOOL CConnectPoolManager::RemoveAll()
{
    if(m_CmdList.IsEmpty())
    {
        return true;
    }

    //遍历链表
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
    
    //释放所有节点
    m_CmdList.RemoveAll();
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//执行命令

BOOL CConnectPoolManager::Execute()
{
    ICommand * pCmd = NULL;
    
    BOOL bRet = TRUE;
    __try
    {
        //临界区锁，保护数据
        m_CsLock.Lock();
        
        if(m_CmdList.IsEmpty())
        {
            CHandleSqlReply::OutputErrorMsg(TEXT("CConnectPoolManager::Manager：队列为空"));
            bRet = TRUE;
            __leave;
        }
        
        //获取命令，且移除命令结点
        pCmd = m_CmdList.RemoveHead();
        
        //命令为空
        if(pCmd == NULL)
        {
            bRet = FALSE;
            __leave;
        }
        
        //执行命令
        pCmd->CmdExecute();
        
        //销毁命令
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
    //关闭连接对象
    try
    {
        m_Conn->Close();
    }
    catch (...)
    {
        return FALSE;
    }
    OutputErrorMsg(TEXT("关闭SQL 服务器成功!"));
    return TRUE;
}

//输出错误信息
void CHandleSqlReply::OutputErrorMsg(LPCTSTR strError)
{
    ::OutputDebugString(strError);
}


BOOL CHandleSqlReply::ConvertReplyStr(_RecordsetPtr& pRs, _OUT_ MyTagCSInfo& pCSInfo)
{
    tstring strReply = TEXT("");
    //显示字段
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
        //遍历结果
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
    

    //处理完毕转换字符串
    LPTSTR pszBuf = new TCHAR[strReply.length() + 1];
    if(pszBuf == NULL)
    {
        CHandleSqlReply::OutputErrorMsg(TEXT("CHandleSqlReply::ConvertReplyStr: 申请缓冲区失败！"));
        return FALSE;
    }

    memcpy(pszBuf, strReply.c_str(), strReply.length() + 1);
    
    pCSInfo.m_Deal.m_dwDataSize = strReply.length() + 1;
    pCSInfo.m_Deal.m_pszData = pszBuf;
    
    ConvertFlags(pCSInfo);
    return TRUE;
}

//转换标志位
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

//转换标志位
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


//用连接对象执行Sql语句
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

//初始化连接对象
BOOL CHandleSqlReply::InitSqlConnect(LPCTSTR strConnect)
{
    BOOL bRet = FALSE;
    try 
    {
        //要先初始化COM，CoInitialize(NULL)，否则会失败
        if(m_Conn.CreateInstance("ADODB.Connection") != S_OK)
        {
            OutputErrorMsg(TEXT("CHandleSqlReply::InitSqlConnect: 获取ADODB实例失败"));
            OutputErrorMsg(GetErrorMsg(GetLastError()));
            return FALSE;
        }
        
        //连接数据库服务器
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
                CHandleSqlReply::OutputErrorMsg(TEXT("CMyConnectPool::InitSqlConnect: 打开数据库失败，关闭连接对象"));
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

//获取错误信息
LPCTSTR CHandleSqlReply::GetErrorMsg(DWORD dwErrID)      
{
    //格式化错误信息
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