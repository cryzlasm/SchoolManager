// MyEvent.cpp: implementation of the CMyEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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