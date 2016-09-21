// MidDlg.h : header file
//

#if !defined(AFX_MIDDLG_H__9D03B1D8_DF67_437B_94CA_C160BFC73385__INCLUDED_)
#define AFX_MIDDLG_H__9D03B1D8_DF67_437B_94CA_C160BFC73385__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMidDlg dialog

#include "../AddIn/MySocket.h"
#include "../AddIn/SocketDeal.h"
#include "../AddIn/ThreadPool.h"

#include "InsertCMD.h"
#include "QueryCMD.h"
#include "DelCMD.h"
#include "AlterCMD.h"

class CMidDlg : public CDialog
{
// Construction
public:
	CMidDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CMidDlg();

    //Ϊ�ͻ��ˣ���������
    CEventServer    m_Server;

    //�̳߳�
    CMyThreadPool<ICommand>   m_Pool;

    //�ж����ݰ������ĸ�����
    BOOL JudgeDeal(MyTagCSInfo& pData);

    //�������ӷ����
    CMySocket       m_Sock;

    //SQL����˵�IP
    CString         m_strServerIp;

    //���ڼ�������˻���
    static DWORD RecvSql(LPVOID lParam);
    CWinThread*     m_pRecvThread;

    //���������߳�
    BOOL EndRecvSql();

// Dialog Data
	//{{AFX_DATA(CMidDlg)
	enum { IDD = IDD_MID_DIALOG };
	CButton	m_ServerConnectCtl;
	CButton	m_ServerBreakCtl;
	CButton	m_ClientServerCtl;
	CButton	m_ClientOverCtl;
	CStatic	m_ClientCtl;
	CStatic	m_ServerCtl;
	CString	m_strServerInfo;
	CString	m_strClientInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMidDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnClientOver();
	afx_msg void OnBtnClientServer();
	afx_msg void OnBtnServerBreak();
	afx_msg void OnBtnServerConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    //Eventģ�Ͷ�ȡ������֮��ת������������̳߳�
    CWinThread*         m_pThreadConvert;

    //���ڴ���Eventģ�ʹ���������
    static DWORD ConvertClientMsgThread(LPVOID lParam);
    
    //�߳��˳���־
    BOOL m_bIsExitConvertThread;
    BOOL GetExitInfo() CONST;
    
    //�˳�ConvertThread
    BOOL EndConvertThread();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDDLG_H__9D03B1D8_DF67_437B_94CA_C160BFC73385__INCLUDED_)
