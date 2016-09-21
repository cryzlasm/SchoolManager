// ClientDlg.h : header file
//

#if !defined(AFX_CLIENTDLG_H__9181B6DF_84B1_4049_AEF2_BE40B09CDC33__INCLUDED_)
#define AFX_CLIENTDLG_H__9181B6DF_84B1_4049_AEF2_BE40B09CDC33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog

//Socket类
#include "../AddIn/MySocket.h"

//通信协议码
#include "../AddIn/SocketDeal.h"

#include "ChangeDlg.h"

#define RowWidth    20   //标题名的基础值

class CClientDlg : public CDialog
{
// Construction
public:
	
	CClientDlg(CWnd* pParent = NULL);	// standard constructor

    //Socket
    CMySocket m_Sock;

    //用于检查服务器是否连接
    BOOL  bIsServerConnect;

    //用于发送的数据信息
    MyTagCDataInfo  m_ClientInfo;

    //用于拼接提交信息的Buf
    CString strPutBuf;

    //用于接收服务端信息
    CWinThread*     m_pRecvThread;              //线程句柄
    static DWORD RecvServer(LPVOID lParam);     //线程回调函数
    BOOL EndRecvServer();                       //结束线程

    //用于更新控件信息
    BOOL ShowDataInDlg(MyTagCSInfo& CSInfo);

    //同步
    BOOL bIsOperated;

    //连接服务器
    BOOL ConnectServer();

    //获取服务器连接状态
    BOOL GetConnectStatus() CONST;

    //获取Edit的信息
    BOOL GetEditInfo();

    //获取Del信息
    BOOL GetDelInfo();

    //用于弹出修改窗口
    BOOL AlterInfo();

    //初始化相关工作
	BOOL InitSomething();
    
    //初始化所有的Edit
    BOOL InitEdit();

    //按操作码提交Edit信息，
    BOOL SubmitData(DWORD dwFlag);

    //Alter专用
    BOOL SubmitDataOfAlter(DWORD dwFlag); 
    
    //修改窗口
    CChangeDlg m_ChangeDlg;

    //Radio
    BOOL m_bRadioStu;
    BOOL m_bRadioCourse;
    BOOL m_bRadioClass;

    //CString Buf
    CString strOne;
    CString strTwo;
    CString strThree;
    CString strFour;
// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_CLIENT_DIALOG };
	CEdit	m_SNameCtl;
	CEdit	m_SIdCtl;
	CEdit	m_SGenderCtl;
	CEdit	m_CouIdCtl;
	CEdit	m_CNameCtl;
	CEdit	m_CouNameCtl;
	CEdit	m_CidCtl;
	CStatic	m_TipsCtl;
	CListCtrl	m_ListCTL;
	CString	m_strSname;
	CString	m_strSid;
	CString	m_strSgender;
	CString	m_strCouID;
	CString	m_strCouName;
	CString	m_strCname;
	CString	m_strCid;
	CString	m_strTips;
	int		m_nRadio;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    
    CMenu m_FunMenu;
	// Generated message map functions
	//{{AFX_MSG(CClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnInsert();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnQuery();
	afx_msg void OnBtnAlter();
	afx_msg void OnRclickListResult(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioClass();
	afx_msg void OnRadioCourse();
	afx_msg void OnRadioStu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDLG_H__9181B6DF_84B1_4049_AEF2_BE40B09CDC33__INCLUDED_)
