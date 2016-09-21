// MidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Mid.h"
#include "MidDlg.h"

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
// CMidDlg dialog

CMidDlg::CMidDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMidDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidDlg)
	m_strServerInfo = _T("");
	m_strClientInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_pThreadConvert = NULL;

    m_bIsExitConvertThread = FALSE;
    
    m_strServerIp = TEXT("127.0.0.1");

    m_pRecvThread = NULL;
}

CMidDlg::~CMidDlg()
{
    int n = 1;
}

void CMidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidDlg)
	DDX_Control(pDX, IDC_BTN_SERVER_CONNECT, m_ServerConnectCtl);
	DDX_Control(pDX, IDC_BTN_SERVER_BREAK, m_ServerBreakCtl);
	DDX_Control(pDX, IDC_BTN_CLIENT_SERVER, m_ClientServerCtl);
	DDX_Control(pDX, IDC_BTN_CLIENT_OVER, m_ClientOverCtl);
	DDX_Control(pDX, IDC_STATIC_SERVERINFO, m_ClientCtl);
	DDX_Control(pDX, IDC_STATIC_CLIENTINFO, m_ServerCtl);
	DDX_Text(pDX, IDC_STATIC_CLIENTINFO, m_strServerInfo);
	DDX_Text(pDX, IDC_STATIC_SERVERINFO, m_strClientInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMidDlg, CDialog)
	//{{AFX_MSG_MAP(CMidDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CLIENT_OVER, OnBtnClientOver)
	ON_BN_CLICKED(IDC_BTN_CLIENT_SERVER, OnBtnClientServer)
	ON_BN_CLICKED(IDC_BTN_SERVER_BREAK, OnBtnServerBreak)
	ON_BN_CLICKED(IDC_BTN_SERVER_CONNECT, OnBtnServerConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidDlg message handlers

BOOL CMidDlg::OnInitDialog()
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

    ((CMidApp*)AfxGetApp())->m_pThis = this;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMidDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMidDlg::OnPaint() 
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
HCURSOR CMidDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMidDlg::OnBtnClientOver() 
{
	// TODO: Add your control notification handler code here
	
    //按钮变灰
    m_ClientOverCtl.EnableWindow(FALSE);
    
    //按钮激活
    m_ClientServerCtl.EnableWindow(TRUE);

    //更改提示区
    m_strServerInfo = TEXT("服务器已关闭...");
    m_ServerCtl.SetWindowText(m_strServerInfo);

    //关闭服务器
    m_Server.EndServer();
    
    //退出线程
    EndConvertThread();

    //销毁线程池
    m_Pool.Destroy();
}

void CMidDlg::OnBtnClientServer() 
{

    //开启服务端
    m_Server.StartServer(8080);

    //开启5个线程的线程池
    m_Pool.Create(5);

    //开启线程，监听Event模型传出通知
    m_pThreadConvert = AfxBeginThread((AFX_THREADPROC)ConvertClientMsgThread, (LPVOID)this);

    //按钮变灰
    m_ClientServerCtl.EnableWindow(FALSE);

    //按钮激活
    m_ClientOverCtl.EnableWindow(TRUE);

    //更改提示区
    m_strServerInfo = TEXT("服务器已开启...");
    m_ServerCtl.SetWindowText(m_strServerInfo);
    m_Sock.OutputErrorMsg(m_strServerInfo);

}

void CMidDlg::OnBtnServerBreak() 
{
    //结束Socket
    if(!m_Sock.SocketOver())
    {
        CMySocket::OutputErrorMsg(TEXT("CMidDlg::OnBtnServerBreak: SQL服务器断开出错，"));

        //更改提示区
        m_strClientInfo = TEXT("SQL服务器断开出错，请联系管理员");
        m_ClientCtl.SetWindowText(m_strClientInfo);
        return;
    }

    if(!EndRecvSql())
    {
        AfxMessageBox(TEXT("关闭SQL接收线程失败"));
        return;
    }

    //按钮变灰
    m_ServerBreakCtl.EnableWindow(FALSE);
    
    //按钮激活
    m_ServerConnectCtl.EnableWindow(TRUE);

    //更改提示区
    m_strClientInfo = TEXT("已断开SQL服务器...");
    m_ClientCtl.SetWindowText(m_strClientInfo);
    m_Sock.OutputErrorMsg(m_strClientInfo);

    
}

void CMidDlg::OnBtnServerConnect() 
{
    if(!m_Sock.StartClient(8088, m_strServerIp))
    {
        CMySocket::OutputErrorMsg(TEXT("CMidDlg::OnBtnServerConnect: SQL服务器连接出错，"));


        //更改提示区
        m_strClientInfo = TEXT("SQL服务器连接出错，请联系管理员");
        m_ClientCtl.SetWindowText(m_strClientInfo);
        return;
    }

    m_pRecvThread = AfxBeginThread((AFX_THREADPROC)RecvSql, this);

    //按钮变灰
    m_ServerConnectCtl.EnableWindow(FALSE);
    
    //按钮激活
    m_ServerBreakCtl.EnableWindow(TRUE);

    //更改提示区
    m_strClientInfo = TEXT("SQL服务器已连接...");
    m_ClientCtl.SetWindowText(m_strClientInfo);
    m_Sock.OutputErrorMsg(m_strClientInfo);
}

BOOL CMidDlg::EndRecvSql()
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

DWORD CMidDlg::RecvSql(LPVOID lParam)
{
    CMidDlg& This = *(CMidDlg*)lParam;

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
        
        //转发数据
        //发送头部
        if(!Sock.SendAll(CSInfo.m_Socket, (LPTSTR)&CSInfo, TagCSInfoSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: 接收Sql服务器头部失败"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql出现致命错误！"));
            AfxEndThread(0);
        }

        //发送后续
        if(!Sock.SendAll(CSInfo.m_Socket, szBuf, CSInfo.m_Deal.m_dwDataSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: 接收Sql服务器后续失败"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql出现致命错误！"));
            AfxEndThread(0);
        }
    }
}

BOOL CMidDlg::GetExitInfo() CONST
{
    return m_bIsExitConvertThread;
}

BOOL CMidDlg::EndConvertThread()
{
    //是否为第一次进入
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //30秒
        #define HALF_MINUTE 30000
        
        //置标志位
        m_bIsExitConvertThread = TRUE;

        //如果线程已经实例，则处理线程
        if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        {
            //等待线程退出,等30秒
            DWORD dwRet = WaitForSingleObject(m_pThreadConvert->m_hThread, HALF_MINUTE);
            if(dwRet == WAIT_TIMEOUT)
            {
                CMySocket::OutputErrorMsg(TEXT("EndServer：等待线程退出超时！"));
            
                //终止线程，
                if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                {
                    CMySocket::OutputErrorMsg(TEXT("EndServer：终止线程失败，可能权限不够"));
                
                    //如果线程无法终止,则暂停掉
                    if(m_pThreadConvert->SuspendThread() != -1)
                    {
                        CMySocket::OutputErrorMsg(TEXT("EndServer：暂停线程，防止操作野指针"));
                    }
                }//End if(!TerminateThread(m_pThreadConvert->m_hThread, 1))


                //关闭线程句柄 AfxBeginThread 会自动关闭
                //::CloseHandle(m_pThreadConvert->m_hThread);
                //m_pThreadConvert->ExitInstance();
                //m_pThreadConvert->m_hThread = INVALID_HANDLE_VALUE;

            }//End if(dwRet == WAIT_TIMEOUT)
        }//End if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)

        bIsFirstInto = FALSE;

    }//End if(bIsFirstInto)
    
    CMySocket::OutputErrorMsg(TEXT("CMidDlg::EndServer：线程退出成功"));

    return TRUE;  
}


BOOL CMidDlg::JudgeDeal(MyTagCSInfo& pData)
{
    BOOL bRet = FALSE;

    ICommand* pCMD = NULL;

    //客户端消息
    switch(pData.m_Deal.m_flag)
    {
    //增
    case CLIENT_MSG_INSERT:
        {
            //生成对应的CMD命令
            pCMD = new CInsertCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: 生成CInsertCMD 错误"));
                return FALSE;
                break;
            }
        }
        break;
    
    //删
    case CLIENT_MSG_DEL:
        {
            //生成对应的CMD命令
            pCMD = new CDelCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: 生成CInsertCMD 错误"));
                return FALSE;
                break;
            }
        }
        break;
    
    //改
    case CLIENT_MSG_ALTER:
        {
            //生成对应的CMD命令
            pCMD = new CAlterCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: 生成CInsertCMD 错误"));
                return FALSE;
                break;
            }
        }
        break;

    //查    
    case CLIENT_MSG_QUERY:
        {
            //生成对应的CMD命令
            pCMD = new CQueryCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: 生成CInsertCMD 错误"));
                return FALSE;
                break;
            }
        }
        break;

    }//End switch(pData.m_Deal.m_flag)


    CMySocket::OutputErrorMsg(TEXT("JudgeDeal: 生成命令成功"));
    //设置CMD命令
    pCMD->SetCMD(pData);
    
    //线程池执行一个命令
    

    return m_Pool.Handle(pCMD);
}

DWORD CMidDlg::ConvertClientMsgThread(LPVOID lParam)
{
    //得到窗口数据
    CMidDlg& This = *(CMidDlg*)lParam;

    //得到Server
    CEventServer& Server = This.m_Server;

    //处理数据, 拷贝一份数据
    MyTagCSInfo CSInfo = {0};

    TCHAR szBuf[MAXBYTE * 2] = {0};

    while(TRUE)
    {
        //询问线程是否有数据
        if(!Server.IsCanHandleData())
        {
            break;
        }

        //判断Event模型线程退出标志
        if(!Server.GetExitInfo())
        {
            CMySocket::OutputErrorMsg(TEXT("ConvertClientMsgThread：等待Dlg通知退出"));

            //暂停800毫秒，看线程是否需要退出
            Sleep(3000);

            //检查Dlg退出标志
            if(This.GetExitInfo())
            {
                break;
            }
        }

        //初始化缓冲区
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(&szBuf, 0, sizeof(szBuf));
        CSInfo.m_Deal.m_pszData = szBuf;

        MyTagCSInfo* pInfo = Server.GetRetData();
        
        ASSERT(pInfo != NULL);
        
        //处理数据, 拷贝一份数据
        //拷贝协议头
        memcpy(&CSInfo, pInfo, TagCSInfoSize);

        //如果有数据则申请空间
        if(CSInfo.m_Deal.m_dwDataSize > 0)
        {
            //拷贝数据
            memcpy(&szBuf, 
                   pInfo->m_Deal.m_pszData, 
                   pInfo->m_Deal.m_dwDataSize + 1);
        }
        else
        {
            CSInfo.m_Deal.m_pszData = NULL;
        }

        //转换对应的命令，给线程池执行命令
        This.JudgeDeal(CSInfo);

        //通知线程，处理结束
        Server.HandleOver(*pInfo);

        //如果线程退出, 本次则解锁
        if(!Server.GetExitInfo())
        {
            Server.m_DataLock.UnLock();
        }

    }//End while(TRUE)

    CMySocket::OutputErrorMsg(TEXT("ConvertClientMsgThread: 线程退出成功"));
    AfxEndThread(This.GetExitInfo());

    return 0;
}

BOOL CMidDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
    //关闭服务器
    m_Server.EndServer();

    //退出线程
    EndConvertThread();

    //销毁线程池
    m_Pool.Destroy();
    

	return CDialog::DestroyWindow();
}
