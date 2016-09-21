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