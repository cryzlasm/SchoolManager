// Student.h : main header file for the STUDENT application
//

#if !defined(AFX_STUDENT_H__3A26D890_D635_4214_B1E2_FC9FBB1D8378__INCLUDED_)
#define AFX_STUDENT_H__3A26D890_D635_4214_B1E2_FC9FBB1D8378__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStudentApp:
// See Student.cpp for the implementation of this class
//

class CComInit
{
public:
	CComInit()
	{
		CoInitialize(NULL);
	}
	~CComInit()
	{
		CoUninitialize();
	}
};

class CStudentApp : public CWinApp
{
public:
	CStudentApp();
	
	CComInit m_InitCom;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStudentApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStudentApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUDENT_H__3A26D890_D635_4214_B1E2_FC9FBB1D8378__INCLUDED_)
