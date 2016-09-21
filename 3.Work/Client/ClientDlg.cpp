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

    //��������ʼΪδ����
    bIsServerConnect = FALSE;

    //��ʼ�����ڴ����������Ϣ
    memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));
    //���ڴ����Buf
    strPutBuf = TEXT("");

    //��ʼ��
    m_FunMenu.LoadMenu(IDR_MENU_FUN);

    //����
    #define OPERATED_LOCK      TRUE            //����
    #define OPERATED_UNLOCK    FALSE           //����
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
//     CString strSName = "��";
//     CString strSGender = "��";
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
//     CString strCName = "����";
//     m_ChangeDlg.SetChangeClass(strCid, strCName);
//     m_ChangeDlg.DoModal();
//     m_ChangeDlg.GetChangeClass(strCid, strCName);
//     exit(0);

    //���ӷ����
    if(!ConnectServer())
    {
        AfxMessageBox(TEXT("����������ʧ�ܣ����飡"));
        exit(0);
    }
    
    
    //��ʼ��
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

    //ƴ���ύ��Ϣ
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

    //���÷�������
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
    
    //ͳ�ƴ����������ܴ�С
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
    //����ͷ
    bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
                           (char*)&SendData,
                           TagDealSize);


    if(SendData.m_pszData != NULL)
    {
        //��������
        bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
                              SendData.m_pszData,
                              SendData.m_dwDataSize);
    }
    

    if(!bRet)
    {
        AfxMessageBox(TEXT("�������ͨ��ʧ�ܣ�����ϵ����Ա"));
        return FALSE;
    }

    m_strTips = TEXT("�ӷ�������ȡ������...");
    m_TipsCtl.SetWindowText(m_strTips);

    return bRet;
}

BOOL CClientDlg::SubmitData(DWORD dwFlag) 
{
    BOOL bRet = FALSE;
    
    //ƴ���ύ��Ϣ
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
    
    //ͳ�ƴ����������ܴ�С
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
    //����ͷ
    bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
        (char*)&SendData,
        TagDealSize);
    
    
    if(SendData.m_pszData != NULL)
    {
        //��������
        bRet = m_Sock.SendAll(m_Sock.GetMainSock(),
            SendData.m_pszData,
            SendData.m_dwDataSize);
    }
    
    
    if(!bRet)
    {
        AfxMessageBox(TEXT("�������ͨ��ʧ�ܣ�����ϵ����Ա"));
        return FALSE;
    }
    
    m_strTips = TEXT("�ӷ�������ȡ������...");
    m_TipsCtl.SetWindowText(m_strTips);
    
    return bRet;
}

BOOL CClientDlg::EndRecvServer()
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

DWORD CClientDlg::RecvServer(LPVOID lParam)
{
    CClientDlg& This = *(CClientDlg*)lParam;
    
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
        Sock.OutputErrorMsg(TEXT("CClientDlg::RecvServer �ɹ���ȡ����"));
        
        //AfxMessageBox(szBuf);
        //��ʾ����

        //�������
        This.bIsOperated = OPERATED_UNLOCK;

        PACKET_FLAG& dwFlag = CSInfo.m_Deal.m_flag;

        if(dwFlag == SERVER_REPLY_QUERY)
        {
            if(!This.ShowDataInDlg(CSInfo))
            {
                Sock.OutputErrorMsg(TEXT("CClientDlg::RecvServer���¿ؼ�ʧ��"));
                AfxMessageBox(TEXT("CMidDlg::RecvSql ��������ʧ��"));

                This.bIsOperated = OPERATED_LOCK;
                AfxEndThread(0);
            }
            This.m_TipsCtl.SetWindowText(TEXT("�ɹ�"));
        }
        //���Ϊ��������
        else if(dwFlag == SERVER_ERROR_ALTER || dwFlag == SERVER_ERROR_DEL|| dwFlag == SERVER_ERROR_INSERT)
        {
            CString strTmp;
            strTmp.Format(TEXT("�������ִ���%s. ����ϵ����Ա"), CSInfo.m_Deal.m_pszData);

            //��ʾ������Ϣ
            This.m_TipsCtl.SetWindowText(strTmp);
        }
        else
        {
            This.m_TipsCtl.SetWindowText(TEXT("�ɹ�"));
        }

    }
}

BOOL CClientDlg::ShowDataInDlg(MyTagCSInfo& CSInfo)
{
    BOOL bRet = FALSE;

    //ɾ��������, ��ʼ���б�
    m_ListCTL.DeleteAllItems();
    ASSERT(m_ListCTL.GetItemCount() == 0);
    while ( m_ListCTL.DeleteColumn (0));
    
    CString strData = CSInfo.m_Deal.m_pszData;

    
    int nDataLen = strData.GetLength();


    int nEndPos = 0;
    
    m_Sock.OutputErrorMsg(TEXT("����ListCtl"));
    m_Sock.OutputErrorMsg(strData);
    int nLen = strData.FindOneOf(TEXT("\r\n"));
    int nCount = 0;
    for(nCount = 0; TRUE; nCount++)
    {
        //�и��ֶ�
        nEndPos = strData.FindOneOf(TEXT("|"));
        
        //��������������˳�
        if(nEndPos >= nLen || nEndPos == -1)
        {
            bRet = TRUE;
            break;
        }

        //�������
        CString strTmp = strData.Mid(0, nEndPos);
        m_ListCTL.InsertColumn(nCount, strTmp);
        m_ListCTL.SetColumnWidth(nCount, RowWidth * strTmp.GetLength());

        //�����Ѿ����������
        strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);

        nLen -= nEndPos + 1;
        nDataLen -= nEndPos + 1;
    }

    DWORD dwRNLen = 2 * sizeof(TCHAR);

    //ȥ��\r\n
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
        //�и��ֶ�
        nEndPos = strData.FindOneOf(TEXT("|"));

        //��������������˳�
        if(nEndPos >= nLen || nEndPos == -1)
        {
            bRet = TRUE;
            break;
        }
        
        //������
        CString strTmp = strData.Mid(0, nEndPos);
        m_ListCTL.InsertItem(dwCount, strTmp);
        //�����Ѿ����������
        strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);
        
        nLen -= nEndPos + 1;
        nDataLen -= nEndPos + 1;

        for(int j = 1; j < nCount; j++)
        {
            //�и��ֶ�
            nEndPos = strData.FindOneOf(TEXT("|"));
            
            //��������������˳�
            if(nEndPos >= nLen || nEndPos == -1)
            {
                bRet = TRUE;
                break;
            }
            //������
            CString strTmp = strData.Mid(0, nEndPos);

            m_ListCTL.SetItemText(dwCount, j, strTmp);

            strData = strData.Mid(nEndPos + 1, strData.GetLength() - nEndPos - 1);
            nLen -= nEndPos + 1;
            nDataLen -= nEndPos + 1;

        }

        //ȥ��\R\N
        int nRNPos = strData.FindOneOf(TEXT("\r\n"));
        strData = strData.Mid(nRNPos + dwRNLen, 
                                strData.GetLength() - dwRNLen
                                          );
    }
    
    

    //��ť����
    bIsOperated = OPERATED_UNLOCK;
    return bRet;
}

void CClientDlg::OnBtnInsert() 
{
	//�������״̬
    if(!GetConnectStatus())
        return;

    //������ͬ��,����ύ��Ϣ
    if(!GetEditInfo())
    {
        AfxMessageBox(TEXT("��һ������������Ϊ��"));
        return;
    }

    //�˴β���δ��ɣ�����ס
    bIsOperated = OPERATED_LOCK;
    
    //��������
    if(!SubmitData(CLIENT_MSG_INSERT))
        return;

    //��������
    
    //�������
    //bIsOperated = OPERATED_UNLOCK;
    
    //���Edit����
    InitEdit();
    UpdateData(FALSE);
}
BOOL CClientDlg::GetDelInfo()
{
    BOOL bRet = FALSE;
    
    //����������������
    if(bIsOperated == OPERATED_LOCK)
    {
        AfxMessageBox("����������δ��ɣ������ĵȴ�..");
        CMySocket::OutputErrorMsg(TEXT("��������: ����������δ���ء�"));
        return FALSE;
    }

    //�ж��Ƿ�ѡ��һ��
    DWORD dwSlelct = m_ListCTL.GetSelectedCount();

    // ���û��ѡ���κ���
    if(!dwSlelct)
    {
        AfxMessageBox(TEXT("��ѡ��һ�У��ٽ��в���"));
        return FALSE;
    }

    //��ʼ��Edit
    InitEdit();

    // ����һ�����ڵ������ȡ����ָ���POSITIONֵ�����û��ѡ���κ����ô������NULL
    POSITION pos =m_ListCTL.GetFirstSelectedItemPosition();

    //�����б���ͼ�ؼ��б�ѡ�����һ�������ֵ
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
        //��ʼ��m_ClientInfo
        memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));
    
        if(m_bRadioStu)
        {
            m_ClientInfo.bStu = TRUE;
            
            //��ȡListCtl ѡ�е���Ϣ
            m_strSid     = m_ListCTL.GetItemText(nItem, 0); 
            m_strSname   = m_ListCTL.GetItemText(nItem, 1); 
            m_strSgender = m_ListCTL.GetItemText(nItem, 2); 
            m_strCid     = m_ListCTL.GetItemText(nItem, 3); 
            
            //����ύ��Ϣ
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
	//�������״̬
    if(!GetConnectStatus())
        return;
    
    //������ͬ��,����ύ��Ϣ
    if(!GetDelInfo())
        return;
    
    //�˴β���δ��ɣ�����ס
    bIsOperated = OPERATED_LOCK;
    
    //��������
    if(!SubmitData(CLIENT_MSG_DEL))
        return;
    
    //��������
    
    //�������
    //bIsOperated = OPERATED_UNLOCK;
    
    //���Edit����
    InitEdit();
    UpdateData(FALSE);
}

void CClientDlg::OnBtnQuery() 
{
	//�������״̬
    if(!GetConnectStatus())
        return;
    
    //������ͬ��,����ύ��Ϣ
    GetEditInfo();
//     if(!GetEditInfo())
//         return;

    //�˴β���δ��ɣ�����ס
    bIsOperated = OPERATED_LOCK;
    
    //��������
    if(!SubmitData(CLIENT_MSG_QUERY))
        return;
    
    //��������
    
    //�������
    //bIsOperated = OPERATED_UNLOCK;

    //���Edit����
    InitEdit();
    UpdateData(FALSE);
}

BOOL CClientDlg::AlterInfo()
{

    if(m_bRadioStu)
    {
        //���þ�����
        m_ChangeDlg.SetChangeStu(m_strSid, m_strSname, m_strSgender, m_strCid);

        //ȡ���Ի���
        if(m_ChangeDlg.DoModal() == IDCANCEL)
        {
            return FALSE;
        }

        //��ȡ������
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

        //ȡ���Ի���
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

        //ȡ���Ի���
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
	//�������״̬
    if(!GetConnectStatus())
        return;

    //������ͬ��,����ύ��Ϣ
    if(!GetDelInfo())
        return;
    
    //�˴β���δ��ɣ�����ס
    bIsOperated = OPERATED_LOCK;
    
    //�����޸Ĵ��ڻ�ȡ�޸���Ϣ
    if(!AlterInfo())
    {
        bIsOperated = OPERATED_UNLOCK;
        return;    
    }

    //��������
    if(!SubmitDataOfAlter(CLIENT_MSG_ALTER))
        return;
    
    //��������
    
    //�������
    //bIsOperated = OPERATED_UNLOCK;

    //���Edit����
    InitEdit();
    UpdateData(FALSE);
}

BOOL CClientDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
    if(!EndRecvServer())
    {
        m_Sock.OutputErrorMsg(TEXT("CClientDlg::DestroyWindow: �����߳�ʧ��"));
    }

    //�ͷŲ˵���Դ
    m_FunMenu.DestroyMenu();
    m_Sock.SocketOver();

	return CDialog::DestroyWindow();
}

BOOL CClientDlg::GetConnectStatus() CONST
{
    //�жϷ������Ƿ�����
    if(!bIsServerConnect)
    {
        AfxMessageBox(TEXT("������δ���ӣ������ӷ�������"));
        CMySocket::OutputErrorMsg(TEXT("��ť������������δ����"));
        return FALSE;
    }
    return TRUE;
}

BOOL CClientDlg::GetEditInfo()
{
    BOOL bRet = FALSE;

    //����������������
    if(bIsOperated == OPERATED_LOCK)
    {
        AfxMessageBox("����������δ��ɣ������ĵȴ�..");
        CMySocket::OutputErrorMsg(TEXT("��������: ����������δ���ء�"));
        return FALSE;
    }

    //bIsOperated = OPERATED_UNLOCK;

    //����Edit������CString
    UpdateData(TRUE);

    //��ʼ��m_ClientInfo
    memset(&m_ClientInfo, 0, sizeof(m_ClientInfo));

    if(m_bRadioStu)
    {
        m_ClientInfo.bStu = TRUE;
        //����ύ��Ϣ
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

    //����ύ����
//     if(!bRet)
//     {
//         AfxMessageBox(TEXT("�ύ��ϢΪ�գ����飡"));
//         CMySocket::OutputErrorMsg(TEXT("��ť�������ύ��ϢΪ�ա�"));
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


    m_strTips = TEXT("���ӷ������ɹ���");
    m_TipsCtl.SetWindowText(m_strTips);

    return TRUE;
}

BOOL CClientDlg::InitSomething()
{
    LONG lStyle;

	//��ȡ��ǰ����style������GetWindowLong
    lStyle = ::GetWindowLong(m_ListCTL.GetSafeHwnd(), GWL_STYLE);

    //�����ʾ��ʽλ
    lStyle &= ~LVS_TYPEMASK; 

    //����style��������
    lStyle |= LVS_REPORT;    

    //����style  ����SewWindowLong����View������
    SetWindowLong(m_ListCTL.m_hWnd, GWL_STYLE, lStyle);

    DWORD dwStyle = m_ListCTL.GetExtendedStyle();

    //ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
    dwStyle |= LVS_EX_FULLROWSELECT;

    //�����ߣ�ֻ������report����listctrl��
    dwStyle |= LVS_EX_GRIDLINES;

    //������չ���
    m_ListCTL.SetExtendedStyle(dwStyle); 


    //��ʼ��ListCtl
    m_ListCTL.InsertColumn(0, TEXT("���"));

    m_ListCTL.InsertColumn(1, TEXT("ѧ��"));
    m_ListCTL.InsertColumn(2, TEXT("����"));
    m_ListCTL.InsertColumn(3, TEXT("�Ա�"));

    m_ListCTL.InsertColumn(4, TEXT("�༶ID"));
    m_ListCTL.InsertColumn(5, TEXT("�༶��"));

    m_ListCTL.InsertColumn(6, TEXT("�γ�ID"));
    m_ListCTL.InsertColumn(7, TEXT("�γ���"));

    
    m_ListCTL.SetColumnWidth(0, RowWidth * _tcslen(TEXT("���")));
    m_ListCTL.SetColumnWidth(1, RowWidth * _tcslen(TEXT("ѧ��")));
    m_ListCTL.SetColumnWidth(2, RowWidth * _tcslen(TEXT("����")));
    m_ListCTL.SetColumnWidth(3, RowWidth * _tcslen(TEXT("�Ա�")));
    m_ListCTL.SetColumnWidth(4, RowWidth * _tcslen(TEXT("�༶ID")));
    m_ListCTL.SetColumnWidth(5, RowWidth * _tcslen(TEXT("�༶��")));
    m_ListCTL.SetColumnWidth(6, RowWidth * _tcslen(TEXT("�γ�ID")));
    m_ListCTL.SetColumnWidth(7, RowWidth * _tcslen(TEXT("�γ���")));


    m_ListCTL.InsertItem(0, TEXT("1"));
    m_ListCTL.InsertItem(1, TEXT("2"));
    m_ListCTL.InsertItem(2, TEXT("3"));
    m_ListCTL.InsertItem(3, TEXT("4"));
    
    m_ListCTL.SetItemText(0, 1, TEXT("S001"));
    m_ListCTL.SetItemText(0, 2, TEXT("����"));
    m_ListCTL.SetItemText(0, 3, TEXT("��"));
    m_ListCTL.SetItemText(0, 4, TEXT("C001"));


    m_pRecvThread = AfxBeginThread((AFX_THREADPROC)RecvServer, this);

    return TRUE;
}



void CClientDlg::OnRclickListResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);  
    // TODO: �ڴ���ӿؼ�֪ͨ����������  
    POSITION pos = m_ListCTL.GetFirstSelectedItemPosition();//���б���ͼ�ؼ��л�ȡ��һ��ѡ�����λ��  
    if (pos!=NULL){  
        POINT pt = {0};  
        GetCursorPos(&pt);//�õ������λ��  
        m_FunMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,
                                                pt.x,
                                                pt.y,
                                                this); //m_newListCtrl��CListCtrl����  
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
