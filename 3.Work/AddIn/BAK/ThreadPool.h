// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
#define AFX_ICOMMAND_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
//����࣬��������һ���࣬������CmdExecute() ����
//***************************************************************************
#include "../AddIn/SocketDeal.h"
class ICommand  
{
public:

    //����CMD��������
    virtual BOOL SetCMD(MyTagCSInfo& pData) = 0;

    //ִ��Cmd
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
//ʹ�ô�ģ���࣬���봴��һ���࣬������CmdExecute() ����
//����ʹ�ö�̬���򴴽�һ������CmdExecute() �����������
//***************************************************************************
template <class T>
class CPoolManager  
{
public:
    CPoolManager();
    virtual ~CPoolManager();
    
    //����һ������
    BOOL Insert(T* Cmd);
    
    //ɾ����������ڵ�
    BOOL RemoveAll();
    
    //ִ������
    BOOL Execute();
    
protected:
    //�ٽ���
    CCriticalSection  m_CsLock; 
    
    //�������
    CList<T*, T*&> m_CmdList;
};
//===========================================================================

//��ʱֻ������
//1. ֪ͨ�߳��˳�����
//2. �ź�������֪ͨ
#define HANDLE_COUNT    2
#define OBJ_FAILED       -1

template <class T>
class CMyThreadPool  
{
public:

    //ʵ������Ĭ��Ϊ5���̵߳��̳߳أ�
    //�����û����봴���̳߳ص��߳�

	CMyThreadPool();

	virtual ~CMyThreadPool();

    //��������,������������м�������
    BOOL Handle(T* pCmd);

    //��ȡ������Ϣ
    static LPCTSTR GetErrorMsg(); 
    
    //�����̳߳�
    BOOL Destroy();

    //����һ���̳߳أ�����ʼ����Ĭ��Ϊ5���߳�
    BOOL Create(INT nCount = 5);
    
    LPCTSTR GetErrStr() const;

protected:

    //������Ϣ
    CString m_strErrStr;
    
    //�̻߳ص�����
    static DWORD AFX_CDECL ThreadFun(LPVOID lParam);
    
    //�Ƿ�Ϊ��һ�ν���Destory����
    BOOL        m_bIsFirstIntoDestroy;

    //�ź���
    HANDLE      m_hSemaphore;

    //�߳̾������
    HANDLE*     m_pThreadAry;
    DWORD       m_dwThreadCount;
    
    //�����˳���Event�¼�
    HANDLE      m_hExitEvent;
    
    //�ȴ��õľ������
    //�ȴ��˳��¼�m_hExitEvent
    //�ȴ��ź������߳��˳��¼�
    HANDLE      m_hWaitHandle[HANDLE_COUNT];

    //�������
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

//����һ���̳߳أ�����ʼ����
template <class T>
BOOL CMyThreadPool<T>::Create(INT nCount)
{
    BOOL bRet = TRUE;
    __try
    {
        if(nCount <= 0)
        {
            TRACE(TEXT("�̲߳���Ϊ0 ��С��0"));
            bRet = FALSE;
        }
    
        //�����ź���,ֱ�Ӵ���21E���ź�
        m_hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
        if(m_hSemaphore == NULL)
        {
            TRACE(TEXT("�����ź���ʧ�� : %s"), CMyThreadPool<T>::GetErrorMsg());
            bRet = FALSE;
        }

        m_hWaitHandle[0] = m_hSemaphore;

        //Ϊ�߳����鿪�ٿռ�
        m_pThreadAry = new HANDLE[nCount];
        if(m_pThreadAry == NULL)
        {
            TRACE(TEXT("�����߳�����ʧ��!"));
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
                TRACE("�����߳�ʧ��");
                nFailCount++;
                if(nFailCount < 5)
                {
                    TRACE("�����߳�ʧ��,������ %d ��, ������Ϣ : %s, ", nFailCount, GetErrorMsg());
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
                    TRACE("�����߳�ʧ�ܣ����Գ��Ρ�");
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
            m_strErrStr = TEXT("�̳߳ش���ʧ�ܣ���򿪵��������Ϣ�����в鿴");
        }
    }

    return bRet;
}

//�̻߳ص�����
/*static*/
template <class T>
DWORD AFX_CDECL CMyThreadPool<T>::ThreadFun(LPVOID lParam)
{
    //��ȡThisָ��
    if(lParam == NULL)
    {
        TRACE(TEXT("�߳��У�This ָ�� ָ��0��ַ"));
        return FALSE;
    }
    CMyThreadPool<T>& This = *(CMyThreadPool<T>*) lParam;
    
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
            TRACE("�����߳�ʧ�ܣ����Գ��Ρ�");
            
            return OBJ_FAILED;
        }

        //�ź����п���
        if(dwRet - WAIT_OBJECT_0 == 0)
        {
            BOOL bRet = This.m_Manager.Execute();
            if(!bRet)
            {
                TRACE(TEXT("�̳߳أ�ִ������ʧ��"));
            }
        }

        //ExitEvent �¼�
        if(dwRet - WAIT_OBJECT_0 == 1)
        {
            return 0;
        }
    }//End while(true)
    
    return 0;
}

//�����̳߳�
template <class T>
BOOL CMyThreadPool<T>::Destroy()
{
    if(!m_bIsFirstIntoDestroy)
    {
        TRACE(TEXT("��ν��������̳߳�"));
        return FALSE;
    }

    m_bIsFirstIntoDestroy = FALSE;

    if(m_pThreadAry == NULL)
    {
        TRACE(TEXT("�߳̾������Ϊ��, ����Ҫ�����̳߳�"));
        return FALSE;
    }

    if(m_hExitEvent != INVALID_HANDLE_VALUE)
    {
        //���߳�֪��Ҫ�˳�������Event�¼�
        SetEvent(m_hExitEvent);
    }
    

    //�ȴ������߳��˳�
    DWORD dwRet = WaitForMultipleObjects(m_dwThreadCount, m_pThreadAry, TRUE, INFINITE);
    if (dwRet != WAIT_FAILED && dwRet != WAIT_TIMEOUT)
    {
        TRACE(TEXT("�ȴ������߳��˳�����ʱ����� : %s"), GetErrorMsg());
        return FALSE;
    }

    //�����߳��˳����ر��߳̾��
    for(DWORD i = 0; i < m_dwThreadCount; i++)
    {
        CloseHandle(m_pThreadAry[i]);
    }

    //�ͷſռ�
    delete m_pThreadAry;
    m_pThreadAry = NULL;

    //��ն���
    m_Manager.RemoveAll();
    
    TRACE(TEXT("�����̳߳سɹ�"));

    return TRUE;
}
//��������
template <class T>
BOOL CMyThreadPool<T>::Handle(T* pCmd)
{
    if(pCmd == NULL)
    {
        TRACE(TEXT("Handle������ָ��Ϊ��"));
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
/* static */
template <class T>
LPCTSTR CMyThreadPool<T>::GetErrorMsg()      
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
template <class T>
CPoolManager<T>::CPoolManager()
{
    
}

template <class T>
CPoolManager<T>::~CPoolManager()
{
    //������������нڵ�
    RemoveAll();
}

//����һ������
template <class T>
BOOL CPoolManager<T>::Insert(T* Cmd)
{
    m_CsLock.Lock();
    
    m_CmdList.AddTail(Cmd);
    
    m_CsLock.Unlock();
    
    return TRUE;
}

//ɾ����������ڵ�
template <class T>
BOOL CPoolManager<T>::RemoveAll()
{
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
template <class T>
BOOL CPoolManager<T>::Execute()
{
    ICommand * pCmd = NULL;
    
    BOOL bRet = TRUE;
    __try
    {
        //�ٽ���������������
        m_CsLock.Lock();
        
        if(m_CmdList.IsEmpty())
        {
            TRACE(TEXT("Manager������Ϊ��"));
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

//End CPP
//============================================================================


#endif // !defined(AFX_THREADPOOL_H__B5E2F512_83E1_498F_B8EE_028916495C03__INCLUDED_)
