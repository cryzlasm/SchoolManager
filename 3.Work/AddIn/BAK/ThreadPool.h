// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
//虚基类，必须派生一个类，类中有CmdExecute() 方法
//***************************************************************************
#include "../AddIn/SocketDeal.h"
class ICommand  
{
public:

    //设置CMD命令内容
    virtual BOOL SetCMD(MyTagCSInfo& pData) = 0;

    //执行Cmd
    virtual BOOL CmdExecute() = 0;
};

#endif
//===========================================================================


#if !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <AFXTEMPL.H>
#include <AFXMT.H>

//===========================================================================
//***************************************************************************
//使用此模版类，必须创建一个类，类中有CmdExecute() 方法
//如需使用多态，则创建一个带有CmdExecute() 方法的虚基类
//***************************************************************************
template <class T>
class CPoolManager  
{
public:
    CPoolManager();
    virtual ~CPoolManager();
    
    //插入一个命令
    BOOL Insert(T* Cmd);
    
    //删除所有命令节点
    BOOL RemoveAll();
    
    //执行命令
    BOOL Execute();
    
protected:
    //临界区
    CCriticalSection  m_CsLock; 
    
    //命令队列
    CList<T*, T*&> m_CmdList;
};
//===========================================================================

//此时只有两个
//1. 通知线程退出的锁
//2. 信号量来的通知
#define HANDLE_COUNT    2
#define OBJ_FAILED       -1

template <class T>
class CMyThreadPool  
{
public:

    //实例对象，默认为5个线程的线程池，
    //可随用户输入创建线程池的线程

	CMyThreadPool();

	virtual ~CMyThreadPool();

    //处理命令,用于在命令队列加入命令
    BOOL Handle(T* pCmd);

    //获取错误信息
    static LPCTSTR GetErrorMsg(); 
    
    //销毁线程池
    BOOL Destroy();

    //创建一个线程池，（初始化）默认为5个线程
    BOOL Create(INT nCount = 5);
    
    LPCTSTR GetErrStr() const;

protected:

    //错误信息
    CString m_strErrStr;
    
    //线程回调函数
    static DWORD AFX_CDECL ThreadFun(LPVOID lParam);
    
    //是否为第一次进入Destory函数
    BOOL        m_bIsFirstIntoDestroy;

    //信号量
    HANDLE      m_hSemaphore;

    //线程句柄数组
    HANDLE*     m_pThreadAry;
    DWORD       m_dwThreadCount;
    
    //用于退出的Event事件
    HANDLE      m_hExitEvent;
    
    //等待用的句柄数组
    //等待退出事件m_hExitEvent
    //等待信号量，线程退出事件
    HANDLE      m_hWaitHandle[HANDLE_COUNT];

    //命令管理
    CPoolManager<T>    m_Manager;
};


//============================================================================
// CPP Begin
template <class T>
CMyThreadPool<T>::CMyThreadPool()
{
    m_hSemaphore = INVALID_HANDLE_VALUE;
    
    m_pThreadAry = NULL;
    m_dwThreadCount = 0;
    
    m_hExitEvent = INVALID_HANDLE_VALUE;
    
    memset(m_hWaitHandle, 0, sizeof(m_hWaitHandle));

    m_bIsFirstIntoDestroy = TRUE;
}

template <class T>
CMyThreadPool<T>::~CMyThreadPool()
{
    Destroy();
}
template <class T>
LPCTSTR CMyThreadPool<T>::GetErrStr() const
{
    return m_strErrStr;
}

//创建一个线程池，（初始化）
template <class T>
BOOL CMyThreadPool<T>::Create(INT nCount)
{
    BOOL bRet = TRUE;
    __try
    {
        if(nCount <= 0)
        {
            TRACE(TEXT("线程不能为0 或小于0"));
            bRet = FALSE;
        }
    
        //创建信号量,直接创建21E的信号
        m_hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
        if(m_hSemaphore == NULL)
        {
            TRACE(TEXT("创建信号量失败 : %s"), CMyThreadPool<T>::GetErrorMsg());
            bRet = FALSE;
        }

        m_hWaitHandle[0] = m_hSemaphore;

        //为线程数组开辟空间
        m_pThreadAry = new HANDLE[nCount];
        if(m_pThreadAry == NULL)
        {
            TRACE(TEXT("创建线程数组失败!"));
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
                TRACE("创建线程失败");
                nFailCount++;
                if(nFailCount < 5)
                {
                    TRACE("创建线程失败,重试中 %d 次, 错误信息 : %s, ", nFailCount, GetErrorMsg());
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
                    TRACE("创建线程失败，重试超次。");
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
            m_strErrStr = TEXT("线程池创建失败，请打开调试输出信息，进行查看");
        }
    }

    return bRet;
}

//线程回调函数
/*static*/
template <class T>
DWORD AFX_CDECL CMyThreadPool<T>::ThreadFun(LPVOID lParam)
{
    //获取This指针
    if(lParam == NULL)
    {
        TRACE(TEXT("线程中：This 指针 指向0地址"));
        return FALSE;
    }
    CMyThreadPool<T>& This = *(CMyThreadPool<T>*) lParam;
    
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
            TRACE("创建线程失败，重试超次。");
            
            return OBJ_FAILED;
        }

        //信号量有空闲
        if(dwRet - WAIT_OBJECT_0 == 0)
        {
            BOOL bRet = This.m_Manager.Execute();
            if(!bRet)
            {
                TRACE(TEXT("线程池：执行命令失败"));
            }
        }

        //ExitEvent 事件
        if(dwRet - WAIT_OBJECT_0 == 1)
        {
            return 0;
        }
    }//End while(true)
    
    return 0;
}

//销毁线程池
template <class T>
BOOL CMyThreadPool<T>::Destroy()
{
    if(!m_bIsFirstIntoDestroy)
    {
        TRACE(TEXT("多次进入销毁线程池"));
        return FALSE;
    }

    m_bIsFirstIntoDestroy = FALSE;

    if(m_pThreadAry == NULL)
    {
        TRACE(TEXT("线程句柄数组为空, 不需要销毁线程池"));
        return FALSE;
    }

    if(m_hExitEvent != INVALID_HANDLE_VALUE)
    {
        //让线程知道要退出，解锁Event事件
        SetEvent(m_hExitEvent);
    }
    

    //等待所有线程退出
    DWORD dwRet = WaitForMultipleObjects(m_dwThreadCount, m_pThreadAry, TRUE, INFINITE);
    if (dwRet != WAIT_FAILED && dwRet != WAIT_TIMEOUT)
    {
        TRACE(TEXT("等待所有线程退出，超时或错误 : %s"), GetErrorMsg());
        return FALSE;
    }

    //所有线程退出，关闭线程句柄
    for(DWORD i = 0; i < m_dwThreadCount; i++)
    {
        CloseHandle(m_pThreadAry[i]);
    }

    //释放空间
    delete m_pThreadAry;
    m_pThreadAry = NULL;

    //清空队列
    m_Manager.RemoveAll();
    
    TRACE(TEXT("销毁线程池成功"));

    return TRUE;
}
//处理命令
template <class T>
BOOL CMyThreadPool<T>::Handle(T* pCmd)
{
    if(pCmd == NULL)
    {
        TRACE(TEXT("Handle：传入指令为空"));
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
/* static */
template <class T>
LPCTSTR CMyThreadPool<T>::GetErrorMsg()      
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
template <class T>
CPoolManager<T>::CPoolManager()
{
    
}

template <class T>
CPoolManager<T>::~CPoolManager()
{
    //结束，清空所有节点
    RemoveAll();
}

//插入一个命令
template <class T>
BOOL CPoolManager<T>::Insert(T* Cmd)
{
    m_CsLock.Lock();
    
    m_CmdList.AddTail(Cmd);
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//删除所有命令节点
template <class T>
BOOL CPoolManager<T>::RemoveAll()
{
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
template <class T>
BOOL CPoolManager<T>::Execute()
{
    ICommand * pCmd = NULL;
    
    BOOL bRet = TRUE;
    __try
    {
        //临界区锁，保护数据
        m_CsLock.Lock();
        
        if(m_CmdList.IsEmpty())
        {
            TRACE(TEXT("Manager：队列为空"));
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

//End CPP
//============================================================================


#endif // !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
