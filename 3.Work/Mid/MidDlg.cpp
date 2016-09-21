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
	
    //��ť���
    m_ClientOverCtl.EnableWindow(FALSE);
    
    //��ť����
    m_ClientServerCtl.EnableWindow(TRUE);

    //������ʾ��
    m_strServerInfo = TEXT("�������ѹر�...");
    m_ServerCtl.SetWindowText(m_strServerInfo);

    //�رշ�����
    m_Server.EndServer();
    
    //�˳��߳�
    EndConvertThread();

    //�����̳߳�
    m_Pool.Destroy();
}

void CMidDlg::OnBtnClientServer() 
{

    //���������
    m_Server.StartServer(8080);

    //����5���̵߳��̳߳�
    m_Pool.Create(5);

    //�����̣߳�����Eventģ�ʹ���֪ͨ
    m_pThreadConvert = AfxBeginThread((AFX_THREADPROC)ConvertClientMsgThread, (LPVOID)this);

    //��ť���
    m_ClientServerCtl.EnableWindow(FALSE);

    //��ť����
    m_ClientOverCtl.EnableWindow(TRUE);

    //������ʾ��
    m_strServerInfo = TEXT("�������ѿ���...");
    m_ServerCtl.SetWindowText(m_strServerInfo);
    m_Sock.OutputErrorMsg(m_strServerInfo);

}

void CMidDlg::OnBtnServerBreak() 
{
    //����Socket
    if(!m_Sock.SocketOver())
    {
        CMySocket::OutputErrorMsg(TEXT("CMidDlg::OnBtnServerBreak: SQL�������Ͽ�����"));

        //������ʾ��
        m_strClientInfo = TEXT("SQL�������Ͽ���������ϵ����Ա");
        m_ClientCtl.SetWindowText(m_strClientInfo);
        return;
    }

    if(!EndRecvSql())
    {
        AfxMessageBox(TEXT("�ر�SQL�����߳�ʧ��"));
        return;
    }

    //��ť���
    m_ServerBreakCtl.EnableWindow(FALSE);
    
    //��ť����
    m_ServerConnectCtl.EnableWindow(TRUE);

    //������ʾ��
    m_strClientInfo = TEXT("�ѶϿ�SQL������...");
    m_ClientCtl.SetWindowText(m_strClientInfo);
    m_Sock.OutputErrorMsg(m_strClientInfo);

    
}

void CMidDlg::OnBtnServerConnect() 
{
    if(!m_Sock.StartClient(8088, m_strServerIp))
    {
        CMySocket::OutputErrorMsg(TEXT("CMidDlg::OnBtnServerConnect: SQL���������ӳ���"));


        //������ʾ��
        m_strClientInfo = TEXT("SQL���������ӳ�������ϵ����Ա");
        m_ClientCtl.SetWindowText(m_strClientInfo);
        return;
    }

    m_pRecvThread = AfxBeginThread((AFX_THREADPROC)RecvSql, this);

    //��ť���
    m_ServerConnectCtl.EnableWindow(FALSE);
    
    //��ť����
    m_ServerBreakCtl.EnableWindow(TRUE);

    //������ʾ��
    m_strClientInfo = TEXT("SQL������������...");
    m_ClientCtl.SetWindowText(m_strClientInfo);
    m_Sock.OutputErrorMsg(m_strClientInfo);
}

BOOL CMidDlg::EndRecvSql()
{
#define WAIT_FIVE_SECOND    5000

    //Ͷ���߳��˳���Ϣ
    PostThreadMessage(m_pRecvThread->m_nThreadID,WM_QUIT,0,0);
    DWORD dwRet = WaitForSingleObject(m_pRecvThread->m_hThread, WAIT_FIVE_SECOND);
    if(dwRet == WAIT_TIMEOUT || dwRet > WAIT_ABANDONED_0)
    {
        //������У�������߳�
        if(!TerminateThread(m_pRecvThread->m_hThread, -1))
        {
            //�޷������������
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
        //��ʼ��
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(szBuf, 0, sizeof(szBuf));

        //��ȡͷ��
        if(!Sock.RecvAll(Sock.GetMainSock(), (LPTSTR)&CSInfo, TagCSInfoSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: ����Sql������ͷ��ʧ��"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql������������"));
            AfxEndThread(0);
        }
        
        //��ȡ����
        if(!Sock.RecvAll(Sock.GetMainSock(), szBuf, CSInfo.m_Deal.m_dwDataSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: ����Sql����������ʧ��"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql������������"));
            AfxEndThread(0);
        }

        //��������
        CSInfo.m_Deal.m_pszData = szBuf;
        
        //ת������
        //����ͷ��
        if(!Sock.SendAll(CSInfo.m_Socket, (LPTSTR)&CSInfo, TagCSInfoSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: ����Sql������ͷ��ʧ��"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql������������"));
            AfxEndThread(0);
        }

        //���ͺ���
        if(!Sock.SendAll(CSInfo.m_Socket, szBuf, CSInfo.m_Deal.m_dwDataSize))
        {
            Sock.OutputErrorMsg(TEXT("CMidDlg::RecvSql: ����Sql����������ʧ��"));
            AfxMessageBox(TEXT("CMidDlg::RecvSql������������"));
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
    //�Ƿ�Ϊ��һ�ν���
    static BOOL bIsFirstInto = TRUE;
    
    if(bIsFirstInto)
    {
        //30��
        #define HALF_MINUTE 30000
        
        //�ñ�־λ
        m_bIsExitConvertThread = TRUE;

        //����߳��Ѿ�ʵ���������߳�
        if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)
        {
            //�ȴ��߳��˳�,��30��
            DWORD dwRet = WaitForSingleObject(m_pThreadConvert->m_hThread, HALF_MINUTE);
            if(dwRet == WAIT_TIMEOUT)
            {
                CMySocket::OutputErrorMsg(TEXT("EndServer���ȴ��߳��˳���ʱ��"));
            
                //��ֹ�̣߳�
                if(!TerminateThread(m_pThreadConvert->m_hThread, 1))
                {
                    CMySocket::OutputErrorMsg(TEXT("EndServer����ֹ�߳�ʧ�ܣ�����Ȩ�޲���"));
                
                    //����߳��޷���ֹ,����ͣ��
                    if(m_pThreadConvert->SuspendThread() != -1)
                    {
                        CMySocket::OutputErrorMsg(TEXT("EndServer����ͣ�̣߳���ֹ����Ұָ��"));
                    }
                }//End if(!TerminateThread(m_pThreadConvert->m_hThread, 1))


                //�ر��߳̾�� AfxBeginThread ���Զ��ر�
                //::CloseHandle(m_pThreadConvert->m_hThread);
                //m_pThreadConvert->ExitInstance();
                //m_pThreadConvert->m_hThread = INVALID_HANDLE_VALUE;

            }//End if(dwRet == WAIT_TIMEOUT)
        }//End if(m_pThreadConvert != NULL && m_pThreadConvert->m_hThread != INVALID_HANDLE_VALUE)

        bIsFirstInto = FALSE;

    }//End if(bIsFirstInto)
    
    CMySocket::OutputErrorMsg(TEXT("CMidDlg::EndServer���߳��˳��ɹ�"));

    return TRUE;  
}


BOOL CMidDlg::JudgeDeal(MyTagCSInfo& pData)
{
    BOOL bRet = FALSE;

    ICommand* pCMD = NULL;

    //�ͻ�����Ϣ
    switch(pData.m_Deal.m_flag)
    {
    //��
    case CLIENT_MSG_INSERT:
        {
            //���ɶ�Ӧ��CMD����
            pCMD = new CInsertCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: ����CInsertCMD ����"));
                return FALSE;
                break;
            }
        }
        break;
    
    //ɾ
    case CLIENT_MSG_DEL:
        {
            //���ɶ�Ӧ��CMD����
            pCMD = new CDelCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: ����CInsertCMD ����"));
                return FALSE;
                break;
            }
        }
        break;
    
    //��
    case CLIENT_MSG_ALTER:
        {
            //���ɶ�Ӧ��CMD����
            pCMD = new CAlterCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: ����CInsertCMD ����"));
                return FALSE;
                break;
            }
        }
        break;

    //��    
    case CLIENT_MSG_QUERY:
        {
            //���ɶ�Ӧ��CMD����
            pCMD = new CQueryCMD;
            if(pCMD == NULL)
            {
                CMySocket::OutputErrorMsg(TEXT("JudgeDeal: ����CInsertCMD ����"));
                return FALSE;
                break;
            }
        }
        break;

    }//End switch(pData.m_Deal.m_flag)


    CMySocket::OutputErrorMsg(TEXT("JudgeDeal: ��������ɹ�"));
    //����CMD����
    pCMD->SetCMD(pData);
    
    //�̳߳�ִ��һ������
    

    return m_Pool.Handle(pCMD);
}

DWORD CMidDlg::ConvertClientMsgThread(LPVOID lParam)
{
    //�õ���������
    CMidDlg& This = *(CMidDlg*)lParam;

    //�õ�Server
    CEventServer& Server = This.m_Server;

    //��������, ����һ������
    MyTagCSInfo CSInfo = {0};

    TCHAR szBuf[MAXBYTE * 2] = {0};

    while(TRUE)
    {
        //ѯ���߳��Ƿ�������
        if(!Server.IsCanHandleData())
        {
            break;
        }

        //�ж�Eventģ���߳��˳���־
        if(!Server.GetExitInfo())
        {
            CMySocket::OutputErrorMsg(TEXT("ConvertClientMsgThread���ȴ�Dlg֪ͨ�˳�"));

            //��ͣ800���룬���߳��Ƿ���Ҫ�˳�
            Sleep(3000);

            //���Dlg�˳���־
            if(This.GetExitInfo())
            {
                break;
            }
        }

        //��ʼ��������
        memset(&CSInfo, 0, sizeof(CSInfo));
        memset(&szBuf, 0, sizeof(szBuf));
        CSInfo.m_Deal.m_pszData = szBuf;

        MyTagCSInfo* pInfo = Server.GetRetData();
        
        ASSERT(pInfo != NULL);
        
        //��������, ����һ������
        //����Э��ͷ
        memcpy(&CSInfo, pInfo, TagCSInfoSize);

        //���������������ռ�
        if(CSInfo.m_Deal.m_dwDataSize > 0)
        {
            //��������
            memcpy(&szBuf, 
                   pInfo->m_Deal.m_pszData, 
                   pInfo->m_Deal.m_dwDataSize + 1);
        }
        else
        {
            CSInfo.m_Deal.m_pszData = NULL;
        }

        //ת����Ӧ��������̳߳�ִ������
        This.JudgeDeal(CSInfo);

        //֪ͨ�̣߳��������
        Server.HandleOver(*pInfo);

        //����߳��˳�, ���������
        if(!Server.GetExitInfo())
        {
            Server.m_DataLock.UnLock();
        }

    }//End while(TRUE)

    CMySocket::OutputErrorMsg(TEXT("ConvertClientMsgThread: �߳��˳��ɹ�"));
    AfxEndThread(This.GetExitInfo());

    return 0;
}

BOOL CMidDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
    //�رշ�����
    m_Server.EndServer();

    //�˳��߳�
    EndConvertThread();

    //�����̳߳�
    m_Pool.Destroy();
    

	return CDialog::DestroyWindow();
}
