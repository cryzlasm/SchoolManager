// StudentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Student.h"
#include "StudentDlg.h"

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
// CStudentDlg dialog

CStudentDlg::CStudentDlg(CWnd* pParent /*=NULL*/)
: CDialog(CStudentDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CStudentDlg)
    m_strAddr = _T("");
    m_strBirth = _T("");
    m_strGender = _T("");
    m_strCallPhone = _T("");
    m_strID = _T("");
    m_strName = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


    m_bIsNameChange = FALSE;
    m_bIsIdChange = FALSE;
    m_bIsAddrChange = FALSE;
    m_bIsPhoneChange = FALSE;
    m_bIsGenderChange = FALSE;
    m_bIsBirthChange = FALSE;
}

void CStudentDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CStudentDlg)
    DDX_Control(pDX, IDC_EDIT_SNAME, m_NameCtl);
    DDX_Control(pDX, IDC_EDIT_SID, m_IdCtl);
    DDX_Control(pDX, IDC_EDIT_SGENDER, m_GenderCtl);
    DDX_Control(pDX, IDC_EDIT_SCALLPHONE, m_CallCtl);
    DDX_Control(pDX, IDC_EDIT_SBIRTH, m_BirthCtl);
    DDX_Control(pDX, IDC_EDIT_SADDR, m_AddrCtl);
    DDX_Control(pDX, IDC_CONNECT_INFO, m_ConnInfoCtl);
    DDX_Control(pDX, IDC_LIST, m_List);
    DDX_Text(pDX, IDC_EDIT_SADDR, m_strAddr);
    DDX_Text(pDX, IDC_EDIT_SBIRTH, m_strBirth);
    DDX_Text(pDX, IDC_EDIT_SGENDER, m_strGender);
    DDX_Text(pDX, IDC_EDIT_SCALLPHONE, m_strCallPhone);
    DDX_Text(pDX, IDC_EDIT_SID, m_strID);
    DDX_Text(pDX, IDC_EDIT_SNAME, m_strName);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStudentDlg, CDialog)
//{{AFX_MSG_MAP(CStudentDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_CONNECT, OnBtnConnect)
    ON_BN_CLICKED(IDC_BTN_DISCONNECT, OnBtnDisconnect)
    ON_BN_CLICKED(IDC_BTN_INPUT, OnBtnInput)
    ON_BN_CLICKED(IDC_BTN_SERCH, OnBtnSerch)
	ON_EN_CHANGE(IDC_EDIT_SID, OnChangeEditSid)
	ON_EN_CHANGE(IDC_EDIT_SNAME, OnChangeEditSname)
	ON_EN_CHANGE(IDC_EDIT_SGENDER, OnChangeEditSgender)
	ON_EN_CHANGE(IDC_EDIT_SADDR, OnChangeEditSaddr)
	ON_EN_CHANGE(IDC_EDIT_SBIRTH, OnChangeEditSbirth)
	ON_EN_CHANGE(IDC_EDIT_SCALLPHONE, OnChangeEditScallphone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStudentDlg message handlers

BOOL CStudentDlg::OnInitDialog()
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
    try
    {
        m_Command.CreateInstance("ADODB.Command");
        m_Conn.CreateInstance("ADODB.Connection");
    }
    catch(_com_error e)
    {
        OutputDebugString(e.ErrorMessage());
        OutputDebugString(TEXT("初始化ADO COM失败"));
    }
    
    InitList();
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStudentDlg::InitList()
{
    m_List.InsertColumn(0, TEXT("IUnKonw"));
    m_List.SetColumnWidth(0, 100);
}

void CStudentDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CStudentDlg::OnPaint() 
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
HCURSOR CStudentDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CStudentDlg::OnBtnConnect() 
{
    //连接数据库服务器
    if(m_Conn->Open(CONNECT_STRING, "", "", adConnectUnspecified) == S_OK)
    {
        m_ConnInfoCtl.SetWindowText(TEXT("连接成功!"));
    }
    else
    {
        OutputDebugString(TEXT("连接数据库错误"));
    }
    
}

void CStudentDlg::OnBtnDisconnect() 
{ 
    //断开
    try
    {
        if(m_Conn->Close() == S_OK)
        {
            m_ConnInfoCtl.SetWindowText(TEXT("断开成功!"));
        }
        else
        {
            OutputDebugString(TEXT("断开数据库错误"));
        }
    }
    catch(...)
    {
        
    }
}

void CStudentDlg::OnBtnInput() 
{
    //更新信息
    UpdateData(TRUE);

    //清除Edit
    EditClear();
}

void CStudentDlg::OnBtnSerch() 
{
    //更新信息
    UpdateData(TRUE);
    
    //拼接数据，并参数开始括号
    CString strColumn = TEXT("(");       //字段
    CString strWhere = TEXT("(");        //where 条件

    BOOL bIsFirstArg = TRUE;

    //判断用户是否输入查询条件
    BOOL bIsChange = FALSE;


    //拼接参数    
    if(m_bIsNameChange)
    {
        bIsChange = TRUE;
        CreateStr(SNAME, strColumn, strWhere, bIsFirstArg);
    }
    
    if(m_bIsIdChange)
    {
        bIsChange = TRUE;
        CreateStr(SID, strColumn, strWhere, bIsFirstArg);
    }
    
    if(m_bIsAddrChange)
    {
        bIsChange = TRUE;
        CreateStr(SADDR, strColumn, strWhere, bIsFirstArg);
    }
    
    if(m_bIsPhoneChange)
    {
        bIsChange = TRUE;
        CreateStr(SPHONE, strColumn, strWhere, bIsFirstArg);
    }
    
    if(m_bIsGenderChange)
    {
        bIsChange = TRUE;
        CreateStr(SGENDER, strColumn, strWhere, bIsFirstArg);
    }
    
    if(m_bIsBirthChange)
    {
        bIsChange = TRUE;
        CreateStr(SBIRTH, strColumn, strWhere, bIsFirstArg);
    }
    
    //添加参数尾括号
    strWhere += TEXT(")");
    strColumn += TEXT(")");
    
    //拼接SQL语句
    CString strExec = TEXT("");

    
    if(bIsChange)
    {
        // [Select (strColumn) from t_Student Where (strWhere)]
        strExec = SQL_SELECT + strColumn + SQL_FROM + SQL_STUDENT + SQL_WHERE + strWhere;
    }
    else
    {
        // [Select * from t_Student]
        strExec = strExec + SQL_SELECT + TEXT("*") + SQL_FROM + SQL_STUDENT;
    }
    

    //执行SQL语句
    //OnExecCMD()

    AfxMessageBox(strExec);

    //清除Edit
    EditClear();
}

void CStudentDlg::CreateStr(CString strTag, CString& strColumn, CString& strWhere,BOOL& bIsFirstArg)
{
    if(!bIsFirstArg)
    {
        strColumn += TEXT(", ");
        strWhere += TEXT(" AND ");
    }
    
    strColumn += strTag;
    
    strWhere = strWhere + strTag + TEXT(" LIKE \'%") + m_strAddr + TEXT("%\'");
    
    bIsFirstArg = FALSE;
}


//执行SQL命令
void CStudentDlg::OnExecCMD()
{
    
} 

void CStudentDlg::EditClear() 
{
    //清除Edit的输入
    m_NameCtl.SetSel(0, -1);
    m_IdCtl.SetSel(0, -1);
    m_GenderCtl.SetSel(0, -1);
    m_CallCtl.SetSel(0, -1);
    m_BirthCtl.SetSel(0, -1);
    m_AddrCtl.SetSel(0, -1);

    
    m_NameCtl.Clear();
    m_IdCtl.Clear();
    m_GenderCtl.Clear();
    m_CallCtl.Clear();
    m_BirthCtl.Clear();
    m_AddrCtl.Clear();

    //清除选中
    m_bIsNameChange = FALSE;
    m_bIsIdChange = FALSE;
    m_bIsAddrChange = FALSE;
    m_bIsPhoneChange = FALSE;
    m_bIsGenderChange = FALSE;
    m_bIsBirthChange = FALSE;
}

BOOL CStudentDlg::DestroyWindow() 
{
    //断开连接
    OnBtnDisconnect();
    
    return CDialog::DestroyWindow();
}

void CStudentDlg::OnChangeEditSid() 
{
    m_bIsIdChange = TRUE;
}

void CStudentDlg::OnChangeEditSname() 
{
    m_bIsNameChange = TRUE;
}

void CStudentDlg::OnChangeEditSgender() 
{
    m_bIsGenderChange = TRUE;
}

void CStudentDlg::OnChangeEditSaddr() 
{
    m_bIsAddrChange = TRUE;
}

void CStudentDlg::OnChangeEditSbirth() 
{
    m_bIsBirthChange = TRUE;
}

void CStudentDlg::OnChangeEditScallphone() 
{
    m_bIsPhoneChange = TRUE;
}
