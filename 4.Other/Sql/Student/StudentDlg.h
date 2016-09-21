// StudentDlg.h : header file
//

#if !defined(AFX_STUDENTDLG_H__A5417046_5290_4E3D_9559_C5FFD302693A__INCLUDED_)
#define AFX_STUDENTDLG_H__A5417046_5290_4E3D_9559_C5FFD302693A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CStudentDlg dialog

#pragma warning( disable : 4146)
#import "C:\Program Files (x86)\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
#define CONNECT_STRING TEXT("Provider=SQLOLEDB.1;Password=cr123654;Persist Security Info=True;User ID=sa;Initial Catalog=CR23;Data Source=192.168.1.166")

#define SQL_SELECT  TEXT("SELECT ")
#define SQL_WHERE   TEXT(" WHERE ")
#define SQL_FROM    TEXT(" FROM ")

#define SQL_STUDENT TEXT(" t_Student ")

#define SID     TEXT("sId")
#define SNAME   TEXT("sName")
#define SBIRTH  TEXT("sBirth")
#define SADDR   TEXT("sAddr")
#define SPHONE  TEXT("sCallPhone")
#define SGENDER TEXT("sGender")

class CStudentDlg : public CDialog
{
// Construction
public:
	CStudentDlg(CWnd* pParent = NULL);	// standard constructor

	_CommandPtr		m_Command;
	_ConnectionPtr	m_Conn; //连接对象
	_RecordsetPtr	m_Rs;   //记录集对象
	
    //初始化List
	void InitList();

    //提交之后清除Edit
    void EditClear();

    //执行SQL命令
    void OnExecCMD();

    //用于处理SQL语句
    void CreateStr(CString strTag, CString& strColumn, CString& strWhere,BOOL& bIsFirstArg);

    //检查EDIT状态
    BOOL m_bIsNameChange;
    BOOL m_bIsIdChange;
    BOOL m_bIsAddrChange;
    BOOL m_bIsPhoneChange;
    BOOL m_bIsGenderChange;
    BOOL m_bIsBirthChange;

// Dialog Data
	//{{AFX_DATA(CStudentDlg)
	enum { IDD = IDD_STUDENT_DIALOG };
	CEdit	m_NameCtl;
	CEdit	m_IdCtl;
	CEdit	m_GenderCtl;
	CEdit	m_CallCtl;
	CEdit	m_BirthCtl;
	CEdit	m_AddrCtl;
	CStatic	m_ConnInfoCtl;
	CListCtrl	m_List;
	CString	m_strAddr;
	CString	m_strBirth;
	CString	m_strGender;
	CString	m_strCallPhone;
	CString	m_strID;
	CString	m_strName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStudentDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStudentDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnConnect();
	afx_msg void OnBtnDisconnect();
	afx_msg void OnBtnInput();
	afx_msg void OnBtnSerch();
	afx_msg void OnChangeEditSid();
	afx_msg void OnChangeEditSname();
	afx_msg void OnChangeEditSgender();
	afx_msg void OnChangeEditSaddr();
	afx_msg void OnChangeEditSbirth();
	afx_msg void OnChangeEditScallphone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUDENTDLG_H__A5417046_5290_4E3D_9559_C5FFD302693A__INCLUDED_)
