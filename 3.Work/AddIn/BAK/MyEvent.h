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

    //Throw BOOL . FALSE ��ʾ����Eventʧ��
    CMyEvent(BOOL bInitiallyOwn = FALSE,    //��ʼ�Ƿ�Ϊ����״̬��FALSE Ϊ����   
             BOOL bManualReset = FALSE,     //�Ƿ�Ϊ�Զ�����FALSEΪ�Զ���
		     LPCTSTR lpszNAme = NULL,       //��������
             LPSECURITY_ATTRIBUTES lpsaAttribute = NULL     //��ȫ����
             ) throw(BOOL);   

	virtual ~CMyEvent();
    
    //����
    BOOL Lock(DWORD dwTimeout = INFINITE);
    
    //����
    BOOL UnLock();
    
    //���״̬
    BOOL IsLock() CONST;

    BOOL ResetEvent();
    BOOL PulseEvent();
    BOOL SetEvent();

    operator HANDLE() CONST;    //����Evenet���
	HANDLE  m_hObject;          //Event�ľ��

protected:
    BOOL m_bIsAutoLock;         //�Ƿ�Ϊ�Զ���
    BOOL m_bIsLock;             //�Ƿ��Ѿ�����
    
};

#endif // !defined(AFX_MYEVENT_H__6E0707D4_BDC6_4D14_B663_98EE9040C4F4__INCLUDED_)
