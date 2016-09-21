// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientDlg)
	m_strSname = _T("");
	m_strSid = _T("");
	m_strSgender = _T("");
	m_strCouID = _T("");
	m_strCouName = _T("");
	m_strCname = _T("");
	m_strCid = _T("");
	m_strTips = _T("");
	m_nRadio = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    //服务器初始为未连接
    bIsServerConnect = FALSE;

    //初始化用于传输的数据信息
    memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));
    //用于传输的Buf
    strPutBuf = TEXT("");

    //初始化
    m_FunMenu.LoadMenu(IDR_MENU_FUN);

    //解锁
    #define OPERATED_LOCK      TRUE            //上锁
    #define OPERATED_UNLOCK    FALSE           //解锁
    bIsOperated = OPERATED_UNLOCK;

    m_pRecvThread = NULL;

    m_bRadioStu = FALSE;
    m_bRadioCourse = FALSE;
    m_bRadioClass = FALSE;

    strOne = TEXT("");
    strTwo = TEXT("");
    strThree = TEXT("");
    strFour = TEXT("");
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientDlg)
	DDX_Control(pDX, IDC_EDIT_SNAME, m_SNameCtl);
	DDX_Control(pDX, IDC_EDIT_SID, m_SIdCtl);
	DDX_Control(pDX, IDC_EDIT_SGENDER, m_SGenderCtl);
	DDX_Control(pDX, IDC_EDIT_COURSEID, m_CouIdCtl);
	DDX_Control(pDX, IDC_EDIT_CLASS_NAME, m_CNameCtl);
	DDX_Control(pDX, IDC_EDIT_COUNAME, m_CouNameCtl);
	DDX_Control(pDX, IDC_EDIT_CLASS_ID, m_CidCtl);
	DDX_Control(pDX, IDC_STATIC_TIPS, m_TipsCtl);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ListCTL);
	DDX_Text(pDX, IDC_EDIT_SNAME, m_strSname);
	DDX_Text(pDX, IDC_EDIT_SID, m_strSid);
	DDX_Text(pDX, IDC_EDIT_SGENDER, m_strSgender);
	DDX_Text(pDX, IDC_EDIT_COURSEID, m_strCouID);
	DDX_Text(pDX, IDC_EDIT_COUNAME, m_strCouName);
	DDX_Text(pDX, IDC_EDIT_CLASS_NAME, m_strCname);
	DDX_Text(pDX, IDC_EDIT_CLASS_ID, m_strCid);
	DDX_Text(pDX, IDC_STATIC_TIPS, m_strTips);
	DDX_Radio(pDX, IDC_RADIO_STU, m_nRadio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	//{{AFX_MSG_MAP(CClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INSERT, OnBtnInsert)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBtnQuery)
	ON_BN_CLICKED(IDC_BTN_ALTER, OnBtnAlter)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RESULT, OnRclickListResult)
	ON_BN_CLICKED(IDC_RADIO_CLASS, OnRadioClass)
	ON_BN_CLICKED(IDC_RADIO_COURSE, OnRadioCourse)
	ON_BN_CLICKED(IDC_RADIO_STU, OnRadioStu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

//     CString strSID = "s001";
//     CString strSName = "周";
//     CString strSGender = "男";
//     CString strSCID = "cr01";
//     m_ChangeDlg.SetChangeStu(strSID, strSName, strSGender, strSCID);
//     m_ChangeDlg.DoModal();
//     m_ChangeDlg.GetChangeStu(strSID, strSName, strSGender, strSCID);

//     CString strCouid = "cou01";
//     CString strCouName = "C++";
//     m_ChangeDlg.SetChangeCourse(strCouid, strCouName);
//     m_ChangeDlg.DoModal();
//     m_ChangeDlg.GetChangeCourse(strCouid, strCouName);

//     CString strCid = "cr01";
//     CString strCName = "科瑞";
//     m_ChangeDlg.SetChangeClass(strCid, strCName);
//     m_ChangeDlg.DoModal();
//     m_ChangeDlg.GetChangeClass(strCid, strCName);
//     exit(0);

    //连接服务端
    if(!ConnectServer())
    {
        AfxMessageBox(TEXT("服务器连接失败，请检查！"));
        exit(0);
    }
    
    
    //初始化
    InitSomething();

    OnRadioStu();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CClientDlg::SubmitDataOfAlter(DWORD dwFlag) 
{
    BOOL bRet = FALSE;

    //拼接提交信息
    CString strBuf = TEXT("");
    
    if(m_bRadioStu)
    {
        m_ClientInfo.bStu = TRUE;

        if(m_ClientInfo.nChangeOne != 0)
        {
            strBuf = strBuf + m_strSid + strOne;
        }

        if(m_ClientInfo.nChangeTwo != 0)
        {
            strBuf = strBuf + m_strSname + strTwo;
        }

        if(m_ClientInfo.nChangeThree != 0)
        {
            strBuf = strBuf + m_strSgender + strThree;
        }

        if(m_ClientInfo.nChangeFour != 0)
        {
            strBuf = strBuf + m_strCid + strFour;
        }
        
    }
    else if(m_bRadioCourse)
    {
        m_ClientInfo.bCourse = TRUE;

        if(m_ClientInfo.nChangeOne != 0)
        {
            strBuf = strBuf + m_strCouID + strOne;
        }
        
        if(m_ClientInfo.nChangeTwo != 0)
        {
            strBuf = strBuf + m_strCouName + strTwo;
        }

    }
    else if(m_bRadioClass)
    {
        m_ClientInfo.bClass = TRUE;

        if(m_ClientInfo.nChangeOne != 0)
        {
            strBuf = strBuf + m_strCid + strOne;
        }
        
        if(m_ClientInfo.nChangeTwo != 0)
        {
            strBuf = strBuf + m_strCname + strTwo;
        }
    }

    //设置发送数据
    MyTagDeal SendData;
    SendData.m_flag = (PACKET_FLAG)dwFlag;
    SendData.m_BufInfo = m_ClientInfo;

    if(!strBuf.IsEmpty())
    {
        SendData.m_pszData = strBuf.GetBuffer(0);
    }
    else
    {
        SendData.m_pszData = NULL;
    }
    
    //统计待发送数据总大小
    SendData.m_dwDataSize = m_ClientInfo.cSidLen +
                            m_ClientInfo.cSNameLen +
                            m_ClientInfo.cSgenderLen +
                            m_ClientInfo.cCouIDLen +
                            m_ClientInfo.cCouNameLen +
                            m_ClientInfo.cCidLen +
                            m_ClientInfo.cCnameLen +
                            m_ClientInfo.nChangeOne + 
                            m_ClientInfo.nChangeTwo +
                            m_ClientInfo.nChangeThree +
                            m_ClientInfo.nChangeFour + 1;
    

    if(SendData.m_dwDataSize == 1)
    {
        SendData.m_dwDataSize--;
    }
    else
    {
        SendData.m_dwDataSize *=  sizeof(TCHAR);
    }
    //发送头
    bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
                           (char*)&SendData,
                           TagDealSize);


    if(SendData.m_pszData != NULL)
    {
        //发送数据
        bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
                              SendData.m_pszData,
                              SendData.m_dwDataSize);
    }
    

    if(!bRet)
    {
        AfxMessageBox(TEXT("与服务器通信失败，请联系管理员"));
        return FALSE;
    }

    m_strTips = TEXT("从服务器获取数据中...");
    m_TipsCtl.SetWindowText(m_strTips);

    return bRet;
}

BOOL CClientDlg::SubmitData(DWORD dwFlag) 
{
    BOOL bRet = FALSE;
    
    //拼接提交信息
    CString strBuf = TEXT("");
    
    strBuf = strBuf + m_strSid
        + m_strSname
        + m_strSgender
        + m_strCouID
        + m_strCouName
        + m_strCid
        + m_strCname;
    
    
    
    MyTagDeal SendData;
    SendData.m_flag = (PACKET_FLAG)dwFlag;
    SendData.m_BufInfo = m_ClientInfo;
    
    if(!strBuf.IsEmpty())
    {
        SendData.m_pszData = strBuf.GetBuffer(0);
    }
    else
    {
        SendData.m_pszData = NULL;
    }
    
    //统计待发送数据总大小
    SendData.m_dwDataSize = m_ClientInfo.cSidLen +
                            m_ClientInfo.cSNameLen +
                            m_ClientInfo.cSgenderLen +
                            m_ClientInfo.cCouIDLen +
                            m_ClientInfo.cCouNameLen +
                            m_ClientInfo.cCidLen +
                            m_ClientInfo.cCnameLen + 1;
    

    
    if(SendData.m_dwDataSize == 1)
    {
        SendData.m_dwDataSize--;
    }
    else
    {
        SendData.m_dwDataSize *=  sizeof(TCHAR);
    }
    //发送头
    bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
        (char*)&SendData,
        TagDealSize);
    
    
    if(SendData.m_pszData != NULL)
    {
        //发送数据
        bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
            SendData.m_pszData,
            SendData.m_dwDataSize);
    }
    
    
    if(!bRet)
    {
        AfxMessageBox(TEXT("与服务器通信失败，请联系管理员"));
        return FALSE;
    }
    
    m_strTips = TEXT("从服务器获取数据中...");
    m_TipsCtl.SetWindowText(m_strTips);
    
    return bRet;
}

BOOL CClientDlg::EndRecvServer()
{
#define WAIT_FIVE_SECOND    5000
    
    //投递线程退出消息
    PostThreadMessage(m_pRecvThread->m_nThreadID,WM_QUIT,0,0);
    DWORD dwRet = WaitForSingleObject(m_pRecvThread->m_hThread, WAIT_FIVE_SECOND);
    if(dwRet == WAIT_TIMEOUT || dwRet > WAIT_ABANDONED_0)
    {
        //如果不行，则结束线程
        if(!TerminateThread(m_pRecvThread->m_hThread, -1))
        {
            //无法结束，则挂起
            if(!SuspendThread(m_pRecvThread->m_hThread))
            {
                m_Sock.m_nErrorCode = GetLastError();
                
                m_Sock.OutputErrorMsg(m_Sock.GetErrorMsg());
                return FALSE;
            }
        }
    }
    
    return TRUE;
    
}

DWORD CClientDlg::RecvServer(LPVOID lParam)
{
    CClientDlg& This = *(CClientDlg*)lParam;
    
    CMySocket& Sock = This.m_Sock;
    
    MyTagCSInfo CSInfo = {0};
    TCHAR szBuf[MAXSHORT] = {0};
    
    while(TRUE)
    {
        //初始化
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(szBuf, 0, sizeof(szBuf));
        
        //收取头部
        if(!Sock.RecvAll(Sock.GetMainSock(), (LPTSTR)&CSInfo, TagCSInfoSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: 接收Sql服务器头部失败"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql出现致命错误！"));
            AfxEndThread(0);
        }
        
        //收取后续
        if(!Sock.RecvAll(Sock.GetMainSock(), szBuf, CSInfo.m_Deal.m_dwDataSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: 接收Sql服务器后续失败"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql出现致命错误！"));
            AfxEndThread(0);
        }
        
        //关联数据
        CSInfo.m_Deal.m_pszData = szBuf;
        Sock.OutputErrorMsg(TEXT("CClientDlg::RecvServer 成功获取数据"));
        
        //AfxMessageBox(szBuf);
        //显示数据

        //用完解锁
        This.bIsOperated = OPERATED_UNLOCK;

        PACKET_FLAG& dwFlag = CSInfo.m_Deal.m_flag;

        if(dwFlag == SERVER_REPLY_QUERY)
        {
            if(!This.ShowDataInDlg(CSInfo))
            {
                Sock.OutputErrorMsg(TEXT("CClientDlg::RecvServer更新控件失败"));
                AfxMessageBox(TEXT("CMidDlg::RecvSql 更新数据失败"));

                This.bIsOperated = OPERATED_LOCK;
                AfxEndThread(0);
            }
            This.m_TipsCtl.SetWindowText(TEXT("成功"));
        }
        //如果为操作错误
        else if(dwFlag == SERVER_ERROR_ALTER || dwFlag == SERVER_ERROR_DEL|| dwFlag == SERVER_ERROR_INSERT)
        {
            CString strTmp;
            strTmp.Format(TEXT("操作出现错误：%s. 请联系管理员"), CSInfo.m_Deal.m_pszData);

            //显示错误信息
            This.m_TipsCtl.SetWindowText(strTmp);
        }
        else
        {
            This.m_TipsCtl.SetWindowText(TEXT("成功"));
        }

    }
}

BOOL CClientDlg::ShowDataInDlg(MyTagCSInfo& CSInfo)
{
    BOOL bRet = FALSE;

    //删除所有列, 初始化列表
    m_ListCTL.DeleteAllItems();
    ASSERT(m_ListCTL.GetItemCount() == 0);
    while ( m_ListCTL.DeleteColumn (0));
    
    CString strData = CSInfo.m_Deal.m_pszData;

    
    int nDataLen = strData.GetLength();


    int nEndPos = 0;
    
    m_Sock.OutputErrorMsg(TEXT("操作ListCtl"));
    m_Sock.OutputErrorMsg(strData);
    int nLen = strData.FindOneOf(TEXT("\r\n"));
    int nCount = 0;
    for(nCount = 0; TRUE; nCount++)
    {
        //切割字段
        nEndPos = strData.FindOneOf(TEXT("|"));
        
        //如果超过标题则退出
        if(nEndPos >= nLen || nEndPos == -1)
        {
            bRet = TRUE;
            break;
        }

        //插入标题
        CString strTmp = strData.Mid(0, nEndPos);
        m_ListCTL.InsertColumn(nCount, strTmp);
        m_ListCTL.SetColumnWidth(nCount, RowWidth * strTmp.GetLength());

        //丢弃已经插入的数据
        strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);

        nLen -= nEndPos + 1;
        nDataLen -= nEndPos + 1;
    }

    DWORD dwRNLen = 2 * sizeof(TCHAR);

    //去掉\r\n
    int nRNPos = strData.FindOneOf(TEXT("\r\n"));
    strData = strData.Mid(nRNPos + dwRNLen, 
                          strData.GetLength() - dwRNLen
                          );

    nEndPos = 0;
    DWORD dwCount = 0;

    
    for(dwCount = 0; TRUE; dwCount++)
    {
        nEndPos = 0;
        nLen = 0;

        nLen = strData.FindOneOf(TEXT("\r\n"));
        //切割字段
        nEndPos = strData.FindOneOf(TEXT("|"));

        //如果超过标题则退出
        if(nEndPos >= nLen || nEndPos == -1)
        {
            bRet = TRUE;
            break;
        }
        
        //插入行
        CString strTmp = strData.Mid(0, nEndPos);
        m_ListCTL.InsertItem(dwCount, strTmp);
        //丢弃已经插入的数据
        strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);
        
        nLen -= nEndPos + 1;
        nDataLen -= nEndPos + 1;

        for(int j = 1; j < nCount; j++)
        {
            //切割字段
            nEndPos = strData.FindOneOf(TEXT("|"));
            
            //如果超过标题则退出
            if(nEndPos >= nLen || nEndPos == -1)
            {
                bRet = TRUE;
                break;
            }
            //插入行
            CString strTmp = strData.Mid(0, nEndPos);

            m_ListCTL.SetItemText(dwCount, j, strTmp);

            strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);
            nLen -= nEndPos + 1;
            nDataLen -= nEndPos + 1;

        }

        //去除\R\N
        int nRNPos = strData.FindOneOf(TEXT("\r\n"));
        strData = strData.Mid(nRNPos + dwRNLen, 
                                strData.GetLength() - dwRNLen
                                          );
    }
    
    

    //按钮解锁
    bIsOperated = OPERATED_UNLOCK;
    return bRet;
}

void CClientDlg::OnBtnInsert() 
{
	//检查连接状态
    if(!GetConnectStatus())
        return;

    //检查操作同步,检查提交信息
    if(!GetEditInfo())
    {
        AfxMessageBox(TEXT("任一插入条件不能为空"));
        return;
    }

    //此次操作未完成，则锁住
    bIsOperated = OPERATED_LOCK;
    
    //发送数据
    if(!SubmitData(CLIENT_MSG_INSERT))
        return;

    //接收数据
    
    //用完解锁
    //bIsOperated = OPERATED_UNLOCK;
    
    //清空Edit数据
    InitEdit();
    UpdateData(FALSE);
}
BOOL CClientDlg::GetDelInfo()
{
    BOOL bRet = FALSE;
    
    //检查其他操作是完成
    if(bIsOperated == OPERATED_LOCK)
    {
        AfxMessageBox("有其他操作未完成，请耐心等待..");
        CMySocket::OutputErrorMsg(TEXT("操作过快: 有其他操作未返回。"));
        return FALSE;
    }

    //判断是否选择一行
    DWORD dwSlelct = m_ListCTL.GetSelectedCount();

    // 如果没有选择任何列
    if(!dwSlelct)
    {
        AfxMessageBox(TEXT("请选中一行，再进行操作"));
        return FALSE;
    }

    //初始化Edit
    InitEdit();

    // 返回一个用于迭代或获取对象指针的POSITION值；如果没有选择任何项，那么将返回NULL
    POSITION pos =m_ListCTL.GetFirstSelectedItemPosition();

    //返回列表视图控件中被选择的下一项的索引值
    int nItem = m_ListCTL.GetNextSelectedItem(pos);  
    if (nItem != -1) //-- Execute this loop as long as GetNextSelectedItem() returns -1
    {

        /*
        strBuf = strBuf + m_strSid
                        + m_strSname
                        + m_strSgender
                        + m_strCouID
                        + m_strCouName
                        + m_strCid
                        + m_strCname;
        */
        //初始化m_ClientInfo
        memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));
    
        if(m_bRadioStu)
        {
            m_ClientInfo.bStu = TRUE;
            
            //获取ListCtl 选中的信息
            m_strSid     = m_ListCTL.GetItemText(nItem, 0); 
            m_strSname   = m_ListCTL.GetItemText(nItem, 1); 
            m_strSgender = m_ListCTL.GetItemText(nItem, 2); 
            m_strCid     = m_ListCTL.GetItemText(nItem, 3); 
            
            //检查提交信息
            m_ClientInfo.cSNameLen = m_strSname.GetLength() * sizeof(TCHAR);

            m_ClientInfo.cSidLen = m_strSid.GetLength() * sizeof(TCHAR);

            m_ClientInfo.cSgenderLen = m_strSgender.GetLength() * sizeof(TCHAR);

            m_ClientInfo.cCidLen = m_strCid.GetLength() * sizeof(TCHAR);

            bRet = TRUE;
        }
        else if(m_bRadioCourse)
        {
            m_ClientInfo.bCourse = TRUE;
            
            m_strCouID   = m_ListCTL.GetItemText(nItem, 0); 
            m_strCouName = m_ListCTL.GetItemText(nItem, 1); 


            m_ClientInfo.cCouIDLen = m_strCouID.GetLength() * sizeof(TCHAR);

            m_ClientInfo.cCouNameLen = m_strCouName.GetLength() * sizeof(TCHAR);
            bRet = TRUE;

        }
        else if(m_bRadioClass)
        {
            m_ClientInfo.bClass = TRUE;

            m_strCid   = m_ListCTL.GetItemText(nItem, 0); 
            m_strCname = m_ListCTL.GetItemText(nItem, 1); 

            m_ClientInfo.cCnameLen = m_strCname.GetLength() * sizeof(TCHAR);

            m_ClientInfo.cCidLen = m_strCid.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
    }
    return bRet;
}

void CClientDlg::OnBtnDel() 
{
	//检查连接状态
    if(!GetConnectStatus())
        return;
    
    //检查操作同步,检查提交信息
    if(!GetDelInfo())
        return;
    
    //此次操作未完成，则锁住
    bIsOperated = OPERATED_LOCK;
    
    //发送数据
    if(!SubmitData(CLIENT_MSG_DEL))
        return;
    
    //接收数据
    
    //用完解锁
    //bIsOperated = OPERATED_UNLOCK;
    
    //清空Edit数据
    InitEdit();
    UpdateData(FALSE);
}

void CClientDlg::OnBtnQuery() 
{
	//检查连接状态
    if(!GetConnectStatus())
        return;
    
    //检查操作同步,检查提交信息
    GetEditInfo();
//     if(!GetEditInfo())
//         return;

    //此次操作未完成，则锁住
    bIsOperated = OPERATED_LOCK;
    
    //发送数据
    if(!SubmitData(CLIENT_MSG_QUERY))
        return;
    
    //接收数据
    
    //用完解锁
    //bIsOperated = OPERATED_UNLOCK;

    //清空Edit数据
    InitEdit();
    UpdateData(FALSE);
}

BOOL CClientDlg::AlterInfo()
{

    if(m_bRadioStu)
    {
        //设置旧数据
        m_ChangeDlg.SetChangeStu(m_strSid, m_strSname, m_strSgender, m_strCid);

        //取消对话框
        if(m_ChangeDlg.DoModal() == IDCANCEL)
        {
            return FALSE;
        }

        //获取新数据
        m_ChangeDlg.GetChangeStu(strOne, strTwo, strThree, strFour);

        m_ClientInfo.nChange = 4;
        m_ClientInfo.nChangeOne = strOne.GetLength();
        m_ClientInfo.nChangeTwo = strTwo.GetLength();
        m_ClientInfo.nChangeThree = strThree.GetLength();
        m_ClientInfo.nChangeFour = strFour.GetLength();
    }
    else if(m_bRadioCourse)
    {
        m_ChangeDlg.SetChangeCourse(m_strCouID, m_strCouName);

        //取消对话框
        if(m_ChangeDlg.DoModal() == IDCANCEL)
        {
            return FALSE;
        }
        m_ChangeDlg.GetChangeCourse(strOne, strTwo);

        m_ClientInfo.nChange = 2;
        m_ClientInfo.nChangeOne = strOne.GetLength();
        m_ClientInfo.nChangeTwo = strTwo.GetLength();
    }
    else if(m_bRadioClass)
    {
        m_ChangeDlg.SetChangeClass(m_strCid, m_strCname);

        //取消对话框
        if(m_ChangeDlg.DoModal() == IDCANCEL)
        {
            return FALSE;
        }
        m_ChangeDlg.GetChangeClass(strOne, strTwo);

        m_ClientInfo.nChange = 2;
        m_ClientInfo.nChangeOne = strOne.GetLength();
        m_ClientInfo.nChangeTwo = strTwo.GetLength();
    }

    return TRUE;
}

void CClientDlg::OnBtnAlter() 
{
	//检查连接状态
    if(!GetConnectStatus())
        return;

    //检查操作同步,检查提交信息
    if(!GetDelInfo())
        return;
    
    //此次操作未完成，则锁住
    bIsOperated = OPERATED_LOCK;
    
    //弹出修改窗口获取修改信息
    if(!AlterInfo())
    {
        bIsOperated = OPERATED_UNLOCK;
        return;    
    }

    //发送数据
    if(!SubmitDataOfAlter(CLIENT_MSG_ALTER))
        return;
    
    //接收数据
    
    //用完解锁
    //bIsOperated = OPERATED_UNLOCK;

    //清空Edit数据
    InitEdit();
    UpdateData(FALSE);
}

BOOL CClientDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
    if(!EndRecvServer())
    {
        m_Sock.OutputErrorMsg(TEXT("CClientDlg::DestroyWindow: 结束线程失败"));
    }

    //释放菜单资源
    m_FunMenu.DestroyMenu();
    m_Sock.SocketOver();

	return CDialog::DestroyWindow();
}

BOOL CClientDlg::GetConnectStatus() CONST
{
    //判断服务器是否在线
    if(!bIsServerConnect)
    {
        AfxMessageBox(TEXT("服务器未连接，请连接服务器！"));
        CMySocket::OutputErrorMsg(TEXT("按钮触发：服务器未连接"));
        return FALSE;
    }
    return TRUE;
}

BOOL CClientDlg::GetEditInfo()
{
    BOOL bRet = FALSE;

    //检查其他操作是完成
    if(bIsOperated == OPERATED_LOCK)
    {
        AfxMessageBox("有其他操作未完成，请耐心等待..");
        CMySocket::OutputErrorMsg(TEXT("操作过快: 有其他操作未返回。"));
        return FALSE;
    }

    //bIsOperated = OPERATED_UNLOCK;

    //更新Edit到本地CString
    UpdateData(TRUE);

    //初始化m_ClientInfo
    memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));

    if(m_bRadioStu)
    {
        m_ClientInfo.bStu = TRUE;
        //检查提交信息
        if(!m_strSname.IsEmpty())
        {
            m_ClientInfo.cSNameLen = m_strSname.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }

        if(!m_strSid.IsEmpty())
        {
            m_ClientInfo.cSidLen = m_strSid.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }

        if(!m_strSgender.IsEmpty())
        {
            m_ClientInfo.cSgenderLen = m_strSgender.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }

        if(!m_strCid.IsEmpty())
        {
            m_ClientInfo.cCidLen = m_strCid.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
    }
    else if(m_bRadioCourse)
    {
        m_ClientInfo.bCourse = TRUE;
        if(!m_strCouID.IsEmpty())
        {
            m_ClientInfo.cCouIDLen = m_strCouID.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
        
        if(!m_strCouName.IsEmpty())
        {
            m_ClientInfo.cCouNameLen = m_strCouName.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
    }
    else if(m_bRadioClass)
    {
        m_ClientInfo.bClass = TRUE;
        if(!m_strCname.IsEmpty())
        {
            m_ClientInfo.cCnameLen = m_strCname.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
        
        if(!m_strCid.IsEmpty())
        {
            m_ClientInfo.cCidLen = m_strCid.GetLength() * sizeof(TCHAR);
            bRet = TRUE;
        }
    }

    //检查提交数据
//     if(!bRet)
//     {
//         AfxMessageBox(TEXT("提交信息为空，请检查！"));
//         CMySocket::OutputErrorMsg(TEXT("按钮触发：提交信息为空。"));
//     }

    return bRet;
}

BOOL CClientDlg::InitEdit()
{
    m_strSname      = TEXT("");
    m_strSid        = TEXT("");
    m_strSgender    = TEXT("");
    m_strCouID      = TEXT("");
    m_strCouName    = TEXT("");
    m_strCname      = TEXT("");
    m_strCid        = TEXT("");

    return TRUE;
}


BOOL CClientDlg::ConnectServer()
{
    if(!m_Sock.StartClient(8080, TEXT("127.0.0.1")))
    {
        return FALSE; 
    }

    bIsServerConnect = TRUE;


    m_strTips = TEXT("连接服务器成功！");
    m_TipsCtl.SetWindowText(m_strTips);

    return TRUE;
}

BOOL CClientDlg::InitSomething()
{
    LONG lStyle;

	//获取当前窗口style，利用GetWindowLong
    lStyle = ::GetWindowLong(m_ListCTL.GetSafeHwnd(), GWL_STYLE);

    //清除显示方式位
    lStyle &= ~LVS_TYPEMASK; 

    //设置style，与运算
    lStyle |= LVS_REPORT;    

    //设置style  利用SewWindowLong设置View的属性
    SetWindowLong(m_ListCTL.m_hWnd, GWL_STYLE, lStyle);

    DWORD dwStyle = m_ListCTL.GetExtendedStyle();

    //选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_FULLROWSELECT;

    //网格线（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES;

    //设置扩展风格
    m_ListCTL.SetExtendedStyle(dwStyle); 


    //初始化ListCtl
    m_ListCTL.InsertColumn(0, TEXT("序号"));

    m_ListCTL.InsertColumn(1, TEXT("学号"));
    m_ListCTL.InsertColumn(2, TEXT("姓名"));
    m_ListCTL.InsertColumn(3, TEXT("性别"));

    m_ListCTL.InsertColumn(4, TEXT("班级ID"));
    m_ListCTL.InsertColumn(5, TEXT("班级名"));

    m_ListCTL.InsertColumn(6, TEXT("课程ID"));
    m_ListCTL.InsertColumn(7, TEXT("课程名"));

    
    m_ListCTL.SetColumnWidth(0, RowWidth * _tcslen(TEXT("序号")));
    m_ListCTL.SetColumnWidth(1, RowWidth * _tcslen(TEXT("学号")));
    m_ListCTL.SetColumnWidth(2, RowWidth * _tcslen(TEXT("姓名")));
    m_ListCTL.SetColumnWidth(3, RowWidth * _tcslen(TEXT("性别")));
    m_ListCTL.SetColumnWidth(4, RowWidth * _tcslen(TEXT("班级ID")));
    m_ListCTL.SetColumnWidth(5, RowWidth * _tcslen(TEXT("班级名")));
    m_ListCTL.SetColumnWidth(6, RowWidth * _tcslen(TEXT("课程ID")));
    m_ListCTL.SetColumnWidth(7, RowWidth * _tcslen(TEXT("课程名")));


    m_ListCTL.InsertItem(0, TEXT("1"));
    m_ListCTL.InsertItem(1, TEXT("2"));
    m_ListCTL.InsertItem(2, TEXT("3"));
    m_ListCTL.InsertItem(3, TEXT("4"));
    
    m_ListCTL.SetItemText(0, 1, TEXT("S001"));
    m_ListCTL.SetItemText(0, 2, TEXT("张三"));
    m_ListCTL.SetItemText(0, 3, TEXT("男"));
    m_ListCTL.SetItemText(0, 4, TEXT("C001"));


    m_pRecvThread = AfxBeginThread((AFX_THREADPROC)RecvServer, this);

    return TRUE;
}



void CClientDlg::OnRclickListResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);  
    // TODO: 在此添加控件通知处理程序代码  
    POSITION pos = m_ListCTL.GetFirstSelectedItemPosition();//在列表视图控件中获取第一个选择项的位置  
    if (pos!=NULL){  
        POINT pt = {0};  
        GetCursorPos(&pt);//得到鼠标点击位置  
        m_FunMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,
                                                pt.x,
                                                pt.y,
                                                this); //m_newListCtrl是CListCtrl对象  
    }   
    *pResult = 0;  
}

void CClientDlg::OnRadioClass() 
{
	// TODO: Add your control notification handler code here

    m_bRadioStu = FALSE;
    m_bRadioCourse = FALSE;
    m_bRadioClass = TRUE;

    m_SNameCtl.EnableWindow(FALSE);
    m_SIdCtl.EnableWindow(FALSE);
    m_SGenderCtl.EnableWindow(FALSE);
    
    m_CouIdCtl.EnableWindow(FALSE);
    m_CouNameCtl.EnableWindow(FALSE);
    
    m_CNameCtl.EnableWindow(TRUE);
	m_CidCtl.EnableWindow(TRUE);
}

void CClientDlg::OnRadioCourse() 
{
	// TODO: Add your control notification handler code here
    m_bRadioStu = FALSE;
    m_bRadioCourse = TRUE;
    m_bRadioClass = FALSE;

    m_SNameCtl.EnableWindow(FALSE);
    m_SIdCtl.EnableWindow(FALSE);
    m_SGenderCtl.EnableWindow(FALSE);
    
    m_CouIdCtl.EnableWindow(TRUE);
    m_CouNameCtl.EnableWindow(TRUE);
    
    m_CNameCtl.EnableWindow(FALSE);
	m_CidCtl.EnableWindow(FALSE);
}

void CClientDlg::OnRadioStu() 
{
	// TODO: Add your control notification handler code here
    m_bRadioStu = TRUE;
    m_bRadioCourse = FALSE;
    m_bRadioClass = FALSE;

    m_SNameCtl.EnableWindow(TRUE);
    m_SIdCtl.EnableWindow(TRUE);
    m_SGenderCtl.EnableWindow(TRUE);

    m_CouIdCtl.EnableWindow(FALSE);
    m_CouNameCtl.EnableWindow(FALSE);

    m_CNameCtl.EnableWindow(FALSE);
	m_CidCtl.EnableWindow(TRUE);
}
