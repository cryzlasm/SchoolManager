#if !defined(AFX_CHANGEDLG_H__8088D8AE_8ACA_48FD_BF7C_3ED93F820B5F__INCLUDED_)
#define AFX_CHANGEDLG_H__8088D8AE_8ACA_48FD_BF7C_3ED93F820B5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeDlg dialog

class CChangeDlg : public CDialog
{
// Construction
public:
	CChangeDlg(CWnd* pParent = NULL);   // standard constructor

    //修改学生, 学生ID 学生名 学生性别， 班级ID
    BOOL SetChangeStu(CString& strSID, CString& strSName, CString& strSGender, CString& strSCid);

    //修改课程 课程ID 课程名
    BOOL SetChangeCourse(CString& strCouID, CString& strCouName);

    //修改班级 班级ID 班级名
    BOOL SetChangeClass(CString& strCID, CString& strCName);

    //修改学生, 学生ID 学生名 学生性别， 班级ID
    BOOL GetChangeStu(CString& strSID, CString& strSName, CString& strSGender, CString& strSCid);
    
    //修改课程 课程ID 课程名
    BOOL GetChangeCourse(CString& strCouID, CString& strCouName);
    
    //修改班级 班级ID 班级名
    BOOL GetChangeClass(CString& strCID, CString& strCName);

    #define ONE_STU     1
    #define TWO_COURSE  2
    #define THREE_CLS   3

    DWORD dwState;

    //临时Buf
    CString strOne;
    CString strTwo;
    CString strThree;
    CString strFour;


// Dialog Data
	//{{AFX_DATA(CChangeDlg)
	enum { IDD = IDD_DLG_CHANGE };
	CEdit	m_strTwoCtl;
	CEdit	m_strOneCtl;
	CEdit	m_strFourCtl;
	CEdit	m_strThreeCtl;
	CStatic	m_Two2;
	CStatic	m_Two;
	CStatic	m_Three2;
	CStatic	m_Three;
	CStatic	m_One2;
	CStatic	m_One;
	CStatic	m_Four2;
	CStatic	m_Four;
	CEdit	m_TwoCtl;
	CEdit	m_ThreeCtl;
	CEdit	m_OneCtl;
	CEdit	m_FourCtl;
	CString	m_strFour;
	CString	m_strOne;
	CString	m_strThree;
	CString	m_strTwo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeDlg)
	afx_msg void OnBtnChange();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL InitSomeThings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEDLG_H__8088D8AE_8ACA_48FD_BF7C_3ED93F820B5F__INCLUDED_)
