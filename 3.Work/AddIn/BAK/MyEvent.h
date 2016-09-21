// MyEvent.h: interface for the CMyEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYEVENT_H__6E0707D4_BDC6_4D14_B663_98EE9040C4F4__INCLUDED_)
#define AFX_MYEVENT_H__6E0707D4_BDC6_4D14_B663_98EE9040C4F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>


#pragma warning (disable:4290)
class CMyEvent  
{
public:

    //Throw BOOL . FALSE 表示创建Event失败
    CMyEvent(BOOL bInitiallyOwn = FALSE,    //初始是否为解锁状态，FALSE 为上锁   
             BOOL bManualReset = FALSE,     //是否为自动锁，FALSE为自动锁
		     LPCTSTR lpszNAme = NULL,       //锁的名字
             LPSECURITY_ATTRIBUTES lpsaAttribute = NULL     //安全属性
             ) throw(BOOL);   

	virtual ~CMyEvent();
    
    //上锁
    BOOL Lock(DWORD dwTimeout = INFINITE);
    
    //解锁
    BOOL UnLock();
    
    //检查状态
    BOOL IsLock() CONST;

    BOOL ResetEvent();
    BOOL PulseEvent();
    BOOL SetEvent();

    operator HANDLE() CONST;    //返回Evenet句柄
	HANDLE  m_hObject;          //Event的句柄

protected:
    BOOL m_bIsAutoLock;         //是否为自动锁
    BOOL m_bIsLock;             //是否已经上锁
    
};

#endif // !defined(AFX_MYEVENT_H__6E0707D4_BDC6_4D14_B663_98EE9040C4F4__INCLUDED_)
