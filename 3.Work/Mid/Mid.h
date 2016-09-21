// Mid.h : main header file for the MID application
//

#if !defined(AFX_MID_H__51B27BC5_2BAF_4136_A2C4_6262FA493DBA__INCLUDED_)
#define AFX_MID_H__51B27BC5_2BAF_4136_A2C4_6262FA493DBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMidApp:
// See Mid.cpp for the implementation of this class
//

class CMidApp : public CWinApp
{
public:
	CMidApp();

    CDialog* m_pThis;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMidApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MID_H__51B27BC5_2BAF_4136_A2C4_6262FA493DBA__INCLUDED_)
